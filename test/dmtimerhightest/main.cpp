#ifndef DMTIMER_USE_HIGH_RESOLUTION
#define DMTIMER_USE_HIGH_RESOLUTION
#endif

#include "dmtimer.h"
#include "dmthread.h"
#include "dmconsole.h"
#include "dmtypes.h"
#include "dmutil.h"
#include <iostream>

class CMain : public IDMConsoleSink,
    public IDMThread,
    public CDMThreadCtrl,
    public ITimerSink
{
public:
    CMain()
        : m_oTimerModule(dmtimerGetModule()),
          m_bStop(false), 
          m_qwOnTimerCount(0)
    {
        HDMConsoleMgr::Instance()->SetHandlerHook(this);
    }
    
    virtual ~CMain()
    {
        std::cout << "CMain destroyed safely." << std::endl;
    }

    typedef enum
    {
        eTimerID_UUID = 0,
        eTimerID_CRON = 1,
        eTimerID_NEW,
        eTimerID_STOP,
    } ETimerID;

    typedef enum
    {
        eTimerTime_UUID = 1000,
        eTimerTime_NEW = 5,
        eTimerTime_STOP = 10000,
    } ETimerTime;

public:
    virtual void ThrdProc() override
    {
        std::cout << "test start" << std::endl;

        dm::any oAny(std::string("hello world"));

        m_oTimerModule->SetTimer(this, eTimerID_UUID, eTimerTime_UUID, eTimerTime_UUID, oAny, false);
        m_oTimerModule->SetTimer(this, eTimerID_NEW, eTimerTime_NEW);
        m_oTimerModule->SetTimer(this, eTimerID_STOP, eTimerTime_STOP);

        bool bBusy = false;
        while (!m_bStop)
        {
            bBusy = false;
            
            if (m_oTimerModule->Run())
            {
                bBusy = true;
            }

            if (__Run())
            {
                bBusy = true;
            }

            if (!m_bStop && !bBusy)
            {
                SleepMs(1);
            }
        }
        std::cout << "test stop" << std::endl;
    }

    virtual void Terminate() override { m_bStop = true; }
    virtual void OnCloseEvent() override { Stop(); }

    virtual void OnTimer(uint64_t qwIDEvent) override {}
    virtual void OnTimer(uint64_t qwIDEvent, dm::any& oAny) override
    {
        switch (qwIDEvent)
        {
        case eTimerID_UUID:
            std::cout << DMFormatDateTime() << " " << GetOnTimerCount() << " " << dm::any_cast<std::string>(oAny) << std::endl;
            break;
        case eTimerID_NEW:
            AddOnTimerCount();
            break;
        case eTimerID_STOP:
            std::cout << DMFormatDateTime() << " test stopping..." << std::endl;
            Stop();
            break;
        default:
            break;
        }
    }

    void AddOnTimerCount() { ++m_qwOnTimerCount; }
    uint64_t GetOnTimerCount() { return m_qwOnTimerCount; }

private:
    bool __Run() { return false; }
private:
    // [关键] 将 timer module 声明在最前面，确保它最后被销毁
    DMTimerPtr m_oTimerModule;

    // 其他成员变量
    volatile bool m_bStop;
    uint64_t m_qwOnTimerCount;
};

int main(int argc, char* argv[])
{
    CMain mainApp;
    mainApp.Start(&mainApp);
    mainApp.WaitFor();

    return 0;
}