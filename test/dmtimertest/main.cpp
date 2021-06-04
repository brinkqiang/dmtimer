
#include "dmutil.h"
#include "dmtimermodule.h"
#include "dmsingleton.h"
#include "dmthread.h"
#include "dmconsole.h"
#include "dmtypes.h"

class CPlayer : public CDMTimerNode
{
public:
    virtual void OnTimer(uint64_t qwIDEvent);
};

class CMain : public IDMConsoleSink,
    public IDMThread,
    public CDMThreadCtrl,
    public CDMTimerNode,
    public TSingleton<CMain>
{
    friend class TSingleton<CMain>;

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
        eTimerID_CRON = 1,
        eTimerID_STOP,
    } ETimerID;

    typedef enum
    {
        eTimerTime_UUID = 1000,
        eTimerTime_STOP = 20000,
    } ETimerTime;

public:
    virtual void ThrdProc()
    {
        std::cout << "test start" << std::endl;

        for (int i = 0; i < eMAX_PLAYER; ++i)
        {
            for (int j = 1; j <= eMAX_PLAYER_EVENT; ++j)
            {
                m_oPlayers[i].SetTimer(j, 500);
            }
        }

        SetTimerCron(eTimerID_CRON, "0/10 * * ? * MON-FRI", [this](uint64_t qwIDEvent)
        {
            auto p = GetTimerElement(qwIDEvent);
            std::cout << DMFormatDateTime() << " " << CMain::Instance()->GetOnTimerCount()
                      << " TimerCron" << std::endl;
        });

        dm::any oAny(std::string("hello world"));

        SetTimerEx(eTimerID_UUID, eTimerTime_UUID, [this,
                   oAny = std::move(oAny)](uint64_t qwIDEvent)
        {
            auto p = GetTimerElement(qwIDEvent);
            std::cout << DMFormatDateTime() << " " << CMain::Instance()->GetOnTimerCount()
                      << " " << dm::any_cast<std::string>(oAny) << std::endl;
        });

        SleepMs(300);
        CDMTimerModule::Instance()->Run();

        SetTimer(eTimerID_STOP, eTimerTime_STOP, eTimerTime_STOP);
        // test interface
        uint64_t qwElapse = GetTimerElapse(eTimerID_UUID);
        std::cout << "test GetTimerElapse: " << qwElapse << std::endl;
        uint64_t qwRemain = GetTimerRemain(eTimerID_UUID);
        std::cout << "test GetTimerRemain: " << qwRemain << std::endl;
        CDMTimerElement* poElement = GetTimerElement(eTimerID_UUID);
        bool bBusy = false;

        while (!m_bStop)
        {
            bBusy = false;

            if (CDMTimerModule::Instance()->Run())
            {
                bBusy = true;
            }

            if (__Run())
            {
                bBusy = true;
            }

            if (!bBusy)
            {
                SleepMs(1);
            }
        }

        std::cout << "test stop" << std::endl;
    }

    virtual void Terminate()
    {
        m_bStop = true;
    }

    virtual void OnCloseEvent()
    {
        Stop();
    }

    virtual void OnTimer(uint64_t qwIDEvent, dm::any& oAny)
    {
        switch (qwIDEvent)
        {
        case eTimerID_UUID:
        {
            std::cout << DMFormatDateTime() << " " << CMain::Instance()->GetOnTimerCount()
                      << " " << dm::any_cast<std::string>(oAny) << std::endl;
        }
        break;

        case eTimerID_STOP:
        {
            //std::cout << DMFormatDateTime() << " test stopping..." << std::endl;
            //Stop();
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
    CMain()
        : m_bStop(false), m_qwOnTimerCount(0)
    {
        HDMConsoleMgr::Instance()->SetHandlerHook(this);
    }

    virtual ~CMain()
    {
    }

private:
    bool __Run()
    {
        return false;
    }

private:
    volatile bool m_bStop;

    CPlayer m_oPlayers[eMAX_PLAYER];

    uint64_t m_qwOnTimerCount;
};

void CPlayer::OnTimer(uint64_t qwIDEvent)
{
    CMain::Instance()->AddOnTimerCount();
}

int main(int argc, char* argv[])
{
    CMain::Instance()->Start(CMain::Instance());
    CMain::Instance()->WaitFor();
    return 0;
}
