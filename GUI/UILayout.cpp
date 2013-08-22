////////////////////////////////////////////////////////////////////////
// UILayout.cpp
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "GUI/UILayout.h"

UILayout::UILayout() :
m_bInited(FALSE), m_controls()
{
    // empty
}

UILayout::~UILayout()
{
    Dispose();
}

void UILayout::OnResize(INT32 iWidth, INT32 iHeight)
{
    INT32 iSize = m_controls.size();
    for (INT32 i = 0; i < iSize; i++)
    {
        LayoutItem& item = m_controls[i];
        UIWindow* pWindow = item.m_pWindow;

        INT32 iWindowX = pWindow->GetX();
        INT32 iWindowY = pWindow->GetY();
        INT32 iWindowWidth = pWindow->GetWidth();
        INT32 iWindowHeight = pWindow->GetHeight();
        BOOL bMoved= FALSE;

        if ((item.m_iFlag & LAYOUT_RIGHT_ALIGN) != 0)
        {
            bMoved = TRUE;
            iWindowX = iWidth - pWindow->GetWidth()- item.m_iRightMargin;
        }
        else if ((item.m_iFlag & LAYOUT_RIGHT_EXTEND) != 0)
        {
            bMoved = TRUE;
            iWindowWidth = iWidth - pWindow->GetX()- item.m_iRightMargin;
        }

        if ((item.m_iFlag & LAYOUT_BOTTOM_ALIGN) != 0)
        {
            bMoved = TRUE;
            iWindowY = iHeight - pWindow->GetHeight()- item.m_iBottomMargin;
        }
        else if ((item.m_iFlag & LAYOUT_BOTTOM_EXTEND) != 0)
        {
            bMoved = TRUE;
            iWindowHeight = iHeight - pWindow->GetY()- item.m_iBottomMargin;
        }

        if (bMoved)
        {
            pWindow->MoveWindow(iWindowX, iWindowY, iWindowWidth, iWindowHeight);
        }
    }
}

BOOL UILayout::IsInited()
{
    return m_bInited;
}

void UILayout::AddControl(UIWindow* pWindow, INT32 iFlag)
{
    TP_ASSERT(pWindow != NULL);

    m_controls.push_back(LayoutItem(pWindow, iFlag));
}

void UILayout::RemoveControl(UIWindow* pWindow)
{
    INT32 iSize = m_controls.size();
    for (INT32 i = 0; i < iSize; i++)
    {
        LayoutItem& item = m_controls[i];

        if (item.m_pWindow == pWindow)
        {
            m_controls[i].m_pWindow = NULL;
            m_controls.erase(m_controls.begin() + i);
            return;
        }
    }
}

void UILayout::ClearControls()
{
    INT32 iSize = m_controls.size();
    for (INT32 i = 0; i < iSize; i++)
    {
        m_controls[i].m_pWindow = NULL;
    }
    m_controls.clear();
}

void UILayout::Init(INT32 iWidth, INT32 iHeight)
{
    size_t iSize = m_controls.size();
    for (size_t i = 0; i < iSize; i++)
    {
        LayoutItem& item = m_controls[i];

        item.m_iRightMargin = iWidth - item.m_pWindow->GetX()- item.m_pWindow->GetWidth();
        item.m_iBottomMargin = iHeight - item.m_pWindow->GetY()- item.m_pWindow->GetHeight();
    }

    m_bInited = true;
}

void UILayout::Dispose()
{
    ClearControls();
}

