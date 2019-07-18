#pragma once
#include "base/noncopyable.h"
#include "base/Thread.h"
#include "base/MutexLock.h"
#include "base/Condition.h"
#include "EventLoop.h"

class EventLoopThread: public noncopyable
{
public:
    EventLoopThread();
    ~EventLoopThread();
    EventLoop* startLoop();
private:
    void threadFunc();
    EventLoop *loop;
    bool exiting;
    Thread thread;
    MutexLock mutex;
    Condition cond;
};
