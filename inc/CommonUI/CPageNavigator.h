////////////////////////////////////////////////////////////////////////
// CPageNavigator.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __CPAGENAVIGATOR_H__
#define __CPAGENAVIGATOR_H__

#include "GUI/UIPage.h"
#include "Common/Defs.h"
#include <vector>

class CPageNavigator
{
public:
    static void Goto(UIPage* spPage);
    static void BackDangerous();
    static UIPage* GetCurrentPage();
    static void BackToEntryPage(UINT32 iParam1 = 0,UINT32 iParam2 = 0,UINT32 iParam3 = 0);
    static void BackToEntryPageOnType(UINT32 iParam1, UINT32 iParam2, UINT32 iParam3);

    static void ClearAllPage();

    static void BackToHome(UINT32 iParam1 = 0,UINT32 iParam2 = 0,UINT32 iParam3 = 0);
    static void BackToPrePage(UINT32 iParam1 = 0,UINT32 iParam2 = 0,UINT32 iParam3 = 0);
    static void BackToEntryPageOnTypeAsync(PageAncestorType type);
    static void BackToEntryPageOnCurrentType();
    static UIPage* GetLastPage();

    static void OnUsbPlugin();
    static void OnUsbPullout();
    static bool IsInStack(const UIWindow* window);
    static bool IsOnTop(const UIWindow* window);

    static void ChangePageStackToType(PageAncestorType type);
    static PageAncestorType GetCurrentActiveType();
private:
    static void SetNewPage(UIPage* spNewPage);
    /**
     * @brief ClearAllPageOnType internal method to clear the pages with the same type
     * @param pat page enum type
     */
    static void ClearAllPageOnType(PageAncestorType type);
    CPageNavigator();
    static bool IsPageTypeValid(PageAncestorType type);
    static void SetCurrentActiveType(PageAncestorType type);
    static UIPage* CreateEntryPageForType(PageAncestorType type);

private:
    static std::vector<UIPage*> s_spVisitedPages[PAT_Count];
    static PageAncestorType s_currentActiveType;
    //static std::vector< UIPage* > s_spVisitedPages;
};

#endif
