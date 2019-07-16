#include "CountdownLatch.h"

CountdownLatch::CountdownLatch(int count): mutex_(), cond_(mutex_),count_(count)
{ }

void CountdownLatch::wait()
{
    MutexLockGuard lock(mutex_);
    while(count_ > 0)
        cond_.wait();
}

void CountdownLatch::countdown()
{
    MutexLockGuard lock(mutex_);
    --count_;
    if(count == 0)
        cond_.notifyAll();
}
