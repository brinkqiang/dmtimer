#include "dmtimernode.h"

CDMTimerNode::CDMTimerNode(IDMTimer* pTimerModule)
    : m_pTimerModule(pTimerModule)
{
}

CDMTimerNode::~CDMTimerNode()
{
    // Node析构时，清理其在TimerModule中注册的所有定时器
    KillAllTimers();
}

void CDMTimerNode::SetTimerModule(IDMTimer* pTimerModule)
{
    m_pTimerModule = pTimerModule;
}

IDMTimer* CDMTimerNode::GetTimerModule() { return m_pTimerModule;}

// --- 代理实现 ---
bool CDMTimerNode::SetTimer(uint64_t qwIDEvent, uint64_t qwElapse, const dm::any& oAny, bool bExact)
{
    if (!m_pTimerModule) return false;
    // 调用模块接口，并将 this 作为 Sink 参数传入
    return m_pTimerModule->SetTimer(this, qwIDEvent, qwElapse, qwElapse, oAny, bExact);
}

bool CDMTimerNode::SetTimer(uint64_t qwIDEvent, uint64_t qwElapse, uint64_t qwFirst)
{
    if (!m_pTimerModule) return false;
    return m_pTimerModule->SetTimer(this, qwIDEvent, qwElapse, qwFirst, dm::any(), false);
}

bool CDMTimerNode::SetTimer(uint64_t qwIDEvent, uint64_t qwElapse)
{
    if (!m_pTimerModule) return false;
    return m_pTimerModule->SetTimer(this, qwIDEvent, qwElapse, qwElapse, dm::any(), false);
}

bool CDMTimerNode::SetTimerLambda(uint64_t qwIDEvent, uint64_t qwElapse, uint64_t qwFirst, DMFunction fFun)
{
    if (!m_pTimerModule) return false;
    return m_pTimerModule->SetTimerLambda(this, qwIDEvent, qwElapse, qwFirst, fFun);
}

bool CDMTimerNode::SetTimerCron(uint64_t qwIDEvent, const std::string& strCron, DMFunction fFun)
{
    if (!m_pTimerModule) return false;
    return m_pTimerModule->SetTimerCron(this, qwIDEvent, strCron, fFun);
}

void CDMTimerNode::KillTimer(uint64_t qwIDEvent)
{
    if (m_pTimerModule)
    {
        m_pTimerModule->KillTimer(this, qwIDEvent);
    }
}

void CDMTimerNode::KillAllTimers()
{
    if (m_pTimerModule)
    {
        m_pTimerModule->KillAllTimers(this);
    }
}

// --- 回调实现 ---
void CDMTimerNode::OnTimer(uint64_t qwIDEvent) 
{
    // Default implementation
}

void CDMTimerNode::OnTimer(uint64_t qwIDEvent, dm::any& oAny)
{
    // Default implementation, calls the simpler version
    OnTimer(qwIDEvent);
}