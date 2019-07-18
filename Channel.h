#pragma once

#include <functional>
#include <memory>

class EventLoop;
class HttpData;

class Channel
{
public:
    typedef std::function<void()> Functor;
    Channel(EventLoop *loop_);
    Channel(EventLoop *loop_, int fd_);
    ~Channel();
    int getFd()
    {
        return fd;
    }
    void setFd(int fd_)
    {
        fd = fd_;
    }
    void setEvents(__uint32_t ev)
    {
        events = ev;
    }
    __uint32_t getEvents()
    {
        return events;
    }
    void setRevents(__uint32_t ev)
    {
        revents = ev;
    }
    void setHolder(std::shared_ptr<HttpData> holder_)
    {
        holder = holder_;
    }
    std::shared_ptr<HttpData>  getHolder()
    {
        std::shared_ptr<HttpData> ret(holder.lock());
        return ret;
    }
    void setReadHandler(Functor &&cb)
    {
        readHandler = cb;
    }
    void setWriteHandler(Functor &&cb)
    {
        writeHandler = cb;
    }
    void setErrorHandler(Functor &&cb)
    {
        errorHandler = cb;
    }
    void setConnHandler(Functor &&cb)
    {
        connHandler = cb;
    }
    bool EqualAndUpdateLastEvents()
    {
        bool ret = (lastEvents == events);
        lastEvents = events;
        return ret;
    }
    __uint32_t getLastEvents()
    {
        return lastEvents;
    }
    void handleEvents();
    void handleRead();
    void handleWrite();
    void handleConn();
private:
    Functor readHandler;
    Functor writeHandler;
    Functor errorHandler;
    Functor connHandler;

    EventLoop *loop;
    int fd;
    __uint32_t events;
    __uint32_t revents;
    __uint32_t lastEvents;
    std::weak_ptr<HttpData> holder;
};
typedef std::shared_ptr<Channel> SP_Channel;
