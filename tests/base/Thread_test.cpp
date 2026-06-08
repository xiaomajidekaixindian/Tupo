#include "tupo/base/MutexLock.h"
#include "tupo/base/Thread.h"
#include <future>
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

// 测试2：测试多个线程同时修改共享数据
TEST_F(ThreadTest, MultipleThreads) {
  const int num_threads = 10;
  const int increments_per_thread = 1000;
  auto worker = [this, increments_per_thread]() {
    for (int i = 0; i < increments_per_thread; ++i) {
      Tupo::base::MutexLockGuard lock(mutex_lock);
      date++; // 受保护的操作
    }
  };
  std::vector<Tupo::base::Thread> threads;
  for (int i = 0; i < num_threads; ++i) {
    threads.emplace_back(worker, "WorkerThread" + std::to_string(i));
  }
  for (auto &thread : threads) {
    // 启动线程
    thread.start();
    // 打印线程ID和名称
    std::cout << "Started thread with ID: " << thread.tid() << std::endl;
    std::cout << "Thread name: " << thread.name() << std::endl;
  }
  for (auto &thread : threads) {
    thread.join();
  }
  EXPECT_EQ(date, num_threads * increments_per_thread);
}

// 测试3：测试线程分离
TEST_F(ThreadTest, DetachThread) {
  auto promise_ptr = std::make_shared<std::promise<void>>();
  std::future<void> future = promise_ptr->get_future();
  {
    Tupo::base::Thread thread([promise_ptr]() mutable {
      std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 模拟工作
      std::cout<<"child thread id: "<<Tupo::base::Thread::currentThreadTid()<<std::endl;
      promise_ptr->set_value(); // 通知测试线程工作完成
    });
    std::cout<<"main thread id: "<<Tupo::base::Thread::currentThreadTid()<<std::endl;
    thread.start();
    thread.detach();
  }
  // 等待分离线程完成
  auto status = future.wait_for(std::chrono::seconds(1));
  EXPECT_EQ(status, std::future_status::ready);
}