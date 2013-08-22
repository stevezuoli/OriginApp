#include "Utility/TimerList.h"

using namespace std;

CTimerList::CTimerList()
{
    //do nothing
}

CTimerList::~CTimerList()
{
    this->ReleaseTimerList();
}

BOOL CTimerList::AddTimer(ITimerTask *pTimerTask, LONG lDelay, LONG lPeriod)
{
    if(NULL == pTimerTask)
    {
        return FALSE;
    }

    if(lDelay < 0 || lPeriod < 0)
    {
        return FALSE;
    }

    PTimerListItem pItem = new TimerListItem(pTimerTask, lDelay, lPeriod);
    if(NULL == pItem)
    {
        return FALSE;
    }

    this->m_vTimerList.push_back(pItem);
    return TRUE;
}

BOOL CTimerList::RemoveTimer(ITimerTask *pTimerTask)
{
    if(NULL == pTimerTask)
    {
        return FALSE;
    }

    INT iNULLCount = 0;
    INT iTimerListLen = this->m_vTimerList.size();
    PTimerListItem pItem = NULL;

    for(INT iIndex = 0; iIndex < iTimerListLen; iIndex++)
    {
        pItem = this->m_vTimerList.at(iIndex);
        if(NULL == pItem)
        {
            iNULLCount++;
            continue;
        }

        if(pTimerTask == pItem->m_pTimerTask)
        {
            delete pItem;
            this->m_vTimerList.erase(m_vTimerList.begin() + iIndex);
            break;
        }
    }

    if(iNULLCount == (iTimerListLen - 1))
    {
        return FALSE;
    }

    return TRUE;
}

INT CTimerList::GetLength()
{
    return this->m_vTimerList.size();
}

PTimerListItem CTimerList::Get(INT iIndex)
{
    if(iIndex < 0 || (UINT)iIndex >= this->m_vTimerList.size())
    {
        return NULL;
    }

    return this->m_vTimerList.at(iIndex);
}

void CTimerList::ReleaseTimerList()
{
    INT iTimerListLen = this->m_vTimerList.size();
    PTimerListItem pItem = NULL;

    for(INT iIndex = 0; iIndex < iTimerListLen; iIndex++)
    {
        pItem = this->m_vTimerList.at(iIndex);
        if(pItem)
        {
            delete pItem;
        }
    }

    this->m_vTimerList.clear();
}

