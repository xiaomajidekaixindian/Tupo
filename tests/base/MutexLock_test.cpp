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

  Tupo::base::MutexLock mutex_lock;
  int date = 0;
};

// 测试1：基本的加锁和解锁功能测试
TEST_F(MutexLockTest, BasicLockUnlock) {
  EXPECT_NO_THROW(mutex_lock.lock());
  EXPECT_NO_THROW(mutex_lock.unlock());
}

// 测试2：RAII包装器构造和析构
TEST_F(MutexLockTest, MutexLockGuardConstruction) {
  EXPECT_NO_THROW({ Tupo::base::MutexLockGuard lock(mutex_lock); });
}

// 测试3：测试多线程环境锁正常使用

TEST_F(MutexLockTest, ThreadMutex) {
  std::atomic<int> counter{0};
  const int num_threads = 10;
  const int increments_per_thread = 1000;

  auto worker = [this, &counter, increments_per_thread]() {
    for (int i = 0; i < increments_per_thread; ++i) {
      mutex_lock.lock();
      counter++; // 受保护的操作
      date++;    // 受保护的操作
      mutex_lock.unlock();
    }
  };

  std::vector<std::thread> threads;
  for (int i = 0; i < num_threads; ++i) {
    threads.emplace_back(worker);
  }

  for (auto &t : threads) {
    t.join();
  }

  EXPECT_EQ(counter, num_threads * increments_per_thread);
  EXPECT_EQ(date, num_threads * increments_per_thread);
}

// 测试4：测试MutexLockGuard在多线程环境下的正确性
TEST_F(MutexLockTest, ThreadMutexGuard) {
  std::atomic<int> counter{0};
  const int num_threads = 10;
  const int increments_per_thread = 1000;
  auto worker = [this, &counter, increments_per_thread]() {
    for (int i = 0; i < increments_per_thread; ++i) {
      Tupo::base::MutexLockGuard lock(mutex_lock);
      counter++; // 受保护的操作
      date++;    // 受保护的操作
    }
  };

  std::vector<std::thread> threads;
  for (int i = 0; i < num_threads; ++i) {
    threads.emplace_back(worker);
  }

  for (auto &t : threads) {
    t.join();
  }

  EXPECT_EQ(counter, num_threads * increments_per_thread);
  EXPECT_EQ(date, num_threads * increments_per_thread);
}