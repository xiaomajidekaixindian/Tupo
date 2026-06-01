#pragma once
#include "tupo/base/Timestamp.h"
#include <functional>

namespace Tupo {
namespace net {
class Timer {

  friend class TimerQueue;
public:
  using TimerCallback = std::function<void()>;

  void run() const {
    if (callback_)
      callback_();
  }
  // 返回到期时间
  Tupo::base::Timestamp expiration() { return expiration_; }

  // 重置定时器
  void restart(Tupo::base::Timestamp now);

  // 检查是否为非重复定时器
  bool repeat() const { return repeat_; }

  // 获取定时器间隔时间
  double interval() { return interval_; }

  // 获取定时器号
  int64_t sequence() const { return sequence_; }

private:
  // cb: 定时器回调函数
  // when: 定时器到期时间
  // interval: 定时器间隔时间，单位为秒，0表示非重复定时器
  // repeat: 是否为重复定时器
  // sequence: 定时器序列号，用于区分不同定时器
  Timer(TimerCallback cb, Tupo::base::Timestamp when, double interval)
      : callback_(std::move(cb)), expiration_(when), interval_(interval),
        repeat_(interval > 0.0), sequence_(0) {}
  Timer(const Timer &) = delete;
  Timer &operator=(const Timer &) = delete;
  Timer(Timer &&) = delete;
  Timer &operator=(Timer &&) = delete;

  const TimerCallback callback_;
  Tupo::base::Timestamp expiration_;
  const double interval_;
  const bool repeat_;
  int64_t sequence_;
};
} // namespace net
} // namespace Tupo