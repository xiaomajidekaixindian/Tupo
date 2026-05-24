#pragma once
#include <cstdint>

namespace Tupo {
namespace base {
class Timestamp {
public:
  Timestamp() : microSecondsSinceEpoch_(0) {}
  explicit Timestamp(int64_t microSconds)
      : microSecondsSinceEpoch_(microSconds) {}

  static Timestamp now();

  // 重新获取定时器时间
  static Timestamp resetTime(Timestamp now, double interval);

  // 返回默认定时器，默认构造表示定时器无效
  static Timestamp invalid() { return Timestamp(); }

  static const int kMicroSecondsPerSecond = 1000 * 1000;
  int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }

  bool operator<(const Timestamp &rhs) const {
    return microSecondsSinceEpoch_ < rhs.microSecondsSinceEpoch_;
  }

private:
  int64_t microSecondsSinceEpoch_;
};
} // namespace base
} // namespace Tupo