#include "dmtimerevent.h"



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
//
//CDMTimerEventGuard::CDMTimerEventGuard(ITimerEventSink* poSink,
//                                       ETimerEventType eType)
//    : m_poSink(poSink), m_eType(eType)
//{
//    CDMTimerEventMgr::Instance()->AddTimerEventSink(m_poSink, m_eType);
//}
//
//CDMTimerEventGuard::~CDMTimerEventGuard()
//{
//    CDMTimerEventMgr::Instance()->DelTimerEventSink(m_poSink, m_eType);
//}

CDMTimerEventNode::CDMTimerEventNode()
{

}

CDMTimerEventNode::~CDMTimerEventNode()
{

}

bool CDMTimerEventNode::AddEvent(ETimerEventType eType)
{
    auto it = m_oSetEventType.find(eType);

    if (it != m_oSetEventType.end())
    {
        return false;
    }

    m_oSetEventType.insert(eType);

    CDMTimerEventMgr::Instance()->AddTimerEventSink(this, eType);

    return true;
}

bool CDMTimerEventNode::DelEvent(ETimerEventType eType)
{
    auto it = m_oSetEventType.find(eType);

    if (it == m_oSetEventType.end())
    {
        return false;
    }

    CDMTimerEventMgr::Instance()->DelTimerEventSink(this, eType);

    m_oSetEventType.erase(it);
    return true;
}

void CDMTimerEventNode::OnEvent(ETimerEventType eEvent)
{

}
