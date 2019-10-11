#include <cassert>
#include <sys/types.h> // pid_t
#include <unistd.h>	// syscall()
#include <syscall.h> // SYS_gettid

#include "Logging.h"
#include "EventLoop.h"
#include "Channel.h"

using namespace yvent;

namespace
{

thread_local EventLoop* t_Eventloop = nullptr;

pid_t gettid()
{
    return static_cast<pid_t>(::syscall(SYS_gettid));
}

}

EventLoop::EventLoop()
        : tid_(gettid()),
          quit_(false),
          poll_(this),
          wakeupfd_( ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC) ),
          runningTasks_(false),
          channel_(this, wakeupfd_)
{
    if(wakeupfd_ < 0) {
        LOG_TRACE("eventfd() err");
        abort();
    }
    assert(t_Eventloop == nullptr);
    t_Eventloop = this;

    channel_.setReadCallback([this](){this->handleRead();});
    channel_.enableRead();
}

EventLoop::~EventLoop()
{
    assert(t_Eventloop == this);
    t_Eventloop = nullptr;
}

bool EventLoop::isInLoopThread()
{
    return gettid() == tid_;
}

void EventLoop::loop()
{
    assert(isInLoopThread());
    LOG_TRACE("tid_:%d,looping",tid_);
    while(!quit_) {
        activeChannels_.clear();
        poll_.poll(activeChannels_);
        for (auto channel: activeChannels_) {
            channel->handleEvents();
        }
        runTasks();
    }
}

void EventLoop::updateChannel(Channel* channel)
{
    LOG_TRACE("fd:%d", channel->fd());
    poll_.updateChannel(channel);
}

void EventLoop::quit()
{
    quit_ = true;
}

void EventLoop::runInLoop(const Task &task)
{
    LOG_TRACE(" ");
    if(isInLoopThread()) {
        task();
    } else {
        addTask(task);
    }
}

void EventLoop::addTask(const Task &task)
{
    LOG_TRACE("addtask...");
    {
        std::lock_guard<std::mutex> guard(mutex_);
        tasks_.push_back(task);
    }
    if(!isInLoopThread() || runningTasks_) {
        wakeup();
    }
}

void EventLoop::wakeup()
{
    LOG_TRACE("wakeup...");
    uint64_t up = 1;
    ssize_t n = ::write(wakeupfd_, &up, sizeof(up));
    if(sizeof(up) != n) {
        LOG_SYSERR("write err:%ld", n);
    }
}

void EventLoop::runTasks()
{
    LOG_TRACE("runTasks...");
    std::vector<Task> tasks;
    runningTasks_ = true;
    {
        std::lock_guard<std::mutex> guard(mutex_);
        tasks_.swap(tasks);
    }
    for(auto &task : tasks) {
        task();
    }
    runningTasks_ = false;
}

void EventLoop::handleRead()
{
    uint64_t one;
    ssize_t n = ::read(wakeupfd_, &one, sizeof(one));
    LOG_TRACE("%ld", n);
    if(sizeof(one) != n) {
        LOG_SYSERR("read err:%ld", n);
    }
}


