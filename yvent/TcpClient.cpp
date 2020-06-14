#include <cassert>
#include "TcpClient.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include "Logging.h"

using namespace yvent;

TcpClient::TcpClient(EventLoop* loop, const InetAddr server):
            loop_(loop),
            server_(server),
            connector_(new Connector(loop, server))
{
    connector_->setNewConnectionCallback([this](const int cfd, const InetAddr& local){
        this->newConnection(cfd, local);
    });
}

TcpClient::~TcpClient()
{
}

void TcpClient::start()
{
    assert(loop_->isInLoopThread());
    connector_->start();
}
void TcpClient::newConnection(const int cfd, const InetAddr& local)
{
    LOG_TRACE("TcpClient newConnection");
    assert(loop_->isInLoopThread());
    struct sockaddr_in peeraddr;
    bzero(&peeraddr, sizeof peeraddr);
    socklen_t addrlen = sizeof(peeraddr);
    if (::getpeername(cfd, reinterpret_cast<sockaddr*>(&peeraddr), &addrlen) < 0)
    {
        LOG_SYSERR("getpeername()");
    }
    InetAddr peer(peeraddr);
    char buf[32];
    snprintf(buf, sizeof buf, ":%s#%d", peer.ip().c_str(), peer.port());
    connection_ = std::make_shared<TcpConnection>(loop_, buf, cfd, local, peer);
    connection_->setReadCallback(messageCallback_);
    connection_->setWriteCompleteCallback(writeCompleteCallback_);
    connection_->setCloseCallback([this](const TcpConnectionPtr& conn){
        this->closeConnection(conn);
    });

    connection_->enableRead();
    if (connectionCallback_) connectionCallback_(connection_);
}

void TcpClient::closeConnection(const TcpConnectionPtr& conn)
{
    assert(loop_->isInLoopThread());
    assert(connection_ != nullptr);
    connection_.reset();
    if (connectionCallback_) connectionCallback_(conn);
}