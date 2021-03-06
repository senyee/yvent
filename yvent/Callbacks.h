#ifndef YVENT_CALLBACKS_H
#define YVENT_CALLBACKS_H
#include <functional>
#include <memory>
#include "InetAddr.h"
namespace yvent
{

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;
using std::placeholders::_5;

class TcpConnection;
class Buffer;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void (const int cfd,const InetAddr& peer)> NewConnectionCallback;
typedef std::function<void()> ErrorCallback;
typedef std::function<void(const TcpConnectionPtr&, Buffer* buffer)> MessageCallback;
typedef std::function<void(const TcpConnectionPtr&)> CloseCallback;
typedef std::function<void(const TcpConnectionPtr&)> WriteCompleteCallback;
typedef std::function<void()> TimerCallback;
} // namespace yvent

#endif //YVENT_CALLBACKS_H