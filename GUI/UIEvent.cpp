////////////////////////////////////////////////////////////////////////
// UIEvent.cpp
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "GUI/UIEvent.h"

UIEvent::UIEvent(DefinedEvents iEventId, UIWindow *pSender, DWORD dwParam1, DWORD dwParam2) 
    : m_iEventId(iEventId)
    , m_pSender(pSender)
    , m_dwParam1(dwParam1)
    , m_dwParam2(dwParam2)
{

}

UIEvent::~UIEvent()
{
}

DefinedEvents UIEvent::GetEventId() const
{
    return m_iEventId;
}

void UIEvent::SetParam(DWORD dwParam1, DWORD dwParam2)
{
    m_dwParam1 = dwParam1;
    m_dwParam2 = dwParam2;
}

DWORD UIEvent::GetParam() const
{
    return m_dwParam1;
}

DWORD UIEvent::GetParam2() const
{
    return m_dwParam2;
}

void UIEvent::SetSender(UIWindow* pSender)
{
    m_pSender = pSender;
}

UIWindow* UIEvent::GetSender() const
{
    return m_pSender;
}

