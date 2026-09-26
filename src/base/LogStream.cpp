#include "tupo/base/LogStream.h"
#include <cstring>
namespace Tupo {
namespace base {
LogStream &LogStream::operator<<(bool v) {
  append(v ? "true" : "false", v ? 4 : 5);
  return *this;
}

LogStream &LogStream::operator<<(int v) {
  char buf[32];
  int len = snprintf(buf, sizeof(buf), "%d", v);
  append(buf, len);
  return *this;
}

LogStream &LogStream::operator<<(unsigned int v) {
  char buf[32];
  int len = snprintf(buf, sizeof(buf), "%u", v);
  append(buf, len);
  return *this;
}

LogStream &LogStream::operator<<(double v) {
  char buf[32];
  int len = snprintf(buf, sizeof(buf), "%.12f", v);
  append(buf, len);
  return *this;
}

LogStream &LogStream::operator<<(char c) {
  append(&c, 1);
  return *this;
}

LogStream &LogStream::operator<<(const char *str) {
  if (str) {
    append(str, strlen(str));
  } else {
    append("(null)", 6);
  }
  return *this;
}

LogStream &LogStream::operator<<(const std::string &str) {
  append(str.c_str(), str.size());
  return *this;
}

void LogStream::append(const char *data, int len) {
  if (current_ + len < kBufferSize) {
    memcpy(buffer_ + current_, data, len);
    current_ += len;
  }
}
} // namespace base
} // namespace Tupo