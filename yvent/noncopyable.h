#ifndef YVENT_NONCOPYABLE_H
#define YVENT_NONCOPYABLE_H

namespace yvent
{

class noncopyable
{
public:
    noncopyable(noncopyable &) = delete;
    void operator = (const noncopyable&) = delete;
protected:
    noncopyable() = default;
    ~noncopyable() = default;
};

}

#endif //YVENT_NONCOPYABLE_H