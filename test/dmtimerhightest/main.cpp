
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
        eTimerID_NEW,
        eTimerID_STOP,
    } ETimerID;

    typedef enum
    {
        eTimerTime_UUID = 1000,
        eTimerTime_NEW = 5,
        eTimerTime_STOP = 20000,
    } ETimerTime;

public:
    virtual void ThrdProc()
    {
        std::cout << "test start" << std::endl;


        dm::any oAny(std::string("hello world"));

        SetTimer(eTimerID_UUID, eTimerTime_UUID, std::move(oAny));
        SetTimer(eTimerID_NEW, eTimerTime_NEW);
        SetTimer(eTimerID_STOP, eTimerTime_STOP);

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
        case eTimerID_NEW:
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


    uint64_t m_qwOnTimerCount;
};

int main(int argc, char* argv[])
{
    CMain::Instance()->Start(CMain::Instance());
    CMain::Instance()->WaitFor();
    return 0;
}
