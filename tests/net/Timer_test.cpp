#include "tupo/base/Timestamp.h"
#include "tupo/net/Timer.h"
#include <gtest/gtest.h>
#include <thread>

using namespace Tupo::net;
using namespace Tupo::base;

class TimerTest : public ::testing::Test {
protected:
  void SetUp() override {
    // 每个测试前的设置
  }

  void TearDown() override {
    // 每个测试后的清理
  }
};

// 测试1：测试一次性定时器
TEST_F(TimerTest, OneTimeTimer) {
  int callCount = 0;
  auto callback = [&callCount]() { callCount++; };

  Timestamp start = Timestamp::now();
  Timer timer(callback, start, 0.0); // interval = 0, 非重复定时器
  // 验证定时器属性
  EXPECT_FALSE(timer.repeat());
  EXPECT_EQ(timer.expiration().microSecondsSinceEpoch(),
            start.microSecondsSinceEpoch());
  // 执行回调
  timer.run();
  EXPECT_EQ(callCount, 1);

  // 再次执行应该不会增加计数（根据设计决定）
  timer.run();
  // 这里取决于你的设计意图，如果允许重复执行就是2，否则就是1
  // EXPECT_EQ(callCount, 1); 或 EXPECT_EQ(callCount, 2);
}

// 测试2：测试重复定时器
TEST_F(TimerTest, RepeatTimer) {
  int callCount = 0;
  auto callback = [&callCount]() { callCount++; };

  Timestamp start = Timestamp::now();
  double interval = 1.0; // 1s间隔
  Timer timer(callback, start, interval);
  // 验证定时器属性
  EXPECT_TRUE(timer.repeat());
  EXPECT_DOUBLE_EQ(timer.interval(), interval);
  EXPECT_EQ(timer.expiration().microSecondsSinceEpoch(),
            start.microSecondsSinceEpoch());
  // 执行回调
  timer.run();
  EXPECT_EQ(callCount, 1);
}

// 测试3：测试定时器重置功能
TEST_F(TimerTest, TimerReset) {
  int callCount = 0;
  auto callback = [&callCount]() { callCount++; };

  Timestamp start = Timestamp::now();
  double interval = 2.0; // 2秒间隔
  Timer timer(callback, start, interval);

  //  重置定时器
  Timestamp resetTime = Timestamp::now();
  timer.restart(resetTime);
  // 验证重置后的到期时间
  Timestamp expectedExpiration = Timestamp::resetTime(resetTime, interval);
  EXPECT_EQ(timer.expiration().microSecondsSinceEpoch(),
            expectedExpiration.microSecondsSinceEpoch());
}
