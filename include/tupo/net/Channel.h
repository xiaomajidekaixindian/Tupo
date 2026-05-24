#pragma once

#include <functional>
#include <poll.h>
namespace Tupo {
namespace net {
class EventLoop;
class Channel {

public:
  typedef std::function<void()> EventCallback;
  Channel(EventLoop *loop, int fd);
  Channel(const Channel &) = delete;
  Channel &operator=(const Channel &) = delete;
  ~Channel();
  // 处理事件
  void handleEvent();

  // 绑定回调函数
  void setReadCallback(const EventCallback &cb) {
    readCallback_ = std::move(cb);
  }
  void setWriteCallback(const EventCallback &cb) {
    writeCallback_ = std::move(cb);
  }
  void setErrorCallback(const EventCallback &cb) {
    errorCallback_ = std::move(cb);
  }
  void setCloseCallback(const EventCallback &cb) {
    closeCallback_ = std::move(cb);
  }

  int fd() const { return fd_; }

  int events() const { return events_; }

  // 注册到事件监听器
  void set_revents(int revt) { revents_ = revt; }

  void set_index(int index) { index_ = index; }
  int index() const { return index_; }

  int revents() { return revents_; }

  // 手动设置事件状态
  void enableReading() {
    events_ |= KReadEvent;
    update();
  }
  void enableWriting() {
    events_ |= kWriteEvent;
    update();
  }
  void disableWriting() {
    events_ &= ~kWriteEvent;
    update();
  }
  void disableAll() {
    events_ = kNoneEvent;
    update();
  }
  // 移除事件
  void remove();

  // 设置添加状态，用于EventLoop移除Channel
  void setAddedToLoop(bool added) { addedToLoop_ = added; }
  //  检查事件状态
  bool isNoneEvent() const { return events_ == kNoneEvent; }
  bool isWriting() const { return events_ & kWriteEvent; }
  bool isReading() const { return events_ & KReadEvent; }

private:
  void update();
  // 手动设置事件
  static const int kNoneEvent = 0;
  static const int KReadEvent = POLLIN | POLLPRI;
  static const int kWriteEvent = POLLOUT;

  Tupo::net::EventLoop *loop_; // 所属的EventLoop
  const int fd_;               // 监听的文件描述符
  int events_;                 // 关心的IO事件，输入事件
  int revents_;                // 目前活动的事件，输出事件
  int index_;                  // 在pollfd或者epoll_event结构体数组的索引
  bool addedToLoop_;           // 是否已添加到 EventLoop

  EventCallback readCallback_;
  EventCallback writeCallback_;
  EventCallback errorCallback_;
  EventCallback closeCallback_;
};
} // namespace net
} // namespace Tupo