
#ifndef __DMTIMEREVENTNODE_H_INCLUDE__
#define __DMTIMEREVENTNODE_H_INCLUDE__

#include <vector>
#include <map>
#include <set>

#include "dmsingleton.h"

enum ETimerEventType
{
    ETimerEventType_EVERYDAY = 1000,
    ETimerEventType_EVERYWEEK = 2000,
    ETimerEventType_EVERYMONTH = 3000,
    ETimerEventType_EVERYYEAR = 4000,
};

class ITimerEventSink
{
public:
    virtual ~ITimerEventSink() = 0;
    virtual void OnEvent(ETimerEventType eEvent) = 0;
};

inline ITimerEventSink::~ITimerEventSink()
{

}

class CDMTimerEventNode : public ITimerEventSink
{
public:
    CDMTimerEventNode();
    virtual ~CDMTimerEventNode();

    bool AddEvent(ETimerEventType eType);
    bool DelEvent(ETimerEventType eType);

    virtual void OnEvent(ETimerEventType eEvent);
private:
    std::set<ETimerEventType> m_oSetEventType;
};

#endif // __DMTIMEREVENTNODE_H_INCLUDE__
