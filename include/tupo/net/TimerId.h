#pragma once
#include <cstdint>

namespace Tupo {
namespace net {
class Timer;
class TimerId {
public:
  TimerId() : timer_(nullptr), sequence_(0) {}
  TimerId(Timer *timer, int64_t sequence)
      : timer_(timer), sequence_(sequence) {}

  Timer *timer() const { return timer_; }
  int64_t sequence() const { return sequence_; }

  // 添加 valid() 方法
  bool valid() const { return timer_ != nullptr; }

private:
  Timer *timer_;
  int64_t sequence_; // 定时器唯一编号
};
} // namespace net
} // namespace Tupo