////////////////////////////////////////////////////////////////////////
// UIPage.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UIPage_H__
#define __UIPage_H__

#include "GUI/UIWindow.h"
#include "GUI/UIContainer.h"
#include "GUI/UIEvent.h"
#include "GUI/IPage.h"
#include "GUI/ITpFont.h"   
#include "CommonUI/UITitleBar.h"
#ifdef KINDLE_FOR_TOUCH	  	
#include "TouchAppUI/UIBottomBar.h"
#endif

class UIPage : public UIContainer
{
public:
    UIPage();
    UIPage(const DWORD dwId);
    UIPage(const DWORD dwId, BOOL bIsScrollable);

    virtual LPCSTR GetClassName() const
    {
        return "UIPage";
    }

    virtual ~UIPage();

    virtual BOOL OnKeyPress(INT32 iKeyCode);
    virtual BOOL OnKeyRelease(INT32 iKeyCode);

    virtual BOOL OnPointerPressed(INT32 iX, INT32 iY);
    virtual BOOL OnPointerDragged(INT32 iX, INT32 iY);
    virtual BOOL OnPointerReleased(INT32 iX, INT32 iY);
    
    virtual void OnChildSetFocus(UIWindow* pWindow);
    virtual void OnNotify(UIEvent rEvent);

    virtual UIDialog* GetDialog();
    virtual void SetLayout(UILayout* pLayout);

    virtual void PopupMenu(UIMenu* pMenu, INT32 iX, INT32 iY);
    virtual UIMenu* GetMenu();

    virtual INT32 GetClientWidth();
    virtual INT32 GetClientHeight();

    virtual UIPage* GetUIPage();

    virtual HRESULT Draw(DK_IMAGE drawingImg);
    virtual void MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight);

    // 当usb挂盘结束时，会调用到此方法，在这里加载需要的资源
    virtual void OnLoad() ;

    // 当usb挂盘时，会调用到此方法，在这里释放影响挂盘的资源
    virtual void OnUnLoad() ;

    // 当进入一个页面时，调用此方法
    virtual void OnEnter() {};
    
    // 当离开一个页面时，调用此方法
    virtual void OnLeave() {};  

//***************************************************************************************************************//
    virtual void Repaint();
    INT32 GetScreenHeight();
    INT32 GetScreenWidth();
    virtual UITitleBar *GetTitleBar();
    void SetScreenOrientation(INT32 iScreenOrientation);
    void PopupDialog(UIDialog* pDialog);
    void Broadcast(UIEvent rEvent);

    void DestroyMenu();
    BOOL DestroyDialog(BOOL fRepaintIt = TRUE);

    
protected:
    virtual void OnResize(INT32 iWidth, INT32 iHeight);
    virtual HRESULT DrawFocus(DK_IMAGE drawingImg);
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
    void Init();
    void Dispose();

private:
    void OnDispose(BOOL bDisposed);

protected:
    UIDialog* m_pDialog;
    UIMenu *m_pCurrentMenu;
    UIMenu *m_pMainMenu;
    UITitleBar  m_titleBar;
#ifdef KINDLE_FOR_TOUCH	 
    UIBottomBar* m_pBottomBar; 	
#endif
    INT32 m_iScreenOrientation;

private:
    BOOL m_bIsScrollable;
    BOOL m_bIsDisposed;
};

#endif

