#pragma once
#include "tupo/net/EventLoop.h"

namespace Tupo {
namespace net {
class Acceptor {
public:
  Acceptor(EventLoop *loop);
  
  void listen() {
    // 在这里可以实现监听逻辑，例如创建套接字、绑定地址、开始监听等
  }
  
private:
  void handleRead() {
    // 在这里可以实现处理新连接的逻辑，例如接受连接、创建新的Channel等
  }
  EventLoop *loop_;
};
} // namespace net
} // namespace Tupo