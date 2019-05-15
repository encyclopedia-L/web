#ifndef EVENTLOOPTHREAD_H_INCLUDED
#define EVENTLOOPTHREAD_H_INCLUDED

#pragma once
#include "base/Condition.h"
#include "base/MutexLock.h"
#include "base/Thread.h"
#include "base/noncopyable.h"
#include "EventLoop.h"

class EventLoopThread :noncopyable
{
public:
    EventLoopThread();
    ~EventLoopThread();
    EventLoop* startLoop();

private:
    void threadFunc();
    EventLoop *loop_;
    bool exiting_;
    Thread thread_;
    MutexLock mutex_;
    Condition cond_;
};

#endif // EVENTLOOPTHREAD_H_INCLUDED
