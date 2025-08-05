#include "dmtimer.h"
#include "dmtimernode.h"
#include "dmthread.h"
#include "dmconsole.h"
#include "dmtypes.h"
#include "dmutil.h"
#include <iostream>

// [改造] 前向声明 CMain，以便 CPlayer 持有其指针
class CMain; 

// [改造] CPlayer 通过指针与 CMain 通信
class CPlayer : public CDMTimerNode
{
public:
    CPlayer() : m_pMain(nullptr) {}
    virtual void OnTimer(uint64_t qwIDEvent) override;
    
    CMain* m_pMain;
};

// [改造] CMain 不再是单例，并继承 ITimerSink
class CMain : public IDMConsoleSink,
    public IDMThread,
    public CDMThreadCtrl,
    public ITimerSink
{


public:
    // [改造] 构造函数创建并持有 timer module
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

    enum
    {
#ifdef _DEBUG
        eMAX_PLAYER = 1 * 10000,
#else
        eMAX_PLAYER = 1 * 10000,
#endif
        eMAX_PLAYER_EVENT = 10,
    };

    typedef enum { eTimerID_UUID = 0, eTimerID_CRON = 1, eTimerID_STOP } ETimerID;
    typedef enum { eTimerTime_UUID = 1000, eTimerTime_STOP = 10000 } ETimerTime;

public:
    virtual void ThrdProc() override
    {
        std::cout << "test start" << std::endl;

        dm::any oAny(std::string("hello world"));

        // [改造] CMain 作为 ITimerSink 直接调用模块的 SetTimer
        m_oTimerModule->SetTimer(this, eTimerID_UUID, eTimerTime_UUID, eTimerTime_UUID, oAny, false);
        m_oTimerModule->SetTimer(this, eTimerID_STOP, eTimerTime_STOP, eTimerTime_STOP, oAny, false);

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
        case eTimerID_UUID:
            // [改造] 不再使用 CMain::Instance()
            std::cout << DMFormatDateTime() << " " << GetOnTimerCount() << " " << dm::any_cast<std::string>(oAny) << std::endl;
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
    // [关键] 将 timer module 声明在最前面，确保它在 CPlayer 数组之后被销毁
    DMTimerPtr m_oTimerModule;

    volatile bool m_bStop;
    CPlayer m_oPlayers[eMAX_PLAYER]; // 声明在 timer module 之后
    uint64_t m_qwOnTimerCount;
};

void CPlayer::OnTimer(uint64_t qwIDEvent)
{
    // [改造] 通过指针回调，而非单例
    if (m_pMain)
    {
        m_pMain->AddOnTimerCount();
    }
}

int main(int argc, char* argv[])
{
    // [改造] 在栈上创建 CMain，确保生命周期可控
    CMain mainApp;
    mainApp.Start(&mainApp);
    mainApp.WaitFor();

    return 0;
}