#include "tupo/net/Acceptor.h"
#include <iostream>
namespace Tupo {
namespace net {
Acceptor::Acceptor(EventLoop *loop, const InetAddress &addr)
    : loop_(loop), acceptSocket_(Socket::createNonblockingOrDie()),
      acceptChannel_(loop, acceptSocket_.fd()), isListening(false),
      localAddr_(addr) {

  acceptSocket_.setReuseAddr(true);
  acceptSocket_.setReusePort(true);

  acceptSocket_.bind(addr.getSockAddr(), addr.getLength());
  acceptChannel_.setReadCallback([this] { this->handleRead(); });
}

void Acceptor::listen() {
  if (!isListening) {
    acceptSocket_.listen();

    printf("Server listening on : %s\n", localAddr_.toIpPort().c_str());

    acceptChannel_.enableReading();

    isListening = true;
  }
}

void Acceptor::handleRead() {
  InetAddress peerAddr;
  int connfd = acceptSocket_.accept(&peerAddr);
  LOG_DEBUG << "对端地址为：" << peerAddr.toIpPort();
  // 处理新连接的逻辑，例如创建一个新的连接对象，注册到事件
  if (connfd >= 0) {
    LOG_DEBUG << "有新连接到来！";
    newConnectionCallback_(connfd, peerAddr);
  } else {
    // 处理accept错误
    LOG_ERROR << "Acceptor::handleRead accept error: " << strerror(errno);
  }
}
} // namespace net
} // namespace Tupo