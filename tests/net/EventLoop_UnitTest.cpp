#include "tupo/net/EventLoop.h"
#include <gtest/gtest.h>

class EventLoopTest : public testing::Test {
protected:
  void SetUp() override { loop_ = std::make_unique<Tupo::net::EventLoop>(); }

  void TearDown() override {
    loop_->quit();
    // 给事件循环一点时间退出
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  std::unique_ptr<Tupo::net::EventLoop> loop_;
};

// 测试1：基本的事创建和销毁
TEST_F(EventLoopTest, CreateAndDestroy) { EXPECT_NE(loop_, nullptr); }

// 测试2：检查事件是否在同一个线程
TEST_F(EventLoopTest, IsInLoopThread) {
  EXPECT_TRUE(loop_->isInLoopThread());
  std::atomic<bool> result{true};
  std::thread t([&]() { result = loop_->isInLoopThread(); });
  t.join();
  EXPECT_FALSE(result);
}

// 测试3：RunInLoop直接调用
TEST_F(EventLoopTest, RunInLoopDirect) {
  std::atomic<bool> executed{false};

  // 在 IO 线程中调用，应该直接执行
  loop_->runInLoop([&]() { executed = true; });

  EXPECT_TRUE(executed);
}

// 测试4：RunInLoop跨线程调用（确保EventLoop在正确的线程中创建和运行）
TEST_F(EventLoopTest, RunInLoopCrossThread) {
  std::atomic<bool> executed{false};
  Tupo::net::EventLoop *loop = nullptr;
  // 在子线程中运行事件循环
  Tupo::base::Thread loopThread([this, &loop]() {
    Tupo::net::EventLoop ev;
    loop = &ev;
    ev.loop(); // 在子线程中阻塞运行
  });
  loopThread.start();
  // 等待事件循环启动
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  // 在另一个线程中投递任务
  Tupo::base::Thread t([this, &executed, &loop]() {
    loop->runInLoop([&]() { executed = true; });
  });
  t.start();

  // 等待任务执行
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  t.join();

  EXPECT_TRUE(executed);

  // 退出事件循环
  loop->quit();
  loopThread.join();
}
