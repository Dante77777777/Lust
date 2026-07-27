#include "CameraImageQueue.h"

CameraImageQueue::CameraImageQueue(size_t maxSize)
{
    if(maxSize <= 0)
    {
        maxSize = 10;
    }
    maxSize_ = maxSize;
    shutDownFlag_ = false;
    for (int i = 0; i < maxSize_; ++i) {
        freeQueue_.push(cv::Mat());
    }
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

size_t CameraImageQueue::maxSize()
{
    return maxSize_;
}

size_t CameraImageQueue::size()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return workQueue_.size();
}


void CameraImageQueue::clear()
{
    std::lock_guard<std::mutex> lock(mutex_);
    while(!workQueue_.empty())
    {
        workQueue_.pop();
    }
    notFullCondition_.notify_all();
}

bool CameraImageQueue::isEmpty()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return workQueue_.empty();
}

bool CameraImageQueue::isFull()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return workQueue_.size() >= maxSize_;
}

bool CameraImageQueue::waitForNotFull(std::unique_lock<std::mutex>& lock, int timeoutMS)
{
    if(shutDownFlag_)
    {
        return false;
    }
    if(workQueue_.size() < maxSize_)
    {
        return true;
    }
    if(timeoutMS <= 0)
    {
        notFullCondition_.wait(lock,[this](){
            return workQueue_.size() < maxSize_ || shutDownFlag_;
        });
        return !shutDownFlag_ && workQueue_.size() < maxSize_;
    }
    else
    {
        auto notRechTimeout =  notFullCondition_.wait_for(lock,std::chrono::milliseconds(timeoutMS),[this](){
            return workQueue_.size() < maxSize_ || shutDownFlag_;
        });
        return !shutDownFlag_ && notRechTimeout && workQueue_.size() < maxSize_;
    }
}

bool CameraImageQueue::waitForNotEmpty(std::unique_lock<std::mutex>& lock, int timeoutMS)
{
    if(shutDownFlag_)
    {
        return false;
    }
    if(!workQueue_.empty())
    {
        return true;
    }
    if(timeoutMS <= 0)
    {
        notEmptyCondition_.wait(lock,[this](){
            return shutDownFlag_ || !workQueue_.empty();
        });
        return !shutDownFlag_ && !workQueue_.empty();
    }
    else
    {
        auto notReachTimeout =  notEmptyCondition_.wait_for(lock,std::chrono::milliseconds(timeoutMS),[this](){
            return shutDownFlag_ || !workQueue_.empty();
        });
        return !shutDownFlag_ && notReachTimeout && !workQueue_.empty();
    }
}


uint32_t CameraImageQueue::push(const cv::Mat& image, int timeoutMS)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if(shutDownFlag_)
    {
        return PUSH_IMGAE_FAILED;
    }
    if(!waitForNotFull(lock,timeoutMS))
    {
        return PUSH_IMGAE_FAILED;
    }
    bool wasEmpty = workQueue_.empty();
    if(freeQueue_.size() != 0)
    {
        cv::Mat temp = freeQueue_.front();
        freeQueue_.pop();
        temp = image.clone();
        workQueue_.push(temp);
    }
    else
    {
        workQueue_.pop();
        workQueue_.push(image);
    }
    lock.unlock();
    if(wasEmpty)
    {
        notEmptyCondition_.notify_one();
    }
    return CHONGMING_OK;
}

uint32_t CameraImageQueue::pop(cv::Mat& image, int timeoutMS)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if(shutDownFlag_)
    {
        return POP_IMGAE_FAILED;
    }
    if(!waitForNotEmpty(lock,timeoutMS))
    {
        return POP_IMGAE_FAILED;
    }
    bool wasFull = (workQueue_.size() >= maxSize_);
    image = workQueue_.front();
    workQueue_.pop();
    freeQueue_.push(image);
    lock.unlock();
    if(wasFull)
    {
        notFullCondition_.notify_one();
    }
    return CHONGMING_OK;
}
