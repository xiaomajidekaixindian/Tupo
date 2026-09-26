#pragma once
#include <string>
namespace Tupo {
namespace base {
class LogStream {
public:
  static const int kBufferSize = 4096;

  LogStream &operator<<(bool v);
  LogStream &operator<<(int v);
  LogStream &operator<<(unsigned int v);
  LogStream &operator<<(double v);
  LogStream &operator<<(char c);
  LogStream &operator<<(const char *str);
  LogStream &operator<<(const std::string &str);

  void append(const char *data, int len);
  const char *data() const { return buffer_; }
  int size() const { return current_; }
  void reset() { current_ = 0; }

private:
  char buffer_[kBufferSize];
  int current_ = 0;
};
} // namespace base
} // namespace Tupo