#include "libdmtimer_impl.h"
#include "dmcroncpp.h"
#include <chrono>
#include <typeinfo> // 需要包含此头文件以使用 typeid

#if defined(_WIN32)
#include <windows.h>
#else
#include <sys/time.h>
#endif

// Helper to get tick count
static inline uint32_t GetTickCount32() {

#if defined(DMTIMER_USE_HIGH_RESOLUTION)
  #if defined(_WIN32) && !defined(__MINGW32__)
    static std::once_flag initializedFlag;
    std::call_once(initializedFlag, []() { 
          timeBeginPeriod(1);
          std::atexit([](){ 
              timeEndPeriod(1);
          });
      });
  #endif
#endif

#if defined(_WIN32) && !defined(__MINGW32__)
    return ::GetTickCount();
#else
    struct timespec ts = { 0 };
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#endif
}

extern "C" DMEXPORT_DLL IDMTimer* DMAPI dmtimerGetModule()
{
    return new DMTimerImpl();
}

DMTimerImpl::DMTimerImpl() 
{ 
    Init(); 
}

DMTimerImpl::~DMTimerImpl() 
{ 
    UnInit(); 
}

void DMTimerImpl::Release(void) 
{ 
    delete this; 
}

void DMTimerImpl::SetTimerInfo(uint64_t qwIDEvent, const std::string& strTimerObjName)
{
    m_qwRunningIDEvent = qwIDEvent;
    m_strRunningTimerObjName = strTimerObjName;
}

void DMTimerImpl::DelTimerInfo()
{
    m_qwRunningIDEvent = 0;
    m_strRunningTimerObjName.clear();
}

uint64_t DMTimerImpl::GetRunningTimerID() const
{
    return m_qwRunningIDEvent;
}

const std::string& DMTimerImpl::GetRunningTimerObjectName() const
{
    return m_strRunningTimerObjName;
}

void DMTimerImpl::SetSpeed(uint32_t dwSpeed)
{
    m_dwSpeed = dwSpeed;
}

void DMTimerImpl::Init()
{
    for (int j = 0; j < TVR_SIZE; ++j) { INIT_LIST_HEAD(m_tv1.vec + j); }
    for (int i = 0; i < TVN_SIZE; ++i)
    {
        INIT_LIST_HEAD(m_tv2.vec + i);
        INIT_LIST_HEAD(m_tv3.vec + i);
        INIT_LIST_HEAD(m_tv4.vec + i);
        INIT_LIST_HEAD(m_tv5.vec + i);
    }
    m_dwSpeed = 10000;
    m_dwTickCount = GetTickCount32();
    m_qwTotalTickCount = 0;
    m_qwLastTime = GetBootTime();
    m_qwCurTime = GetBootTime();
    m_qwRunningIDEvent = 0;
    m_strRunningTimerObjName.clear();
}

void DMTimerImpl::UnInit()
{
    for (int j = 0; j < TVN_SIZE; ++j)
    {
        __ReleaseElementList(m_tv5.vec + j);
        __ReleaseElementList(m_tv4.vec + j);
        __ReleaseElementList(m_tv3.vec + j);
        __ReleaseElementList(m_tv2.vec + j);
    }
    for (int i = 0; i < TVR_SIZE; ++i)
    {
        __ReleaseElementList(m_tv1.vec + i);
    }
}

void DMTimerImpl::__ReleaseElementList(struct list_head* head)
{
    struct list_head work_list;
    list_replace_init(head, &work_list);
    while (!list_empty(&work_list))
    {
        CDMTimerElement* timer = __GetTimerInfoByEntry(work_list.next);
        RemoveTimerElementFromWheel(timer);
        ReleaseElement(timer);
    }
}

uint64_t DMTimerImpl::GetCurTime(void) 
{ 
    return m_qwCurTime; 
}

uint64_t DMTimerImpl::GetBootTime()
{
    uint32_t dwCurTime = GetTickCount32();
    uint32_t dwRealPassedTime = dwCurTime - m_dwTickCount;
    m_dwTickCount = dwCurTime;
    uint64_t qwPassedTime = static_cast<uint64_t>(dwRealPassedTime) * m_dwSpeed / 10000;
    m_qwTotalTickCount += qwPassedTime;
    return m_qwTotalTickCount;
}

CDMTimerElement* DMTimerImpl::FetchElement() { return m_oTimerElementPool.FetchObj(); }
void DMTimerImpl::ReleaseElement(CDMTimerElement* pElement) { m_oTimerElementPool.ReleaseObj(pElement); }
bool DMTimerImpl::TimerPending(CDMTimerElement* pElement) { return pElement->m_stEntry.next != NULL; }

void DMTimerImpl::RemoveTimerElementFromWheel(CDMTimerElement* pElement)
{
    if (TimerPending(pElement))
    {
        list_del(&pElement->m_stEntry);
        pElement->m_stEntry.next = NULL;
        pElement->m_stEntry.prev = NULL;
    }
}

void DMTimerImpl::AddTimerElement(CDMTimerElement* pElement)
{
    uint64_t qwExpires = pElement->m_qwNextTime;
    uint64_t idx = static_cast<uint64_t>(qwExpires - m_qwLastTime);
    struct list_head* vec = NULL;

    if (idx < TVR_SIZE) { vec = m_tv1.vec + (qwExpires & TVR_MASK); }
    else if (idx < (1 << (TVR_BITS + TVN_BITS))) { vec = m_tv2.vec + ((qwExpires >> TVR_BITS) & TVN_MASK); }
    else if (idx < (1 << (TVR_BITS + 2 * TVN_BITS))) { vec = m_tv3.vec + ((qwExpires >> (TVR_BITS + TVN_BITS)) & TVN_MASK); }
    else if (idx < (1 << (TVR_BITS + 3 * TVN_BITS))) { vec = m_tv4.vec + ((qwExpires >> (TVR_BITS + 2 * TVN_BITS)) & TVN_MASK); }
    else if ((int64_t)idx < 0) { vec = m_tv1.vec + (m_qwLastTime & TVR_MASK); }
    else
    {
        if (idx > 0xffffffffUL)
        {
            idx = 0xffffffffUL;
            qwExpires = idx + m_qwLastTime;
        }
        vec = m_tv5.vec + ((qwExpires >> (TVR_BITS + 3 * TVN_BITS)) & TVN_MASK);
    }
    list_add_tail(&pElement->m_stEntry, vec);
}

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"
#endif
CDMTimerElement* DMTimerImpl::__GetTimerInfoByEntry(struct list_head* head)
{
    return (CDMTimerElement*)((char*)head - offsetof(CDMTimerElement, m_stEntry));
}
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

int DMTimerImpl::__Cascade(TVec* tv, int idx)
{
    struct list_head tv_list;
    list_replace_init(tv->vec + idx, &tv_list);
    while (!list_empty(&tv_list))
    {
        CDMTimerElement* pTimer = __GetTimerInfoByEntry(tv_list.next);
        RemoveTimerElementFromWheel(pTimer);
        AddTimerElement(pTimer);
    }
    return idx;
}

int DMTimerImpl::Run(void)
{
    int nEvents = 0;
    m_qwCurTime = GetBootTime();

    while (DM_TIME_NOT_EQ(m_qwCurTime, m_qwLastTime))
    {
        int index = m_qwLastTime & TVR_MASK;
        if (!index && (!__Cascade(&m_tv2, DM_INDEX(m_qwLastTime, 0))) && (!__Cascade(&m_tv3, DM_INDEX(m_qwLastTime, 1))) && (!__Cascade(&m_tv4, DM_INDEX(m_qwLastTime, 2))))
        {
            __Cascade(&m_tv5, DM_INDEX(m_qwLastTime, 3));
        }

        struct list_head work_list;
        list_replace_init(m_tv1.vec + index, &work_list);

        while (!list_empty(&work_list))
        {
            CDMTimerElement* timer = __GetTimerInfoByEntry(work_list.next);
            
            RemoveTimerElementFromWheel(timer);
            if (timer->m_bErased)
            {
                ReleaseElement(timer);
                continue;
            }

            SetTimerInfo(timer->m_qwID, typeid(*(timer->m_poTimerSink)).name());

            if (timer->m_funTimer)
            {
                timer->m_funTimer(timer->m_qwID);
            }
            else
            {
                timer->m_poTimerSink->OnTimer(timer->m_qwID, timer->m_oAny);
            }
            
            DelTimerInfo();
            
            nEvents++;

            if (timer->m_bErased)
            {
                ReleaseElement(timer);
                continue;
            }

            if (timer->m_strCron.empty())
            {
                timer->m_qwNextTime += timer->m_qwCD;

                if (!timer->m_bExact && m_qwCurTime >= timer->m_qwNextTime)
                {
                    timer->m_qwNextTime = m_qwCurTime + timer->m_qwCD;
                }
            }
            else
            {
                auto cex = cron::make_cron(timer->m_strCron);
                time_t cur = time(0);
                auto next = cron::cron_next(cex, cur) - cur;
                timer->m_qwNextTime = m_qwCurTime + next * 1000;
            }
            AddTimerElement(timer);
        }
        ++m_qwLastTime;
    }
    return nEvents;
}

bool DMTimerImpl::SetTimer(ITimerSink* pSink, uint64_t qwIDEvent, uint64_t qwElapse)
{
    return SetTimer(pSink, qwIDEvent, qwElapse, qwElapse, dm::any(), false);
}

bool DMTimerImpl::SetTimer(ITimerSink* pSink, uint64_t qwIDEvent, uint64_t qwElapse, uint64_t qwFirst, const dm::any& oAny, bool bExact)
{
    if (!pSink) return false;

    auto& timer_map = m_oAllTimers[pSink];
    auto timer_it = timer_map.find(qwIDEvent);
    if (timer_it != timer_map.end())
    {
        timer_it->second->Kill();
        timer_map.erase(timer_it);
    }
    
    CDMTimerElement* poNewTimer = FetchElement();
    if (!poNewTimer) return false;

    poNewTimer->Reset();
    poNewTimer->m_poTimerSink = pSink;
    poNewTimer->m_qwID = qwIDEvent;
    poNewTimer->m_qwCD = qwElapse;
    poNewTimer->m_qwNextTime = GetBootTime() + qwFirst;
    poNewTimer->m_oAny = oAny;
    poNewTimer->m_bExact = bExact;

    timer_map[qwIDEvent] = poNewTimer;
    AddTimerElement(poNewTimer);

    return true;
}

bool DMTimerImpl::SetTimerLambda(ITimerSink* pSink, uint64_t qwIDEvent, uint64_t qwElapse, uint64_t qwFirst, DMFunction fFun)
{
    if (!pSink || !fFun) return false;
    KillTimer(pSink, qwIDEvent);

    CDMTimerElement* poNewTimer = FetchElement();
    if (!poNewTimer) return false;
    
    poNewTimer->Reset();
    poNewTimer->m_poTimerSink = pSink;
    poNewTimer->m_qwID = qwIDEvent;
    poNewTimer->m_qwCD = qwElapse;
    poNewTimer->m_qwNextTime = GetBootTime() + qwFirst;
    poNewTimer->m_funTimer = fFun;

    m_oAllTimers[pSink][qwIDEvent] = poNewTimer;
    AddTimerElement(poNewTimer);
    return true;
}

bool DMTimerImpl::SetTimerCron(ITimerSink* pSink, uint64_t qwIDEvent, const std::string& strCron, DMFunction fFun)
{
    if (!pSink || strCron.empty() || !fFun) return false;
    KillTimer(pSink, qwIDEvent);

    CDMTimerElement* poNewTimer = FetchElement();
    if (!poNewTimer) return false;

    auto cex = cron::make_cron(strCron);
    time_t cur = time(0);
    auto next = cron::cron_next(cex, cur) - cur;

    poNewTimer->Reset();
    poNewTimer->m_poTimerSink = pSink;
    poNewTimer->m_qwID = qwIDEvent;
    poNewTimer->m_strCron = strCron;
    poNewTimer->m_funTimer = fFun;
    poNewTimer->m_qwNextTime = GetBootTime() + next * 1000;
    
    m_oAllTimers[pSink][qwIDEvent] = poNewTimer;
    AddTimerElement(poNewTimer);
    return true;
}

void DMTimerImpl::KillTimer(ITimerSink* pSink, uint64_t qwIDEvent)
{
    if (!pSink) return;

    auto sink_it = m_oAllTimers.find(pSink);
    if (sink_it != m_oAllTimers.end())
    {
        auto& timer_map = sink_it->second;
        auto timer_it = timer_map.find(qwIDEvent);
        if (timer_it != timer_map.end())
        {
            timer_it->second->Kill();
            timer_map.erase(timer_it);
        }
    }
}

void DMTimerImpl::KillAllTimers(ITimerSink* pSink)
{
    if (!pSink) return;

    auto sink_it = m_oAllTimers.find(pSink);
    if (sink_it != m_oAllTimers.end())
    {
        for (auto const& [id, element] : sink_it->second)
        {
            element->Kill();
        }
        m_oAllTimers.erase(sink_it);
    }
}