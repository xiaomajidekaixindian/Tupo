#include "tupo/base/Logger.h"
#include "tupo/net/EventLoopThreadPool.h"
#include <gtest/gtest.h>
class EventLoopThreadPoolTest : public ::testing::Test {
protected:
  void SetUp() override {}
  void TearDown() override {}
};

// 测试1：测试无线程池的情况下，getNextLoop() 返回 baseEventLoop
TEST_F(EventLoopThreadPoolTest, StartAndGetNextLoop) {
  Tupo::net::EventLoop baseLoop;
  Tupo::net::EventLoopThreadPool pool(&baseLoop);
  Tupo::net::EventLoop *loop = pool.getNextLoop();
  EXPECT_EQ(loop, &baseLoop);
}

// 测试2：测试有线程池的情况下，getNextLoop() 返回不同的 EventLoop 对象
TEST_F(EventLoopThreadPoolTest, StartWithThreadsAndGetNextLoop) {
  Tupo::net::EventLoop baseLoop;
  Tupo::net::EventLoopThreadPool pool(&baseLoop, 4);
  pool.start();
  for (int i = 0; i < 8; ++i) {
    Tupo::net::EventLoop *loop = pool.getNextLoop();
    loop->runInLoop(
        [loop]() { LOG_DEBUG << "Task running in EventLoop: " << loop; });
    EXPECT_NE(loop, &baseLoop);
    LOG_DEBUG << "Got EventLoop: " << loop;
  }
}