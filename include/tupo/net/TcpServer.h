#pragma once
#include "tupo/net/InetAddress.h"
#include "tupo/net/TcpConnection.h"
#include "tupo/net/Acceptor.h"
#include <unordered_map>
namespace Tupo {
namespace net {
class EventLoop;
class Acceptor;
class TcpServer {
public:
  using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

  TcpServer(EventLoop *loop, const InetAddress &addr);

  void start();
private:
  void onNewConnection(int sockfd, const InetAddress &peerAddr);
  
  bool isStart_;
  EventLoop *loop_;
  Acceptor acceptor_;
  InetAddress localAddr_;
  // 管理连接
  std::unordered_map<int, TcpConnectionPtr> connections_;
};
}
}