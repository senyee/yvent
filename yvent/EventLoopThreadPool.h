#ifndef YVENT_EVENTLOOPTHREADPOOL_H
#define YVENT_EVENTLOOPTHREADPOOL_H

#include <vector>
#include "noncopyable.h"
namespace yvent
{

class EventLoop;

class EventLoopThreadPool:public noncopyable
{
public:
    EventLoopThreadPool(EventLoop* baseLoop);
    ~EventLoopThreadPool();
    void setThreadNum(int threadNum) { threadNum_ = threadNum; }
    void start();
    EventLoop* getNextEventLoop();
private:
    EventLoop* baseLoop_;
    bool started_;
    int threadNum_;
    int nextEventLoop_;
    std::vector<EventLoop *> loops_;
};

}//yvent

#endif //YVENT_EVENTLOOPTHREADPOOL_H