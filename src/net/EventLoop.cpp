#include "tupo/net/EventLoop.h"
#include "tupo/base/Logger.h"
#include "tupo/net/Poller.h"
#include "tupo/net/poller/PollPoller.h"
#include <sys/eventfd.h>
namespace Tupo {
namespace net {

namespace {

const int kPollTimeMs = 10000;

int createEventfd() {
  int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (evtfd < 0) {
    LOG_ERROR << "Failed in eventfd";
  }
  return evtfd;
}

thread_local EventLoop *t_loopInThisThread = nullptr;

} // namespace
EventLoop::EventLoop()
    : looping_(false), quit_(false),
      threadId_(Tupo::base::Thread::currentThreadTid()),
      poller_(Poller::newDefaultPoller(this)), activeChannels_(),
      timerQueue_(new TimerQueue(this)), wakeupFd_(createEventfd()),
      wakeupChannel_(new Channel(this, wakeupFd_)) {
  if (t_loopInThisThread != nullptr) {
    LOG_ERROR << "Another EventLoop " << t_loopInThisThread
              << " exists in this thread " << threadId_;
  } else {
    t_loopInThisThread = this;
    LOG_INFO << "EventLoop created in thread " << threadId_;
  }
  // 跨线程唤醒
  // wakeupChannel_->setReadCallback([this]() { this->handleRead(); });
  // wakeupChannel_->enableReading();
};

EventLoop::~EventLoop() {
  assert(!looping_);
  if (t_loopInThisThread == this) {
    t_loopInThisThread = nullptr;
  }
  wakeupChannel_->remove();
  if (wakeupFd_ >= 0) {
    ::close(wakeupFd_);
    wakeupFd_ = -1;
  }
}

void EventLoop::loop() {
  assert(!looping_);    // 防止重复进入事件循环
  assertInLoopThread(); // 确保在正确的线程
  looping_ = true;
  quit_ = false;
  LOG_INFO << "EventLoop " << this << " start looping";
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
  LOG_INFO << "EventLoop" << this << "stop looping";
  looping_ = false;
}

void EventLoop::quit() {
  LOG_INFO << "EventLoop quit" << this;
  quit_ = true;
  // 如果在其他线程调用，需要唤醒 poll，否则可能永久阻塞
  if (!isInLoopThread()) {
    wakeup();
  }
}

EventLoop *EventLoop::getEventLoopOfCurrent() { return t_loopInThisThread; }

void EventLoop::updateChannel(Channel *channel) {
  // 1. 关键：确保在IO线程中调用
  assertInLoopThread();
  // 2. 记录调试信息
  LOG_INFO << "EventLoop::updateChannel fd = " << channel->fd()
           << " events = " << channel->events();
  poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel) {
  assertInLoopThread();
  poller_->removeChannel(channel);
  channel->setAddedToLoop(false); // 通知 Channel 状态已改变
}

void EventLoop::abortNotInLoopThread() {
  LOG_ERROR << "EventLoop::abortNotInLoopThread - EventLoop " << this
            << " was created in threadId_ = " << threadId_
            << ", current thread id = "
            << Tupo::base::Thread::currentThreadTid();
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
    LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
  }
}

void EventLoop::handleRead() {
  uint64_t one = 1;
  ssize_t n = ::read(wakeupFd_, &one, sizeof one);
  if (n != sizeof one) {
    LOG_ERROR << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
  }
}
} // namespace net
} // namespace Tupo