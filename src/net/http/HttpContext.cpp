#include "tupo/net/http/HttpContext.h"
#include "tupo/net/Buffer.h"
#include <algorithm>

namespace Tupo {
namespace net {
HttpContext::HttpContext() : state_(HttpRequest::kExpectRequestLine) {}

/**
 * 核心算法：每次都计算\r的位置，以及每行开头到\r的长度
 */

bool HttpContext::ParseRequest(Buffer *buffer, HttpRequest *request) {

  const char *begin = buffer->begin();
  const char *end = begin + buffer->readableBytes();

  while (state_ != HttpRequest::kGotAll) {
    const char *crlf = std::search(begin, end, "\r\n", "\r\n" + 2);
    if (crlf == end) {
      // 数据不完整
      break;
    }
    const char *lineEnd = crlf + 2;
    size_t lineLength = crlf - begin;

    switch (state_) {
    case HttpRequest::kExpectRequestLine:
      if (!processRequestLine(begin, crlf, request)) {
        return false;
      }
      state_ = HttpRequest::kExpectHeaders;
      buffer->retrieve(lineEnd - begin);
      break;
    case HttpRequest::kExpectHeaders:
      if (lineLength == 0) {
        size_t contentLen = request->contentLength();
        buffer->retrieve(lineEnd - begin);

        if (contentLen > 0) {
          state_ = HttpRequest::kExpectBody;
        } else {
          state_ = HttpRequest::kGotAll;
        }
      } else {
        if (!processHeaders(begin, crlf, request)) {
          return false;
        }
        buffer->retrieve(lineEnd - begin);
      }
      break;
    case HttpRequest::kExpectBody:

      if (!processBody(begin, crlf, request)) {
        return false;
      }
      buffer->retrieve(request->contentLength());
      state_ = HttpRequest::kGotAll;
    // size_t contentLen = request->contentLength();
    // size_t bodyLen = buffer->readableBytes();
    default:
      // 未知情况
      break;
    }
  }
}

bool HttpContext::processRequestLine(const char *begin, const char *end,
                                     HttpRequest *request) {
  std::string line(begin, end - begin);

  // 解析请求行：GET /index.html HTTP/1.1

  // 找第一个空格
  size_t pos1 = line.find(' ');
  if (pos1 == std::string::npos) {
    return false;
  }
  // 找第二个空格
  size_t pos2 = line.find(' ', pos1 + 1);
  if (pos2 == std::string::npos) {
    return false;
  }

  std::string method = line.substr(0, pos1);
  if (method == "GET") {
    request->setMethod(HttpRequest::kGet);
  } else if (method == "POST") {
    request->setMethod(HttpRequest::kPost);
  } else if (method == "PUT") {
    request->setMethod(HttpRequest::kPut);
  } else if (method == "DELETE") {
    request->setMethod(HttpRequest::kDelete);
  } else if (method == "HEAD") {
    request->setMethod(HttpRequest::kHead);
  } else {
    return false;
  }

  // 路径
  request->setPath(line.substr(pos1 + 1, pos2 - pos1 - 1));

  // 版本
  request->setVersion(line.substr(pos2 + 1));

  return true;
}
bool HttpContext::processHeaders(const char *begin, const char *end,
                                 HttpRequest *request) {

  std::string line(begin, end - begin);

  // 解析请求头：Host: localhost:8080
  size_t pos = line.find(':');
  if (pos == std::string::npos) {
    return false;
  }
  std::string host = line.substr(0, pos);
  std::string port = line.substr(pos + 1);

  // 去除首尾空格
  host.erase(0, host.find_first_not_of(" \t"));
  host.erase(host.find_last_not_of(" \t") + 1);
  port.erase(0, port.find_first_not_of(" \t"));
  port.erase(port.find_last_not_of(" \t") + 1);

  request->setHeader(host, port);
  return true;
}

bool HttpContext::processBody(const char *begin, const char *end,
                              HttpRequest *request) {
  size_t contentLen = request->contentLength();
  if (contentLen <= 0) {
    state_ = HttpRequest::kGotAll;
    return true;
  }

  // 剩余数据长度
  size_t available = end - begin;

  // 如果数据不够，半路截断等待下一次
  if (available < contentLen) {
    return false;
  }

  std::string body(begin, contentLen);
  request->setBody(body);
  return true;
}

// 长连接需要更新状态
void HttpContext::reset() { state_ = HttpRequest::kExpectRequestLine; }
} // namespace net
} // namespace Tupo