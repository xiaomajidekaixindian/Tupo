#pragma once
#include "tupo/net/http/HttpRequest.h"
namespace Tupo {
namespace net {
class Buffer;
class HttpContext {
public:
  HttpContext();
  bool ParseRequest(Buffer *buffer, HttpRequest *request);
  const HttpRequest::ParseState getState() const { return state_; }

  void reset();

private:

  HttpRequest::ParseState state_;

#ifdef NDEBUG
  bool processRequestLine(const char *begin, const char *end,
                          HttpRequest *request);
  bool processHeaders(const char *begin, const char *end, HttpRequest *request);
  bool processBody(const char *begin, const char *end, HttpRequest *request);
#endif

#ifdef DEBUG
public:
  bool processRequestLine(const char *begin, const char *end,
                          HttpRequest *request);
  bool processHeaders(const char *begin, const char *end, HttpRequest *request);
  bool processBody(const char *begin, const char *end, HttpRequest *request);
#endif

};
} // namespace net
} // namespace Tupo