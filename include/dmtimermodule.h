
// Copyright (c) 2018 brinkqiang
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

#ifndef __DMTIMERMODULE_H_INCLUDE__
#define __DMTIMERMODULE_H_INCLUDE__

#include "dmsingleton.h"
#include "dmrapidpool.h"
#include "dmtimernode.h"

#ifdef _MSC_VER
struct timezone {
    int  tz_minuteswest; /* minutes W of Greenwich */
    int  tz_dsttime;     /* type of dst correction */
};

static inline int gettimeofday(struct ::timeval* tv, struct timezone* tz);
#endif

static inline unsigned int GetTickCount32() {
#ifdef _MSC_VER
    return ::GetTickCount();
#else
    struct timespec ts = { 0 };
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#endif
}

class CDMTimeElapse {
  public:
    CDMTimeElapse() {
        Start();
    }

    inline void Start() {
        m_dwStart = GetTickCount32();
    }

    inline unsigned int End() {
        return GetTickCount32() - m_dwStart;
    }

  private:
    unsigned int m_dwStart;
};


class CDMTimerModule : public CDMSafeSingleton<CDMTimerModule> {
  public:
    friend class CDMSafeSingleton<CDMTimerModule>;

    enum {
        eMAX_POOL_S = 50000,
        eMAX_POOL_I = 1000,
        eMAX_TIME_COUNT = eMAX_POOL_S * eMAX_POOL_I,
    };

  public:
    CDMTimerModule();

    virtual ~CDMTimerModule();

    void Init();
    void UnInit();
    int Run();

    uint64_t GetCurTime();

    CDMTimerElement* FetchElement();
    void ReleaseElement( CDMTimerElement* pElement );

    void AddTimerElement( CDMTimerElement* pElement );
    void RemoveTimerElement( CDMTimerElement* pElement );
  public:
    uint64_t GetBootTime();

  private:
    void    __ReleaseElement( struct list_head* head );
    int     __Cascade( TVec* tv, int idx );
    bool    __TimerPending( CDMTimerElement* pElement );

    CDMTimerElement* __GetTimerInfoByEntry( list_head* head );
  private:
    CDynamicRapidPool<CDMTimerElement, eMAX_POOL_S, eMAX_POOL_I>
    m_oTimerElementPool;

    uint64_t m_qwLastTime;
    uint64_t m_qwCurTime;

    unsigned int m_dwTickCount;
    uint64_t m_qwTotalTickCount;

    TVec_Root   m_tv1;
    TVec        m_tv2;
    TVec        m_tv3;
    TVec        m_tv4;
    TVec        m_tv5;
};

#endif // __DMTIMERMODULE_H_INCLUDE__

