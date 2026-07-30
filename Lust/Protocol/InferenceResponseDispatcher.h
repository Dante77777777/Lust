#ifndef INFERENCERESPONSEDISPATCHER_H
#define INFERENCERESPONSEDISPATCHER_H

#include <QObject>
#include <QString>

#include "InferenceResponseCodec.h"
#include "../Transport/ResponseStreamDecoder.h"


class InferenceResponseDispatcher final : public QObject
{
    Q_OBJECT
public:
    explicit InferenceResponseDispatcher(QObject* parent = nullptr);

public slots:
    void dispatch(const ReceivedResponsePacket& packet);

signals:
    void sigInferenceResultReceived(quint64 sequence, const InferenceResponseCodec::InferenceResultPayload& result);

    void sigInferenceErrorReceived(quint64 sequence, const InferenceResponseCodec::InferenceErrorPayload& error);

    void sigStopAcquisitionRequested(quint64 requestSequence);

    void sigResponseRejected(quint64 sequence, const QString& errorMessage);
};







#endif
