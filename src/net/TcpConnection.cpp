#include "tupo/net/TcpConnection.h"

namespace Tupo {
namespace net {
TcpConnection::TcpConnection(int sockfd, EventLoop *loop,
     const InetAddress &localAddr, const InetAddress &peerAddr)
    : socket_(sockfd), loop_(loop), 
    localAddr_(localAddr), peerAddr_(peerAddr) {
        
    }

void TcpConnection::handleRead(){}

void TcpConnection::handleWrite(){}

void TcpConnection::handleError(){}

void TcpConnection::handleClose() {}


}

}