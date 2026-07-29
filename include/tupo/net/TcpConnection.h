#pragma once

#include "tupo/net/Socket.h"
#include "tupo/net/InetAddress.h"
#include <memory>
#include <functional>
namespace Tupo {
namespace net {

class EventLoop;
class TcpConnection {
public:
  using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
  using TcpConnectionCallback = std::function<void(const TcpConnectionPtr &)>;
  using CloseCallback = std::function<void(const TcpConnectionPtr &)>;

  TcpConnection(int sockfd, EventLoop *loop ,const InetAddress &localAddr, const InetAddress &peerAddr);

  void setConnectionCallback(TcpConnectionCallback cb) { tcpConnectionCallback_ = std::move(cb); }
  void setCloseCallback(CloseCallback cb) { closeCallback_ = std::move(cb); }
private:
  void handleRead();
  void handleWrite();
  void handleError();
  void handleClose();

  Socket socket_; 
  EventLoop *loop_;
  InetAddress localAddr_;
  InetAddress peerAddr_;

  TcpConnectionCallback tcpConnectionCallback_;
  CloseCallback closeCallback_;
};
}
}