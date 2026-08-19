#pragma once
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
namespace Tupo {
namespace net {
/**
 * @brief HTTP 响应封装类
 *
 * 返回值设计原则（黄金法则）：W
 * - 成员变量（如 body_, statusMessage_）永远存在 → 返回 const
 * std::string&（零拷贝，安全）
 * - 容器查询（如 headers_ 的查找）可能不存在 → 返回
 * std::optional<std::string>（明确表达“可能没有”）
 *
 * 调用者注意：
 * - 若持有 const std::string& 引用，需确保 HttpResponse
 * 对象生命周期长于该引用。
 */

class HttpResponse {
public:
  enum StatusCode {
    k200Ok = 200,               // 成功
    k301MovedPermanently = 301, // 重定向
    k400BadRequest = 400,
    k404NotFound = 404,           // 资源不存在
    k500InternalServerError = 500 // 内部错误
  };
  HttpResponse();

  void setStatusCode(StatusCode code) { statusCode_ = code; }
  StatusCode statusCode() const { return statusCode_; }

  void setStatusMessage(const std::string &msg) { statusMessage_ = msg; }
  const std::string &statusMessage() const { return statusMessage_; }

  void setHeader(const std::string &key, const std::string &value) {
    headers_[key] = value;
  }
  std::optional<std::string> getHeader(const std::string &key) const;

  void setBody(const std::string &body) { body_ = body; }
  const std::string &body() const { return body_; }
  void appendBody(const std::string &data) { body_ += data; }

  // 序列化成 HTTP 响应字符串
  std::string serialize() const;

private:
  StatusCode statusCode_;
  std::string statusMessage_;
  std::unordered_map<std::string, std::string> headers_;
  std::string body_;
};
} // namespace net
} // namespace Tupo