
#ifndef __DMTIMEREVENT_H__
#define __DMTIMEREVENT_H__

#include "dmtimernode.h"
#include "dmsingleton.h"

class CDMTimerEventMgr : public TSingleton<CDMTimerEventMgr>, public CDMTimerNode
{
    friend class TSingleton<CDMTimerEventMgr>;
public:
    CDMTimerEventMgr();
    virtual ~CDMTimerEventMgr();

    bool Init();

public:
    // event
    virtual void OnTimer(uint64_t qwIDEvent,  dm::any& oAny );
private:
    bool LoadConfig();
private:
    
};

#endif // __DMTIMEREVENT_H__