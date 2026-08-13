#include "tupo/net/TcpConnection.h"
#include "tupo/net/EventLoop.h"
#include <iostream>

namespace Tupo {
namespace net {
TcpConnection::TcpConnection(int connfd, EventLoop *loop,
     const InetAddress &localAddr, const InetAddress &peerAddr)
    : socket_(std::make_unique<Socket>(connfd)), loop_(loop), 
    localAddr_(localAddr), peerAddr_(peerAddr),
    channel_(std::make_unique<Channel>(loop,connfd)),
    state_(kConnecting),connfd_(connfd),
    inputBuffer_(std::make_unique<Buffer>()),
    outputBuffer_(std::make_unique<Buffer>()) {
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

void TcpConnection::send(const std::string &msg){
    if(state_== kConnected){
        if(loop_->isInLoopThread()){
            sendInLoop(msg);
        }else{
            loop_->runInLoop([this,msg]{
                this->sendInLoop(msg);
            });
        }
    }
}

void TcpConnection::send(const char *data, size_t len){
    if(state_== kConnected){
        if(loop_->isInLoopThread()){
            sendInLoop(data,len);
        }else{
            loop_->runInLoop([this,data,len]{
                this->sendInLoop(data,len);
            });
        }
    }
}

void TcpConnection::send(Buffer &&buffer){
    if(state_== kConnected){
        if(loop_->isInLoopThread()){
            sendInLoop(buffer);
        }else{
            // 跨线程：移动到堆上，用 shared_ptr 管理
            auto data = std::make_shared<Buffer>(std::move(buffer));
            loop_->runInLoop([this,data]{
                this->sendInLoop(*data);
            });
        }
    }
}

void TcpConnection::sendInLoop(const std::string &msg){
    sendInLoop(msg.data(), msg.size());
}

void TcpConnection::sendInLoop(const char *data, size_t len){
    if(state_ == kDisconnected){
        std::cout<<"TcpConnection::sendInLoop - disconnected, give up writing"<<std::endl;
        return;
    }

    ssize_t writable = 0;
    size_t remaining = len;
    if(outputBuffer_->readableBytes() == 0){
        writable = ::write(connfd_, data, len);
        if(writable >= 0){
            remaining = len - writable;
            if(remaining == 0 && writeCompleteCallback_){
                loop_->runInLoop([this]{
                    writeCompleteCallback_(shared_from_this());
                });
            }
        } else {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                return;
            }
            writable = 0;
            remaining = len;
        }
    }

    if(remaining > 0){
        outputBuffer_->append(data + writable, remaining);
        if(!channel_->isWriting()){
            channel_->enableWriting();
        }
    }
}

void TcpConnection::sendInLoop(const Buffer &buffer){
     if(state_ == kDisconnected){
        std::cout<<"TcpConnection::sendInLoop - disconnected, give up writing"<<std::endl;
        return;
    }

    ssize_t writable = 0;
    size_t remaining = buffer.readableBytes();
    if(outputBuffer_->readableBytes() == 0){
        writable = ::write(connfd_, buffer.readablePtr(), buffer.readableBytes());
        if(writable >= 0){
            remaining = buffer.readableBytes() - writable;
            if(remaining == 0 && writeCompleteCallback_){
                loop_->runInLoop([this]{
                    writeCompleteCallback_(shared_from_this());
                });    
            }
        } else {
            // 真实出错
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                return;
        }
            writable = 0;
            remaining = buffer.readableBytes();
    }
}
    if(remaining > 0){
        outputBuffer_ -> append(buffer.readablePtr() + writable, remaining);
        if(!channel_->isWriting()){
            channel_->enableWriting();
        }
    }
}

void TcpConnection::shutdown(){
    if(state_ == kConnected){
        setState(kDisconnecting);
        loop_->runInLoop([this]{
            this->shutdownInLoop();
        });
    }
}

void TcpConnection::shutdownInLoop(){
    if (state_ == kDisconnecting) {
        if (outputBuffer_->readableBytes() == 0) {
            // 输出缓冲区为空，直接关闭写端
            socket_->shutdownWrite();
            // 不关闭 fd，等待对端关闭
        } else {
            // 还有数据要发送，等待 handleWrite 发送完再关闭
        }
    }
}

void TcpConnection::handleRead(){
    int savedErrno = 0;
    ssize_t n = inputBuffer_->readFd(connfd_, &savedErrno);
    
    if (n > 0) {
        // 有数据，直接处理（或者通知用户）
        if (messageCallback_) {
            messageCallback_(shared_from_this(),*inputBuffer_);
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

void TcpConnection::handleWrite(){
    if(channel_->isWriting()){
        ssize_t n = ::write(connfd_, outputBuffer_->readablePtr(), outputBuffer_->readableBytes());
        if(n > 0){
            outputBuffer_->retrieve(n);
            if(outputBuffer_->readableBytes() == 0){
                channel_->disableWriting();
                if(writeCompleteCallback_){
                    loop_->runInLoop([this]{
                        writeCompleteCallback_(shared_from_this());
                    });
                }
                 // 如果在关闭中，且数据已发送完，关闭连接
                if (state_ == kDisconnecting) {
                    shutdownInLoop();
                }
        }
    } else {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            handleError();
        }
    }
}
}

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