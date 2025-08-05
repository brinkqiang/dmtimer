#include "dmtimer.h"
#include "dmtimernode.h"
#include "dmthread.h"
#include "dmconsole.h"
#include "dmtypes.h"
#include "dmutil.h"
#include <iostream>

// Forward declare CMain so CPlayer can hold a pointer to it
class CMain; 

// CPlayer现在需要一个指向CMain的指针来回调计数器
class CPlayer : public CDMTimerNode
{
public:
    CPlayer() : m_pMain(nullptr) {}
    virtual void OnTimer(uint64_t qwIDEvent) override;
    
    CMain* m_pMain;
};

class CMain : public IDMConsoleSink,
    public IDMThread,
    public CDMThreadCtrl,
    public ITimerSink
{
public:
    // 构造函数现在接收一个模块指针，并注入给基类
    CMain()
        : m_oTimerModule(dmtimerGetModule()),
          m_bStop(false), m_qwOnTimerCount(0)
    {
        HDMConsoleMgr::Instance()->SetHandlerHook(this);
    }
    
    // 析构函数现在是100%安全的
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

    typedef enum
    {
        eTimerID_UUID = 0,
        eTimerID_STOP = 1,
    } ETimerID;

    typedef enum
    {
        eTimerTime_UUID = 1000,
        eTimerTime_STOP = 10000,
    } ETimerTime;

public:
    virtual void ThrdProc() override
    {
        std::cout << "test start" << std::endl;

        for (int i = 0; i < eMAX_PLAYER; ++i)
        {
            m_oPlayers[i].SetTimerModule(m_oTimerModule.get());
            m_oPlayers[i].m_pMain = this; // 将this指针(CMain实例)交给Player
            for (int j = 1; j <= eMAX_PLAYER_EVENT; ++j)
            {
                m_oPlayers[i].SetTimer(j, 500);
            }
        }

        dm::any oAny(std::string("hello world"));

        m_oTimerModule->SetTimer(this, eTimerID_UUID, eTimerTime_UUID, eTimerTime_UUID, oAny, false);
        m_oTimerModule->SetTimer(this, eTimerID_STOP, eTimerTime_STOP, eTimerTime_STOP, oAny, false);

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

    virtual void Terminate() override
    {
        m_bStop = true;
    }

    virtual void OnCloseEvent() override
    {
        Stop();
    }

    virtual void OnTimer(uint64_t qwIDEvent) override
    {
    }

    virtual void OnTimer(uint64_t qwIDEvent, dm::any& oAny) override
    {
        switch (qwIDEvent)
        {
        case eTimerID_UUID:
        {
            std::cout << DMFormatDateTime() << " " << GetOnTimerCount()
                << " " << dm::any_cast<std::string>(oAny) << std::endl;
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

    void AddOnTimerCount()
    {
        ++m_qwOnTimerCount;
    }
    uint64_t GetOnTimerCount()
    {
        return m_qwOnTimerCount;
    }

private:
    bool __Run() { return false; }

private:
    DMTimerPtr m_oTimerModule;
    volatile bool m_bStop;
    CPlayer m_oPlayers[eMAX_PLAYER];
    uint64_t m_qwOnTimerCount;

};

// CPlayer通过持有的CMain指针来回调，而不是通过不安全的全局单例
void CPlayer::OnTimer(uint64_t qwIDEvent)
{
    if (m_pMain)
    {
        m_pMain->AddOnTimerCount();
    }
}

int main(int argc, char* argv[])
{
    CMain mainApp;
    mainApp.Start(&mainApp);
    mainApp.WaitFor();

    return 0;
}