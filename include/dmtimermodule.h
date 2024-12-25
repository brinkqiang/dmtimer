
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

#ifndef __DMTIMERMODULE_H_INCLUDE__
#define __DMTIMERMODULE_H_INCLUDE__

#include "dmsingleton.h"
#include "dmrapidpool.h"
#include "dmtimernode.h"

#include <mutex>
#include <chrono>

#if defined(DMTIMER_USE_HIGH_RESOLUTION) && defined(_WIN32)

#include <timeapi.h>

#pragma comment(lib, "winmm.lib")
#endif
#include <iostream>

#ifdef _WIN32
struct timezone {
    int  tz_minuteswest; /* minutes W of Greenwich */
    int  tz_dsttime;     /* type of dst correction */
};

#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64

typedef union {
    uint64_t ft_scalar;
    FILETIME ft_struct;
} FT;

static inline int gettimeofday(struct timeval* tv, struct timezone* tz) {
    FT ft;
    static int tzflag = 0;

    if (NULL != tv) {
        GetSystemTimeAsFileTime(&ft.ft_struct);
        ft.ft_scalar /= 10;
        ft.ft_scalar -= DELTA_EPOCH_IN_MICROSECS;
        tv->tv_sec = (long)(ft.ft_scalar / 1000000UL);
        tv->tv_usec = (long)(ft.ft_scalar % 1000000UL);
    }

    if (NULL != tz) {
        if (!tzflag) {
            _tzset();
            tzflag++;
        }

        tz->tz_minuteswest = _timezone / 60;
        tz->tz_dsttime = _daylight;
    }

    return 0;
}
#else
#include <sys/time.h>
#endif


static inline uint32_t GetTickCount32() {

#if defined(DMTIMER_USE_HIGH_RESOLUTION)

#ifdef _WIN32
	static std::once_flag initializedFlag;
	std::call_once(initializedFlag, []() { 
        timeBeginPeriod(1);
        std::atexit([](){ 
            timeEndPeriod(1);
        });
    });

#endif

	auto now = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
#elif defined(_WIN32)
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

    inline uint32_t End() {
        return GetTickCount32() - m_dwStart;
    }

  private:
    uint32_t m_dwStart;
};


class CDMTimerModule : public CDMSafeSingleton<CDMTimerModule> {
  public:
    friend class CDMSafeSingleton<CDMTimerModule>;

    static const uint32_t MAX_POOL_S = 50000;
    static const uint32_t MAX_POOL_I = 1000;
    static const uint32_t MAX_POOL_COUNT = MAX_POOL_S * MAX_POOL_I;
    static const uint32_t SPEED_DEFAULT = 10000;

  public:
    CDMTimerModule();

    virtual ~CDMTimerModule();

    void Init();
    void UnInit();
    int Run();

    uint64_t GetCurTime();
    void SetSpeed(uint32_t dwSpeed);

    CDMTimerElement* FetchElement();
    void ReleaseElement( CDMTimerElement* pElement );

    void AddTimerElement( CDMTimerElement* pElement );
    void RemoveTimerElement( CDMTimerElement* pElement );
  public:
    uint64_t GetBootTime();

    void    SetTimerInfo(uint64_t qwIDEvent, const std::string& strTimerObjName);
    void    DelTimerInfo();

  private:
    void    __ReleaseElement( struct list_head* head );
    int     __Cascade( TVec* tv, int idx );
    bool    __TimerPending( CDMTimerElement* pElement );

    CDMTimerElement* __GetTimerInfoByEntry( list_head* head );
  private:
    typedef CDynamicRapidPool<CDMTimerElement, MAX_POOL_S, MAX_POOL_I> TimerElementPool;

    TimerElementPool m_oTimerElementPool;

    uint64_t m_qwLastTime;
    uint64_t m_qwCurTime;

    uint32_t m_dwTickCount;
    uint64_t m_qwTotalTickCount;

    TVec_Root   m_tv1;
    TVec        m_tv2;
    TVec        m_tv3;
    TVec        m_tv4;
    TVec        m_tv5;

    uint64_t m_qwIDEvent;
    std::string m_strTimerObjName;
    uint32_t       m_dwSpeed;
};

#endif // __DMTIMERMODULE_H_INCLUDE__

