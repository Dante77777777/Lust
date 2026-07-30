#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QAbstractSocket>
#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QByteArray>

class QTcpSocket;

class TcpClient final : public QObject
{
    Q_OBJECT

public:
    explicit TcpClient(QObject* parent = nullptr);
    static constexpr qint64 MAX_PENDING_BYTES = 16LL * 1024LL * 1024LL;
    ~TcpClient() override = default;

    void connectToServer(const QString& host, quint16 port);

    void disconnectFromServer();

    bool isConnected() const ;

    QAbstractSocket::SocketState state() const;

    bool sendPacket(const QByteArray& packet);

    qint64 pendingBytes() const;
signals:
    void connected();
    void disconnected();
    void stateChanged(QAbstractSocket::SocketState state);

    void connectionError(const QString& errorMessage);

    // 一个完整数据包已经放入 Qt 的发送缓冲区
    void packetQueued(qint64 byteCount);

    // 数据实际写入操作系统网络层
    void bytesWritten(qint64 byteCount);

    // 收到一段服务端 TCP 字节。
    // 一次信号不保证对应一个完整协议包。
    void bytesReceived(const QByteArray& bytes);

    void sendError(const QString& errorMessage);

private:
    QTcpSocket* socket_ = nullptr;
};





#endif
