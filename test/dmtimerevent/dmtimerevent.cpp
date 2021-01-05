#include "dmtimerevent.h"



CDMTimerEventMgr::CDMTimerEventMgr()
{

}

CDMTimerEventMgr::~CDMTimerEventMgr()
{

}

bool CDMTimerEventMgr::Init()
{
    if (!LoadConfig())
    {
        return false;
    }
    return true;
}

bool CDMTimerEventMgr::LoadConfig()
{
    return true;
}

void CDMTimerEventMgr::OnTimer(uint64_t qwIDEvent,  dm::any& oAny )
{
    int nType = dm::any_cast<int>(oAny);

    switch (qwIDEvent)
    {
    case 0:
        break;
    default:
        break;
    }
}
