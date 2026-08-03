#pragma once

#include "tupo/net/Socket.h"
#include "tupo/net/InetAddress.h"
#include <memory>
#include <functional>
#include "tupo/net/Channel.h"

namespace Tupo {
namespace net {

class EventLoop;
class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
  using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
  using TcpConnectionCallback = std::function<void(const TcpConnectionPtr &)>;
  using MessageCallback = std::function<void(const TcpConnectionPtr &)>;
  using CloseCallback = std::function<void(const TcpConnectionPtr &)>;

  // 连接状态
  enum State {
      kDisconnected,
      kConnecting,
      kConnected,
      kDisconnecting
  };

  TcpConnection(int sockfd, EventLoop *loop ,const InetAddress &localAddr, const InetAddress &peerAddr);

  // 连接与销毁
  void connectEstablished();
  void connectDestroyed();

  void setConnectionCallback(TcpConnectionCallback cb) { tcpConnectionCallback_ = std::move(cb); }
  void setCloseCallback(CloseCallback cb) { closeCallback_ = std::move(cb); }

  // 返回当前连接文件描述符
  const int getFd(){ return channel_->fd(); };
private:
  void handleRead();
  void handleWrite();
  void handleError();
  void handleClose();

  void setState(State state);

  std::unique_ptr<Socket> socket_; 
  EventLoop *loop_;
  InetAddress localAddr_;
  InetAddress peerAddr_;
  std::unique_ptr<Channel> channel_;
  State state_;

  int connfd_;

  TcpConnectionCallback tcpConnectionCallback_;
  MessageCallback messageCallback_;
  CloseCallback closeCallback_;
};
}
}