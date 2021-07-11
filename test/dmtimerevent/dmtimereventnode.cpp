#include "dmtimereventnode.h"
#include "dmtimereventmodule.h"

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
