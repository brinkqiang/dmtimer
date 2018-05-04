
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

#ifndef __DMTIMERNODE_H_INCLUDE__
#define __DMTIMERNODE_H_INCLUDE__

#include "dmtimersink.h"

#include <string>
#include <map>

class CDMTimerNode :
    public ITimerSink {
  public:
    typedef std::map<uint64_t, CDMTimerElement*> TimerElementMap;
    typedef TimerElementMap::iterator TimerElementMapIt;
    typedef TimerElementMap::const_iterator TimerElementMapCIt;
  public:
    CDMTimerNode();
    virtual ~CDMTimerNode();

    CDMTimerNode( const CDMTimerNode& oNode );
    CDMTimerNode& operator=( const CDMTimerNode& oNode );

    void Reset();
    void CopyFrom( const CDMTimerNode& oNode );

    bool SetTimer( uint64_t qwIDEvent, uint64_t qwElapse );

    bool SetTimer( uint64_t qwIDEvent, uint64_t qwElapse, const dm::any& oAny,
                   bool bExact = false );

    void KillTimer( uint64_t qwIDEvent );

    void KillTimer();

    uint64_t GetTimerElapse( uint64_t qwIDEvent );
    uint64_t GetTimerRemain( uint64_t qwIDEvent );

    CDMTimerElement* GetTimerElement( uint64_t qwIDEvent );

    virtual void OnTimer( uint64_t qwIDEvent );
    virtual void OnTimer( uint64_t qwIDEvent, dm::any& oAny );
  private:
    TimerElementMap m_oTimerElementMap;
};

#endif // __DMTIMERNODE_H_INCLUDE__

