#include "tupo/net/TcpServer.h"
#include "tupo/net/EventLoop.h"
#include <iostream>

namespace Tupo {
namespace net {

TcpServer::TcpServer(EventLoop *loop, const InetAddress &listenAddr)
    : isStart_(false), loop_(loop), acceptor_(loop, listenAddr),
      localAddr_(listenAddr) {
  acceptor_.setNewConnectionCallback(
      [this](int connfd, const InetAddress &peerAddr) {
        this->onNewConnection(connfd, peerAddr);
      });
}
void TcpServer::start() {
  if (!isStart_) {
    isStart_ = true;
    acceptor_.listen();
  }
}

std::string TcpServer::toIp() const { return localAddr_.toIp(); }

uint16_t TcpServer::toPort() const { return localAddr_.toPort(); }

std::string TcpServer::toIpPort() const { return localAddr_.toIpPort(); }

void TcpServer::onNewConnection(int connfd, const InetAddress &peerAddr) {
  auto conn =
      std::make_shared<TcpConnection>(connfd, loop_, localAddr_, peerAddr);

  conn->setConnectionCallback(tcpConnectionCallback_);
  conn->setMessageCallback(messageCallback_);
  conn->setWriteCompleteCallback(writeCompleteCallback_);
  conn->setCloseCallback(
      [this](const TcpConnectionPtr &conn) { removeConnection(conn); });
  connections_[connfd] = conn;

  // 启动连接
  conn->connectEstablished();
}

void TcpServer::removeConnection(const TcpConnectionPtr &conn) {
  loop_->runInLoop([this, conn] { removeConnectionInLoop(conn); });
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn) {
  size_t n = connections_.erase(conn->getFd());
  if (n > 0) {
    conn->connectDestroyed();
  }
}

} // namespace net
} // namespace Tupo