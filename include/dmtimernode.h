
// Copyright (c) 2018 brinkqiang (brink.qiang@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef __DMTIMERNODE_H_INCLUDE__
#define __DMTIMERNODE_H_INCLUDE__

#include "dmtimersink.h"
#include "dmtimer.h" // 依赖新接口
#include <string>

class CDMTimerNode :
    public ITimerSink
{
public:
    // 构造时传入 IDMTimer 实例
    CDMTimerNode(IDMTimer* pTimerModule = nullptr);
    virtual ~CDMTimerNode();

    // 允许后绑定
    void SetTimerModule(IDMTimer* pTimerModule);
    IDMTimer* GetTimerModule();

    // --- 兼容旧的继承使用方式 ---
    // 这些方法将被代理到持有的 IDMTimer 实例
    bool SetTimer(uint64_t qwIDEvent, uint64_t qwElapse, const dm::any& oAny, bool bExact);
    bool SetTimer(uint64_t qwIDEvent, uint64_t qwElapse, uint64_t qwFirst);
    bool SetTimer(uint64_t qwIDEvent, uint64_t qwElapse);
    bool SetTimerLambda(uint64_t qwIDEvent, uint64_t qwElapse, uint64_t qwFirst, DMFunction fFun);
    bool SetTimerCron(uint64_t qwIDEvent, const std::string& strCron, DMFunction fFun);

    void KillTimer(uint64_t qwIDEvent);
    void KillAllTimers();

    // OnTimer 回调接口保持不变
    virtual void OnTimer(uint64_t qwIDEvent);
    virtual void OnTimer(uint64_t qwIDEvent, dm::any& oAny);

private:
    // 指向定时器模块接口的指针
    IDMTimer* m_pTimerModule;
};

#endif // __DMTIMERNODE_H_INCLUDE__