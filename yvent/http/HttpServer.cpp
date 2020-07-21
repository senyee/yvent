#include "HttpServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "yvent/Logging.h"
using namespace yvent;

HttpServer::HttpServer(EventLoop* loop, const InetAddr& listenAddr):
                        server_(loop, listenAddr)
{
    server_.setConnectionCallback([this](const TcpConnectionPtr& conn){
        this->onConnection(conn);
    });
    server_.setMessageCallback([this](const TcpConnectionPtr& conn, Buffer* buffer){
        this->onMessage(conn, buffer);
    });
}

void HttpServer::onConnection(const TcpConnectionPtr& conn)
{
    if (conn->connected()) {
        conn->setContext(HttpContext());
    }
}

void HttpServer::onMessage(const TcpConnectionPtr& conn, Buffer* buffer)
{
    LOG_TRACE("HttpServer::onMessage %s", conn->name().c_str());
    HttpContext* context = std::any_cast<HttpContext>(conn->getMutableContext());
    if (!context->parseRequest(buffer)) {
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }

    if (context->parseDone()) {
        onRequest(conn, context->request());
        context->reset();
    }
}

void HttpServer::onRequest(const TcpConnectionPtr& conn, const HttpRequest& req)
{
    LOG_TRACE("HttpServer::onRequest %s", conn->name().c_str());
    const std::string& connection = req.getHeader("Connection");
    bool close = connection == "close" ||
        (req.version() == HttpRequest::Http10 && connection != "Keep-Alive");
    HttpResponse response(close);
    if(httpCallback_)
        httpCallback_(req, &response);
    Buffer buf;
    response.appendToBuffer(&buf);
    conn->send(buf);
    if (response.closeConnection()) {
        conn->shutdown();
    }
}