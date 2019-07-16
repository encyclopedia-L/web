#ifndef THREAD_H_INCLUDED
#define THREAD_H_INCLUDED

#pragma once
#include "noncopyable.h"
#include "CountDownLatch.h"
#include <pthread.h>
#include <functional>

class Thread: noncopyable
{
public:
    typedef std::function<void()> ThreadFunc;
    explicit Thread(const ThreadFunc&, const std::string& name = std::string());
    ~Thread();
    void start();
    int join();
    bool started() const {return started_;}
    pid_t tid() const {return tid_;}
    const std::string& name() const {return name_;}
private:
    void setDefaultName();
    bool started_;
    bool joined_;
    pthread_t pthreadId_;
    pid_t tid_;
    ThreadFunc func_;
    std::string name_;
    CountdownLatch latch_;
};


#endif // THREAD_H_INCLUDED
