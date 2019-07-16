#ifndef LOGFILE_H_INCLUDED
#define LOGFILE_H_INCLUDED

#pragma once
#include "noncopyable.h"
#include "FileUtil.h"
#include "MutexLock.h"
#include <memory>

class LogFile: noncopyable
{
    LogFile(const std::string& basename, int flushEveryN = 1024);
    ~LogFile();

    void append(const char* logline, int len);
    void flush();
    bool rollFile();

private:
    void append_unlocked(const char* logline, int len);

    const std::string basename_;
    const int flushEveryN_;

    int count_;
    std::unique_ptr<MutexLock> mutex_;
    std::unique_ptr<AppendFile> file_;
};


#endif // LOGFILE_H_INCLUDED
