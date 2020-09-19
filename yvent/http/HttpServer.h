#ifndef YVENT_HTTPSERVER_H
#define YVENT_HTTPSERVER_H

#include "HttpServer.h"
#include "yvent/TcpServer.h"
#include "HttpContext.h"
namespace yvent
{

class HttpRequest;
class HttpResponse;

class HttpServer
{
public:
    typedef std::function<void (const TcpConnectionPtr& conn, const HttpRequest&, HttpResponse*)> HttpCallback;
    HttpServer(EventLoop* loop, const InetAddr& listenAddr);

    void setHttpCallback(const HttpCallback& cb)
    {
        httpCallback_ = cb;
    }

    void setThreadNum(int numThreads)
    {
        server_.setThreadNum(numThreads);
    }

    void start()
    {
        server_.start();
    }
private:
    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn, Buffer* buffer);
    void onRequest(const TcpConnectionPtr&, const HttpRequest&);
    TcpServer server_;
    HttpCallback httpCallback_;
};



} // namespace yvent


#endif //YVENT_HTTPSERVER_H