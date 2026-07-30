#include "ResponseStreamDecoder.h"

ResponseStreamDecoder::ResponseStreamDecoder(QObject* parent)
    : QObject(parent)
{

}


qsizetype ResponseStreamDecoder::bufferedBytes() const noexcept
{
    return receiveBuffer_.size();
}


void ResponseStreamDecoder::reset()
{
    receiveBuffer_.clear();
}


void ResponseStreamDecoder::appendBytes(const QByteArray& bytes)
{
    if(bytes.isEmpty())
    {
        return ;
    }
    receiveBuffer_.append(bytes);

    while(true)
    {
        FrameProtocol::FrameHeader header;
        QByteArray payload;
        QString errorMessage;

        const FrameProtocol::DecodeResult result = FrameProtocol::tryDecodePacket(receiveBuffer_,header,payload,&errorMessage);

        if(result == FrameProtocol::DecodeResult::Complete)
        {
            ReceivedResponsePacket packet;
            packet.header = header;
            packet.payload = std::move(payload);
            emit sigResponseDecoded(packet);
            continue;
        }

        if(result == FrameProtocol::DecodeResult::InvalidData)
        {
            receiveBuffer_.clear();
            if(errorMessage.isEmpty())
            {
                errorMessage = QStringLiteral("Invalid response protocol data");
            }

            emit sigProtocolError(errorMessage);
            return ;
        }

        if(receiveBuffer_.size() > MAX_BUFFERED_BYTES)
        {
            const qsizetype bufferedSize = receiveBuffer_.size();

            receiveBuffer_.clear();
            emit sigBufferOverflow(bufferedSize);
            return ;
        }
        return ;
    }
}
