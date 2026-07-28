#include "tupo/net/Acceptor.h"
#include <iostream>
namespace Tupo {
namespace net {
Acceptor::Acceptor(EventLoop *loop,const InetAddress &addr) : loop_(loop),
    acceptSocket_(Socket::createNonblockingOrDie()),
    acceptChannel_(loop, acceptSocket_.fd()),
    isListening(false)
{
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.setReusePort(true);

    acceptSocket_.bind(addr.getSockAddr(), sizeof(addr.getSockAddr()));
    acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

void Acceptor::listen(){
  if(!isListening){
    acceptSocket_.listen();

    acceptChannel_.enableReading();

    isListening = true;
  }
}

void Acceptor::handleRead() {
  InetAddress peerAddr;
  int connfd = acceptSocket_.accept(&peerAddr);
  // 处理新连接的逻辑，例如创建一个新的连接对象，注册到事件
  if(connfd>=0){
    newConnectionCallback_(connfd, peerAddr);
  } else {
    // 处理accept错误
    std::cerr << "Acceptor::handleRead accept error: " << strerror(errno) << std::endl;
  }
}
} // namespace net
} // namespace Tupo