#pragma once
#include <cstddef>
#include <string>
#include <unordered_map>
namespace Tupo {
namespace net {
class HttpRequest {
public:
  // 请求方式
  enum Method {
    kInvalid, // 无效/未知
    kGet,     // 获取资源
    kPost,    // 创建资源
    kPut,     // 全量修改
    kDelete,  // 删除资源
    kHead,    // 仅获取请求头
  };
  // 解析状态
  enum ParseState {
    kExpectRequestLine, // 解析请求行
    kExpectHeaders,     // 正在解析请求头
    kExpectBody,        // 正在解析消息体
    kGotAll             // 全部解析完成
  };
  HttpRequest();

  // 设置/获取方法
  void setMethod(Method method) { method_ = method; }
  Method getMethod() { return method_; }
  const char *methodString() const;

  // 设置/获取解析状态
  void setState(ParseState state) { parseState_ = state; }
  ParseState getState() { return parseState_; }

  // 设置/获取请求头
  void setHeader(const std::string &key, const std::string &value) {
    headers_[key] = value;
  }
  std::string getHeader(const std::string &key) const;
  const std::unordered_map<std::string, std::string> &headers() const {
    return headers_;
  }

  // 设置/获取路径
  void setPath(const std::string &path) { path_ = path; }
  const std::string &path() const { return path_; }

  // 设置/获取版本
  void setVersion(const std::string &version) { version_ = version; }
  const std::string &version() const { return version_; }

  // 设置/获取消息体
  void setBody(const std::string &body) { body_ = body; }
  const std::string &body() const { return body_; }
  void appendBody(const std::string &data) { body_ += data; }

  // 获取 Content-Length
  size_t contentLength() const;

  
private:
  Method method_;
  ParseState parseState_;
  std::unordered_map<std::string, std::string> headers_;
  std::string path_;
  std::string version_;
  std::string body_;

#ifdef DEBUG
public:
  size_t contentLength_;
  void setContentLength(size_t contentLength){
    contentLength_ = contentLength;
  }
#endif
};
} // namespace net
} // namespace Tupo