////////////////////////////////////////////////////////////////////////
// UICompoundListBox.h
// Contact: zhangxb
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////
#include "GUI/UICompoundListBox.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UISizer.h"
#include "Framework/CNativeThread.h"

const char* UICompoundListBox::ListTurnPage = "ListTurnPage";
const char* UICompoundListBox::ListItemClick = "ListItemClick";
const char* UICompoundListBox::ListItemDelete = "ListItemDelete";

UICompoundListBox::UICompoundListBox()
: UIContainer(NULL, IDSTATIC)
, m_iSelectedItem(0)
, m_iVisibleItemNum(0)
, m_iItemHeight(0)
, m_iItemWidth(0) 
, m_iItemSpacing(0)
, m_bIsDisposed(FALSE)
{
    m_flingGestureListener.SetListBox(this);
    m_gestureDetector.SetListener(&m_flingGestureListener);
    m_inFlingCheck = false;
    if (!m_windowSizer)
    {
        UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
        SetSizer(mainSizer);
    }
}

UICompoundListBox::UICompoundListBox(UIContainer* pParent, const DWORD dwId)
: UIContainer(pParent, dwId)
, m_iSelectedItem(0)
, m_iVisibleItemNum(0)
, m_iItemHeight(0)
, m_iItemWidth(0) 
, m_iItemSpacing(0)
, m_bIsDisposed(FALSE)
{
    m_flingGestureListener.SetListBox(this);
    m_gestureDetector.SetListener(&m_flingGestureListener);
    m_inFlingCheck = false;
    if (!m_windowSizer)
    {
        UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
        SetSizer(mainSizer);
    }
    if (pParent != NULL)
    {
        pParent->AppendChild(this);
    }
}

UICompoundListBox::~UICompoundListBox()
{
}

BOOL UICompoundListBox::OnKeyPress(INT32 iKeyCode)
{   
#ifdef KINDLE_FOR_TOUCH
    return UIWindow::OnKeyPress(iKeyCode);
#else
    switch (iKeyCode)
    {
    case KEY_UP:
    case KEY_DOWN:
        return HandleListUpDown(iKeyCode == KEY_UP);
        break;
    case KEY_OKAY:
        return OnItemClick(m_iSelectedItem);
    default:
        break;
    }
    return TRUE;
#endif
}

bool UICompoundListBox::OnItemClick(INT32 iSelectedItem)
{
    if(m_iSelectedItem != iSelectedItem)
    {
        UpdatePartialRegion(m_iSelectedItem, iSelectedItem);
        m_iSelectedItem = iSelectedItem;
        return true;
    }
    return false;
}

BOOL UICompoundListBox::OnKeyRelease(INT32 iKeyCode)
{
    return TRUE;
}

HRESULT UICompoundListBox::DrawBackGround(DK_IMAGE drawingImg)
{
    CTpGraphics grf(drawingImg);
    HRESULT hr(S_OK);
    RTN_HR_IF_FAILED(grf.EraserBackGround());

    return hr;
}

HRESULT UICompoundListBox::Draw(DK_IMAGE drawingImg)
{
    if (!m_bIsVisible)
    {
        return S_OK;
    }

    UpdateListUI();
    return UIContainer::Draw(drawingImg);
}

void UICompoundListBox::UpdatePartialRegion(INT32 iOldSelectedItem, INT32 iNewSelectedItem)
{
    if((size_t)iOldSelectedItem >= GetChildrenCount() || (size_t)iNewSelectedItem >= GetChildrenCount())
        return;
    if(iOldSelectedItem != iNewSelectedItem)
    {
        GetChildByIndex(iOldSelectedItem)->SetFocus(FALSE);
        GetChildByIndex(iNewSelectedItem)->SetFocus(TRUE);
    }
}

BOOL UICompoundListBox::HandleListUpDown(BOOL fKeyUp)
{
    INT32 iOldItem = m_iSelectedItem;

    if (fKeyUp)
    {
        m_iSelectedItem--;
    }
    else
    {
        m_iSelectedItem++;
    }

    if (m_iSelectedItem < 0)
    {
        m_iSelectedItem = 0;
        SetFocus(FALSE);
        return TRUE;
    }

    if (m_iSelectedItem >= m_iVisibleItemNum)
    {
        m_iSelectedItem = m_iVisibleItemNum - 1;
        SetFocus(FALSE);
        return TRUE;
    }

    UpdatePartialRegion(iOldItem, m_iSelectedItem);
    return FALSE;
}

INT32 UICompoundListBox::GetMaxDisplayNum()
{
    INT32 iItemHeight = GetItemHeight();
    if (iItemHeight < 1)
    {
        return 0;
    }

    return m_iHeight / (iItemHeight + m_iItemSpacing);
}

BOOL UICompoundListBox::SetItemHeight(INT32 _iItemHeight)
{
    if (_iItemHeight < 0)
        return false;
    m_iItemHeight = _iItemHeight;
    return true;
}

BOOL UICompoundListBox::SetItemWidth(INT32 _iItemWidth)
{
    if (_iItemWidth < 0)
        return false;
    m_iItemWidth = _iItemWidth;
    return true;
}

BOOL UICompoundListBox::SetItemSpacing(INT32 _iItemSpacing)
{
    if (_iItemSpacing < 0 || _iItemSpacing > m_iHeight)
        return false;
    m_iItemSpacing = _iItemSpacing;
    return true;
}

void UICompoundListBox::UpdateListUI()
{
}

dkSize UICompoundListBox::GetMinSize() const
{
    dkSize s = UIContainer::GetMinSize();
    if (s.GetWidth() <= 0)
        s.SetWidth(WidthDefault(dkDefaultCoord));
    if (s.GetHeight() <= 0)
        s.SetHeight(WidthDefault(dkDefaultCoord));

    return s;
}

#ifdef KINDLE_FOR_TOUCH
BOOL UICompoundListBox::HandleLongTap(INT32 selectedItem)
{
    return true;
}
#endif

bool UICompoundListBox::OnChildClick(UIWindow* child)
{
    for (int i = 0; i <  m_iVisibleItemNum; ++i)
    {
        if (child == GetChildByIndex(i))
        {
            m_inFlingCheck = false;
            SNativeMessage msg;
            msg.iType = KMessageItemClick;
            msg.hWnd = GetHandle();
            msg.iParam1 = i;
            CNativeThread::Send(msg);
            return true;
        }
    }
    return UIContainer::OnChildClick(child);
}

bool UICompoundListBox::OnChildLongPress(UIWindow* child)
{
    for (int i = 0; i <  m_iVisibleItemNum; ++i)
    {
        if (child == GetChildByIndex(i))
        {
            m_inFlingCheck = false;
            return HandleLongTap(i);
        }
    }
    return UIContainer::OnChildLongPress(child);
}

bool UICompoundListBox::OnInterceptionTouchEvent(MoveEvent* moveEvent)
{
    bool result = m_gestureDetector.OnTouchEvent(moveEvent);
    if (MoveEvent::ACTION_DOWN == moveEvent->GetActionMasked())
    {
        m_inFlingCheck = true;
    }
    return result;
}

bool UICompoundListBox::OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy)
{
    DebugPrintf(DLC_GESTURE, "UICompoundListBox::OnFling(%d)", direction);
    switch (direction)
    {
        case FD_UP:
        case FD_LEFT:
            return HandleListTurnPage(false);
            break;
        case FD_RIGHT:
        case FD_DOWN:
            return HandleListTurnPage(true);
            break;
        default:
            break;
    }
    return false;
}

bool UICompoundListBox::OnTouchEvent(MoveEvent* moveEvent)
{
    if (MoveEvent::ACTION_DOWN == moveEvent->GetActionMasked())
    {
        m_inFlingCheck = true;
        moveEvent->SetAction(MoveEvent::ACTION_CANCEL);
        m_gestureDetector.OnTouchEvent(moveEvent);
        moveEvent->SetAction(MoveEvent::ACTION_DOWN);
    }
    m_gestureDetector.OnTouchEvent(moveEvent);
    return true;
}


bool UICompoundListBox::HandleListTurnPage(bool pageUp)
{
    if (DoHandleListTurnPage(pageUp))
    {
        ListTurnPageEvent args(this);
        FireEvent(ListTurnPage, args);
    }
    return true;
}
bool UICompoundListBox::DoHandleListTurnPage(bool PageUp)
{
    return true;
}

void UICompoundListBox::OnMessage(const SNativeMessage& msg)
{
    switch (msg.iType)
    {
        case KMessageItemClick:
            OnItemClick(msg.iParam1);
            break;
        default:
            break;
    }
}
