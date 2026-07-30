#include "TcpClient.h"

TcpClient::TcpClient(QObject* parent)
    : QObject(parent)
    , socket_(new QTcpSocket(this))
{
    connect(socket_,&QTcpSocket::connected,this,&TcpClient::connected);

    connect(socket_,&QTcpSocket::disconnected,this,&TcpClient::disconnected);

    connect(socket_,&QTcpSocket::stateChanged,this,&TcpClient::stateChanged);

    connect(socket_,&QTcpSocket::errorOccurred,this,[this](QAbstractSocket::SocketError socketError){
        if(socketError == QAbstractSocket::RemoteHostClosedError)
        {
            return ;
        }

        emit connectionError(socket_->errorString());
    });

    connect(socket_,&QTcpSocket::bytesWritten,this,&TcpClient::bytesWritten);

    connect(socket_,&QTcpSocket::readyRead,this,[this](){
        const QByteArray bytes = socket_->readAll();
        if(!bytes.isEmpty())
        {
            emit bytesReceived(bytes);
        }
    });
}

void TcpClient::connectToServer(const QString& host, quint16 port)
{
    const QString normalizedHost =host.trimmed();

    if(normalizedHost.isEmpty() || port == 0)
    {
        emit connectionError(QStringLiteral("Invalid TCP endpoint"));
        return ;
    }

    if(socket_->state() != QAbstractSocket::UnconnectedState)
    {
        socket_->abort();
    }
    socket_->connectToHost(normalizedHost,port);
}

void TcpClient::disconnectFromServer()
{
    if(socket_->state() == QAbstractSocket::UnconnectedState)
    {
        return ;
    }

    if(socket_->state() == QAbstractSocket::ConnectingState || socket_->state() == QAbstractSocket::HostLookupState)
    {
        socket_->abort();
        return ;
    }

    socket_->disconnectFromHost();
}

bool TcpClient::isConnected() const
{
    return socket_->state() == QAbstractSocket::ConnectedState;
}

QAbstractSocket::SocketState TcpClient::state() const
{
    return socket_->state();
}


bool TcpClient::sendPacket(const QByteArray& packet)
{
    if(packet.isEmpty())
    {
        emit sendError(QStringLiteral("Packet is Empty"));
        return false;
    }

    if(!isConnected())
    {
        emit sendError(QStringLiteral("TCP Client is not Connected"));
        return false;
    }

    const qint64 packetSize = static_cast<qint64>(packet.size());

    const qint64 currentPendingBytes = socket_->bytesToWrite();

    if(packetSize > MAX_PENDING_BYTES || currentPendingBytes > MAX_PENDING_BYTES - packetSize)
    {
        emit sendError(QStringLiteral("TCP Send Buffer Limit Exceeded"));
        return false;
    }

    const qint64 acceptedBytes = socket_->write(packet);
    if(acceptedBytes < 0)
    {
        emit sendError(socket_->errorString());
        return false;
    }

    if(acceptedBytes != packetSize)
    {
        /*
         * 协议包只写入一部分会破坏 TCP 字节流中的
         * 帧边界，因此关闭连接，避免继续发送错误数据。
         */
        socket_->abort();
        emit sendError(QString("Incomplete Packet Write"));
        return false;
    }
    emit packetQueued(acceptedBytes);
    return true;
}

qint64 TcpClient::pendingBytes() const
{
    return socket_->bytesToWrite();
}
