#pragma once
#include "tupo/net/EventLoop.h"
#include "tupo/net/Socket.h"
#include "tupo/net/Channel.h"
#include "tupo/net/InetAddress.h"
namespace Tupo {
namespace net {
class Acceptor {
public:
  // 新连接，传递给TcpServer，来创建TcpConnection对象
  using NewConnectionCallback = std::function<void(int sockfd, const InetAddress &)>;


  Acceptor(EventLoop *loop,const InetAddress &addr);
  
  // 监听
  void listen();
  
  void setNewConnectionCallback(NewConnectionCallback cb) {
     newConnectionCallback_ = std::move(cb);
  }

private:
  // 处理新连接
  void handleRead();

  EventLoop *loop_;
  Socket acceptSocket_;
  Channel acceptChannel_;
  InetAddress localAddr_;
  bool isListening;
  NewConnectionCallback newConnectionCallback_;
};
} // namespace net
} // namespace Tupo