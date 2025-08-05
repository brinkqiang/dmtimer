#include "dmtimer.h"
#include "dmtimernode.h"
#include "dmthread.h"
#include "dmconsole.h"
#include "dmtypes.h"
#include "dmutil.h"
#include "dmsingleton.h"
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

// [改造] CMain 不再是单例，并继承 ITimerSink 以便自身能接收定时器事件
class CMain : public IDMConsoleSink,
    public IDMThread,
    public CDMThreadCtrl,
    public ITimerSink,
    public CDMSafeSingleton<CMain>
{


public:
    // [改造] 构造函数负责创建并持有 timer module
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
        eMAX_PLAYER = 100 * 10000,
#endif
        eMAX_PLAYER_EVENT = 10,
    };

    typedef enum { eTimerID_UUID = 0, eTimerID_CRON = 1, eTimerID_STOP } ETimerID;
    typedef enum { eTimerTime_UUID = 1000, eTimerTime_STOP = 10000 } ETimerTime;

public:
    virtual void ThrdProc() override
    {
        // [改造] 调用自有模块的 SetSpeed
        m_oTimerModule->SetSpeed(20000);

        std::cout << "test start" << std::endl;

        for (int i = 0; i < eMAX_PLAYER; ++i)
        {
            // [改造] 注入依赖
            m_oPlayers[i].SetTimerModule(m_oTimerModule.get());
            m_oPlayers[i].m_pMain = this;
            for (int j = 1; j <= eMAX_PLAYER_EVENT; ++j)
            {
                m_oPlayers[i].SetTimer(j, 500);
            }
        }

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

    virtual void OnTimer(uint64_t qwIDEvent) override {}
    virtual void OnTimer(uint64_t qwIDEvent, dm::any& oAny) override
    {
        switch (qwIDEvent)
        {
        case eTimerID_UUID:
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
    // [关键] 将 timer module 声明在最前面，确保它最后被销毁
    DMTimerPtr m_oTimerModule;

    // 其他成员变量
    volatile bool m_bStop;
    CPlayer m_oPlayers[eMAX_PLAYER];
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
    CMain::Instance()->Start(CMain::Instance());
    CMain::Instance()->WaitFor();

    return 0;
}