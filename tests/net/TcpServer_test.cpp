#include <gtest/gtest.h>
#include "tupo/net/TcpServer.h"
#include "tupo/net/EventLoop.h"
#include "tupo/net/InetAddress.h"
#include "tupo/net/TimerQueue.h"
#include "tupo/net/TimerId.h"
#include "tupo/net/TcpConnection.h"
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>

namespace Tupo{
namespace net{
class TcpServerTest : public ::testing::Test {
protected:
  void SetUp() override {
    loop_ = std::make_unique<Tupo::net::EventLoop>();
    server_ = std::make_unique<Tupo::net::TcpServer>(loop_.get(), Tupo::net::InetAddress(8080));
    clientFd_ = -1;
  }

  void TearDown() override {
    if (clientFd_ >= 0) {
      close(clientFd_);
      clientFd_ = -1;
    }
  }

  // 真实 TCP 客户端连接服务器监听端口，返回客户端 fd；失败返回 -100
  int connectToServer() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
      return -100;
    }
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(server_->toPort());
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
    if (connect(fd, reinterpret_cast<sockaddr *>(&serverAddr), sizeof(serverAddr)) != 0) {
      close(fd);
      return -100;
    }
    return fd;
  }

  std::unique_ptr<Tupo::net::EventLoop> loop_;
  std::unique_ptr<Tupo::net::TcpServer> server_;
  int clientFd_;
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
    int fds[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, fds);
    int clientFd = fds[0];
    int serverFd = fds[1];
    
    // 设置非阻塞
    int flags = fcntl(serverFd, F_GETFL, 0);
    fcntl(serverFd, F_SETFL, flags | O_NONBLOCK);
    
    // 创建 TcpConnection
    InetAddress localAddr(8080);
    InetAddress peerAddr(8080);
    auto conn = std::make_shared<TcpConnection>(
        serverFd, loop_.get(), localAddr, peerAddr
    );
    conn->connectEstablished();
    
    std::string receivedData;
    conn->setMessageCallback([this,&receivedData](const Tupo::net::TcpConnection::TcpConnectionPtr &conn, Tupo::net::Buffer &buffer) {
        receivedData = buffer.retrieveAllAsString();
    });
    
    // 发送数据
    std::string testMsg = "Hello, TcpConnection!";
    ssize_t n = write(clientFd, testMsg.data(), testMsg.size());
    ASSERT_EQ(n, testMsg.size());
    
    // 手动触发读事件
    conn->handleRead();
    
    // 验证
    EXPECT_EQ(receivedData, testMsg);
    
    close(clientFd);
    // serverFd 由 TcpConnection 内部的 Socket 管理，析构时自动关闭
}

// 测试3：通过 std::string 发送数据，并验证接收
TEST_F(TcpServerTest, SendDataByString){
  int fds[2];
  socketpair(AF_UNIX, SOCK_STREAM, 0, fds);
  int clientFd = fds[0];
  int serverFd = fds[1];

  // 设置非阻塞
  int flags = fcntl(serverFd, F_GETFL, 0);
  fcntl(serverFd, F_SETFL, flags | O_NONBLOCK);

  // 创建 TcpConnection
  InetAddress localAddr(8080);
  InetAddress peerAddr(8080);
  auto conn = std::make_shared<TcpConnection>(
      serverFd, loop_.get(), localAddr, peerAddr
  );
  conn->connectEstablished();

  // 使用 std::string 发送数据
  const std::string testMsg = "Hello, TcpConnection!";
  conn->send(testMsg);

  // 从对端读取数据并验证
  char buf[256] = {0};
  ssize_t n = read(clientFd, buf, sizeof(buf));
  ASSERT_GT(n, 0);
  std::string received(buf, n);
  EXPECT_EQ(received, testMsg);

  close(clientFd);
}

// 测试4：通过 char* 发送数据，并验证接收
TEST_F(TcpServerTest, SendDataByChar){
  int fds[2];
  socketpair(AF_UNIX, SOCK_STREAM, 0, fds);
  int clientFd = fds[0];
  int serverFd = fds[1];

  // 设置非阻塞
  int flags = fcntl(serverFd, F_GETFL, 0);
  fcntl(serverFd, F_SETFL, flags | O_NONBLOCK);

  // 创建 TcpConnection
  InetAddress localAddr(8080);
  InetAddress peerAddr(8080);
  auto conn = std::make_shared<TcpConnection>(
      serverFd, loop_.get(), localAddr, peerAddr
  );
  conn->connectEstablished();

  // 使用 char* 发送数据
  const char *testMsg = "Hello, char*!";
  size_t len = strlen(testMsg);
  conn->send(testMsg, len);

  // 从对端读取数据并验证
  char buf[256] = {0};
  ssize_t n = read(clientFd, buf, sizeof(buf));
  ASSERT_GT(n, 0);
  std::string received(buf, n);
  EXPECT_EQ(received, testMsg);
  EXPECT_EQ(received.size(), len);

  close(clientFd);
}

// 测试5：通过 Buffer 发送数据，并验证接收
TEST_F(TcpServerTest, SendDataByBuffer){
  int fds[2];
  socketpair(AF_UNIX, SOCK_STREAM, 0, fds);
  int clientFd = fds[0];
  int serverFd = fds[1];

  // 设置非阻塞
  int flags = fcntl(serverFd, F_GETFL, 0);
  fcntl(serverFd, F_SETFL, flags | O_NONBLOCK);

  // 创建 TcpConnection
  InetAddress localAddr(8080);
  InetAddress peerAddr(8080);
  auto conn = std::make_shared<TcpConnection>(
      serverFd, loop_.get(), localAddr, peerAddr
  );
  conn->connectEstablished();

  // 构造 Buffer 并发送数据
  const std::string testMsg = "Hello, Buffer!";
  Tupo::net::Buffer buffer;
  buffer.append(testMsg);
  conn->send(std::move(buffer));

  // 从对端读取数据并验证
  char buf[256] = {0};
  ssize_t n = read(clientFd, buf, sizeof(buf));
  ASSERT_GT(n, 0);
  std::string received(buf, n);
  EXPECT_EQ(received, testMsg);

  close(clientFd);
}

// 测试6：真实 TCP 客户端 connect 到监听端口，跑事件循环，
// 断言连接建立回调被触发、connections_.size() == 1（覆盖 onNewConnection 整条链路）
TEST_F(TcpServerTest, AcceptConnectionTest) {
  int connectionCallbackCalls = 0;
  int connectResult = -1;

  server_->setConnectionCallback([&](const Tupo::net::TcpConnection::TcpConnectionPtr &conn) {
    ++connectionCallbackCalls;
  });

  server_->start();

  loop_->runAfter(0.1, [&] {
    clientFd_ = connectToServer();
    connectResult = clientFd_ >= 0 ? 0 : -1;
  });
  loop_->runAfter(2.0, [&] { loop_->quit(); });  // 兜底超时

  loop_->loop();

  ASSERT_EQ(connectResult, 0);
  EXPECT_EQ(connectionCallbackCalls, 1);
  ASSERT_EQ(server_->connections_.size(), 1u);
}

// 测试7：真实连接发送数据，验证经过 server 装配的 messageCallback_ 收到数据
// （覆盖 TcpServer.cpp onNewConnection 中的 conn->setMessageCallback(messageCallback_)）
TEST_F(TcpServerTest, ServerMessageCallbackTest) {
  const std::string testMsg = "Hello from real TCP client!";
  std::string received;
  int connectResult = -1;
  ssize_t writeResult = -1;

  server_->messageCallback_ = [&](const Tupo::net::TcpConnection::TcpConnectionPtr &conn,
                                  Tupo::net::Buffer &buffer) {
    received = buffer.retrieveAllAsString();
    loop_->quit();
  };

  server_->start();

  loop_->runAfter(0.1, [&] {
    clientFd_ = connectToServer();
    connectResult = clientFd_ >= 0 ? 0 : -1;
    if (connectResult == 0) {
      writeResult = write(clientFd_, testMsg.data(), testMsg.size());
    }
  });
  loop_->runAfter(2.0, [&] { loop_->quit(); });  // 兜底超时

  loop_->loop();

  ASSERT_EQ(connectResult, 0);
  ASSERT_EQ(writeResult, static_cast<ssize_t>(testMsg.size()));
  EXPECT_EQ(received, testMsg);
}

// 测试8：客户端断开 → handleClose → closeCallback_ → removeConnection → removeConnectionInLoop，
// 断言 connections_ 变空（覆盖 TcpServer.cpp removeConnection/removeConnectionInLoop）
TEST_F(TcpServerTest, CloseRemoveConnectionTest) {
  int connectionCallbackCalls = 0;
  int connectResult = -1;

  server_->setConnectionCallback([&](const Tupo::net::TcpConnection::TcpConnectionPtr &conn) {
    ++connectionCallbackCalls;
    if (connectionCallbackCalls == 1) {
      // 连接建立完成：关闭客户端，触发服务器 handleClose 断开链路
      if (clientFd_ >= 0) {
        close(clientFd_);
        clientFd_ = -1;
      }
    } else if (connectionCallbackCalls == 2) {
      // connectDestroyed 触发，说明 removeConnectionInLoop 已执行完
      loop_->quit();
    }
  });

  server_->start();

  loop_->runAfter(0.1, [&] {
    clientFd_ = connectToServer();
    connectResult = clientFd_ >= 0 ? 0 : -1;
  });
  loop_->runAfter(2.0, [&] { loop_->quit(); });  // 兜底超时

  loop_->loop();

  ASSERT_EQ(connectResult, 0);
  // 连接建立 + 连接销毁各触发一次 connectionCallback
  EXPECT_EQ(connectionCallbackCalls, 2);
  EXPECT_EQ(server_->connections_.size(), 0u);
}
}
}