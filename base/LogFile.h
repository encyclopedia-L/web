#pragma once

#include "FileUtil.h"
#include "noncopyable.h"
#include "MutexLock.h"
#include <memory>
#include <string>

class LogFile: public noncopyable
{
public:
    LogFile(const std::string& basename_, int flushEveryN_ = 1024);
    ~LogFile();
    void append(const char* logline, int len);
    void flush();

private:
    void append_unlocked(const char* logline, int len);

    const std::string basename;
    const int flushEveryN;
    int count;
    std::unique_ptr<MutexLock> mutex;
    std::unique_ptr<AppendFile> file;
};
