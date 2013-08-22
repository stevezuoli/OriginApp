#ifndef __TIMERLIST_INCLUDE__
#define __TIMERLIST_INCLUDE__
#include <vector>
#include "dkBaseType.h"
#include "ITimerTask.h"

typedef struct Timer_List_Item
{
    ITimerTask *m_pTimerTask;
    LONG m_lNewInterval;
    LONG m_lOldInterval;
    INT m_iElapse;

    Timer_List_Item(ITimerTask *pTimerTask,
                        LONG lNewInterval,
                        LONG lOldInterval)
        : m_pTimerTask(pTimerTask)
        , m_lNewInterval(lNewInterval)
        , m_lOldInterval(lOldInterval)
        , m_iElapse(0)
    {

    }
}TimerListItem, *PTimerListItem;


class CTimerList
{
public:
    CTimerList();
    virtual ~CTimerList();

    BOOL AddTimer(ITimerTask *pTimerTask, LONG lDelay, LONG lPeriod);
    BOOL RemoveTimer(ITimerTask *pTimerTask);

    INT GetLength();
    PTimerListItem Get(INT iIndex);

private:
    void ReleaseTimerList();

private:
    std::vector<PTimerListItem> m_vTimerList;
};

#endif
