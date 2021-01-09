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
        && InitEveryDay()
        && InitEveryWeek()
        && InitEveryMonth()
        && InitEveryYear()
        && true;
}

bool CDMTimerEventMgr::InitEveryDay()
{
    return true;
}

bool CDMTimerEventMgr::InitEveryWeek()
{
    return true;
}

bool CDMTimerEventMgr::InitEveryMonth()
{
    return true;
}

bool CDMTimerEventMgr::InitEveryYear()
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
