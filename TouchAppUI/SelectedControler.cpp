#include "TouchAppUI/SelectedControler.h"
#include "interface.h"


SelectedControler::SelectedControler()
{
}

SelectedControler::~SelectedControler()
{
}

bool SelectedControler::InitMember()
{
    return (NULL != m_pPage);
}

bool SelectedControler::SetStartPos(int x, int y,int iDiffDistance, bool _isVerticalForms)
{
    if (NULL == m_pPage)
    {
        return false;
    }

    const int mode = DKE_HITTEST_TEXT_WORD;
    m_pointStart = m_pointLast = m_pointEnd = DK_POS(x, y);
    m_rightSelecter = true;
    IDKEGallery* pGallery = NULL;
    m_selectingInGallery = m_pPage->HitTestGallery(m_pointStart, &pGallery) && pGallery;
    if (m_selectingInGallery)
    {
        m_pPage->FreeHitTestGallery(pGallery);
    }

    bool ret = HitTestTextRangeByMode(m_pointStart, mode, &m_posBegin, &m_posEnd);
    m_posLast = m_posEnd;
    m_pointPageStart = m_pointStart;
    return ret;
}

bool SelectedControler::MoveToEndOfPage()
{
    DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s START", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    DK_FLOWPOSITION posLast;
    if(m_posBegin < m_pPage->GetEndPosition() && m_posBegin >= m_pPage->GetBeginPosition())
    {
        posLast  = m_pPage->GetEndPosition();
        m_pointPageStart = m_pointStart;
    }
    else if(m_posBegin < m_pPage->GetBeginPosition())
    {
        posLast  = m_pPage->GetBeginPosition();
        m_pointPageStart = m_pointUpLeft;
    }
    else
    {
        posLast  = m_pPage->GetEndPosition();
        m_pointPageStart = m_pointDownRight;
    }

    if (m_rightSelecter)
    {
        m_posLast = m_posEnd;
        m_posEnd = posLast;
    }
    else
    {
        m_posLast = m_posBegin;
        m_posBegin = posLast;
    }
    DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s END", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return true;
}

bool SelectedControler::MoveToBeginOfPage()
{
    DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s START", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (NULL == m_pPage)
    {
        return false;
    }
    DK_FLOWPOSITION posPageBegin = m_pPage->GetBeginPosition();
    DK_FLOWPOSITION posPageEnd = m_pPage->GetEndPosition();
    DK_FLOWPOSITION posLast;
    if(m_posBegin < posPageEnd && m_posBegin > posPageBegin)
    {
        posLast  = posPageBegin;
        m_pointPageStart = m_pointStart;
    }
    else if(m_posBegin > posPageEnd)
    {
        posLast  = posPageEnd;
        m_pointPageStart = m_pointDownRight;
    }
    else
    {
        posLast  = posPageBegin;
        m_pointPageStart = m_pointUpLeft;
    }

    if (m_rightSelecter)
    {
        m_posLast = m_posEnd;
        m_posEnd = posLast;
    }
    else
    {
        m_posLast = m_posBegin;
        m_posBegin = posLast;
    }
    DebugPrintf(DLC_SELECTEDCONTROLER, "%s, %d, %s, %s END", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return true;
}

bool SelectedControler::Move(int x, int y, bool rightHandler)
{
    if (NULL == m_pPage)
    {
        return false;
    }
    m_rightSelecter = rightHandler;
    DK_FLOWPOSITION posLast;
    bool bRet = GetPosition(x, y, posLast, !rightHandler);
    if (bRet)
    {
        if (rightHandler)
        {
            m_posLast = m_posEnd;
            m_posEnd = posLast;
        }
        else
        {
            m_posLast = m_posBegin;
            m_posBegin = posLast;
        }
    }
    return bRet;
}


