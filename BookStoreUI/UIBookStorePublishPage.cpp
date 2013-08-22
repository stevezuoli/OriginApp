#include <tr1/functional>
#include "BookStoreUI/UIBookStorePublishPage.h"
#include "BookStore/DataUpdateArgs.h"
#include "I18n/StringManager.h"
#include "GUI/UISizer.h"
#include "GUI/CTpGraphics.h"
#include "Common/WindowsMetrics.h"

using namespace dk::bookstore;
using namespace WindowsMetrics;

static const int LINEHEIGHT = 1;

UIBookStorePublishPage::UIBookStorePublishPage()
    : UIBookStoreNavigation(StringManager::GetPCSTRById(BOOKSTORE_PUBLISH))
{
    SubscribeMessageEvent(BookStoreInfoManager::EventPublishingListUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStorePublishPage::OnMessageListBoxUpdate),
        this,
        std::tr1::placeholders::_1)
        );
}

UIBookStorePublishPage::~UIBookStorePublishPage()
{
}

UISizer* UIBookStorePublishPage::CreateMidSizer()
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (NULL == m_pMidSizer)
    {
        m_pMidSizer = new UIBoxSizer(dkHORIZONTAL);
        UISizer* pMidSizer = new UIGridSizer(PUBLISHCOUNT / LINE_COUNT, LINE_COUNT, LINEHEIGHT, LINEHEIGHT);
        if (m_pMidSizer && pMidSizer)
        {
            const int minHeight = GetWindowMetrics(UIBookStorePublishingItemHeightIndex);
            for (int i = 0; i < PUBLISHCOUNT; ++i)
            {
                m_itemList[i].SetMinHeight(minHeight);
                AppendChild(&m_itemList[i]);
                pMidSizer->Add(&m_itemList[i], UISizerFlags(1).Expand());
            }
            m_pMidSizer->Add(pMidSizer, UISizerFlags(1).Expand());
        }
        else
        {
            SafeDeletePointer(pMidSizer);
            SafeDeletePointer(m_pMidSizer);
        }
    }
    return m_pMidSizer;
}

FetchDataResult UIBookStorePublishPage::FetchInfo()
{
    SetFetchStatus(FETCHING);
    FetchDataResult fdr = FDR_FAILED;
    BookStoreInfoManager* manager = BookStoreInfoManager::GetInstance();
    if (manager)
    {
        fdr = manager->FetchPublishList(PUBLISHCOUNT);
    }
    else
    {
        SetFetchStatus(FETCH_FAIL);
    }
    return fdr;
}

void UIBookStorePublishPage::SetFetchStatus(FETCH_STATUS status)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s m_fetchStatus = %d, status = %d", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, m_fetchStatus, status);
    if (m_fetchStatus != status)
    {
        SetVisible(status == FETCH_SUCCESS);
        UIBookStoreNavigation::SetFetchStatus(status);
    }
}

bool UIBookStorePublishPage::OnMessageListBoxUpdate(const EventArgs& args)
{
    const DataUpdateArgs& msgArgs = dynamic_cast<const DataUpdateArgs&>(args);
    int i = 0;
    if(msgArgs.succeeded && msgArgs.startIndex == 0)
    {
        std::vector<model::BasicObjectSPtr>::iterator itr = msgArgs.dataList.begin();
        while ((itr != msgArgs.dataList.end()) && (i < PUBLISHCOUNT))
        {
            if ((*itr) && ((*itr)->GetObjectType() == model::OT_PUBLISH))
            {
                m_itemList[i].SetInfoSPtr(*itr);
                i++;
            }
            itr++;
        }
#ifndef KINDLE_FOR_TOUCH
        if (m_itemList[0].IsVisible())
        {
            m_itemList[0].SetFocus(true);
        }
#endif
        UpdateWindow();
    }
    SetFetchStatus(msgArgs.succeeded ? ((i > 0) ? FETCH_SUCCESS : FETCH_NOELEM) : FETCH_FAIL);
    return true;
}

HRESULT UIBookStorePublishPage::DrawBackGround(DK_IMAGE drawingImg)
{
    UIBookStoreNavigation::DrawBackGround(drawingImg);
    if (m_pMidSizer && IsVisible())
    {
        CTpGraphics grf(drawingImg);
        dkSize midSize = m_pMidSizer->GetSize();
        const int left = 0;
        int top = m_itemList[0].GetY();
        const int midLeft = m_itemList[0].GetWidth();
        const int width = m_iWidth;
        const int height = midSize.GetHeight();

        grf.DrawLine(midLeft, top, LINEHEIGHT, height, SOLID_STROKE);
        top += m_itemList[0].GetHeight();
        for (int i = 0; i < (PUBLISHCOUNT - LINE_COUNT); i += LINE_COUNT)
        {
            grf.DrawLine(left, top, width, LINEHEIGHT, SOLID_STROKE);
            top += m_itemList[i].GetHeight() + LINEHEIGHT;
        }
    }
    return S_OK;
}

BOOL UIBookStorePublishPage::SetVisible(BOOL bVisible)
{
    if (m_bIsVisible != bVisible)
    {
        for (int i = 0; i < PUBLISHCOUNT; i++)
        {
            m_itemList[i].SetVisible(bVisible);
        }
    }
    return UIWindow::SetVisible(bVisible);
}

BOOL UIBookStorePublishPage::OnKeyPress(INT32 iKeyCode)
{
#ifndef KINDLE_FOR_TOUCH
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (m_mnuMain.IsVisible())
    {
        return UIBookStorePage::OnKeyPress(iKeyCode);
    }
#endif
    return UIBookStoreNavigation::OnKeyPress(iKeyCode);
}

