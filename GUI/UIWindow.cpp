////////////////////////////////////////////////////////////////////////
// UIWindow.cpp
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "GUI/UIWindow.h"
#include "GUI/UIContainer.h"
#include "Framework/CDisplay.h"
#include "GUI/UITimerTask.h"
#include "GUI/GUIHelper.h"  
#include "GUI/GUISystem.h"
#include "GUI/EventSet.h"
#include "GUI/MessageEventArgs.h"     		   	    
#include "GUI/UISizer.h"
#include "GUI/CTpGraphics.h"
#include "CommonUI/CPageNavigator.h"
#include "CommonUI/UISoftKeyboardIME.h"
#ifdef KINDLE_FOR_TOUCH	  
#include "drivers/MoveEvent.h"
#include "GUI/GUIConfiguration.h"
#endif
#include "Utility/TimerThread.h"
#include "Utility/SystemUtil.h"
#include <iostream>
#include <tr1/functional>
#include "GUI/GlobalEventListener.h"
#include "drivers/DeviceInfo.h"
#include "Common/WindowsMetrics.h"
#include "CommonUI/UIIMEManager.h"

using namespace std;
using dk::utility::SystemUtil;
using namespace WindowsMetrics;

#ifdef KINDLE_FOR_TOUCH
using dk::utility::TimerThread;

const char* UIWindow::EventClick = "UIWindow::EventClick";
const char* UIWindow::EventLongPress = "UIWindow::EventLongPress";

static bool TouchHook(MoveEvent* event, void* data)
{
    int handle = (int)data;
    UIWindow* window = (UIWindow*)GUISystem::GetInstance()->GetEventListenerByHandle(handle);
    if (NULL == window)
    {
        return false;
    }
    return window->OnHookTouch(event);
}
#endif

UIWindow::UIWindow()
    : m_IsChanged(TRUE)
    , m_pParent(NULL)
    , m_isDisable(FALSE)
#ifdef KINDLE_FOR_TOUCH
    , m_TouchBlocked(false)
#endif
    , m_iLeftMargin(0)
    , m_iTopMargin(0)
    , m_bIsContain(false)
    , m_bIsDisposed(FALSE)
    , m_dwId(IDSTATIC)
    // TODO: 如果失败就惨了。。。想想还有没有别的办法来调用到dispose
#ifdef KINDLE_FOR_TOUCH
    , m_windowFlags(WF_DEFAULT)
    , m_privateFlags(0)
    , m_autoLayout(FALSE)
    , m_touchSlot(0)
    , m_hasPerformLongPress(false)
    , m_touchDownX(-1)
    , m_touchDownY(-1)
    , m_outClickSlot(false)
    , m_hookingTouch(false)
#else
    , _OnDispose(new Delegates())
    , m_autoLayout(FALSE)
    , Event_OnDispose(*_OnDispose)
#endif
{
    Init();
}

UIWindow::UIWindow(UIContainer* pParent, DWORD rstrId)
    : m_IsChanged(TRUE)
    , m_pParent(pParent)
    , m_bIsFocus(FALSE)
    , m_bIsVisible(FALSE)
    , m_isDisable(FALSE)
    , m_iTop(0)
    , m_iLeft(0)
    , m_iWidth(0)
    , m_iHeight(0)
    , m_bIsTabStop(FALSE)
    , m_iTabIndex(0)
    , m_pEventHook(NULL)
    , m_timers()
    , m_cancelTimers() 
#ifdef KINDLE_FOR_TOUCH
    , m_TouchBlocked(false)
#endif
    , m_iLeftMargin(0)
    , m_iTopMargin(0)
    , m_bIsContain(false)
    , m_bIsDisposed(FALSE)
    , m_iTimerId(0)
    , m_dwId(rstrId)
#ifdef KINDLE_FOR_TOUCH
    , m_windowFlags(WF_DEFAULT) 
    , m_privateFlags(0)
    , m_autoLayout(FALSE)
    , m_touchSlot(0)
    , m_hasPerformLongPress(false)
    , m_touchDownX(-1)
    , m_touchDownY(-1)
    , m_outClickSlot(false)
    , m_hookingTouch(false)
#else
    , _OnDispose(new Delegates())
    , m_autoLayout(FALSE)
    , Event_OnDispose(*_OnDispose)
#endif
{
    Init();
}

UIWindow::~UIWindow()
{
#ifdef KINDLE_FOR_TOUCH
    UnhookTouch();
#endif
    if (m_windowSizer)
    {
        delete m_windowSizer;
        m_windowSizer = NULL;
    }
    

    Dispose();
#ifndef KINDLE_FOR_TOUCH
    EventArg e;
    e.strMess = this->GetClassName();
    _OnDispose->OnEvent(this,e);
    delete _OnDispose;
    for (DK_AUTO(pos, m_connections.begin()); pos != m_connections.end(); ++pos)
    {
        (*pos)->Disconnect();
    }
    m_connections.clear();
#endif
}


#ifdef KINDLE_FOR_TOUCH
bool UIWindow::ContainPoint(int _x, int _y)
{
    if (!m_bIsVisible)
        return false;
    
    int iLeft = this->m_iLeft;
    int iTop = this->m_iTop;

    UIContainer* _pParent = this->GetParent();
    while ( _pParent != NULL)
    {
        iLeft += _pParent->m_iLeft;
        iTop += _pParent->m_iTop;
        _pParent = _pParent->GetParent();
    }
    return (_x >= iLeft && _x < (iLeft + m_iWidth) && _y >= iTop && _y < (iTop + m_iHeight));
}
#endif

void UIWindow::Init()
{
    m_minWidth = m_maxWidth = dkDefaultCoord;
    m_minHeight = m_maxHeight = dkDefaultCoord;
    m_windowSizer = NULL;
    m_containingSizer = NULL;

    m_bIsFocus = FALSE;
    m_bIsVisible = TRUE;
    m_iTop = 0;
    m_iLeft = 0;
    m_iWidth = dkDefaultCoord;
    m_iHeight = dkDefaultCoord;
    m_bIsTabStop = TRUE;
    m_iTabIndex = 0;

    m_pEventHook = NULL;

    m_iTimerId = 0;

    m_bIsDisposed = FALSE;
    m_IsChanged = TRUE;
    m_zPosition = ZPOSITION_NORMAL;
    m_updateWindowFlag = PAINT_FLAG_NONE;
    m_backgroundColor = ColorManager::knWhite;
}

void UIWindow::SetImage(const std::string& srcImage)
{
    m_bgImage = srcImage;
    return;
}


std::string UIWindow::GetImage()
{
    return m_bgImage;
}

void UIWindow::ReleaseTimers()
{
    for (size_t i = 0; i < m_timers.size(); i++)
    {
        UITimerTask* pTimerTask = m_timers[i];
        pTimerTask->Cancel();
        delete pTimerTask;
    }

    m_timers.clear();

    for (size_t i = 0; i < m_cancelTimers.size(); i++)
    {
        delete m_cancelTimers[i];
    }

    m_cancelTimers.clear();
}

void UIWindow::OnResize(INT32 iWidth, INT32 iHeight)
{
    // this is empty, sub classes should override it
    UNUSED(iWidth);
    UNUSED(iHeight);
}

void UIWindow::OnEvent(UIEvent rEvent)
{
    DebugPrintf(DLC_MESSAGE, "%s::UIWindow::OnEvent(EventID=%d, Param=%d, Param2=%d), Id=%d, Text=%s",
        GetClassName(), rEvent.GetEventId(), rEvent.GetParam(), rEvent.GetParam2(), GetId(), GetText());

    UIWindow *pSender=rEvent.GetSender();
    if (pSender)
    {
        DebugPrintf(DLC_MESSAGE, "Sender=(Class=%s, Id=%d, Text=%s)",
            pSender->GetClassName(), pSender->GetId(), pSender->GetText());
    }
    else
    {
        DebugLog(DLC_MESSAGE, "Sender=NULL");
    }

    switch (rEvent.GetEventId())
    {
    case COMMAND_EVENT:
        OnCommand(rEvent.GetParam(), rEvent.GetSender(), rEvent.GetParam2());
        break;
    case CLICK_EVENT:
        OnClick();
        break;
    case SIZE_EVENT:
        if (m_pParent)
        {
            m_pParent->Layout();
        }
    default:
        break;
    }

    if (m_pEventHook != NULL)
    {
        m_pEventHook->HandleEvent(rEvent);
    }

    if (m_pParent != NULL)
    {
        m_pParent->OnNotify(rEvent);
    }
}

void UIWindow::OnNotify(UIEvent rEvent)
{
    DebugPrintf(DLC_MESSAGE, "%s::UIWindow::OnNotify(EventID=%d, Param=%d, Param2=%d), Id=%d, Text=%s",
        GetClassName(), rEvent.GetEventId(), rEvent.GetParam(), rEvent.GetParam2(), GetId(), GetText());

    UIWindow *pSender=rEvent.GetSender();
    if (pSender)
    {
        DebugPrintf(DLC_MESSAGE, "Sender=(Class=%s, Id=%d, Text=%s)",
            pSender->GetClassName(), pSender->GetId(), pSender->GetText());
    }
    else
    {
        DebugLog(DLC_MESSAGE, "Sender=NULL");
    }

    switch (rEvent.GetEventId())
    {
    case COMMAND_EVENT:
        OnCommand(rEvent.GetParam(), rEvent.GetSender(), rEvent.GetParam2());
        break;
    default:
        break;
    }

    if (m_pEventHook != NULL)
    {
        m_pEventHook->HandleEvent(rEvent);
    }

    if (m_pParent != NULL)
    {
        m_pParent->OnNotify(rEvent);
    }
}

void UIWindow::OnClick()
{
    DebugPrintf(DLC_MESSAGE, "UIWindow::OnClick(), ClassName=%s, Id=%d, Text=%s", GetClassName(), GetId(), GetText());
    if (m_pParent)
    {
        m_pParent->OnEvent(UIEvent(COMMAND_EVENT, this, GetId()));
    }
}

void UIWindow::OnCommand(DWORD dwCmdId, UIWindow *pSender, DWORD dwParam)
{
    // this is empty, sub classes should override it
    UNUSED(dwCmdId);
    UNUSED(pSender);
    UNUSED(dwParam);
}

void UIWindow::OnBroadcast(UIEvent rEvent)
{
    // empty

    // the following code is done to avoid compile warning
    UNUSED(rEvent);
}

void UIWindow::OnDispose(BOOL bDisposed)
{
    DebugPrintf(DLC_UIWINDOW, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (bDisposed)
    {
        return;
    }
    m_bIsDisposed = TRUE;

    m_bIsFocus = FALSE;
    m_bIsVisible = TRUE;
    m_iTop = 0;
    m_iLeft = 0;
    m_iWidth = 0;
    m_iHeight = 0;
    m_bIsTabStop = TRUE;

    if (m_pParent)
    {
        m_pParent->RemoveChild(this, false);
        m_pParent = NULL;
    }
    m_pEventHook = NULL;

    m_iTimerId = 0;

    ReleaseTimers();
    DebugPrintf(DLC_UIWINDOW, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

BOOL UIWindow::OnKeyPress(INT32 iKeyCode)
{
    DebugPrintf(DLC_MESSAGE, "%s::UIWindow::OnKeyPress(%d)", GetClassName(), iKeyCode);

    switch(iKeyCode)
    {
#ifdef KINDLE_FOR_TOUCH
    case KEY_HOME:
        {
            UISoftKeyboardIME* pIME = UISoftKeyboardIME::GetInstance();
            if (pIME && pIME->IsShow())
                pIME->HideIME();
            CPageNavigator::BackToHome();
        }
        break;
#else
    case KEY_PTRSC:		
	case KEY_PTRSC_K4NT:
        CDisplay::GetDisplay()->PrintSrceen();
        break;
    case KEY_OKAY:
        {
            EventArg e;
            e.iEventId = GetId();
            e.strMess = this->GetClassName();
            Event_OnClick.OnEvent(this,e);
        }
        if (m_bIsFocus)
        {
            UIEvent clickEvent(CLICK_EVENT,this,m_dwId);
            OnEvent(clickEvent);
        }
        else
        {
            if(m_pParent)
            {
                UIEvent clickEvent(CLICK_EVENT,this,m_dwId);
                m_pParent->OnEvent(clickEvent);
            }
            else
            {
                return TRUE;
            }
        }
        break;
    case KEY_ENTER:
        {
            CDisplay::GetDisplay()->ScreenRefresh();
        }
        break;
#endif
    default:
        return TRUE;
        break;
    }

    return FALSE;
}


HRESULT UIWindow::DrawFocus(DK_IMAGE imgParent)
{
    return S_OK;
}

HRESULT UIWindow::DrawBackGround(DK_IMAGE drawingImg)
{
    return S_OK;
}

void UIWindow::PopupDialog(UIDialog* pDialog)
{
    if (m_pParent != NULL)
    {
        m_pParent->PopupDialog(pDialog);
    }
}

void UIWindow::SetEnable(BOOL bEnable)
{
    m_bIsTabStop = bEnable;
}

void UIWindow::MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight)
{
    BOOL bStartPointChanged = (m_iTop != iTop || m_iLeft != iLeft);
    BOOL bSizeChanged = (m_iWidth != iWidth || m_iHeight != iHeight);

    if (bStartPointChanged)
    {
        m_iTop = iTop;
        m_iLeft = iLeft;
    }

    if (bSizeChanged)
    {
        m_iWidth = iWidth;
        m_iHeight = iHeight;
        OnResize(iWidth, iHeight);
    }
}

INT32 UIWindow::GetClientWidth()
{
    return GetWidth();
}

INT32 UIWindow::GetClientHeight()
{
    return GetHeight();
}

BOOL UIWindow::IsFocus() const
{
    return m_bIsFocus;
}

void UIWindow::SetFocus(BOOL bIsFocus)
{
    if (m_bIsFocus == bIsFocus)
    {
        return;
    }

    if (bIsFocus && m_pParent != NULL)
    {
        m_pParent->OnChildSetFocus(this);
    }

    m_bIsFocus = bIsFocus;

    UIEvent focusEvent(bIsFocus ? FOCUS_EVENT : BLUR_EVENT);
    OnEvent(focusEvent);

    UpdateFocus();
}

HRESULT UIWindow::UpdateFocus()
{
    //UpdateWindow();
    return S_OK;
}

BOOL UIWindow::IsEnable() const
{
    return m_bIsTabStop;
}

void UIWindow::SetReverseFocus(BOOL bIsFocus)
{
    SetFocus(bIsFocus);
}

void UIWindow::SetEventHook(IEventHook* pHook)
{
    m_pEventHook = pHook;
}

int UIWindow::GetAbsoluteX() const
{
    return m_pParent == NULL ? m_iLeft : m_pParent->GetAbsoluteX()+ m_iLeft;
}

int UIWindow::GetAbsoluteY() const
{
    return m_pParent == NULL ? m_iTop : m_pParent->GetAbsoluteY()+ m_iTop;
}

void UIWindow::OnTimer(INT32 iId)
{
    TRACE(CString(TEXT("Begin On Timer for id ["))+ iId + TEXT("]"));
    INT32 iIndex = GetUITimerTaskIndex(iId);

    if (-1 == iIndex)
    {
        return;
    }

    UITimerTask* pTimerTask = m_timers[iIndex];
    if (!pTimerTask->IsRepeat())
    {
        CancelTimer(iIndex);
    }

    TRACE(CString(TEXT("End On Timer for id ["))+ iId + TEXT("]"));
}

INT32 UIWindow::RegisterTimer(INT64 liInterval, BOOL bRepeat)
{
    TP_ASSERT(liInterval > 0);

    UITimerTask *pTmp = new UITimerTask(this, m_iTimerId, liInterval, bRepeat);
    if(pTmp)
    {
        int iId = m_iTimerId++;
        m_timers.push_back(pTmp);
        return iId;
    }
    else
    {
        return -1;
    }
}

BOOL UIWindow::UnregisterTimer(INT32 iId)
{
    INT32 iIndex = GetUITimerTaskIndex(iId);

    if (-1 == iIndex)
    {
        return FALSE;
    }

    CancelTimer(iIndex);

    return TRUE;
}

void UIWindow::CancelTimer(INT32 iIndex)
{
    UITimerTask* pTimerTask = m_timers[iIndex];
    pTimerTask->Cancel();
    m_timers.erase(m_timers.begin() + iIndex);

    // now the timer task instance will not be deleted
    // Note, here we can control the size of the cancel timers, it will be added if necessary
    m_cancelTimers.push_back(pTimerTask);
}

INT32 UIWindow::GetUITimerTaskIndex(INT32 iId)
{
    for (size_t i = 0; i < m_timers.size(); i++)
    {
        if (m_timers[i]->GetTimerId()== iId)
        {
            return i;
        }
    }

    return -1;
}

void UIWindow::SetMiddleKeyInfo(const CString& rstrTxt, SPtr<ITpImage> spIcon)
{
    if (m_pParent != NULL)
    {
        m_pParent->SetMiddleKeyInfo(rstrTxt, spIcon);
    }
}

void UIWindow::OnMenuChanged()
{
    TRACE("UIWindow::OnMenuChanged");

    if (m_pParent)
    {
        m_pParent->OnChildMenuChanged(this);
    }
}

bool UIWindow::PointInWindow(int x, int y)
{
#ifdef KINDEL_FOR_TOUCH
    return m_bIsVisible 
        && x >= -m_touchSlot && x < m_iWidth + m_touchSlot 
        && y >= -m_touchSlot && y < m_iHeight + m_touchSlot;
#else
    return m_bIsVisible && x >= 0 && x < m_iWidth && y >= 0 && y < m_iHeight;
#endif
}

bool UIWindow::PointInWindow(int x, int y, int slot)
{
    return m_bIsVisible && x >= -slot && x < m_iWidth + slot && y >= -slot && y < m_iHeight + slot;
}

void UIWindow::SynchronizedCall(IRunnable& /*rRunnable*/)
{

}

UIContainer* UIWindow::GetParent() const
{
    return m_pParent;
}

const DWORD UIWindow::GetId()
{
    return m_dwId;
}

void UIWindow::SetId(DWORD rstrId)
{
    m_dwId = rstrId;
}

void UIWindow::SetParent(UIContainer* pParent)
{
    m_pParent = pParent;
}

void UIWindow::Dispose()
{
    OnDispose(m_bIsDisposed);
}

INT32 UIWindow::GetX() const
{
    return m_iLeft;
}

INT32 UIWindow::GetY() const
{
    return m_iTop;
}

BOOL UIWindow::IsVisible() const
{
    return m_bIsVisible;
}

bool UIWindow::SetLeftMargin(int _iLeftMargin)
{
    if(_iLeftMargin < 0)
        return false;
    m_iLeftMargin = _iLeftMargin;
    return true;
}

bool UIWindow::SetTopMargin(int _iTopMargin)
{
    if(_iTopMargin < 0)
        return false;
    m_iTopMargin = _iTopMargin;
    return true;
}

BOOL UIWindow::SetVisible(BOOL bVisible)
{
#ifndef KINDLE_FOR_TOUCH
    EventArg e;
    e.strMess = this->GetClassName();
    e.strMess.append(" SetVisible");
    if(bVisible)
    {
        e.strMess.append(" True");
        Event_OnShow.OnEvent(this,e);
    }
    else
    {
        e.strMess.append(" False");
        Event_OnHide.OnEvent(this,e);
    }
#endif
    return m_bIsVisible = bVisible;
}


//显示当前控件，或者用父窗口内容，即当前窗口消失
void UIWindow::Show(BOOL bIsShow, BOOL bIsRepaint)
{
    if (m_bIsVisible == bIsShow)
    {
        return;
    }

    DebugPrintf(DLC_EINK_PAINT, "%s::UIWindow::Show()", GetClassName());

    this->SetVisible(bIsShow);

    UIEvent showEvent(bIsShow ? SHOW_EVENT : HIDE_EVENT);
    OnEvent(showEvent);

    if(FALSE == bIsRepaint)
        return;

    if (bIsShow)
    {
        UpdateWindow();
    }
    else
    {
        // 把下面的窗口全都重新画一遍。
        UIWindow::Repaint();
    }
}

void UIWindow::SetUpdateWindowFlag(paintflag_t updateWindowFlag)
{
    m_updateWindowFlag = updateWindowFlag;
}

void UIWindow::SetZPosition(int zPosition)
{
    m_zPosition = zPosition;
}

int UIWindow::GetZPosition() const
{
    return m_zPosition;
}

bool UIWindow::IsDisplay() const
{
    bool ret = IsVisible() && IsAllParentsVisible();
    GUISystem* pGUI = GUISystem::GetInstance();
    if (ret && pGUI)
    {
        UIContainer* pContainer = pGUI->GetTopFullScreenContainer();
        if (pContainer)
        {
            ret = ((pContainer == this) || IsChildOf(pContainer));
        }
    }
    return ret;
}

bool UIWindow::IsChildOfTopPage()
{
    return (GetUIPage() == CDisplay::GetDisplay()->GetCurrentPage());
}

bool UIWindow::IsAllParentsVisible() const
{
    UIContainer* pParent = m_pParent;
    while (pParent)
    {
        if (pParent->IsVisible())
        {
            pParent = pParent->GetParent();
        }
        else
        {
            return false;
        }
    }
    return true;
}

void UIWindow::AppendChildToTopFullScreenContainer()
{
    UIContainer* pParent = m_pParent;
    GUISystem* pGUI = GUISystem::GetInstance();
    if (pGUI)
    {
        pParent = pGUI->GetTopFullScreenContainer();
    }
    if (pParent)
    {
        pParent->AppendChild(this);
        SetParent(pParent);
    }
}

UIWindow* UIWindow::GetTopmostWindow() const
{
    UIContainer* pTopFullScreen = GUISystem::GetInstance()->GetTopFullScreenContainer();
    if (pTopFullScreen)
    {
        int index = pTopFullScreen->GetChildrenCount() - 1;
        while (index >= 0)
        {
            UIWindow* pChild = pTopFullScreen->GetChildByIndex(index);
            if (pChild && pChild->IsDisplay())
            {
                return (pChild->GetZPosition() >= ZPOSITION_DIALOG) ? pChild : NULL;
            }
            index--;
        }
    }
    return NULL;
}

HRESULT UIWindow::UpdateWindow(paintflag_t paintFlag, bool drawImmediately)
{
    UIWindow* pTopmostWnd = GetTopmostWindow();
    if (pTopmostWnd)
    {
        //输入法在中英文切换时需要重绘底层窗口
        if (((this == pTopmostWnd) && (GetZPosition() != ZPOSITION_IME))
            || IsChildOf((UIContainer*)pTopmostWnd))
        {
            return RedrawWindow(paintFlag, drawImmediately);
        }
        else
        {
            UIContainer* pWnd = GUISystem::GetInstance()->GetTopFullScreenContainer();
            if (pWnd)
            {
                return pWnd->RedrawWindow(paintFlag, drawImmediately);
            }
        }
    }
    return RedrawWindow(paintFlag, drawImmediately);
}

HRESULT UIWindow::RedrawWindow(paintflag_t paintFlag, bool drawImmediately)
{
    DebugPrintf(DLC_MESSAGE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (!IsDisplay())
    {
        return S_OK;
    }

    HRESULT hr(S_OK);
    DK_IMAGE imgParent;
    DK_RECT rectParent;
    RTN_HR_IF_FAILED(GetParentDC(imgParent, rectParent));
    RTN_HR_IF_FAILED(Draw(imgParent));

    DK_RECT updatedRect = {
        rectParent.left + m_iLeft,
        rectParent.top + m_iTop,
        rectParent.left + m_iLeft + m_iWidth,
        rectParent.top + m_iTop + m_iHeight
    };
    UpdateToScreen(updatedRect, paintFlag, drawImmediately);
    return hr;
}

HRESULT UIWindow::UpdateWindow(int left, int top, int width, int height, paintflag_t paintFlag, bool drawImmediately)
{

    UIWindow* pTopmostWnd = GetTopmostWindow();
    if (pTopmostWnd)
    {
        //输入法在中英文切换时需要重绘底层窗口
        if (((this == pTopmostWnd) && (GetZPosition() != ZPOSITION_IME))
            || IsChildOf((UIContainer*)pTopmostWnd))
        {
            return RedrawWindow(left, top, width, height, paintFlag, drawImmediately);
        }
        else
        {
            UIContainer* pWnd = GUISystem::GetInstance()->GetTopFullScreenContainer();
            if (pWnd)
            {
                return pWnd->RedrawWindow(paintFlag, drawImmediately);
            }
        }
    }
    return RedrawWindow(left, top, width, height, paintFlag, drawImmediately);
}

HRESULT UIWindow::RedrawWindow(int left, int top, int width, int height, paintflag_t paintFlag, bool drawImmediately)
{
    if (!IsDisplay())
    {
        return S_OK;
    }

    HRESULT hr(S_OK);
    DK_IMAGE imgParent;
    DK_RECT rectParent;
    RTN_HR_IF_FAILED(GetParentDC(imgParent, rectParent));
    RTN_HR_IF_FAILED(Draw(imgParent));

    DK_RECT updatedRect = {
        rectParent.left + m_iLeft + left,
        rectParent.top + m_iTop + top,
        rectParent.left + m_iLeft + left + width,
        rectParent.top + m_iTop + top + height
    };
    UpdateToScreen(updatedRect, paintFlag, drawImmediately);
    return hr;
}

HRESULT UIWindow::GetParentDC(DK_IMAGE &rImgParent, DK_RECT &rRectParent)
{
    HRESULT hr(S_OK);
    CDisplay* display = CDisplay::GetDisplay();

    INT32 iParentX = (m_pParent != NULL ? m_pParent->GetAbsoluteX() : 0);
    INT32 iParentY = (m_pParent != NULL ? m_pParent->GetAbsoluteY() : 0);
    INT32 iParentWidth = (m_pParent != NULL ? m_pParent->GetWidth() : display->GetScreenWidth());
    INT32 iParentHeight = (m_pParent != NULL ? m_pParent->GetHeight() : display->GetScreenHeight());

    DK_IMAGE graphics = display->GetMemDC();
    rRectParent.left = iParentX;
    rRectParent.top = iParentY;
    rRectParent.right = iParentX + iParentWidth;
    rRectParent.bottom =iParentY + iParentHeight;
    RTN_HR_IF_FAILED(CropImage(
        graphics,
        rRectParent,
        &rImgParent
        ));

    return hr;
}

void UIWindow::UpdateToScreen(DK_RECT updatedRect, paintflag_t paintFlag, bool drawImmediately)
{
    CDisplay::GetDisplay()->ScreenRepaint(
        updatedRect.left,
        updatedRect.top,
        updatedRect.right - updatedRect.left,
        updatedRect.bottom - updatedRect.top,
        paintFlag,
        drawImmediately
        );
}

void UIWindow::DrawBackgroundWithRoundBorder(DK_IMAGE img)
{
    DK_IMAGE imgSelf;

    DK_RECT rcSelf={0, 0, m_iWidth, m_iHeight};

    CropImage(
        img,
        rcSelf,
        &imgSelf);

    CTpGraphics grp(imgSelf);

    SPtr<ITpImage> spImg = ImageManager::GetImage(IMAGE_MENU_BACK);
    if (spImg)
    {
        grp.DrawImageStretchBlend(spImg.Get(), 0, 0, m_iWidth, m_iHeight);
    }
}

INT32 UIWindow::GetHeight() const
{
    return m_iHeight;
}

INT32 UIWindow::GetWidth() const
{
    return m_iWidth;
}

INT32 UIWindow::GetTabIndex() const
{
    return m_iTabIndex;
}

void UIWindow::SetTabIndex(const int iTabIndex)
{
    m_iTabIndex = iTabIndex;
}

void UIWindow::Repaint()
{
    DebugPrintf(DLC_MESSAGE, "%s::UIWindow::Repaint() m_pParent=0x%08X", GetClassName(), m_pParent);
    /*UIWindow* top = this;
    while (top != NULL && top->GetParent() != NULL)
    {
        if (top != this && !top->IsVisible())
        {
            return;
        }
        top = top->GetParent();
    }
    // 如果不是顶层窗口，则不repaint
    if (top != NULL && CPageNavigator::IsInStack(top) && !CPageNavigator::IsOnTop(top))
    {
        return;
    }*/
    // 如果不是顶层窗口，则不repaint
    if ( !IsChildOfTopPage())
    {
        return;
    }


    //if (m_pParent != NULL)
    //{
    //    m_pParent->Repaint();
    //}
    CDisplay* pDisplay = CDisplay::GetDisplay();
    if (pDisplay)
    {
        pDisplay->Repaint(PAINT_FLAG_REPAINT);
    }
    
    DebugPrintf(DLC_MESSAGE, "%s::UIWindow::Repaint() Exit..", GetClassName());
}

LPCSTR UIWindow::GetText() const
{
    return m_strText;
}

INT32 UIWindow::GetScreenHeight()
{
    return m_pParent ? m_pParent->GetScreenHeight() : DeviceInfo::GetDisplayScreenHeight();
}

INT32 UIWindow::GetScreenWidth()
{
    return m_pParent ? m_pParent->GetScreenWidth() : DeviceInfo::GetDisplayScreenWidth();
}

BOOL UIWindow::OnKeyRelease(INT32 iKeyCode)
{
    // Note, this is empty
    UNUSED(iKeyCode);

    return TRUE;
}

void UIWindow::PopupMenu(UIMenu* pMenu, INT32 iX, INT32 iY)
{
    if (m_pParent)
    {
        m_pParent->PopupMenu(pMenu, GetX() + iX, GetY() + iY);
    }
}

UIPage* UIWindow::GetUIPage()
{
    return (NULL != m_pParent) ? m_pParent->GetUIPage() : NULL;
}

void UIWindow::SetSizer(UISizer* sizer, bool deleteOldSizer)
{
    if (sizer == m_windowSizer)
        return;

    if (m_windowSizer)
    {
        m_windowSizer->SetContainingWindow(NULL);
        if (deleteOldSizer)
            delete m_windowSizer;
    }
    
    m_windowSizer = sizer;
    if (m_windowSizer)
    {
        m_windowSizer->SetContainingWindow((UIWindow*)this);
    }

    //SetAutoLayout(m_windowSizer != NULL);
}

bool UIWindow::InformFirstDirection(int direction, int size, int availableOtherDir)
{
    return GetSizer() && GetSizer()->InformFirstDirection(direction, size, availableOtherDir);
}

void UIWindow::DoSetSize(int x, int y, int width, int height, int sizerFlags)
{
    (void)(sizerFlags);

    if (x>=0 && y>=0 && width>0 && height>0)
    {
        const bool sizeChange = m_iWidth != width || m_iHeight != height;
        if (sizeChange || m_iTop != x || m_iLeft != y)
        {
            MoveWindow(x, y, width, height);
        }
    }
    else
    {
        m_iWidth = width;
        m_iHeight = height;
    }

}

void UIWindow::SetSize(int x, int y, int width, int height, int sizerFlags)
{
    DoSetSize(x, y, width, height, sizerFlags);
}

bool UIWindow::Layout()
{
    if (m_windowSizer)
    {
        //struct timeval startTime, endTime;
        //gettimeofday(&startTime, NULL);
        m_windowSizer->SetDimension(0, 0, m_iWidth, m_iHeight);
        //gettimeofday(&endTime, NULL);
        //unsigned long elaspedTime = (endTime.tv_sec - startTime.tv_sec)*1e6 + (endTime.tv_usec - startTime.tv_usec);
        //double elaspedTimeD = elaspedTime/1000000.0;
        //DebugPrintf(DLC_GUI, "%s::Layout used Time: %f", GetClassName(), elaspedTimeD);
        return true;
    }

    return false;
}

bool UIWindow::IsFullScreen() const
{
    return IsVisible() && m_iWidth == DeviceInfo::GetDisplayScreenWidth() && m_iHeight == DeviceInfo::GetDisplayScreenHeight();
}

UIContainer* UIWindow::GetTopFullScreenContainer()
{
    return NULL;
}

void UIWindow::MakeCenter(int width, int height)
{
    int screenWidth = DeviceInfo::GetDisplayScreenWidth();
    int screenHeight = DeviceInfo::GetDisplayScreenHeight();
    int left = (screenWidth - width) / 2;
    int top = (screenHeight - height) /2;
    UIContainer* parent = GetParent();
    if (parent)
    {
        left -= parent->GetAbsoluteX();
        top -= parent->GetAbsoluteY();
    }
    MoveWindow(left, top, width, height);
}

void UIWindow::MakeCenterAboveIME(int width, int height)
{
    int screenWidth = DeviceInfo::GetDisplayScreenWidth();
    int left = (screenWidth - width) / 2;
    const int imeTopY = GetWindowMetrics(UISoftKeyboardIMETopYIndex);
    const int imeUpVertSpacing = GetWindowMetrics(IMEUpVertSpacingIndex);
    int top = imeTopY - imeUpVertSpacing - height;
    UIContainer* parent = GetParent();
    if (parent)
    {
        left -= parent->GetAbsoluteX();
        top -= parent->GetAbsoluteY();
    }
    MoveWindow(left, top, width, height);
}


#ifdef KINDLE_FOR_TOUCH
void UIWindow::SetPressed(bool pressed)
{
    bool needRefresh = pressed != ((m_privateFlags & PF_PRESSED) == PF_PRESSED);
    if (needRefresh)
    {
        if (pressed)
        {
            m_privateFlags |= PF_PRESSED;
        }
        else
        {
            m_privateFlags &= ~PF_PRESSED;
        }
        if ((pressed && IsUpdateOnPress())
                    || (!pressed && IsUpdateOnUnPress()))
        {
            //DebugPrintf(DLC_DIAGNOSTIC, "Update on press %d, %s", IsPressed(), GetText());
            CDisplay::CacheDisabler disableCache;
            UpdateWindow(m_updateWindowFlag, true);
        }
    }
}

bool UIWindow::IsPressed()
{
    return m_privateFlags & PF_PRESSED;
}

void UIWindow::SetTouchSlot(int toushSlot)
{
    m_touchSlot = toushSlot;
}

int UIWindow::GetTouchSlot() const
{
    return m_touchSlot;
}

bool UIWindow::DispatchTouchEvent(MoveEvent* moveEvent)
{
    return OnTouchEvent(moveEvent);
}

bool UIWindow::OnTouchEvent(MoveEvent* moveEvent)
{
    DebugPrintf(DLC_GUI, "%s::OnTouchEvent(), %d", GetClassName(), moveEvent->GetActionMasked());
    if (!IsEnable())
    {
        if (moveEvent->GetAction() == MoveEvent::ACTION_UP && (m_privateFlags & PF_PRESSED) != 0)
        {
            SetPressed(false);
        }
        if (!ShouldPropogateTouch())
        {
            return true;
        }
        return (m_windowFlags & WF_CLICKABLE) == WF_CLICKABLE;
    }

    if (m_windowFlags & WF_CLICKABLE)
    {
        switch (moveEvent->GetAction())
        {
        case MoveEvent::ACTION_UP:
            if (IsPressed())
            {
                m_lastUpEvent = *moveEvent;
                StartUnPress(false, m_lastDownEvent.GetEventTime() + 100, 0);
                //SetPressed(false); //, moveEvent->GetEventTime() + 100, 0);
                StopCheckForLongClick();
            }
            break;
        case MoveEvent::ACTION_MOVE:
            {
                int x = moveEvent->GetX();
                int y = moveEvent->GetY();
                if (!PointInWindow(x, y, GUIConfiguration::GetInstance()->GetTouchSlot()))
                {
                    SetPressed(false);
                }
                if (IsPressed() && 
                        (x - m_touchDownX) * (x - m_touchDownX) + (y - m_touchDownY) * (y - m_touchDownY) > GUIConfiguration::GetInstance()->GetClickSlotSquare())
                {
                    SetPressed(false);
                }
                if (!IsPressed())
                {
                    if (IsUpdateOnPress())
                    {
                        UpdateWindow(PAINT_FLAG_DU, true);
                    }
                    StopCheckForLongClick();
                }
            }
            break;
        case MoveEvent::ACTION_DOWN:
            m_touchDownX = moveEvent->GetX();
            m_touchDownY = moveEvent->GetY();
            m_lastDownEvent = *moveEvent;
            m_outClickSlot = false;
            m_hasPerformLongPress = false;
            SetPressed(true);
            if (IsLongClickable())
            {
                StartCheckForLongClick(false, moveEvent->GetEventTime() + GUIConfiguration::GetInstance()->GetLongPressTimeout(), 0);
            }
            break;
        case MoveEvent::ACTION_CANCEL:
            if (IsPressed())
            {
                StopCheckForLongClick();
            }
            DebugPrintf(DLC_GUI, "Cancel %s", GetClassName());
            SetPressed(false);
            if (IsUpdateOnPress())
            {
                UpdateWindow(PAINT_FLAG_DU, true);
            }
            break;
        }
        return true;
    }
    if (ShouldPropogateTouch())
    {
        return false;
    }
    else
    {
        return true;
    }
}

IMPLEMENT_TIMER_THREAD_FUNC_WITHSTART(UIWindow, CheckForLongClick);

void UIWindow::DoCheckForLongClick()
{
    if (IsPressed())
    {
        PerformLongClick();
    }
}

void UIWindow::PerformLongClick()
{
    DebugPrintf(DLC_GUI, "%s::PerformLongClick()", GetClassName());
    m_hasPerformLongPress = true;
    UIContainer* parent = GetParent();
    if (NULL == parent || !parent->OnChildLongPress(this))
    {
        LongPressArgs longPressArgs(this);
        FireEvent(EventLongPress, longPressArgs);
    }
}

IMPLEMENT_TIMER_THREAD_FUNC_WITHSTART(UIWindow, UnPress);
void UIWindow::DoUnPress()
{
    if (IsPressed())
    {
        //DebugPrintf(DLC_DIAGNOSTIC, "DoUnpress %s", GetText());
        SetPressed(false);
        if (!m_hasPerformLongPress)
        {
            PerformClick(&m_lastUpEvent);
        }
    }
}

void UIWindow::PerformClick(MoveEvent* moveEvent)
{
    UIContainer* parent = GetParent();
    if (NULL == parent || !parent->OnChildClick(this))
    {
        DebugPrintf(DLC_GUI, "%s::PerformClick()", GetClassName());
        ClickArgs clickArgs(this, moveEvent->GetX(), moveEvent->GetY());
        FireEvent(EventClick, clickArgs);
    }
}


bool UIWindow::PointInWindowAbsolute(int xAbs, int yAbs)
{
    int x = GetAbsoluteX();
    int y = GetAbsoluteY();
    return PointInWindow(xAbs - x, yAbs - y);
}

void UIWindow::HookTouch()
{
    if (m_hookingTouch)
    {
        return;
    }
    GlobalEventListener::GetInstance()->RegisterTouchHook(&TouchHook, (void*)GetHandle());
    m_hookingTouch = true;
}

void UIWindow::UnhookTouch()
{
    if (!m_hookingTouch)
    {
        return;
    }
    GlobalEventListener::GetInstance()->UnRegisterTouchHook(&TouchHook, (void*)GetHandle());
    m_hookingTouch = false;
}

bool UIWindow::OnHookTouch(MoveEvent* moveEvent)
{
    return false;
}

void UIWindow::DrawBackgroundEx(
        DK_IMAGE drawingImg,
        dkColor backgroundColor,
        bool showBorder,
        int borderLineWidth,
        bool upLeftRound,
        bool upRightRound,
        bool downRightRound,
        bool downLeftRound)
{
    CTpGraphics grf(drawingImg);
    int width = drawingImg.iWidth;
    int height = drawingImg.iHeight;
    grf.EraserBackGround(backgroundColor);
    if (showBorder)
    {
        grf.FillRect(0, 0, width, height, ColorManager::knBlack);
        grf.FillRect(borderLineWidth, borderLineWidth, width - borderLineWidth, height - borderLineWidth, backgroundColor);
        if (upLeftRound)
        {
            SPtr<ITpImage> pUpLeftImage = ImageManager::GetImage(!IsEnable() ? IMAGE_ICON_BTN_UPLEFT_DISABLE : (this->IsFocus() ? IMAGE_TOUCH_ICON_BTN_UPLEFT_FOCUS : IMAGE_TOUCH_ICON_BTN_UPLEFT));
            grf.DrawImageBlend(pUpLeftImage.Get(), 0, 0, 0, 0, pUpLeftImage->GetWidth(),  pUpLeftImage->GetHeight());
        }
        if (upRightRound)
        {
            SPtr<ITpImage> pUpRightImage = ImageManager::GetImage(!IsEnable() ? IMAGE_ICON_BTN_UPRIGHT_DISABLE : (this->IsFocus() ? IMAGE_TOUCH_ICON_BTN_UPRIGHT_FOCUS : IMAGE_TOUCH_ICON_BTN_UPRIGHT));
            int iUpRightWidth = pUpRightImage->GetWidth();
            grf.DrawImageBlend(pUpRightImage.Get(), width - iUpRightWidth, 0, 0, 0, iUpRightWidth,  pUpRightImage->GetHeight());
        }
        if(downRightRound)
        {
            SPtr<ITpImage> pLeftDownImage = ImageManager::GetImage(!IsEnable() ? IMAGE_ICON_BTN_LEFTDOWN_DISABLE : (this->IsFocus() ? IMAGE_TOUCH_ICON_BTN_LEFTDOWN_FOCUS : IMAGE_TOUCH_ICON_BTN_LEFTDOWN));
            int iLeftDownWidth  = pLeftDownImage->GetWidth();
            int iLeftDownHeight = pLeftDownImage->GetHeight();
            grf.DrawImageBlend(pLeftDownImage.Get(), 0, height - iLeftDownHeight, 0, 0, iLeftDownWidth,  iLeftDownHeight);
        }
        if(downLeftRound)
        {
            SPtr<ITpImage> pRightDownImage = ImageManager::GetImage(!IsEnable() ? IMAGE_ICON_BTN_RIGHTDOWN_DISABLE : (this->IsFocus() ? IMAGE_TOUCH_ICON_BTN_RIGHTDOWN_FOCUS : IMAGE_TOUCH_ICON_BTN_RIGHTDOWN));
            int iRightDownWidth  = pRightDownImage->GetWidth();
            int iRightDownHeight = pRightDownImage->GetHeight();
            grf.DrawImageBlend(pRightDownImage.Get(), width - iRightDownWidth, height - iRightDownHeight, 0, 0, iRightDownWidth,  iRightDownHeight);
        }
    }
}

void UIWindow::SetUpdateOnPress(bool update)
{
    if (update)
    {
        m_windowFlags |= WF_UPDATE_ON_PRESS;
    }
    else
    {
        m_windowFlags &= ~WF_UPDATE_ON_PRESS;
    }
}

bool UIWindow::IsUpdateOnPress() const
{
    return m_windowFlags & WF_UPDATE_ON_PRESS;
}

void UIWindow::SetUpdateOnUnPress(bool update)
{
    if (update)
    {
        m_windowFlags |= WF_UPDATE_ON_UNPRESS;
    }
    else
    {
        m_windowFlags &= ~WF_UPDATE_ON_UNPRESS;
    }
}

bool UIWindow::IsUpdateOnUnPress() const
{
    return m_windowFlags & WF_UPDATE_ON_UNPRESS;
}
#endif

bool UIWindow::IsChildOf(UIContainer* container) const
{
    UIContainer* parent = GetParent();
    while (parent != NULL && parent != container)
    {
        parent = parent->GetParent();
    }
    return parent != NULL;
}

