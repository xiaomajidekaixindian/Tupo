#include "tupo/base/Timestamp.h"
#include "tupo/net/EventLoop.h"
#include "tupo/net/TimerId.h"
#include "tupo/net/TimerQueue.h"
#include <functional>
#include <gtest/gtest.h>
#include <memory>
#include <vector>
class TimerQueueTest : public ::testing::Test {
protected:
  void SetUp() override {
    loop_ = new Tupo::net::EventLoop(); 
    callbackCount = 0; // 初始化计数器
  }

  void TearDown() override {
    loop_->quit(); // 确保事件循环退出
    delete loop_;
  }

  void simpleCallback() { callbackCount++; }

  int callbackCount;
  Tupo::net::EventLoop *loop_;
};

//Test 1: 测试添加定时器
TEST_F(TimerQueueTest, AddTimer) {
  {
    {
    Tupo::base::Timestamp when = Tupo::base::Timestamp::now();
    Tupo::base::Timestamp when2(when.microSecondsSinceEpoch() + 1000000); // 1秒后
    Tupo::net::TimerQueue timers(loop_);
    Tupo::net::TimerId timerId =
      timers.addTimer([this]() { this->simpleCallback(); }, when2, 0.0);
    EXPECT_TRUE(timerId.valid());
    }
  }
}

// Test 2: 测试添加重复定时器 - 验证 TimerId 有效性
TEST_F(TimerQueueTest, AddRepeatingTimer_ValidTimerId) {
  Tupo::base::Timestamp when = Tupo::base::Timestamp::now();
  Tupo::base::Timestamp when2(when.microSecondsSinceEpoch() + 1000000); // 1秒后
  Tupo::net::TimerQueue timers(loop_);
  Tupo::net::TimerId timerId =
      timers.addTimer([this]() { this->simpleCallback(); }, when2, 1.0);
  // EXPECT_EQ(callbackCount, 1);
  EXPECT_TRUE(timerId.valid());
}

// Test 3：测试通过事件循环触发定时器
TEST_F(TimerQueueTest, TimerLooping) {
  Tupo::base::Timestamp when = Tupo::base::Timestamp::now();
  // Tupo::net::TimerQueue timers(loop_.get());
  //  Tupo::net::TimerId timerId =
  //      timers.addTimer([this]() { this->simpleCallback(); }, when, 1.0);
  Tupo::base::Timestamp when2(when.microSecondsSinceEpoch() + 1000000); // 1秒后
  Tupo::net::TimerId timerId =
      loop_->runAt(when2, [this]() { this->simpleCallback(); });

  
  EXPECT_TRUE(timerId.valid());
  // 运行事件循环一段时间，让定时器有机会触发
  loop_->runAfter(3, [this]() { this->loop_->quit(); }); // 5s后退出循环
  loop_->loop();                                           // 启动事件循环
  // 因为回调没有被执行
  EXPECT_EQ(callbackCount, 1);
  EXPECT_TRUE(loop_->isQuits());
}

// Test 4：测试runAfter延时定时器
TEST_F(TimerQueueTest, RunAfterTest) {
  Tupo::net::TimerId timerId =
      loop_->runAfter(1.0, [this]() { this->simpleCallback(); });
  EXPECT_TRUE(timerId.valid());
  loop_->runAfter(3, [this]() { this->loop_->quit(); }); // 5s后退出循环
  loop_->loop();                                           // 启动事件循环
  EXPECT_EQ(callbackCount, 1);
  EXPECT_TRUE(loop_->isQuits());
}

// Test 5：测试runEvery重复定时器
TEST_F(TimerQueueTest, RunEveryTest) {
  Tupo::net::TimerId timerId =
      loop_->runEvery(1.0, [this]() { this->simpleCallback(); });
  EXPECT_TRUE(timerId.valid());
  loop_->runAfter(5, [this]() { this->loop_->quit(); }); // 5s后退出循环
  loop_->loop();                                           // 启动事件循环
  EXPECT_GE(callbackCount, 4); // 至少触发4次（1s、2s、3s、4s）
  EXPECT_TRUE(loop_->isQuits());
  std::cout<<"callbackCount:"<<callbackCount<<std::endl;
}