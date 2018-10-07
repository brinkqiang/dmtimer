
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

#ifndef __DMTHREAD_H_INCLUDE__
#define __DMTHREAD_H_INCLUDE__

#include "dmos.h"

#include <thread>
#include <memory>

class IDMThread {
  public:
    virtual ~IDMThread() {}
    virtual void ThrdProc() = 0;
    virtual void Terminate() = 0;
};

class IDMThreadCtrl {
  public:
    virtual ~IDMThreadCtrl() {}
    virtual void Resume() = 0;
    virtual void Suspend() = 0;
    virtual void Stop() = 0;
    virtual bool Kill(unsigned int dwExitCode) = 0;
    virtual bool WaitFor() = 0;
    virtual std::thread::id GetThreadID() = 0;
    virtual IDMThread* GetThread() = 0;
    virtual void Release() = 0;
};

class CDMThreadCtrl : public IDMThreadCtrl {
  public:
    CDMThreadCtrl() {
        m_bIsStop = true;
        m_bNeedWaitFor = true;
        m_poThread = NULL;
    }

    virtual ~CDMThreadCtrl() {
    }

  public:
    virtual void Resume(void) {

    }

    virtual void Suspend() {

    }

    virtual void Stop(void) {
        m_poThread->Terminate();
    }

    virtual bool Kill(unsigned int dwExitCode) {
        return true;
    }

    virtual bool WaitFor() {
        if (m_oThread->joinable()) {
            m_oThread->join();
        }

        return true;
    }

    virtual void Release(void) {
        delete this;
    }

    virtual std::thread::id GetThreadID(void) {
        return m_oThread->get_id();
    }

    virtual IDMThread* GetThread(void) {
        return m_poThread;
    }

    static void* StaticThreadFunc(void* arg) {
        CDMThreadCtrl* poCtrl = (CDMThreadCtrl*)arg;
        poCtrl->m_bIsStop = false;

        poCtrl->m_poThread->ThrdProc();
        return 0;
    }

    bool Start(IDMThread* poThread, bool bNeedWaitFor = true) {
        m_poThread = poThread;

        m_bNeedWaitFor = bNeedWaitFor;

        std::unique_ptr<std::thread> oThread(new std::thread(StaticThreadFunc, this));

        m_oThread = std::move(oThread);

        if (!bNeedWaitFor) {
            m_oThread->detach();
        }

        return true;
    }

  protected:
    volatile bool   m_bIsStop;
    volatile bool   m_bNeedWaitFor;
    std::unique_ptr<std::thread> m_oThread;
    IDMThread*      m_poThread;
};

inline IDMThreadCtrl* CreateThreadCtrl() {
    return new CDMThreadCtrl;
}

#endif // __DMTHREAD_H_INCLUDE__
