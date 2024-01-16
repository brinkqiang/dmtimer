# dmtimer

Copyright (c) 2013-2018 brinkqiang (brink.qiang@gmail.com)

[![dmtimer](https://img.shields.io/badge/brinkqiang-dmtimer-blue.svg?style=flat-square)](https://github.com/brinkqiang/dmtimer)
[![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](https://github.com/brinkqiang/dmtimer/blob/master/LICENSE)
[![blog](https://img.shields.io/badge/Author-Blog-7AD6FD.svg)](https://brinkqiang.github.io/)
[![Open Source Love](https://badges.frapsoft.com/os/v3/open-source.png)](https://github.com/brinkqiang)
[![GitHub stars](https://img.shields.io/github/stars/brinkqiang/dmtimer.svg?label=Stars)](https://github.com/brinkqiang/dmtimer) 
[![GitHub forks](https://img.shields.io/github/forks/brinkqiang/dmtimer.svg?label=Fork)](https://github.com/brinkqiang/dmtimer)

## Build status
| [Linux][lin-link] | [Mac][mac-link] | [Windows][win-link] |
| :---------------: | :----------------: | :-----------------: |
| ![lin-badge]      | ![mac-badge]       | ![win-badge]        |

[lin-badge]: https://github.com/brinkqiang/dmtimer/workflows/linux/badge.svg "linux build status"
[lin-link]:  https://github.com/brinkqiang/dmtimer/actions/workflows/linux.yml "linux build status"
[mac-badge]: https://github.com/brinkqiang/dmtimer/workflows/mac/badge.svg "mac build status"
[mac-link]:  https://github.com/brinkqiang/dmtimer/actions/workflows/mac.yml "mac build status"
[win-badge]: https://github.com/brinkqiang/dmtimer/workflows/win/badge.svg "win build status"
[win-link]:  https://github.com/brinkqiang/dmtimer/actions/workflows/win.yml "win build status"

## Intro
C++ high performance timer
```cpp

#include "dmutil.h"
#include "dmtimermodule.h"
#include "dmsingleton.h"
#include "dmthread.h"
#include "dmconsole.h"
#include "dmtypes.h"

class CPlayer : public CDMTimerNode {
  public:
    virtual void OnTimer( uint64_t qwIDEvent );
};

class CMain :
    public IDMConsoleSink,
    public IDMThread,
    public CDMThreadCtrl,
    public CDMTimerNode,
    public TSingleton<CMain> {
    friend class TSingleton<CMain>;

    enum {
        eMAX_PLAYER = 100 * 10000,
        eMAX_PLAYER_EVENT = 10,
    };

    typedef enum {
        eTimerID_UUID = 0,
        eTimerID_STOP,
    } ETimerID;

    typedef enum {
        eTimerTime_UUID = 1000,
        eTimerTime_STOP = 10000,
    } ETimerTime;

  public:

    virtual void ThrdProc() {
        std::cout << "test start" << std::endl;

        for ( int i = 0; i < eMAX_PLAYER; ++i ) {
            for ( int j = 1; j <= eMAX_PLAYER_EVENT; ++j ) {
                m_oPlayers[i].SetTimer( j, 500 );
            }
        }

        SetTimer( eTimerID_UUID, eTimerTime_UUID,
                  dm::any( std::string( "hello world" ) ) );
        SleepMs( 300 );
        CDMTimerModule::Instance()->Run();

        SetTimer(eTimerID_STOP, eTimerTime_STOP);
        // test interface
        uint64_t qwElapse = GetTimerElapse( eTimerID_UUID );
        std::cout << "test GetTimerElapse: " << qwElapse << std::endl;
        uint64_t qwRemain = GetTimerRemain( eTimerID_UUID );
        std::cout << "test GetTimerRemain: " << qwRemain << std::endl;
        CDMTimerElement* poElement = GetTimerElement( eTimerID_UUID );
        bool bBusy = false;

        while ( !m_bStop ) {
            bBusy = false;

            if ( CDMTimerModule::Instance()->Run() ) {
                bBusy = true;
            }

            if ( __Run() ) {
                bBusy = true;
            }

            if ( !bBusy ) {
                SleepMs( 1 );
            }
        }

        std::cout << "test stop" << std::endl;
    }

    virtual void Terminate() {
        m_bStop = true;
    }

    virtual void OnCloseEvent() {
        Stop();
    }

    virtual void OnTimer( uint64_t qwIDEvent, dm::any& oAny ) {
        switch ( qwIDEvent ) {
        case eTimerID_UUID: {
            std::cout << DMFormatDateTime() << " " << CMain::Instance()->GetOnTimerCount()
                      << " " << dm::any_cast<std::string>( oAny ) << std::endl;
        }
        break;

        case eTimerID_STOP: {
            std::cout << DMFormatDateTime() << " test stopping..." << std::endl;
            Stop();
        }
        break;

        default:
            break;
        }
    }

    void AddOnTimerCount() {
        ++m_qwOnTimerCount;
    }
    uint64_t GetOnTimerCount() {
        return m_qwOnTimerCount;
    }
  private:
    CMain()
        : m_bStop( false ), m_qwOnTimerCount( 0 ) {
        HDMConsoleMgr::Instance()->SetHandlerHook( this );
    }

    virtual ~CMain() {

    }

  private:
    bool __Run() {
        return false;
    }
  private:
    volatile bool   m_bStop;

    CPlayer m_oPlayers[eMAX_PLAYER];

    uint64_t  m_qwOnTimerCount;
};

void CPlayer::OnTimer( uint64_t qwIDEvent ) {
    CMain::Instance()->AddOnTimerCount();
}

int main( int argc, char* argv[] ) {
    CMain::Instance()->Start( CMain::Instance());
    CMain::Instance()->WaitFor();
    return 0;
}

```
## API test
```
Running main() from gtest_main.cc
[==========] Running 7 tests from 7 test cases.
[----------] Global test environment set-up.
[----------] 1 test from clock_realtime_coarse
[ RUN      ] clock_realtime_coarse.clock_realtime_coarse
185306418897129146
[       OK ] clock_realtime_coarse.clock_realtime_coarse (625 ms)
[----------] 1 test from clock_realtime_coarse (630 ms total)

[----------] 1 test from clock_realtime
[ RUN      ] clock_realtime.clock_realtime
185306526911819519
[       OK ] clock_realtime.clock_realtime (1609 ms)
[----------] 1 test from clock_realtime (1609 ms total)

[----------] 1 test from clock_monotonic
[ RUN      ] clock_monotonic.clock_monotonic
253981295601526960
[       OK ] clock_monotonic.clock_monotonic (1608 ms)
[----------] 1 test from clock_monotonic (1608 ms total)

[----------] 1 test from gettimeofday
[ RUN      ] gettimeofday.gettimeofday
185306852249981606
[       OK ] gettimeofday.gettimeofday (1681 ms)
[----------] 1 test from gettimeofday (1681 ms total)

[----------] 1 test from chrono_system_clock
[ RUN      ] chrono_system_clock.chrono_system_clock
185308930355752908
[       OK ] chrono_system_clock.chrono_system_clock (39878 ms)
[----------] 1 test from chrono_system_clock (39878 ms total)

[----------] 1 test from chrono_steady_clock
[ RUN      ] chrono_steady_clock.chrono_steady_clock
253987528206487313
[       OK ] chrono_steady_clock.chrono_steady_clock (39935 ms)
[----------] 1 test from chrono_steady_clock (39935 ms total)

[----------] 1 test from high_resolution_clock
[ RUN      ] high_resolution_clock.high_resolution_clock
185316910674516278
[       OK ] high_resolution_clock.high_resolution_clock (39863 ms)
[----------] 1 test from high_resolution_clock (39863 ms total)

[----------] Global test environment tear-down
[==========] 7 tests from 7 test cases ran. (125204 ms total)
[  PASSED  ] 7 tests.
```


## Contacts

## Thanks
pudding_186@163.com  
will@i007.cc
