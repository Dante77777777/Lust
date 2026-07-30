#include <QHostAddress>
#include <QSignalSpy>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtTest>

#include "Protocol/FrameProtocol.h"
#include "Protocol/InferenceResponseCodec.h"
#include "Protocol/InferenceResponseDispatcher.h"
#include "Transport/InferenceRequestTracker.h"
#include "Transport/ResponseStreamDecoder.h"
#include "Transport/TcpClient.h"

class TcpResponsePipelineTest final : public QObject
{
    Q_OBJECT

private slots:
    void receivesFragmentedInferenceResult();
    void receivesStickyResultAndError();
};

namespace
{

QByteArray makeResultPacket(
    quint64 sequence,
    const QString& detectionLevel,
    double score
)
{
    InferenceResponseCodec::
        InferenceResultPayload result;
    result.detectionLevel = detectionLevel;
    result.score = score;
    result.inferenceMs = 21;
    result.defectArea = 84;

    const QByteArray payload =
        InferenceResponseCodec::encodeResult(result);

    FrameProtocol::FrameHeader header;
    header.messageType =
        FrameProtocol::MessageType::InferenceResult;
    header.pixelFormat =
        FrameProtocol::PixelFormat::Unknown;
    header.sequence = sequence;
    header.timestampMs = 123456;

    return FrameProtocol::encodePacket(
        header,
        payload
    );
}

QByteArray makeErrorPacket(
    quint64 sequence,
    const QString& errorCode
)
{
    InferenceResponseCodec::
        InferenceErrorPayload error;
    error.errorCode = errorCode;
    error.message =
        QStringLiteral("Frame processing failed");

    const QByteArray payload =
        InferenceResponseCodec::encodeError(error);

    FrameProtocol::FrameHeader header;
    header.messageType =
        FrameProtocol::MessageType::InferenceError;
    header.pixelFormat =
        FrameProtocol::PixelFormat::Unknown;
    header.sequence = sequence;
    header.timestampMs = 123457;

    return FrameProtocol::encodePacket(
        header,
        payload
    );
}

QTcpSocket* connectPipeline(
    QTcpServer& server,
    TcpClient& client
)
{
    if(!server.listen(QHostAddress::LocalHost, 0))
    {
        return nullptr;
    }

    client.connectToServer(
        QStringLiteral("127.0.0.1"),
        server.serverPort()
    );

    if(!server.waitForNewConnection(2000))
    {
        return nullptr;
    }

    return server.nextPendingConnection();
}

}

void TcpResponsePipelineTest::
receivesFragmentedInferenceResult()
{
    QTcpServer server;
    TcpClient client;
    ResponseStreamDecoder decoder;
    InferenceResponseDispatcher dispatcher;
    InferenceRequestTracker tracker(1000);

    int acceptedResultCount = 0;

    connect(
        &client,
        &TcpClient::bytesReceived,
        &decoder,
        &ResponseStreamDecoder::appendBytes
    );
    connect(
        &decoder,
        &ResponseStreamDecoder::sigResponseDecoded,
        &dispatcher,
        &InferenceResponseDispatcher::dispatch
    );
    connect(
        &dispatcher,
        &InferenceResponseDispatcher::
            sigInferenceResultReceived,
        &tracker,
        [&tracker, &acceptedResultCount](
            quint64 sequence,
            const InferenceResponseCodec::
                InferenceResultPayload&)
        {
            if(tracker.completeRequest(sequence))
            {
                ++acceptedResultCount;
            }
        }
    );

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
    QSignalSpy completedSpy(
        &tracker,
        &InferenceRequestTracker::
            sigRequestCompleted
    );
    QSignalSpy unknownSpy(
        &tracker,
        &InferenceRequestTracker::
            sigUnknownResponse
    );

    QTcpSocket* peerSocket =
        connectPipeline(server, client);
    QVERIFY(peerSocket != nullptr);

    const QByteArray packet =
        makeResultPacket(
            9001,
            QStringLiteral("REVIEW"),
            0.64
        );
    QVERIFY(!packet.isEmpty());
    QVERIFY(tracker.recordRequest(9001));
    QCOMPARE(tracker.pendingCount(), 1);

    QCOMPARE(
        peerSocket->write(packet.left(17)),
        17
    );
    QVERIFY(peerSocket->flush());

    QTRY_COMPARE_WITH_TIMEOUT(
        decoder.bufferedBytes(),
        17,
        2000
    );
    QCOMPARE(resultSpy.count(), 0);

    QCOMPARE(
        peerSocket->write(packet.mid(17)),
        static_cast<qint64>(packet.size() - 17)
    );
    QVERIFY(peerSocket->flush());

    QTRY_COMPARE_WITH_TIMEOUT(
        resultSpy.count(),
        1,
        2000
    );

    QCOMPARE(rejectedSpy.count(), 0);
    QCOMPARE(decoder.bufferedBytes(), 0);
    QCOMPARE(tracker.pendingCount(), 0);
    QCOMPARE(completedSpy.count(), 1);
    QCOMPARE(unknownSpy.count(), 0);
    QCOMPARE(acceptedResultCount, 1);
    QCOMPARE(
        resultSpy.at(0).at(0).toULongLong(),
        9001
    );

    const auto result = qvariant_cast<
        InferenceResponseCodec::
            InferenceResultPayload
    >(resultSpy.at(0).at(1));

    QCOMPARE(
        result.detectionLevel,
        QStringLiteral("REVIEW")
    );
    QCOMPARE(result.score, 0.64);
    QCOMPARE(result.inferenceMs, 21);
    QCOMPARE(result.defectArea, 84);

    /*
     * 同一响应再次到达时，Dispatcher 仍能解码，
     * 但请求跟踪器会把它识别为重复响应。
     */
    QCOMPARE(
        peerSocket->write(packet),
        static_cast<qint64>(packet.size())
    );
    QVERIFY(peerSocket->flush());

    QTRY_COMPARE_WITH_TIMEOUT(
        resultSpy.count(),
        2,
        2000
    );
    QTRY_COMPARE_WITH_TIMEOUT(
        unknownSpy.count(),
        1,
        2000
    );

    QCOMPARE(completedSpy.count(), 1);
    QCOMPARE(acceptedResultCount, 1);
    QCOMPARE(tracker.pendingCount(), 0);

    client.disconnectFromServer();
    peerSocket->deleteLater();
}

void TcpResponsePipelineTest::
receivesStickyResultAndError()
{
    QTcpServer server;
    TcpClient client;
    ResponseStreamDecoder decoder;
    InferenceResponseDispatcher dispatcher;
    InferenceRequestTracker tracker(1000);

    connect(
        &client,
        &TcpClient::bytesReceived,
        &decoder,
        &ResponseStreamDecoder::appendBytes
    );
    connect(
        &decoder,
        &ResponseStreamDecoder::sigResponseDecoded,
        &dispatcher,
        &InferenceResponseDispatcher::dispatch
    );
    connect(
        &dispatcher,
        &InferenceResponseDispatcher::
            sigInferenceResultReceived,
        &tracker,
        [&tracker](
            quint64 sequence,
            const InferenceResponseCodec::
                InferenceResultPayload&)
        {
            tracker.completeRequest(sequence);
        }
    );
    connect(
        &dispatcher,
        &InferenceResponseDispatcher::
            sigInferenceErrorReceived,
        &tracker,
        [&tracker](
            quint64 sequence,
            const InferenceResponseCodec::
                InferenceErrorPayload&)
        {
            tracker.completeRequest(sequence);
        }
    );

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
    QSignalSpy completedSpy(
        &tracker,
        &InferenceRequestTracker::
            sigRequestCompleted
    );
    QSignalSpy unknownSpy(
        &tracker,
        &InferenceRequestTracker::
            sigUnknownResponse
    );

    QTcpSocket* peerSocket =
        connectPipeline(server, client);
    QVERIFY(peerSocket != nullptr);

    const QByteArray stickyBytes =
        makeResultPacket(
            9100,
            QStringLiteral("OK"),
            0.12
        )
        + makeErrorPacket(
            9101,
            QStringLiteral("INFERENCE_FAILED")
        );
    QVERIFY(!stickyBytes.isEmpty());
    QVERIFY(tracker.recordRequest(9100));
    QVERIFY(tracker.recordRequest(9101));
    QCOMPARE(tracker.pendingCount(), 2);

    QCOMPARE(
        peerSocket->write(stickyBytes),
        static_cast<qint64>(stickyBytes.size())
    );
    QVERIFY(peerSocket->flush());

    QTRY_COMPARE_WITH_TIMEOUT(
        resultSpy.count(),
        1,
        2000
    );
    QTRY_COMPARE_WITH_TIMEOUT(
        errorSpy.count(),
        1,
        2000
    );

    QCOMPARE(
        resultSpy.at(0).at(0).toULongLong(),
        9100
    );
    QCOMPARE(
        errorSpy.at(0).at(0).toULongLong(),
        9101
    );
    QCOMPARE(decoder.bufferedBytes(), 0);
    QCOMPARE(tracker.pendingCount(), 0);
    QCOMPARE(completedSpy.count(), 2);
    QCOMPARE(unknownSpy.count(), 0);

    const auto error = qvariant_cast<
        InferenceResponseCodec::
            InferenceErrorPayload
    >(errorSpy.at(0).at(1));
    QCOMPARE(
        error.errorCode,
        QStringLiteral("INFERENCE_FAILED")
    );

    client.disconnectFromServer();
    peerSocket->deleteLater();
}

QTEST_GUILESS_MAIN(TcpResponsePipelineTest)

#include "TcpResponsePipelineTest.moc"
