#pragma once

#include "MutexLock.h"
#include "noncopyable.h"
#include <pthread.h>

class Condition : public noncopyable
{
public:
    Condition(MutexLock &mutex_): mutex(mutex_)
    {
        pthread_cond_init(&cond,nullptr);
    }
    ~Condition()
    {
        pthread_cond_destroy(&cond);
    }
    void wait()
    {
        pthread_cond_wait(&cond,mutex.get());
    }
    void notify()
    {
        pthread_cond_signal(&cond);
    }
    void notifyAll()
    {
        pthread_cond_broadcast(&cond);
    }
private:
    MutexLock &mutex;
    pthread_cond_t cond;
};
