#include "tupo/base/Timestamp.h"
#include "tupo/net/Timer.h"
#include "tupo/net/TimerId.h"
#include <gtest/gtest.h>
using namespace Tupo::base;
using namespace Tupo::net;

class TimerIdTest : public ::testing::Test {
protected:
  void SetUp() override {
    // 每个测试前的设置
    timer_ = new Timer([]() {}, Timestamp::now(), 0.0);
    timerId_ = TimerId(timer_, 100);
  }
  void TearDown() override {
    // 每个测试后的清理
    delete timer_;
  }
  Timer *timer_ = nullptr;
  TimerId timerId_;
};

// 1.测试默认构造函数
TEST_F(TimerIdTest, TimerIsReturn) { 
    TimerId timerid;
    EXPECT_FALSE(timerid.valid());
    EXPECT_EQ(timerid.timer(), nullptr);
    EXPECT_EQ(timerid.sequence(), 0);
 }