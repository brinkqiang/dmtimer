
#ifndef __DMTIMEREVENT_H__
#define __DMTIMEREVENT_H__

#include <vector>
#include <map>
#include <set>

#include "dmtimernode.h"
#include "dmtimereventnode.h"

#include "dmsingleton.h"

typedef std::set<ITimerEventSink*> SetTimerEventSink;


class CDMTimerEventMgr : public CDMSafeSingleton<CDMTimerEventMgr>,
    public CDMTimerNode
{
    friend class TSingleton<CDMTimerEventMgr>;
public:
    CDMTimerEventMgr();
    virtual ~CDMTimerEventMgr();

    virtual bool Init();

    void AddTimerEventSink(ITimerEventSink* poSink, ETimerEventType eType);
    void DelTimerEventSink(ITimerEventSink* poSink, ETimerEventType eType);
public:
    // event
    virtual void OnTimer(uint64_t qwIDEvent,  dm::any& oAny );
private:
    bool LoadConfig();
    bool InitEvent();
private:
    bool __InitEveryDay();
    bool __InitEveryWeek();
    bool __InitEveryMonth();
    bool __InitEveryYear();
private:

    std::map<ETimerEventType, SetTimerEventSink> m_oMapTimerEventSink;
};

#endif // __DMTIMEREVENT_H__