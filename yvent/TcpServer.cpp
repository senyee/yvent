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
    std::shared_ptr<TcpConnection> conn(new TcpConnection(loop_, cfd, host_, peer) );
    conn->setReadCallback(messageCallback_);
    conn->enableRead();
    connSet_.insert(conn);
}
