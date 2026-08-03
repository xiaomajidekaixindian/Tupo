#include "tupo/net/TcpConnection.h"


namespace Tupo {
namespace net {
TcpConnection::TcpConnection(int connfd, EventLoop *loop,
     const InetAddress &localAddr, const InetAddress &peerAddr)
    : socket_(std::make_unique<Socket>(connfd)), loop_(loop), 
    localAddr_(localAddr), peerAddr_(peerAddr),
    channel_(std::make_unique<Channel>(loop,connfd)),
    state_(kConnecting),connfd_(connfd){
        channel_->setReadCallback([this]{
            this->handleRead();
        });
        channel_->setWriteCallback([this]{
            this->handleWrite();
        });
        channel_->setErrorCallback([this]{
            this->handleError();
        });
        channel_->setCloseCallback([this]{
            this->handleClose();
        });
    }

void TcpConnection::connectEstablished(){
    setState(kConnected);
    channel_->enableReading();

    if(tcpConnectionCallback_) {
        tcpConnectionCallback_(shared_from_this());
    }
}

void TcpConnection::connectDestroyed(){
    setState(kDisconnected);
    channel_->disableAll();
    if(tcpConnectionCallback_){
        tcpConnectionCallback_(shared_from_this());
    }
}

void TcpConnection::handleRead(){
      char buf[1024];
    int n = ::read(connfd_, buf, sizeof(buf));
    
    if (n > 0) {
        // 有数据，直接处理（或者通知用户）
        if (messageCallback_) {
            messageCallback_(shared_from_this());
        }
    } else if (n == 0) {
        handleClose();  // 走关闭流程
    } else {
        // 错误处理
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            handleError();
        }
    }
}

void TcpConnection::handleWrite(){}

void TcpConnection::handleError(){}

void TcpConnection::handleClose(){
    if(state_ == kConnected || state_ == kDisconnecting){
        setState(kDisconnected);
        channel_->disableAll();
        if(closeCallback_){
            closeCallback_(shared_from_this());
        }
    }
}

void TcpConnection::setState(State state){
    state_ = state;
}


}

}