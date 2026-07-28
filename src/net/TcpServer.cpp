#include "tupo/net/TcpServer.h"

namespace Tupo {
namespace net {

TcpServer::TcpServer(EventLoop *loop, const InetAddress &addr)
    : loop_(loop), acceptor_(loop, addr) {
        acceptor_.setNewConnectionCallback([this](int sockfd, const InetAddress &peerAddr) {
            this->onNewConnection(sockfd, peerAddr);
        });
    }

void TcpServer::onNewConnection(int sockfd, const InetAddress &peerAddr) {
    // 创建TcpConnection对象
    InetAddress localAddr; 

}
} // namespace net
} // namespace Tupo