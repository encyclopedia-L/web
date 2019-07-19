#include "Epoll.h"
#include "base/Logging.h"
#include "HttpData.h"
#include <assert.h>
#include <stdio.h>
#include <vector>
#include <memory>

using namespace std;

const int EVENTSNUM = 4096;
const int EPOLLWAIT_TIME = 10000;

Epoll::Epoll()
:epfd(epoll_create(1000)),events(EVENTSNUM)
{
    assert(epfd > 0);
}

Epoll::~Epoll()
{ }

void Epoll::epoll_add(SP_Channel request, int timeout)
{
    int fd = request->getFd();
    if(timeout > 0)
    {
        add_timer(request,timeout);
        fd2http[fd] = request->getHolder();
    }
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->getEvents();
    request->EqualAndUpdateLastEvents();
    fd2chan[fd] = request;
    if(epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&event) < 0)
    {
        perror("epoll_add error");
        fd2chan[fd].reset();
    }
}

void Epoll::epoll_mod(SP_Channel request, int timeout)
{
    if(timeout > 0)
        add_timer(request,timeout);
    int fd = request->getFd();
    if(!request->EqualAndUpdateLastEvents())
    {
        struct epoll_event event;
        event.data.fd = fd;
        event.events = request->getEvents();
        if(epoll_ctl(epfd,EPOLL_CTL_MOD,fd,&event) < 0)
        {
            perror("epoll_mod error");
            fd2chan[fd].reset();
        }
    }
}

void Epoll::epoll_del(SP_Channel request)
{
    int fd = request->getFd();
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->getEvents();
    if(epoll_ctl(epfd,EPOLL_CTL_DEL,fd,&event) < 0)
        perror("epoll_del error");
    fd2chan[fd].reset();
    fd2http[fd].reset();
}

std::vector<SP_Channel> Epoll::poll()
{
    while(true)
    {
        int event_count = epoll_wait(epfd,&*events.begin(),events.size(),EPOLLWAIT_TIME);
        if(event_count < 0)
            perror("epoll wait error");
        std::vector<SP_Channel> ret = getEventsRequest(event_count);
        if(ret.size() > 0)
            return ret;
    }
}

std::vector<SP_Channel> Epoll::getEventsRequest(int events_num)
{
    std::vector<SP_Channel> ret;
    for(int i = 0; i < events_num; ++i)
    {
        int fd = events[i].data.fd;
        SP_Channel cur = fd2chan[fd];
        if(cur)
        {
            cur->setRevents(events[i].events);
            cur->setEvents(0);
            ret.push_back(cur);
        }
        else
            LOG << "SP cur is invalid";
    }
    return ret;
}

void Epoll::add_timer(SP_Channel request_data, int timeout)
{
    shared_ptr<HttpData> t = request_data->getHolder();
    if(t)
        timerManager.addTimer(t,timeout);
    else
        LOG << "timer add fail";
}

void Epoll::handleExpired()
{
    timerManager.handleExpiredEvent();
}
