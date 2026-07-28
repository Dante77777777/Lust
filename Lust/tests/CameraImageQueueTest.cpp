#include <chrono>
#include <future>
#include <thread>

#include <QElapsedTimer>
#include <QtTest>

#include "CameraInterface/CameraImageQueue.h"

class CameraImageQueueTest : public QObject
{
    Q_OBJECT

private slots:
    void pushAndPopPreserveImage();
    void fullQueueTimesOut();
    void clearRestoresAllBuffers();
    void returnedImageDoesNotAliasPoolBuffer();
    void clearWakesBlockedProducer();
    void shutdownWakesBlockedConsumer();
};

void CameraImageQueueTest::pushAndPopPreserveImage()
{
    CameraImageQueue queue(2);

    QVERIFY(queue.maxSize() == 2);
    QVERIFY(queue.size() == 0);
    QVERIFY(queue.isEmpty());
    QVERIFY(!queue.isFull());

    const cv::Mat input(
        4,
        4,
        CV_8UC3,
        cv::Scalar(10, 20, 30)
    );

    QCOMPARE(
        queue.push(input, 50),
        static_cast<uint32_t>(CHONGMING_OK)
    );

    QVERIFY(queue.size() == 1);
    QVERIFY(!queue.isEmpty());

    cv::Mat output;

    QCOMPARE(
        queue.pop(output, 50),
        static_cast<uint32_t>(CHONGMING_OK)
    );

    QVERIFY(queue.size() == 0);
    QVERIFY(queue.isEmpty());
    QCOMPARE(output.rows, input.rows);
    QCOMPARE(output.cols, input.cols);
    QCOMPARE(output.type(), input.type());
    QCOMPARE(cv::norm(input, output, cv::NORM_INF), 0.0);
}

void CameraImageQueueTest::fullQueueTimesOut()
{
    CameraImageQueue queue(1);

    const cv::Mat firstFrame(
        2,
        2,
        CV_8UC3,
        cv::Scalar(1, 2, 3)
    );

    const cv::Mat secondFrame(
        2,
        2,
        CV_8UC3,
        cv::Scalar(4, 5, 6)
    );

    QCOMPARE(
        queue.push(firstFrame, 50),
        static_cast<uint32_t>(CHONGMING_OK)
    );

    QVERIFY(queue.isFull());

    QElapsedTimer timer;
    timer.start();

    const auto ret = queue.push(secondFrame, 50);
    const qint64 elapsedMs = timer.elapsed();

    QCOMPARE(
        ret,
        static_cast<uint32_t>(PUSH_IMGAE_FAILED)
    );

    QVERIFY(elapsedMs >= 30);
    QVERIFY(elapsedMs < 1000);
    QVERIFY(queue.isFull());
    QVERIFY(queue.size() == 1);
}

void CameraImageQueueTest::clearRestoresAllBuffers()
{
    CameraImageQueue queue(2);

    const cv::Mat frameA(
        2,
        2,
        CV_8UC3,
        cv::Scalar(10, 10, 10)
    );

    const cv::Mat frameB(
        2,
        2,
        CV_8UC3,
        cv::Scalar(20, 20, 20)
    );

    QCOMPARE(
        queue.push(frameA, 50),
        static_cast<uint32_t>(CHONGMING_OK)
    );

    QCOMPARE(
        queue.push(frameB, 50),
        static_cast<uint32_t>(CHONGMING_OK)
    );

    QVERIFY(queue.isFull());
    QVERIFY(queue.size() == 2);

    queue.clear();

    QVERIFY(queue.isEmpty());
    QVERIFY(!queue.isFull());
    QVERIFY(queue.size() == 0);

    QCOMPARE(
        queue.push(frameA, 50),
        static_cast<uint32_t>(CHONGMING_OK)
    );

    QCOMPARE(
        queue.push(frameB, 50),
        static_cast<uint32_t>(CHONGMING_OK)
    );

    QVERIFY(queue.isFull());
    QVERIFY(queue.size() == 2);
}

void CameraImageQueueTest::returnedImageDoesNotAliasPoolBuffer()
{
    CameraImageQueue queue(1);

    const cv::Mat firstFrame(
        2,
        2,
        CV_8UC3,
        cv::Scalar(10, 20, 30)
    );

    QCOMPARE(
        queue.push(firstFrame, 50),
        static_cast<uint32_t>(CHONGMING_OK)
    );

    cv::Mat firstOutput;

    QCOMPARE(
        queue.pop(firstOutput, 50),
        static_cast<uint32_t>(CHONGMING_OK)
    );

    const cv::Mat secondFrame(
        2,
        2,
        CV_8UC3,
        cv::Scalar(200, 210, 220)
    );

    QCOMPARE(
        queue.push(secondFrame, 50),
        static_cast<uint32_t>(CHONGMING_OK)
    );

    const cv::Vec3b firstPixel =
        firstOutput.at<cv::Vec3b>(0, 0);

    QCOMPARE(firstPixel[0], static_cast<uchar>(10));
    QCOMPARE(firstPixel[1], static_cast<uchar>(20));
    QCOMPARE(firstPixel[2], static_cast<uchar>(30));
}

void CameraImageQueueTest::clearWakesBlockedProducer()
{
    CameraImageQueue queue(1);

    const cv::Mat firstFrame(
        2,
        2,
        CV_8UC3,
        cv::Scalar(1, 1, 1)
    );

    const cv::Mat secondFrame(
        2,
        2,
        CV_8UC3,
        cv::Scalar(2, 2, 2)
    );

    QCOMPARE(
        queue.push(firstFrame, 50),
        static_cast<uint32_t>(CHONGMING_OK)
    );

    auto producerFuture =
        std::async(
            std::launch::async,
            [&queue, &secondFrame]()
            {
                return queue.push(secondFrame, 0);
            }
        );

    std::this_thread::sleep_for(
        std::chrono::milliseconds(50)
    );

    queue.clear();

    const auto status =
        producerFuture.wait_for(
            std::chrono::milliseconds(500)
        );

    if (status != std::future_status::ready)
    {
        queue.shutDown();
        QFAIL("clear() did not wake the blocked producer");
    }

    QCOMPARE(
        producerFuture.get(),
        static_cast<uint32_t>(CHONGMING_OK)
    );

    QVERIFY(queue.size() == 1);
    QVERIFY(queue.isFull());
}

void CameraImageQueueTest::shutdownWakesBlockedConsumer()
{
    CameraImageQueue queue(1);

    auto consumerFuture =
        std::async(
            std::launch::async,
            [&queue]()
            {
                cv::Mat output;
                return queue.pop(output, 0);
            }
        );

    std::this_thread::sleep_for(
        std::chrono::milliseconds(50)
    );

    queue.shutDown();

    const auto status =
        consumerFuture.wait_for(
            std::chrono::milliseconds(500)
        );

    if (status != std::future_status::ready)
    {
        QFAIL("shutDown() did not wake the blocked consumer");
    }

    QCOMPARE(
        consumerFuture.get(),
        static_cast<uint32_t>(POP_IMGAE_FAILED)
    );
}

QTEST_APPLESS_MAIN(CameraImageQueueTest)

#include "CameraImageQueueTest.moc"
