#include "BookStoreUI/UIBookStorePublishingItem.h"
#include "BookStoreUI/UIBookStorePublishBooksPage.h"
#include "I18n/StringManager.h"
#include "CommonUI/CPageNavigator.h"
#include "CommonUI/UIUtility.h"
#include "drivers/DeviceInfo.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"

using namespace WindowsMetrics;
using namespace dk::bookstore;

UIBookStorePublishingItem::UIBookStorePublishingItem()
	: UIListItemBase(NULL, IDSTATIC)
{
#ifndef KINDLE_FOR_TOUCH
    SetEnable(true);
#endif
    Init();
}

UIBookStorePublishingItem::~UIBookStorePublishingItem()
{

}

void UIBookStorePublishingItem::Init()
{
    m_cover.Initialize(CUSTOMIZED, IMAGE_BOOKSTORE_DEFAULTPUBLISHCOVER);
    m_cover.ShowBorder(false);
    m_cover.SetMinSize(GetWindowMetrics(UIBookStorePublishingItemCoverWidthIndex), 
        GetWindowMetrics(UIBookStorePublishingItemCoverHeightIndex));
    AppendChild(&m_cover);

    if (!m_windowSizer)
    {
        UISizer* pMainSizer = new UIBoxSizer(dkVERTICAL);
        if (pMainSizer)
        {
            pMainSizer->AddStretchSpacer();
            pMainSizer->Add(&m_cover, UISizerFlags().Align(dkALIGN_CENTER).Border(dkALL, GetWindowMetrics(UIDialogBorderLineIndex)));
            pMainSizer->AddStretchSpacer();
            SetSizer(pMainSizer);
        }
    }
}

void UIBookStorePublishingItem::SetInfoSPtr(dk::bookstore::model::BasicObjectSPtr info, int index)
{
    m_publishInfo.reset((model::AdInfo*)info.get()->Clone());
    if(NULL != m_publishInfo)
    {
        m_cover.SetCoverUrl(m_publishInfo->GetPictureUrl());
        Layout();
    }
}

bool UIBookStorePublishingItem::OnChildClick(UIWindow* child)
{
    OnClick();
    return true;
}

HRESULT UIBookStorePublishingItem::DrawBackGround(DK_IMAGE drawingImg)
{
    CTpGraphics grf(drawingImg);   
    HRESULT hr(S_OK);
    RTN_HR_IF_FAILED(grf.EraserBackGround());
#ifndef KINDLE_FOR_TOUCH
    if (IsFocus())
    {
        UIUtility::DrawBorder(drawingImg, 0, 0, 0, 0, 0, false, true);
    }
#endif
    return hr;
}

#ifdef KINDLE_FOR_TOUCH
void UIBookStorePublishingItem::PerformClick(MoveEvent* moveEvent)
{
    OnClick();
}
#endif

void UIBookStorePublishingItem::OnClick()
{
    if (m_publishInfo)
    {
        UIBookStorePublishBooksPage *page = new UIBookStorePublishBooksPage(m_publishInfo->GetReferenceId(), m_publishInfo->GetName());
        if (page)
        {
            page->MoveWindow(0, 0, DeviceInfo::GetDisplayScreenWidth(), DeviceInfo::GetDisplayScreenHeight());
            CPageNavigator::Goto(page);
        }
    }
}

#ifndef KINDLE_FOR_TOUCH
BOOL UIBookStorePublishingItem::OnKeyPress(INT32 iKeyCode)
{
    switch(iKeyCode)
    {
    case KEY_OKAY:
    case KEY_ENTER:
        if (IsFocus())
        {
            UIEvent clickEvent(CLICK_EVENT, this, GetId());
            OnEvent(clickEvent);
        }
        break;
    default:
        break;
    }
    return true;
}

#endif


