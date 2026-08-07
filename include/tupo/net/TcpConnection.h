#pragma once
#include <gtest/gtest_prod.h>
#include <memory>
#include <functional>
#include "tupo/net/Socket.h"
#include "tupo/net/InetAddress.h"
#include "tupo/net/Channel.h"
#include "tupo/net/Buffer.h"

namespace Tupo {
namespace net {
class TcpServerTest;
class EventLoop;
class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
  FRIEND_TEST(TcpServerTest, DataTransmissionTest);

  using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
  using TcpConnectionCallback = std::function<void(const TcpConnectionPtr &)>;
  using MessageCallback = std::function<void(const TcpConnectionPtr &, Buffer &)>;
  using WriteCompleteCallback = std::function<void(const TcpConnectionPtr &)>;
  using CloseCallback = std::function<void(const TcpConnectionPtr &)>;

  // 连接状态
  enum State {
      kDisconnected,      // 已断开连接
      kConnecting,        // 正在连接
      kConnected,         // 已连接
      kDisconnecting       // 正在断开连接  
  };

  TcpConnection(int sockfd, EventLoop *loop ,const InetAddress &localAddr, const InetAddress &peerAddr);

  // 连接与销毁
  void connectEstablished();
  void connectDestroyed();

  void setConnectionCallback(TcpConnectionCallback cb) { tcpConnectionCallback_ = std::move(cb); }
  void setCloseCallback(CloseCallback cb) { closeCallback_ = std::move(cb); }
  void setMessageCallback(MessageCallback cb) { messageCallback_ = std::move(cb); }
  void setWriteCompleteCallback(WriteCompleteCallback cb) { writeCompleteCallback_ = std::move(cb); }


  // 返回当前连接文件描述符
  const int getFd(){ return channel_->fd(); };
  
  void send(const std::string &msg);
  void send(const char *data, size_t len);
  void send(const Buffer &buffer);

  // 主动关闭
  void shutdown();
  void shutdownInLoop();
private:
  void handleRead();
  void handleWrite(); 
  void handleError();
  void handleClose(); // 被动关闭

  void setState(State state);

  void sendInLoop(const std::string &msg);
  void sendInLoop(const char *data, size_t len);
  void sendInLoop(Buffer buffer);
  std::unique_ptr<Socket> socket_; 
  EventLoop *loop_;
  InetAddress localAddr_;
  InetAddress peerAddr_;
  std::unique_ptr<Channel> channel_;
  State state_;
  int connfd_;

  std::unique_ptr<Buffer> inputBuffer_;
  std::unique_ptr<Buffer> outputBuffer_;

  TcpConnectionCallback tcpConnectionCallback_;
  MessageCallback messageCallback_;
  WriteCompleteCallback writeCompleteCallback_;
  CloseCallback closeCallback_;
};
}
}