#include "InferenceRequestTracker.h"

#include <QPair>
#include <QVector>

InferenceRequestTracker::InferenceRequestTracker(
    int timeoutMs,
    QObject* parent
)
    : QObject(parent)
    , timeoutMs_(qMax(1, timeoutMs))
{
    elapsedTimer_.start();

    timeoutTimer_.setInterval(
        qBound(
            1,
            timeoutMs_ / 2,
            1000
        )
    );

    connect(
        &timeoutTimer_,
        &QTimer::timeout,
        this,
        &InferenceRequestTracker::checkTimeouts
    );

    timeoutTimer_.start();
}

int InferenceRequestTracker::timeoutMs()
    const noexcept
{
    return timeoutMs_;
}

qsizetype InferenceRequestTracker::pendingCount()
    const noexcept
{
    return pendingRequests_.size();
}

bool InferenceRequestTracker::contains(
    quint64 sequence
) const
{
    return pendingRequests_.contains(sequence);
}

bool InferenceRequestTracker::canAcceptRequest()
    const noexcept
{
    return pendingRequests_.size()
        < MAX_PENDING_REQUESTS;
}

bool InferenceRequestTracker::recordRequest(
    quint64 sequence
)
{
    if(pendingRequests_.contains(sequence))
    {
        emit sigDuplicateRequest(sequence);
        return false;
    }

    if(!canAcceptRequest())
    {
        emit sigCapacityExceeded(
            sequence,
            pendingRequests_.size()
        );
        return false;
    }

    pendingRequests_.insert(
        sequence,
        elapsedTimer_.elapsed()
    );

    return true;
}

bool InferenceRequestTracker::completeRequest(
    quint64 sequence
)
{
    auto iterator =
        pendingRequests_.find(sequence);

    if(iterator == pendingRequests_.end())
    {
        emit sigUnknownResponse(sequence);
        return false;
    }

    const qint64 startedAtMs = iterator.value();

    pendingRequests_.erase(iterator);

    const qint64 elapsedMs =
        elapsedTimer_.elapsed() - startedAtMs;

    emit sigRequestCompleted(
        sequence,
        elapsedMs
    );

    return true;
}

void InferenceRequestTracker::clear()
{
    const qsizetype clearedCount =
        pendingRequests_.size();

    pendingRequests_.clear();

    if(clearedCount > 0)
    {
        emit sigPendingRequestsCleared(
            clearedCount
        );
    }
}

void InferenceRequestTracker::checkTimeouts()
{
    const qint64 currentTimeMs =
        elapsedTimer_.elapsed();

    QVector<QPair<quint64, qint64>>
        timedOutRequests;

    auto iterator = pendingRequests_.begin();

    while(iterator != pendingRequests_.end())
    {
        const qint64 elapsedMs =
            currentTimeMs - iterator.value();

        if(elapsedMs < timeoutMs_)
        {
            ++iterator;
            continue;
        }

        timedOutRequests.append(
            qMakePair(
                iterator.key(),
                elapsedMs
            )
        );

        iterator =
            pendingRequests_.erase(iterator);
    }

    /*
     * 先删除全部超时项，再发送信号，避免同步信号回调
     * 修改 pendingRequests_ 导致迭代器失效。
     */
    for(const auto& request : timedOutRequests)
    {
        emit sigRequestTimedOut(
            request.first,
            request.second
        );
    }
}
