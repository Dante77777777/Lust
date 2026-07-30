#ifndef RESPONSESTREAMDECODER_H
#define RESPONSESTREAMDECODER_H

#include <QByteArray>
#include <QObject>
#include <QString>

#include <utility>

#include "../Protocol/FrameProtocol.h"

struct ReceivedResponsePacket
{
    FrameProtocol::FrameHeader header;
    QByteArray payload;
};

Q_DECLARE_METATYPE(ReceivedResponsePacket)

class ResponseStreamDecoder final : public QObject
{
    Q_OBJECT
public:
    static constexpr qsizetype MAX_BUFFERED_BYTES =static_cast<qsizetype>(FrameProtocol::HEADER_SIZE) + static_cast<qsizetype>(FrameProtocol::MAX_PAYLOAD_SIZE);

    explicit ResponseStreamDecoder(QObject* parent = nullptr);

    qsizetype bufferedBytes() const noexcept;

    void reset();

public slots:
    void appendBytes(const QByteArray& bytes);

signals:
    void sigResponseDecoded(const ReceivedResponsePacket& packet);

    void sigProtocolError(const QString& errorMessage);

    void sigBufferOverflow(qsizetype bufferedBytes);

private:
    QByteArray receiveBuffer_;
};








#endif
