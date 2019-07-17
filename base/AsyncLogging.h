#pragma once

#include "Thread.h"
#include "MutexLock.h"
#include "CountDownLatch.h"
#include "LogStream.h"
#include "noncopyable.h"
#include <functional>
#include <string>
#include <vector>
#include <memory>

class AsyncLogging : public noncopyable
{
public:
    AsyncLogging(const std::string basename, int flushInterval_ = 2);
    ~AsyncLogging()
    {
        if(running)
            stop();
    }
    void append(const char* logline, int len);
    void start()
    {
        running = true;
        thread.start();
        latch.wait();
    }
    void stop()
    {
        running = false;
        cond.notify();
        thread.join();
    }
private:
    void threadFunc();
    typedef FixedBuffer<kLargeBuffer> Buffer;
    typedef std::vector<std::shared_ptr<Buffer> > BufferVec;
    typedef std::shared_ptr<Buffer> BufferPtr;
    const int flushInterval;
    bool running;
    std::string basename_;
    Thread thread;
    MutexLock mutex;
    Condition cond;
    BufferPtr currentBuffer;
    BufferPtr nextBuffer;
    BufferVec buffers;
    CountDownLatch latch;
};
