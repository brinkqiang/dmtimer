
#ifndef __DMTIMEREVENT_H__
#define __DMTIMEREVENT_H__

#include <vector>
#include <map>

#include "dmtimernode.h"
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
    virtual ETimerEventType GetEventType() = 0;
};

typedef std::vector<ITimerEventSink*> VecTimerEventSink;

class CDMTimerEventMgr : public TSingleton<CDMTimerEventMgr>, public CDMTimerNode
{
    friend class TSingleton<CDMTimerEventMgr>;
public:
    CDMTimerEventMgr();
    virtual ~CDMTimerEventMgr();

    bool Init();

    void AddTimerEventSink(ITimerEventSink* poSink, ETimerEventType eType);
public:
    // event
    virtual void OnTimer(uint64_t qwIDEvent,  dm::any& oAny );
private:
    bool LoadConfig();
private:

    std::map<ETimerEventType, VecTimerEventSink> m_oMapTimerEventSink;
};

#endif // __DMTIMEREVENT_H__