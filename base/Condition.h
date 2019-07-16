#ifndef CONDITION_H_INCLUDED
#define CONDITION_H_INCLUDED

#pragma once
#include "MutexLock.h"
#include "noncopyable.h"
#include <pthread.h>
#include <errno.h>
#include <time.h>

class Condition: noncopyable
{
public:
    Condition(const Condition &_mutex):mutex(_mutex)
    {
        pthread_cond_init(&cond, NULL);
    }
    ~Condition()
    {
        pthread_cond_destroy(&cond);
    }
    void wait()
    {
        pthread_cond_wait(&cond, mutex.get());
    }
    void notify()
    {
        pthread_cond_signal(&cond);
    }
    void notifyAll()
    {
        pthread_cond_broadcast(&cond);
    }
    bool waitForSeconds(int seconds)
    {
        struct timespec abstime;
        clock_gettime(CLOCK_REALTIME, &abstime);
        abstime.tv_sec += static_cast<time_t>(seconds);
        return ETIMEDOUT == pthread_cond_timedwait(&cond, mutex.get(), &abstime);
    }
private:
    MutexLock &mutex;
    pthread_cond_t cond;
};


#endif // CONDITION_H_INCLUDED
