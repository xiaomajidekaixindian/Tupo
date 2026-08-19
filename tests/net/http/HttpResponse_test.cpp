#include <gtest/gtest.h>
#include "tupo/net/http/HttpResponse.h"

using namespace Tupo::net;

class HttpResponseTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 每个测试前执行
    }

    void TearDown() override {
        // 每个测试后执行
    }
};

// ========== 测试 1：默认构造 ==========
TEST_F(HttpResponseTest, DefaultConstructor) {
    HttpResponse resp;
    
    EXPECT_EQ(resp.statusCode(), HttpResponse::k200Ok);
    EXPECT_TRUE(resp.statusMessage().empty());
    EXPECT_TRUE(resp.body().empty());
}

// ========== 测试 2：设置状态码 ==========
TEST_F(HttpResponseTest, SetStatusCode) {
    HttpResponse resp;
    
    resp.setStatusCode(HttpResponse::k404NotFound);
    EXPECT_EQ(resp.statusCode(), HttpResponse::k404NotFound);
    
    resp.setStatusCode(HttpResponse::k500InternalServerError);
    EXPECT_EQ(resp.statusCode(), HttpResponse::k500InternalServerError);
}

// ========== 测试 3：设置状态消息 ==========
TEST_F(HttpResponseTest, SetStatusMessage) {
    HttpResponse resp;
    
    resp.setStatusMessage("OK");
    EXPECT_EQ(resp.statusMessage(), "OK");
    
    resp.setStatusMessage("Not Found");
    EXPECT_EQ(resp.statusMessage(), "Not Found");
}

// ========== 测试 4：设置响应头 ==========
TEST_F(HttpResponseTest, SetAndGetHeader) {
    HttpResponse resp;
    
    resp.setHeader("Content-Type", "text/html");
    EXPECT_EQ(resp.getHeader("Content-Type"), "text/html");
    
    resp.setHeader("Content-Length", "1024");
    EXPECT_EQ(resp.getHeader("Content-Length"), "1024");
    
    // 测试不存在的 Header
    EXPECT_EQ(resp.getHeader("Not-Exist"), std::nullopt);
}

// ========== 测试 5：设置消息体 ==========
TEST_F(HttpResponseTest, SetBody) {
    HttpResponse resp;
    
    resp.setBody("<html><body>Hello</body></html>");
    EXPECT_EQ(resp.body(), "<html><body>Hello</body></html>");
    
    resp.setBody("");
    EXPECT_TRUE(resp.body().empty());
}

// ========== 测试 6：追加消息体 ==========
TEST_F(HttpResponseTest, AppendBody) {
    HttpResponse resp;
    
    resp.appendBody("Hello");
    EXPECT_EQ(resp.body(), "Hello");
    
    resp.appendBody(" World");
    EXPECT_EQ(resp.body(), "Hello World");
}

// ========== 测试 7：序列化 - 200 OK ==========
TEST_F(HttpResponseTest, Serialize200OK) {
    HttpResponse resp;
    resp.setStatusCode(HttpResponse::k200Ok);
    resp.setBody("Hello, World!");
    
    std::string serialized = resp.serialize();
    
    // 验证状态行
    EXPECT_TRUE(serialized.find("HTTP/1.1 200 OK") != std::string::npos);
    
    // 验证 Content-Length 自动添加
    EXPECT_TRUE(serialized.find("Content-Length: 13") != std::string::npos);
    
    // 验证消息体
    EXPECT_TRUE(serialized.find("Hello, World!") != std::string::npos);
}

// ========== 测试 8：序列化 - 404 Not Found ==========
TEST_F(HttpResponseTest, Serialize404NotFound) {
    HttpResponse resp;
    resp.setStatusCode(HttpResponse::k404NotFound);
    resp.setBody("<html><body><h1>404</h1></body></html>");
    resp.setHeader("Content-Type", "text/html");
    
    std::string serialized = resp.serialize();
    
    // 验证状态行
    EXPECT_TRUE(serialized.find("HTTP/1.1 404 Not Found") != std::string::npos);
    
    // 验证自定义 Header
    EXPECT_TRUE(serialized.find("Content-Type: text/html") != std::string::npos);
}

// ========== 测试 9：序列化 - 自定义状态消息 ==========
TEST_F(HttpResponseTest, SerializeCustomStatusMessage) {
    HttpResponse resp;
    resp.setStatusCode(HttpResponse::k200Ok);
    resp.setStatusMessage("Custom OK Message");
    resp.setBody("Custom Body");
    
    std::string serialized = resp.serialize();
    
    EXPECT_TRUE(serialized.find("HTTP/1.1 200 Custom OK Message") != std::string::npos);
    EXPECT_TRUE(serialized.find("Custom Body") != std::string::npos);
}

// ========== 测试 10：序列化 - 空消息体 ==========
TEST_F(HttpResponseTest, SerializeEmptyBody) {
    HttpResponse resp;
    resp.setStatusCode(HttpResponse::k200Ok);
    
    std::string serialized = resp.serialize();
    
    // 不应该有 Content-Length
    EXPECT_TRUE(serialized.find("Content-Length: 0") == std::string::npos);
    
    // 应该以 \r\n\r\n 结尾
    EXPECT_TRUE(serialized.find("\r\n\r\n") != std::string::npos);
}

// ========== 测试 11：序列化 - 多个 Header ==========
TEST_F(HttpResponseTest, SerializeMultipleHeaders) {
    HttpResponse resp;
    resp.setStatusCode(HttpResponse::k200Ok);
    resp.setHeader("Content-Type", "application/json");
    resp.setHeader("Cache-Control", "no-cache");
    resp.setHeader("X-Custom-Header", "custom-value");
    resp.setBody("{\"status\":\"ok\"}");
    
    std::string serialized = resp.serialize();
    
    EXPECT_TRUE(serialized.find("Content-Type: application/json") != std::string::npos);
    EXPECT_TRUE(serialized.find("Cache-Control: no-cache") != std::string::npos);
    EXPECT_TRUE(serialized.find("X-Custom-Header: custom-value") != std::string::npos);
}

// ========== 测试 12：状态码对应的默认状态消息 ==========
TEST_F(HttpResponseTest, DefaultStatusMessage) {
    HttpResponse resp;
    
    // 200
    resp.setStatusCode(HttpResponse::k200Ok);
    std::string s = resp.serialize();
    EXPECT_TRUE(s.find("200 OK") != std::string::npos);
    
    // 301
    resp.setStatusCode(HttpResponse::k301MovedPermanently);
    s = resp.serialize();
    EXPECT_TRUE(s.find("301 Moved Permanently") != std::string::npos);
    
    // 400
    resp.setStatusCode(HttpResponse::k400BadRequest);
    s = resp.serialize();
    EXPECT_TRUE(s.find("400 Bad Request") != std::string::npos);
    
    // 404
    resp.setStatusCode(HttpResponse::k404NotFound);
    s = resp.serialize();
    EXPECT_TRUE(s.find("404 Not Found") != std::string::npos);
    
    // 500
    resp.setStatusCode(HttpResponse::k500InternalServerError);
    s = resp.serialize();
    EXPECT_TRUE(s.find("500 Internal Server Error") != std::string::npos);
}

// ========== 测试 13：协议格式完整性 ==========
TEST_F(HttpResponseTest, ProtocolFormatIntegrity) {
    HttpResponse resp;
    resp.setStatusCode(HttpResponse::k200Ok);
    resp.setHeader("Content-Type", "text/plain");
    resp.setBody("Hello");
    
    std::string s = resp.serialize();
    
    // 验证格式：状态行 \r\n Headers \r\n \r\n Body
    size_t pos = s.find("\r\n");
    ASSERT_NE(pos, std::string::npos);
    
    // 验证头结束符
    size_t headerEnd = s.find("\r\n\r\n");
    ASSERT_NE(headerEnd, std::string::npos);
    
    // 验证 Body 在最后
    EXPECT_EQ(s.substr(s.size() - 5), "Hello");
}

// ========== 测试 14：Connection 默认值 ==========
TEST_F(HttpResponseTest, DefaultConnectionHeader) {
    HttpResponse resp;
    resp.setStatusCode(HttpResponse::k200Ok);
    resp.setBody("Hello");
    
    std::string s = resp.serialize();
    
    // 默认应该添加 Connection: close
    EXPECT_TRUE(s.find("Connection: close") != std::string::npos);
}

// ========== 测试 15：覆盖默认 Connection ==========
TEST_F(HttpResponseTest, OverrideConnectionHeader) {
    HttpResponse resp;
    resp.setStatusCode(HttpResponse::k200Ok);
    resp.setHeader("Connection", "keep-alive");
    resp.setBody("Hello");
    
    std::string s = resp.serialize();
    
    EXPECT_TRUE(s.find("Connection: keep-alive") != std::string::npos);
    EXPECT_FALSE(s.find("Connection: close") != std::string::npos);
}

// ========== 测试 16：只读操作 ==========
TEST_F(HttpResponseTest, ReadOnlyOperations) {
    HttpResponse resp;
    resp.setStatusCode(HttpResponse::k200Ok);
    resp.setHeader("Content-Type", "text/html");
    resp.setBody("Hello");
    
    const HttpResponse& constResp = resp;
    
    EXPECT_EQ(constResp.statusCode(), HttpResponse::k200Ok);
    EXPECT_EQ(constResp.getHeader("Content-Type"), "text/html");
    EXPECT_EQ(constResp.body(), "Hello");
}

// ========== 测试 17：性能 - 序列化大消息体 ==========
TEST_F(HttpResponseTest, SerializeLargeBody) {
    HttpResponse resp;
    resp.setStatusCode(HttpResponse::k200Ok);
    resp.setHeader("Content-Type", "text/plain");
    
    // 构造 1MB 数据
    std::string largeBody(1024 * 1024, 'A');
    resp.setBody(largeBody);
    
    std::string serialized = resp.serialize();
    
    // 验证大小：body + headers
    EXPECT_GT(serialized.size(), largeBody.size());
    EXPECT_TRUE(serialized.find(largeBody) != std::string::npos);
}

// ========== 测试 18：空响应（只有状态行） ==========
TEST_F(HttpResponseTest, EmptyResponse) {
    HttpResponse resp;
    resp.setStatusCode(HttpResponse::k200Ok);
    
    std::string s = resp.serialize();
    
    // 应该有状态行和空行
    EXPECT_TRUE(s.find("HTTP/1.1 200 OK") != std::string::npos);
    EXPECT_TRUE(s.find("\r\n\r\n") != std::string::npos);
}