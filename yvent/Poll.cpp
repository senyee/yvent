#include <sys/epoll.h>
#include <unistd.h>
#include <errno.h>
#include <cassert>
#include "Poll.h"
#include "Logging.h"
#include "Channel.h"
#include "EventLoop.h"

using namespace yvent;


Poll::Poll(EventLoop* loop)
    :loop_(loop),
    events_(128),
    epfd_( epoll_create1(0) )
{
    LOG_TRACE("epfd_:%d",epfd_);
    if(-1 == epfd_) {
        LOG_SYSFATAL("epfd_ creat failed");
    }
}
Poll::~Poll()
{
    ::close(epfd_);
}

void Poll::poll(ChannelList& channelList)
{
    LOG_TRACE("poll...");
    int maxEvents = static_cast<int>(events_.size());
    int n = epoll_wait(epfd_, events_.data(), maxEvents, -1);
    LOG_TRACE("poll %d events come", n);
    if(-1 == n) {
        if (errno != EINTR) {
            LOG_SYSERR("Poll::EINTR");
        }
    } else if( 0 == n) {
        LOG_DEBUG("Poll timeout");
    } else {
        for (int i = 0; i < n; i++) {
            Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
            channel->setRevent(events_[i].events);
            LOG_DEBUG("events: %d -> %d", channel->fd(), channel->revents());
            channelList.push_back(channel);
        }
        if(n == maxEvents) {
            events_.resize(2 * events_.size());
        }
    }
}

void Poll::updateChannel(Channel* channel)
{
    assert(loop_->isInLoopThread());
    int op = 0;
    int state = channel->state();
    LOG_TRACE("fd:%d, state:%d", channel->fd(), state);
    if(state == Channel::kNew) {
        assert(!channel->isNoEvent());
        op = EPOLL_CTL_ADD;
        channel->setState(Channel::kAdded);
    } else if(channel->isNoEvent()) {
        op = EPOLL_CTL_DEL;
        channel->setState(Channel::kDeleted);
    } else {
        op = EPOLL_CTL_MOD;
    }
    update(op, channel);
}

void Poll::update(int op, Channel* channel)
{
    LOG_TRACE("op:%d,fd:%d", op, channel->fd());
    struct epoll_event ev;
    ev.events = channel->events();
    ev.data.ptr = channel;
    int ret = ::epoll_ctl(epfd_, op, channel->fd(), &ev);
    if(-1 == ret) {
        LOG_SYSERR("epoll_ctl err");
    }
}
