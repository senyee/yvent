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
          poll_(this)
{
    assert(t_Eventloop == nullptr);
    t_Eventloop = this;
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
