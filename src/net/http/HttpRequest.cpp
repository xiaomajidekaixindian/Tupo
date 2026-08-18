#include "tupo/net/http/HttpRequest.h"

namespace Tupo {
namespace net {

HttpRequest::HttpRequest()
    : method_(kInvalid), parseState_(kExpectRequestLine) {}

const char *HttpRequest::methodString() const {
  switch (method_) {
  case kGet:
    return "GET";
  case kPost:
    return "POST";
  case kPut:
    return "PUT";
  case kDelete:
    return "DELETE";
  case kHead:
    return "HEAD";
  default:
    return "UNKNOWN";
  }
}

std::string HttpRequest::getHeader(const std::string &key) const {
  auto it = headers_.find(key);
  if (it != headers_.end()) {
    return it->second;
  }
  return "";
}

size_t HttpRequest::contentLength() const {
  std::string len = getHeader("Content-Length");
  if(len.empty()){
    return 0;
  }
  return std::stoul(len);
}
} // namespace net
} // namespace Tupo