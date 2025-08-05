#ifndef __DMTIMER_H_INCLUDE__
#define __DMTIMER_H_INCLUDE__

#include "dmos.h"
#include "dmmoduleptr.h"
#include "dmany.h"
#include <functional>
#include <string>

// Forward declarations
class ITimerSink;

// 定义定时器回调函数类型
typedef std::function<void(uint64_t qwIDEvent)> DMFunction;

// 核心定时器模块接口
class IDMTimer
{
public:
    virtual ~IDMTimer() {}

    // 释放模块
    virtual void DMAPI Release(void) = 0;

    // 驱动定时器运转，需要在主循环中频繁调用
    virtual int DMAPI Run(void) = 0;

    // 获取模块的当前逻辑时间（毫秒）
    virtual uint64_t DMAPI GetCurTime(void) = 0;

    // --- 解耦方式（推荐）---
    // 参数: pSink, 定时器ID, 间隔, 首次触发时间, any参数, 是否精确(忽略延迟)
    virtual bool DMAPI SetTimer(ITimerSink* pSink, uint64_t qwIDEvent, uint64_t qwElapse, uint64_t qwFirst, const dm::any& oAny, bool bExact) = 0;

    // Lambda 方式
    virtual bool DMAPI SetTimerLambda(ITimerSink* pSink, uint64_t qwIDEvent, uint64_t qwElapse, uint64_t qwFirst, DMFunction fFun) = 0;

    // Cron 方式
    virtual bool DMAPI SetTimerCron(ITimerSink* pSink, uint64_t qwIDEvent, const std::string& strCron, DMFunction fFun) = 0;

    // 销毁指定定时器
    virtual void DMAPI KillTimer(ITimerSink* pSink, uint64_t qwIDEvent) = 0;

    // 销毁某个Sink对象上的所有定时器（重要：Sink对象销毁前必须调用）
    virtual void DMAPI KillAllTimers(ITimerSink* pSink) = 0;
};

// 定义接口的智能指针
typedef DmModulePtr<IDMTimer> DMTimerPtr;

// C-style 工厂函数，用于创建模块实例
extern "C" DMEXPORT_DLL IDMTimer* DMAPI dmtimerGetModule();
typedef IDMTimer* (DMAPI* PFN_dmtimerGetModule)();

#endif // __DMTIMER_H_INCLUDE__