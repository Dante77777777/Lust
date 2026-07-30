#ifndef INFERENCEREQUESTTRACKER_H
#define INFERENCEREQUESTTRACKER_H

#include <QElapsedTimer>
#include <QHash>
#include <QObject>
#include <QTimer>
#include <QtGlobal>

class InferenceRequestTracker final : public QObject
{
    Q_OBJECT

public:
    static constexpr int DEFAULT_TIMEOUT_MS = 5000;

    static constexpr qsizetype
        MAX_PENDING_REQUESTS = 512;

    explicit InferenceRequestTracker(
        int timeoutMs = DEFAULT_TIMEOUT_MS,
        QObject* parent = nullptr
    );

    int timeoutMs() const noexcept;

    qsizetype pendingCount() const noexcept;

    bool contains(quint64 sequence) const;

    bool canAcceptRequest() const noexcept;

    bool recordRequest(quint64 sequence);

    bool completeRequest(quint64 sequence);

    void clear();

signals:
    void sigRequestCompleted(
        quint64 sequence,
        qint64 elapsedMs
    );

    void sigRequestTimedOut(
        quint64 sequence,
        qint64 elapsedMs
    );

    void sigUnknownResponse(quint64 sequence);

    void sigDuplicateRequest(quint64 sequence);

    void sigCapacityExceeded(
        quint64 sequence,
        qsizetype pendingCount
    );

    void sigPendingRequestsCleared(
        qsizetype clearedCount
    );

private slots:
    void checkTimeouts();

private:
    QHash<quint64, qint64> pendingRequests_;

    QElapsedTimer elapsedTimer_;
    QTimer timeoutTimer_;

    int timeoutMs_ = DEFAULT_TIMEOUT_MS;
};

#endif
