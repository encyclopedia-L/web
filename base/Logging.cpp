#include "Logging.h"
#include "Thread.h"
#include "AsyncLogging.h"
#include "CurrentThread.h"
#include <assert.h>
#include <iostream>
#include <time.h>
#include <sys/time.h>

using namespace std;

static pthread_once_t once_control_ = PTHREAD_ONCE_INIT;
static AsyncLogging *AsyncLogger_;
string Logger::logFileName = "/WebServer.log";

void once_init()
{
    AsyncLogger_ = new AsyncLogging(Logger::getLogFileName());
    AsyncLogger_->start();
}

void output(const char* msg, int len)
{
    pthread_once(PTHREAD_ONCE_INIT,once_init);
    AsyncLogger_.append(msg,len);
}

Logger::Impl::Impl(const char *fileName, int line_)
:stream(),line(line_),basename_(fileName)
{
    formatTime();
}

void Logger::Impl::formatTime()
{
    struct timeval tv;
    time_t time;
    char str_t[26] = {0};
    gettimeofday (&tv, NULL);
    time = tv.tv_sec;
    struct tm* p_time = localtime(&time);
    strftime(str_t, 26, "%Y-%m-%d %H:%M:%S\n", p_time);
    stream << str_t;
}

Logger::Logger(const char *fileName, int line)
:impl(fileName, line)
{}

Logger::~Logger()
{
    impl.stream << " -- " << impl_.basename_ << ':' << impl_.line << '\n';
    const LogStream::Buffer& buf(stream().buffer());
    output(buf.data(), buf.length());
}
