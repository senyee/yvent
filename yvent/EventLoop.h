#ifndef YVENT_EVENTLOOP_H
#define YVENT_EVENTLOOP_H

#include <atomic>
#include <sys/eventfd.h>
#include <mutex>
#include <functional>
#include "noncopyable.h"
#include "Poll.h"
#include "Channel.h"
namespace yvent
{
class Poll;
class EventLoop:public noncopyable
{
public:
    typedef std::function<void()> Task;
    EventLoop();
    ~EventLoop();

    void loop();
    bool isInLoopThread();
    void updateChannel(Channel* channel);
    void quit();
    void runInLoop(const Task &task);
    void addTask(const Task &task);

public:
    void wakeup();
private:
    void runTasks();
    void handleRead();
private:
    const pid_t tid_;
    std::atomic_bool quit_;
    Poll poll_;

    Poll::ChannelList activeChannels_;
    int wakeupfd_;
    std::vector<Task> tasks_;
    std::mutex mutex_;
    std::atomic_bool runningTasks_;
    Channel channel_;
    bool looping_;
};

}//namespace yvent



#endif //YVENT_EVENTLOOP_H