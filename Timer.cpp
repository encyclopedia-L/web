#include "Timer.h"
#include <sys/time.h>

using namespace std;

TimerNode::TimerNode(std::shared_ptr<HttpData> requestData, int timeout)
:deleted(false),SPHttpData(requestData)
{
    struct timeval now;
    gettimeofday(&now,nullptr);
    expiredTime = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + timeout;
}

TimerNode::TimerNode(TimerNode &tn)
:SPHttpData(tn.SPHttpData)
{ }

TimerNode::~TimerNode()
{
    if(SPHttpData)
        SPHttpData->handleClose();
}

void TimerNode::update(int timeout)
{
    struct timeval now;
    gettimeofday(&now,nullptr);
    expiredTime = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + timeout;
}

bool TimerNode::isValid()
{
    struct timeval now;
    gettimeofday(&now,nullptr);
    size_t tmp = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000));
    if(tmp < expiredTime)
        return true;
    else
    {
        this->setDeleted();
        return false;
    }
}

void TimerNode::clearReq()
{
    SPHttpData.reset();
    this->setDeleted();
}

TimerManager::TimerManager()
{ }

TimerManager::~TimerManager()
{ }

void TimerManager::addTimer(std::shared_ptr<HttpData> SPHttpData, int timeout)
{
    SPTimerNode new_node(new TimerNode(SPHttpData, timeout));
    timerNodeQueue.push(new_node);
    SPHttpData->linkTimer(new_node);
}

void TimerManager::handleExpiredEvent()
{
    while(!timerNodeQueue.empty())
    {
        SPTimerNode ptimer_now = timerNodeQueue.top();
        if(ptimer_now->isDeleted())
            timerNodeQueue.pop();
        else if(ptimer_now->isValid() == false)
            timerNodeQueue.pop();
        else
            break;
    }
}
