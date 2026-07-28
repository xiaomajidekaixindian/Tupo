#pragma once
#include "tupo/net/EventLoop.h"
#include "tupo/net/Socket.h"
#include "tupo/net/InetAddress.h"

namespace Tupo {
namespace net {
class TcpConnection {
public:
  
  TcpConnection(int sockfd, EventLoop *loop ,const InetAddress &localAddr, const InetAddress &peerAddr);
private:
  Socket socket_; 
  EventLoop *loop_;
  InetAddress localAddr_;
  InetAddress peerAddr_;
};
}
}