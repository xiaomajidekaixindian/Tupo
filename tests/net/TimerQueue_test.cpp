#include "tupo/base/Timestamp.h"
#include "tupo/net/EventLoop.h"
#include "tupo/net/TimerId.h"
#include "tupo/net/TimerQueue.h"
#include <functional>
#include <gtest/gtest.h>
#include <vector>
class TimerQueueTest : public ::testing::Test {
protected:
  void SetUp() override {
    loop_ = std::make_unique<Tupo::net::EventLoop>();
    callbackCount = 0; // 初始化计数器
  }

  void TearDown() override {}

  void simpleCallback() { callbackCount++; }

  int callbackCount;
  std::unique_ptr<Tupo::net::EventLoop> loop_;
};

// Test 1: 测试添加定时器
TEST_F(TimerQueueTest, AddTimer) {
  Tupo::base::Timestamp when = Tupo::base::Timestamp::now();
  Tupo::net::TimerQueue timers(loop_.get());
  Tupo::net::TimerId timerId =
      timers.addTimer([this]() { this->simpleCallback(); }, when, 0.0);
  // EXPECT_EQ(callbackCount, 1);
  EXPECT_TRUE(timerId.valid());
}

// Test 2: 测试添加重复定时器 - 验证 TimerId 有效性
TEST_F(TimerQueueTest, AddRepeatingTimer_ValidTimerId) {
  Tupo::base::Timestamp when = Tupo::base::Timestamp::now();
  Tupo::net::TimerQueue timers(loop_.get());
  Tupo::net::TimerId timerId =
      timers.addTimer([this]() { this->simpleCallback(); }, when, 1.0);
  // EXPECT_EQ(callbackCount, 1);
  EXPECT_TRUE(timerId.valid());
}

// Test 3：测试通过事件循环触发定时器
TEST_F(TimerQueueTest, TimerLooping) {
  Tupo::base::Timestamp when = Tupo::base::Timestamp::now();
  // Tupo::net::TimerQueue timers(loop_.get());
  //  Tupo::net::TimerId timerId =
  //      timers.addTimer([this]() { this->simpleCallback(); }, when, 1.0);
  Tupo::net::TimerId timerId =
      loop_->runAt(when, [this]() { this->simpleCallback(); });

  EXPECT_TRUE(timerId.valid());
  // 运行事件循环一段时间，让定时器有机会触发
  loop_->runAfter(0.2, [this]() { this->loop_->quit(); }); // 200ms后退出循环
  loop_->loop();                                           // 启动事件循环
  // 因为回调没有被执行
  EXPECT_EQ(callbackCount, 1);
}

// Test 4：测试EventLoop定时器功能
TEST_F(TimerQueueTest, TimerLoop) {}
