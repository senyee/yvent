#include <stdio.h>
#include <set>
#include <iostream>
#include "yvent/EventLoop.h"
#include "yvent/TcpServer.h"

using namespace yvent;

class ChatServer
{
public:
    ChatServer(EventLoop* loop, const InetAddr& server);
    ~ChatServer();
    void start()
    {
        server_.start();
    }
private:
    void onConnection(const TcpConnectionPtr& conn)
    {
        
        if (conn->connected()) {
            std::cout << conn->name() << ":online" << std::endl;
            connections_.insert(conn);
        } else {
            std::cout << conn->name() << ":offline" << std::endl;
            connections_.erase(conn);
        }
        std::cout << "users:" << connections_.size() << std::endl;
    }
    void onMessage(const TcpConnectionPtr& conn, Buffer* buffer)
    {
        std::string message = buffer->retrieveAllAsString();
        for (auto& c : connections_) {
            //std::cout << c->name() << ":" << buffer->retrieveAllAsString() << std::endl;
            if( conn != c) {
                std::cout << c->name() << " send\n";
                c->send(message);
            }
        }
    }
private:
    typedef std::set<TcpConnectionPtr> ConnectionList;
    TcpServer server_;
    ConnectionList connections_;
};

ChatServer::ChatServer(EventLoop* loop, const InetAddr& server):
            server_(loop, server)
{
    server_.setMessageCallback([this](const TcpConnectionPtr& conn, Buffer* buffer){
        this->onMessage(conn, buffer);
    });
    server_.setConnectionCallback([this](const TcpConnectionPtr& conn){
        this->onConnection(conn);
    });
}

ChatServer::~ChatServer()
{
}

int main(int argc, char* argv[])
{
    EventLoop loop;
    InetAddr server(1234);
    ChatServer chatServer(&loop, server);
    chatServer.start();
    loop.loop();

    return 0;
}