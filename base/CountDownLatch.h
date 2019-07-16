#pragma once

#include "Condition.h"
#include "noncopyable.h"
#include "MutexLock.h"

class CountDownLatch : public noncopyable
{
public:
    explicit CountDownLatch(int count_):mutex(),condition(mutex),count(count_) {}
    void wait()
    {
        MutexLockGuard lock(mutex);
        while(count > 0)
            condition.wait();
    }
    void countDown()
    {
        MutexLockGuard lock(mutex);
        --count;
        if(count == 0)
            condition.notifyAll();
    }
private:
    mutable MutexLock mutex;
    Condition condition;
    int count;
};
