#pragma once
#include "base/noncopyable.h"
#include "EventLoopThread.h"
#include "base/Logging.h"
#include <memory>
#include <vector>

class EventLoopThreadPool: noncopyable
{
public:
    EventLoopThreadPool(EventLoop* baseLoop, int numThreads);
    ~EventLoopThreadPool() {LOG << "~EventLoopThreadPool()";}
    void start();
    EventLoop* getNextLoop();
private:
    EventLoop *baseLoop;
    bool started;
    int numThreads;
    int next;
    std::vector<std::shared_ptr<EventLoopThread>> threads;
    std::vector<EventLoop*> loops;
};
