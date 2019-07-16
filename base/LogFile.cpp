#include "LogFile.h"
#include <stdio.h>

using namespace std;

LogFile::LogFile(const string& basename_, int flushEveryN_ = 1024)
:basename(basename_),flushEveryN(flushEveryN_),count(0),mutex(new MutexLock)
{
    file.reset(new AppendFile(basename));
}

LogFile::~LogFile()
{}

void LogFile::append(const char* logline, int len)
{
    MutexLockGuard lock(mutex);
    append_unlocked(logline,len);
}

void LogFile::flush()
{
    MutexLockGuard lock(mutex);
    file->flush();
}

void LogFile::append_unlocked(const char* logline, int len)
{
    file->append(logline,len);
    ++count;
    if(count >= flushEveryN)
    {
        count = 0;
        file->flush();
    }
}
