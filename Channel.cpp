#include "Channel.h"
#include "EventLoop.h"
#include <sys/epoll.h>

Channel::Channel(EventLoop *loop_)
:loop(loop_),events(0),lastEvents(0)
{ }

Channel::Channel(EventLoop *loop_,int fd_)
:loop(loop_),fd(fd_),events(0),lastEvents(0)
{ }

Channel::~Channel()
{ }

void Channel::handleRead()
{
    if(readHandler)
        readHandler();
}

void Channel::handleWrite()
{
    if(writeHandler)
        writeHandler();
}

void Channel::handleConn()
{
    if(connHandler)
        connHandler();
}

void Channel::handleEvents()
{
    events = 0;
    if((revents & EPOLLHUP) && !(revents & EPOLLIN))
    {
        events = 0;
        return;
    }
    if(revents & EPOLLERR)
    {
        if(errorHandler)
            errorHandler();
        events = 0;
        return;
    }
    if(revents & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
        handleRead();
    if(revents & EPOLLOUT)
        handleWrite();
    handleConn();
}
