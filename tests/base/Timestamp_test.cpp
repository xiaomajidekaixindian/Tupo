#include "tupo/base/Timestamp.h"
#include <chrono>
#include <gtest/gtest.h>
#include <thread>
// 测试用例
class TimestampTest : public ::testing::Test {
protected:
  void SetUp() override {
    // 测试前的设置代码
  }

  void TearDown() override {
    // 测试后的清理代码
  }
};

// 测试1：测试默认构造函数
TEST_F(TimestampTest, DefaultConstructor) {
  Tupo::base::Timestamp timestamp;
  EXPECT_EQ(timestamp.microSecondsSinceEpoch(), 0);
}

// 测试2：测试带参构造函数
TEST_F(TimestampTest, ParameterizedConstructor) {
  int64_t testValue = 123456789;
  Tupo::base::Timestamp ts(testValue);
  EXPECT_EQ(ts.microSecondsSinceEpoch(), testValue);
}

// 测试3： 测试 now() 方法
TEST_F(TimestampTest, NowMethod) {
  auto ts1 = Tupo::base::Timestamp::now();
  auto ts2 = Tupo::base::Timestamp::now();

  std::cout << "ts1的微秒值为:" << ts1.microSecondsSinceEpoch() << std::endl;
  std::cout << "ts2的微秒值为:" << ts2.microSecondsSinceEpoch() << std::endl;

  // 验证时间戳不为0
  EXPECT_GT(ts1.microSecondsSinceEpoch(), 0);
  EXPECT_GT(ts2.microSecondsSinceEpoch(), 0);

  // 验证第二个时间戳应该大于等于第一个（考虑到执行时间）
  EXPECT_LE(ts1.microSecondsSinceEpoch(), ts2.microSecondsSinceEpoch());
}

// 测试4：测试resetTime()方法

TEST_F(TimestampTest, resetTimeMethod) {
  auto ts = Tupo::base::Timestamp::now();
  double interval = 3.0;
  Tupo::base::Timestamp time = Tupo::base::Timestamp::resetTime(ts, interval);
  std::cout << "3秒后时间为:" << time.microSecondsSinceEpoch() << std::endl;
}