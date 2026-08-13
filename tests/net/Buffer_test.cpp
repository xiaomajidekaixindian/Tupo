#include <gtest/gtest.h>
#include "tupo/net/Buffer.h"
#include <string>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>

namespace Tupo{
namespace net{

class BufferTest : public ::testing::Test {
protected:
    void SetUp() override {
        buffer_ = std::make_unique<Buffer>();
    }

    void TearDown() override {
        buffer_.reset();
    }

    std::unique_ptr<Buffer> buffer_;
};

// 测试默认构造
TEST_F(BufferTest, DefaultConstructor) {
    EXPECT_EQ(buffer_->readableBytes(), 0);
    EXPECT_EQ(buffer_->writableBytes(), Buffer::kInitialSize);
    EXPECT_EQ(buffer_->prependableBytes(), Buffer::kCheapPrepend);
}

// 测试指定初始大小的构造
TEST_F(BufferTest, ConstructorWithSize) {
    Buffer customBuffer(1024);
    EXPECT_EQ(customBuffer.readableBytes(), 0);
    EXPECT_EQ(customBuffer.writableBytes(), 1024);
}

// 测试 append 和 readableBytes
TEST_F(BufferTest, AppendAndReadableBytes) {
    std::string data = "Hello, World!";
    buffer_->append(data);
    
    EXPECT_EQ(buffer_->readableBytes(), data.size());
    EXPECT_EQ(buffer_->writableBytes(), Buffer::kInitialSize - data.size());
    
    // 获取可读数据验证
    const char* peek = buffer_->readablePtr();
    EXPECT_EQ(std::string(peek, data.size()), data);
}

// 测试 append 多个数据
TEST_F(BufferTest, AppendMultiple) {
    buffer_->append("Hello", 5);
    buffer_->append(" World", 6);
    
    EXPECT_EQ(buffer_->readableBytes(), 11);
    EXPECT_EQ(std::string(buffer_->readablePtr(), 11), "Hello World");
}

// 测试 retrieve
TEST_F(BufferTest, Retrieve) {
    buffer_->append("Hello World", 11);
    
    buffer_->retrieve(6);
    EXPECT_EQ(buffer_->readableBytes(), 5);
    EXPECT_EQ(std::string(buffer_->readablePtr(), 5), "World");
    
    buffer_->retrieve(5);
    EXPECT_EQ(buffer_->readableBytes(), 0);
    EXPECT_EQ(buffer_->readablePtr(), buffer_->begin() + Buffer::kCheapPrepend);
}

// 测试 retrieveAll
TEST_F(BufferTest, RetrieveAll) {
    buffer_->append("Hello World", 11);
    buffer_->retrieveAll();
    
    EXPECT_EQ(buffer_->readableBytes(), 0);
    EXPECT_EQ(buffer_->writableBytes(), Buffer::kInitialSize);
}

// 测试 retrieve 超过可读数据
TEST_F(BufferTest, RetrieveMoreThanReadable) {
    buffer_->append("Hello", 5);
    buffer_->retrieve(10);
    
    EXPECT_EQ(buffer_->readableBytes(), 0);
}

// 测试自动扩容
TEST_F(BufferTest, AutoExpand) {
    size_t initialWritable = buffer_->writableBytes();
    std::string largeData(initialWritable + 100, 'A');
    buffer_->append(largeData);
    
    EXPECT_EQ(buffer_->readableBytes(), largeData.size());
    EXPECT_GT(buffer_->writableBytes(), 0);
    
    // 验证数据完整性
    EXPECT_EQ(std::string(buffer_->readablePtr(), largeData.size()), largeData);
}

// 测试 makeSpace 不扩容
TEST_F(BufferTest, MakeSpaceNoExpand) {
    size_t initialWritable = buffer_->writableBytes();
    size_t need = initialWritable / 2;
    
    buffer_->append(std::string(need, 'A'));
    EXPECT_EQ(buffer_->readableBytes(), need);
    EXPECT_EQ(buffer_->writableBytes(), initialWritable - need);
}

// 测试 makeSpace 复用前置空间
TEST_F(BufferTest, MakeSpaceReusePrepend) {
    // 写入数据并消费一部分，制造前置空间
    buffer_->append("Hello World", 11);
    buffer_->retrieve(6); // 消费 "Hello "
    
    size_t prependable = buffer_->prependableBytes();
    size_t writable = buffer_->writableBytes();
    size_t need = writable + 1; // 需要扩容
    
    // 但前置空间 + 可写空间足够
    std::string data(need, 'B');
    buffer_->append(data);
    
    // 验证数据正确性
    EXPECT_EQ(buffer_->readableBytes(), 5 + need);
    EXPECT_EQ(std::string(buffer_->readablePtr(), 5), "World");
}

// 测试 readFd
TEST_F(BufferTest, ReadFd) {
    int fds[2];
    ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, fds), 0);
    
    std::string message = "Hello from socket!";
    ssize_t sent = write(fds[1], message.c_str(), message.size());
    ASSERT_EQ(sent, static_cast<ssize_t>(message.size()));
    
    int savedErrno = 0;
    ssize_t n = buffer_->readFd(fds[0], &savedErrno);
    
    EXPECT_EQ(n, sent);
    EXPECT_EQ(buffer_->readableBytes(), sent);
    EXPECT_EQ(std::string(buffer_->readablePtr(), sent), message);
    
    close(fds[0]);
    close(fds[1]);
}

// 测试 readFd 读取大于缓冲区
TEST_F(BufferTest, ReadFdLargeData) {
    int fds[2];
    ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, fds), 0);
    
    // 生成大于缓冲区可写空间的数据
    size_t largeSize = buffer_->writableBytes() + 10000;
    std::string largeData(largeSize, 'X');
    ssize_t sent = write(fds[1], largeData.c_str(), largeData.size());
    ASSERT_EQ(sent, static_cast<ssize_t>(largeData.size()));
    
    int savedErrno = 0;
    ssize_t n = buffer_->readFd(fds[0], &savedErrno);
    
    EXPECT_EQ(n, sent);
    EXPECT_EQ(buffer_->readableBytes(), sent);
    EXPECT_EQ(std::string(buffer_->readablePtr(), sent), largeData);
    
    close(fds[0]);
    close(fds[1]);
}

// 测试 readFd 空读取
TEST_F(BufferTest, ReadFdEmpty) {
    int fds[2];
    socketpair(AF_UNIX, SOCK_STREAM, 0, fds);
    
    int flags = fcntl(fds[0], F_GETFL, 0);
    fcntl(fds[0], F_SETFL, flags | O_NONBLOCK);
    flags = fcntl(fds[1], F_GETFL, 0);
    fcntl(fds[1], F_SETFL, flags | O_NONBLOCK);
    
    Buffer buffer;
    int savedErrno = 0;
    ssize_t n = buffer.readFd(fds[0], &savedErrno);
    
    EXPECT_EQ(n, -1);
    EXPECT_EQ(savedErrno, EAGAIN);  // 或 EWOULDBLOCK
}

// 测试 append 空字符串
TEST_F(BufferTest, AppendEmptyString) {
    buffer_->append("");
    EXPECT_EQ(buffer_->readableBytes(), 0);
    
    std::string empty;
    buffer_->append(empty);
    EXPECT_EQ(buffer_->readableBytes(), 0);
}

// 测试 append 字符串
TEST_F(BufferTest, AppendString) {
    std::string str = "Test String";
    buffer_->append(str);
    
    EXPECT_EQ(buffer_->readableBytes(), str.size());
    EXPECT_EQ(std::string(buffer_->readablePtr(), str.size()), str);
}

// 测试多次 retrieve 和 append 混合操作
TEST_F(BufferTest, MixedOperations) {
    buffer_->append("First", 5);
    buffer_->retrieve(3);
    EXPECT_EQ(buffer_->readableBytes(), 2);
    EXPECT_EQ(std::string(buffer_->readablePtr(), 2), "st");
    
    buffer_->append("Second", 6);
    EXPECT_EQ(buffer_->readableBytes(), 8);
    EXPECT_EQ(std::string(buffer_->readablePtr(), 8), "stSecond");
    
    buffer_->retrieveAll();
    EXPECT_EQ(buffer_->readableBytes(), 0);
    
    buffer_->append("Third", 5);
    EXPECT_EQ(buffer_->readableBytes(), 5);
    EXPECT_EQ(std::string(buffer_->readablePtr(), 5), "Third");
}

// 测试 peek 和 begin 的一致性
TEST_F(BufferTest, PeekAndBegin) {
    buffer_->append("Test", 4);
    EXPECT_EQ(buffer_->readablePtr(), buffer_->begin() + Buffer::kCheapPrepend);
    
    buffer_->retrieve(2);
    EXPECT_EQ(buffer_->readablePtr(), buffer_->begin() + Buffer::kCheapPrepend + 2);
}

// 性能测试：大数据 append
TEST_F(BufferTest, LargeAppend) {
    const size_t largeSize = 1024 * 1024; // 1MB
    std::string largeData(largeSize, 'A');
    
    buffer_->append(largeData);
    EXPECT_EQ(buffer_->readableBytes(), largeSize);
    
    // 验证数据完整性（只验证首尾）
    EXPECT_EQ(buffer_->readablePtr()[0], 'A');
    EXPECT_EQ(buffer_->readablePtr()[largeSize - 1], 'A');
}

// 测试 makeSpace 的边界条件
TEST_F(BufferTest, MakeSpaceBoundary) {
    // 填充缓冲区到接近满
    size_t writable = buffer_->writableBytes();
    buffer_->append(std::string(writable, 'A'));
    EXPECT_EQ(buffer_->writableBytes(), 0);
    
    // 再添加数据，触发扩容
    buffer_->append("B", 1);
    EXPECT_GT(buffer_->readableBytes(), writable);
    EXPECT_GT(buffer_->writableBytes(), 0);
}

// 测试 readFd 错误处理
TEST_F(BufferTest, ReadFdError) {
    int invalidFd = -1;
    int savedErrno = 0;
    ssize_t n = buffer_->readFd(invalidFd, &savedErrno);
    
    EXPECT_LT(n, 0);
    EXPECT_EQ(savedErrno, EBADF);
}
}
}