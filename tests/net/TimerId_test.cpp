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
    //timer_ = new Timer([]() {}, Timestamp::now(), 0.0);
    //timerId_ = TimerId(timer_, 100);
  }
  void TearDown() override {
    // 每个测试后的清理
    //delete timer_;
  }
  //Timer *timer_ = nullptr;
  //TimerId timerId_;
};

#if 0 

// 1.测试默认构造函数
TEST_F(TimerIdTest, TimerNonConstruct) {
  TimerId timerid;
  EXPECT_FALSE(timerid.valid());
  EXPECT_EQ(timerid.timer(), nullptr);
  EXPECT_EQ(timerid.sequence(), 0);
}

// 2.测试带参数构造函数
TEST_F(TimerIdTest, TimerWithParamsConstruct) {
  EXPECT_TRUE(timerId_.valid());
  EXPECT_EQ(timerId_.timer(), timer_);
  EXPECT_EQ(timerId_.sequence(), 100);
}

// 3.测试 valid() 方法
TEST_F(TimerIdTest, ValidMethod) {
  EXPECT_TRUE(timerId_.valid());
  TimerId invalidTimerId;
  EXPECT_FALSE(invalidTimerId.valid());
}

// 4.测试拷贝构造
TEST_F(TimerIdTest, CopyConstructor) {
  TimerId copyTimerId(timerId_);
  EXPECT_TRUE(copyTimerId.valid());
  EXPECT_EQ(copyTimerId.timer(), timer_);
  EXPECT_EQ(copyTimerId.sequence(), 100);
}

#endif