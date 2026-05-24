## 基于Reactor模式的C++高性能网络服务框架Tupo

### 目录结构

```txt
Tupo/
├── include/
│   ├── base/
│   │   ├── MutexLock.h
│   └── net/
│   ├── 
│   ├── 
│   └──
├── src/
│   ├── base/
│   │   └── MutexLock.cpp
│   └── net/
│       ├── 
│       ├── 
│       └── 
└── tests/
    ├── test_mutex.cpp
    └── 
```


```txt
Phase 1:
base/ → noncopyable.h → Atomic.h → Mutex.h → Condition.h → Timestamp.h
net/ → Channel.h → Poller.h → EventLoop.h

Phase 2:  
base/ → Thread.h → ThreadPool.h → Logging.h
net/ → Timer.h → TimerQueue.h → Acceptor.h → TcpConnection.h

Phase 3:
base/ → AsyncLogging.h → Singleton.h
net/ → Buffer.h → TcpServer.h → EventLoopThreadPool.h
```

### 代码工作流程

![代码工作流程](.resource/image.png)