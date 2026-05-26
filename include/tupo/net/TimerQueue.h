#pragma once
#include "tupo/base/Timestamp.h"
#include "tupo/net/Channel.h"
#include "tupo/net/Timer.h"
#include "tupo/net/TimerId.h"
#include <atomic>
#include <set>
#include <utility>
#include <vector>

namespace Tupo {
namespace net {
class EventLoop;
class TimerId;

class TimerQueue {
public:
  explicit TimerQueue(EventLoop *loop);
  TimerQueue(const TimerQueue &) = delete;
  TimerQueue &operator=(const TimerQueue &) = delete;
  ~TimerQueue();

  TimerId addTimer(const Timer::TimerCallback cb, Tupo::base::Timestamp when,
                   double interval);

private:
  void handleRead();

  typedef std::pair<Tupo::base::Timestamp, Timer *> Entry;
  typedef std::set<Entry> TimerList;
  typedef std::pair<Timer *, int64_t> ActiveTimer;
  typedef std::set<ActiveTimer> ActiveTimerSet;
  EventLoop *loop_;
  const int timerfd_;
  Channel timerfdChannel_;

  // 移除过期的定时器
  std::vector<Entry> getExpired(Tupo::base::Timestamp now);
  // 处理已到期的定时器，并重新设置下一个定时器触发时间。
  void reset(const std::vector<Entry> &expired, Tupo::base::Timestamp now);

  // 插入定时器
  bool insert(Timer *timer);
  TimerList timers_; // 按时间排序的定时器集合

  // 取消定时器
  ActiveTimerSet activeTimers_;
  std::atomic_bool callingExpiredTimers_;
  ActiveTimerSet cancelingTimers_;
};
} // namespace net
} // namespace Tupo