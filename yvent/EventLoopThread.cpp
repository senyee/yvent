#include <cassert>
#include "EventLoopThread.h"
#include "EventLoop.h"
using namespace yvent;

EventLoopThread::EventLoopThread(const ThreadInitCallback &cb)
        :loop_(nullptr),
        threadInitCallback_(cb),
        mutex_(),
        cond_(),
        start_(false)
{

}

EventLoopThread::~EventLoopThread()
{
    if(start_) {
        if(loop_) {
            loop_->quit();
        }
    thread_.join();
    }
}

EventLoop* EventLoopThread::starLoop()
{
    assert(false == start_);
    assert(nullptr == loop_);

    start_ = true;
    thread_ = std::thread([this](){this->threadFunc();});
    {
        std::unique_lock lock(mutex_);
        while(nullptr == loop_) {
            cond_.wait(lock);
        }
    }
    return loop_;
}

void EventLoopThread::threadFunc()
{
    assert(nullptr == loop_);
    EventLoop loop;
    loop_ = &loop;
    cond_.notify_all();
    loop.loop();
    loop_ = nullptr;
}