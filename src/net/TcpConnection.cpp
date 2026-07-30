#include "tupo/net/TcpConnection.h"
#include "tupo/net/Channel.h"

namespace Tupo {
namespace net {
TcpConnection::TcpConnection(int sockfd, EventLoop *loop,
     const InetAddress &localAddr, const InetAddress &peerAddr)
    : socket_(std::make_unique<Socket>(sockfd)), loop_(loop), 
    localAddr_(localAddr), peerAddr_(peerAddr),
    channel_(std::make_unique<Channel>(loop,sockfd)),
    state_(kConnecting){
        
    }

void TcpConnection::connectEstablished(){
    setState(kConnected);
    channel_->enableReading();

    tcpConnectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed(){}

void TcpConnection::handleRead(){}

void TcpConnection::handleWrite(){}

void TcpConnection::handleError(){}

void TcpConnection::handleClose() {}

void TcpConnection::setState(State state){

}


}

}