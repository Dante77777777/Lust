#include <QtTest>

#include "Protocol/FrameProtocol.h"

class FrameProtocolTest : public QObject
{
    Q_OBJECT

private slots:
    void roundTripPreservesHeaderAndPayload();
    void fragmentedPacketWaitsForRemainingBytes();
    void stickyPacketsDecodeOneAtATime();
    void inferenceMessagesRoundTrip();
    void invalidMagicIsRejected();
    void unsupportedVersionIsRejected();
    void unknownMessageTypeIsRejected();
    void oversizedPayloadIsRejected();
};

namespace
{

FrameProtocol::FrameHeader makeHeader(quint64 sequence)
{
    FrameProtocol::FrameHeader header;
    header.messageType =
        FrameProtocol::MessageType::ImageFrame;
    header.pixelFormat =
        FrameProtocol::PixelFormat::Jpeg;
    header.sequence = sequence;
    header.timestampMs = 1785211200123ULL;
    header.width = 1920;
    header.height = 1080;
    return header;
}

void setBigEndian16(
    QByteArray& packet,
    qsizetype offset,
    quint16 value
)
{
    packet[offset] =
        static_cast<char>((value >> 8) & 0xFFU);
    packet[offset + 1] =
        static_cast<char>(value & 0xFFU);
}

void setBigEndian32(
    QByteArray& packet,
    qsizetype offset,
    quint32 value
)
{
    packet[offset] =
        static_cast<char>((value >> 24) & 0xFFU);
    packet[offset + 1] =
        static_cast<char>((value >> 16) & 0xFFU);
    packet[offset + 2] =
        static_cast<char>((value >> 8) & 0xFFU);
    packet[offset + 3] =
        static_cast<char>(value & 0xFFU);
}

}

void FrameProtocolTest::roundTripPreservesHeaderAndPayload()
{
    const auto expectedHeader = makeHeader(42);
    const QByteArray expectedPayload =
        QByteArray::fromHex("FFD8FFE000104A46494600FFD9");

    QByteArray receiveBuffer =
        FrameProtocol::encodePacket(
            expectedHeader,
            expectedPayload
        );

    QCOMPARE(
        receiveBuffer.size(),
        static_cast<qsizetype>(
            FrameProtocol::HEADER_SIZE
        ) + expectedPayload.size()
    );

    FrameProtocol::FrameHeader actualHeader;
    QByteArray actualPayload;
    QString errorMessage;

    const auto result =
        FrameProtocol::tryDecodePacket(
            receiveBuffer,
            actualHeader,
            actualPayload,
            &errorMessage
        );

    QCOMPARE(
        static_cast<int>(result),
        static_cast<int>(
            FrameProtocol::DecodeResult::Complete
        )
    );
    QVERIFY(errorMessage.isEmpty());
    QVERIFY(receiveBuffer.isEmpty());
    QCOMPARE(
        static_cast<int>(actualHeader.messageType),
        static_cast<int>(expectedHeader.messageType)
    );
    QCOMPARE(
        static_cast<int>(actualHeader.pixelFormat),
        static_cast<int>(expectedHeader.pixelFormat)
    );
    QCOMPARE(actualHeader.sequence, expectedHeader.sequence);
    QCOMPARE(
        actualHeader.timestampMs,
        expectedHeader.timestampMs
    );
    QCOMPARE(actualHeader.width, expectedHeader.width);
    QCOMPARE(actualHeader.height, expectedHeader.height);
    QCOMPARE(
        actualHeader.payloadSize,
        static_cast<quint32>(expectedPayload.size())
    );
    QCOMPARE(actualPayload, expectedPayload);
}

void FrameProtocolTest::fragmentedPacketWaitsForRemainingBytes()
{
    const QByteArray payload("fragmented-image");
    const QByteArray packet =
        FrameProtocol::encodePacket(
            makeHeader(1),
            payload
        );

    QByteArray receiveBuffer =
        packet.left(FrameProtocol::HEADER_SIZE + 3);
    const QByteArray originalFragment = receiveBuffer;

    FrameProtocol::FrameHeader header;
    QByteArray decodedPayload;

    auto result =
        FrameProtocol::tryDecodePacket(
            receiveBuffer,
            header,
            decodedPayload
        );

    QCOMPARE(
        static_cast<int>(result),
        static_cast<int>(
            FrameProtocol::DecodeResult::NeedMoreData
        )
    );
    QCOMPARE(receiveBuffer, originalFragment);
    QVERIFY(decodedPayload.isEmpty());

    receiveBuffer.append(
        packet.mid(FrameProtocol::HEADER_SIZE + 3)
    );

    result =
        FrameProtocol::tryDecodePacket(
            receiveBuffer,
            header,
            decodedPayload
        );

    QCOMPARE(
        static_cast<int>(result),
        static_cast<int>(
            FrameProtocol::DecodeResult::Complete
        )
    );
    QCOMPARE(decodedPayload, payload);
    QVERIFY(receiveBuffer.isEmpty());
}

void FrameProtocolTest::stickyPacketsDecodeOneAtATime()
{
    const QByteArray firstPayload("first");
    const QByteArray secondPayload("second");

    const QByteArray firstPacket =
        FrameProtocol::encodePacket(
            makeHeader(100),
            firstPayload
        );
    const QByteArray secondPacket =
        FrameProtocol::encodePacket(
            makeHeader(101),
            secondPayload
        );

    QByteArray receiveBuffer =
        firstPacket + secondPacket;

    FrameProtocol::FrameHeader header;
    QByteArray payload;

    auto result =
        FrameProtocol::tryDecodePacket(
            receiveBuffer,
            header,
            payload
        );

    QCOMPARE(
        static_cast<int>(result),
        static_cast<int>(
            FrameProtocol::DecodeResult::Complete
        )
    );
    QCOMPARE(header.sequence, 100ULL);
    QCOMPARE(payload, firstPayload);
    QCOMPARE(receiveBuffer, secondPacket);

    result =
        FrameProtocol::tryDecodePacket(
            receiveBuffer,
            header,
            payload
        );

    QCOMPARE(
        static_cast<int>(result),
        static_cast<int>(
            FrameProtocol::DecodeResult::Complete
        )
    );
    QCOMPARE(header.sequence, 101ULL);
    QCOMPARE(payload, secondPayload);
    QVERIFY(receiveBuffer.isEmpty());
}

void FrameProtocolTest::inferenceMessagesRoundTrip()
{
    const QList<FrameProtocol::MessageType> messageTypes{
        FrameProtocol::MessageType::InferenceResult,
        FrameProtocol::MessageType::InferenceError
    };

    for (const FrameProtocol::MessageType messageType
         : messageTypes)
    {
        auto expectedHeader = makeHeader(7001);
        expectedHeader.messageType = messageType;
        expectedHeader.pixelFormat =
            FrameProtocol::PixelFormat::Unknown;
        expectedHeader.width = 0;
        expectedHeader.height = 0;
        const QByteArray expectedPayload(
            "{\"status\":\"OK\"}");

        QByteArray receiveBuffer =
            FrameProtocol::encodePacket(
                expectedHeader,
                expectedPayload);
        QVERIFY(!receiveBuffer.isEmpty());

        FrameProtocol::FrameHeader actualHeader;
        QByteArray actualPayload;
        QString errorMessage;

        const auto result =
            FrameProtocol::tryDecodePacket(
                receiveBuffer,
                actualHeader,
                actualPayload,
                &errorMessage);

        QCOMPARE(
            static_cast<int>(result),
            static_cast<int>(
                FrameProtocol::DecodeResult::Complete));
        QVERIFY(errorMessage.isEmpty());
        QVERIFY(receiveBuffer.isEmpty());
        QCOMPARE(
            static_cast<int>(actualHeader.messageType),
            static_cast<int>(messageType));
        QCOMPARE(actualHeader.sequence, 7001ULL);
        QCOMPARE(
            static_cast<int>(actualHeader.pixelFormat),
            static_cast<int>(
                FrameProtocol::PixelFormat::Unknown));
        QCOMPARE(actualHeader.width, 0U);
        QCOMPARE(actualHeader.height, 0U);
        QCOMPARE(actualPayload, expectedPayload);
    }
}

void FrameProtocolTest::invalidMagicIsRejected()
{
    QByteArray receiveBuffer =
        FrameProtocol::encodePacket(
            makeHeader(1),
            QByteArray("payload")
        );
    receiveBuffer[0] = '\0';
    const QByteArray originalPacket = receiveBuffer;

    FrameProtocol::FrameHeader header;
    QByteArray payload;
    QString errorMessage;

    const auto result =
        FrameProtocol::tryDecodePacket(
            receiveBuffer,
            header,
            payload,
            &errorMessage
        );

    QCOMPARE(
        static_cast<int>(result),
        static_cast<int>(
            FrameProtocol::DecodeResult::InvalidData
        )
    );
    QCOMPARE(
        errorMessage,
        QStringLiteral("Invalid protocol magic")
    );
    QCOMPARE(receiveBuffer, originalPacket);
}

void FrameProtocolTest::unsupportedVersionIsRejected()
{
    QByteArray receiveBuffer =
        FrameProtocol::encodePacket(
            makeHeader(1),
            QByteArray("payload")
        );
    setBigEndian16(receiveBuffer, 4, 2);

    FrameProtocol::FrameHeader header;
    QByteArray payload;
    QString errorMessage;

    const auto result =
        FrameProtocol::tryDecodePacket(
            receiveBuffer,
            header,
            payload,
            &errorMessage
        );

    QCOMPARE(
        static_cast<int>(result),
        static_cast<int>(
            FrameProtocol::DecodeResult::InvalidData
        )
    );
    QCOMPARE(
        errorMessage,
        QStringLiteral("Unsupported protocol version")
    );
}

void FrameProtocolTest::unknownMessageTypeIsRejected()
{
    QByteArray receiveBuffer =
        FrameProtocol::encodePacket(
            makeHeader(1),
            QByteArray("payload")
        );
    setBigEndian16(receiveBuffer, 8, 999);

    FrameProtocol::FrameHeader header;
    QByteArray payload;
    QString errorMessage;

    const auto result =
        FrameProtocol::tryDecodePacket(
            receiveBuffer,
            header,
            payload,
            &errorMessage
        );

    QCOMPARE(
        static_cast<int>(result),
        static_cast<int>(
            FrameProtocol::DecodeResult::InvalidData
        )
    );
    QCOMPARE(
        errorMessage,
        QStringLiteral("Unknown message type")
    );
}

void FrameProtocolTest::oversizedPayloadIsRejected()
{
    QByteArray receiveBuffer =
        FrameProtocol::encodePacket(
            makeHeader(1),
            QByteArray("payload")
        );
    setBigEndian32(
        receiveBuffer,
        36,
        FrameProtocol::MAX_PAYLOAD_SIZE + 1
    );

    FrameProtocol::FrameHeader header;
    QByteArray payload;
    QString errorMessage;

    const auto result =
        FrameProtocol::tryDecodePacket(
            receiveBuffer,
            header,
            payload,
            &errorMessage
        );

    QCOMPARE(
        static_cast<int>(result),
        static_cast<int>(
            FrameProtocol::DecodeResult::InvalidData
        )
    );
    QCOMPARE(
        errorMessage,
        QStringLiteral("Payload is too large")
    );
}

QTEST_APPLESS_MAIN(FrameProtocolTest)

#include "FrameProtocolTest.moc"
