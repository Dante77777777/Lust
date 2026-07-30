#include <QSignalSpy>
#include <QtTest>

#include "Protocol/InferenceResponseCodec.h"
#include "Protocol/InferenceResponseDispatcher.h"

class InferenceResponseDispatcherTest final
    : public QObject
{
    Q_OBJECT

private slots:
    void decodesInferenceResult();
    void decodesInferenceError();
    void rejectsInvalidResultPayload();
    void rejectsInvalidErrorPayload();
    void acceptsStopAcquisitionRequest();
    void rejectsInvalidStopAcquisitionRequest();
    void rejectsUnexpectedMessageType();
};

void InferenceResponseDispatcherTest::
decodesInferenceResult()
{
    InferenceResponseDispatcher dispatcher;
    QSignalSpy resultSpy(
        &dispatcher,
        &InferenceResponseDispatcher::
            sigInferenceResultReceived
    );
    QSignalSpy rejectedSpy(
        &dispatcher,
        &InferenceResponseDispatcher::
            sigResponseRejected
    );

    InferenceResponseCodec::
        InferenceResultPayload expected;
    expected.detectionLevel = QStringLiteral("NG");
    expected.score = 0.91;
    expected.inferenceMs = 18;
    expected.defectArea = 260;

    QString encodeError;
    const QByteArray payload =
        InferenceResponseCodec::encodeResult(
            expected,
            &encodeError
        );
    QVERIFY2(
        !payload.isEmpty(),
        qPrintable(encodeError)
    );

    ReceivedResponsePacket packet;
    packet.header.messageType =
        FrameProtocol::MessageType::InferenceResult;
    packet.header.sequence = 123;
    packet.payload = payload;

    dispatcher.dispatch(packet);

    QCOMPARE(resultSpy.count(), 1);
    QCOMPARE(rejectedSpy.count(), 0);
    QCOMPARE(
        resultSpy.at(0).at(0).toULongLong(),
        123
    );

    const auto actual = qvariant_cast<
        InferenceResponseCodec::
            InferenceResultPayload
    >(resultSpy.at(0).at(1));

    QCOMPARE(
        actual.detectionLevel,
        expected.detectionLevel
    );
    QCOMPARE(actual.score, expected.score);
    QCOMPARE(
        actual.inferenceMs,
        expected.inferenceMs
    );
    QCOMPARE(
        actual.defectArea,
        expected.defectArea
    );
}

void InferenceResponseDispatcherTest::
decodesInferenceError()
{
    InferenceResponseDispatcher dispatcher;
    QSignalSpy errorSpy(
        &dispatcher,
        &InferenceResponseDispatcher::
            sigInferenceErrorReceived
    );
    QSignalSpy rejectedSpy(
        &dispatcher,
        &InferenceResponseDispatcher::
            sigResponseRejected
    );

    InferenceResponseCodec::
        InferenceErrorPayload expected;
    expected.errorCode =
        QStringLiteral("FRAME_DROPPED");
    expected.message =
        QStringLiteral("Frame was replaced");

    QString encodeError;
    const QByteArray payload =
        InferenceResponseCodec::encodeError(
            expected,
            &encodeError
        );
    QVERIFY2(
        !payload.isEmpty(),
        qPrintable(encodeError)
    );

    ReceivedResponsePacket packet;
    packet.header.messageType =
        FrameProtocol::MessageType::InferenceError;
    packet.header.sequence = 456;
    packet.payload = payload;

    dispatcher.dispatch(packet);

    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(rejectedSpy.count(), 0);
    QCOMPARE(
        errorSpy.at(0).at(0).toULongLong(),
        456
    );

    const auto actual = qvariant_cast<
        InferenceResponseCodec::
            InferenceErrorPayload
    >(errorSpy.at(0).at(1));

    QCOMPARE(
        actual.errorCode,
        expected.errorCode
    );
    QCOMPARE(
        actual.message,
        expected.message
    );
}

void InferenceResponseDispatcherTest::
rejectsInvalidResultPayload()
{
    InferenceResponseDispatcher dispatcher;
    QSignalSpy resultSpy(
        &dispatcher,
        &InferenceResponseDispatcher::
            sigInferenceResultReceived
    );
    QSignalSpy rejectedSpy(
        &dispatcher,
        &InferenceResponseDispatcher::
            sigResponseRejected
    );

    ReceivedResponsePacket packet;
    packet.header.messageType =
        FrameProtocol::MessageType::InferenceResult;
    packet.header.sequence = 700;
    packet.payload =
        QByteArrayLiteral(R"({"score":"bad"})");

    dispatcher.dispatch(packet);

    QCOMPARE(resultSpy.count(), 0);
    QCOMPARE(rejectedSpy.count(), 1);
    QCOMPARE(
        rejectedSpy.at(0).at(0).toULongLong(),
        700
    );
    QVERIFY(
        rejectedSpy.at(0).at(1)
            .toString()
            .contains(
                QStringLiteral(
                    "Invalid inference result"
                )
            )
    );
}

void InferenceResponseDispatcherTest::
rejectsInvalidErrorPayload()
{
    InferenceResponseDispatcher dispatcher;
    QSignalSpy errorSpy(
        &dispatcher,
        &InferenceResponseDispatcher::
            sigInferenceErrorReceived
    );
    QSignalSpy rejectedSpy(
        &dispatcher,
        &InferenceResponseDispatcher::
            sigResponseRejected
    );

    ReceivedResponsePacket packet;
    packet.header.messageType =
        FrameProtocol::MessageType::InferenceError;
    packet.header.sequence = 701;
    packet.payload = QByteArrayLiteral(
        R"({"schema_version":1})"
    );

    dispatcher.dispatch(packet);

    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(rejectedSpy.count(), 1);
    QCOMPARE(
        rejectedSpy.at(0).at(0).toULongLong(),
        701
    );
    QVERIFY(
        rejectedSpy.at(0).at(1)
            .toString()
            .contains(
                QStringLiteral(
                    "Invalid inference error"
                )
            )
    );
}

void InferenceResponseDispatcherTest::
acceptsStopAcquisitionRequest()
{
    InferenceResponseDispatcher dispatcher;
    QSignalSpy stopSpy(
        &dispatcher,
        &InferenceResponseDispatcher::
            sigStopAcquisitionRequested);
    QSignalSpy rejectedSpy(
        &dispatcher,
        &InferenceResponseDispatcher::
            sigResponseRejected);

    ReceivedResponsePacket packet;
    packet.header.messageType =
        FrameProtocol::MessageType::
            StopAcquisitionRequest;
    packet.header.pixelFormat =
        FrameProtocol::PixelFormat::Unknown;
    packet.header.sequence = 750;

    dispatcher.dispatch(packet);

    QCOMPARE(stopSpy.count(), 1);
    QCOMPARE(
        stopSpy.first().at(0).toULongLong(),
        750ULL);
    QCOMPARE(rejectedSpy.count(), 0);
}

void InferenceResponseDispatcherTest::
rejectsInvalidStopAcquisitionRequest()
{
    InferenceResponseDispatcher dispatcher;
    QSignalSpy stopSpy(
        &dispatcher,
        &InferenceResponseDispatcher::
            sigStopAcquisitionRequested);
    QSignalSpy rejectedSpy(
        &dispatcher,
        &InferenceResponseDispatcher::
            sigResponseRejected);

    ReceivedResponsePacket packet;
    packet.header.messageType =
        FrameProtocol::MessageType::
            StopAcquisitionRequest;
    packet.header.pixelFormat =
        FrameProtocol::PixelFormat::Jpeg;
    packet.header.sequence = 751;
    packet.header.width = 10;
    packet.payload = QByteArrayLiteral("invalid");

    dispatcher.dispatch(packet);

    QCOMPARE(stopSpy.count(), 0);
    QCOMPARE(rejectedSpy.count(), 1);
    QCOMPARE(
        rejectedSpy.first().at(0).toULongLong(),
        751ULL);
}

void InferenceResponseDispatcherTest::
rejectsUnexpectedMessageType()
{
    InferenceResponseDispatcher dispatcher;
    QSignalSpy resultSpy(
        &dispatcher,
        &InferenceResponseDispatcher::
            sigInferenceResultReceived
    );
    QSignalSpy errorSpy(
        &dispatcher,
        &InferenceResponseDispatcher::
            sigInferenceErrorReceived
    );
    QSignalSpy rejectedSpy(
        &dispatcher,
        &InferenceResponseDispatcher::
            sigResponseRejected
    );

    ReceivedResponsePacket packet;
    packet.header.messageType =
        FrameProtocol::MessageType::ImageFrame;
    packet.header.sequence = 800;
    packet.payload = QByteArrayLiteral("image");

    dispatcher.dispatch(packet);

    QCOMPARE(resultSpy.count(), 0);
    QCOMPARE(errorSpy.count(), 0);
    QCOMPARE(rejectedSpy.count(), 1);
    QCOMPARE(
        rejectedSpy.at(0).at(0).toULongLong(),
        800
    );
    QCOMPARE(
        rejectedSpy.at(0).at(1).toString(),
        QStringLiteral(
            "Unexpected response message type"
        )
    );
}

QTEST_GUILESS_MAIN(InferenceResponseDispatcherTest)

#include "InferenceResponseDispatcherTest.moc"
