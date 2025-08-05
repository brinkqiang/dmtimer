#ifndef DMTIMER_USE_HIGH_RESOLUTION
#define DMTIMER_USE_HIGH_RESOLUTION
#endif

#include "dmtimer.h"
#include "dmthread.h"
#include "dmconsole.h"
#include "dmtypes.h"
#include "dmutil.h"
#include <iostream>

// [改造] CMain不再是单例，并继承 ITimerSink
class CMain : public IDMConsoleSink,
    public IDMThread,
    public CDMThreadCtrl,
    public ITimerSink
{
public:
    // [改造] 构造函数创建并持有 timer module
    CMain()
        : m_bStop(false), 
          m_qwOnTimerCount(0),
          m_oTimerModule(dmtimerGetModule()) // 遵循您的要求，将模块初始化后置
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

        // [改造] 直接调用模块的SetTimer，并传入this作为Sink
        m_oTimerModule->SetTimer(this, eTimerID_STOP, eTimerTime_STOP, eTimerTime_STOP, dm::any(), false);

        // [改造] Cron定时器，同样传入this作为Sink，Lambda中直接使用this
        m_oTimerModule->SetTimerCron(this, eTimerID_CRON, "0/10 * * ? * MON-FRI", [this](uint64_t qwIDEvent)
        {
            std::cout << DMFormatDateTime() << " " << this->GetOnTimerCount() << " TimerCron" << std::endl;
        });

        dm::any oAny(std::string("hello world"));

        // [改造] Lambda定时器，同样传入this作为Sink，Lambda中直接使用this
        m_oTimerModule->SetTimerLambda(this, eTimerID_UUID, eTimerTime_UUID, eTimerTime_UUID, [this,
            oAny = std::move(oAny)](uint64_t qwIDEvent)
        {
            this->AddOnTimerCount();
            std::cout << DMFormatDateTime() << " " << this->GetOnTimerCount() << " Lambda" << std::endl;
        });

        bool bBusy = false;
        while (!m_bStop)
        {
            bBusy = false;
            
            // [改造] 调用自有模块的 Run
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

    virtual void OnTimer(uint64_t qwIDEvent) override {} // 实现 ITimerSink 的虚函数
    virtual void OnTimer(uint64_t qwIDEvent, dm::any& oAny) override
    {
        switch (qwIDEvent)
        {
        case eTimerID_UUID: // 这个ID现在被Lambda定时器使用，所以此回调不会被触发
        {
            // [改造] 直接调用成员函数
            std::cout << DMFormatDateTime() << " " << GetOnTimerCount() << " " << dm::any_cast<std::string>(oAny) << std::endl;
        }
        break;
        case eTimerID_NEW: // 这个ID未被使用
        {
            AddOnTimerCount();
        }
        break;
        case eTimerID_STOP:
        {
            std::cout << DMFormatDateTime() << " test stopping..." << std::endl;
            Stop();
        }
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
    // [关键] 遵循您的要求，将 timer module 后置。
    // 在此特定版本中，因为没有其他成员或基类在析构时依赖它，所以是安全的。
    volatile bool m_bStop;
    uint64_t m_qwOnTimerCount;
    DMTimerPtr m_oTimerModule;
};

int main(int argc, char* argv[])
{
    // [改造] 在栈上创建 CMain，确保生命周期可控
    CMain mainApp;
    mainApp.Start(&mainApp);
    mainApp.WaitFor();

    return 0;
}