
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

#ifndef __DMTIMERSINK_H_INCLUDE__
#define __DMTIMERSINK_H_INCLUDE__

#include "dmtypes.h"
#include "dmos.h"
#include "dmlist.h"
#include "dmany.h"

#define TVN_BITS 6
#define TVR_BITS 8
#define TVN_SIZE (1 << TVN_BITS)
#define TVR_SIZE (1 << TVR_BITS)
#define TVN_MASK (TVN_SIZE - 1)
#define TVR_MASK (TVR_SIZE - 1)

#define TIME_NOT_EQ(x, y) ((x) != (y))
#define INDEX(t, N) ((t >> (TVR_BITS + (N) * TVN_BITS)) & TVN_MASK)

#define DM_MAGIC_USED       (0xcc)
#define DM_MAGIC_UNUSED     (0xcd)

typedef struct tvec {
    struct list_head vec[TVN_SIZE];
} TVec;

typedef struct tvec_root {
    struct list_head vec[TVR_SIZE];
} TVec_Root;

class ITimerSink {
  public:
    virtual ~ITimerSink() = 0;
    virtual void OnTimer( uint64_t qwIDEvent ) = 0;
    virtual void OnTimer( uint64_t qwIDEvent, dm::any& oAny ) {
        OnTimer( qwIDEvent );
    }
};

inline ITimerSink::~ITimerSink() {
}

class CDMTimerElement {
  public:
    CDMTimerElement() {
        Reset();
    }
    ~CDMTimerElement() {
        Reset();
    }

    inline void Reset() {
        m_stEntry.next = NULL;
        m_stEntry.prev = NULL;
        m_qwNextTime = 0;
        m_qwElapse = 0;
        m_qwID = 0;
        m_poTimerSink = NULL;
        m_bErased = false;
        m_bExact = false;
    }

    inline void Kill() {
        m_bErased = true;
    }

  public:
    struct list_head    m_stEntry;

    uint64_t  m_qwNextTime;
    uint64_t  m_qwElapse;
    uint64_t  m_qwID;

    ITimerSink*         m_poTimerSink;

    dm::any             m_oAny;

    bool                m_bErased;
    bool                m_bExact;
};

#endif // __DMTIMERSINK_H_INCLUDE__

