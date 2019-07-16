#ifndef FILEUTIL_H_INCLUDED
#define FILEUTIL_H_INCLUDED

#pragma once
#include "noncopyable.h"

class AppendFile : noncopyable
{
public:
    explicit AppendFile(std::string filename);
    ~AppendFile();
    void append(const char *logline, const size_t len);
    void flush();
private:
    size_t write(const char *logline, size_t len);
    FILE* fp_;
    char buffer_[64*1024];
};

#endif // FILEUTIL_H_INCLUDED