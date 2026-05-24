#include "tupo/net/Channel.h"
#include "tupo/net/EventLoop.h"
#include "tupo/net/poller/PollPoller.h"
#include <fcntl.h>
#include <gtest/gtest.h>
#include <sys/socket.h>
#include <unistd.h>

class PollPollerTest : public ::testing::Test {
protected:
  void SetUp() override {
    loop_ = new Tupo::net::EventLoop();
    poller_ = new Tupo::net::PollPoller(loop_);

    // 创建一对socket用于测试
    int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, sockpair_);
    ASSERT_NE(ret, -1);

    // 设置为非阻塞
    fcntl(sockpair_[0], F_SETFL, O_NONBLOCK);
    fcntl(sockpair_[1], F_SETFL, O_NONBLOCK);
    channel_ = new Tupo::net::Channel(loop_, sockpair_[0]);
  }
  int createNonBlockingSocket() {
    int fd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    if (fd < 0) {
      std::cerr << "Failed to create socket: " << strerror(errno);
    }
    return fd;
  }
  void TearDown() override {
    delete channel_;
    close(sockpair_[0]);
    close(sockpair_[1]);
    delete loop_;
    delete poller_;
  }
  Tupo::net::EventLoop *loop_;
  Tupo::net::PollPoller *poller_;
  Tupo::net::Channel *channel_;
  int sockpair_[2];
};

// 测试Poller构造和基本功能
TEST_F(PollPollerTest, ConstructorAndBasic) {
  ASSERT_NE(poller_, nullptr);
  ASSERT_NE(channel_, nullptr);

  ASSERT_FALSE(poller_->hasChannel(channel_));

  // 添加channel之后在测试
  poller_->updateChannel(channel_);
  ASSERT_TRUE(poller_->hasChannel(channel_));
}

// 测试轮询功能（无事件情况）
TEST_F(PollPollerTest, PollNoEvents) {
  Tupo::net::Channel channel(loop_, sockpair_[0]);
  poller_->updateChannel(&channel);

  Tupo::net::Poller::ChannelList activeChannels;

  // 短暂轮询，应该没有事件
  poller_->poll(10, &activeChannels);
  // 注意：这里可能有事件也可能没有，取决于系统调度
}

// 测试轮询功能（有读事件）
TEST_F(PollPollerTest, PollWithReadEvent) {
  Tupo::net::Channel channel(loop_, sockpair_[0]);
  channel.enableReading();

  // 向另一个socket写入数据，触发读事件
  const char *test_data = "把回忆拼好给你";
  ssize_t ret = write(sockpair_[1], test_data, strlen(test_data));
  ASSERT_GT(ret, 0);

  Tupo::net::Poller::ChannelList activeChannels;
  loop_->poller()->poll(10, &activeChannels);

  // 可能检测到读事件，但不一定立即
  if (!activeChannels.empty()) {
    ASSERT_EQ(activeChannels[0]->fd(), sockpair_[0]);
  }
}

// 测试Channel移除
TEST_F(PollPollerTest, RemoveTest) {
  Tupo::net::Channel channel(loop_, sockpair_[0]);

  channel.enableReading();

  // 注意，这里要用loop_的poller，我的EventLoop构造函数会调用
  // Poller::newDefaultPoller(this) 创建自己的
  // poller，而不是测试夹具里面的poller
  // Channel 通过 loop_ 访问的是 EventLoop 内部的 poller，不是你手动创建的
  // poller_
  ASSERT_TRUE(loop_->poller()->hasChannel(&channel));

  loop_->poller()->removeChannel(&channel);
  ASSERT_FALSE(loop_->poller()->hasChannel(&channel));
}

// 测试多Channel管理
TEST_F(PollPollerTest, MultipleChannels) {
  // 创建另一个socket pair用于测试多Channel
  int sockpair2[2];
  int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, sockpair2);

  ASSERT_NE(ret, -1);

  Tupo::net::Channel channel1(loop_, sockpair_[0]);
  Tupo::net::Channel channel2(loop_, sockpair2[0]);

  channel1.enableReading();
  channel2.enableWriting();

  ASSERT_TRUE(loop_->poller()->hasChannel(&channel1));
  ASSERT_TRUE(loop_->poller()->hasChannel(&channel2));

  // 清理
  loop_->poller()->removeChannel(&channel1);
  loop_->poller()->removeChannel(&channel2);

  close(sockpair2[0]);
  close(sockpair2[1]);
}

// 测试事件类型处理
TEST_F(PollPollerTest, EventTypes) {
  Tupo::net::Channel channel(loop_, sockpair_[0]);
  channel.enableReading();

  // 写入数据触发读事件
  const char *test_data = "把回忆拼好给你";
  int ret = write(sockpair_[1], test_data, strlen(test_data));
  ASSERT_NE(ret, -1);
  Tupo::net::Poller::ChannelList activeChannel;
  loop_->poller()->poll(100, &activeChannel);

  if (!activeChannel.empty()) {
    ASSERT_TRUE(activeChannel[0]->revents() & POLLIN);
  }
}

// 测试修改Channel的事件
TEST_F(PollPollerTest, UpdateChannelModify) {
  int fd = createNonBlockingSocket();
  Tupo::net::Channel channel(loop_, fd);
  EXPECT_NO_THROW(channel.enableReading());
  ASSERT_TRUE(channel.isReading());
  ::usleep(100);
  // 修改为写事件
  EXPECT_NO_THROW(channel.enableWriting());
  ASSERT_TRUE(channel.isWriting());
}
// 主函数
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}