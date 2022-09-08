
#include "dmtimernode.h"
#include "dmtimermodule.h"
#include "dmcroncpp.h"

CDMTimerNode::CDMTimerNode() {}

CDMTimerNode::~CDMTimerNode()
{
    Reset();
}

CDMTimerNode::CDMTimerNode(const CDMTimerNode& oNode)
{
    CopyFrom(oNode);
}

CDMTimerNode& CDMTimerNode::operator=(const CDMTimerNode& oNode)
{
    if (this != &oNode)
    {
        Reset();
        CopyFrom(oNode);
    }

    return *this;
}

void CDMTimerNode::Reset()
{
    KillTimer();
}

void CDMTimerNode::CopyFrom(const CDMTimerNode& oNode)
{
    for (TimerElementMapCIt It = oNode.m_oTimerElementMap.begin();
        It != oNode.m_oTimerElementMap.end(); ++It)
    {
        CDMTimerElement* poNewTimer = CDMTimerModule::Instance()->FetchElement();

        if (NULL == poNewTimer)
        {
            DMASSERT(0);
            continue;
        }

        *poNewTimer = *(It->second);
        poNewTimer->m_poTimerSink = this;
        CDMTimerModule::Instance()->AddTimerElement(poNewTimer);
        uint64_t qwIDEvent = poNewTimer->m_qwID;
        TimerElementMapIt It2 = m_oTimerElementMap.find(qwIDEvent);

        if (It2 != m_oTimerElementMap.end())
        {
            It2->second->Kill();
            m_oTimerElementMap[qwIDEvent] = poNewTimer;
        }
        else
        {
            m_oTimerElementMap[qwIDEvent] = poNewTimer;
        }
    }
}

bool CDMTimerNode::SetTimerCron(uint64_t qwIDEvent, const std::string& strCron,
    DMFunction fFun)
{
    if (strCron.empty())
    {
        DMASSERT(0);
        return false;
    }

    CDMTimerElement* poNewTimer = CDMTimerModule::Instance()->FetchElement();

    if (NULL == poNewTimer)
    {
        DMASSERT(0);
        return false;
    }

    auto cex = cron::make_cron(strCron);
    time_t cur = time(0);
    auto next = cron::cron_next(cex, cur) - cur;

    poNewTimer->m_poTimerSink = this;
    poNewTimer->m_qwID = qwIDEvent;
    poNewTimer->m_bErased = false;

    poNewTimer->m_strCron = strCron;
    poNewTimer->m_funTimer = fFun;

    poNewTimer->m_qwNextTime = CDMTimerModule::Instance()->GetBootTime() + next *
        1000;

    CDMTimerModule::Instance()->AddTimerElement(poNewTimer);
    TimerElementMapIt It = m_oTimerElementMap.find(qwIDEvent);

    if (It != m_oTimerElementMap.end())
    {
        It->second->Kill();
        m_oTimerElementMap[qwIDEvent] = poNewTimer;
    }
    else
    {
        m_oTimerElementMap[qwIDEvent] = poNewTimer;
    }

    return true;
}

bool CDMTimerNode::SetTimer(uint64_t qwIDEvent, uint64_t qwCD)
{
    return SetTimer(qwIDEvent, qwCD, qwCD, dm::any(), false);
}

bool CDMTimerNode::SetTimer(uint64_t qwIDEvent,
    uint64_t qwCD, const dm::any& oAny,
    bool bExact)
{
    return SetTimer(qwIDEvent, qwCD, qwCD, oAny, bExact);
}

bool CDMTimerNode::SetTimer(uint64_t qwIDEvent, uint64_t qwCD,
    uint64_t qwRemain)
{
    return SetTimer(qwIDEvent, qwCD, qwRemain, dm::any(), false);
}

bool CDMTimerNode::SetTimer(uint64_t qwIDEvent, uint64_t qwCD,
    uint64_t qwFirst, const dm::any& oAny,
    bool bExact)
{
    CDMTimerElement* poNewTimer = CDMTimerModule::Instance()->FetchElement();

    if (NULL == poNewTimer)
    {
        DMASSERT(0);
        return false;
    }

    poNewTimer->m_poTimerSink = this;
    poNewTimer->m_qwID = qwIDEvent;
    poNewTimer->m_qwCD = qwCD;
    poNewTimer->m_bErased = false;
    poNewTimer->m_bExact = bExact;
    poNewTimer->m_oAny = oAny;
    poNewTimer->m_qwNextTime = CDMTimerModule::Instance()->GetBootTime() + qwFirst;
    CDMTimerModule::Instance()->AddTimerElement(poNewTimer);
    TimerElementMapIt It = m_oTimerElementMap.find(qwIDEvent);

    if (It != m_oTimerElementMap.end())
    {
        It->second->Kill();
        m_oTimerElementMap[qwIDEvent] = poNewTimer;
    }
    else
    {
        m_oTimerElementMap[qwIDEvent] = poNewTimer;
    }

    return true;
}

bool CDMTimerNode::SetTimerLambda(uint64_t qwIDEvent, uint64_t qwCD,
    DMFunction fFun)
{
    return SetTimerLambda(qwIDEvent, qwCD, qwCD, dm::any(), false, fFun);
}

bool CDMTimerNode::SetTimerLambda(uint64_t qwIDEvent, uint64_t qwCD,
    uint64_t qwFirst, DMFunction fFun)
{
    return SetTimerLambda(qwIDEvent, qwCD, qwFirst, dm::any(), false, fFun);
}

bool CDMTimerNode::SetTimerLambda(uint64_t qwIDEvent, uint64_t qwCD,
    uint64_t qwFirst, const dm::any& oAny,
    bool bExact, DMFunction fFun)
{
    CDMTimerElement* poNewTimer = CDMTimerModule::Instance()->FetchElement();

    if (NULL == poNewTimer)
    {
        DMASSERT(0);
        return false;
    }

    poNewTimer->m_poTimerSink = this;
    poNewTimer->m_qwID = qwIDEvent;
    poNewTimer->m_qwCD = qwCD;
    poNewTimer->m_bErased = false;
    poNewTimer->m_bExact = bExact;
    poNewTimer->m_oAny = oAny;
    poNewTimer->m_qwNextTime = CDMTimerModule::Instance()->GetBootTime() + qwFirst;
    poNewTimer->m_funTimer = fFun;
    CDMTimerModule::Instance()->AddTimerElement(poNewTimer);
    TimerElementMapIt It = m_oTimerElementMap.find(qwIDEvent);

    if (It != m_oTimerElementMap.end())
    {
        It->second->Kill();
        m_oTimerElementMap[qwIDEvent] = poNewTimer;
    }
    else
    {
        m_oTimerElementMap[qwIDEvent] = poNewTimer;
    }

    return true;
}

void CDMTimerNode::KillTimer(uint64_t qwIDEvent)
{
    TimerElementMapIt It = m_oTimerElementMap.find(qwIDEvent);

    if (It == m_oTimerElementMap.end())
    {
        return;
    }

    It->second->Kill();
    m_oTimerElementMap.erase(It);
}

void CDMTimerNode::KillTimer()
{
    for (TimerElementMapIt It = m_oTimerElementMap.begin();
        It != m_oTimerElementMap.end(); ++It)
    {
        It->second->Kill();
    }

    m_oTimerElementMap.clear();
}


bool CDMTimerNode::PauseTimer(uint64_t qwIDEvent)
{
    TimerElementMapIt It = m_oTimerElementMap.find(qwIDEvent);

    if (It == m_oTimerElementMap.end())
    {
        return false;
    }

    It->second->PauseTimer();
    return true;
}

bool CDMTimerNode::ResumeTimer(uint64_t qwIDEvent)
{
    TimerElementMapIt It = m_oTimerElementMap.find(qwIDEvent);

    if (It == m_oTimerElementMap.end())
    {
        return false;
    }

    It->second->ResumeTimer();
    return true;
}

uint64_t CDMTimerNode::GetTimerElapse(uint64_t qwIDEvent)
{
    TimerElementMapIt It = m_oTimerElementMap.find(qwIDEvent);

    if (It == m_oTimerElementMap.end())
    {
        return 0;
    }

    uint64_t qwBootTime = CDMTimerModule::Instance()->GetCurTime();
    uint64_t qwStartTime = It->second->m_qwNextTime - It->second->m_qwCD;
    return qwBootTime > qwStartTime ? (qwBootTime - qwStartTime) : 0;
}

uint64_t CDMTimerNode::GetTimerRemain(uint64_t qwIDEvent)
{
    TimerElementMapIt It = m_oTimerElementMap.find(qwIDEvent);

    if (It == m_oTimerElementMap.end())
    {
        return 0;
    }

    uint64_t qwBootTime = CDMTimerModule::Instance()->GetCurTime();
    uint64_t qwNextTime = It->second->m_qwNextTime;
    return qwNextTime > qwBootTime ? (qwNextTime - qwBootTime) : 0;
}

CDMTimerElement* CDMTimerNode::GetTimerElement(uint64_t qwIDEvent)
{
    TimerElementMapIt It = m_oTimerElementMap.find(qwIDEvent);

    if (It == m_oTimerElementMap.end())
    {
        return NULL;
    }

    return It->second;
}

void CDMTimerNode::OnTimer(uint64_t qwIDEvent) {}

void CDMTimerNode::OnTimer(uint64_t qwIDEvent, dm::any& oAny)
{
    OnTimer(qwIDEvent);
}
