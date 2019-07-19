#pragma once
#include "EventLoop.h"
#include "Channel.h"
#include "EventLoopThreadPool.h"
#include <memory>

class Server
{
public:
    Server(EventLoop *loop_, int threadNum_, int port_);
    ~Server() { }
    EventLoop* getLoop() const { return loop; }
    void start();
    void handNewConn();
    void handThisConn() { loop->updatePoller(acceptChannel); }
private:
    EventLoop *loop;
    int threadNum;
    std::unique_ptr<EventLoopThreadPool> eventLoopThreadPool;
    bool started;
    std::shared_ptr<Channel> acceptChannel;
    int port;
    int listenFd;
    static const int MAXFDS = 100000;
};
