////////////////////////////////////////////////////////////////////////
// MenuItem.cpp
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "GUI/MenuItem.h"
#include "GUI/UIMenu.h"
#include "interface.h"

MenuItem::MenuItem(

    DWORD dwMenuId,
    LPCSTR szMenuLabel,
    char chHotKey,
    INT32 iShortcutKey,
    BOOL bIsActive,
    UIMenu* pSubMenu,
    INT32 iImageId,
    INT32 iSelectIcon
)
    : m_dwMenuId(dwMenuId)
    , m_strMenuLabel(szMenuLabel)
    , m_chHotKey(chHotKey)
    , m_iShortcutKey(iShortcutKey)
    , m_bIsActive(bIsActive)
    , m_pSubMenu(pSubMenu)
    , m_iImageId(iImageId)
    ,m_iSelectIconId(iSelectIcon)
    ,m_bIsVisible(TRUE)
{

}

MenuItem::~MenuItem()
{
    if (m_pSubMenu)
    {
        m_pSubMenu->Destroy();
        m_pSubMenu = NULL;
    }
}

DWORD MenuItem::GetMenuId() const
{
    return m_dwMenuId;
}

void MenuItem::SetMenuLabel(const CString& rstrMenuLabel)
{
    m_strMenuLabel = rstrMenuLabel;
}

LPCSTR MenuItem::GetMenuLabel() const
{
    return m_strMenuLabel;
}

void MenuItem::SetItemSelectImage(INT32 iImgID)
{
    m_iSelectIconId = iImgID;
}

INT32 MenuItem::GetItemSelectImage()
{
    return m_iSelectIconId;
}


UIMenu* MenuItem::GetSubMenu()
{
    return m_pSubMenu;
}

void MenuItem::SetSubMenu(UIMenu* pSubMenu)
{
    m_pSubMenu = pSubMenu;
}

void MenuItem::Dispose()
{
    if (m_pSubMenu)
    {
        m_pSubMenu->Destroy();
    }
}

BOOL MenuItem::IsActive()
{
    return m_bIsActive;
}

void MenuItem::SetActive(BOOL bActive)
{
    m_bIsActive = bActive;
}

void MenuItem::SetShortcutKey(INT32 iShortKey)
{
    m_iShortcutKey = iShortKey;
}

INT32 MenuItem::GetShortcutKey() const
{
    return m_iShortcutKey;
}

void MenuItem::SetItemLeftImage(INT32 iImgID)
{
    m_iImageId = iImgID;
}

INT32 MenuItem::GetItemLeftImage() const
{
    return m_iImageId;
}

void MenuItem::SetHotKey(char chkey)
{
    m_chHotKey = chkey;
}

char  MenuItem::GetHotKey() const
{
    return m_chHotKey;
}

BOOL MenuItem::IsStatic()
{
    return IDSTATIC==m_dwMenuId;
}

BOOL MenuItem::IsSeparator()
{
    return IDSEPARATOR==m_dwMenuId;
}

