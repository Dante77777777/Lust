#include "TcpClient.h"

TcpClient::TcpClient(QObject* parent)
    : QObject(parent)
    , socket_(new QTcpSocket(this))
{
    connect(socket_,&QTcpSocket::connected,this,&TcpClient::connected);

    connect(socket_,&QTcpSocket::disconnected,this,&TcpClient::disconnected);

    connect(socket_,&QTcpSocket::stateChanged,this,&TcpClient::stateChanged);

    connect(socket_,&QTcpSocket::errorOccurred,this,[this](){
        emit connectionError(socket_->errorString());
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
