#include <functional>
#include <memory>
#include "TcpServer.h"
#include "InetAddr.h"
#include "Logging.h"
#include "Callbacks.h"
#include "TcpConnection.h"
using namespace yvent;

TcpServer::TcpServer(EventLoop *loop, const InetAddr &host)
        :loop_(loop),
        host_(host),
        nextConnId_(0),
        acceptor_(loop, host)
{
    acceptor_.setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::~TcpServer()
{
}

void TcpServer::start()
{
    acceptor_.listen();
}

void TcpServer::newConnection(int cfd, const InetAddr &peer)
{
    LOG_TRACE("cfd=%d", cfd);
    char namebuf[128] = {0};
    snprintf(namebuf, 128, "%s:%d#%d",peer.ip().c_str(),peer.port(),nextConnId_);
    LOG_TRACE("newConnection:%s", namebuf);
    ++nextConnId_;
    std::string name(namebuf);
    std::shared_ptr<TcpConnection> conn(new TcpConnection(loop_, name, cfd, host_, peer) );
    conn->setReadCallback(messageCallback_);
    conn->setCloseCallback([this](const TcpConnectionPtr &tcpConnectionPtr){this->handleClose(tcpConnectionPtr);});
    conn->enableRead();
    connections_[name] = conn;
}

void TcpServer::handleClose(const TcpConnectionPtr &tcpConnectionPtr)
{
    LOG_INFO("close %s",tcpConnectionPtr->name().c_str());
    connections_.erase(tcpConnectionPtr->name());
}
