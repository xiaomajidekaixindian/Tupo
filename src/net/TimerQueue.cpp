#include "tupo/net/TimerQueue.h"
#include "tupo/base/Timestamp.h"
#include "tupo/net/EventLoop.h"
#include <algorithm>
#include <assert.h>
#include <iostream>
#include <string.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <utility> // 添加这个用于 std::make_pair
namespace Tupo {
namespace net {

namespace detail {
int createTimerfd() {
  int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  if (timerfd < 0) {
    std::cerr << "Failed in timerfd_create" << std::endl;
  }
  return timerfd;
}

// 读取timerfd，清除可读状态
void readTimerfd(int timerfd, Tupo::base::Timestamp now) {
  uint64_t howmany;
  ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
  if (n != sizeof(howmany)) {
    std::cerr << "TimerQueue::handleRead() reads " << n
              << " bytes instead of 8";
  }
}

// 重置定时器
void resetTimerfd(int timerfd, Tupo::base::Timestamp expration) {
  struct itimerspec newValue;
  struct itimerspec oldValue;
  memset(&oldValue, 0, sizeof(newValue));
  memset(&newValue, 0, sizeof(newValue));

  // 计算超时时间
  Tupo::base::Timestamp now = Tupo::base::Timestamp::now();
  int64_t microSeconds =
      expration.microSecondsSinceEpoch() - now.microSecondsSinceEpoch();
  // 3. 最小时间保护（重要！）
  if (microSeconds < 100) // 确保至少100微秒
  {
    microSeconds = 100;
  }

  // 设置定时器
  newValue.it_value.tv_sec =
      microSeconds / Tupo::base::Timestamp::kMicroSecondsPerSecond;
      
  newValue.it_value.tv_nsec =
      (microSeconds % Tupo::base::Timestamp::kMicroSecondsPerSecond) * 1000;

  int ret = timerfd_settime(timerfd, 0, &newValue, &oldValue);
  if (ret) {
    std::cout << "timerfd_settime()" << std::endl;
  }
}
} // namespace detail

TimerQueue::TimerQueue(EventLoop *loop)
    : loop_(loop), timerfd_(detail::createTimerfd()),
      timerfdChannel_(loop, timerfd_), callingExpiredTimers_(false) {
  std::cout << "TimerQueue created" << std::endl;
  timerfdChannel_.setReadCallback([this]() { this->handleRead(); });
  timerfdChannel_.enableReading();
}

TimerQueue::~TimerQueue() {}

TimerId TimerQueue::addTimer(const Timer::TimerCallback cb,
                             Tupo::base::Timestamp when, double interval) {
  Timer *timer = new Timer(std::move(cb), when, interval);

  // 插入定时器到集合中
  auto result = timers_.insert(Entry(when, timer));
  return TimerId(timer, timer->sequence());
}

void TimerQueue::handleRead() {
  loop_->assertInLoopThread();

  Tupo::base::Timestamp now(Tupo::base::Timestamp::now());

  // 读取timerfd，清除可读状态
  detail::readTimerfd(timerfd_, now);
  
  // 处理过期的定时器
  std::vector<Entry> expired = getExpired(now);
  callingExpiredTimers_ = true;
  cancelingTimers_.clear();
  for (const Entry &it : expired) {
    it.second->run();
  }
  callingExpiredTimers_ = false;
}
std::vector<TimerQueue::Entry>
TimerQueue::getExpired(Tupo::base::Timestamp now) {
  std::vector<Entry> expired;

  // 找出所有<=now的定时器
  Entry sentry = std::make_pair(now, reinterpret_cast<Timer *>(UINTPTR_MAX));

  // 1. 一次查找找到分界点
  TimerList::iterator end = timers_.lower_bound(sentry);
  assert(end == timers_.end() || now < end->first);

  // 2. 批量拷贝到期的定时器
  std::copy(timers_.begin(), end, back_inserter(expired));

  // 3. 批量删除已处理的定时器
  timers_.erase(timers_.begin(), end);
  for (const Entry &it : expired) {
    ActiveTimer timer(it.second, it.second->sequence());
    size_t n = activeTimers_.erase(timer);
    assert(n == 1);
  }
  assert(timers_.size() == activeTimers_.size());

  return expired;
}

void TimerQueue::reset(const std::vector<Entry> &expired,
                       Tupo::base::Timestamp now) {
  Tupo::base::Timestamp nextExpire;
  for (const Entry &it : expired) {
    ActiveTimer timer(it.second, it.second->sequence());

    // 如果是重复定时器且未取消
    if (it.second->repeat() &&
        cancelingTimers_.find(timer) == cancelingTimers_.end()) {
      // 重新启动并插入
      it.second->restart(now);
      insert(it.second);
    } else {
      delete it.second;
    }
    if (!timers_.empty()) {
      nextExpire = timers_.begin()->first;
    }
    }
}

bool TimerQueue::insert(Timer *timer) {
  loop_->assertInLoopThread();
  assert(timers_.size() == activeTimers_.size());

  bool earliestChanged = false;
  Tupo::base::Timestamp when = timer->expiration();
  TimerList::iterator it = timers_.begin();
  if (it == timers_.end() || when < it->first) {
    earliestChanged = true;
  }

  {
    std::pair<TimerList::iterator, bool> result =
        timers_.insert(Entry(when, timer));
    assert(result.second);
    (void)result;
  }
  {
    std::pair<ActiveTimerSet::iterator, bool> result =
        activeTimers_.insert(ActiveTimer(timer, timer->sequence()));
    assert(result.second);
    (void)result;
  }
  assert(timers_.size() == activeTimers_.size());
  return earliestChanged;
}

} // namespace net
} // namespace Tupo