////////////////////////////////////////////////////////////////////////
// MenuItem.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __MENUITEM_H__
#define __MENUITEM_H__

#include "GUI/UIWindow.h"

class MenuItem
{
    ////////////////////Constructor Section/////////////////////////
public:
    MenuItem(
        DWORD dwMenuId,
        LPCSTR szMenuLabel,
        char chHotKey=0,
        INT32 iShortcutKey=0,
        BOOL bIsActive=TRUE,
        UIMenu* pSubMenu=NULL,
        INT32 iImageId=IMAGE_BUTTON_HEAD_24,
        INT32 iSelectIcon=-1
    );

    virtual ~MenuItem();
    ////////////////////Constructor Section/////////////////////////

    ////////////////////Method Section/////////////////////////
public:
    BOOL IsSeparator();
    BOOL IsStatic();

    DWORD GetMenuId() const;

    void SetShortcutKey(INT32 iShortKey);
    INT32 GetShortcutKey() const;

    void SetMenuLabel(const CString& rstrMenuLabel);
    LPCSTR GetMenuLabel() const;

    UIMenu* GetSubMenu();
    void SetSubMenu(UIMenu* pSubMenu);

    void Dispose();

    BOOL IsActive();
    void SetActive(BOOL bActive);

    void SetItemLeftImage(INT32 iImgID);
    INT32 GetItemLeftImage() const;

    void SetItemSelectImage(INT32 iImgID);
   INT32 GetItemSelectImage();


    void SetHotKey(char);
    char GetHotKey() const;

    BOOL IsVisible()
    {
        return m_bIsVisible;
    }

    BOOL SetVisible(BOOL bVisible)
    {
        return m_bIsVisible = bVisible;
    }

private:
    DWORD m_dwMenuId;
    CString m_strMenuLabel;

    char m_chHotKey;
    INT32 m_iShortcutKey;

    BOOL m_bIsActive;

    UIMenu* m_pSubMenu;
    INT32 m_iImageId;
    INT32 m_iSelectIconId;
    BOOL m_bIsVisible;
};

#endif

