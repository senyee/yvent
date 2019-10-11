#ifndef YVENT_EVENTLOOPTHREAD_H
#define YVENT_EVNETLOOPTHREAD_H
#include <functional>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "noncopyable.h"
namespace yvent
{
class EventLoop;
class EventLoopThread: public noncopyable
{
public:
    typedef std::function<void(EventLoop*)> ThreadInitCallback;
    EventLoopThread(const ThreadInitCallback &cb);
    ~EventLoopThread();

    EventLoop* starLoop();
private:
    void threadFunc();
private:
    EventLoop *loop_;
    ThreadInitCallback threadInitCallback_;
    std::mutex mutex_;
    std::condition_variable cond_;
    std::thread thread_;
    bool start_;
};

} // namespace yvent

#endif //YVENT_EVNETLOOPTHREAD_H