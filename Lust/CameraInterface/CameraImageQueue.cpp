#include "CameraImageQueue.h"

CameraImageQueue::CameraImageQueue(size_t maxSize)
    : maxSize_(maxSize == 0 ? 10 : maxSize)
{
    for (size_t i = 0; i < maxSize_; ++i) {
        freeQueue_.push(cv::Mat());
    }

    assert(invariantHoldsLocked());
}


CameraImageQueue::~CameraImageQueue()
{
    shutDown();
}

void CameraImageQueue::shutDown()
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        shutDownFlag_ = true;
    }
    notEmptyCondition_.notify_all();
    notFullCondition_.notify_all();
}

size_t CameraImageQueue::maxSize() const noexcept
{
    return maxSize_;
}

size_t CameraImageQueue::size() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return workQueue_.size();
}


void CameraImageQueue::clear()
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        while(!workQueue_.empty())
        {
            freeQueue_.push(std::move(workQueue_.front()));
            workQueue_.pop();
        }
        assert(invariantHoldsLocked());
    }

    notFullCondition_.notify_all();
}

bool CameraImageQueue::isEmpty() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return workQueue_.empty();
}

bool CameraImageQueue::isFull() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return freeQueue_.empty();
}

bool CameraImageQueue::waitForNotFull(std::unique_lock<std::mutex>& lock, int timeoutMs)
{
    if(shutDownFlag_)
    {
        return false;
    }
    if(!freeQueue_.empty())
    {
        return true;
    }
    if(timeoutMs <= 0)
    {
        notFullCondition_.wait(lock,[this](){
            return !freeQueue_.empty() || shutDownFlag_;
        });
        return !shutDownFlag_ && !freeQueue_.empty();
    }
    else
    {
        auto notRechTimeout =  notFullCondition_.wait_for(lock,std::chrono::milliseconds(timeoutMs),[this](){
            return !freeQueue_.empty() || shutDownFlag_;
        });
        return !shutDownFlag_ && notRechTimeout && !freeQueue_.empty();
    }
}

bool CameraImageQueue::waitForNotEmpty(std::unique_lock<std::mutex>& lock, int timeoutMs)
{
    if(shutDownFlag_)
    {
        return false;
    }
    if(!workQueue_.empty())
    {
        return true;
    }
    if(timeoutMs <= 0)
    {
        notEmptyCondition_.wait(lock,[this](){
            return shutDownFlag_ || !workQueue_.empty();
        });
        return !shutDownFlag_ && !workQueue_.empty();
    }
    else
    {
        auto notReachTimeout =  notEmptyCondition_.wait_for(lock,std::chrono::milliseconds(timeoutMs),[this](){
            return shutDownFlag_ || !workQueue_.empty();
        });
        return !shutDownFlag_ && notReachTimeout && !workQueue_.empty();
    }
}


uint32_t CameraImageQueue::push(const cv::Mat& image, int timeoutMs)
{
    if(image.empty())
    {
        return INVALID_INPUT;
    }
    std::unique_lock<std::mutex> lock(mutex_);
    if(shutDownFlag_)
    {
        return PUSH_IMGAE_FAILED;
    }
    if(!waitForNotFull(lock,timeoutMs))
    {
        return PUSH_IMGAE_FAILED;
    }
    if(freeQueue_.empty())
    {
        return PUSH_IMGAE_FAILED;
    }

    cv::Mat buffer = std::move(freeQueue_.front());
    freeQueue_.pop();
    image.copyTo(buffer);
    workQueue_.push(std::move(buffer));
    assert(invariantHoldsLocked());
    lock.unlock();
    notEmptyCondition_.notify_one();
    return CHONGMING_OK;
}

uint32_t CameraImageQueue::pop(cv::Mat& image, int timeoutMs)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if(shutDownFlag_)
    {
        return POP_IMGAE_FAILED;
    }
    if(!waitForNotEmpty(lock,timeoutMs))
    {
        return POP_IMGAE_FAILED;
    }
    if(workQueue_.empty())
    {
        return POP_IMGAE_FAILED;
    }

    cv::Mat buffer = std::move(workQueue_.front());
    workQueue_.pop();
    buffer.copyTo(image);
    freeQueue_.push(std::move(buffer));
    assert(invariantHoldsLocked());

    lock.unlock();
    notFullCondition_.notify_one();
    return CHONGMING_OK;
}


bool CameraImageQueue::invariantHoldsLocked() const noexcept
{
    return freeQueue_.size() + workQueue_.size() == maxSize_;
}
