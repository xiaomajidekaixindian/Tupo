#include <gtest/gtest.h>
#include "tupo/net/TcpServer.h"
#include "tupo/net/EventLoop.h"
class TcpServerTest : public ::testing::Test {
protected:
  void SetUp() override {
    loop_ = std::make_unique<Tupo::net::EventLoop>();
    server_ = std::make_unique<Tupo::net::TcpServer>(loop_.get(), Tupo::net::InetAddress(8080));
  }

  void TearDown() override {
    // 每个测试后的清理
  }
  std::unique_ptr<Tupo::net::EventLoop> loop_;
  std::unique_ptr<Tupo::net::TcpServer> server_;
};

// 测试1：建立新的连接
TEST_F(TcpServerTest, NewConnectionTest) {
  server_->start();
  loop_->loop(); // 启动事件循环
}