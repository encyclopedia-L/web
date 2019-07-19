#include "EventLoop.h"
#include "base/Logging.h"
#include "base/CurrentThread.h"
#include "Util.h"
#include <sys/eventfd.h>
#include <stdlib.h>
#include <unistd.h>
#include <functional>

using namespace std;

__thread EventLoop* t_loopInThisThread = 0;

int createEventfd()
{
    int evfd = eventfd(0,EFD_NONBLOCK | EFD_CLOEXEC);
    if(evfd < 0)
    {
        LOG << "Creating eventfd failed";
        abort();
    }
    return evfd;
}

EventLoop::EventLoop()
:looping(false),wakeupfd(createEventfd()),quit_(false),eventHandling(false),mutex(),
callingPendingFunctors(false),poller(new Epoll()),pwakeupChannel(new Channel(this,wakeupfd)),threadId(CurrentThread::tid())
{
    if(t_loopInThisThread)
        {//LOG << "Another EventLoop " << t_loopInThisThread << " exists in this thread " << threadId;
	}
    else
        t_loopInThisThread = this;
    pwakeupChannel->setEvents(EPOLLIN | EPOLLET);
    pwakeupChannel->setReadHandler(bind(&EventLoop::handleRead, this));
    pwakeupChannel->setConnHandler(bind(&EventLoop::handleConn, this));
    poller->epoll_add(pwakeupChannel,0);
}

EventLoop::~EventLoop()
{
    close(wakeupfd);
    t_loopInThisThread = nullptr;
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = writen(wakeupfd,(char*)(&one),sizeof(one));
    if(n != sizeof(one))
    {
        LOG << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
    }
}

void EventLoop::handleRead()
{
    uint64_t one = 1;
    ssize_t n = readn(wakeupfd,&one,sizeof(one));
    if(n != sizeof(one))
    {
        LOG << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
    }
    pwakeupChannel->setEvents(EPOLLIN | EPOLLET);
}

void EventLoop::handleConn()
{
    poller->epoll_mod(pwakeupChannel,0);
}

void EventLoop::runInLoop(Functor&& cb)
{
    if(isInLoopThread())
        cb();
    else
        queueInLoop(std::move(cb));
}

void EventLoop::queueInLoop(Functor&& cb)
{
    {
        MutexLockGuard lock(mutex);
        pendingFunctors.emplace_back(std::move(cb));
    }
    if(!isInLoopThread() || callingPendingFunctors)
        wakeup();
}

void EventLoop::loop()
{
    assert(!looping);
    assert(isInLoopThread());
    looping = true;
    quit_ = false;
    vector<SP_Channel> ret;
    while(!quit_)
    {
        ret.clear();
        ret = poller->poll();
        eventHandling = true;
        for(auto &it : ret)
            it->handleEvents();
        eventHandling = false;
        doPendingFunctors();
        poller->handleExpired();
    }
    looping = false;
}

void EventLoop::doPendingFunctors()
{
    vector<Functor> func;
    callingPendingFunctors = true;
    {
        MutexLockGuard lock(mutex);
        func.swap(pendingFunctors);
    }
    for(size_t i = 0; i < func.size(); ++i)
        func[i]();
    callingPendingFunctors = false;
}

void EventLoop::quit()
{
    quit_ = true;
    if(!isInLoopThread())
        wakeup();
}
