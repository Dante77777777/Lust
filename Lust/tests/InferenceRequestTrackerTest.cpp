#include <QSignalSpy>
#include <QtTest>

#include "Transport/InferenceRequestTracker.h"

class InferenceRequestTrackerTest final
    : public QObject
{
    Q_OBJECT

private slots:
    void recordsAndCompletesRequest();
    void rejectsUnknownResponse();
    void rejectsDuplicateRequest();
    void timesOutPendingRequest();
    void clearsPendingRequests();
    void enforcesPendingCapacity();
};

void InferenceRequestTrackerTest::
recordsAndCompletesRequest()
{
    InferenceRequestTracker tracker(1000);
    QSignalSpy completedSpy(
        &tracker,
        &InferenceRequestTracker::
            sigRequestCompleted
    );
    QSignalSpy unknownSpy(
        &tracker,
        &InferenceRequestTracker::
            sigUnknownResponse
    );

    QCOMPARE(tracker.timeoutMs(), 1000);
    QCOMPARE(tracker.pendingCount(), 0);
    QVERIFY(tracker.canAcceptRequest());

    QVERIFY(tracker.recordRequest(42));
    QCOMPARE(tracker.pendingCount(), 1);
    QVERIFY(tracker.contains(42));

    QTest::qWait(10);

    QVERIFY(tracker.completeRequest(42));
    QCOMPARE(tracker.pendingCount(), 0);
    QVERIFY(!tracker.contains(42));
    QCOMPARE(completedSpy.count(), 1);
    QCOMPARE(unknownSpy.count(), 0);
    QCOMPARE(
        completedSpy.at(0).at(0).toULongLong(),
        42
    );
    QVERIFY(
        completedSpy.at(0).at(1).toLongLong()
        >= 0
    );
}

void InferenceRequestTrackerTest::
rejectsUnknownResponse()
{
    InferenceRequestTracker tracker;
    QSignalSpy unknownSpy(
        &tracker,
        &InferenceRequestTracker::
            sigUnknownResponse
    );

    QVERIFY(!tracker.completeRequest(99));

    QCOMPARE(unknownSpy.count(), 1);
    QCOMPARE(
        unknownSpy.at(0).at(0).toULongLong(),
        99
    );
    QCOMPARE(tracker.pendingCount(), 0);
}

void InferenceRequestTrackerTest::
rejectsDuplicateRequest()
{
    InferenceRequestTracker tracker;
    QSignalSpy duplicateSpy(
        &tracker,
        &InferenceRequestTracker::
            sigDuplicateRequest
    );

    QVERIFY(tracker.recordRequest(7));
    QVERIFY(!tracker.recordRequest(7));

    QCOMPARE(duplicateSpy.count(), 1);
    QCOMPARE(
        duplicateSpy.at(0).at(0).toULongLong(),
        7
    );
    QCOMPARE(tracker.pendingCount(), 1);
}

void InferenceRequestTrackerTest::
timesOutPendingRequest()
{
    InferenceRequestTracker tracker(30);
    QSignalSpy timedOutSpy(
        &tracker,
        &InferenceRequestTracker::
            sigRequestTimedOut
    );

    QVERIFY(tracker.recordRequest(1234));

    QTRY_COMPARE_WITH_TIMEOUT(
        timedOutSpy.count(),
        1,
        1000
    );

    QCOMPARE(tracker.pendingCount(), 0);
    QVERIFY(!tracker.contains(1234));
    QCOMPARE(
        timedOutSpy.at(0).at(0).toULongLong(),
        1234
    );
    QVERIFY(
        timedOutSpy.at(0).at(1).toLongLong()
        >= 30
    );

    QVERIFY(!tracker.completeRequest(1234));
}

void InferenceRequestTrackerTest::
clearsPendingRequests()
{
    InferenceRequestTracker tracker;
    QSignalSpy clearedSpy(
        &tracker,
        &InferenceRequestTracker::
            sigPendingRequestsCleared
    );

    QVERIFY(tracker.recordRequest(1));
    QVERIFY(tracker.recordRequest(2));
    QCOMPARE(tracker.pendingCount(), 2);

    tracker.clear();

    QCOMPARE(tracker.pendingCount(), 0);
    QCOMPARE(clearedSpy.count(), 1);
    QCOMPARE(
        clearedSpy.at(0).at(0).toLongLong(),
        2
    );

    tracker.clear();
    QCOMPARE(clearedSpy.count(), 1);
}

void InferenceRequestTrackerTest::
enforcesPendingCapacity()
{
    InferenceRequestTracker tracker;
    QSignalSpy capacitySpy(
        &tracker,
        &InferenceRequestTracker::
            sigCapacityExceeded
    );

    for(qsizetype index = 0;
        index
            < InferenceRequestTracker::
                MAX_PENDING_REQUESTS;
        ++index)
    {
        QVERIFY(
            tracker.recordRequest(
                static_cast<quint64>(index)
            )
        );
    }

    QCOMPARE(
        tracker.pendingCount(),
        InferenceRequestTracker::
            MAX_PENDING_REQUESTS
    );
    QVERIFY(!tracker.canAcceptRequest());

    const quint64 rejectedSequence = 9999;
    QVERIFY(
        !tracker.recordRequest(rejectedSequence)
    );

    QCOMPARE(capacitySpy.count(), 1);
    QCOMPARE(
        capacitySpy.at(0).at(0).toULongLong(),
        rejectedSequence
    );
    QCOMPARE(
        capacitySpy.at(0).at(1).toLongLong(),
        InferenceRequestTracker::
            MAX_PENDING_REQUESTS
    );
    QCOMPARE(
        tracker.pendingCount(),
        InferenceRequestTracker::
            MAX_PENDING_REQUESTS
    );
}

QTEST_GUILESS_MAIN(InferenceRequestTrackerTest)

#include "InferenceRequestTrackerTest.moc"
