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

void CDMTimerEventMgr::OnTimer(uint64_t qwIDEvent,  dm::any& oAny )
{
    auto& v = m_oMapTimerEventSink[(ETimerEventType)qwIDEvent];

    for (auto& it : v)
    {
        it->OnEvent((ETimerEventType)qwIDEvent);
    }
}
