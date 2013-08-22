////////////////////////////////////////////////////////////////////////
// UIBottomBar.cpp
// Contact:
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////
#include "TouchAppUI/UIBottomBar.h"
#include "Framework/CDisplay.h"
#include "I18n/StringManager.h"
#include "GUI/CTpGraphics.h"
#include "../Common/FileManager_DK.h"
#include "TouchAppUI/UIHomePage.h"
#include "TouchAppUI/UISystemSettingPageNew.h"
#include "BookStoreUI/UIDKBookStoreIndexPage.h"
#include "PersonalUI/UIPersonalPage.h"
#include "CommonUI/UIUtility.h"
#include "Common/CAccountManager.h"
#include "Common/WindowsMetrics.h"
#include "GUI/UISizer.h"

#define UIBOTTOMBAR_SELECTEDLINE      6
#define UIBOTTOMBAR_DEFAULTLINE       1
using namespace dk::account;
using namespace WindowsMetrics;

SINGLETON_CPP(UIBottomBar)
    
UIBottomBar::UIBottomBar()
    : UIButtonGroup(NULL)
{
    InitUI();
}

UIBottomBar::~UIBottomBar()
{
}

void UIBottomBar::InitUI()
{
    const int fontSize = GetWindowFontSize(UIBottomBarIndex);
    m_bottomLinePixel = 0;

    SetMinHeight(GetWindowMetrics(UIBottomBarHeightIndex));
    SetSplitLineHeight(GetWindowMetrics(UIBottomBarSplitLineHeightIndex));
    const int btnIds[PAT_Count] = 
    {
        ID_BTN_TOUCH_BOOKSHELF, ID_BTN_TOUCH_BOOKSTORE, ID_BTN_TOUCH_APPLICATION, ID_BTN_TOUCH_SETTING
    };
    const SPtr<ITpImage> focusedImgs[PAT_Count] = 
    {
        ImageManager::GetImage(IMAGE_TOUCH_HOME), ImageManager::GetImage(IMAGE_TOUCH_STORE), 
        ImageManager::GetImage(IMAGE_TOUCH_ACCOUNT), ImageManager::GetImage(IMAGE_TOUCH_SETTING)
    };

    for ( int i=0; i<PAT_Count; ++i)
    {
        m_btns[i].Initialize(btnIds[i], "", fontSize);
        m_btns[i].SetIcons(focusedImgs[i], focusedImgs[i], UIButton::ICON_LEFT);
        m_btns[i].SetUpdateOnPress(false);
        m_btns[i].SetUpdateOnUnPress(false);

        AddButton(m_btns + i);
    }
    CDisplay *pDisplay = CDisplay::GetDisplay();
    if(pDisplay)
    {
        m_minWidth = pDisplay->GetScreenWidth();
        m_minHeight = GetWindowMetrics(UIBottomBarHeightIndex);
    }
}

void UIBottomBar::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    int currentFocusedIndex = IndexOfWindow(pSender);
    if (currentFocusedIndex < 0 || currentFocusedIndex >= (int)GetChildrenCount())
    {
        return;
    }

    if (m_currentFocusedIndex == currentFocusedIndex)
    {
        CPageNavigator::BackToEntryPageOnTypeAsync((PageAncestorType)m_currentFocusedIndex);
    }
    else
    {
        //Hack:check the net status before the entry of dk-bookstore
        DebugPrintf(DLC_DIAGNOSTIC, "UIBottomBar::OnCommand: %d, %d", m_currentFocusedIndex, currentFocusedIndex);
        if ((PageAncestorType)currentFocusedIndex == PAT_BookStore)
        {
            if (!UIUtility::CheckAndReConnectWifi())
            {
                return;
            }
        }

        m_currentFocusedIndex = IndexOfWindow(pSender);
        CPageNavigator::ChangePageStackToType((PageAncestorType)m_currentFocusedIndex);
    }
}
  
HRESULT UIBottomBar::DrawFocusedSymbol(DK_IMAGE& image)
{
    HRESULT hr(S_OK);
    UISizerItem* focusedItem = m_pBtnListSizer->GetItem(m_currentFocusedIndex);
    if (focusedItem)
    {
        CTpGraphics grfButton(image);
        hr = grfButton.DrawLine(focusedItem->GetPosition().x, 0, focusedItem->GetSize().GetWidth(), GetWindowMetrics(UIBottomBarFocusedLinePixelIndex), SOLID_STROKE);
    }

    return hr;
}
