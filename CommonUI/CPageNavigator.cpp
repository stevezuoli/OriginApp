////////////////////////////////////////////////////////////////////////
// CPageNavigator.cpp
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "Framework/CDisplay.h"
#include "Common/FileManager_DK.h"
#include "Common/FileSorts_DK.h"
#include "CommonUI/CPageNavigator.h"
#include "TouchAppUI/UIHomePage.h"
#include "TouchAppUI/UISystemSettingPageNew.h"
#include "BookStoreUI/UIDKBookStoreIndexPage.h"
#include "PersonalUI/UIPersonalPage.h"
#include <vector>

PageAncestorType CPageNavigator::s_currentActiveType = PAT_BookShelf;

std::vector<UIPage*> CPageNavigator::s_spVisitedPages[PAT_Count];

bool CPageNavigator::IsPageTypeValid(PageAncestorType type)
{
    return (type>=0 && type<PAT_Count);
}

CPageNavigator::CPageNavigator()
{
    // empty
}

void CPageNavigator::Goto(UIPage* pPage)
{
    if (NULL == pPage)
    {
        return;
    }

    UIPage *pCurPage = GetCurrentPage();
    if (NULL != pCurPage)
    {
        pCurPage->OnLeave();
    }

    pPage->OnEnter();

    SetNewPage(pPage);
    s_spVisitedPages[s_currentActiveType].push_back(pPage);
}

//note:return last page in the same stack
UIPage* CPageNavigator::GetLastPage()
{
    unsigned int iSize = s_spVisitedPages[s_currentActiveType].size();
    if (iSize < 2)
    {
        return NULL;
    }
    return s_spVisitedPages[s_currentActiveType][iSize - 2];
}

void CPageNavigator::BackDangerous()
{
    unsigned int iSize = s_spVisitedPages[s_currentActiveType].size();
    if (iSize < 2)
    {
        return;
    }

    // back to the last visited page
    UIPage *pCurPage = s_spVisitedPages[s_currentActiveType][iSize - 1];
    UIPage *pPrevPage = s_spVisitedPages[s_currentActiveType][iSize - 2];
    if(NULL == pCurPage || NULL == pPrevPage)
    {
        return;
    }
    pCurPage->OnLeave();
    delete pCurPage;
    // dispose the page in the top position and remove it from the visited page
    s_spVisitedPages[s_currentActiveType].pop_back();

    pPrevPage->OnEnter();
    SetNewPage(pPrevPage);
}

void CPageNavigator::BackToEntryPage(UINT32 iParam1,UINT32 iParam2,UINT32 iParam3)
{
    CDKFileManager* pFileManager = CDKFileManager::GetFileManager();
    CDisplay *pDisplay = CDisplay::GetDisplay();
    if(NULL == pFileManager || NULL == pDisplay)
    {
        return ;
    }
    ClearAllPage();

    pFileManager->SortFile(DFC_Book);
    UIPage* pInitedPage = new UIHomePage(NORMALPAGE);
    s_currentActiveType = PAT_BookShelf;

    if(pInitedPage)
    {
        pInitedPage->MoveWindow(0, 0, pDisplay->GetScreenWidth(), pDisplay->GetScreenHeight());
        CPageNavigator::Goto(pInitedPage);
    }
}

void CPageNavigator::BackToEntryPageOnType(UINT32 iParam1, UINT32 iParam2, UINT32 iParam3)
{
    if (iParam1 >= PAT_Count)
    {
        return;
    }

    UIPage *pCurPage = GetCurrentPage();
    if (NULL != pCurPage)
    {
        pCurPage->OnLeave();
    }

    PageAncestorType type = (PageAncestorType)iParam1;
    CDisplay*       pDisplay     = CDisplay::GetDisplay();
    ClearAllPageOnType(type);
    s_currentActiveType = type;
    UIPage* pPage = CreateEntryPageForType(type);
    if(pPage && pDisplay)
    {
        UIBottomBar* pBottomBar = UIBottomBar::GetInstance();
        if (pBottomBar)
        {
            pBottomBar->SetFocusedIndex(type);
        }
        pPage->MoveWindow(0, 0, pDisplay->GetScreenWidth(), pDisplay->GetScreenHeight());
        Goto(pPage);
    }
}

UIPage* CPageNavigator::CreateEntryPageForType(PageAncestorType type)
{
    UIPage* pPage = NULL;
    switch(type)
    {
        case PAT_BookShelf:
            {
                CDKFileManager* pFileManager = CDKFileManager::GetFileManager();
                if(NULL != pFileManager)
                {
                    pFileManager->SortFile(DFC_Book);
                    pPage = new UIHomePage(NORMALPAGE);
                }
            }
            break;
        case PAT_BookStore:
            {
                pPage = new UIDKBookStoreIndexPage();
            }
            break;
        case PAT_Personal:
            {
                pPage = new UIPersonalPage();
            }
            break;
        case PAT_SystemSetting:
            {
                pPage = new UISystemSettingPageNew();
            }
            break;
        default:
            break;
    }
    return pPage;
}

void CPageNavigator::ClearAllPageOnType(PageAncestorType type)
{
    if (!IsPageTypeValid(type))
    {
        return;
    }

    unsigned int iSize = s_spVisitedPages[type].size();
    while (iSize > 0)
    {
        UIPage* pPage = s_spVisitedPages[type].back();
        if (NULL != pPage)
        {
            delete pPage;
        }
        s_spVisitedPages[type].pop_back();
        --iSize;
    }
}

void CPageNavigator::ClearAllPage()
{
    unsigned int iSize = s_spVisitedPages[s_currentActiveType].size();
    if(iSize > 1)
    {
        UIPage *pPage = s_spVisitedPages[s_currentActiveType][iSize - 1];
        if (NULL != pPage)
            pPage->OnLeave();
    }

    for ( int i=0; i<PAT_Count; ++i)
    {
        ClearAllPageOnType((PageAncestorType)i);
    }
}



UIPage* CPageNavigator::GetCurrentPage()
{
    if (!IsPageTypeValid(s_currentActiveType))
    {
        return NULL;
    }

    if (s_spVisitedPages[s_currentActiveType].empty())
    {
        return NULL;
    }
    return s_spVisitedPages[s_currentActiveType].back();
}

void CPageNavigator::SetNewPage(UIPage* spNewPage)
{
    if (NULL == spNewPage)
        return ;

    //not necessary to show current page, even flash to memory
    //if (spCurrentPage)
    //{
    //  spCurrentPage->Show(FALSE, FALSE);
    //}

    //spNewPage->MoveWindow(0, 0, CDisplay::GetDisplay().GetScreenWidth(), CDisplay::GetDisplay().GetScreenHeight());
    CDisplay *pDisplay = CDisplay::GetDisplay();
    if(pDisplay)
    {
        pDisplay->SetCurrentPage(spNewPage);
        spNewPage->Show(TRUE, FALSE);
        pDisplay->Repaint();
    }
}

void CPageNavigator::OnUsbPlugin()
{
    for (int i=0; i<PAT_Count; ++i)
    {
        unsigned int pageNumber = s_spVisitedPages[i].size();
        for (unsigned int j=0; j<pageNumber; ++j)
        {
            UIPage* pPage = s_spVisitedPages[i][j];
            if (NULL != pPage)
            {
                pPage->OnUnLoad();
            }
        }
    }
}

void CPageNavigator::OnUsbPullout()
{
    for (int i=0; i<PAT_Count; ++i)
    {
        unsigned int pageNumber = s_spVisitedPages[i].size();
        for (unsigned int j=0; j<pageNumber; ++j)
        {
            UIPage* pPage = s_spVisitedPages[i][j];
            if (NULL != pPage)
            {
                pPage->OnLoad();
            }
        }
    }

    CDisplay* pDisplay = CDisplay::GetDisplay();
    if (NULL != pDisplay)
        pDisplay->Repaint();
}


void CPageNavigator::BackToHome(UINT32 iParam1,UINT32 iParam2,UINT32 iParam3)
{
    SNativeMessage homeMsg;
    homeMsg.iType = KMessageGoHome;
    homeMsg.iParam1 = iParam1;
    homeMsg.iParam2 = iParam2;
    homeMsg.iParam3 = iParam3;
    INativeMessageQueue::GetMessageQueue()->Send(homeMsg);
}

void CPageNavigator::BackToEntryPageOnTypeAsync(PageAncestorType type)
{
    SNativeMessage backToEntryMsg;
    backToEntryMsg.iType = KMessageGoPageStackEntry;
    backToEntryMsg.iParam1 = type;
    backToEntryMsg.iParam2 = 0;
    backToEntryMsg.iParam3 = 0;
    INativeMessageQueue::GetMessageQueue()->Send(backToEntryMsg);
}

void CPageNavigator::BackToPrePage(UINT32 iParam1,UINT32 iParam2,UINT32 iParam3)
{
    SNativeMessage BackMsg;
    BackMsg.iType = KMessageGoPrePage;
    BackMsg.iParam1 = iParam1;
    BackMsg.iParam2 = iParam2;
    BackMsg.iParam3 = iParam3;
    INativeMessageQueue::GetMessageQueue()->Send(BackMsg);
}


bool CPageNavigator::IsInStack(const UIWindow* window)
{
    for (int i=0; i<PAT_Count; ++i)
    {
        unsigned int pageNumber = s_spVisitedPages[i].size();
        for (unsigned int j=0; j<pageNumber; ++j)
        {
            if (s_spVisitedPages[i][j] == window)
            {
                return true;
            }
        }
    }

    return false;
}

bool CPageNavigator::IsOnTop(const UIWindow* window)
{
    if (!IsPageTypeValid(s_currentActiveType) || s_spVisitedPages[s_currentActiveType].empty())
    {
        return false;
    }
    return s_spVisitedPages[s_currentActiveType].back() == window;
}

void CPageNavigator::SetCurrentActiveType(PageAncestorType type)
{
    s_currentActiveType = type;
}

void CPageNavigator::ChangePageStackToType(PageAncestorType type)
{
    if (IsPageTypeValid(type))
    {
        CDisplay::GetDisplay()->SetFullRepaint(true);
        if (s_spVisitedPages[type].empty())
        {
            BackToEntryPageOnTypeAsync(type);
        }
        else
        {
            UIPage *pCurPage = GetCurrentPage();
            if (NULL != pCurPage)
            {
                pCurPage->OnLeave();
            }

            s_currentActiveType = type;
            UIPage* pPage = s_spVisitedPages[type].back();
            pPage->OnEnter();
            SetNewPage(pPage);
        }
    }
}

void CPageNavigator::BackToEntryPageOnCurrentType()
{
    BackToEntryPageOnTypeAsync(s_currentActiveType);
}

PageAncestorType CPageNavigator::GetCurrentActiveType()
{
    return s_currentActiveType;
}