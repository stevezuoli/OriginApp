////////////////////////////////////////////////////////////////////////
// UIWindow.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UIWINDOW_H__
#define __UIWINDOW_H__

#include "CommandID.h"

#include "Utility/BasicType.h"
#include "GUI/EventListener.h"
#include "Public/GUI/IUITimerListener.h"
#include "GUI/ITpImage.h"
#include "GUI/ITpGraphics.h"
#include "GUI/UIEvent.h"
#include "GUI/IEventHook.h"
#include "Utility/IRunnable.h"
#include "DkSPtr.h"

//#include "Public/GUI/IUIWindow.h"
#include "GUI/IGUI.h"
#include "GUI/IMenuSource.h"
#include "GUI/CTpImage.h"
#include "GUI/Event.h"
#include "Utility/ImageManager.h"
#include "GUI/EventArgs.h"

#include "dkImage.h"
#include "dkWinError.h"
#include "interface.h"
#include "Framework/INativeMessageQueue.h"
#include "GUI/EventSet.h"
#include "Common/Defs.h"
#ifdef KINDLE_FOR_TOUCH
#include "drivers/TouchDrivers.h"
#include "drivers/MoveEvent.h"
#else
#include "Framework/Delegate.h"
#endif

//using namespace DkFormat;

class UIContainer;
class UIMenu;
class UIDialog;
class UIPage;
class UITimerTask;
class UIEvent;
class EventSet;
class UISizer;
class UIWindow;
#ifdef KINDLE_FOR_TOUCH
class MoveEvent;
class ClickArgs: public EventArgs
{
public:
    ClickArgs(UIWindow* win, int x_, int y_)
        : window(win)
        , x(x_)
        , y(y_)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new ClickArgs(*this);
    }
    UIWindow* window;
    int x;
    int y;
};

class LongPressArgs: public EventArgs
{
public:
    LongPressArgs(UIWindow* win)
        : window(win)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new LongPressArgs(*this);
    }
    UIWindow* window;
};
#endif
    
enum
{
    ZPOSITION_NORMAL = 0,
    ZPOSITION_CONTAINER = 1,
    ZPOSITION_DIALOG = 2,
    ZPOSITION_IME = 3,
};

class UIWindow 
    : public IUITimerListener
    , public EventSet
    , public EventListener
{
public:
#ifdef KINDLE_FOR_TOUCH
    static const char* EventClick;
    static const char* EventLongPress;
#endif
    virtual const char* GetClassName() const
    {
        return "UIWindow";
    }

    enum
    {
        INVALID_TIMER_ID = -1
    };

    ////////////////////Constructor Section/////////////////////////
    virtual ~UIWindow();
    ////////////////////Constructor Section/////////////////////////

    ////////////////////Method Section/////////////////////////

    virtual BOOL OnKeyPress(int iKeyCode);
    virtual BOOL OnKeyRelease(int iKeyCode);

    virtual void SetImage(const std::string& srcImage);
    virtual std::string GetImage();

    virtual void Repaint();

    virtual void PopupMenu(UIMenu* pMenu, int iX, int iY);

    virtual void MoveWindow(int iLeft, int iTop, int iWidth, int iHeight);

    virtual int GetClientWidth();
    virtual int GetClientHeight();

    virtual BOOL IsFocus() const;
    virtual void SetFocus(BOOL bIsFocus);
    virtual BOOL IsEnable() const;
    virtual void SetEnable(BOOL bEnable);

    virtual void SetReverseFocus(BOOL bIsFocus);
    virtual void SetEventHook(IEventHook* pHook);

    // TODO: Should add ScreenToClient, ClientToScreen to replace these
    virtual int GetAbsoluteX() const;
    virtual int GetAbsoluteY() const;

    virtual void OnTimer(int iId);
    virtual int RegisterTimer(INT64 liInterval, BOOL bRepeat);
    BOOL UnregisterTimer(int iId);

    virtual void SetMiddleKeyInfo(const CString& rstrTxt, SPtr<ITpImage> spIcon);

    bool PointInWindowAbsolute(int xAbs, int yAbs);
    // iX, iY is relative to window
    bool PointInWindow(int x, int y);
    bool PointInWindow(int x, int y, int slot);

    void SynchronizedCall(IRunnable& rRunnable);

    virtual UIContainer* GetParent() const;
    virtual void SetParent(UIContainer* pParent);

    virtual const DWORD GetId();
    virtual void SetId(DWORD rstrId);

    virtual int GetX() const;
    virtual int GetY() const;
    virtual int GetWidth() const;
    virtual int GetHeight() const;
    int GetRight() const
    {
        return GetX() + GetWidth();
    }

    int GetBottom() const
    {
        return GetY() + GetHeight();
    }

    virtual bool SetLeftMargin(int _iLeftMargin);
    virtual bool SetTopMargin(int _iTopMargin);

    virtual int  GetLeftMargin() const { return m_iLeftMargin; }
    virtual int  GetTopMargin() const { return m_iTopMargin; }

    virtual BOOL IsVisible() const;
    virtual BOOL SetVisible(BOOL bVisible);
    virtual void Show(BOOL bIsShow, BOOL bIsRepaint = TRUE);
    virtual BOOL IsShown() const { return IsVisible(); }

    virtual int GetTabIndex() const;
    virtual void SetTabIndex(const int iTabIndex);

    virtual LPCSTR GetText() const;
    virtual int GetScreenHeight();
    virtual int GetScreenWidth();

    virtual HRESULT Draw(DK_IMAGE drawingImg) = 0;

    /// <summary>
    /// Responses to the event sent from its parent.
    /// This does nothing in this class.
    /// </summary>
    virtual void OnBroadcast(UIEvent rEvent);
    virtual void OnEvent(UIEvent rEvent);

    virtual void OnNotify(UIEvent rEvent);
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    virtual void OnClick();

    virtual void PopupDialog(UIDialog* pDialog);

    virtual UIPage* GetUIPage();
    bool IsChildOfTopPage();
    bool IsAllParentsVisible() const;
    // TODO(JUGH) 如果修改IsShown会影响到UISizer的相关设置(IsChildOf()=FALSE)，待统一处理
    bool IsDisplay() const;
    HRESULT RedrawWindow(paintflag_t paintFlag = PAINT_FLAG_NONE, bool drawImmediately = false);
    HRESULT RedrawWindow(int left, int top, int width, int height,
        paintflag_t paintFlag = PAINT_FLAG_NONE, bool drawImmediately = false);
    void AppendChildToTopFullScreenContainer();
    UIWindow* GetTopmostWindow() const;

    void SetUpdateWindowFlag(paintflag_t updateWindowFlag);
    void SetZPosition(int zPosition);
    int GetZPosition() const;

    // 立即重刷控件, 只刷新控件区域到屏幕上，不会重刷整屏
    virtual HRESULT UpdateWindow(paintflag_t paintFlag = PAINT_FLAG_NONE, bool drawImmediately = false);
    virtual HRESULT UpdateWindow(int left, int top, int width, int height,
        paintflag_t paintFlag = PAINT_FLAG_NONE, bool drawImmediately = false);

#ifndef KINDLE_FOR_TOUCH
    virtual void OnMessage(const SNativeMessage& /* msg */){}
#endif

    //Get the default size for the new window if no explicit size given.TLWS
    //have their own default size so this is just for non top-level windows.
    static int WidthDefault(int w) {return w == dkDefaultCoord ? 20 : w;};
    static int HeightDefault(int h) {return h == dkDefaultCoord ? 20 : h;};

    dkSize GetEffectiveMinSize() const;

    virtual bool Layout();

    virtual void SetMinSize(int w, int h)
    {
        m_minWidth = w;
        m_minHeight = h;
    }
    virtual void SetMinSize(const dkSize& minSize)
    {
        m_minWidth = minSize.x;
        m_minHeight = minSize.y;
    }
    virtual void SetMaxSize(const dkSize& maxSize)
    {
        m_maxWidth = maxSize.x;
        m_maxHeight = maxSize.y;
    }
    void SetSize(const dkSize& size)
    {
        DoSetSize(dkDefaultCoord, dkDefaultCoord, size.x, size.y, dkSIZE_USE_EXISTING);
    }
    virtual void DoSetSize(int x, int y, int width, int height, int sizerFlags = dkSIZE_AUTO);
    virtual void SetClientSize(const dkSize& size)
    {
        SetSize(size);
    }
    void SetSize(int x, int y, int width, int height, int sizeFlags = dkSIZE_AUTO);

    virtual dkSize GetMinSize() const
    {
        return dkSize(m_minWidth, m_minHeight);
    }
    virtual dkSize GetMaxSize() const
    {
        return dkSize(m_maxWidth, m_maxHeight);
    }
    virtual dkSize GetSize() const
    {
        return dkSize(m_iWidth, m_iHeight);
    }
    virtual dkSize GetClientSize() const
    {
        return GetSize();
    }

    void SetSizer(UISizer* sizer, bool deleteOldSizer = true);
    UISizer* GetSizer() const
    {
        return m_windowSizer;
    }

    void SetContainingSizer(UISizer* sizer)
    {
        m_containingSizer = sizer;
    }
    UISizer* GetContainingSizer() const
    {
        return m_containingSizer;
    }
    
    virtual void SetText(const char* pchTxt){}

    void SetMinWidth(int minWidth)
    {
        m_minWidth = minWidth;
    }
    void SetMinHeight(int minHeight)
    {
        m_minHeight = minHeight;
    }

    int GetMinWidth() const
    {
        return GetMinSize().x;
    }
    int GetMinHeight() const
    {
        return GetMinSize().y;
    }
    int GetMaxWidth() const
    {
        return GetMaxSize().x;
    }
    int GetMaxHeight() const
    {
        return GetMaxSize().y;
    }
    // wxSizer and friends use this to give a chance to a component to recalc
    // its min size once one of the final size components is known. Override
    // this function when that is useful (such as for wxStaticText which can
    // stretch over several lines). Parameter availableOtherDir
    // tells the item how much more space there is available in the opposite
    // direction (-1 if unknown).
    virtual bool InformFirstDirection(int direction, int size, int availableOtherDir);

    void SetAutoLayout(BOOL autoLayout)
    {
        m_autoLayout = autoLayout;
    }

    bool GetAutoLayout() const
    {
        return m_autoLayout;
    }

    bool IsFullScreen() const;
    virtual UIContainer* GetTopFullScreenContainer();

#ifdef KINDLE_FOR_TOUCH
    virtual bool ContainPoint(int _x, int _y);
    virtual bool OnTouchEvent(MoveEvent* moveEvent);
    virtual bool DispatchTouchEvent(MoveEvent* moveEvent);
#endif
    void SetBackgroundColor(dkColor color)
    {
        m_backgroundColor = color;
    }
    dkColor GetBackgroundColor() const
    {
        return m_backgroundColor;
    }

    bool IsChildOf(UIContainer* container) const;
    
protected:
    UIWindow();
    UIWindow(UIContainer* pParent, const DWORD rstrId);
    virtual HRESULT DrawFocus(DK_IMAGE drawingImg);
    virtual void OnResize(int iWidth, int iHeight);
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
    virtual void OnMenuChanged();
    void Dispose();
    void Init();
    virtual HRESULT UpdateFocus();
    HRESULT GetParentDC(DK_IMAGE &rImgParent, DK_RECT &rRectParent);
    void UpdateToScreen(DK_RECT rectParent, paintflag_t paintFlag = PAINT_FLAG_NONE, bool drawImmediately = false);
    void DrawBackgroundWithRoundBorder(DK_IMAGE img);

private:
    int GetUITimerTaskIndex(int iId);
    void CancelTimer(int iIndex);
    void ReleaseTimers();
    void OnDispose(BOOL bDisposed);

    ////////////////////Method Section/////////////////////////
protected:
    BOOL m_IsChanged;
    UIContainer* m_pParent;
    BOOL m_bIsFocus;
    BOOL m_bIsVisible;
    bool m_isDisable;
    int m_iTop;
    int m_iLeft;
    int m_iWidth;
    int m_iHeight;
    BOOL m_bIsTabStop;
    int m_iTabIndex;

    IEventHook* m_pEventHook;

    std::vector<UITimerTask*> m_timers;
    std::vector<UITimerTask*> m_cancelTimers;

#ifdef KINDLE_FOR_TOUCH
    bool m_TouchBlocked;
#endif
    int m_iLeftMargin;
    int m_iTopMargin;
    // ′|àíòì2????￠?ìó|

    //! tracks auto event connections we make.
    BOOL m_bIsContain;
    UISizer *m_windowSizer;
    UISizer *m_containingSizer;
    int m_minWidth;
    int m_minHeight;
    int m_maxWidth;
    int m_maxHeight;

private:
    paintflag_t m_updateWindowFlag;
    int m_zPosition;
    BOOL m_bIsDisposed;
    int m_iTimerId;
    DWORD  m_dwId;
    std::string m_bgImage;
    CString m_strText;
#ifndef KINDLE_FOR_TOUCH
    Delegates *_OnDispose;
#endif
    BOOL m_autoLayout;
public:
#ifndef KINDLE_FOR_TOUCH
    Delegates Event_OnClick;
    Delegates Event_OnShow;
    Delegates Event_OnHide;
    Delegates& Event_OnDispose;
#endif
    void MakeCenter(int width, int height);
    void MakeCenterAboveIME(int width, int height);
#ifdef KINDLE_FOR_TOUCH
    virtual bool OnHookTouch(MoveEvent* moveEvent);
    void SetLongClickable(bool longClickable)
    {
        if (longClickable)
        {
            m_windowFlags |= WF_LONG_CLICKABLE;
        }
        else
        {
            m_windowFlags &= ~WF_LONG_CLICKABLE;
        }
    }
    void SetClickable(bool clickable)
    {
        if (clickable)
        {
            m_windowFlags |= WF_CLICKABLE;
        }
        else
        {
            m_windowFlags &= ~WF_CLICKABLE;
        }
    }
    bool IsClickable() const
    {
        return m_windowFlags & WF_CLICKABLE;
    }
    bool IsLongClickable() const
    {
        return m_windowFlags & WF_LONG_CLICKABLE;
    }
    void HookTouch();
    void UnhookTouch();
    void SetUpdateOnPress(bool update);
    bool IsUpdateOnPress() const;
    void SetUpdateOnUnPress(bool update);
    bool IsUpdateOnUnPress() const;
    void SetTouchSlot(int toushSlot);
    int GetTouchSlot() const;
protected:
    void DrawBackgroundEx(
            DK_IMAGE drawingImg,
            dkColor backgroundColor,
            bool showBorder,
            int borderLineWidth,
            bool upLeftRound,
            bool upRightRound,
            bool downRightRound,
            bool downLeftRound);
    void TryHideIME(MoveEvent* event);
    bool ShouldPropogateTouch() const
    {
        return m_windowFlags & WF_PROPAGATE_TOUCH;
    }
    void SetPropogateTouch(bool propogate)
    {
        if (propogate)
        {
            m_windowFlags |= WF_PROPAGATE_TOUCH;
        }
        else
        {
            m_windowFlags &= ~WF_PROPAGATE_TOUCH;
        }
    }

    enum WindowFlags
    {
        WF_CLICKABLE = 0x01,
        WF_LONG_CLICKABLE = 0x02,
        WF_PROPAGATE_TOUCH = 0x04,
        WF_UPDATE_ON_PRESS = 0x08,
        WF_UPDATE_ON_UNPRESS = 0x10,
        WF_DEFAULT = WF_PROPAGATE_TOUCH

    };
    uint32_t m_windowFlags;
    enum PrivateFlags
    {
        PF_PRESSED = 0x01,
        PF_PREPRESSED = 0x02

    };
    void SetPressed(bool pressed);
    bool IsPressed();

    uint32_t m_privateFlags;
    int m_touchSlot;
    bool m_hasPerformLongPress;

    DECLARE_TIMER_THREAD_FUNC(CheckForLongClick);
    DECLARE_TIMER_THREAD_FUNC(UnPress);
    virtual void PerformLongClick();
    virtual void PerformClick(MoveEvent* moveEvent);
    int m_touchDownX, m_touchDownY;
    bool m_outClickSlot;

private:
    MoveEvent m_lastUpEvent;
    MoveEvent m_lastDownEvent;
    bool m_hookingTouch;
#endif
private:
    dkColor m_backgroundColor;
};
#endif

