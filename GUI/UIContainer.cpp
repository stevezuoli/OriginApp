////////////////////////////////////////////////////////////////////////
// UIContainer.cpp
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "GUI/UIContainer.h"
#include "GUI/UIMenu.h"
#include "GUI/GUIHelper.h"
#include "GUI/UISizer.h"
#include "interface.h"
#include "GUI/GUISystem.h"
#include <assert.h>
#include "drivers/DeviceInfo.h"


#define MAX_TABINDEX    (+999999999)
#define MIN_TABINDEX    (-999999999)
#define INVALID_INDEX   (-1)

#define OUTPUT_CHILDREN 0

struct SHORTCUT_MAP
{

    DWORD dwCmdId;
    int iKeyCode;
};

static const SHORTCUT_MAP g_rgkShortcutKeyTable[]=
{
    {ID_MNU_SYSTEM_SETTINGS,    ALT_DOWN |  KEY_SYM },
    {ID_MNU_DELETE_BOOK,                    KEY_DEL },
    {ID_MNU_VIEW_BOOKMARK,           ALT_DOWN |  KEY_DOT },
    {ID_MNU_ABOUT_BOOK,         ALT_DOWN |  KEY_FONT},
    {ID_BTN_SEARCH,                         KEY_S},
    {ID_BTN_SORT,                           KEY_X},
    {ID_BTN_REFRESH,                        KEY_R},
    {ID_BTN_MANUAL_ADD,                         KEY_M},
    {ID_MNU_ADD_BOOKMARK,       ALT_DOWN | KEY_D},
    {ID_MNU_SEARCH_IN_BOOK,     ALT_DOWN | KEY_K},
    {ID_MNU_VIEW_TOC,           ALT_DOWN | KEY_L},
    {ID_MNU_GOTO_PAGE,          ALT_DOWN | KEY_F},
    {ID_MNU_AUTO_PAGEDOWN,      ALT_DOWN | KEY_J},
    {ID_MNU_ZOOM_OUT,           SHIFT_DOWN | KEY_DOWN},
    {ID_MNU_ZOOM_IN,            SHIFT_DOWN | KEY_UP},
};

UIContainer::UIContainer()
    : UIWindow(NULL, IDSTATIC)
    , m_bIsBubbleNotify(FALSE)
    , m_iFocusedChild(0)
    , m_pLayout(NULL)
    , m_iFocusedChildInMenu(0)
    , m_bIsDisposed(FALSE)
    , m_firstTouchTarget(NULL)
{
    Init();
    m_bIsContain = true;
}

UIContainer::UIContainer(UIContainer* pParent, const DWORD dwId, BOOL bBubbleNotify)
    : UIWindow(pParent, dwId)
    , m_bIsBubbleNotify(bBubbleNotify)
    , m_iFocusedChild(0)
    , m_pLayout(NULL)
    , m_iFocusedChildInMenu(0)
    , m_bIsDisposed(FALSE)
    , m_firstTouchTarget(NULL)
{
    if (pParent != NULL)
    {
        pParent->AppendChild(this);
    }

    Init();
}

UIContainer::UIContainer(UIContainer* pParent, const DWORD dwId)
    : UIWindow(pParent, dwId)
    , m_bIsBubbleNotify(FALSE)
    , m_iFocusedChild(0)
    , m_pLayout(NULL)
    , m_iFocusedChildInMenu(0)
    , m_bIsDisposed(FALSE)
    , m_firstTouchTarget(NULL)
{
    if (pParent != NULL)
    {
        pParent->AppendChild(this);
    }

    Init();
}

UIContainer::~UIContainer()
{
    ClearTouchTargets();
    Dispose();
}

void UIContainer::Dispose()
{
    UIWindow::Dispose();
    OnDispose(m_bIsDisposed);
}


void UIContainer::Init()
{
    //  UIWindow::Init();
    m_bIsDisposed = FALSE;
    m_iFocusedChild = -1;
    m_pLayout = NULL;
    m_iFocusedChildInMenu = -1;
    m_bIsContain = true;
    SetZPosition(ZPOSITION_CONTAINER);
}

BOOL UIContainer::NextFocus(BOOL bIsReverse)
{
    if (bIsReverse)
    {
        return SetChildWindowFocus(GetPreFocusIndex(m_iFocusedChild), TRUE);
    }
    else
    {
        return SetChildWindowFocus(GetNextFocusIndex(m_iFocusedChild), FALSE);
    }
}

void UIContainer::SetAppropriateFocus(BOOL bIsFocus, BOOL bIsReverse)
{
    UIWindow::SetFocus(bIsFocus);

    if (m_childWins.size()< 1)
    {
        return;
    }

    if (bIsFocus)
    {
        INT32 iFocus = bIsReverse ? GetPreFocusIndex(m_childWins.size()) : GetNextFocusIndex(-1);
        if (iFocus != -1)
        {
            SetChildWindowFocus(iFocus, bIsReverse);
        }
    }
    else
    {
        UIWindow* pWin = GetChildByIndex(m_iFocusedChild);
        if (pWin != NULL)
        {
            pWin->SetFocus(FALSE);
            pWin = NULL;
        }

        m_iFocusedChild = -1;
    }
}

void UIContainer::OnChildSetFocus(UIWindow* pWindow)
{
    INT32 iIndex = GetChildIndex(pWindow);

    if (m_iFocusedChild == iIndex)
    {
        return;
    }

    if (m_pParent != NULL)
    {
        m_pParent->OnChildSetFocus(this);
    }

    ClearFocus();

    m_iFocusedChild = iIndex;
    m_bIsFocus = TRUE;
}

void UIContainer::OnResize(INT32 iWidth, INT32 iHeight)
{
    UIWindow::OnResize(iWidth, iHeight);

    if (m_pLayout != NULL)
    {
        if (!m_pLayout->IsInited())
        {
            m_pLayout->Init(GetClientWidth(), GetClientHeight());
        }
        else
        {
            m_pLayout->OnResize(GetClientWidth(), GetClientHeight());
        }
    }
}

HRESULT UIContainer::DrawFocus(DK_IMAGE drawingImg)
{
    return S_OK;
    // empty
}

void UIContainer::OnNotify(UIEvent rEvent)
{
    if (m_bIsBubbleNotify && m_pParent != NULL)
    {
        m_pParent->OnNotify(rEvent);
    }
}

void UIContainer::OnBroadcast(UIEvent rEvent)
{
    // broadcast the event to all the child windows
    for (size_t i = 0; i < GetChildrenCount(); i++)
    {
        UIWindow* pWin = m_childWins[i];
        if (pWin != NULL)
        {
            pWin->OnBroadcast(rEvent);
            pWin = NULL;
        }
    }
}

UIWindow* UIContainer::GetChildByIndex(size_t index) const
{
    if (index < 0 || index >= GetChildrenCount())
    {
        return NULL;
    }

    return m_childWins[index];
}

void UIContainer::OnDispose(BOOL bDisposed)
{
    if (bDisposed)
    {
        return;
    }

    m_bIsDisposed = TRUE;

    m_iFocusedChild = -1;

    ClearAllChild(false);

    // dispose the layout
    if (m_pLayout != NULL)
    {
        m_pLayout->Dispose();
        m_pLayout = NULL;
    }
}

BOOL UIContainer::SetChildWindowFocus(INT32 iNewFocus, BOOL bIsReverse)
{
#ifndef KINDLE_FOR_TOUCH
    INT32 iSize = m_childWins.size();
    if (iNewFocus < 0 || iNewFocus >= iSize)
    {
        m_iFocusedChild = iNewFocus;
        return FALSE;
    }

    UIWindow* pWin = m_childWins[iNewFocus];
    if (NULL == pWin)
    {
        return FALSE;
    }

    if (!pWin->IsEnable())
    {
        return FALSE;
    }

    ClearFocus();

    if (bIsReverse)
    {
        pWin->SetReverseFocus(TRUE);
    }
    else
    {
        pWin->SetFocus(TRUE);
    }

    m_iFocusedChild = iNewFocus;
#endif
    return TRUE;
}

BOOL UIContainer::SendKeyToChildren(INT32 iKeyCode)
{
    UIWindow* pWin = GetChildByIndex(m_iFocusedChild);
    if (pWin)
    {
        DebugPrintf(DLC_MESSAGE, "FocusChild=(ClassName=%s, Id=%d, Text=%s, IsFocus=%d, IsVisible=%d)",
            pWin->GetClassName(), pWin->GetId(), pWin->GetText(), pWin->IsFocus(), pWin->IsVisible()
            );
    }

    if (pWin && pWin->IsFocus()&& pWin->IsVisible())
    {
        if (!pWin->OnKeyPress(iKeyCode))
        {
            return FALSE;
        }
    }

    return TRUE;
}

BOOL UIContainer::SendHotKeyToChildren(INT32 iKeyCode)
{
    if (DeviceInfo::IsK3() && IsAlpha(iKeyCode))
    {
        UINT32 iSize = m_childWins.size();
        for (UINT32 i = 0; i < iSize; i++)
        {
            UIWindow* pWin = m_childWins[i];
            if (pWin)
            {
                if(pWin ->IsVisible() && (!pWin->OnKeyPress(iKeyCode)))
                {
                    return false;
                }
            }
            else
            {
                DebugPrintf(DLC_ERROR, "%s: FAILED to get child %d/%d !!!", GetClassName(), i, iSize);
            }
        }
    }
    return TRUE;
}

BOOL UIContainer::OnKeyPress(INT32 iKeyCode)
{
    DebugPrintf(DLC_MESSAGE,"%s::UIContainer::OnKeyPress(%d) m_iFocusedChild=%d", GetClassName(), iKeyCode, m_iFocusedChild);
#ifdef KINDLE_FOR_TOUCH
    return UIWindow::OnKeyPress(iKeyCode);
#else
    if (!SendKeyToChildren(iKeyCode))
    {
        return FALSE;
    }

    switch (iKeyCode)
    {
    case KEY_LEFT:
        MoveFocus(DIR_LEFT);
        break;
    case KEY_UP:
        MoveFocus(DIR_TOP);
        break;
    case KEY_RIGHT:
        MoveFocus(DIR_RIGHT);
        break;
    case KEY_DOWN:
        MoveFocus(DIR_DOWN);
        break;
    default:
        if(SendHotKeyToChildren(iKeyCode))
        {
            DebugLog(DLC_MESSAGE, "UIPage::OnKeyPress() Processing Shortcut keys");
            for (UINT i=0; i<ARRSIZE(g_rgkShortcutKeyTable); i++)
            {
                if (iKeyCode == g_rgkShortcutKeyTable[i].iKeyCode)
                {
                    OnEvent(UIEvent(COMMAND_EVENT, this, g_rgkShortcutKeyTable[i].dwCmdId));
                    return FALSE;
                }
            }
            return UIWindow::OnKeyPress(iKeyCode);
        }
    }

    return FALSE;
#endif
}

HRESULT UIContainer::MoveFocus(MoveDirection dir,BOOL bLoop)
{
    DebugPrintf(DLC_FOCUS, "%s::UIContainer::MoveFocus(%d)", GetClassName(), dir);

    // TODO: 用新的函数替换此接口.
    // return MoveFocusWithStructTable(dir);
    // WARNING: 默认孩子之间不能有重叠。

    UIWindow* pWin = GetChildByIndex(m_iFocusedChild);

    if (pWin)
    {
        DebugPrintf(DLC_FOCUS, "CurrentFocus=%s(Text=%s, Id=%d), FocusRect=(%d, %d, %d, %d)",
            pWin->GetClassName(), pWin->GetText(), pWin->GetId(),
            pWin->GetX(), pWin->GetY(), pWin->GetX() + pWin->GetWidth(), pWin->GetY() + pWin->GetHeight());
    }
    else
    {
        DebugPrintf(DLC_FOCUS, "CurrentFocus is NULL, ContainerRect=(%d, %d, %d, %d)",
            this->GetX(), this->GetY(), this->GetX() + this->GetWidth(), this->GetY() + this->GetHeight());
    }

    int X(GetWidth() / 2),
        Y(GetHeight() / 2),
        iMinDist(GetWidth() + GetHeight()),
        iMinIndex(-1),
        iMaxDist(-1);

    switch (dir)
    {
    case DIR_LEFT:
        if (pWin)
        {
            X = pWin->GetX();
            Y = pWin->GetY() + pWin->GetHeight() / 2;
        }
        else
        {
            X = GetWidth();
            Y = GetHeight() / 2;
        }
        for (size_t i = 0; i < m_childWins.size(); i++)
        {
            pWin = GetChildByIndex(i);
            if (pWin && pWin->IsEnable() && pWin->IsVisible() &&
                pWin->GetX() + pWin->GetWidth() <= X)
            {
                int iNewDist = X - (pWin->GetX() + pWin->GetWidth()) + ABS(pWin->GetY() +  pWin->GetHeight() / 2 - Y);
                if(bLoop)
                {
                    if (iNewDist > iMaxDist && iNewDist < iMinDist)
                    {
                        iMaxDist = iNewDist;
                        iMinIndex = i;
                    }
                }
                else
                {
                    if (iNewDist < iMinDist)
                    {
                        iMinDist = iNewDist;
                        iMinIndex = i;
                    }
                }
            }
        }
        break;
    case DIR_TOP:
        if (pWin)
        {
            X = pWin->GetX() + pWin->GetWidth() / 2;
            Y = pWin->GetY();
        }
        else
        {
            X = GetWidth() / 2;
            Y = GetHeight();
        }

        for (size_t i = 0; i < GetChildrenCount(); i++)
        {
            pWin = m_childWins[i];
            if (pWin && pWin->IsEnable() && pWin->IsVisible() &&
                pWin->GetY() + pWin->GetHeight() <= Y)
            {
                int iNewDist = ABS(pWin->GetX() + pWin->GetWidth() / 2 - X) + Y - (pWin->GetY() +  pWin->GetHeight());
                if(bLoop)
                {
                    if (iNewDist > iMaxDist && iNewDist < iMinDist)
                    {
                        iMaxDist = iNewDist;
                        iMinIndex = i;
                    }
                }
                else
                {
                    if (iNewDist < iMinDist)
                    {
                        iMinDist = iNewDist;
                        iMinIndex = i;
                    }
                }
            }
        }
        break;
    case DIR_RIGHT:
        if (pWin)
        {
            X = pWin->GetX() + pWin->GetWidth();
            Y = pWin->GetY() + pWin->GetHeight() / 2;
        }
        else
        {
            X = 0;
            Y = GetHeight() / 2;
        }
        for (size_t i = 0; i < m_childWins.size(); i++)
        {
            pWin = m_childWins[i];
            if (pWin && pWin->IsEnable() && pWin->IsVisible() &&
                pWin->GetX() >= X)
            {
                int iNewDist = pWin->GetX() - X + ABS(pWin->GetY() +  pWin->GetHeight() / 2 - Y);
                if(bLoop)
                {
                    if (iNewDist > iMaxDist && iNewDist < iMinDist)
                    {
                        iMaxDist = iNewDist;
                        iMinIndex = i;
                    }
                }
                else
                {
                    if (iNewDist < iMinDist)
                    {
                        iMinDist = iNewDist;
                        iMinIndex = i;
                    }
                }
            }
        }
        break;
    case DIR_DOWN:
        if (pWin)
        {
            X = pWin->GetX() + pWin->GetWidth() / 2;
            Y = pWin->GetY() + pWin->GetHeight();
        }
        else
        {
            X = GetWidth() / 2;
            Y = 0;
        }
        for (size_t i = 0; i < m_childWins.size(); i++)
        {
            pWin = m_childWins[i];
            if (pWin && pWin->IsEnable() && pWin->IsVisible() &&
                pWin->GetY() >= Y)
            {
                int iNewDist = ABS(pWin->GetX() + pWin->GetWidth() / 2 - X) + pWin->GetY() - Y;
                if(bLoop)
                {
                    if (iNewDist > iMaxDist && iNewDist < iMinDist)
                    {
                        iMaxDist = iNewDist;
                        iMinIndex = i;
                    }
                }
                else
                {
                    if (iNewDist < iMinDist)
                    {
                        iMinDist = iNewDist;
                        iMinIndex = i;
                    }
                }
            }
        }
        break;
    default:
        return E_INVALIDARG;
        break;
    }

    if(-1 == iMinIndex && !bLoop)
    {
        switch(dir)
        {
        case DIR_LEFT:
            return MoveFocus(DIR_RIGHT,true);
        case DIR_RIGHT:
            return MoveFocus(DIR_LEFT,true);
        case DIR_TOP:
            return MoveFocus(DIR_DOWN,true);
        case DIR_DOWN:
            return MoveFocus(DIR_TOP,true);
        default:
            return E_INVALIDARG;
        }
    }
    BOOL fRet(FALSE);
    if (iMinIndex >= 0)
    {

        fRet = SetChildWindowFocus(iMinIndex, FALSE);
    }

    pWin = GetChildByIndex(iMinIndex);
    if (pWin)
    {
        DebugPrintf(DLC_FOCUS, "NewFocus=%s(Text=%s, Id=%d), FocusRect=(%d, %d, %d, %d), fRet=%d",
            pWin->GetClassName(), pWin->GetText(), pWin->GetId(),
            pWin->GetX(), pWin->GetY(), pWin->GetX() + pWin->GetWidth(), pWin->GetY() + pWin->GetHeight(),
            fRet);
    }
    else
    {
        DebugPrintf(DLC_FOCUS, "NewFocus is NULL, fRet=%d", fRet);
    }

    return (fRet ? S_OK : S_FALSE);
}

static const int kiMaxOffset = 0xfff;
void UIContainer::PushWinToLine(VectorLine& line, UIWindow*pWin)
{
    VectorLine::iterator iter = line.begin();
    UIWindow* pCur = NULL;
    for (; iter != line.end(); iter++)
    {
        pCur = *iter;
        if (pWin->GetX() < pCur->GetX())
        {
            line.insert(iter, pWin);
            return;
        }
    }

    line.push_back(pWin);
}

void UIContainer::PushWinToTable(VectorTable& table, UIWindow*pWin, int iLineGap)
{
    VectorLine* pLine = NULL;
    UIWindow* pCur = NULL;
    int iY = pWin->GetY();

    VectorTable::iterator iter = table.begin();
    for (; iter != table.end(); iter++)
    {
        pLine = *iter;
        pCur = pLine->at(0);
        if (abs(pCur->GetY() - iY) < iLineGap)
        {
            PushWinToLine(*pLine, pWin);
            return;
        }

        if (pCur->GetY() - iY > iLineGap)
        {
           VectorLine* pInsetLine = new VectorLine;
           table.insert(iter, pInsetLine);
           PushWinToLine(*pInsetLine, pWin);
           return;
        }
    }

    VectorLine* pInsetLine = new VectorLine;
    table.push_back(pInsetLine);
    PushWinToLine(*pInsetLine, pWin);
}

BOOL UIContainer::StructTable(VectorTable& table, int iLineGap)
{
    UIWindow* pWin = NULL;
    for (size_t i = 0; i < GetChildrenCount(); i++)
    {
        pWin = GetChildByIndex(i);
        if (pWin->IsEnable() && pWin->IsVisible())
        {
            PushWinToTable(table, pWin, iLineGap);
        }
    }

    return TRUE;
}

bool UIContainer::CompareZPosition(UIWindow* pWnd1, UIWindow* pWnd2)
{
    if (pWnd1 && pWnd2)
    {
        return (pWnd1->GetZPosition() < pWnd2->GetZPosition());
    }
    return (NULL == pWnd1);
}
HRESULT UIContainer::MoveFocusWithStructTable(int iKey, int iLineGap)
{
    // 1.构建一个二维表
    HRESULT hr = S_OK;
    VectorTable table;
    StructTable(table, iLineGap);
    VectorLine* pLine = NULL;
    UIWindow* pWin = NULL;
    int iCurLine = 0;
    int iOffset = 0;
    int iCurLineLen = 0;
    for(UINT i = 0; i < table.size(); i++)
    {
        pLine = table.at(i);
        for(UINT j = 0; j < pLine->size(); j++)
        {
            pWin = pLine->at(j);
            if (pWin->IsFocus())
            {
                iCurLine = i;
                iOffset = j;
                i = table.size();
                iCurLineLen = pLine->size();
                break;
            }
        }
    }

    if (table.size() < 1)
    {
        return hr;
    }

    pWin = table.at(iCurLine)->at(iOffset);
    pWin->SetFocus(FALSE);

    switch (iKey)
    {
    case KEY_LEFT:
        if (iOffset < 1)
        {
            iOffset = kiMaxOffset;
            if (iCurLine > 0)
            {
                iCurLine--;
            }
            else
            {
                iCurLine = table.size() - 1;
            }
        }
        else
        {
            iOffset--;
        }

        break;
    case KEY_RIGHT:
        if (iOffset < iCurLineLen - 1)
        {
            iOffset++;
        }
        else
        {
            iOffset = 0;
            if (iCurLine < (int)(table.size() - 1))
            {
                iCurLine++;
            }
            else
            {
                iCurLine = 0;
            }
        }
        break;
    case KEY_UP:
        if (iCurLine > 0)
        {
            iCurLine--;
        }
        else
        {
            iCurLine = table.size() - 1;
        }

        break;
    case KEY_DOWN:
        if (iCurLine < (int)(table.size() - 1))
        {
            iCurLine++;
        }
        else
        {
            iCurLine = 0;
        }

        break;
    default:
        hr = E_FAIL;
        break;
    }


    if (SUCCEEDED(hr))
    {
        iCurLineLen = (table.at(iCurLine))->size();
        if (iOffset > iCurLineLen - 1)
        {
            iOffset = iCurLineLen - 1;
        }

        pWin = table.at(iCurLine)->at(iOffset);
        pWin->SetFocus(TRUE);
    }

    for(UINT i = 0; i < table.size(); i++)
    {
        delete table.at(i);
    }

    table.clear();
    return hr;
}

void UIContainer::SetRelayNotify(BOOL bIsRelay)
{
    m_bIsBubbleNotify = bIsRelay;
}

INT32 UIContainer::ClientToScreenY(INT32 iY)
{
    return m_pParent == NULL ? iY : (m_pParent->ClientToScreenY(m_iTop) + iY);
}

void UIContainer::ClearFocus()
{
    UINT32 iSize = GetChildrenCount();
    for (UINT32 i = 0; i < iSize; i++)
    {
        UIWindow* pWin = GetChildByIndex(i);

        if (pWin && pWin->IsFocus())
        {
            pWin->SetFocus(FALSE);
            pWin = NULL;
        }
    }
}

void UIContainer::SetFocus(BOOL bIsFocus)
{
    SetAppropriateFocus(bIsFocus, FALSE);
}

void UIContainer::SetReverseFocus(BOOL bIsFocus)
{
    SetAppropriateFocus(bIsFocus, TRUE);
}

INT32 UIContainer::GetPreFocusIndex(INT32 iFocus)
{
    UIWindow* pWin = GetChildByIndex(iFocus);
    if (NULL == pWin)
    {
        return INVALID_INDEX;
    }

    INT32 iCurrTabIndex = pWin->GetTabIndex();
    INT32 iValue = MAX_TABINDEX;
    INT32 iValueIndex =  INVALID_INDEX;
    INT32 iValueMax = MIN_TABINDEX;
    INT32 iValueMaxIndex =  INVALID_INDEX;

    INT32 i = GetPrevChild(iFocus);
    while (i != iFocus)
    {
        pWin = GetChildByIndex(i);
        if (NULL != pWin && pWin->IsEnable()&& pWin->IsVisible())
        {
            INT32 iTabIndex = pWin->GetTabIndex();
            if (iTabIndex > iCurrTabIndex)
            {
                if (iTabIndex > iValueMax)
                {
                    // Find the biggest one which is bigger
                    iValueMax = iTabIndex;
                    iValueMaxIndex = i;
                }
            }
            else
            {
                if (iCurrTabIndex - iTabIndex < iValue)
                {
                    // Find the nearest one which is smaller
                    iValue = iCurrTabIndex - iTabIndex;
                    iValueIndex = i;
                }
            }
        }
        i = GetPrevChild(i);
    }

    pWin = NULL;
    if (INVALID_INDEX != iValueIndex)
    {
        return iValueIndex;
    }
    else
    {
        if (INVALID_INDEX != iValueMaxIndex)
        {
            return iValueMaxIndex;
        }
    }

    pWin = GetChildByIndex(iFocus);
    if (NULL != pWin && pWin->IsEnable()&& pWin->IsVisible())
    {
        pWin = NULL;
        return iFocus;
    }

    pWin = NULL;
    return INVALID_INDEX;
}

INT32 UIContainer::GetAppropriateFocus()
{
    INT32 iValueMin= MAX_TABINDEX;
    INT32 iValueMinIndex = -1;
    size_t childrenSize = GetChildrenCount();
    for (size_t i = 0; i < childrenSize; i++)
    {
        UIWindow* pWin = GetChildByIndex(i);
        if (NULL == pWin || !pWin->IsEnable() || !pWin->IsVisible())
        {
            pWin = NULL;
            continue;
        }

        if (pWin->GetTabIndex()< iValueMin)
        {
            iValueMin = pWin->GetTabIndex();
            iValueMinIndex = i;
        }
        pWin = NULL;

    }

    return iValueMinIndex;
}

INT32 UIContainer::GetNextChild(INT32 iIdx)
{
    if (iIdx >= (INT32) GetChildrenCount())
    {
        return 0;
    }

    return iIdx + 1;
}

INT32 UIContainer::GetPrevChild(INT32 iIdx)
{
    if (iIdx > 0)
    {
        return iIdx - 1;
    }

    return ((int)GetChildrenCount())- 1;
}

INT32 UIContainer::GetNextFocusIndex(INT32 iFocus)
{
    if (INVALID_INDEX == iFocus)
    {
        return GetAppropriateFocus();
    }

    UIWindow* pWin = GetChildByIndex(iFocus);
    if (NULL == pWin)
    {
        return INVALID_INDEX;
    }

    INT32 iCurrTabIndex = pWin->GetTabIndex();
    INT32 iValue= MAX_TABINDEX;
    INT32 iValueIndex=  INVALID_INDEX;
    INT32 iValueMin= MAX_TABINDEX;
    INT32 iValueMinIndex=  INVALID_INDEX;

    INT32 i = GetNextChild(iFocus);
    while (i != iFocus)
    {
        pWin = GetChildByIndex(i);
        if (NULL != pWin && pWin->IsEnable()&& pWin->IsVisible())
        {
            INT32 iTabIndex = pWin->GetTabIndex();
            if (iTabIndex < iCurrTabIndex)
            {
                if (iTabIndex < iValueMin)
                {
                    // Find the smallest one which is smaller
                    iValueMin = iTabIndex;
                    iValueMinIndex = i;
                }
            }
            else
            {
                if (iTabIndex - iCurrTabIndex < iValue)
                {
                    // Find the nearest one which is smaller
                    iValue = iTabIndex - iCurrTabIndex;
                    iValueIndex = i;
                }
            }
        }
        i = GetNextChild(i);
    }

    pWin = NULL;
    if (INVALID_INDEX != iValueIndex)
    {
        return iValueIndex;
    }
    else
    {
        if (INVALID_INDEX != iValueMinIndex)
        {
            return iValueMinIndex;
        }
    }

    pWin = GetChildByIndex(iFocus);
    if (NULL != pWin && pWin->IsEnable()&& pWin->IsVisible())
    {
        pWin = NULL;
        return iFocus;
    }

    pWin = NULL;
    return INVALID_INDEX;
}

void UIContainer::SetLayout(UILayout* pLayout)
{
    m_pLayout = pLayout;
}

UILayout* UIContainer::GetLayout()
{
    return m_pLayout;
}

#if OUTPUT_CHILDREN
void SaveDKImage(DkFormat::DK_IMAGE image, const char* filename)
{
    BITMAPFILEHEADER bmp_file_header;
    int widthRound = (image.iWidth + 3) & 0xFFFFFFFC;
    memset(&bmp_file_header, 0, sizeof(BITMAPFILEHEADER));
    bmp_file_header.bfOffBits = 0x36 + 256*4;
    bmp_file_header.bfSize = widthRound * image.iHeight + 0x36 + 256*4;
    bmp_file_header.bfType = 0x4d42;

    BITMAPINFOHEADER bmp_info_header;
    memset(&bmp_info_header, 0, sizeof(BITMAPINFOHEADER));
    bmp_info_header.biSize = sizeof(BITMAPINFOHEADER);
    bmp_info_header.biWidth = image.iWidth;
    bmp_info_header.biHeight = image.iHeight;
    bmp_info_header.biPlanes = 1;
    bmp_info_header.biBitCount = 8;
    bmp_info_header.biSizeImage = widthRound * image.iHeight;
    bmp_info_header.biClrUsed = 256;

    int plus[256] = {0};
    for ( int i=0; i<256; ++i)
    {
        memset(plus+i, 255-i, 4);
    }

    char bmpFileName[256] = {0};
    if (filename)
    {
        sprintf(bmpFileName, "%s.bmp", filename);
    }
    else
    {
        time_t rawtime;
        struct tm *info = NULL;
        char name[256] = {0};

        time(&rawtime);
        info = localtime(&rawtime);
        strftime(name, 256, "20%y%m%d%H%M%S", info);

        sprintf(bmpFileName, "%s.bmp", name);
    }
    FILE* fp = fopen(bmpFileName, "w+");
    if (fp)
    {
        fwrite(&bmp_file_header, sizeof(BITMAPFILEHEADER), 1, fp);
        fwrite(&bmp_info_header, sizeof(BITMAPINFOHEADER), 1, fp);
        fwrite(plus, 256*4, 1, fp);
        char *data = new char[widthRound* image.iHeight];
        if (data)
        {
            int i = image.iHeight;
            while (i > 0)
            {
                memcpy(data + (i-1)*widthRound, image.pbData + (image.iHeight - i)*image.iWidth, image.iWidth);
                i--;
            }
            fwrite(data, widthRound*image.iHeight, 1, fp);
            delete []data;
            data = NULL;
        }

        fclose(fp);
    }
}
#endif

HRESULT UIContainer::Draw(DK_IMAGE drawingImg)
{
    DebugPrintf(DLC_GUI, "Entering HRESULT %s::Draw()", GetClassName());
    DebugPrintf(DLC_GUI, "%s: drawingImg=(%x, %d, %d, %d, %d)",
        GetClassName(),
        drawingImg.pbData,
        drawingImg.iColorChannels,
        drawingImg.iWidth,
        drawingImg.iHeight,
        drawingImg.iStrideWidth
        );

    if (!IsDisplay())
    {
        return S_OK;
    }

    if (drawingImg.pbData == NULL)
    {
        return E_FAIL;
    }

    HRESULT hr(S_OK);
    DK_IMAGE imgSelf;
    DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
    DebugPrintf(DLC_GUI, "%s: rcSelf=(%d, %d, %d, %d)",
        GetClassName(),
        rcSelf.left,
        rcSelf.top,
        m_iWidth,
        m_iHeight
        );

    RTN_HR_IF_FAILED(CropImage(
        drawingImg,
        rcSelf,
        &imgSelf
        ));

    RTN_HR_IF_FAILED(DrawBackGround(imgSelf));
    UINT32 iSize = GetChildrenCount();

    DebugPrintf(DLC_GUI,
        "%s: m_childWins.size() = %d",
        GetClassName(),
        iSize
        );


#if OUTPUT_CHILDREN
    static int index = 0;
#endif
    for (UINT32 i = 0; i < iSize; i++)
    {
        UIWindow* pWin = GetChildByIndex(i);
        if (pWin && pWin->IsVisible())
        {
            DebugPrintf(DLC_GUI_VERBOSE,
                    "%s: Drawing Child %d / %d : %s, (%d, %d)-(%d, %d), %s ...",
                    GetClassName(), i, iSize, pWin->GetClassName(),
                    pWin->GetX(), pWin->GetY(),
                    pWin->GetWidth(), pWin->GetHeight(),
                    pWin->GetText());
            hr = pWin->Draw(imgSelf);
            if (!SUCCEEDED(hr))
            {
                DebugPrintf(DLC_GUI_VERBOSE, "Draw child failed");
            }
#if OUTPUT_CHILDREN
            char buf[100];
            snprintf(buf, DK_DIM(buf), "/home/michaelpeng/temp/container_%s_%s_%d", GetClassName(), pWin->GetClassName(), index++);
            SaveDKImage(imgSelf, buf);
#endif
        }
        else
        {
            DebugPrintf(DLC_GUI_VERBOSE, "%s: FAILED to get child %d/%d !!!", GetClassName(), i, iSize);
        }
    }

    RTN_HR_IF_FAILED(DrawFocus(imgSelf));

    DebugPrintf(DLC_GUI, "Leaving HRESULT %s::Draw()", GetClassName());
    return hr;
}

BOOL UIContainer::OnKeyRelease(INT32 iKeyCode)
{
    UIWindow* pWin = GetChildByIndex(m_iFocusedChild);
    if (pWin && pWin->IsFocus())
    {
        if (!pWin->OnKeyRelease(iKeyCode))
        {
            pWin = NULL;
            return FALSE;
        }
    }
    pWin = NULL;

    return TRUE;
}

void UIContainer::AppendChild(UIWindow* pWindow)
{
    if (pWindow == NULL)
    {
        return;
    }

    if (GetChildIndex(pWindow)!= -1)
    {
        pWindow->SetParent(this);
        return;
    }

    UIWindow* pFocusedWnd = GetChildByIndex(m_iFocusedChild);
    pWindow->SetParent(this);
    const int zPos = GetZPosition();
    if (zPos > pWindow->GetZPosition())
    {
        pWindow->SetZPosition(zPos);
    }
    m_childWins.push_back(pWindow);
    
    stable_sort(m_childWins.begin(), m_childWins.end(),CompareZPosition);
    if (pFocusedWnd)
    {
        m_iFocusedChild = GetChildIndex(pFocusedWnd);
    }
}
INT32 UIContainer::GetChildIndex(UIWindow* pWindow)
{
    UINT32 iSize = GetChildrenCount();
    UINT32 iIndex = 0;

    for (iIndex = 0; iIndex < iSize; iIndex++)
    {
        if (pWindow == GetChildByIndex(iIndex))
        {
            return iIndex;
        }
    }

    return -1;
}

#ifdef KINDLE_FOR_TOUCH

UIWindow* UIContainer::GetChildByPoint(int _x, int _y)
{
    UINT32 iSize = m_childWins.size();
    for (UINT32 iIndex = 0; iIndex < iSize; iIndex++)
    {
        UIWindow* pWnd = m_childWins[iIndex];
        if (pWnd && pWnd->ContainPoint(_x, _y))
        {
            return pWnd;
        }
    }

    return NULL;
}
#endif

bool UIContainer::RemoveChild(UIWindow* pWindow, bool doDelete)
{
    if (pWindow == NULL)
    {
        return false;
    }

    INT32 iIndex = GetChildIndex(pWindow);
    if (iIndex == -1)
    {
        return false;
    }

    if (m_iFocusedChild >= iIndex)
    {
        INT32 iFocusedChild = m_iFocusedChild;
        //TODO      jzn
        //NextFocus(TRUE);
        if (iFocusedChild == m_iFocusedChild)
        {
            m_iFocusedChild = INVALID_INDEX;
        }
    }

    pWindow->SetParent(NULL);
    m_childWins[iIndex] = NULL;
    m_childWins.erase(m_childWins.begin() + iIndex);

    // remove the window from layout
    if (m_pLayout != NULL)
    {
        m_pLayout->RemoveControl(pWindow);
    }
    RemoveTouchTarget(pWindow);
    if (doDelete)
    {
        delete pWindow;
    }

    return true;
}

bool UIContainer::RemoveChildByIndex(size_t index, bool doDelete)
{
    return RemoveChild(GetChildByIndex(index), doDelete);
}
UINT32 UIContainer::GetChildrenCount() const
{
    return m_childWins.size();
}

void UIContainer::OnChildMenuChanged(UIWindow* pWindow)
{
    INT32 iIndex = GetChildIndex(pWindow);

    if (iIndex != -1 && m_iFocusedChild == iIndex)
    {
        m_iFocusedChildInMenu = -2;
    }
}

void UIContainer::OnMenuChanged()
{
    m_iFocusedChildInMenu = -2;
}

void UIContainer::ClearAllChild(bool doDelete)
{

    for (size_t i = 0; i < GetChildrenCount(); i++)
    {
        UIWindow *pWin = GetChildByIndex(i);
        if (pWin)
        {
            pWin->SetParent(NULL);
            RemoveTouchTarget(pWin);
            m_childWins[i] = NULL;
            if (doDelete)
            {
                delete pWin;
            }
        }
    }
    m_childWins.clear();
}

BOOL UIContainer::IsShortkey(INT32 iKeyCode)
{
    for (UINT i=0; i<ARRSIZE(g_rgkShortcutKeyTable); i++)
    {
        if (iKeyCode == g_rgkShortcutKeyTable[i].iKeyCode)
        {
            return TRUE;
        }
    }
    return FALSE;
}

void UIContainer::UpdateSectionUI(bool bIsFull)
{
    return;

}

dkSize UIContainer::GetMinSize() const
{
    dkSize s;
    if (m_minWidth == dkDefaultCoord || m_minHeight == dkDefaultCoord)
    {
        if (m_windowSizer)
        {
            s = m_windowSizer->CalcMin();
        }
    }
    return dkSize(dkDefaultCoord == m_minWidth ? s.GetWidth() : m_minWidth, 
                  dkDefaultCoord == m_minHeight ? s.GetHeight() : m_minHeight);
}

void UIContainer::DoSetSize(int x, int y, int width, int height, int sizerFlags)
{
    UIWindow::DoSetSize(x, y, width, height, sizerFlags);
    if (x>=0 && y>=0 && width>0 && height>0)
    {
        Layout();
    }
}

bool UIContainer::DispatchTouchEvent(MoveEvent* moveEvent)
{
    DebugPrintf(DLC_GESTURE, "%s::DispatchTouchEvent(), %08X begin, (%d, %d)", GetClassName(), this, moveEvent->GetX(), moveEvent->GetY());
    bool handled = false;
    int action = moveEvent->GetAction();
    int actionMasked = moveEvent->GetActionMasked();
    if (actionMasked == MoveEvent::ACTION_DOWN)
    {
        CancelAndClearTouchTargets(moveEvent);
    }
    bool intercepted;
    bool hasFirstTouchTargetInit = (m_firstTouchTarget != NULL);
    // sometimes first target may be destroyed in intercept, so remember
    // the state here
    if (actionMasked == MoveEvent::ACTION_DOWN || NULL != m_firstTouchTarget)
    {
        intercepted = OnInterceptionTouchEvent(moveEvent);
        if (intercepted)
        {
            DebugPrintf(DLC_GESTURE, "Intercepted");
        }
        // restore events in case it was changed
        moveEvent->SetAction(action);
    }
    else
    {
        // the action isn't an initial down and the firstTouchTarget is not NULL
        // so the window continues to intercept touch events
        intercepted = true;
    }
    bool canceled = MoveEvent::ACTION_CANCEL == actionMasked;
    bool alreadyDispatchToNewTouchTarget = false;
    TouchTarget* newTouchTarget = NULL;
    if (!canceled && !intercepted)
    {
        if (MoveEvent::ACTION_DOWN == actionMasked)
        {
            size_t childSize = m_childWins.size();
            if (childSize > 0)
            {
                int x = moveEvent->GetX();
                int y = moveEvent->GetY();
                for (int i = (int)childSize - 1; i >= 0; --i)
                {
                    UIWindow* child = m_childWins[i];
                    DebugPrintf(DLC_GESTURE, "%s::DispatchTouchEvent, check child %s, (%d, %d)-(%d,%d), %s", GetClassName(), child->GetClassName(),
                            child->GetX(), child->GetY(), child->GetWidth(), child->GetHeight(), child->GetText());
                    if (!CanWindowReceiveTouchEvents(child)
                            || !IsTransformedTouchPointInWindow(x, y, child))
                    {
                        continue;
                    }
                    newTouchTarget = GetTouchTarget(child);
                    if (newTouchTarget != NULL)
                    {
                        break;
                    }
                    if (DispatchTransformedTouchEvent(moveEvent, false, child, moveEvent->GetPointerIdBits()))
                    {
                        newTouchTarget = AddTouchTarget(child, moveEvent->GetPointerIdBits());
                        alreadyDispatchToNewTouchTarget = true;
                        break;
                    }
                } // for m_childWins
            }
        }
    }

    if (NULL == m_firstTouchTarget && !hasFirstTouchTargetInit)
    {
        // no touch target so treat this as an ordinary view
        handled = DispatchTransformedTouchEvent(moveEvent, canceled, NULL, moveEvent->GetPointerIdBits());
    }
    else
    {
        // dispatch to touch targets, excluding the new target if we've done already
        TouchTarget* prev = NULL;
        TouchTarget* curTouchTarget = m_firstTouchTarget;
        while(curTouchTarget != NULL)
        {
            TouchTarget* next = curTouchTarget->next;
            if (alreadyDispatchToNewTouchTarget && curTouchTarget == newTouchTarget)
            {
                handled = true;
            }
            else
            {
                bool cancelChild = intercepted;
                if (DispatchTransformedTouchEvent(moveEvent, cancelChild, curTouchTarget->child, moveEvent->GetPointerIdBits()))
                {
                    handled = true;
                }
                if (cancelChild)
                {
                    if (prev == NULL)
                    {
                        m_firstTouchTarget = next;
                    }
                    else
                    {
                        prev->next = next;
                    }
                    delete curTouchTarget;
                    curTouchTarget = next;
                    continue;
                }
            }
            prev = curTouchTarget;
            curTouchTarget = next;
        }
    }

    if (canceled || MoveEvent::ACTION_UP == actionMasked)
    {
        ClearTouchTargets();
    }
    DebugPrintf(DLC_GESTURE, "%s::DispatchTouchEvent(), %08X end", GetClassName(), this);
    return handled;
}

bool UIContainer::DispatchTransformedTouchEvent(
            MoveEvent* event,
            bool cancel,
            UIWindow* child,
            int desiredPointerId)
{
    bool handled;
    int oldAction = event->GetAction();
    if (cancel || oldAction == MoveEvent::ACTION_CANCEL)
    {
        event->SetAction(MoveEvent::ACTION_CANCEL);
        if (NULL == child)
        {
            handled = UIWindow::DispatchTouchEvent(event);
        }
        else
        {
            handled = child->DispatchTouchEvent(event);
        }
        event->SetAction(oldAction);
        return handled;
    }
    if (NULL == child)
    {
        handled = UIWindow::DispatchTouchEvent(event);
    }
    else
    {
        int offsetX = -child->GetX();
        int offsetY = -child->GetY();
        event->OffsetLocation(offsetX, offsetY);
        handled = child->DispatchTouchEvent(event);
        event->OffsetLocation(-offsetX, -offsetY);
    }
    return handled;
}

void UIContainer::CancelAndClearTouchTargets(MoveEvent* event)
{
    for (TouchTarget* curTouchTarget = m_firstTouchTarget;
            curTouchTarget!= NULL;
            curTouchTarget= curTouchTarget->next)
    {
        DispatchTransformedTouchEvent(
                event,
                true,
                curTouchTarget->child,
                event->GetPointerIdBits());
    }
    ClearTouchTargets();
}

void UIContainer::ClearTouchTargets()
{

    while (m_firstTouchTarget)
    {
        TouchTarget* next = m_firstTouchTarget->next;
        delete m_firstTouchTarget;
        m_firstTouchTarget = next;
    }
}

bool UIContainer::CanWindowReceiveTouchEvents(UIWindow* child) const
{
    return child->IsVisible() && child->IsEnable();
}

bool UIContainer::IsTransformedTouchPointInWindow(int x, int y, UIWindow* child) const
{
    int localX = x - child->GetX();
    int localY = y - child->GetY();
    return child->PointInWindow(localX, localY);
}

UIContainer::TouchTarget* UIContainer::GetTouchTarget(UIWindow* child) const
{
    for (TouchTarget* curTouchTarget = m_firstTouchTarget;
            curTouchTarget != NULL;
            curTouchTarget = curTouchTarget->next)
    {
        if (curTouchTarget->child == child)
        {
            return curTouchTarget;
        }
    }
    return NULL;
}

UIContainer::TouchTarget* UIContainer::AddTouchTarget(UIWindow* child, int pointerId)
{
    DebugPrintf(DLC_GESTURE, "%08X, %s::AddTouchTarget() %08X, %s", this, GetClassName(), child, child->GetClassName());
    TouchTarget* newTouchTarget = new TouchTarget(child, pointerId);
    if (NULL == newTouchTarget)
    {
        return NULL;
    }
    newTouchTarget->next = m_firstTouchTarget;
    m_firstTouchTarget = newTouchTarget;
    return newTouchTarget;
}
bool UIContainer::OnInterceptionTouchEvent(MoveEvent*)
{
    return false;
}

void UIContainer::RemoveTouchTarget(UIWindow* child)
{
    TouchTarget* curTouchTarget = m_firstTouchTarget;
    TouchTarget* prev = NULL;
    while (curTouchTarget)
    {
        TouchTarget* next = curTouchTarget->next;
        if (curTouchTarget->child == child)
        {
            delete curTouchTarget;
            if (prev == NULL)
            {
                m_firstTouchTarget = next;
            }
            else
            {
                prev->next = next;
            }
            curTouchTarget = next;
            continue;
        }
        prev = curTouchTarget;
        curTouchTarget = next;
        continue;
    }
}

UIContainer::TouchTarget::TouchTarget(UIWindow* child_, int pointerId_)
            : child(child_)
            , pointerId(pointerId_)
            , next(NULL)
{
    DebugPrintf(DLC_GESTURE, "UIContainer::CreateTouchTarget: %s, this target: %08X", child->GetClassName(), this);
}

UIContainer::TouchTarget::~TouchTarget()
{
    DebugPrintf(DLC_GESTURE, "UIContainer::DestroyTouchTarget: %s, this: %08X", child->GetClassName(), this);
}

UIContainer* UIContainer::GetTopFullScreenContainer()
{
    if (!IsFullScreen())
    {
        return NULL;
    }
    for (int i = GetChildrenCount() - 1; i>= 0; --i)
    {
        UIWindow* win = GetChildByIndex(i);
        if (win)
        {
            UIContainer* topContainer = win->GetTopFullScreenContainer();
            if (NULL != topContainer)
            {
                return topContainer;
            }
        }
    }
    return this;
}
