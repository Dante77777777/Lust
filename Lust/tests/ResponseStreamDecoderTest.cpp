#include <QSignalSpy>
#include <QtTest>

#include "Protocol/FrameProtocol.h"
#include "Transport/ResponseStreamDecoder.h"

class ResponseStreamDecoderTest final : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void decodesFragmentedPacket();
    void decodesStickyPacketsInOrder();
    void clearsInvalidDataAndRecovers();
    void resetClearsIncompletePacket();
};

namespace
{

QByteArray makePacket(
    FrameProtocol::MessageType messageType,
    quint64 sequence,
    const QByteArray& payload
)
{
    FrameProtocol::FrameHeader header;
    header.messageType = messageType;
    header.pixelFormat =
        FrameProtocol::PixelFormat::Unknown;
    header.sequence = sequence;
    header.timestampMs = 123456;

    return FrameProtocol::encodePacket(
        header,
        payload
    );
}

ReceivedResponsePacket packetFromSpy(
    const QSignalSpy& spy,
    qsizetype index
)
{
    return qvariant_cast<ReceivedResponsePacket>(
        spy.at(index).at(0)
    );
}

}

void ResponseStreamDecoderTest::initTestCase()
{
    qRegisterMetaType<ReceivedResponsePacket>(
        "ReceivedResponsePacket"
    );
}

void ResponseStreamDecoderTest::decodesFragmentedPacket()
{
    ResponseStreamDecoder decoder;
    QSignalSpy decodedSpy(
        &decoder,
        &ResponseStreamDecoder::sigResponseDecoded
    );
    QSignalSpy errorSpy(
        &decoder,
        &ResponseStreamDecoder::sigProtocolError
    );

    QVERIFY(decodedSpy.isValid());
    QVERIFY(errorSpy.isValid());

    const QByteArray payload(
        R"({"schemaVersion":1,"score":0.95})"
    );
    const QByteArray packet = makePacket(
        FrameProtocol::MessageType::InferenceResult,
        42,
        payload
    );
    QVERIFY(!packet.isEmpty());

    decoder.appendBytes(packet.left(13));

    QCOMPARE(decodedSpy.count(), 0);
    QCOMPARE(decoder.bufferedBytes(), 13);

    decoder.appendBytes(packet.mid(13, 20));

    QCOMPARE(decodedSpy.count(), 0);
    QCOMPARE(decoder.bufferedBytes(), 33);

    decoder.appendBytes(packet.mid(33));

    QCOMPARE(decodedSpy.count(), 1);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(decoder.bufferedBytes(), 0);

    const ReceivedResponsePacket decoded =
        packetFromSpy(decodedSpy, 0);

    QCOMPARE(decoded.header.sequence, 42);
    QCOMPARE(
        static_cast<quint16>(
            decoded.header.messageType
        ),
        static_cast<quint16>(
            FrameProtocol::MessageType::InferenceResult
        )
    );
    QCOMPARE(decoded.payload, payload);
}

void ResponseStreamDecoderTest::decodesStickyPacketsInOrder()
{
    ResponseStreamDecoder decoder;
    QSignalSpy decodedSpy(
        &decoder,
        &ResponseStreamDecoder::sigResponseDecoded
    );

    const QByteArray firstPayload(
        R"({"result":"OK"})"
    );
    const QByteArray secondPayload(
        R"({"error":"inference_failed"})"
    );

    const QByteArray firstPacket = makePacket(
        FrameProtocol::MessageType::InferenceResult,
        100,
        firstPayload
    );
    const QByteArray secondPacket = makePacket(
        FrameProtocol::MessageType::InferenceError,
        101,
        secondPayload
    );

    decoder.appendBytes(firstPacket + secondPacket);

    QCOMPARE(decodedSpy.count(), 2);
    QCOMPARE(decoder.bufferedBytes(), 0);

    const ReceivedResponsePacket firstDecoded =
        packetFromSpy(decodedSpy, 0);
    const ReceivedResponsePacket secondDecoded =
        packetFromSpy(decodedSpy, 1);

    QCOMPARE(firstDecoded.header.sequence, 100);
    QCOMPARE(firstDecoded.payload, firstPayload);
    QCOMPARE(secondDecoded.header.sequence, 101);
    QCOMPARE(secondDecoded.payload, secondPayload);
}

void ResponseStreamDecoderTest::clearsInvalidDataAndRecovers()
{
    ResponseStreamDecoder decoder;
    QSignalSpy decodedSpy(
        &decoder,
        &ResponseStreamDecoder::sigResponseDecoded
    );
    QSignalSpy errorSpy(
        &decoder,
        &ResponseStreamDecoder::sigProtocolError
    );

    QByteArray invalidPacket = makePacket(
        FrameProtocol::MessageType::InferenceResult,
        1,
        QByteArrayLiteral("{}")
    );
    QVERIFY(!invalidPacket.isEmpty());

    invalidPacket[0] = '\0';
    decoder.appendBytes(invalidPacket);

    QCOMPARE(decodedSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(decoder.bufferedBytes(), 0);
    QVERIFY(
        !errorSpy.at(0).at(0).toString().isEmpty()
    );

    const QByteArray validPayload(
        R"({"result":"recovered"})"
    );
    decoder.appendBytes(
        makePacket(
            FrameProtocol::MessageType::InferenceResult,
            2,
            validPayload
        )
    );

    QCOMPARE(decodedSpy.count(), 1);

    const ReceivedResponsePacket decoded =
        packetFromSpy(decodedSpy, 0);
    QCOMPARE(decoded.header.sequence, 2);
    QCOMPARE(decoded.payload, validPayload);
}

void ResponseStreamDecoderTest::resetClearsIncompletePacket()
{
    ResponseStreamDecoder decoder;
    QSignalSpy decodedSpy(
        &decoder,
        &ResponseStreamDecoder::sigResponseDecoded
    );

    const QByteArray packet = makePacket(
        FrameProtocol::MessageType::InferenceError,
        77,
        QByteArrayLiteral(
            R"({"message":"temporary error"})"
        )
    );

    decoder.appendBytes(packet.left(20));

    QCOMPARE(decoder.bufferedBytes(), 20);
    QCOMPARE(decodedSpy.count(), 0);

    decoder.reset();

    QCOMPARE(decoder.bufferedBytes(), 0);

    decoder.appendBytes(packet);

    QCOMPARE(decodedSpy.count(), 1);
    QCOMPARE(decoder.bufferedBytes(), 0);

    const ReceivedResponsePacket decoded =
        packetFromSpy(decodedSpy, 0);
    QCOMPARE(decoded.header.sequence, 77);
}

QTEST_GUILESS_MAIN(ResponseStreamDecoderTest)

#include "ResponseStreamDecoderTest.moc"
