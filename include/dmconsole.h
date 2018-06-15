
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

#ifndef __DMCONSOLE_H_INCLUDE__
#define __DMCONSOLE_H_INCLUDE__

#include "dmos.h"
#include "dmsingleton.h"

class IDMConsoleSink {
  public:
    virtual ~IDMConsoleSink() {};

    virtual void OnCloseEvent() {};
};

class HDMConsoleMgr : public TSingleton<HDMConsoleMgr> {
    friend class TSingleton<HDMConsoleMgr>;

    typedef void(*consolehandler_t)(int);
  public:
    void SetHandlerHook( IDMConsoleSink* pSink ) {
        m_pConsoleSink = pSink;

        m_phHandler = signal( SIGINT, &HDMConsoleMgr::OnConsoleEvent );

        if ( SIG_ERR == m_phHandler ) {
            DMASSERT( 0 );
        }
    }

    void OnCloseEvent() {
        if ( m_bOnce && m_pConsoleSink ) {
            m_bOnce = false;
            m_pConsoleSink->OnCloseEvent();
        }
    }

    static void OnConsoleEvent( int nEventType ) {
        switch ( nEventType ) {
        case SIGINT:
        case SIGILL:
        case SIGFPE:
        case SIGSEGV:
        case SIGTERM:
        case SIGABRT: {
            HDMConsoleMgr::Instance()->OnCloseEvent();
        }
        break;

        default:
            DMASSERT( 0 );
            break;
        }
    }
  private:

    consolehandler_t m_phHandler;


  public:
    HDMConsoleMgr() {
        m_pConsoleSink = NULL;
        m_bOnce = true;
    }

    ~HDMConsoleMgr() {
    }

  private:
    IDMConsoleSink*   m_pConsoleSink;
    bool            m_bOnce;
};

#endif // __DMCONSOLE_H_INCLUDE__
