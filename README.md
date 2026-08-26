## 基于Reactor模式的C++高性能网络服务框架Tupo

### 目录结构

```txt
Tupo/
├── CMakeLists.txt                  # 顶层构建脚本,生成 tupo_base / tupo_net 两个静态库
├── README.md
├── build.sh                        # 一键编译脚本
├── .clang-format                   # 代码风格配置
├── .resource/
│   └── image.png                   # 代码工作流程图
├── examples/
│   └── ThreadEventLoop.cpp         # 示例:多线程各跑一个 EventLoop
├── include/tupo/                   # 公共头文件(接口)
│   ├── base/                       # 基础库
│   │   ├── MutexLock.h             # 互斥锁封装
│   │   ├── Condition.h             # 条件变量
│   │   ├── Thread.h                # 线程封装(移动语义 + promise/future 同步)
│   │   └── Timestamp.h             # 时间戳
│   └── net/                        # 网络库
│       ├── EventLoop.h             # 事件循环核心(one loop per thread)
│       ├── Channel.h               # fd 事件分发
│       ├── Poller.h                # IO 多路复用抽象接口
│       ├── Timer.h                 # 定时器
│       ├── TimerId.h               # 定时器标识
│       ├── TimerQueue.h            # 定时器队列(timerfd 驱动)
│       ├── Socket.h                # socket 封装
│       ├── InetAddress.h           # 网络地址封装
│       ├── Acceptor.h              # 监听与 accept
│       ├── TcpConnection.h         # TCP 连接
│       ├── TcpServer.h             # TCP 服务端
│       └── poller/
│           ├── EpollPoller.h       # epoll 实现
│           └── PollPoller.h        # poll 实现
├── src/                            # 实现(与 include 一一对应)
│   ├── base/
│   │   ├── MutexLock.cpp
│   │   ├── Condition.cpp
│   │   ├── Thread.cpp
│   │   └── Timestamp.cpp
│   └── net/
│       ├── EventLoop.cpp
│       ├── Channel.cpp
│       ├── Poller.cpp
│       ├── Timer.cpp
│       ├── TimerQueue.cpp
│       ├── Socket.cpp
│       ├── InetAddress.cpp
│       ├── Acceptor.cpp
│       ├── TcpConnection.cpp
│       ├── TcpServer.cpp
│       └── poller/
│           ├── EpollPoller.cpp
│           └── PollPoller.cpp
└── tests/                          # TDD 单元测试
    ├── CMakeLists.txt              # 测试构建脚本
    ├── build.sh
    ├── base/
    │   ├── MutexLock_test.cpp
    │   ├── Thread_test.cpp
    │   └── Timestamp_test.cpp
    └── net/
        ├── EpollPoller_test.cpp
        ├── PollPoller_test.cpp
        ├── EventLoop_test.cpp
        ├── EventLoop_UnitTest.cpp
        ├── Timer_test.cpp
        ├── TimerId_test.cpp
        ├── TimerQueue_test.cpp
        ├── TimerQueueMultiThreadTest.cpp
        ├── Socket_test.cpp
        ├── InetAddress_test.cpp
        └── TcpServer_test.cpp
```

### 开发路线图

```txt
Phase 1:
base/ → MutexLock.h → Condition.h → Timestamp.h 
net/ → Channel.h → Poller.h → EpollPoller.h → PollPoller.h → EventLoop.h

Phase 2:  
base/ → Thread.h → ThreadPool.h → Logging.h 
net/ → Timer.h → TimerQueue.h → Acceptor.h → TcpConnection.h

Phase 3:
base/ → AsyncLogging.h → Singleton.h
net/ → Buffer.h → TcpServer.h → EventLoopThreadPool.h
```

### 代码工作流程

![代码工作流程](.resource/image.png)

### 将Channel 的文件描述符注册、修改或删除到 epoll 实例中

```text
用户代码
    |
    v
channel->enableReading()
    |
    v    
Channel::update()
    |
    v
loop_->updateChannel(this)
    |
    v
EPollPoller::updateChannel(channel)
    |
    v
poller_->updateChannel(channel)
    |
    v
EPollPoller::update(operation, channel)
    |
    v
epoll_ctl(epollfd_, operation, fd, &event)  ← 系统调用
```

### TimerQueue

**reset()算法**

### InetAddress

**业务场景**

场景1：服务端监听所有网卡

```cpp
// 我想在 8080 端口提供服务，接受任何网卡的连接
InetAddress addr(8080);  
// 内部：0.0.0.0:8080（所有网卡）
```

场景2：服务端只监听本地（测试/调试用）

```cpp
// 我只想让本机连接，不让外部访问
InetAddress addr(8080, true);  
// 内部：127.0.0.1:8080（只能本机访问）
```

场景3：客户端连接指定服务器

```cpp
// 我要连接 192.168.1.100 的 8080 端口
InetAddress serverAddr("192.168.1.100", 8080);
```

场景4：从 accept 获取客户端地址

```cpp
// accept 系统调用返回的是 C 结构体
struct sockaddr_in clientAddr;
socklen_t len = sizeof(clientAddr);
int connfd = accept(listenFd, (struct sockaddr*)&clientAddr, &len);

// 需要把它转成 InetAddress 才能方便使用
InetAddress peer(clientAddr);  // 封装成 InetAddress
std::cout << "新连接来自: " << peer.toIpPort() << std::endl;
// 输出：新连接来自: 192.168.1.50:54321
```

### 问题


**问题一**

问题：定时器不触发，poll一直超时10秒


问题描述：测试TimerQueue时候，通过runAt来添加定时器触发回调，并通过runAfter在5s之后，停止poll，但是发现一直阻塞，一直超时10s，然后每10s超时一次

根本原因：
- timerfd就像一个只能设置一个时间的闹钟
- 我添加了定时器到timers_，但没有调用resetTimerfd()也就是没有通过timerfd_settime设置timerfd定时器
- 所以闹钟根本没上弦，永远不会响

解决方案：
- 添加定时器时，如果是最早的，必须调用resetTimerfd()
- 定时器触发后，如果有下一个定时器，再次调用resetTimerfd()


一句话总结：
timers_是备忘录，timerfd是闹钟，备忘录改了必须同步闹钟


**问题二**

问题：Thread类封装关于构造函数和线程id的获取


问题描述：在进行Thread类进行TDD测试发现，移动构造和移动赋值没有实现，拷贝构造和拷贝赋值声明了，属于五法则。关于线程id获取，第一次是调用currentThreadTid获取的是调用该函数线程的id，第二次是主线程过早获取tid，没有等待子线程完成，导致获取的线程id为0。

根本原因：

五法则问题：
- 类中包含std::atomic成员，其移动构造被删除
- 声明了拷贝构造/赋值的删除，但没有实现移动语义
- 编译器不会自动生成移动操作，导致类型不可移动

线程ID问题：

- start()中启动子线程后立即返回，没有同步机制
- 主线程调用getTid()时，子线程可能还未执行tid_ = currentThreadTid()
- currentThreadTid()是静态方法，返回调用者的TID，而非存储的tid_

解决方案：

- 实现移动构造和移动赋值，删除拷贝构造和拷贝赋值的原因是，线程的资源不能被两个对象同时管理
- 利用promise/future进行一次性的线程同步，底层原理是条件变量+mutex

