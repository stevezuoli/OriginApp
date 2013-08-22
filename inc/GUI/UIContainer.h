////////////////////////////////////////////////////////////////////////
// UIContainer.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UICONTAINER_H__
#define __UICONTAINER_H__

#include <vector>
#include "GUI/UIWindow.h"
#include "GUI/UILayout.h"

#include "GUI/IGUI.h"
#include "Public/Base/TPComBase.h"
#include "GUI/IEventHook.h"
#include "Framework/INativeMessageQueue.h"

class UIMenu;

class UIContainer : public UIWindow
{
public:
    UIContainer();
    UIContainer(UIContainer* pParent, DWORD dwId, BOOL bBubbleNotify);
    UIContainer(UIContainer* pParent, DWORD dwId=IDSTATIC);

    virtual LPCSTR GetClassName() const
    {
        return ("UIContainer");
    }

    virtual ~UIContainer();

    virtual void OnNotify(UIEvent rEvent);
    virtual BOOL OnKeyRelease(INT32 iKeyCode);
    virtual BOOL OnKeyPress(INT32 iKeyCode);
    virtual void OnChildSetFocus(UIWindow* pWindow);
    virtual void OnChildMenuChanged(UIWindow* pWindow);

    /// <summary>
    /// Responses to the event sent from its parent.
    /// This class only boradcasts the event to all the child container.
    /// </summary>
    virtual void OnBroadcast(UIEvent rEvent);

    virtual void ClearFocus();
    virtual void SetFocus(BOOL bIsFocus);
    virtual void SetReverseFocus(BOOL bIsFocus);
    virtual INT32 GetPreFocusIndex(INT32 iFocus);
    virtual INT32 GetNextFocusIndex(INT32 iFocus);

    virtual INT32 ClientToScreenY(INT32 iY);
    void SetLayout(UILayout* pLayout);
    virtual UILayout* GetLayout();
    virtual void SetRelayNotify(BOOL bIsRelay);

    virtual HRESULT Draw(DK_IMAGE drawingImg);

    void AppendChild(UIWindow* pWindow);
    bool RemoveChild(UIWindow* pWindow, bool doDelete);
    bool RemoveChildByIndex(size_t index, bool doDelete);
    size_t GetChildrenCount() const;
    UIWindow* GetChildByIndex(size_t iIndex) const;
#ifdef KINDLE_FOR_TOUCH
    virtual UIWindow* GetChildByPoint(int _x, int _y);
    virtual bool DispatchTouchEvent(MoveEvent* moveEvent);
    virtual bool OnInterceptionTouchEvent(MoveEvent* moveEvent);
#endif
    virtual BOOL IsShortkey(INT32 iKeyCode);
    virtual HRESULT MoveFocus(MoveDirection dir, BOOL bLoop=false);
    virtual void OnWifiMessage(SNativeMessage event){};

    virtual void OnTTSMessage(SNativeMessage* msg){};

    virtual void OnSerializedBookUpdate(SNativeMessage event){};
    virtual void OnFailedToConnectServer(SNativeMessage event){};
    // zhangjingdan add for pdf dlg.
    // iLineGap 同一行内允许y坐标的差距.
    HRESULT MoveFocusWithStructTable(int iKey, int iLineGap = 10);

	// move from UIPage for UIDialog
	virtual void UpdateSectionUI(bool bIsFull = false);
    //virtual bool Layout();
    virtual dkSize GetMinSize() const;
    virtual void DoSetSize(int x, int y, int width, int height, int sizerFlags = dkSIZE_AUTO);
    virtual bool OnChildClick(UIWindow* child)
    {
        if (NULL != GetParent())
        {
            return GetParent()->OnChildClick(child);
        }
        return false;
    }
    virtual bool OnChildLongPress(UIWindow* child)
    {
        if (NULL != GetParent())
        {
            return GetParent()->OnChildLongPress(child);
        }
        return false;
    }
    virtual UIContainer* GetTopFullScreenContainer();

    BOOL NextFocus(BOOL bIsReverse);
protected:
    INT32 GetChildIndex(UIWindow* pWindow);
    virtual void OnResize(INT32 iWidth, INT32 iHeight);
    virtual BOOL SetChildWindowFocus(INT32 iNewFocus, BOOL bIsReverse);
    virtual void SetAppropriateFocus(BOOL bIsFocus, BOOL bIsReverse);
    virtual HRESULT DrawFocus(DK_IMAGE drawingImg);
    virtual void OnMenuChanged();
    void ClearAllChild(bool doDelete);
    void Init();
    void Dispose();
    BOOL SendKeyToChildren(INT32 iKeyCode);
    BOOL SendHotKeyToChildren(INT32 iKeyCode);

private: // zhangjingdan add.
    typedef std::vector< UIWindow* > VectorLine;
    typedef std::vector< VectorLine* > VectorTable;
    void PushWinToTable(VectorTable& table, UIWindow*pWin, int iLineGap);
    void PushWinToLine(VectorLine& line, UIWindow*pWin);
    BOOL StructTable(VectorTable& table, int iLineGap);
    static bool CompareZPosition(UIWindow* pWnd1, UIWindow* pWnd2);
private:
    INT32 GetPrevChild(INT32 iIdx);
    INT32 GetNextChild(INT32 iIdx);
    INT32 GetAppropriateFocus();
    void OnDispose(BOOL bDisposed);
    ////////////////////Method Section/////////////////////////

    ////////////////////Field Section/////////////////////////
protected:
    BOOL m_bIsBubbleNotify;
    INT32 m_iFocusedChild;
    UILayout* m_pLayout;

    INT32 m_iFocusedChildInMenu;

private:
    BOOL m_bIsDisposed;
    ////////////////////Field Section/////////////////////////
protected:
    struct TouchTarget
    {
        TouchTarget(UIWindow* child_, int pointerId_);
        ~TouchTarget();
        UIWindow* child;
        int pointerId;
        TouchTarget* next;
    };
    bool DispatchTransformedTouchEvent(
            MoveEvent* event,
            bool cancel,
            UIWindow* child, 
            int desiredPointerId);

    void CancelAndClearTouchTargets(MoveEvent* event); 
    void ClearTouchTargets();
    TouchTarget* m_firstTouchTarget;
    bool CanWindowReceiveTouchEvents(UIWindow* child) const;
    bool IsTransformedTouchPointInWindow(int x, int y, UIWindow* child) const;
    TouchTarget* GetTouchTarget(UIWindow* child) const;
    TouchTarget* AddTouchTarget(UIWindow* child, int pointerId);
    void RemoveTouchTarget(UIWindow* child);
         
private:
    std::vector<UIWindow*> m_childWins;
};
#endif
