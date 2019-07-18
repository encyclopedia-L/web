#include "EventLoopThreadPool.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop_, int numThreads_)
:baseLoop(baseLoop_),started(false),numThreads(numThreads_),next(0)
{
    if(numThreads <= 0)
    {
        LOG << "numThreads_ <= 0";
        abort();
    }
}

void EventLoopThreadPool::start()
{
    baseLoop->assertInLoopThread();
    started = true;
    for(int i = 0; i < numThreads; ++i)
    {
        std::shared_ptr<EventLoopThread> t(new EventLoopThread());
        threads.push_back(t);
        loops.push_back(t->startLoop());
    }
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
    baseLoop->assertInLoopThread();
    assert(started);
    EventLoop *loop = baseLoop;
    if(!loops.empty())
    {
        loop = loops[next];
        next = (next + 1) % numThreads;
    }
    return loop;
}
