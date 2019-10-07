#include <sys/epoll.h>
#include <cassert>
#include "Channel.h"
#include "EventLoop.h"
#include "Logging.h"
using namespace yvent;

const int Channel::kNew = -1;
const int Channel::kAdded = 1;
const int Channel::kDeleted =2;

Channel::Channel(EventLoop* loop, int fd)
        :loop_(loop),
        fd_(fd),
        events_(0),
        revents_(0),
        state_(kNew)
{

}

Channel::~Channel()
{
    
}

void Channel::handleEvents()
{
    LOG_TRACE("revents_=%d",revents_);
    assert(loop_->isInLoopThread());
    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {

    }
    if (revents_ & EPOLLERR) {
        
    }
    if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
        if (readCallback_) readCallback_();
    }
    if (revents_ & EPOLLOUT) {
        if (writeCallback_) writeCallback_();
    }
}

void Channel::update()
{
    loop_->updateChannel(this);
}

