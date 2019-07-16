#pragma once
#include "noncopyable.h"
#include <string>
#include <stdio.h>

class AppendFile: public noncopyable
{
public:
    explicit AppendFile(std::string filename);
    ~AppendFile();
    void append(const char *logline, const size_t len);
    void flush();
private:
    size_t write(const char *logline,size_t len);
    FILE *fp;
    char buffer[64*1024];
};
