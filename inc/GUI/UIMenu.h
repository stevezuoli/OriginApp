////////////////////////////////////////////////////////////////////////
// UIMenu.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UIMENU_H__
#define __UIMENU_H__

#include <vector>
#include "GUI/UIDialog.h"
#include "GUI/MenuItem.h"
#include "DkSPtr.h"

#include "Public/Base/TPComBase.h"

using namespace DkFormat;

typedef enum MenuStyle
{
    Vertical
    ,Horizontal
}UIMenuStyle;


class UIPage;

class UIMenu
    : public UIDialog
{
public:
    virtual LPCSTR GetClassName() const
    {
        return ("UIMenu");
    }

    UIMenu(UIMenuStyle style);
    UIMenu(UIWindow* pEventHandler, UIMenuStyle style=Vertical);
    UIMenu(UIMenu& rMenu);

    virtual ~UIMenu();

    virtual BOOL OnKeyPress(INT32 iKeyCode);
    virtual BOOL OnPointerPressed(INT32 iX, INT32 iY);
    virtual BOOL OnPointerDragged(INT32 iX, INT32 iY);
    virtual BOOL OnPointerReleased(INT32 iX, INT32 iY);
    virtual void OnSelect();

    virtual BOOL IsExclusive();
    virtual void SetExclusive(BOOL bIsExclusive);

    virtual UIWindow* GetEventHandler();
    virtual void SetEventHandler(UIWindow* pEventHandler);

    virtual void InsertMenu(UIMenu* pMenu, INT32 iPos);
    virtual void AppendMenu(DWORD dwMenuId, INT32 iShortKey, const CString& rstrLabel, INT32 iImageID, char cHotKey, BOOL bIsActive=TRUE,INT32 SelectIcon =-1);
    virtual void AppendStaticMenu(const CString& rstrLabel);
    virtual void AppendSeparator();
    virtual void DestroySubMenu();
    virtual void MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight);
    virtual void SetUIPage(UIPage* pUIPage);
    virtual BOOL IsCached();
    virtual void SetUpLevelMenu(UIMenu* pMenu);

    virtual MenuItem* GetItemByID(INT32 iItemId);
    virtual MenuItem* GetItem(INT32 iItem);
    virtual void SetItemActive(INT32 iItemId, BOOL bActive);
    virtual void SetItemLabel(INT32 iItemId, const CString& rstrLabel);
    virtual void SetItemSelectIcon(INT32 iItemId, INT32 iSelectImage);
    virtual INT32 GetItemHeight(INT32 iItem);
    virtual INT32 GetItemCount();
    virtual BOOL ContainsItemId(DWORD dwId);
    virtual INT32 GetContentHeight();

    virtual const CString& GetMenuName();
    virtual void SetMenuName(const CString& rstrName);
    virtual void Repaint();

    virtual void PopupMenu(INT32 iX, INT32 iY, INT32 iScreenWidth, INT32 iScreenHeight);
    virtual void Destroy();

    virtual void SetStyle(UIMenuStyle style);
    virtual UIMenuStyle GetStyle();

    virtual HRESULT Draw(DK_IMAGE drawingImg);

    virtual void ClearAllItem();
    virtual void SetItemVisible(INT32 iItemId, BOOL bVisible);
    virtual void SetSelectItem(INT32 iItemId);

    virtual BOOL SetVisible(BOOL bVisible);
    virtual void Show(BOOL bIsShow, BOOL bIsRepaint = TRUE);

    virtual void OnNotify(UIEvent rEvent);
    void Dispose();
    void Init();
    virtual void OnMenuChanged();
    INT32 GetMenuItemSelectLine(int &x,int &y,int &w,int &h,int indexItem);


    void OnDispose(BOOL bIsDisposed);
    void PrePopup();
    INT32 Point2Item(INT32 iX, INT32 iY);
    void PopSubMenu();
    void PopSubMenu(MenuItem* pItem);
    void DrawBackground(DK_IMAGE drawingImg);
    INT32 DrawMenuItem(DK_IMAGE drawingImg, MenuItem* pItem, INT32 iItemIndex, INT32 iMenuItemX, INT32 iMenuItemY);
    void DrawMenuItemSeparator(DK_IMAGE drawingImg, INT32 iMenuItemX, INT32 iMenuItemY, INT32 iItemWidth,
            INT32 iItemHeight);
    void DrawMenuItemBack(DK_IMAGE drawingImg, INT32 iMenuItemX, INT32 iMenuItemY, INT32 iItemWidth,
            INT32 iItemHeight, BOOL bIsSelected);
    void DrawSubmenuArrow(DK_IMAGE drawingImg, INT32 iMenuItemX, INT32 iMenuItemY, INT32 iItemWidth,
            INT32 iItemHeight);
    void ClickMenuItem();
    ////////////////////Method Section/////////////////////////

    ////////////////////Field Section/////////////////////////

    BOOL DrawSelectItem(INT32 iOldItem);
    INT32 GetMenuItemHeight(MenuItem* pItem);
    void SetMenuType(INT32 type);

    DWORD GetSelectedMenuId();
    INT32 GetSelectedMenuItem();
    void  SetSelectedMenuItem(INT32 _item);
    
    INT32 GetScreenWidth();
    void  SetScreenWidth(INT32 _value);
    INT32 GetScreenHeight();
    void  SetScreenHeight(INT32 _value);

    void ShowSelectedIcon(bool showIcon);
    bool IsShowSelectedIcon();

    int GetLeftMarginOfTitle();

protected:
    UIWindow* m_pEventHandler;

    void SetModified();
    void ClearModified();
    BOOL IsModified();

    SPtr<ITpImage> m_spBackImgCache;
    std::vector<MenuItem*> m_menuItems;

    ITpFont* m_pFont;
    DKFontAttributes m_ifontAttr;

    UIPage* m_pUIPage;

    CString m_strName;

    BOOL m_bIsExclusive;


    UIMenuStyle  m_style;

    UIMenu* m_pLeftMenu;
    UIMenu* m_pRightMenu;
    UIMenu* m_pUpLevelMenu;
    UIMenu* m_pSubMenu;
    BOOL m_bIsDisposed;
    BOOL m_bModified;
    INT32 m_iMenuType;
    void Update(BOOL bForce = FALSE/* if bForce, even invisble we'll MoveWindow */);
    bool m_showSelectedIcon;
    bool m_showHotKey;

private:
    INT32 m_iSelectedMenuItem;
    INT32 m_iScreenWidth;
    INT32 m_iScreenHeight;
    ////////////////////Field Section/////////////////////////
};

#endif
