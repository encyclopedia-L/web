#ifndef COUNTDOWNLATCH_H_INCLUDED
#define COUNTDOWNLATCH_H_INCLUDED

#pragma once
#include "noncopyable.h"
#include "MutexLock.h"
#include "Condition.h"

// CountDownLatch的主要作用是确保Thread中传进去的func真的启动了以后
// 外层的start才返回
class CountdownLatch: noncopyable
{
public:
    explicit CountdownLatch(int count);
    void wait();
    void countdown();
private:
    mutable MutexLock mutex_;
    Condition cond_;
    int count_;
};

#endif // COUNTDOWNLATCH_H_INCLUDED
