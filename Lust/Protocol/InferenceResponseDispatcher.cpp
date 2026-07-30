#include "InferenceResponseDispatcher.h"

InferenceResponseDispatcher::InferenceResponseDispatcher(QObject* parent)
    : QObject(parent)
{
    qRegisterMetaType<InferenceResponseCodec::InferenceResultPayload>("InferenceResponseCodec::""InferenceResultPayload");

    qRegisterMetaType<InferenceResponseCodec::InferenceErrorPayload>("InferenceResponseCodec::""InferenceErrorPayload");
}


void InferenceResponseDispatcher::dispatch(const ReceivedResponsePacket& packet)
{
    const quint64 sequence = packet.header.sequence;

    if(packet.header.messageType == FrameProtocol::MessageType::InferenceResult)
    {
        InferenceResponseCodec::InferenceResultPayload result;

        QString errorMessage;

        const bool decoded = InferenceResponseCodec::decodeResult(packet.payload,&result,&errorMessage);

        if(!decoded)
        {
            if(errorMessage.isEmpty())
            {
                errorMessage = QStringLiteral("Unknown inference result payload error");
            }

            emit sigResponseRejected(sequence,QStringLiteral("Invalid inference result: %1").arg(errorMessage));
            return;
        }

        emit sigInferenceResultReceived(sequence,result);
        return;
    }

    if(packet.header.messageType == FrameProtocol::MessageType::InferenceError)
    {
        InferenceResponseCodec::InferenceErrorPayload error;

        QString errorMessage;

        const bool decoded = InferenceResponseCodec::decodeError(packet.payload,&error,&errorMessage);

        if(!decoded)
        {
            if(errorMessage.isEmpty())
            {
                errorMessage = QStringLiteral("Unknown inference error payload error");
            }

            emit sigResponseRejected(sequence, QStringLiteral("Invalid inference error: %1").arg(errorMessage));
            return;
        }

        emit sigInferenceErrorReceived(sequence, error);
        return;
    }


    emit sigResponseRejected(sequence, QStringLiteral("Unexpected response message type"));
}
