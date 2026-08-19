#pragma once
#include "tupo/net/Acceptor.h"
#include "tupo/net/InetAddress.h"
#include "tupo/net/TcpConnection.h"
#include <unordered_map>
namespace Tupo {
namespace net {
class EventLoop;
class Acceptor;
class TcpServer {
public:
  FRIEND_TEST(TcpServerTest, SendDataByString);
  FRIEND_TEST(TcpServerTest, AcceptConnectionTest);
  FRIEND_TEST(TcpServerTest, ServerMessageCallbackTest);
  FRIEND_TEST(TcpServerTest, CloseRemoveConnectionTest);

  using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
  using TcpConnectionCallback = std::function<void(const TcpConnectionPtr &)>;
  using MessageCallback =
      std::function<void(const TcpConnectionPtr &, Buffer &)>;
  using WriteCompleteCallback = std::function<void(const TcpConnectionPtr &)>;
  TcpServer(EventLoop *loop, const InetAddress &addr);

  void start();

  std::string toIp() const;

  uint16_t toPort() const;

  std::string toIpPort() const;

  // 用户用
  void setConnectionCallback(TcpConnectionCallback cb) {
    tcpConnectionCallback_ = std::move(cb);
  }
  void setMessageCallback(MessageCallback cb) {
    messageCallback_ = std::move(cb);
  }
  void setWriteCompleteCallback(WriteCompleteCallback cb) {
    writeCompleteCallback_ = std::move(cb);
  }

private:
  void onNewConnection(int sockfd, const InetAddress &peerAddr);
  void removeConnection(const TcpConnectionPtr &conn);
  void removeConnectionInLoop(const TcpConnectionPtr &conn);

  bool isStart_;
  EventLoop *loop_;
  Acceptor acceptor_;
  InetAddress localAddr_;

  TcpConnectionCallback tcpConnectionCallback_;
  MessageCallback messageCallback_;
  WriteCompleteCallback writeCompleteCallback_;
  // 管理连接
  std::unordered_map<int, TcpConnectionPtr> connections_;
};
} // namespace net
} // namespace Tupo