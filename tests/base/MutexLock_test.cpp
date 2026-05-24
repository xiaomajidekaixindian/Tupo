// MutexLock_test.cpp
#include "tupo/base/MutexLock.h"

#include <atomic>
#include <gtest/gtest.h>
#include <thread>
#include <vector>

class MutexLockTest : public testing::Test {
protected:
  void SetUp() override {
    // 每个测试前的设置
  }

  void TearDown() override {
    // 每个测试后的清理
  }

  Tupo::base::MutexLock mutex;
  int date = 0;
};

// 测试1：基本的加锁和解锁功能测试
TEST_F(MutexLockTest, BasicLockUnlock) {
  EXPECT_NO_THROW(mutex.lock());
  EXPECT_NO_THROW(mutex.unlock());
}

// 测试2：RAII包装器构造和析构
TEST_F(MutexLockTest_BasicLockUnlock_Test, MutexLockGuardConstruction) {
  EXPECT_NO_THROW({ Tupo::base::MutexLockGuard lock(mutex); });
}

// 测试3：测试多线程环境锁正常使用

TEST_F(MutexLockTest_BasicLockUnlock_Test, ThreadMutex) {}