#include <QHostAddress>
#include <QSignalSpy>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtTest>

#include "Protocol/FrameProtocol.h"
#include "Transport/TcpClient.h"

class TcpClientTest : public QObject
{
    Q_OBJECT

private slots:
    void startsDisconnected();
    void rejectsInvalidEndpoint();
    void connectsAndDisconnectsFromLocalServer();
    void reportsRemoteDisconnect();
    void rejectsInvalidSendRequests();
    void sendsCompleteProtocolPacket();
    void preservesPacketOrder();
    void rejectsPacketLargerThanPendingLimit();
    void receivesServerBytesWithoutLoss();
};

namespace
{

FrameProtocol::FrameHeader makeFrameHeader(
    quint64 sequence
)
{
    FrameProtocol::FrameHeader header;
    header.messageType =
        FrameProtocol::MessageType::ImageFrame;
    header.pixelFormat =
        FrameProtocol::PixelFormat::Jpeg;
    header.sequence = sequence;
    header.timestampMs = 1785211200123ULL;
    header.width = 640;
    header.height = 480;
    return header;
}

}

void TcpClientTest::startsDisconnected()
{
    TcpClient client;

    QVERIFY(!client.isConnected());
    QCOMPARE(
        client.state(),
        QAbstractSocket::UnconnectedState
    );
}

void TcpClientTest::rejectsInvalidEndpoint()
{
    TcpClient client;
    QSignalSpy errorSpy(
        &client,
        &TcpClient::connectionError
    );

    client.connectToServer(QStringLiteral("   "), 9000);

    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(
        errorSpy.takeFirst().at(0).toString(),
        QStringLiteral("Invalid TCP endpoint")
    );
    QVERIFY(!client.isConnected());

    client.connectToServer(
        QStringLiteral("127.0.0.1"),
        0
    );

    QCOMPARE(errorSpy.count(), 1);
    QCOMPARE(
        errorSpy.takeFirst().at(0).toString(),
        QStringLiteral("Invalid TCP endpoint")
    );
    QVERIFY(!client.isConnected());
}

void TcpClientTest::connectsAndDisconnectsFromLocalServer()
{
    QTcpServer server;
    QVERIFY(
        server.listen(QHostAddress::LocalHost, 0)
    );

    TcpClient client;
    QSignalSpy connectedSpy(
        &client,
        &TcpClient::connected
    );
    QSignalSpy disconnectedSpy(
        &client,
        &TcpClient::disconnected
    );
    QSignalSpy errorSpy(
        &client,
        &TcpClient::connectionError
    );

    client.connectToServer(
        QStringLiteral("127.0.0.1"),
        server.serverPort()
    );

    QTRY_COMPARE_WITH_TIMEOUT(
        connectedSpy.count(),
        1,
        2000
    );
    QTRY_VERIFY_WITH_TIMEOUT(
        server.hasPendingConnections(),
        2000
    );

    QTcpSocket* peerSocket =
        server.nextPendingConnection();

    QVERIFY(peerSocket != nullptr);
    QVERIFY(client.isConnected());
    QCOMPARE(
        client.state(),
        QAbstractSocket::ConnectedState
    );
    QCOMPARE(errorSpy.count(), 0);

    client.disconnectFromServer();

    QTRY_COMPARE_WITH_TIMEOUT(
        disconnectedSpy.count(),
        1,
        2000
    );
    QTRY_COMPARE_WITH_TIMEOUT(
        client.state(),
        QAbstractSocket::UnconnectedState,
        2000
    );
    QVERIFY(!client.isConnected());

    peerSocket->deleteLater();
}

void TcpClientTest::reportsRemoteDisconnect()
{
    QTcpServer server;
    QVERIFY(
        server.listen(QHostAddress::LocalHost, 0)
    );

    TcpClient client;
    QSignalSpy connectedSpy(
        &client,
        &TcpClient::connected
    );
    QSignalSpy disconnectedSpy(
        &client,
        &TcpClient::disconnected
    );
    QSignalSpy errorSpy(
        &client,
        &TcpClient::connectionError
    );

    client.connectToServer(
        QStringLiteral("127.0.0.1"),
        server.serverPort()
    );

    QTRY_COMPARE_WITH_TIMEOUT(
        connectedSpy.count(),
        1,
        2000
    );
    QTRY_VERIFY_WITH_TIMEOUT(
        server.hasPendingConnections(),
        2000
    );

    QTcpSocket* peerSocket =
        server.nextPendingConnection();

    QVERIFY(peerSocket != nullptr);
    peerSocket->disconnectFromHost();

    QTRY_COMPARE_WITH_TIMEOUT(
        disconnectedSpy.count(),
        1,
        2000
    );
    QTRY_VERIFY_WITH_TIMEOUT(
        !client.isConnected(),
        2000
    );
    QCOMPARE(errorSpy.count(), 0);

    peerSocket->deleteLater();
}

void TcpClientTest::rejectsInvalidSendRequests()
{
    TcpClient client;
    QSignalSpy sendErrorSpy(
        &client,
        &TcpClient::sendError
    );
    QSignalSpy packetQueuedSpy(
        &client,
        &TcpClient::packetQueued
    );

    QVERIFY(!client.sendPacket(QByteArray()));
    QCOMPARE(sendErrorSpy.count(), 1);
    QCOMPARE(packetQueuedSpy.count(), 0);

    QVERIFY(
        !client.sendPacket(
            QByteArray("not-connected")
        )
    );
    QCOMPARE(sendErrorSpy.count(), 2);
    QCOMPARE(packetQueuedSpy.count(), 0);
    QCOMPARE(client.pendingBytes(), 0);
}

void TcpClientTest::sendsCompleteProtocolPacket()
{
    QTcpServer server;
    QVERIFY(
        server.listen(QHostAddress::LocalHost, 0)
    );

    TcpClient client;
    QSignalSpy connectedSpy(
        &client,
        &TcpClient::connected
    );
    QSignalSpy packetQueuedSpy(
        &client,
        &TcpClient::packetQueued
    );
    QSignalSpy bytesWrittenSpy(
        &client,
        &TcpClient::bytesWritten
    );
    QSignalSpy sendErrorSpy(
        &client,
        &TcpClient::sendError
    );

    client.connectToServer(
        QStringLiteral("127.0.0.1"),
        server.serverPort()
    );

    QTRY_COMPARE_WITH_TIMEOUT(
        connectedSpy.count(),
        1,
        2000
    );
    QTRY_VERIFY_WITH_TIMEOUT(
        server.hasPendingConnections(),
        2000
    );

    QTcpSocket* peerSocket =
        server.nextPendingConnection();
    QVERIFY(peerSocket != nullptr);

    const QByteArray payload =
        QByteArray::fromHex("FFD8FFE000104A46494600FFD9");
    const QByteArray packet =
        FrameProtocol::encodePacket(
            makeFrameHeader(7),
            payload
        );

    QVERIFY(!packet.isEmpty());
    QVERIFY(client.sendPacket(packet));
    QCOMPARE(packetQueuedSpy.count(), 1);
    QCOMPARE(
        packetQueuedSpy.takeFirst().at(0).toLongLong(),
        static_cast<qint64>(packet.size())
    );
    QCOMPARE(sendErrorSpy.count(), 0);

    QTRY_VERIFY_WITH_TIMEOUT(
        peerSocket->bytesAvailable()
            >= packet.size(),
        2000
    );
    QCOMPARE(peerSocket->readAll(), packet);

    QTRY_VERIFY_WITH_TIMEOUT(
        bytesWrittenSpy.count() >= 1,
        2000
    );

    client.disconnectFromServer();
    peerSocket->deleteLater();
}

void TcpClientTest::preservesPacketOrder()
{
    QTcpServer server;
    QVERIFY(
        server.listen(QHostAddress::LocalHost, 0)
    );

    TcpClient client;
    QSignalSpy connectedSpy(
        &client,
        &TcpClient::connected
    );
    QSignalSpy packetQueuedSpy(
        &client,
        &TcpClient::packetQueued
    );

    client.connectToServer(
        QStringLiteral("127.0.0.1"),
        server.serverPort()
    );

    QTRY_COMPARE_WITH_TIMEOUT(
        connectedSpy.count(),
        1,
        2000
    );
    QTRY_VERIFY_WITH_TIMEOUT(
        server.hasPendingConnections(),
        2000
    );

    QTcpSocket* peerSocket =
        server.nextPendingConnection();
    QVERIFY(peerSocket != nullptr);

    const QByteArray firstPacket =
        FrameProtocol::encodePacket(
            makeFrameHeader(100),
            QByteArray("first-frame")
        );
    const QByteArray secondPacket =
        FrameProtocol::encodePacket(
            makeFrameHeader(101),
            QByteArray("second-frame")
        );
    const QByteArray expectedBytes =
        firstPacket + secondPacket;

    QVERIFY(client.sendPacket(firstPacket));
    QVERIFY(client.sendPacket(secondPacket));
    QCOMPARE(packetQueuedSpy.count(), 2);

    QTRY_VERIFY_WITH_TIMEOUT(
        peerSocket->bytesAvailable()
            >= expectedBytes.size(),
        2000
    );
    QCOMPARE(
        peerSocket->readAll(),
        expectedBytes
    );

    client.disconnectFromServer();
    peerSocket->deleteLater();
}

void TcpClientTest::rejectsPacketLargerThanPendingLimit()
{
    QTcpServer server;
    QVERIFY(
        server.listen(QHostAddress::LocalHost, 0)
    );

    TcpClient client;
    QSignalSpy connectedSpy(
        &client,
        &TcpClient::connected
    );
    QSignalSpy sendErrorSpy(
        &client,
        &TcpClient::sendError
    );
    QSignalSpy packetQueuedSpy(
        &client,
        &TcpClient::packetQueued
    );

    client.connectToServer(
        QStringLiteral("127.0.0.1"),
        server.serverPort()
    );

    QTRY_COMPARE_WITH_TIMEOUT(
        connectedSpy.count(),
        1,
        2000
    );
    QTRY_VERIFY_WITH_TIMEOUT(
        server.hasPendingConnections(),
        2000
    );

    QTcpSocket* peerSocket =
        server.nextPendingConnection();
    QVERIFY(peerSocket != nullptr);

    const QByteArray oversizedPacket(
        static_cast<qsizetype>(
            TcpClient::MAX_PENDING_BYTES + 1
        ),
        'x'
    );

    QVERIFY(!client.sendPacket(oversizedPacket));
    QCOMPARE(sendErrorSpy.count(), 1);
    QCOMPARE(packetQueuedSpy.count(), 0);
    QCOMPARE(client.pendingBytes(), 0);
    QCOMPARE(peerSocket->bytesAvailable(), 0);

    client.disconnectFromServer();
    peerSocket->deleteLater();
}


void TcpClientTest::receivesServerBytesWithoutLoss()
{
    QTcpServer server;
    QVERIFY(
        server.listen(QHostAddress::LocalHost, 0)
    );

    TcpClient client;
    QSignalSpy connectedSpy(
        &client,
        &TcpClient::connected
    );
    QSignalSpy receivedSpy(
        &client,
        &TcpClient::bytesReceived
    );

    client.connectToServer(
        QStringLiteral("127.0.0.1"),
        server.serverPort()
    );

    QTRY_COMPARE_WITH_TIMEOUT(
        connectedSpy.count(),
        1,
        2000
    );
    QTRY_VERIFY_WITH_TIMEOUT(
        server.hasPendingConnections(),
        2000
    );

    QTcpSocket* peerSocket =
        server.nextPendingConnection();
    QVERIFY(peerSocket != nullptr);

    const QByteArray firstBytes(
        "response-header-fragment"
    );
    const QByteArray secondBytes(
        "-and-json-payload"
    );

    QCOMPARE(
        peerSocket->write(firstBytes),
        static_cast<qint64>(firstBytes.size())
    );

    QTRY_VERIFY_WITH_TIMEOUT(
        receivedSpy.count() >= 1,
        2000
    );

    QCOMPARE(
        peerSocket->write(secondBytes),
        static_cast<qint64>(secondBytes.size())
    );

    QByteArray receivedBytes;
    QTRY_VERIFY_WITH_TIMEOUT(
        ([&receivedSpy, &receivedBytes,
          &firstBytes, &secondBytes]()
            {
                receivedBytes.clear();

                for(const QList<QVariant>& arguments
                    : receivedSpy)
                {
                    receivedBytes.append(
                        arguments.at(0).toByteArray()
                    );
                }

                return receivedBytes.size()
                    >= firstBytes.size()
                        + secondBytes.size();
            }()),
        2000
    );

    QCOMPARE(
        receivedBytes,
        firstBytes + secondBytes
    );

    client.disconnectFromServer();
    peerSocket->deleteLater();
}


QTEST_GUILESS_MAIN(TcpClientTest)

#include "TcpClientTest.moc"
