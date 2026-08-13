#include "tupo/net/EventLoop.h"
#include <gtest/gtest.h>
#include <sys/timerfd.h>

class EventLoopTest : public testing::Test {
protected:
  // 提供自定义的构造函数
  EventLoopTest() : thread_([this] { loop_.loop(); }, "TestThread") {}
  void SetUp() override {
    // 每个测试前的设置
    // 启动线程运行事件循环
    thread_.start();
  }

  void TearDown() override {
    // 每个测试后的清理
    // 停止事件循环并结束线程
    loop_.quit();
    thread_.join();
  }
  Tupo::net::EventLoop loop_;
  Tupo::base::Thread thread_;
};

// 测试1：检查事件是否在同一个线程

TEST_F(EventLoopTest, isInLoopThread) { EXPECT_TRUE(loop_.isInLoopThread()); }

Tupo::net::EventLoop *g_loop;

void timeout() {
  std::cout << "Timeout!" << std::endl;
  g_loop->quit();
}

int main() {
  Tupo::net::EventLoop loop;
  g_loop = &loop;

  int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  {
    // Channel 不拥有 fd，必须在 Channel 析构（从 poller 移除）之后再关闭 fd，
    // 否则 fd 已关闭但仍在 epoll 中注册，存在 fd 被复用后误触发事件的风险
    Tupo::net::Channel channel(&loop, timerfd);
    channel.setReadCallback(timeout);
    channel.enableReading();

    struct itimerspec howlong;
    memset(&howlong, 0, sizeof(howlong));
    howlong.it_value.tv_sec = 5;

    timerfd_settime(timerfd, 0, &howlong, NULL);

    loop.loop();
  }
  ::close(timerfd);
}
