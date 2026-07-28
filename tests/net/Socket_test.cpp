#include "tupo/net/Socket.h"
#include <gtest/gtest.h>

class SocketTest : public ::testing::Test {
  void SetUp() override {
    // 每个测试前的初始化
  }

  void TearDown() override {
    // 每个测试后的清理
  }
};

// 测试1：Socket 创建和基本功能
TEST_F(SocketTest, CreateAndClose) {
  int fd = Tupo::net::Socket::createNonblockingOrDie();
  EXPECT_GT(fd, 0);

  Tupo::net::Socket sock(fd);
  EXPECT_EQ(sock.fd(), fd);
}

// 测试2：Socket选项测试
TEST_F(SocketTest, SetSocketOptions) {
  int fd = Tupo::net::Socket::createNonblockingOrDie();
  Tupo::net::Socket sock(fd);

  // 测试各种选项设置（不报错即为成功）
  EXPECT_NO_THROW(sock.setReuseAddr(true));
  EXPECT_NO_THROW(sock.setReusePort(true));
  EXPECT_NO_THROW(sock.setTcpNoDelay(true));
  EXPECT_NO_THROW(sock.setKeepAlive(true));
}

// 测试3：Socket绑定和监听
TEST_F(SocketTest, BindAndListen) {
  int fd = Tupo::net::Socket::createNonblockingOrDie();
  Tupo::net::Socket sock(fd);
  Tupo::net::InetAddress addr(8080); // 绑定到8080
  EXPECT_NO_THROW(sock.bind(addr.getSockAddr(), sizeof(addr.getSockAddr())));
  EXPECT_NO_THROW(sock.listen());
}