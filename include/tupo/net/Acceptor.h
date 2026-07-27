#pragma once
#include "tupo/net/EventLoop.h"
#include "tupo/net/Socket.h"
#include "tupo/net/Channel.h"
#include "tupo/net/InetAddress.h"
namespace Tupo {
namespace net {
class Acceptor {
public:
  Acceptor(EventLoop *loop,const InetAddress &addr);
  
  // 监听
  void listen() {
  }
  
private:
  // 处理新连接
  void handleRead() {
  
  }

  EventLoop *loop_;
  Socket acceptSocket_;
  Channel acceptChannel_;
  bool isListening;
};
} // namespace net
} // namespace Tupo