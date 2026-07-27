#include "tupo/net/Acceptor.h"

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
    isListening = true;
  }
}

void Acceptor::handleRead() {
  InetAddress peerAddr;
  acceptSocket_.accept(peerAddr.getSockAddr(), sizeof(peerAddr.getSockAddr()));
  // 处理新连接的逻辑，例如创建一个新的连接对象，注册到事件
  
}
} // namespace net
} // namespace Tupo