#include "tupo/base/MutexLock.h"
#include "tupo/base/Thread.h"
#include <gtest/gtest.h>
class ThreadTest : public testing::Test {
protected:
  void SetUp() override {
    // 每个测试前的设置
  }

  void TearDown() override {
    // 每个测试后的清理
  }
  int date = 0;
  Tupo::base::MutexLock mutex_lock;
};

// 测试1：基本的线程创建和执行测试
TEST_F(ThreadTest, BasicThreadExecution) {
  Tupo::base::Thread thread([this]() {
    Tupo::base::MutexLockGuard lock(mutex_lock);
    date = 42; // 受保护的操作
  });
  thread.start();
  thread.join();
  EXPECT_EQ(date, 42);
}