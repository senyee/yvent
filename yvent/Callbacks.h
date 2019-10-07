#ifndef YVENT_CALLBACKS_H
#define YVENT_CALLBACKS_H
#include <functional>
namespace yvent
{

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;
using std::placeholders::_5;

typedef std::function<void(char *buf, int len)> MessageCallback;
} // namespace yvent

#endif //YVENT_CALLBACKS_H