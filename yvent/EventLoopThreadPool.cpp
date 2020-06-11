#include <cassert>
#include "EventLoopThreadPool.h"
#include "EventLoop.h"
#include "EventLoopThread.h"

using namespace yvent;

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop):
                    baseLoop_(baseLoop),
                    started_(false),
                    threadNum_(0),
                    nextEventLoop_(0)
{
}

EventLoopThreadPool::~EventLoopThreadPool()
{

}

void EventLoopThreadPool::start()
{
    assert(!started_);
    assert(baseLoop_->isInLoopThread());

    started_ = true;
    
    for(int i = 0; i < threadNum_; i++) {
        
        EventLoopThread* eventThread = new EventLoopThread(nullptr);
        loops_.push_back(eventThread->starLoop());
    }
}

EventLoop* EventLoopThreadPool::getNextEventLoop()
{
    assert(baseLoop_->isInLoopThread());
    EventLoop *loop = baseLoop_;

    if (!loops_.empty()) {
        loop = loops_[nextEventLoop_++];
        nextEventLoop_ %= static_cast<int>(loops_.size());
    }
    return loop;
}