#pragma once
#include "Channel.h"
#include "Timer.h"
#include "HttpData.h"
#include <vector>
#include <memory>
#include <sys/epoll.h>

class Epoll
{
public:
    Epoll();
    ~Epoll();
    void epoll_add(SP_Channel request, int timeout);
    void epoll_mod(SP_Channel request, int timeout);
    void epoll_del(SP_Channel requestt);
    std::vector<std::shared_ptr<Channel> > poll();
    std::vector<std::shared_ptr<Channel> > getEventsRequest(int events_num);
    void add_timer(std::shared_ptr<Channel> request_data, int timeout);
    void handleExpired();
    int getEpollfd()
    {
        return epfd;
    }
private:
    static const int MAXFDS = 100000;
    int epfd;
    std::vector<struct epoll_event> events;
    std::shared_ptr<Channel> fd2chan[MAXFDS];
    std::shared_ptr<HttpData> fd2http[MAXFDS];
    TimerManager timerManager;
};
