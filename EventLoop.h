#pragma once

#include "base/CurrentThread.h"
#include "Epoll.h"
#include "Channel.h"
#include "base/MutexLock.h"
#include <assert.h>
#include <memory>
#include <vector>
#include <functional>

class EventLoop
{
public:
    typedef std::function<void()> Functor;
    EventLoop();
    ~EventLoop();
    void loop();
    void quit();
    void runInLoop(Functor&& cb);
    void queueInLoop(Functor&& cb);
    void assertInLoopThread()
    {
        assert(isInLoopThread());
    }
    bool isInLoopThread() const
    {
        return threadId == CurrentThread::tid();
    }
private:
    void wakeup();
    void handleRead();
    void handleConn();
    void doPendingFunctors();

    bool looping;
    int wakeupfd;
    bool quit_;
    bool eventHandling;
    mutable MutexLock mutex;
    bool callingPendingFunctors;
    std::vector<Functor> pendingFunctors;
    std::shared_ptr<Epoll> poller;
    std::shared_ptr<Channel> pwakeupChannel;
    const pid_t threadId;
};
