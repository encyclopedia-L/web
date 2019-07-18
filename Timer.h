#pragma once

#include "HttpData.h"
#include <memory>
#include <queue>
#include <deque>

class TimerNode
{
public:
    TimerNode(std::shared_ptr<HttpData> requestData, int timeout);
    TimerNode(TimerNode &tn);
    ~TimerNode();
    void update(int timeout);
    bool isValid();
    void clearReq();
    void setDeleted() { deleted = true;}
    bool isDeleted() (return deleted;)
    size_t getExpTime() {return expiredTime;}
private:
    bool deleted;
    size_t expiredTime;
    std::shared_ptr<HttpData> SPHttpData;
};

struct TimerCmp
{
    bool operator()(std::shared_ptr<TimerNode> &a, std::shared_ptr<TimerNode> &b) const
    {
        return a->getExpTime() > b->getExpTime();
    }
};

class TimerManager
{
public:
    TimerManager();
    ~TimerManager();
    void addTimer(std::shared_ptr<HttpData> SPHttpData, int timeout);
    void handleExpiredEvent();
private:
    typedef std::shared_ptr<TimerNode> SPTimerNode;
    std::priority_queue<SPTimerNode,std::deque<SPTimerNode>,TimerCmp> timerNodeQueue;
}
