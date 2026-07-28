#pragma once
#include "tupo/net/Acceptor.h"
#include "tupo/net/EventLoop.h"
#include "tupo/net/InetAddress.h"
#include "tupo/net/TcpConnection.h"

namespace Tupo {
namespace net {
class TcpServer {
public:
  TcpServer(EventLoop *loop, const InetAddress &addr);

private:
  void onNewConnection(int sockfd, const InetAddress &peerAddr);
  EventLoop *loop_;
  Acceptor acceptor_;

};
}
}