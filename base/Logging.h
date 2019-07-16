#pragma once

#include "LogStream.h"
#include <string>
#include <stdio.h>

class AsyncLogging;

class Logger
{
public:
    Logger(const char *fileName, int line);
    ~Logger();
    static void setLogFileName(std::string fileName)
    {
        logFileName = fileName;
    }
    static std::string getLogFileName()
    {
        return logFileName;
    }
private:
    class Impl
    {
    public:
        Impl(const char *fileName, int line);
        void formatTime();

        LogStream stream;
        int line;
        std::string basename_;
    };
    Impl impl;
    static std::string logFileName;
};

#define LOG Logger(__FILE__,__LINE__).stream()
