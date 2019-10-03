#ifndef YVENT_EVENTLOOP_H
#define YVENT_EVENTLOOP_H

#include <atomic>

namespace yvent
{

class EventLoop
{
public:
    EventLoop();
    ~EventLoop();

    void loop();
    bool isInLoopThread();
private:
    const pid_t tid_;
    std::atomic_bool quit_;
};

}//yvent



#endif //YVENT_EVENTLOOP_H