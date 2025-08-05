#ifndef __DMTIMER_H_INCLUDE__
#define __DMTIMER_H_INCLUDE__

#include "dmos.h"
#include "dmmoduleptr.h"
#include "dmany.h"
#include "dmtimersink.h"
#include <functional>
#include <string>

typedef std::function<void(uint64_t qwIDEvent)> DMFunction;

class IDMTimer
{
public:
    virtual ~IDMTimer() {}
    
    virtual void DMAPI Release(void) = 0;
    virtual int DMAPI Run(void) = 0;
    virtual uint64_t DMAPI GetCurTime(void) = 0;
    virtual void DMAPI SetSpeed(uint32_t dwSpeed) = 0;

    // [新增] 用于调试，记录和获取当前正在运行的定时器信息
    virtual void DMAPI SetTimerInfo(uint64_t qwIDEvent, const std::string& strTimerObjName) = 0;
    virtual void DMAPI DelTimerInfo() = 0;
    virtual uint64_t DMAPI GetRunningTimerID() const = 0;
    virtual const std::string& DMAPI GetRunningTimerObjectName() const = 0;

    virtual bool DMAPI SetTimer(ITimerSink* pSink, uint64_t qwIDEvent, uint64_t qwElapse) = 0;
    virtual bool DMAPI SetTimer(ITimerSink* pSink, uint64_t qwIDEvent, uint64_t qwElapse, uint64_t qwFirst, const dm::any& oAny, bool bExact) = 0;
    virtual bool DMAPI SetTimerLambda(ITimerSink* pSink, uint64_t qwIDEvent, uint64_t qwElapse, uint64_t qwFirst, DMFunction fFun) = 0;
    virtual bool DMAPI SetTimerCron(ITimerSink* pSink, uint64_t qwIDEvent, const std::string& strCron, DMFunction fFun) = 0;
    virtual void DMAPI KillTimer(ITimerSink* pSink, uint64_t qwIDEvent) = 0;
    virtual void DMAPI KillAllTimers(ITimerSink* pSink) = 0;
};

typedef DmModulePtr<IDMTimer> DMTimerPtr;

extern "C" DMEXPORT_DLL IDMTimer* DMAPI dmtimerGetModule();
typedef IDMTimer* (DMAPI* PFN_dmtimerGetModule)();

#endif // __DMTIMER_H_INCLUDE__