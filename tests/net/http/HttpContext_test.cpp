#include "tupo/net/http/HttpContext.h"
#include <gtest/gtest.h>

namespace Tupo {
namespace net {
class HttpContextTest : public ::testing::Test {
protected:
  void SetUp() override { context.reset(); }

  HttpContext context;
  HttpRequest request;
};

//===============解析请求行测试=====================

/**
 * 测试目的：验证正确解析标准 GET 请求行
 * 输入：GET /index.html HTTP/1.1\r\n
 * 期望：方法=GET，路径=/index.html，版本=HTTP/1.1
 */
TEST_F(HttpContextTest, ProcessRequestLine_ValidGet) {
  const char *data = "GET /index.html HTTP/1.1\r\n";
  const char *begin = data;
  const char *end = data + strlen(data);
  const char *crlf = std::search(begin, end, "\r\n", "\r\n" + 2);
  ASSERT_NE(crlf, end);

  bool result = context.processRequestLine(begin, crlf, &request);

  EXPECT_TRUE(result);
  EXPECT_EQ(request.getMethod(), HttpRequest::kGet);
  EXPECT_EQ(request.path(), "/index.html");
  EXPECT_EQ(request.version(), "HTTP/1.1");
}

/**
 * 测试目的：验证 POST 请求行
 * 输入：POST /api/users HTTP/1.1\r\n
 * 期望：方法=POST，路径=/api/users，版本=HTTP/1.1
 */
TEST_F(HttpContextTest, ProcessRequestLine_ValidPost) {
  const char *data = "POST /api/users HTTP/1.1\r\n";
  const char *begin = data;
  const char *end = data + strlen(data);
  const char *crlf = std::search(begin, end, "\r\n", "\r\n" + 2);

  bool result = context.processRequestLine(begin, crlf, &request);

  EXPECT_TRUE(result);
  EXPECT_EQ(request.getMethod(), HttpRequest::kPost);
  EXPECT_EQ(request.path(), "/api/users");
  EXPECT_EQ(request.version(), "HTTP/1.1");
}

/**
 * 测试目的：验证 PUT 请求行
 * 输入：PUT /api/users/1 HTTP/1.1\r\n
 * 期望：方法=PUT，路径=/api/users/1，版本=HTTP/1.1
 */
TEST_F(HttpContextTest, ProcessRequestLine_ValidPut) {
  const char *data = "PUT /api/users/1 HTTP/1.1\r\n";
  const char *begin = data;
  const char *end = data + strlen(data);
  const char *crlf = std::search(begin, end, "\r\n", "\r\n" + 2);

  bool result = context.processRequestLine(begin, crlf, &request);

  EXPECT_TRUE(result);
  EXPECT_EQ(request.getMethod(), HttpRequest::kPut);
  EXPECT_EQ(request.path(), "/api/users/1");
  EXPECT_EQ(request.version(), "HTTP/1.1");
}

/**
 * 测试目的：验证 DELETE 请求行
 * 输入：DELETE /api/users/1 HTTP/1.1\r\n
 * 期望：方法=DELETE，路径=/api/users/1，版本=HTTP/1.1
 */
TEST_F(HttpContextTest, ProcessRequestLine_ValidDelete) {
  const char *data = "DELETE /api/users/1 HTTP/1.1\r\n";
  const char *begin = data;
  const char *end = data + strlen(data);
  const char *crlf = std::search(begin, end, "\r\n", "\r\n" + 2);

  bool result = context.processRequestLine(begin, crlf, &request);

  EXPECT_TRUE(result);
  EXPECT_EQ(request.getMethod(), HttpRequest::kDelete);
  EXPECT_EQ(request.path(), "/api/users/1");
  EXPECT_EQ(request.version(), "HTTP/1.1");
}

/**
 * 测试目的：验证 HEAD 请求行
 * 输入：HEAD /index.html HTTP/1.1\r\n
 * 期望：方法=HEAD，路径=/index.html，版本=HTTP/1.1
 */
TEST_F(HttpContextTest, ProcessRequestLine_ValidHead) {
  const char *data = "HEAD /index.html HTTP/1.1\r\n";
  const char *begin = data;
  const char *end = data + strlen(data);
  const char *crlf = std::search(begin, end, "\r\n", "\r\n" + 2);

  bool result = context.processRequestLine(begin, crlf, &request);

  EXPECT_TRUE(result);
  EXPECT_EQ(request.getMethod(), HttpRequest::kHead);
  EXPECT_EQ(request.path(), "/index.html");
  EXPECT_EQ(request.version(), "HTTP/1.1");
}

/**
 * 测试目的：验证带查询参数的 URL
 * 输入：GET /api/users?id=1&name=test HTTP/1.1\r\n
 * 期望：完整保留查询参数
 */
TEST_F(HttpContextTest, ProcessRequestLine_WithQueryParams) {
  const char *data = "GET /api/users?id=1&name=test HTTP/1.1\r\n";
  const char *begin = data;
  const char *end = data + strlen(data);
  const char *crlf = std::search(begin, end, "\r\n", "\r\n" + 2);

  bool result = context.processRequestLine(begin, crlf, &request);

  EXPECT_TRUE(result);
  EXPECT_EQ(request.getMethod(), HttpRequest::kGet);
  EXPECT_EQ(request.path(), "/api/users?id=1&name=test");
  EXPECT_EQ(request.version(), "HTTP/1.1");
}

/**
 * 测试目的：验证 HTTP/1.0 请求
 * 输入：GET /index.html HTTP/1.0\r\n
 * 期望：版本=HTTP/1.0
 */
TEST_F(HttpContextTest, ProcessRequestLine_HTTP10) {
  const char *data = "GET /index.html HTTP/1.0\r\n";
  const char *begin = data;
  const char *end = data + strlen(data);
  const char *crlf = std::search(begin, end, "\r\n", "\r\n" + 2);

  bool result = context.processRequestLine(begin, crlf, &request);

  EXPECT_TRUE(result);
  EXPECT_EQ(request.getMethod(), HttpRequest::kGet);
  EXPECT_EQ(request.path(), "/index.html");
  EXPECT_EQ(request.version(), "HTTP/1.0");
}

/**
 * 测试目的：验证请求行缺少空格时解析失败
 * 输入：GET/index.htmlHTTP/1.1（没有空格分隔）
 * 期望：返回 false
 */
TEST_F(HttpContextTest, ProcessRequestLine_NoSpaces) {
  const char *data = "GET/index.htmlHTTP/1.1\r\n";
  const char *begin = data;
  const char *end = data + strlen(data);
  const char *crlf = std::search(begin, end, "\r\n", "\r\n" + 2);

  bool result = context.processRequestLine(begin, crlf, &request);
  EXPECT_FALSE(result);
}

/**
 * 测试目的：验证只有一个空格时解析失败
 * 输入：GET /index.htmlHTTP/1.1（缺少第二个空格）
 * 期望：返回 false
 */
TEST_F(HttpContextTest, ProcessRequestLine_OnlyOneSpace) {
  const char *data = "GET /index.htmlHTTP/1.1\r\n";
  const char *begin = data;
  const char *end = data + strlen(data);
  const char *crlf = std::search(begin, end, "\r\n", "\r\n" + 2);

  bool result = context.processRequestLine(begin, crlf, &request);
  EXPECT_FALSE(result);
}

/**
 * 测试目的：验证未知方法被拒绝
 * 输入：UNKNOWN /index.html HTTP/1.1\r\n
 * 期望：返回 false
 */
TEST_F(HttpContextTest, ProcessRequestLine_UnknownMethod) {
  const char *data = "UNKNOWN /index.html HTTP/1.1\r\n";
  const char *begin = data;
  const char *end = data + strlen(data);
  const char *crlf = std::search(begin, end, "\r\n", "\r\n" + 2);

  bool result = context.processRequestLine(begin, crlf, &request);
  EXPECT_FALSE(result);
}

/**
 * 测试目的：验证空请求行被拒绝
 * 输入：\r\n
 * 期望：返回 false
 */
TEST_F(HttpContextTest, ProcessRequestLine_EmptyLine) {
  const char *data = "\r\n";
  const char *begin = data;
  const char *end = data + strlen(data);
  const char *crlf = std::search(begin, end, "\r\n", "\r\n" + 2);

  bool result = context.processRequestLine(begin, crlf, &request);
  EXPECT_FALSE(result);
}

// ============ processHeaders 测试 ============

/**
 * 测试目的：验证正确解析标准 Host 请求头
 * 输入：Host: localhost:8080\r\n
 * 期望：保存为 key=Host, value=localhost:8080
 */
TEST_F(HttpContextTest, ProcessHeaders_ValidHost) {
  const char *data = "Host: localhost:8080\r\n";
  const char *begin = data;
  const char *end = data + strlen(data);
  const char *crlf = std::search(begin, end, "\r\n", "\r\n" + 2);

  bool result = context.processHeaders(begin, crlf, &request);

  EXPECT_TRUE(result);
  EXPECT_EQ(request.getHeader("Host"), "localhost:8080");
}

/**
 * 测试目的：验证 Content-Type 请求头
 * 输入：Content-Type: application/json\r\n
 * 期望：保存为 key=Content-Type, value=application/json
 */
TEST_F(HttpContextTest, ProcessHeaders_ValidContentType) {
  const char *data = "Content-Type: application/json\r\n";
  const char *begin = data;
  const char *end = data + strlen(data);
  const char *crlf = std::search(begin, end, "\r\n", "\r\n" + 2);

  bool result = context.processHeaders(begin, crlf, &request);

  EXPECT_TRUE(result);
  EXPECT_EQ(request.getHeader("Content-Type"), "application/json");
}

/**
 * 测试目的：验证 Content-Length 请求头
 * 输入：Content-Length: 1024\r\n
 * 期望：保存为 key=Content-Length, value=1024
 */
TEST_F(HttpContextTest, ProcessHeaders_ValidContentLength) {
  const char *data = "Content-Length: 1024\r\n";
  const char *begin = data;
  const char *end = data + strlen(data);
  const char *crlf = std::search(begin, end, "\r\n", "\r\n" + 2);

  bool result = context.processHeaders(begin, crlf, &request);

  EXPECT_TRUE(result);
  EXPECT_EQ(request.getHeader("Content-Length"), "1024");
}

/**
 * 测试目的：验证 User-Agent 请求头
 * 输入：User-Agent: Mozilla/5.0\r\n
 * 期望：保存为 key=User-Agent, value=Mozilla/5.0
 */
TEST_F(HttpContextTest, ProcessHeaders_ValidUserAgent) {
  const char *data = "User-Agent: Mozilla/5.0\r\n";
  const char *begin = data;
  const char *end = data + strlen(data);
  const char *crlf = std::search(begin, end, "\r\n", "\r\n" + 2);

  bool result = context.processHeaders(begin, crlf, &request);

  EXPECT_TRUE(result);
  EXPECT_EQ(request.getHeader("User-Agent"), "Mozilla/5.0");
}

/**
 * 测试目的：验证 Accept 请求头
 * 输入：Accept: text/html,application/xhtml+xml\r\n
 * 期望：保存为 key=Accept, value=text/html,application/xhtml+xml
 */
TEST_F(HttpContextTest, ProcessHeaders_ValidAccept) {
  const char *data = "Accept: text/html,application/xhtml+xml\r\n";
  const char *begin = data;
  const char *end = data + strlen(data);
  const char *crlf = std::search(begin, end, "\r\n", "\r\n" + 2);

  bool result = context.processHeaders(begin, crlf, &request);

  EXPECT_TRUE(result);
  EXPECT_EQ(request.getHeader("Accept"), "text/html,application/xhtml+xml");
}

/**
 * 测试目的：验证头部值去除首尾空格
 * 输入：User-Agent:   Mozilla/5.0   \r\n
 * 期望：保存为 User-Agent: Mozilla/5.0（去除空格）
 */
TEST_F(HttpContextTest, ProcessHeaders_WithSpaces) {
  const char *data = "User-Agent:   Mozilla/5.0   \r\n";
  const char *begin = data;
  const char *end = data + strlen(data);
  const char *crlf = std::search(begin, end, "\r\n", "\r\n" + 2);

  bool result = context.processHeaders(begin, crlf, &request);

  EXPECT_TRUE(result);
  EXPECT_EQ(request.getHeader("User-Agent"), "Mozilla/5.0");
}

/**
 * 测试目的：验证 Tab 字符被去除
 * 输入：Accept:\ttext/html\t\r\n
 * 期望：保存为 Accept: text/html（去除 Tab）
 */
TEST_F(HttpContextTest, ProcessHeaders_WithTabs) {
  const char *data = "Accept:\ttext/html\t\r\n";
  const char *begin = data;
  const char *end = data + strlen(data);
  const char *crlf = std::search(begin, end, "\r\n", "\r\n" + 2);

  bool result = context.processHeaders(begin, crlf, &request);

  EXPECT_TRUE(result);
  EXPECT_EQ(request.getHeader("Accept"), "text/html");
}

/**
 * 测试目的：验证头部名称包含连字符
 * 输入：Content-Encoding: gzip\r\n
 * 期望：保存为 key=Content-Encoding, value=gzip
 */
TEST_F(HttpContextTest, ProcessHeaders_KeyWithDash) {
  const char *data = "Content-Encoding: gzip\r\n";
  const char *begin = data;
  const char *end = data + strlen(data);
  const char *crlf = std::search(begin, end, "\r\n", "\r\n" + 2);

  bool result = context.processHeaders(begin, crlf, &request);

  EXPECT_TRUE(result);
  EXPECT_EQ(request.getHeader("Content-Encoding"), "gzip");
}

/**
 * 测试目的：验证头部名称包含下划线
 * 输入：X_Custom_Header: custom_value\r\n
 * 期望：保存为 key=X_Custom_Header, value=custom_value
 */
TEST_F(HttpContextTest, ProcessHeaders_KeyWithUnderscore) {
  const char *data = "X_Custom_Header: custom_value\r\n";
  const char *begin = data;
  const char *end = data + strlen(data);
  const char *crlf = std::search(begin, end, "\r\n", "\r\n" + 2);

  bool result = context.processHeaders(begin, crlf, &request);

  EXPECT_TRUE(result);
  EXPECT_EQ(request.getHeader("X_Custom_Header"), "custom_value");
}

/**
 * 测试目的：验证只有冒号没有值的头部
 * 输入：Header:\r\n
 * 期望：保存为 Header: ""（空值）
 */
TEST_F(HttpContextTest, ProcessHeaders_OnlyColonNoValue) {
  const char *data = "Header:\r\n";
  const char *begin = data;
  const char *end = data + strlen(data);
  const char *crlf = std::search(begin, end, "\r\n", "\r\n" + 2);

  bool result = context.processHeaders(begin, crlf, &request);

  EXPECT_TRUE(result);
  EXPECT_EQ(request.getHeader("Header"), "");
}

/**
 * 测试目的：验证多个冒号的情况
 * 输入：Header: value: with: colons\r\n
 * 期望：只找第一个冒号，后面的冒号作为值的一部分
 */
TEST_F(HttpContextTest, ProcessHeaders_MultipleColons) {
  const char *data = "Header: value: with: colons\r\n";
  const char *begin = data;
  const char *end = data + strlen(data);
  const char *crlf = std::search(begin, end, "\r\n", "\r\n" + 2);

  bool result = context.processHeaders(begin, crlf, &request);

  EXPECT_TRUE(result);
  EXPECT_EQ(request.getHeader("Header"), "value: with: colons");
}

/**
 * 测试目的：验证没有冒号的头部被拒绝
 * 输入：InvalidHeaderWithoutColon\r\n
 * 期望：返回 false
 */
TEST_F(HttpContextTest, ProcessHeaders_NoColon) {
  const char *data = "InvalidHeaderWithoutColon\r\n";
  const char *begin = data;
  const char *end = data + strlen(data);
  const char *crlf = std::search(begin, end, "\r\n", "\r\n" + 2);

  bool result = context.processHeaders(begin, crlf, &request);
  EXPECT_FALSE(result);
}

/**
 * 测试目的：验证空请求头行被拒绝
 * 输入：\r\n
 * 期望：返回 false（空行不应被 processHeaders 处理）
 */
TEST_F(HttpContextTest, ProcessHeaders_EmptyLine) {
  const char *data = "\r\n";
  const char *begin = data;
  const char *end = data + strlen(data);
  const char *crlf = std::search(begin, end, "\r\n", "\r\n" + 2);

  bool result = context.processHeaders(begin, crlf, &request);
  EXPECT_FALSE(result);
}

// ============ processBody 测试 ============

/**
 * 测试目的：验证正确解析完整的请求体
 * 输入：begin 指向 body 开始，end 指向 body 结束，Content-Length = 10
 * 期望：body 被正确设置
 */
TEST_F(HttpContextTest, ProcessBody_CompleteBody) {
  // 先设置 Content-Length
  request.setHeader("Content-Length", "10");
  request.setContentLength(10);

  const char *data = "1234567890";
  const char *begin = data;
  const char *end = data + strlen(data);

  bool result = context.processBody(begin, end, &request);

  EXPECT_TRUE(result);
  EXPECT_EQ(request.body(), "1234567890");
  EXPECT_EQ(request.body().size(), 10);
}

/**
 * 测试目的：验证 JSON 格式的请求体
 * 输入：{"name":"Alice","age":25}
 * 期望：完整保留 JSON 字符串
 */
TEST_F(HttpContextTest, ProcessBody_JsonBody) {
  request.setHeader("Content-Length", "25");
  request.setContentLength(25);

  const char *data = "{\"name\":\"Alice\",\"age\":25}";
  const char *begin = data;
  const char *end = data + strlen(data);

  bool result = context.processBody(begin, end, &request);

  EXPECT_TRUE(result);
  EXPECT_EQ(request.body(), "{\"name\":\"Alice\",\"age\":25}");
}

/**
 * 测试目的：验证 Content-Length = 0 时没有 body
 * 输入：Content-Length = 0
 * 期望：直接返回 true，body 为空
 */
TEST_F(HttpContextTest, ProcessBody_ZeroContentLength) {
  request.setHeader("Content-Length", "0");
  request.setContentLength(0);

  const char *data = "";
  const char *begin = data;
  const char *end = data;

  bool result = context.processBody(begin, end, &request);

  EXPECT_TRUE(result);
  EXPECT_EQ(request.body(), "");
}

/**
 * 测试目的：验证数据不够时返回 false
 * 输入：Content-Length = 100，但只有 50 字节数据
 * 期望：返回 false，body 为空
 */
TEST_F(HttpContextTest, `) {
  request.setHeader("Content-Length", "100");
  request.setContentLength(100);

  const char *data = "1234567890"; // 只有 10 字节
  const char *begin = data;
  const char *end = data + strlen(data);

  bool result = context.processBody(begin, end, &request);

  EXPECT_FALSE(result);
  EXPECT_EQ(request.body(), ""); // body 没有被设置
}

/**
 * 测试目的：验证数据正好等于 Content-Length
 * 输入：Content-Length = 10，数据正好 10 字节
 * 期望：返回 true，body 完整
 */
TEST_F(HttpContextTest, ProcessBody_ExactContentLength) {
  request.setHeader("Content-Length", "10");
  request.setContentLength(10);

  const char *data = "1234567890";
  const char *begin = data;
  const char *end = data + strlen(data);

  bool result = context.processBody(begin, end, &request);

  EXPECT_TRUE(result);
  EXPECT_EQ(request.body(), "1234567890");
  EXPECT_EQ(request.body().size(), 10);
}

/**
 * 测试目的：验证数据大于 Content-Length（可能包含后续请求数据）
 * 输入：Content-Length = 10，数据有 20 字节
 * 期望：只读取 10 字节，剩余数据保留
 */
TEST_F(HttpContextTest, ProcessBody_MoreDataThanContentLength) {
  request.setHeader("Content-Length", "10");
  request.setContentLength(10);

  const char *data = "1234567890ABCDEFGHIJ"; // 20 字节
  const char *begin = data;
  const char *end = data + strlen(data);

  bool result = context.processBody(begin, end, &request);

  EXPECT_TRUE(result);
  EXPECT_EQ(request.body(), "1234567890");
  EXPECT_EQ(request.body().size(), 10);
}

/**
 * 测试目的：验证包含空格的 body
 * 输入：Content-Length = 11，body = "Hello World"
 * 期望：完整保留空格
 */
TEST_F(HttpContextTest, ProcessBody_WithSpaces) {
  request.setHeader("Content-Length", "11");
  request.setContentLength(11);

  const char *data = "Hello World";
  const char *begin = data;
  const char *end = data + strlen(data);

  bool result = context.processBody(begin, end, &request);

  EXPECT_TRUE(result);
  EXPECT_EQ(request.body(), "Hello World");
  EXPECT_EQ(request.body().size(), 11);
}

/**
 * 测试目的：验证包含换行符的 body
 * 输入：Content-Length = 11，body = "Hello\nWorld"
 * 期望：完整保留换行符
 */
TEST_F(HttpContextTest, ProcessBody_WithNewline) {
  request.setHeader("Content-Length", "11");
  request.setContentLength(11);

  const char *data = "Hello\nWorld";
  const char *begin = data;
  const char *end = data + strlen(data);

  bool result = context.processBody(begin, end, &request);

  EXPECT_TRUE(result);
  EXPECT_EQ(request.body(), "Hello\nWorld");
  EXPECT_EQ(request.body().size(), 11);
}

/**
 * 测试目的：验证中文字符的 body（UTF-8 编码）
 * 输入：Content-Length = 6，body = "你好"
 * 期望：完整保留中文字符
 */
TEST_F(HttpContextTest, ProcessBody_ChineseCharacters) {
  request.setHeader("Content-Length", "6");
  request.setContentLength(6);

  const char *data = "你好"; // UTF-8 编码下 6 字节
  const char *begin = data;
  const char *end = data + strlen(data);

  bool result = context.processBody(begin, end, &request);

  EXPECT_TRUE(result);
  EXPECT_EQ(request.body(), "你好");
  EXPECT_EQ(request.body().size(), 6);
}

/**
 * 测试目的：验证空 body（空字符串）
 * 输入：Content-Length = 0，body = ""
 * 期望：返回 true，body 为空
 */
TEST_F(HttpContextTest, ProcessBody_EmptyBody) {
  request.setHeader("Content-Length", "0");
  request.setContentLength(0);

  const char *data = "";
  const char *begin = data;
  const char *end = data;

  bool result = context.processBody(begin, end, &request);

  EXPECT_TRUE(result);
  EXPECT_EQ(request.body(), "");
}

/**
 * 测试目的：验证二进制数据（包含 \0）
 * 输入：Content-Length = 5，body 包含 \0
 * 期望：完整保留二进制数据
 */
TEST_F(HttpContextTest, ProcessBody_BinaryData) {
  request.setHeader("Content-Length", "5");
  request.setContentLength(5);

  // 包含 \0 的二进制数据
  char data[5] = {'H', 'e', '\0', 'l', 'o'};
  const char *begin = data;
  const char *end = data + 5;

  bool result = context.processBody(begin, end, &request);

  EXPECT_TRUE(result);
  EXPECT_EQ(request.body().size(), 5);
  EXPECT_EQ(request.body()[0], 'H');
  EXPECT_EQ(request.body()[1], 'e');
  EXPECT_EQ(request.body()[2], '\0');
  EXPECT_EQ(request.body()[3], 'l');
  EXPECT_EQ(request.body()[4], 'o');
}

/**
 * 测试目的：验证没有设置 Content-Length 时直接完成
 * 输入：Content-Length 不存在（返回 0）
 * 期望：返回 true，body 为空
 */
TEST_F(HttpContextTest, ProcessBody_NoContentLength) {
  // 不设置 Content-Length，contentLength() 返回 0

  const char *data = "some data";
  const char *begin = data;
  const char *end = data + strlen(data);

  bool result = context.processBody(begin, end, &request);

  EXPECT_TRUE(result);
  EXPECT_EQ(request.body(), "");
}

/**
 * 测试目的：验证多次调用 processBody 的增量解析（数据不够时）
 * 场景：body 分两次到达
 * 第一次：只有部分数据，返回 false
 * 第二次：数据完整，返回 true
 */
TEST_F(HttpContextTest, ProcessBody_IncrementalParsing) {
  request.setHeader("Content-Length", "20");
  request.setContentLength(20);

  // 第一次：只有 10 字节
  const char *data1 = "1234567890";
  const char *begin1 = data1;
  const char *end1 = data1 + strlen(data1);

  bool result1 = context.processBody(begin1, end1, &request);
  EXPECT_FALSE(result1);
  EXPECT_EQ(request.body(), "");

  // 第二次：又有 10 字节
  const char *data2 = "1234567890";
  const char *begin2 = data2;
  const char *end2 = data2 + strlen(data2);

  bool result2 = context.processBody(begin2, end2, &request);
  EXPECT_TRUE(result2);
  EXPECT_EQ(request.body(), "1234567890");
}
} // namespace net
} // namespace Tupo