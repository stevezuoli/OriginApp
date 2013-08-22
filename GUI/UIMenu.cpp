////////////////////////////////////////////////////////////////////////
// UIMenu.cpp
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "GUI/UIMenu.h"
#include "GUI/UIPage.h"
#include "Utility/ImageManager.h"
#include "GUI/FontManager.h"
#include "Utility/ColorManager.h"
#include "Framework/CDisplay.h"
#include "GUI/CTpGraphics.h"
#include "interface.h"
#include "drivers/DeviceInfo.h"

#define MENU_ID_CANCEL (101)
#define MENU_ID_SELECT (100)

#define COLOR_HILIGHT (0xFFFFFF)
#define COLOR_OUTBORDER (0xAAAAAA)
#define COLOR_BACKGROUND (0xEAEAEA)
#define COLOR_SHADOW (0x888888)
#define COLOR_BORDER (0xAAAAAA)
#define COLOR_HILIGHT_BACK (0x5555FF)

//MENU_ITEM_HEIGHT
#define MENU_IMAGE_HEIGHT (23)
#define MENU_ROW_HEIGHT (18)
#define MENU_ITEM_HEIGHT  (MENU_IMAGE_HEIGHT + MENU_ROW_HEIGHT)

#define MENU_SEPARATOR_HEIGHT (10)
#define MARGIN (24)
#define MARGIN_TXT_ITEM (5)
#define MENU_ITEM_SPACE (2)
#define ITEM_KEY_X (6)
#define ITEM_KEY_Y (2)
#define MENU_ITEM_IMAGE_TO_TEXT  (12)
#define MENU_TOP (30)
#define MENU_RIGHT (580)
#define MENU_HOTKEY_ICON_SIZE (24)

#define MENU_SOLID_LINE_HEIGHT (5)
#define MENU_DOT_LINE_HEIGHT (1)
#define MENU_ICON_WIDTH (12)

UIMenu::UIMenu(UIMenuStyle style)
    : m_pEventHandler(NULL)
    , m_spBackImgCache()
    , m_menuItems()
    , m_pFont(NULL)
    , m_ifontAttr()
    , m_pUIPage(NULL)
    , m_strName("")
    , m_bIsExclusive(FALSE)
    , m_style()
    , m_pLeftMenu(NULL)
    , m_pRightMenu(NULL)
    , m_pUpLevelMenu(NULL)
    , m_pSubMenu(NULL)
    , m_bIsDisposed(FALSE)
    , m_bModified(FALSE)
    , m_iMenuType(0)
    , m_iSelectedMenuItem(0)
    , m_iScreenWidth(0)
    , m_iScreenHeight(0)
{
    Init();
    m_style = style;
}

UIMenu::UIMenu(UIWindow* pEventHandler,UIMenuStyle style)
    : m_pEventHandler(pEventHandler)
    , m_spBackImgCache()
    , m_menuItems()
    , m_pFont(NULL)
    , m_ifontAttr()
    , m_pUIPage(NULL)
    , m_strName("")
    , m_bIsExclusive(FALSE)
    , m_style(style)
    , m_pLeftMenu(NULL)
    , m_pRightMenu(NULL)
    , m_pUpLevelMenu(NULL)
    , m_pSubMenu(NULL)
    , m_bIsDisposed(FALSE)
    , m_bModified(FALSE)
    , m_iMenuType(0)
    , m_iSelectedMenuItem(0)
    , m_iScreenWidth(0)
    , m_iScreenHeight(0)
{
    Init();
    m_style = style;
}

UIMenu::UIMenu(UIMenu& rMenu)
    : m_pEventHandler(NULL)
    , m_spBackImgCache()
    , m_menuItems()
    , m_pFont(NULL)
    , m_ifontAttr()
    , m_pUIPage(NULL)
    , m_strName("")
    , m_bIsExclusive(FALSE)
    , m_style()
    , m_pLeftMenu(NULL)
    , m_pRightMenu(NULL)
    , m_pUpLevelMenu(NULL)
    , m_pSubMenu(NULL)
    , m_bIsDisposed(FALSE)
    , m_bModified(FALSE)
    , m_iMenuType(0)
    , m_iSelectedMenuItem(0)
    , m_iScreenWidth(0)
    , m_iScreenHeight(0)
{
    Init();
    m_pEventHandler = rMenu.m_pEventHandler;
    m_spBackImgCache = rMenu.m_spBackImgCache;
    m_bIsExclusive = rMenu.m_bIsExclusive;
    m_pFont = rMenu.m_pFont;

    // this two menus are shallow copied
    m_pUpLevelMenu = rMenu.m_pUpLevelMenu;
    m_pSubMenu = rMenu.m_pSubMenu;

    m_strName = rMenu.m_strName;

    m_pUIPage = rMenu.m_pUIPage;
    m_iScreenWidth = rMenu.m_iScreenWidth;
    m_iScreenHeight = rMenu.m_iScreenHeight;
    m_style = rMenu.m_style;
    // the left and right menu are both set to null
    m_pLeftMenu = NULL;
    m_pRightMenu = NULL;

    INT32 iSize = rMenu.m_menuItems.size();
    for (INT32 i = 0; i < iSize; i++)
    {
        // the newly created menu item will be released in the destructor
        MenuItem *pTmp =  new MenuItem(*rMenu.m_menuItems[i]);
        if(pTmp)
        {
            m_menuItems.push_back(pTmp);
        }
        else
        {
            break;
        }
    }

    m_iSelectedMenuItem = rMenu.m_iSelectedMenuItem;
}

UIMenu::~UIMenu()
{
    Dispose();
}

void UIMenu::Dispose()
{
    UIWindow::Dispose();
    OnDispose(m_bIsDisposed);
}

void UIMenu::OnDispose(BOOL bIsDisposed)
{
    //UIWindow::OnDispose(bDisposed);

    if (bIsDisposed)
    {
        return;
    }

    m_bIsDisposed = TRUE;
    INT32 iSize = m_menuItems.size();
    for (INT32 i = 0; i < iSize; i++)
    {
        MenuItem* pItem = m_menuItems[i];
        if(!pItem)
        {
            continue;
        }
        pItem->Dispose();
        delete pItem;
    }
    m_menuItems.clear();

    // Note, though left menu and right menu variables are defined in UIWindow
    // but these two variables are created in this class, so they are deleted
    // in this class too.
    if (m_pLeftMenu)
    {
        delete m_pLeftMenu;
        m_pLeftMenu = NULL;
    }

    if (m_pRightMenu)
    {
        delete m_pRightMenu;
        m_pRightMenu = NULL;
    }

    if (m_pUpLevelMenu)
    {
        //m_pUpLevelMenu->Dispose();
        m_pUpLevelMenu = NULL;
    }

    m_pEventHandler = NULL;

    if (m_pUIPage)
    {
        m_pUIPage->DestroyMenu();
        m_pUIPage = NULL;
    }

    if (m_pSubMenu)
    {
        //m_pSubMenu->Dispose();
        m_pSubMenu = NULL;
    }

    m_iSelectedMenuItem = -1;

}


void UIMenu::SetStyle(UIMenuStyle style)
{
    m_style = style;
    return;
}

UIMenuStyle UIMenu::GetStyle()
{
    return m_style;
}

void UIMenu::Init()
{
    m_bIsDisposed = FALSE;
    m_bIsExclusive = FALSE;
    m_bModified = FALSE;
    m_pFont = NULL;
    m_iSelectedMenuItem = 0;
    m_pUpLevelMenu = NULL;
    m_pSubMenu = NULL;
    m_pUIPage = NULL;
    m_iScreenWidth = 0;
    m_iScreenHeight = 0;
    m_pLeftMenu = NULL;
    m_pRightMenu = NULL;
    m_style = Vertical;
    m_ifontAttr.SetFontAttributes(0, 0, 18);
    m_pFont = FontManager::GetInstance()->GetFont(FONT_MENU);
    m_showSelectedIcon = false;
    m_showHotKey = false;
}

void UIMenu::SetModified()
{
    m_bModified = TRUE;
}

void UIMenu::ClearModified()
{
    m_bModified = FALSE;
}

BOOL UIMenu::IsModified()
{
    return m_bModified;
}

INT32 UIMenu::GetMenuItemHeight(MenuItem* pItem)
{
    INT32 iItemHeight = 0;
    if (pItem->IsSeparator())
    {
        iItemHeight = MENU_SEPARATOR_HEIGHT + MENU_ITEM_SPACE;
    }
    else if (pItem->IsStatic())
    {
        iItemHeight = MENU_ITEM_HEIGHT;
    }
    else
    {
        iItemHeight = MENU_ITEM_HEIGHT;
    }

    return iItemHeight;
}

BOOL UIMenu::DrawSelectItem(INT32 iOldItem)
{

    if(iOldItem != m_iSelectedMenuItem)
    {
        CDisplay* display = CDisplay::GetDisplay();
        DK_IMAGE pGraphics = display->GetMemDC();
        CTpGraphics prg(pGraphics);
        int x = 0;
        int y = 0;
        int w = 0;
        int h = 0;

        GetMenuItemSelectLine(x,y,w,h,iOldItem);
        prg.FillRect(m_iLeft + x, m_iTop + y, m_iLeft + x + w + 2, m_iTop + y + h + 1, 0x00000000);
        prg.DrawLine(x + m_iLeft, y + m_iTop + MENU_SOLID_LINE_HEIGHT / 2 -MENU_DOT_LINE_HEIGHT,  w - 1, MENU_DOT_LINE_HEIGHT,DOTTED_STROKE);
        
        display->ScreenRepaint(x + m_iLeft,y + m_iTop,w,h);
        x = y = w = h = 0;
        //display->ScreenRepaint(m_iLeft, m_iTop, m_iWidth, m_iHeight, m_iLeft, m_iTop);
        GetMenuItemSelectLine(x,y,w,h,m_iSelectedMenuItem);
        prg.DrawLine(x + m_iLeft,y + m_iTop, w, MENU_SOLID_LINE_HEIGHT,SOLID_STROKE);

        //TODO::
        //Repaint();
        display->ScreenRepaint(x + m_iLeft,y + m_iTop,w,h);
        return FALSE;
    }
    return TRUE;
}

BOOL UIMenu::OnKeyPress(INT32 iKeyCode)
{
    if (m_pSubMenu)
    {
        return m_pSubMenu->OnKeyPress(iKeyCode);
    }

    // number shotcut key support
    DebugPrintf(DLC_INPUT, "Menu::OnKeyPress(INT32 iKeyCode) = %d %d %d", iKeyCode, KEY_UP, KEY_DOWN);

    INT32 iOldItem = m_iSelectedMenuItem;

    if (m_showHotKey && IsAlpha(iKeyCode))
    {
        char cKey = VirtualKeyToChar(iKeyCode);
        DebugPrintf(DLC_INPUT, "Menu::OnKeyPress(INT32 iKeyCode)  KEY PRESS");
        INT32 iSize = m_menuItems.size();
        for (INT32 i = 0; i < iSize; i++)
        {
            MenuItem* pItem = m_menuItems[i];

            if (!pItem || pItem->IsSeparator() || pItem->IsStatic() || !pItem->IsActive())
            {
                continue;
            }

            if (pItem->GetHotKey() == cKey || pItem->GetHotKey() == cKey - 32)
            {
                m_iSelectedMenuItem = i;
                Show(FALSE);
                ClickMenuItem();
                return FALSE;
            }
        }
        DebugPrintf(DLC_INPUT, "Menu::OnKeyPress(INT32 iKeyCode) end");
    }
    else
    {
        switch (iKeyCode)
        {
        case KEY_UP:
            //TODO jzn only one or none or one Separator item;
            if(Vertical == m_style)
            {
                do
                {
                    if (--m_iSelectedMenuItem < 0)
                    {
                        m_iSelectedMenuItem = m_menuItems.size() - 1;
                    }
                } while ((m_menuItems[m_iSelectedMenuItem])->IsSeparator() || (m_menuItems[m_iSelectedMenuItem])->IsStatic() || !(m_menuItems[m_iSelectedMenuItem])-> IsVisible() || !(m_menuItems[m_iSelectedMenuItem])-> IsActive());
                DebugPrintf(DLC_GUI, "iOldItem = %d m_iSelectedMenuItem = %d\n", iOldItem, m_iSelectedMenuItem);
                DrawSelectItem(iOldItem);
            }
            break;
        case KEY_DOWN:
            if(Vertical == m_style)
            {
                do
                {
                    INT32 iSize = m_menuItems.size();
                    if (++m_iSelectedMenuItem >= iSize)
                    {
                        m_iSelectedMenuItem = 0;
                    }
                } while ((m_menuItems[m_iSelectedMenuItem])->IsSeparator() || (m_menuItems[m_iSelectedMenuItem])->IsStatic() || !(m_menuItems[m_iSelectedMenuItem])-> IsVisible() || !(m_menuItems[m_iSelectedMenuItem])-> IsActive());
                DebugPrintf(DLC_GUI, "dItem = %d m_iSelectedMenuItem = %d\n", iOldItem, m_iSelectedMenuItem);
                DrawSelectItem(iOldItem);
            }
            break;
        case KEY_OKAY:
            Show(FALSE);
            ClickMenuItem();
            break;
        case KEY_BACK:
            Show(FALSE);
            break;
        case KEY_LEFT:
            if(Horizontal == m_style)
            {
                do
                {
                    if (--m_iSelectedMenuItem < 0)
                    {
                        m_iSelectedMenuItem = m_menuItems.size() - 1;
                    }
                }while ((m_menuItems[m_iSelectedMenuItem])->IsSeparator() || (m_menuItems[m_iSelectedMenuItem])->IsStatic());
                DebugPrintf(DLC_GUI, "iOldItem = %d m_iSelectedMenuItem = %d\n", iOldItem, m_iSelectedMenuItem);
                DrawSelectItem(iOldItem);
            }
            break;
        case KEY_RIGHT:
            if(Horizontal == m_style)
            {
                do
                {
                    INT32 iSize = m_menuItems.size();
                    if (++m_iSelectedMenuItem >= iSize)
                    {
                        m_iSelectedMenuItem = 0;
                    }
                } while ((m_menuItems[m_iSelectedMenuItem])->IsSeparator() || (m_menuItems[m_iSelectedMenuItem])->IsStatic());
                DebugPrintf(DLC_GUI, "dItem = %d m_iSelectedMenuItem = %d\n", iOldItem, m_iSelectedMenuItem);
                DrawSelectItem(iOldItem);
            }
            break;
        default:
            //UIPage::OnKeyPress(iKeyCode);
            if(UIContainer::IsShortkey(iKeyCode))
                return FALSE;

            return UIWindow::OnKeyPress(iKeyCode);
            break;
        }
    }
    return FALSE;
}

BOOL UIMenu::OnPointerPressed(INT32 iX, INT32 iY)
{
    // Note, this is not implemented in this milestone
    UNUSED(iX);
    UNUSED(iY);
    return TRUE;
}

BOOL UIMenu::OnPointerDragged(INT32 iX, INT32 iY)
{
    // Note, this is not implemented in this milestone
    UNUSED(iX);
    UNUSED(iY);
    return TRUE;
}

BOOL UIMenu::OnPointerReleased(INT32 iX, INT32 iY)
{
    // Note, this is not implemented in this milestone
    UNUSED(iX);
    UNUSED(iY);
    return TRUE;
}

BOOL UIMenu::IsExclusive()
{
    return m_bIsExclusive;
}

void UIMenu::InsertMenu(UIMenu* pMenu, INT32 iPos)
{
    TP_ASSERT(pMenu != NULL);

    if (-1 == iPos)
    {
        iPos = m_menuItems.size();
    }

    INT32 iSize = pMenu->m_menuItems.size();
    for (INT32 i = 0; i < iSize; i++)
    {
        // the newly created menu item will be released in the destructor
        MenuItem *pTmp = new MenuItem(*pMenu->m_menuItems[i]) ;
        if(pTmp)
        {
            m_menuItems.insert(m_menuItems.begin() + iPos + i, pTmp);
        }
        else
        {
            break;
        }
    }
}

void UIMenu::MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight)
{
    if(m_style == Vertical)
    {
        INT32 iMenuRight = iLeft + iWidth;
        INT32 iMenuTop = iTop;
        m_iLeft = iMenuRight - 2 * MARGIN;
        m_iTop = iMenuTop;
        m_iWidth = 2 * MARGIN;
        m_iHeight = 2 * MARGIN;
        INT32 iSize = m_menuItems.size();
        for (INT32 i = 0; i < iSize; i++)
        {
            MenuItem* pItem = m_menuItems[i];
            if(!pItem)
            {
                continue;
            }
            if(!(pItem->IsVisible()))
                continue;
            if (pItem->IsSeparator())
            {
                m_iHeight += MENU_SEPARATOR_HEIGHT;
            }
            else if (pItem->IsStatic())
            {
                m_iHeight += MENU_ITEM_HEIGHT;
                m_ifontAttr.SetFontAttributes(0, 0, 24);
                ITpFont *pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));

                INT32 nWidth = MARGIN + pFont->StringWidth(pItem->GetMenuLabel()) + MARGIN;
                m_iWidth = nWidth > m_iWidth?nWidth:m_iWidth;
                if(m_iWidth > 300)
                    m_iWidth = 300;
                m_iLeft = iMenuRight -m_iWidth;
            }
            else
            {
                INT32 iImgWidth = 24;
                INT32 iShortKeyWidth = 0;
                m_ifontAttr.SetFontAttributes(0, 0, 14);
                ITpFont *pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
                if(pItem->GetShortcutKey() & ALT_DOWN)
                    iShortKeyWidth = pFont->StringWidth("Alt+Sym") + MARGIN_TXT_ITEM;
                else if (pItem->GetShortcutKey() & SHIFT_DOWN)
                    iShortKeyWidth = pFont->StringWidth("Shift+Sym") + MARGIN_TXT_ITEM;

                m_ifontAttr.SetFontAttributes(0, 0, 18);
                pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));

                INT32 nWidth = GetLeftMarginOfTitle();
                const int textWidth = pFont->StringWidth(pItem->GetMenuLabel()) + 5;
                if(m_iMenuType == 0)
                {
                    if (m_showHotKey)
                    {
                        nWidth += (MARGIN << 1) + textWidth + iShortKeyWidth;
                    }
                    else
                    {
                        nWidth += (MARGIN << 1) + textWidth;
                    }
                }
                else if(m_iMenuType == ID_MNU_SORT)
                {
                    nWidth += (MARGIN << 1) + textWidth;
                }
                m_iWidth = nWidth > m_iWidth?nWidth:m_iWidth;
                if (m_iWidth > 300)
                    m_iWidth = 300;
                m_iLeft = iMenuRight - m_iWidth;
                m_iHeight += MENU_ITEM_HEIGHT;
            }
        }
        if(iSize > 0)
        {
            m_iHeight -= MENU_ROW_HEIGHT;
        }
    }
    else
    {
        //        TODO:坐标计算问题
        m_iLeft = iLeft;
        m_iTop = iTop;
        m_iHeight = 3 * MARGIN;
        m_iWidth = CDisplay::GetDisplay()->GetScreenWidth() - m_iLeft - 30;
    }
}

void UIMenu::SetExclusive(BOOL bIsExclusive)
{
    m_bIsExclusive = bIsExclusive;
}

UIWindow* UIMenu::GetEventHandler()
{
    return m_pEventHandler;
}


void UIMenu::AppendStaticMenu(const CString& rstrLabel)
{
    MenuItem* pMenuItem = new MenuItem(IDSTATIC, rstrLabel);
    if(pMenuItem)
    {
        m_menuItems.push_back(pMenuItem);
        SetModified();
        Update();
    }

}

void UIMenu::AppendMenu(DWORD dwMenuId, INT32 iShortKey, const CString& rstrLabel, INT32 iImageID, char cHotKey, BOOL bIsActive,INT32 SelectIcon)
{
    MenuItem* pMenuItem = new MenuItem(
        dwMenuId,
        rstrLabel,
        cHotKey,
        iShortKey,
        bIsActive,
        NULL,
        iImageID,
        SelectIcon
        );
    if(!pMenuItem)
    {
        return;
    }
    m_showHotKey = m_showHotKey || ((cHotKey != KEY_RESERVED) && DeviceInfo::IsK3());
    // TODO: jzn 加载图片后计算宽度，会增加读盘次数和浪费时间，但是不用做反复修改code
    m_menuItems.push_back(pMenuItem);
    SetModified();
    Update();
}

void UIMenu::SetUIPage(UIPage* pUIPage)
{
    m_pUIPage = pUIPage;
}

BOOL UIMenu::IsCached()
{
    return m_spBackImgCache;
}

void UIMenu::SetEventHandler(UIWindow* pEventHandler)
{
    m_pEventHandler = pEventHandler;
}

void UIMenu::SetUpLevelMenu(UIMenu* pMenu)
{
    m_pUpLevelMenu = pMenu;
}

MenuItem* UIMenu::GetItemByID(INT32 iItemId)
{
    INT32 iSize = m_menuItems.size();
    for (INT32 i = 0; i < iSize; i++)
    {
        MenuItem* pItem = m_menuItems[i];
        if(!pItem)
        {
            continue;
        }
        DWORD rstrId = pItem->GetMenuId();
        if (rstrId == (DWORD)iItemId) {
            return pItem;
        }
    }

    return NULL;
}

void UIMenu::SetItemActive(INT32 iItemId, BOOL bActive)
{
    MenuItem* pItem = GetItemByID(iItemId);

    if (pItem)
    {
        pItem->SetActive(bActive);
    }
}

void UIMenu::SetItemLabel(INT32 iItemId, const CString& rstrLabel)
{
    MenuItem* pItem = GetItemByID(iItemId);

    if (pItem)
    {
        pItem->SetMenuLabel(rstrLabel);
    }
}

void UIMenu::SetItemSelectIcon(INT32 iItemId, INT32 iSelectImage)
{
    INT32 iSize = m_menuItems.size();
    for (INT32 i = 0; i < iSize; i++)
    {
        MenuItem* pItem = m_menuItems[i];
        if(pItem)
        {
            pItem->SetItemSelectImage(-1);
        }
    }
    MenuItem* pItem = GetItemByID(iItemId);
    if (pItem)
    {
        pItem->SetItemSelectImage(iSelectImage);
    }
}


INT32 UIMenu::GetItemHeight(INT32 iItem)
{
    MenuItem* pItem = GetItem(iItem);

    if (pItem == NULL)
    {
        return 0;
    }

    return pItem->IsSeparator() ? MENU_SEPARATOR_HEIGHT + MENU_ITEM_SPACE : m_pFont->GetHeight() + MENU_ITEM_SPACE;
}

void UIMenu::AppendSeparator()
{
    MenuItem* pItem = new MenuItem(IDSEPARATOR, "");
    if(pItem)
    {
        m_menuItems.push_back(pItem);
    }
}

INT32 UIMenu::GetContentHeight()
{
    INT32 iLineHeight = m_pFont->GetHeight();
    INT32 iContentHeight = MARGIN * 2;

    INT32 iSize = m_menuItems.size();
    for (INT32 i = 0; i < iSize; i++) {
        MenuItem* pItem = m_menuItems[i];
        if(!pItem)
        {
            continue;
        }
        if (!pItem->IsSeparator())
        {
            iContentHeight += iLineHeight + MENU_ITEM_SPACE;
        }
        else
        {
            iContentHeight += MENU_SEPARATOR_HEIGHT + MENU_ITEM_SPACE;
        }
    }

    return iContentHeight;
}

void UIMenu::OnSelect()
{
    Destroy();

    if (m_pUpLevelMenu)
    {
        m_pUpLevelMenu->OnSelect();
    }
}

void UIMenu::DestroySubMenu()
{
    if (m_pSubMenu)
    {
        m_pSubMenu->DestroySubMenu();

        // this class doesn't create the sub menu so it doesn't delete it
        m_pSubMenu = NULL;
    }

    //Repaint();
}

BOOL UIMenu::ContainsItemId(DWORD dwId)
{
    INT32 iSize = m_menuItems.size();
    for (INT32 i = 0; i < iSize; i++) {
        MenuItem* pItem = m_menuItems[i];
        if (pItem &&(pItem->GetMenuId() == dwId))
        {
            return TRUE;
        }
    }
    return FALSE;
}

const CString& UIMenu::GetMenuName()
{
    return m_strName;
}

void UIMenu::Destroy()
{
    if (m_pUpLevelMenu)
    {
        m_pUpLevelMenu->DestroySubMenu();
    }
    else
    {
        if (m_pUIPage)
        {
            m_pUIPage->DestroyMenu();
            m_pUIPage = NULL;
        }
    }
}

void UIMenu::SetMenuName(const CString& rstrName)
{
    m_strName = rstrName;
}

void UIMenu::Repaint()
{
	DebugPrintf(DLC_ZHY,"########### %s, UIMenu::Repaint",GetClassName());
    if (m_pUpLevelMenu)
    {
        m_pUpLevelMenu->Repaint();
    }
    else if (m_pUIPage)
    {
        m_pUIPage->Repaint();
    }
    UIWindow::UpdateWindow();
}

MenuItem* UIMenu::GetItem(INT32 iItem)
{
    INT32 iSize = m_menuItems.size();
    if (iItem < 0 || iItem > iSize - 1)
    {
        return NULL;
    }

    return m_menuItems[iItem];
}

HRESULT UIMenu::Draw(DK_IMAGE drawingImg)
{
    if (!m_bIsVisible)
    {
        return S_OK;
    }

    if (drawingImg.pbData == NULL)
    {
        return E_POINTER;
    }

    HRESULT hr(S_OK);
    DK_IMAGE imgSelf;
    DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
    RTN_HR_IF_FAILED(CropImage(
        drawingImg,
        rcSelf,
        &imgSelf
        ));

    DrawBackground(imgSelf);

    INT32 iMenuItemX =  MARGIN;
    INT32 iMenuItemY =  MARGIN;

    INT32 iSize = m_menuItems.size();
    if(m_style == Vertical)
    {
        for (INT32 i = 0; i < iSize; i++)
        {
            MenuItem* pItem = m_menuItems[i];
            if(pItem && pItem->IsVisible())
                iMenuItemY  += DrawMenuItem(imgSelf, pItem, i, iMenuItemX, iMenuItemY);
        }
    }
    else
    {
        for (INT32 i = 0; i < iSize; i++)
        {
            MenuItem* pItem = m_menuItems[i];
            if(pItem)
            {
                iMenuItemX  += DrawMenuItem(imgSelf, pItem, i, iMenuItemX, iMenuItemY);
            }
        }
    }
    if (m_pSubMenu)
    {
        m_pSubMenu->Draw(imgSelf);
    }

    return hr;
}

INT32 UIMenu::GetItemCount()
{
    return m_menuItems.size();
}

void UIMenu::PopupMenu(INT32 iX, INT32 iY, INT32 iScreenWidth, INT32 iScreenHeight)
{
    if (m_showSelectedIcon)
    {
        SetItemSelectIcon(GetSelectedMenuId(), IMAGE_SELECTICON);
    }

    PrePopup();
    if (!IsVisible())
    {
        Update(TRUE);
    }

    SetSelectedMenuItem(m_iSelectedMenuItem);
    m_iScreenWidth = iScreenWidth;
    m_iScreenHeight = iScreenHeight;

    m_iLeft = (iX + m_iWidth <= m_iScreenWidth) ? iX : m_iScreenWidth - m_iWidth - MARGIN;
    m_iTop = (iY <= m_iScreenHeight - iY) ? iY : iY - m_iHeight;

    if (m_iLeft < 0)
    {
        m_iLeft = 0;
    }

    if (m_iTop < 0)
    {
        m_iTop = 0;
    }

    m_bIsVisible = TRUE;

    Repaint();
}

void UIMenu::OnNotify(UIEvent rEvent)
{
    if (rEvent.GetEventId()== MENU_CLICK_EVENT)
    {
        CString para1 = rEvent.GetParam();

        if (para1 == MENU_ID_SELECT)
        {
            ClickMenuItem();
            return;
        }

        if (para1 == MENU_ID_CANCEL)
        {
            Destroy();
            return;
        }
    }

    UIWindow::OnNotify(rEvent);
}

void UIMenu::PrePopup()
{
    return;

    INT32 iLineHeight = m_pFont->GetHeight();
    m_iHeight = 0;

    INT32 iShortKey = 1;
    INT32 iSize = m_menuItems.size();
    for (INT32 i = 0; i < iSize; i++) {
        MenuItem* pItem = m_menuItems[i];
        if(!pItem)
        {
            continue;
        }
        if (!pItem->IsSeparator())
        {
            if (iShortKey <= 10)
            {
                pItem->SetShortcutKey(iShortKey++);

                // FIXME: here should use StringBuffer to operate
                CString str(pItem->GetShortcutKey());
                str = str + TEXT(". ");

                pItem->SetMenuLabel(str);
            }

            INT32 iStrWidth = m_pFont->StringWidth(pItem->GetMenuLabel()) + MARGIN * 2 + MARGIN_TXT_ITEM * 2
                + (pItem->GetSubMenu() != NULL ? iLineHeight : 0);

            if (m_iWidth < iStrWidth)
            {
                m_iWidth = iStrWidth;
            }
        }
    }

    m_iHeight += GetContentHeight();
}

INT32 UIMenu::Point2Item(INT32 iX, INT32 iY)
{
    // Note, it is not implemented in this milestone
    UNUSED(iX);
    UNUSED(iY);
    return 0;
}

void UIMenu::PopSubMenu()
{
    MenuItem* pItem = m_menuItems[m_iSelectedMenuItem];

    if (pItem)
    {
        PopSubMenu(pItem);
    }
}

int UIMenu::GetLeftMarginOfTitle()
{
    int margin = 0;
    if (m_showHotKey)
    {
        margin = (MENU_HOTKEY_ICON_SIZE + MENU_ITEM_IMAGE_TO_TEXT);
    }
    else if (m_showSelectedIcon)
    {
        margin = MENU_ICON_WIDTH + MENU_ITEM_IMAGE_TO_TEXT;
    }
    return margin;
}

void UIMenu::PopSubMenu(MenuItem* pItem)
{
    if (pItem->GetSubMenu()) {
        if (m_pSubMenu != pItem->GetSubMenu())
        {
            DestroySubMenu();
            m_pSubMenu = pItem->GetSubMenu();
            m_pSubMenu->PopupMenu(m_iLeft + m_iWidth, m_iTop + (m_pFont->GetHeight() + MENU_ITEM_SPACE) * m_iSelectedMenuItem
                + m_pFont->GetHeight() / 2, m_iScreenWidth, m_iScreenHeight);
        }
        else
        {
            DestroySubMenu();
        }

        Repaint();
    }
}

void UIMenu::DrawBackground(DK_IMAGE drawingImg)
{
    CTpGraphics prg(drawingImg);
    SPtr<ITpImage> spImg = ImageManager::GetImage(IMAGE_MENU_BACK);
    if (spImg)
    {
        prg.DrawImageStretchBlend(spImg.Get(), 0, 0, m_iWidth, m_iHeight);
    }
}

INT32 UIMenu::GetMenuItemSelectLine(int &x, int &y, int &w, int &h, int indexItem)
{
    INT32 iSize = m_menuItems.size();
    x = MARGIN + GetLeftMarginOfTitle(); 
    y = MARGIN;
    w = h = 0;
    if (indexItem>=iSize)
    {
        indexItem = iSize-1;
    }
    if(m_style == Vertical)
    {
        for (INT32 i = 0; i < indexItem; i++)
        {
            MenuItem*pItem = m_menuItems[i];
            if(!pItem)
            {
                continue;
            }
            if (pItem->IsSeparator())
            {
                y  += (MENU_SEPARATOR_HEIGHT + MENU_ITEM_SPACE);
            }
            else if (pItem->IsStatic())
            {
                y  += MENU_ITEM_HEIGHT;
            }
            else if(pItem->IsVisible())
            {
                y  += MENU_ITEM_HEIGHT;
            }
        }
        if((m_iMenuType == 0) || (m_iMenuType == ID_MNU_SORT))
        {
            y += (MENU_IMAGE_HEIGHT);
            w = m_iWidth - MARGIN + 2 - x;
            h = MENU_SOLID_LINE_HEIGHT;
        }
    }
    else
    {
        ITpFont *pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, 0);
        for (INT32 i = 0; i < indexItem; i++)
        {
            MenuItem* pItem = m_menuItems[i];
            if(!pItem)
            {
                continue;
            }
            if (pItem->IsSeparator())
            {
                x +=  (MARGIN >> 2);
            }
            else if (pItem->IsStatic())
            {
                x  += (pFont->StringWidth(pItem->GetMenuLabel()) + MARGIN );
            }
            else
            {
                x += pFont->StringWidth(pItem->GetMenuLabel()) + MARGIN + GetLeftMarginOfTitle();
            }
        }

        y += (MENU_IMAGE_HEIGHT);
        MenuItem* pItem = m_menuItems[indexItem];
        if(pItem)
        {
            w =  pFont->StringWidth(pItem->GetMenuLabel());
        }
        h = MENU_SOLID_LINE_HEIGHT;
    }
    return 0;
}


INT32 UIMenu::DrawMenuItem(DK_IMAGE drawingImg, MenuItem* pItem, INT32 iItemIndex, INT32 iMenuItemX, INT32 iMenuItemY)
{
    if(!pItem)
    {
        return 0;
    }
    INT32 iItemHeight = 0;
    INT32 iItemWidth  = 0;
    CTpGraphics prg(drawingImg);
    if (pItem->IsSeparator())
    {
        iItemWidth = m_iWidth - MARGIN * 2;
        iItemHeight = MENU_SEPARATOR_HEIGHT + MENU_ITEM_SPACE;
        DrawMenuItemSeparator(drawingImg, iMenuItemX, iMenuItemY, iItemWidth, iItemHeight);
    }
    else if (pItem->IsStatic())
    {
        iItemHeight = MENU_ITEM_HEIGHT;
        m_ifontAttr.SetFontAttributes(0, 0, 24);
        ITpFont *pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
        if(pFont)
        {
            prg.DrawStringUtf8(pItem->GetMenuLabel(), iMenuItemX, iMenuItemY, m_pFont);
            //DKFontAttributes StaticFont;
            iItemWidth = pFont->StringWidth(pItem->GetMenuLabel());
        }
    }
    else
    {
        if(m_iMenuType == 0)
        {
            iItemHeight = MENU_ITEM_HEIGHT;
            BOOL isSelected = (iItemIndex == m_iSelectedMenuItem);
            INT32 iColor = 0;
            if (pItem->IsActive())
            {
                iColor = ColorManager::GetColor(/*isSelected ? COLOR_SELECTION_TXT :*/ COLOR_SOFTKEY_TXT_HIT);
            }
            else
            {
                iColor = ColorManager::GetColor(COLOR_MENUITEM_INACTIVE);
            }
            m_ifontAttr.SetFontAttributes(0, 0, 18);
            ITpFont *pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, iColor);
            if(pFont == NULL)
            {
                return 0;
            }
            int usedWidth = 0;
            if (m_showHotKey)
            {
                //draw icon
                SPtr<ITpImage> spImg = ImageManager::GetImage(pItem->GetItemLeftImage());
                prg.DrawImageBlend(spImg.Get(), iMenuItemX, iMenuItemY, 0, 0, spImg.Get()->GetWidth(),  spImg.Get()->GetHeight());

                // draw hot key
                char A = pItem->GetHotKey();
                INT32 iFontWidth = pFont->StringWidth(CString(&A, 1));
                INT32 iFontHeight = (spImg.Get()->GetHeight() - 18) /2;
                iFontWidth = (spImg.Get()->GetWidth() - iFontWidth) / 2;
                prg.DrawStringUtf8(CString(&A, 1).GetBuffer(), iMenuItemX + iFontWidth, iMenuItemY + iFontHeight, pFont);
                const int hotKeyWidth = spImg.Get()->GetWidth() + MENU_ITEM_IMAGE_TO_TEXT;
                iMenuItemX += hotKeyWidth;
                iItemWidth += hotKeyWidth;

                // draw right key
                char strShortKey[100] = {0};
                char strAltKey[16] = {0};
                if(ALT_DOWN & pItem->GetShortcutKey())
                {
                    sprintf(strAltKey, "%s", "Alt+");
                }
                else if(SHIFT_DOWN & pItem->GetShortcutKey())
                {
                    sprintf(strAltKey, "%s", "Shift+");
                }
                else
                {
                    strAltKey[0] = '\0';
                }
                if (0xFF & pItem->GetShortcutKey())
                {
                    if((0xFF & pItem->GetShortcutKey()) == KEY_SYM)
                    {
                        sprintf(strShortKey, "%s%s", strAltKey, "Sym");
                    }
                    else if((0xFF & pItem->GetShortcutKey()) == KEY_DEL)
                    {
                        sprintf(strShortKey, "%s%s",  strAltKey, "Del");
                    }
                    else if((0xFF & pItem->GetShortcutKey()) == KEY_DOT)
                    {
                        sprintf(strShortKey, "%s%c",  strAltKey, '.');
                    }
                    else if((0xFF & pItem->GetShortcutKey()) == KEY_FONT)
                    {
                        sprintf(strShortKey, "%s%s",  strAltKey, "Aa");
                    }
                    else if((0xFF & pItem->GetShortcutKey()) == KEY_DOWN)
                    {
                        //              TODO:上下箭头为utf-8编码
                        sprintf(strShortKey, "%s",  strAltKey);
                        INT32 iLen = strlen(strShortKey);
                        strShortKey[iLen++] = 0xE2;
                        strShortKey[iLen++] = 0x86;
                        strShortKey[iLen++] = 0x93;
                        strShortKey[iLen++] = '\0';
                    }
                    else if((0xFF & pItem->GetShortcutKey()) == KEY_UP)
                    {
                        //              TODO:上下箭头为utf-8编码
                        sprintf(strShortKey, "%s",  strAltKey);
                        INT32 iLen = strlen(strShortKey);
                        strShortKey[iLen++] = 0xE2;
                        strShortKey[iLen++] = 0x86;
                        strShortKey[iLen++] = 0x91;
                        strShortKey[iLen++] = '\0';
                    }
                    else
                    {
                        sprintf(strShortKey, "%s%c", strAltKey, VirtualKeyToChar((0xFF & pItem->GetShortcutKey())) - 32);
                    }

                    DKFontAttributes fontAttr;
                    fontAttr.SetFontAttributes(0, 0, 14);
                    pFont = FontManager::GetInstance()->GetFont(fontAttr, ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
                    if(pFont)
                    {
                        usedWidth = pFont->StringWidth(strShortKey);
                        prg.DrawStringUtf8(strShortKey, m_iWidth - MARGIN - usedWidth, iMenuItemY + 3, pFont);
                        usedWidth += MARGIN_TXT_ITEM;
                    }
                }
            }

            //draw lable
            int idrawlable_x = iMenuItemX;
            string lable = pItem->GetMenuLabel();
            const int leftWidth = m_iWidth - MARGIN - usedWidth -  iMenuItemX;
            pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, iColor);
            if(pFont && pFont->StringWidth(lable.c_str()) >= leftWidth)
            {
                lable += string("...");
                while (pFont->StringWidth(lable.c_str()) > leftWidth)
                {
                    lable = lable.substr(0, lable.length() - 4);
                    lable += string("...");
                }
            }
            prg.DrawStringUtf8(lable.c_str(), iMenuItemX, iMenuItemY + MENU_ITEM_SPACE, pFont);
            const int textWidth = pFont->StringWidth(lable.c_str());
            iItemWidth += textWidth + usedWidth;

            // draw submenu right arrow
            if (pItem->GetSubMenu())
            {
                DrawSubmenuArrow(drawingImg, iMenuItemX + iItemWidth, iMenuItemY, iItemWidth, iItemHeight);
            }
            if (m_showSelectedIcon)
            {
                //TODO:
                int iSelectId = pItem->GetItemSelectImage();
                if(iSelectId > 0)
                {
                    SPtr<ITpImage> spImg = ImageManager::GetImage(iSelectId);
                    prg.DrawImageBlend(spImg.Get(), iMenuItemX, iMenuItemY - 5 * MENU_ITEM_SPACE, 0, 0, spImg.Get()->GetWidth(),  spImg.Get()->GetHeight());
                }
            }

            // draw select line
            int iSelectLine_x = iMenuItemX;
            int iSelectLine_y = iMenuItemY + MENU_IMAGE_HEIGHT ;
            int iSelectLine_w = 0;
            if (m_style == Vertical)
            {
                iSelectLine_w  =  m_iWidth - MARGIN * 2 + 2 - GetLeftMarginOfTitle();
            }
            else
            {
                iSelectLine_w = textWidth;
            }
            if (isSelected)
            {
                prg.DrawLine(iSelectLine_x, iSelectLine_y,  iSelectLine_w , MENU_SOLID_LINE_HEIGHT, SOLID_STROKE);
            }
            else
            {
                prg.DrawLine(iSelectLine_x, iSelectLine_y,  iSelectLine_w - 1 , MENU_DOT_LINE_HEIGHT, DOTTED_STROKE);
            }
        }
        else if(m_iMenuType == ID_MNU_SORT)
        {
            iItemHeight = MENU_ITEM_HEIGHT;
            BOOL isSelected = (iItemIndex == m_iSelectedMenuItem);
            if (m_showSelectedIcon)
            {
                //draw select icon
                int iSelectId = pItem->GetItemSelectImage();
                if(iSelectId > 0)
                {  
                    SPtr<ITpImage> spImg = ImageManager::GetImage(iSelectId);
                    prg.DrawImageBlend(spImg.Get(), iMenuItemX, iMenuItemY + 3 * MENU_ITEM_SPACE, 0, 0, spImg.Get()->GetWidth(),  spImg.Get()->GetHeight());
                }
            }
            iMenuItemX += GetLeftMarginOfTitle();

            //draw label
            m_ifontAttr.SetFontAttributes(0, 0, 18);
            int iColor = ColorManager::GetColor(COLOR_SOFTKEY_TXT_HIT);
            ITpFont *pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, iColor);

            prg.DrawStringUtf8(pItem->GetMenuLabel(), iMenuItemX, iMenuItemY + MENU_ITEM_SPACE, pFont);

         // draw select line
            int iSelectLine_x = iMenuItemX;
            int iSelectLine_y = iMenuItemY + MENU_IMAGE_HEIGHT ;
            int iSelectLine_w = 0;
            if (m_style == Vertical)
            {
                iSelectLine_w +=  m_iWidth - MARGIN * 2;
            }
            else
            {
                iSelectLine_w += pFont->StringWidth(pItem->GetMenuLabel());
            }

            iSelectLine_w -= GetLeftMarginOfTitle();
            
            if (isSelected)
            {
                prg.DrawLine(iSelectLine_x, iSelectLine_y,  iSelectLine_w , MENU_SOLID_LINE_HEIGHT, SOLID_STROKE);
            }
            else
            {
                prg.DrawLine(iSelectLine_x, iSelectLine_y,  iSelectLine_w - 1, MENU_DOT_LINE_HEIGHT, DOTTED_STROKE);
            }
        }
    }
    if (m_style == Vertical)
    {
        return iItemHeight;
    }
    else
    {
        iItemWidth += MARGIN;
        return iItemWidth;
    }
}


void UIMenu::DrawMenuItemSeparator(DK_IMAGE drawingImg, INT32 iMenuItemX, INT32 iMenuItemY, INT32 iItemWidth,
    INT32 iItemHeight)
{
    CTpGraphics grp(drawingImg);
    //    pGraphics->SetColor(COLOR_HILIGHT);
    //    pGraphics->SetStrokeStyle(SOLID_STROKE);
    INT32 iMidY = iMenuItemY + iItemHeight / 2;
    grp.DrawLine(iMenuItemX, iMidY, iMenuItemX + iItemWidth, iMidY, SOLID_STROKE);
    //    pGraphics->SetColor(COLOR_SHADOW);
    grp.DrawLine(iMenuItemX, iMidY + 1, iMenuItemX + iItemWidth, iMidY + 1, SOLID_STROKE);
}

void UIMenu::DrawMenuItemBack(DK_IMAGE drawingImg, INT32 iMenuItemX, INT32 iMenuItemY, INT32 iItemWidth,
    INT32 iItemHeight, BOOL bIsSelected)
{
    //do nothing
}

void UIMenu::DrawSubmenuArrow(DK_IMAGE drawingImg, INT32 iMenuItemX, INT32 iMenuItemY, INT32 iItemWidth,
    INT32 iItemHeight)
{
    //do nothing
}

void UIMenu::ClickMenuItem()
{
    MenuItem* pItem = m_menuItems[m_iSelectedMenuItem];
    if(!pItem)
    {
        return;
    }
    if (pItem->GetSubMenu() == NULL)
    {
        if (!pItem->IsActive())
        {
            return;
        }
        //TODO 可以为每个item定制单独的pEventHandler
        if (m_pEventHandler)
        {
            m_pEventHandler->OnEvent(UIEvent(COMMAND_EVENT, this, pItem->GetMenuId()));
        }
        //m_pEventHandler->OnNotify(event);
        //        OnSelect();
        //        Repaint();
    }
    else
    {
        PopSubMenu(pItem);
    }
}

void UIMenu::OnMenuChanged()
{
    // it is empty
    // this method override the method in the UIWindow
    // because UIMenu doesn't need to tell his parent its menu is changed.
}

void  UIMenu::ClearAllItem()
{
    INT32 iSize = m_menuItems.size();
    if (iSize > 0)
    {
        for (INT32 i = 0; i < iSize; i++) {
            MenuItem* pItem = m_menuItems[i];
            if(pItem)
            {
                delete pItem;
            }
        }
        m_menuItems.clear();
        m_iSelectedMenuItem = 0;
        SetModified();
        Update();
    }


    //Show(BOOL bIsShow);
}

void UIMenu::SetItemVisible(INT32 iItemId, BOOL bVisible)
{
    MenuItem* pItem = GetItemByID(iItemId);

    if (pItem)
    {

        if ((bVisible && !pItem->IsVisible())
                || (!bVisible && pItem->IsVisible()))
        {
            pItem->SetVisible(bVisible);
            SetModified();
            Update();
        }
    }
}

void UIMenu::SetSelectItem(INT32 iItemId)
{
    INT32 iSize = m_menuItems.size();
    for (INT32 i = 0; i < iSize; i++)
    {
        MenuItem* pItem = m_menuItems[i];
        if(!pItem)
        {
            continue;
        }
        if(!pItem->IsActive())
            continue;
        if(pItem->IsSeparator())
            continue;
        if(pItem->IsStatic())
            continue;
        if(!pItem->IsVisible())
            continue;
        if(pItem->GetMenuId() == (UINT32)iItemId)
        {
            if (m_iSelectedMenuItem != i)
            {
                m_iSelectedMenuItem = i;
                SetModified();
                Update();
            }
        }
    }

}

void UIMenu::Update(BOOL bForce)
{
    if (IsVisible() || bForce)
    {
        if (IsModified())
        {
            ClearModified();
            MoveWindow(m_iLeft, m_iTop, m_iWidth, m_iHeight);
        }
    }
}

BOOL UIMenu::SetVisible(BOOL bVisible)
{
    if (bVisible && !IsVisible())
    {
        Update(TRUE);
    }
    return UIDialog::SetVisible(bVisible);
}

void UIMenu::Show(BOOL bIsShow, BOOL bIsRepaint)
{
    if (bIsShow && !IsVisible())
    {
        Update(TRUE);
    }
    UIDialog::Show(bIsShow, bIsRepaint);
}

void UIMenu::SetMenuType(INT32 type)
{
    m_iMenuType = type;
    m_showSelectedIcon = (m_iMenuType == ID_MNU_SORT) && (Vertical == m_style);
    m_showHotKey = m_showHotKey && (m_iMenuType == 0);
}

void UIMenu::ShowSelectedIcon(bool showIcon)
{
    m_showSelectedIcon = (!m_showHotKey) && showIcon;
}

bool UIMenu::IsShowSelectedIcon()
{
    return m_showSelectedIcon;
}

DWORD UIMenu::GetSelectedMenuId()
{
    if ((m_iSelectedMenuItem >= 0) && ((size_t)m_iSelectedMenuItem < m_menuItems.size()))
    {
        MenuItem* pItem = m_menuItems[m_iSelectedMenuItem];
        if (pItem)
        {
            return pItem->GetMenuId();
        }
    }
    return IDSTATIC;
}

INT32 UIMenu::GetSelectedMenuItem()
{
    return m_iSelectedMenuItem;
}

void  UIMenu::SetSelectedMenuItem(INT32 _item)
{
    m_iSelectedMenuItem = (_item < 0) ? 0 : _item;
}

INT32 UIMenu::GetScreenWidth()
{
    return m_iScreenWidth;
}

void  UIMenu::SetScreenWidth(INT32 _value)
{
    m_iScreenWidth = _value;
}
INT32 UIMenu::GetScreenHeight()
{
    return m_iScreenHeight;
}
void  UIMenu::SetScreenHeight(INT32 _value)
{
    m_iScreenHeight = _value;
}
