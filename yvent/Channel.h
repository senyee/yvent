#ifndef YVENT_CHANNET_H
#define YVENT_CHANNET_H

#include <vector>
#include <sys/epoll.h>
#include <functional>
#include "yvent/noncopyable.h"

namespace yvent
{
class EventLoop;
class Channel:public noncopyable
{
public:
    typedef std::function<void()> EventCallBack;
    Channel(EventLoop * loop_, int fd);
    ~Channel();

    int fd() const { return fd_; }
    int events() const { return events_; }
    int revents() const { return revents_; }
    void setRevent(int revent) { revents_ = revent; }
    bool isNoEvent() {return 0 == events_;}
    void handleEvents();

    int state() const {return state_;}
    void setState(int state) {state_ = state;}

    void enableRead() {events_ |= EPOLLIN | EPOLLPRI; update();}
    void enableWrite()  {events_ |= EPOLLOUT; update();}
    void disableWrite() {events_ &= ~EPOLLOUT; update();}
    void disableAll() {events_ = 0; update();}
    void update();

    void setReadCallback(const EventCallBack &cb) {readCallback_ = cb;}
    void setWriteCallback(const EventCallBack &cb) {writeCallback_ = cb;}
    void setCloseCallback(const EventCallBack &cb) {closeCallback_ = cb;}
    void setErrorCallback(const EventCallBack &cb) {errorCallback_ = cb;}

    bool isReading() const { return events_ & EPOLLIN; }
    bool isWriting() const { return events_ & EPOLLOUT; }
public:
    static const int kNew;
    static const int kAdded;
    static const int kDeleted;

private:
    EventLoop * loop_;
    int fd_;

private:
    int events_;
    int revents_;
    int state_;
    EventCallBack readCallback_;
    EventCallBack writeCallback_;
    EventCallBack closeCallback_;
    EventCallBack errorCallback_;
};

} // namespace yvent




#endif //YVENT_CHANNET_H