#pragma once

class noncopyable
{
public:
    noncopyable() = default;
    ~noncopyable() = default;
    noncopyable(const noncopyable &) = delete;
    const noncopyable& operator=(const noncopyable&) = delete;
};
