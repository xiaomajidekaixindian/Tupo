#include "tupo/net/http/HttpResponse.h"
namespace Tupo {
namespace net {
HttpResponse::HttpResponse() : statusCode_(k200Ok) {}

std::optional<std::string>
HttpResponse::getHeader(const std::string &key) const {
  auto it = headers_.find(key);
  if (it != headers_.end()) {
    return it->second;
  }
  return std::nullopt;
}

std::string HttpResponse::serialize() const {
  std::ostringstream oss;

  // 状态行
  oss << "HTTP/1.1 " << statusCode_ << " ";
  if (!statusMessage_.empty()) {
    oss << statusMessage_;
  } else {
    switch (statusCode_) {
    case k200Ok:
      oss << "OK";
      break;
    case k301MovedPermanently:
      oss << "Moved Permanently";
      break;
    case k400BadRequest:
      oss << "Bad Request";
      break;
    case k404NotFound:
      oss << "Not Found";
      break;
    case k500InternalServerError:
      oss << "Internal Server Error";
      break;
    default:
      oss << "Unknown";
      break;
    }
  }
  oss << "\r\n";

  // 响应头
  // 没有设置就自动添加，没有消息就没必要设置头了
  if (!getHeader("Content-Length").has_value() && !body_.empty()) {
    oss << "Content-Length: " << body_.size() << "\r\n";
  }
  // 如果没有设置Connection，默认close
  if (!getHeader("Connection").has_value()) {
    oss << "Connection: close\r\n";
  }
  for (const auto &pair : headers_) {
    oss << pair.first << ": " << pair.second << "\r\n";
  }

  oss << "\r\n";

  // 3. 消息体
  oss << body_;

  return oss.str();
}
} // namespace net
} // namespace Tupo