#include <cassert>
#include <sys/types.h> // pid_t
#include <unistd.h>	// syscall()
#include <syscall.h> // SYS_gettid

#include "Logging.h"
#include "EventLoop.h"

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
          quit_(false)
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
    LOG_TRACE("looping\n");
}