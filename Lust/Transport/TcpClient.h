#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QAbstractSocket>
#include <QObject>
#include <QString>
#include <QTcpSocket>

class QTcpSocket;

class TcpClient final : public QObject
{
    Q_OBJECT

public:
    explicit TcpClient(QObject* parent = nullptr);
    ~TcpClient() override = default;

    void connectToServer(const QString& host, quint16 port);

    void disconnectFromServer();

    bool isConnected() const ;

    QAbstractSocket::SocketState state() const;
signals:
    void connected();
    void disconnected();
    void stateChanged(QAbstractSocket::SocketState state);

    void connectionError(const QString& errorMessage);

private:
    QTcpSocket* socket_ = nullptr;
};





#endif
