#include "dmtimereventmodule.h"



CDMTimerEventMgr::CDMTimerEventMgr()
{

}

CDMTimerEventMgr::~CDMTimerEventMgr()
{

}

bool CDMTimerEventMgr::Init()
{
    if (!LoadConfig())
    {
        return false;
    }

    if (!InitEvent())
    {
        return false;
    }
    return true;
}

void CDMTimerEventMgr::AddTimerEventSink(ITimerEventSink* poSink,
    ETimerEventType eType)
{
    m_oMapTimerEventSink[eType].insert(poSink);
}

void CDMTimerEventMgr::DelTimerEventSink(ITimerEventSink* poSink,
    ETimerEventType eType)
{
    m_oMapTimerEventSink[eType].erase(poSink);
}

bool CDMTimerEventMgr::LoadConfig()
{
    return true;
}

bool CDMTimerEventMgr::InitEvent()
{
    return true
        && __InitEveryDay()
        && __InitEveryWeek()
        && __InitEveryMonth()
        && __InitEveryYear()
        && true;
}

bool CDMTimerEventMgr::__InitEveryDay()
{
    return true;
}

bool CDMTimerEventMgr::__InitEveryWeek()
{
    return true;
}

bool CDMTimerEventMgr::__InitEveryMonth()
{
    return true;
}

bool CDMTimerEventMgr::__InitEveryYear()
{
    return true;
}

void CDMTimerEventMgr::OnTimer(uint64_t qwIDEvent, dm::any& oAny)
{
    auto& v = m_oMapTimerEventSink[(ETimerEventType)qwIDEvent];

    for (auto& it : v)
    {
        it->OnEvent((ETimerEventType)qwIDEvent);
    }
}
