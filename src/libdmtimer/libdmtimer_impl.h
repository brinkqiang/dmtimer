#ifndef __LIBDMTIMER_IMPL_H_INCLUDE__
#define __LIBDMTIMER_IMPL_H_INCLUDE__

#include "dmtimer.h"
#include "dmtimersink.h"
#include "dmrapidpool.h"
#include <map>
#include <string>

class DMTimerImpl : public IDMTimer
{
public:
    DMTimerImpl();
    virtual ~DMTimerImpl();

public: // IDMTimer interface
    virtual void DMAPI Release(void) override;
    virtual int DMAPI Run(void) override;
    virtual uint64_t DMAPI GetCurTime(void) override;

    virtual bool DMAPI SetTimer(ITimerSink* pSink, uint64_t qwIDEvent, uint64_t qwElapse, uint64_t qwFirst, const dm::any& oAny, bool bExact) override;
    virtual bool DMAPI SetTimerLambda(ITimerSink* pSink, uint64_t qwIDEvent, uint64_t qwElapse, uint64_t qwFirst, DMFunction fFun) override;
    virtual bool DMAPI SetTimerCron(ITimerSink* pSink, uint64_t qwIDEvent, const std::string& strCron, DMFunction fFun) override;

    virtual void DMAPI KillTimer(ITimerSink* pSink, uint64_t qwIDEvent) override;
    virtual void DMAPI KillAllTimers(ITimerSink* pSink) override;

private: // private helpers
    void Init();
    void UnInit();
    uint64_t GetBootTime();
    void AddTimerElement(CDMTimerElement* pElement);
    bool TimerPending(CDMTimerElement* pElement);
    void RemoveTimerElementFromWheel(CDMTimerElement* pElement);
    CDMTimerElement* FetchElement();
    void ReleaseElement(CDMTimerElement* pElement);

    void __ReleaseElementList(struct list_head* head);
    int __Cascade(TVec* tv, int idx);
    CDMTimerElement* __GetTimerInfoByEntry(list_head* head);

private:
    typedef CDynamicRapidPool<CDMTimerElement, 50000, 1000> TimerElementPool;
    typedef std::map<uint64_t, CDMTimerElement*> TimerElementMap;

    std::map<ITimerSink*, TimerElementMap> m_oAllTimers;

    TVec_Root m_tv1;
    TVec m_tv2, m_tv3, m_tv4, m_tv5;

    TimerElementPool m_oTimerElementPool;
    uint64_t m_qwLastTime;
    uint64_t m_qwCurTime;
    uint32_t m_dwTickCount;
    uint64_t m_qwTotalTickCount;
    uint32_t m_dwSpeed;
};

#endif // __LIBDMTIMER_IMPL_H_INCLUDE__