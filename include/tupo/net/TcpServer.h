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
  using TcpConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
  TcpServer(EventLoop *loop, const InetAddress &addr);

  void start();

  std::string toIp()const;

  uint16_t toPort()const;

  std::string toIpPort()const;
private:
  void onNewConnection(int sockfd, const InetAddress &peerAddr);
  void removeConnection(const TcpConnectionPtr & conn);
  void removeConnectionInLoop(const TcpConnectionPtr & conn);
  bool isStart_;
  EventLoop *loop_;
  Acceptor acceptor_;
  InetAddress localAddr_;

  TcpConnectionCallback tcpConnectionCallback_;

  // 管理连接
  std::unordered_map<int, TcpConnectionPtr> connections_;
};
}
}