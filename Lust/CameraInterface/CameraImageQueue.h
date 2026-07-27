#ifndef CAMERAIMAGEQUEUE
#define CAMERAIMAGEQUEUE

#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
#include <chrono>
#include "opencv2/opencv.hpp"
#include "CameraError.h"

class CameraImageQueue
{
public:
    explicit CameraImageQueue(size_t maxSize = 10);
    ~CameraImageQueue();

    CameraImageQueue(const CameraImageQueue&) = delete;
    CameraImageQueue& operator=(const CameraImageQueue&) = delete;

    CameraImageQueue(CameraImageQueue&& other) = delete;
    CameraImageQueue& operator=(CameraImageQueue&& other) = delete;

    uint32_t push(const cv::Mat& image, int timeoutMS);
    uint32_t pop(cv::Mat& image, int timeoutMs);

    bool isEmpty();
    bool isFull();

    void clear();
    size_t maxSize();
    size_t size();
    void shutDown();

private:
    mutable std::mutex mutex_;
    std::atomic<bool> shutDownFlag_;
    std::condition_variable notFullCondition_;
    std::condition_variable notEmptyCondition_;

    size_t maxSize_;
    std::queue<cv::Mat> freeQueue_;
    std::queue<cv::Mat> workQueue_;

    bool waitForNotFull(std::unique_lock<std::mutex>& lock, int timeoutMS);
    bool waitForNotEmpty(std::unique_lock<std::mutex>& lock, int timeoutMS);
};
































#endif
