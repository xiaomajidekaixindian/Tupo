#include "tupo/net/TcpServer.h"
#include "tupo/net/EventLoop.h"
#include <iostream>

namespace Tupo {
namespace net {

TcpServer::TcpServer(EventLoop *loop, const InetAddress &listenAddr)
    : isStart_(false),loop_(loop), acceptor_(loop, listenAddr),localAddr_(listenAddr) {
        acceptor_.setNewConnectionCallback([this](int sockfd, const InetAddress &peerAddr) {
            this->onNewConnection(sockfd, peerAddr);
    });
}
void TcpServer::start(){
    if (!isStart_) {
        isStart_ = true;
        acceptor_.listen();
    }
}

std::string TcpServer::toIp() const{
    return localAddr_.toIp();
}

uint16_t TcpServer::toPort() const{
    return localAddr_.toPort();
}

std::string TcpServer::toIpPort() const{
    return localAddr_.toIpPort();
}

void TcpServer::onNewConnection(int sockfd, const InetAddress &peerAddr) {
    auto conn = std::make_shared<TcpConnection>(sockfd, loop_, localAddr_, peerAddr);
    std::cout<<"有新连接"<<std::endl;

    conn->setConnectionCallback(tcpConnectionCallback_);

    connections_[sockfd] = conn; 
}
} // namespace net
} // namespace Tupo