#pragma once
#include "tupo/base/MutexLock.h"
#include "tupo/base/Thread.h"
#include "tupo/base/Timestamp.h"
#include "tupo/net/TimerId.h"
#include "tupo/net/TimerQueue.h"
#include <assert.h>
#include <atomic>
#include <memory>
#include <pthread.h>
#include <sys/types.h>
#include <vector>
namespace Tupo {
namespace net {
class Poller;
class EventLoop;
class Channel;
extern __thread EventLoop *t_loopInThisThread; // 声明

class EventLoop {
public:
  EventLoop();

  ~EventLoop();

  EventLoop(const EventLoop &) = delete;
  EventLoop &operator=(const EventLoop &) = delete;
  EventLoop(const EventLoop &&) = delete;
  EventLoop &&operator=(const EventLoop &&) = delete;

  // 核心方法；运行事件循环
  void loop();

  // 停止事件循环
  void quit();

  // Channel管理
  void updateChannel(Channel *channel);

  // 线程安全相关
  void assertInLoopThread() {
    if (!isInLoopThread()) {
      abortNotInLoopThread();
    }
  }

  EventLoop *getEventLoopOfCurrent() { return t_loopInThisThread; }
  // 判断是否在事件线程中
  bool isInLoopThread() const {
    return threadId_ == Tupo::base::Thread::currentThreadTid();
  }

  // 检查事件是否还在事件循环中，防止意外关闭，用于Channel析构
  void removeChannel(Channel *channel);

  // 获取内部的Poller用于测试
  Poller *poller() { return poller_.get(); }
  const Poller *poller() const { return poller_.get(); }

  // 解决多线程环境下，非IO线程安全地向IO线程（即EventLoop所在线程）提交任务的同步问题
  using Functor = std::function<void()>;
  void runInLoop(const Functor &cb);

  // 定时器操作
  typedef std::function<void()> TimerCallback;

  // 在指定的绝对时间点执行一次回调函数
  TimerId runAt(const Tupo::base::Timestamp &time, TimerCallback cb);

  // 在延迟指定时间后执行一次回调函数。
  TimerId runAfter(double delay, TimerCallback cb);

  // 每隔指定的时间间隔重复执行回调函数。
  TimerId runEvery(double interval, TimerCallback cb);

  // 唤醒epoll_wait,否则会一直阻塞
  void wakeup();

  // 判断是否quit
  bool isQuits() const { return quit_; }

private:
  void abortNotInLoopThread();

  std::atomic<bool> looping_;
  std::atomic<bool> quit_;
  const pid_t threadId_;           // 所属线程ID
  std::unique_ptr<Poller> poller_; // Poller对象
  // Tupo::base::MutexLockGuard mutex_; // 保护pendingFunctors_的互斥锁
  typedef std::vector<Channel *> ChannelList;
  ChannelList activeChannels_;
  std::unique_ptr<TimerQueue> timerQueue_;

  void handleRead(); // 处理wakeup
  // 唤醒事件循环
  int wakeupFd_;
  // 包装唤醒的文件描述符
  std::unique_ptr<Channel> wakeupChannel_;

  // std::vector<Functor> pendingFunctors_;
};
} // namespace net
} // namespace Tupo