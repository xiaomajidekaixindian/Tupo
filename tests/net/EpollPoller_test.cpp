#include "tupo/net/Channel.h"
#include "tupo/net/EventLoop.h"
#include "tupo/net/poller/EpollPoller.h"
#include <fcntl.h>
#include <gtest/gtest.h>
#include <sys/socket.h>
#include <unistd.h>
#include "tupo/net/Socket.h"
namespace Tupo {
namespace net {
class EpollPollerTest : public ::testing::Test {
protected:
  void SetUp() override {
    loop_ = new EventLoop();
  }
  void TearDown() override { delete loop_; }
  EventLoop *loop_;
  int createNonBlockingSocket() {
    int fd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (fd < 0) {
      std::cerr << "Failed to create socket: " << strerror(errno);
    }
    return fd;
  }
};
// 测试1：测试构造函数
TEST_F(EpollPollerTest, ConstructorAndDestructor) {
  SUCCEED(); // SetUp 和 TearDown 已经测试了构造和析构
}

// 测试2：测试添加channel到epoll
TEST_F(EpollPollerTest, UpdateChannelAdd) {
  int fd = createNonBlockingSocket();
  {
    Channel channel(loop_, fd);
    EXPECT_NO_THROW(channel.enableReading(););
  }
  ::close(fd);
}

// 测试3：测试修改Channel的事件
TEST_F(EpollPollerTest, UpdateChannelModify) {
  int fd = createNonBlockingSocket();
  Channel channel(loop_, fd);
  EXPECT_NO_THROW(channel.enableReading());
  ASSERT_TRUE(channel.isReading());
  ::usleep(100);
  // 修改为写事件
  EXPECT_NO_THROW(channel.enableWriting());
  ASSERT_TRUE(channel.isWriting());
  // Channel 不拥有 fd，测试结束后必须手动关闭，否则 fd 泄漏
  ::close(fd);
}

// 测试4：测试移除channel
TEST_F(EpollPollerTest, RemoveChannel) {
  int fd = createNonBlockingSocket();

  Channel channel(loop_, fd);
  channel.enableReading();

  EXPECT_NO_THROW(loop_->poller()->removeChannel(&channel));
  ::close(fd);
}

// 测试5：测试轮询超时
TEST_F(EpollPollerTest, PollTimeout) {
  Poller::ChannelList activeChannels;

  // 测试超时情况，应该没有事件
  loop_->poller()->poll(10, &activeChannels);

  EXPECT_TRUE(activeChannels.empty()); // 活跃通道列表应该为空
}

// 测试6：测试查找活跃的 channels
TEST_F(EpollPollerTest, FindActiveChannels) {
  int fds[2];
  // 创建socket对用于测试
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) < 0) {
    GTEST_FAIL() << "Failed to create socket pair: " << strerror(errno);
  }
  // 设置为非阻塞
  ::fcntl(fds[0], F_SETFL, fcntl(fds[0], F_GETFL) | O_NONBLOCK);
  ::fcntl(fds[1], F_SETFL, fcntl(fds[1], F_GETFL) | O_NONBLOCK);
  Channel channel1(loop_, fds[0]);
  Channel channel2(loop_, fds[1]);

  channel1.enableReading();
  channel2.enableWriting();

  Poller::ChannelList activeChannels;

  // 写数据到socket，保证有数据可读
  const char *testdata = "把回忆拼好给你";
  ssize_t n = write(fds[1], testdata, strlen(testdata));
  EXPECT_GT(n, 0);

  // 轮询事件
  loop_->poller()->poll(100, &activeChannels);

  // Channel 不拥有 fd，测试结束后必须手动关闭，否则 fd 泄漏
  ::close(fds[0]);
  ::close(fds[1]);
}

// 测试7：测试多个 channels 的情况
TEST_F(EpollPollerTest, MultipleChannels) {
  const int kChannelCount = 5;
  std::vector<int> fds;
  std::vector<Channel *> channels;

  // 创建多个 channels
  for (int i = 0; i < kChannelCount; ++i) {
    int fd = createNonBlockingSocket();
    fds.push_back(fd);

    Channel *channel = new Channel(loop_, fd);
    channel->enableReading(); // 启用读事件
    channels.push_back(channel);
  }

  // 清理资源
  for (size_t i = 0; i < channels.size(); ++i) {
    loop_->poller()->removeChannel(channels[i]); // 从 poller 移除
    delete channels[i];                          // 删除 channel 对象
    ::close(fds[i]);                             // 关闭文件描述符
  }
}

// 测试8：测试无效文件描述符情况
//  测试无效文件描述符的情况
TEST_F(EpollPollerTest, InvalidFileDescriptor) {
  int invalidFd = -1; // 无效的文件描述符

  Channel channel(loop_, invalidFd);
  // 对于无效的文件描述符，epoll_ctl 应该失败
  // 但我们的实现应该能够处理这种情况而不崩溃

  EXPECT_NO_THROW(channel.enableReading());
}

// 测试9：高负载测试 - 测试大量channels情况
TEST_F(EpollPollerTest, HighLoadChannels) {
  const int kHighLoadCount = 100; // 测试100个channels
  std::vector<int> fds;
  std::vector<Channel *> channels;
  // 创建大量channels
  for (int i = 0; i < kHighLoadCount; ++i) {
    // tcp_poll() 对 TCP_CLOSE 状态的 socket 会无条件置 EPOLLHUP 事件位
    int fd = createNonBlockingSocket();
    fds.push_back(fd);

    Channel *channel = new Channel(loop_, fd);
    channel->enableReading();
    channels.push_back(channel);

    // 验证添加成功
    EXPECT_TRUE(channel->isReading());
  }
  // 测试轮询空事件（应该超时）
  Poller::ChannelList activeChannels;
  loop_->poller()->poll(100, &activeChannels);

  // 不为空
  EXPECT_TRUE(activeChannels.empty());

  // 清理资源 - 测试批量移除
  for (size_t i = 0; i < channels.size(); ++i) {
    EXPECT_NO_THROW(loop_->poller()->removeChannel(channels[i]));
    delete channels[i];
    ::close(fds[i]);
  }
}

// 测试10：测试fd内核状态
TEST_F(EpollPollerTest, TcpNoDelayActuallySet) {
  int fd = createNonBlockingSocket();
  Tupo::net::Socket sock(fd);
  sock.setTcpNoDelay(true);

  int on = 0;
  socklen_t len = sizeof(on);
  ASSERT_EQ(getsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &on, &len), 0);
  EXPECT_EQ(on, 1);  // 真正读到内核里设置的 1

  sock.setTcpNoDelay(false);
  ASSERT_EQ(getsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &on, &len), 0);
  EXPECT_EQ(on, 0);
}
} // namespace net
} // namespace Tupo
