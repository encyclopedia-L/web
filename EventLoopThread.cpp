#include "EventLoopThread.h"
#include <assert.h>

EventLoopThread::EventLoopThread()
:loop(nullptr),exiting(false),
thread(bind(&EventLoopThread::threadFunc, this), "EventLoopThread"),
mutex(),cond(mutex)
{ }

EventLoopThread::~EventLoopThread()
{
    exiting = true;
    if(loop != nullptr)
    {
        loop->quit();
        thread.join();
    }
}

EventLoop* EventLoopThread::startLoop()
{
    assert(!thread.started());
    thread.start();
    {
        MutexLockGuard lock(mutex);
        while(loop == nullptr)
            cond.wait();
    }
    return loop;
}

void EventLoopThread::threadFunc()
{
    EventLoop loop_;
    {
        MutexLockGuard lock(mutex);
        loop = &loop_;
        cond.notify();
    }
    loop_.loop();
    loop = nullptr;
}
