#include "AsyncLogging.h"
#include "LogFile.h"
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <functional>

using namespace std;

AsyncLogging::AsyncLogging(string logFileName_,int flushInterval_)
:flushInterval(flushInterval_),running(false),basename_(logFileName_),
thread(std::bind(&AsyncLogging::threadFunc,this),"Logging"),
mutex(),cond(mutex),currentBuffer(new Buffer),nextBuffer(new Buffer),
buffers(),latch(1)
{
    assert(logFileName_.size() > 1);
    currentBuffer->bzero();
    nextBuffer->bzero();
    buffers.reserve(16);
}

void AsyncLogging::append(const char* logline, int len)
{
    MutexLockGuard lock(mutex);
    if(currentBuffer->avail() > len)
        currentBuffer->append(logline,len);
    else
    {
        buffers.push_back(currentBuffer);
        currentBuffer.reset();
        if(nextBuffer)
            currentBuffer = std::move(nextBuffer);
        else
            currentBuffer.reset(new Buffer);
        currentBuffer->append(logline,len);
        cond.notify();
    }
}

void AsyncLogging::threadFunc()
{
    assert(running == true);
    latch.countDown();
    LogFile output(basename_);
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer2->bzero();
    BufferVec bufferstoWrite;
    bufferstoWrite.reserve(16);
    while(running)
    {
        assert(newBuffer1 && newBuffer1->length() == 0);
        assert(newBuffer2 && newBuffer2->length() == 0);
        assert(bufferstoWrite.empty());
        {
            MutexLockGuard lock(mutex);
            if(buffers.empty())
                cond.wait();
            buffers.push_back(currentBuffer);
            currentBuffer.reset();
            currentBuffer = std::move(newBuffer1);
            bufferstoWrite.swap(buffers);
            if(!nextBuffer)
                nextBuffer = std::move(newBuffer2);
        }

        assert(!bufferstoWrite.empty());
        if(bufferstoWrite.size() > 25)
            bufferstoWrite.erase(bufferstoWrite.begin()+2,bufferstoWrite.end());
        for(size_t i = 0; i < bufferstoWrite.size(); ++i)
        {
            output.append(bufferstoWrite[i]->data(),bufferstoWrite[i]->length());
        }
        if(bufferstoWrite.size() > 2)
            bufferstoWrite.resize(2);

        if(!newBuffer1)
        {
            assert(!bufferstoWrite.empty());
            newBuffer1 = bufferstoWrite.back();
            bufferstoWrite.pop_back();
            newBuffer1->reset();
        }
        if(!newBuffer2)
        {
            assert(!bufferstoWrite.empty());
            newBuffer2 = bufferstoWrite.back();
            bufferstoWrite.pop_back();
            newBuffer2->reset();
        }
        bufferstoWrite.clear();
        output.flush();
    }
    output.flush();
}
