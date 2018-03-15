
#include "dmtimernode.h"
#include "dmtimermodule.h"

CDMTimerNode::CDMTimerNode() {}

CDMTimerNode::~CDMTimerNode() { Reset(); }

CDMTimerNode::CDMTimerNode(const CDMTimerNode &oNode) { CopyFrom(oNode); }

CDMTimerNode &CDMTimerNode::operator=(const CDMTimerNode &oNode) {
    if (this != &oNode) {
        Reset();
        CopyFrom(oNode);
    }

    return *this;
}

void CDMTimerNode::Reset() { KillTimer(); }

void CDMTimerNode::CopyFrom(const CDMTimerNode &oNode) {
    for (TimerElementMapCIt It = oNode.m_oTimerElementMap.begin();
            It != oNode.m_oTimerElementMap.end(); ++It) {
        CDMTimerElement *poNewTimer = CDMTimerModule::Instance()->FetchElement();
        if (NULL == poNewTimer) {
            DMASSERT(0);
            continue;
        }

        *poNewTimer = *(It->second);

        poNewTimer->m_poTimerSink = this;

        CDMTimerModule::Instance()->AddTimerElement(poNewTimer);

        unsigned long long qwIDEvent = poNewTimer->m_qwID;

        TimerElementMapIt It2 = m_oTimerElementMap.find(qwIDEvent);
        if (It2 != m_oTimerElementMap.end()) {
            It2->second->Kill();
            m_oTimerElementMap[qwIDEvent] = poNewTimer;
        }
        else {
            m_oTimerElementMap[qwIDEvent] = poNewTimer;
        }
    }
}

bool CDMTimerNode::SetTimer(unsigned long long qwIDEvent, unsigned long long qwElapse) {
    CDMTimerElement *poNewTimer = CDMTimerModule::Instance()->FetchElement();

    if (NULL == poNewTimer) {
        DMASSERT(0);
        return false;
    }
    poNewTimer->m_poTimerSink = this;

    poNewTimer->m_qwID = qwIDEvent;
    poNewTimer->m_qwElapse = qwElapse;
    poNewTimer->m_bErased = false;
    poNewTimer->m_bExact = false;
    poNewTimer->m_oAny = dm::any();

    poNewTimer->m_qwNextTime = CDMTimerModule::Instance()->GetBootTime() + qwElapse;

    CDMTimerModule::Instance()->AddTimerElement(poNewTimer);

    TimerElementMapIt It = m_oTimerElementMap.find(qwIDEvent);
    if (It != m_oTimerElementMap.end()) {
        It->second->Kill();
        m_oTimerElementMap[qwIDEvent] = poNewTimer;
    }
    else {
        m_oTimerElementMap[qwIDEvent] = poNewTimer;
    }

    return true;
}

bool CDMTimerNode::SetTimer(unsigned long long qwIDEvent,
        unsigned long long qwElapse, const dm::any &oAny,
        bool bExact) {
    CDMTimerElement *poNewTimer = CDMTimerModule::Instance()->FetchElement();

    if (NULL == poNewTimer) {
        DMASSERT(0);
        return false;
    }
    poNewTimer->m_poTimerSink = this;

    poNewTimer->m_qwID = qwIDEvent;
    poNewTimer->m_qwElapse = qwElapse;
    poNewTimer->m_bErased = false;
    poNewTimer->m_bExact = bExact;
    poNewTimer->m_oAny = oAny;

    poNewTimer->m_qwNextTime = CDMTimerModule::Instance()->GetBootTime() + qwElapse;

    CDMTimerModule::Instance()->AddTimerElement(poNewTimer);

    TimerElementMapIt It = m_oTimerElementMap.find(qwIDEvent);
    if (It != m_oTimerElementMap.end()) {
        It->second->Kill();
        m_oTimerElementMap[qwIDEvent] = poNewTimer;
    }
    else {
        m_oTimerElementMap[qwIDEvent] = poNewTimer;
    }

    return true;
}

void CDMTimerNode::KillTimer(unsigned long long qwIDEvent) {
    TimerElementMapIt It = m_oTimerElementMap.find(qwIDEvent);
    if (It == m_oTimerElementMap.end()) {
        return;
    }

    It->second->Kill();
    m_oTimerElementMap.erase(It);
}

void CDMTimerNode::KillTimer() {
    for (TimerElementMapIt It = m_oTimerElementMap.begin();
            It != m_oTimerElementMap.end(); ++It) {
        It->second->Kill();
    }
    m_oTimerElementMap.clear();
}

unsigned long long CDMTimerNode::GetTimerElapse(unsigned long long qwIDEvent) {
    TimerElementMapIt It = m_oTimerElementMap.find(qwIDEvent);
    if (It == m_oTimerElementMap.end()) {
        return 0;
    }
    unsigned long long qwBootTime = CDMTimerModule::Instance()->GetCurTime();
    unsigned long long qwStartTime = It->second->m_qwNextTime - It->second->m_qwElapse;
    return qwBootTime > qwStartTime ? (qwBootTime - qwStartTime) : 0;
}

unsigned long long CDMTimerNode::GetTimerRemain(unsigned long long qwIDEvent) {
    TimerElementMapIt It = m_oTimerElementMap.find(qwIDEvent);
    if (It == m_oTimerElementMap.end()) {
        return 0;
    }

    unsigned long long qwBootTime = CDMTimerModule::Instance()->GetCurTime();
    unsigned long long qwNextTime = It->second->m_qwNextTime;
    return qwNextTime > qwBootTime ? (qwNextTime - qwBootTime) : 0;
}

CDMTimerElement *CDMTimerNode::GetTimerElement(unsigned long long qwIDEvent) {
    TimerElementMapIt It = m_oTimerElementMap.find(qwIDEvent);
    if (It == m_oTimerElementMap.end()) {
        return NULL;
    }
    return It->second;
}

void CDMTimerNode::OnTimer(unsigned long long qwIDEvent) {}

void CDMTimerNode::OnTimer(unsigned long long qwIDEvent, dm::any &oAny) {
    OnTimer(qwIDEvent);
}
