#include <gtest/gtest.h>
#include "tupo/net/TcpServer.h"
#include "tupo/net/EventLoop.h"
#include "tupo/net/InetAddress.h"
#include "tupo/net/TimerQueue.h"
#include "tupo/net/TimerId.h"
#include "tupo/net/TcpConnection.h"
#include <fcntl.h>
namespace Tupo {
namespace net {
class TcpServerTest : public ::testing::Test {
protected:
  void SetUp() override {
    loop_ = std::make_unique<Tupo::net::EventLoop>();
    server_ = std::make_unique<Tupo::net::TcpServer>(loop_.get(), Tupo::net::InetAddress(8080));

    // 创建双向管道
    int fds[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, fds);
    clientfd_ = fds[0];
    serverfd_ = fds[1];

    // 设置非阻塞
    int flags = fcntl(serverfd_, F_GETFL, 0);
    fcntl(serverfd_, F_SETFL, flags | O_NONBLOCK);

    // 创建TcpConnection
    Tupo::net::InetAddress localAddr(8080);
    Tupo::net::InetAddress peerAddr(8080);
    connection_ = std::make_shared<Tupo::net::TcpConnection>(clientfd_, loop_.get(), localAddr, peerAddr);
    
    connection_->setMessageCallback([this](const Tupo::net::TcpConnection::TcpConnectionPtr &conn, Tupo::net::Buffer &buffer) {
        receivedData_ = buffer.retrieveAllAsString();
    });

    connection_->connectEstablished();

  }

  void TearDown() override {
    // 每个测试后的清理
      close(serverfd_);
      close(clientfd_);
  }

    // 辅助函数：从客户端发送数据
  void sendData(const std::string& msg) {
     ssize_t n = write(serverfd_, msg.data(), msg.size());
     ASSERT_EQ(n, msg.size());
     callbackCalled_ = true;
  }
  int clientfd_;
  int serverfd_;
  std::string receivedData_;
  std::unique_ptr<Tupo::net::EventLoop> loop_;
  std::unique_ptr<Tupo::net::TcpServer> server_;
  std::shared_ptr<Tupo::net::TcpConnection> connection_;
   bool callbackCalled_ = false;
};

// 测试1：建立新的连接和关闭连接
TEST_F(TcpServerTest, NewConnectionTest) {
  loop_->runAfter(5.0, [this]() {
    std::cout << "5秒后关闭服务器" << std::endl;
    loop_->quit();
  });

  server_->start();
  printf("Server listening on : %s\n", server_->toIpPort().c_str()); 
  loop_->loop(); // 启动事件循环
}

// 测试2：模拟发送数据，并接收
TEST_F(TcpServerTest, DataTransmissionTest) {
   std::string testMsg = "Hello, TcpConnection!";
    
    // 1. 客户端发送数据
    sendData(testMsg);
    
    // 2. 手动触发读事件（模拟 EventLoop 调用）
    connection_->handleRead();
    
    // 3. 验证数据
    EXPECT_TRUE(callbackCalled_);
    EXPECT_EQ(receivedData_, testMsg);
    std::cout<<"length: "<<receivedData_.length()<<std::endl;
}
}
}