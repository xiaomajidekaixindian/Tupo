#include "tupo/net/EventLoop.h"
#include "tupo/net/Poller.h"
#include "tupo/net/poller/PollPoller.h"
#include <iostream>
#include <sys/eventfd.h>
namespace Tupo {
namespace net {
__thread EventLoop *t_loopInThisThread = nullptr;
namespace {

const int kPollTimeMs = 10000;

int createEventfd() {
  int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (evtfd < 0) {
    std::cerr << "Failed in eventfd" << std::endl;
  }
  return evtfd;
}

} // namespace
EventLoop::EventLoop()
    : looping_(false), quit_(false),
      threadId_(Tupo::base::Thread::currentThreadTid()),
      poller_(Poller::newDefaultPoller(this)), activeChannels_(),
      timerQueue_(new TimerQueue(this)), wakeupFd_(createEventfd()),
      wakeupChannel_(new Channel(this, wakeupFd_)) {
  if (t_loopInThisThread != nullptr) {
    std::cout << "Another EventLoop exists in this thread " << threadId_
              << std::endl;
  } else {
    t_loopInThisThread = this;
    std::cout << "EventLoop created in thread " << threadId_ << std::endl;
  }
  wakeupChannel_->setReadCallback([this]() { this->handleRead(); });
  wakeupChannel_->enableReading();
};

EventLoop::~EventLoop() {
  assert(!looping_);
  if (t_loopInThisThread == this) {
    t_loopInThisThread = nullptr;
  }
  wakeupChannel_->remove();
  ::close(wakeupChannel_->fd());
}

void EventLoop::loop() {
  assert(!looping_);    // 防止重复进入事件循环
  assertInLoopThread(); // 确保在正确的线程
  looping_ = true;
  quit_ = false;
  std::cout << "EventLoop " << this << " start looping" << std::endl;
  while (!quit_) {
    // 清空活动通道列表
    activeChannels_.clear();
    int timeout = 10000; // 10s超时
    poller_->poll(timeout, &activeChannels_);
    for (auto it : activeChannels_) {
      it->handleEvent();
    }
    doPendingFunctors();
  }
  std::cout << "EventLoop" << this << "stop looping" << std::endl;
  looping_ = false;
}

void EventLoop::quit() {
  std::cout << "EventLoop quit" << std::endl;
  quit_ = true;
  // 如果在其他线程调用，需要唤醒 poll，否则可能永久阻塞
  if (!isInLoopThread()) {
    wakeup();
  }
}

void EventLoop::updateChannel(Channel *channel) {
  // 1. 关键：确保在IO线程中调用
  assertInLoopThread();
  // 2. 记录调试信息
  std::cout << "EventLoop::updateChannel fd = " << channel->fd()
            << " events = " << channel->events() << std::endl;
  poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel) {
  assertInLoopThread();
  poller_->removeChannel(channel);
  channel->setAddedToLoop(false); // 通知 Channel 状态已改变
}

void EventLoop::abortNotInLoopThread() {
  std::cout << "EventLoop::abortNotInLoopThread - EventLoop " << this
            << " was created in threadId_ = " << threadId_
            << ", current thread id = "
            << Tupo::base::Thread::currentThreadTid();
}

void EventLoop::runInLoop(const Functor &cb) {
  if (isInLoopThread()) {
    cb();
  } else {
    queueInLoop(cb);
  }
}

void EventLoop::runInLoop(Functor &&cb) {
  if (isInLoopThread()) {
    cb();
  } else {
    queueInLoop(std::move(cb));
  }
}

void EventLoop::queueInLoop(const Functor &cb) {
  {
    Tupo::base::MutexLockGuard lock(mutex_);
    pendingFunctors_.emplace_back(cb);
  }
  wakeup();
}

void EventLoop::queueInLoop(Functor &&cb) {
  {
    Tupo::base::MutexLockGuard lock(mutex_);
    pendingFunctors_.push_back(std::move(cb));
  }
  wakeup();
}

void EventLoop::doPendingFunctors() {
  std::vector<Functor> functors;
  {
    Tupo::base::MutexLockGuard lock(mutex_);
    functors.swap(pendingFunctors_);
  }
  for (const auto &functor : functors) {
    functor();
  }
}

TimerId EventLoop::runAt(const Tupo::base::Timestamp &time, TimerCallback cb) {
  return timerQueue_->addTimer(std::move(cb), time, 0.0);
}

TimerId EventLoop::runAfter(double delay, TimerCallback cb) {
  Tupo::base::Timestamp time(
      Tupo::base::Timestamp::resetTime(Tupo::base::Timestamp::now(), delay));
  return runAt(time, std::move(cb));
}

TimerId EventLoop::runEvery(double interval, TimerCallback cb) {
  Tupo::base::Timestamp time(
      Tupo::base::Timestamp::resetTime(Tupo::base::Timestamp::now(), interval));
  return timerQueue_->addTimer(std::move(cb), time, interval);
}

void EventLoop::wakeup() {
  uint64_t one = 1;
  ssize_t n = ::write(wakeupFd_, &one, sizeof(one));
  if (n != sizeof(one)) {
    std::cerr << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
  }
}

void EventLoop::handleRead() {
  uint64_t one = 1;
  ssize_t n = ::read(wakeupFd_, &one, sizeof one);
  if (n != sizeof one) {
    std::cerr << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
  }
}
} // namespace net
} // namespace Tupo