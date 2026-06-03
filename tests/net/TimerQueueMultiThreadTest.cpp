#include <gtest/gtest.h>
#include "tupo/base/Thread.h"
#include "tupo/net/TimerQueue.h"
#include "tupo/net/EventLoop.h"

class TimerQueueMultiThreadTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建一个EventLoop实例
        loop_ = std::make_unique<Tupo::net::EventLoop>();
    }
    void TearDown() override {
        // 确保事件循环停止
        loop_->quit();
        thread_->join();
    }
    void startLoopInThread() {
        thread_ = std::make_unique<Tupo::base::Thread>([this]() {
            loop_->loop();
        });
        thread_->start();
    }
    std::unique_ptr<Tupo::net::EventLoop> loop_;
    std::unique_ptr<Tupo::base::Thread> thread_;
};

// test1: 在多线程环境下，定时器能否正确执行
TEST_F(TimerQueueMultiThreadTest, TimerExecution) {
    startLoopInThread();
    std::atomic<bool> timerExecuted(false);
    loop_->runAfter(1.0, [&timerExecuted]() {
        timerExecuted = true;
    }); 
    thread_->join(); // 等待事件循环线程结束
    EXPECT_TRUE(timerExecuted); // 验证定时器是否执行
}    