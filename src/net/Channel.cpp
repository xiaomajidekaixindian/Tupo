#include "tupo/net/Channel.h"
#include "tupo/net/EventLoop.h"
#include <iostream>
#include <poll.h>
namespace Tupo {
namespace net {
Channel::Channel(Tupo::net::EventLoop *loop, int fd)
    : loop_(loop), fd_(fd), events_(0), revents_(0), index_(-1),
      addedToLoop_(false) {}
Channel::~Channel() { remove(); }
void Channel::handleEvent() {
  if (revents_ & POLLNVAL) {
    std::cerr << "Channel::handle_event() POLLNVAL" << std::endl;
  }
  if (revents_ & (POLLERR | POLLNVAL)) {
    if (errorCallback_)
      errorCallback_();
  }
  if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
    if (readCallback_)
      readCallback_();
  }
  if (revents_ & POLLOUT) {
    if (writeCallback_)
      writeCallback_();
  }
}

void Channel::remove() {
  if (loop_ && addedToLoop_) {
    loop_->removeChannel(this);
    addedToLoop_ = false; // 标记已移除
  }
}

void Channel::update() {
  loop_->updateChannel(this);
  addedToLoop_ = true;
}
}; // namespace net
} // namespace Tupo