#include <tr1/functional>
#include "BookStoreUI/UIBookDetailedInfoPage.h"
#include "BookStoreUI/UIBookStoreTopicPage.h"
#include "BookStoreUI/UIBookStoreChartsPage.h"
#include "BookStoreUI/UIBookStoreChartsListBoxPage.h"
#include "BookStore/DataUpdateArgs.h"
#include "Framework/CDisplay.h"
#include "CommonUI/CPageNavigator.h"
#include "CommonUI/UIUtility.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"

using namespace dk::bookstore;
using namespace WindowsMetrics;

UIBookStoreChartsPage::UIBookStoreChartsPage()
    : UIBookStoreNavigation(StringManager::GetPCSTRById(BOOKSTORE_RANKING))
{
    SubscribeMessageEvent(BookStoreInfoManager::EventRankAdsListUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreChartsPage::OnMessageRankAdsUpdate),
        this,
        std::tr1::placeholders::_1)
        );

    for (int i = 0; i < s_chartsAdsCounts; i++ )
    {
#ifdef KINDLE_FOR_TOUCH
        SubscribeEvent(UIWindow::EventClick,
#else
        SubscribeEvent(UIBookCoverView::EventBookCoverViewTouched,
#endif
            m_chartAds[i],
            std::tr1::bind(
            std::tr1::mem_fn(&UIBookStoreChartsPage::OnMessageRankAdsTouch),
            this,
            std::tr1::placeholders::_1)
            );
    }
}

UIBookStoreChartsPage::~UIBookStoreChartsPage()
{
}

bool UIBookStoreChartsPage::OnMessageRankAdsUpdate(const EventArgs& args)
{
    const DataUpdateArgs& msgArgs = dynamic_cast<const DataUpdateArgs&>(args);
    m_adsInfoList.clear();
    if(msgArgs.succeeded && msgArgs.startIndex == 0)
    {
        int adsCount = msgArgs.dataList.size();
        if (adsCount > s_chartsAdsCounts)
        {
            adsCount = s_chartsAdsCounts;
        }

        for (int i = 0; i < adsCount; ++i)
        {
            dk::bookstore::model::AdInfo* adInfo = (dk::bookstore::model::AdInfo*)msgArgs.dataList[i]->Clone();
            m_adsInfoList.push_back(model::AdInfoSPtr(adInfo));
        }
        UpdateRankAds();
        return true;
    }
    return false;
}

void UIBookStoreChartsPage::UpdateRankAds()
{
    int i = 0;
    for(; i < (int)m_adsInfoList.size(); i++)
    {
        string url = m_adsInfoList[i]->GetPictureUrl();
        m_chartAds[i].SetCoverUrl(url);
        m_chartAds[i].SetVisible(TRUE);
    }
    for(; i < s_chartsAdsCounts; i++)
    {
        m_chartAds[i].SetVisible(FALSE);
    }
#ifndef KINDLE_FOR_TOUCH
    if (m_chartAds[0].IsVisible())
    {
        m_chartAds[0].SetFocus(true);
    }
#endif
    Layout();
    UpdateWindow();
}

bool UIBookStoreChartsPage::OnMessageRankAdsTouch(const EventArgs& args)
{
    if(!UIUtility::CheckAndReConnectWifi())
    {
        return false;
    }
#ifdef KINDLE_FOR_TOUCH
    const ClickArgs& clickArgs = (const ClickArgs&)(args);
    UIBookCoverView* coverView = (UIBookCoverView*)clickArgs.window;
    if (0 == m_adsInfoList.size() || !coverView)
    {
        FetchAds();
        return true;
    }
    const string& coverUrl = coverView->GetCoverUrl();
#else   
    const BookCoverViewTouchedEventArgs& msgArgs = dynamic_cast<const BookCoverViewTouchedEventArgs&>(args);
    if (msgArgs.m_coverUrl.empty())
    {
        return false;
    }
    const string& coverUrl = msgArgs.m_coverUrl;
#endif
    int itemCount = (int)m_adsInfoList.size() < s_chartsAdsCounts ? m_adsInfoList.size() : s_chartsAdsCounts;
    for (int i = 0;i < itemCount; i++)
    {
        if(m_adsInfoList[i]->GetPictureUrl() == coverUrl)
        {
            CDisplay *pDisplay = CDisplay::GetDisplay();
            model::ObjectType type = m_adsInfoList[i]->GetReferenceType();
            UIPage *pPage = NULL;
            if (type == model::OT_BOOK)
            {
                pPage = new UIBookDetailedInfoPage(m_adsInfoList[i]->GetReferenceId(),m_adsInfoList[i]->GetName());
            }
            else if(type == model::OT_TOPIC)
            {
                string referenceId = m_adsInfoList[i]->GetReferenceId();
                if (!referenceId.empty())
                {
                    int topicId = atoi(referenceId.c_str());
                    pPage = new UIBookStoreTopicPage(topicId, m_adsInfoList[i]->GetName());
                }
            }
            if (pPage && pDisplay)
            {
                CDisplay::GetDisplay()->SetFullRepaint(true);
                pPage->MoveWindow(0,0,pDisplay->GetScreenWidth(),pDisplay->GetScreenHeight());
                CPageNavigator::Goto(pPage);
                return true;
            }
        }
    }
    return false;
}

void UIBookStoreChartsPage::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    switch (dwCmdId)
    {
        case ID_BIN_BOOKSTORE_CHARTS_PAY:
        case ID_BIN_BOOKSTORE_CHARTS_MONTHLY:
        case ID_BIN_BOOKSTORE_CHARTS_FREE:
        case ID_BIN_BOOKSTORE_CHARTS_FAVOR:
        case ID_BIN_BOOKSTORE_CHARTS_DOUBAN:
        case ID_BIN_BOOKSTORE_CHARTS_DANGDANG:
        case ID_BIN_BOOKSTORE_CHARTS_AMAZON:
        case ID_BIN_BOOKSTORE_CHARTS_JINGDONG:
            {
                for (unsigned int i = 0; i < m_vChartItemAttrs.size(); ++i)
                {
                    if (m_vChartItemAttrs[i].m_commandId == dwCmdId)
                    {
                        UIBookStoreChartsListBoxPage* pPage = 
                            new UIBookStoreChartsListBoxPage(StringManager::GetPCSTRById(m_vChartItemAttrs[i].m_titleId), m_vChartItemAttrs[i].m_rankType);
                        pPage->MoveWindow(0,0,CDisplay::GetDisplay()->GetScreenWidth(),CDisplay::GetDisplay()->GetScreenHeight());
                        CPageNavigator::Goto(pPage);
                        return;
                    }
                }
            }
            break;
        default:
            UIBookStoreNavigation::OnCommand(dwCmdId, pSender, dwParam);
            break;
    }
}

void UIBookStoreChartsPage::InitUI()
{
    m_vChartItemAttrs.clear();
    
    m_vChartItemAttrs.push_back(ChartItemAttr(CRT_PAY, BOOKSTORE_PAYCHARTS, ID_BIN_BOOKSTORE_CHARTS_PAY));
    m_vChartItemAttrs.push_back(ChartItemAttr(CRT_MONTHLY, BOOKSTORE_MONTHLYPAY, ID_BIN_BOOKSTORE_CHARTS_MONTHLY));
    m_vChartItemAttrs.push_back(ChartItemAttr(CRT_FAVOURITE, BOOKSTORE_FAVOURPAY, ID_BIN_BOOKSTORE_CHARTS_FAVOR));
    m_vChartItemAttrs.push_back(ChartItemAttr(CRT_FREE, BOOKSTORE_FREECHARTS, ID_BIN_BOOKSTORE_CHARTS_FREE));
    m_vChartItemAttrs.push_back(ChartItemAttr(CRT_DOUBAN, BOOKSTORE_RANKING_DOUBAN, ID_BIN_BOOKSTORE_CHARTS_DOUBAN));
    m_vChartItemAttrs.push_back(ChartItemAttr(CRT_DANGDANG, BOOKSTORE_RANKING_DANGDANG, ID_BIN_BOOKSTORE_CHARTS_DANGDANG));
    m_vChartItemAttrs.push_back(ChartItemAttr(CRT_AMAZON, BOOKSTORE_RANKING_AMAZON, ID_BIN_BOOKSTORE_CHARTS_AMAZON));
    m_vChartItemAttrs.push_back(ChartItemAttr(CRT_JINGDONG, BOOKSTORE_RANKING_JINGDONG, ID_BIN_BOOKSTORE_CHARTS_JINGDONG));

    for (unsigned int i = 0; i < m_vChartItemAttrs.size(); ++i)
    {
        m_chartItems[i].SetTopMargin(GetWindowMetrics(UIDKBookStoreIndexPageItemTopMarginIndex));
        m_chartItems[i].SetMinSize(dkSize(GetWindowMetrics(UIBookStoreChartsPageItemWidthIndex), 
                    GetWindowMetrics(UIBookStoreChartsPageItemHeightIndex)));
        m_chartItems[i].InitItem(m_vChartItemAttrs[i].m_commandId, StringManager::GetPCSTRById(m_vChartItemAttrs[i].m_titleId));
    }

    m_bShowFetchInfo = false;

    FetchAds();
}

UISizer* UIBookStoreChartsPage::CreateMidSizer()
{
    InitUI();

    const int horizonItemMargin = GetWindowMetrics(UIPixelValue12Index);
    UISizer* adsSizer = new UIGridSizer(s_chartsAdsRows, s_chartsAdsCols, horizonItemMargin, horizonItemMargin);
    for (int i = 0; i < s_chartsAdsRows * s_chartsAdsCols; ++i)
        {
            m_chartAds[i].Initialize(CUSTOMIZED, IMAGE_BOOKSTORE_DEFAULTADCOVER);
            m_chartAds[i].SetMinSize(dkSize(GetWindowMetrics(UIDKBookStoreIndexPageItemWidthIndex), 
                GetWindowMetrics(UIDKBookStoreIndexPageCoverHeightIndex)));
            adsSizer->Add(m_chartAds + i, UISizerFlags().Expand().ReserveSpaceEvenIfHidden());
            AppendChild(m_chartAds + i);
        }

    UISizer* chartsSizer = new UIGridSizer(s_chartsRows, s_chartsCols, horizonItemMargin, horizonItemMargin);

    for (int i = 0; i < CRT_COUNTS; ++i)
        {
            chartsSizer->Add(m_chartItems + i, UISizerFlags().Expand());
            AppendChild(m_chartItems + i);
        }

    if (NULL == m_pMidSizer)
    {
        m_pMidSizer = new UIBoxSizer(dkVERTICAL);
        m_pMidSizer->AddSpacer(GetWindowMetrics(UIBookStoreChartsPageTopSpaceIndex));
        m_pMidSizer->Add(adsSizer, UISizerFlags().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIHorizonMarginIndex)));
        m_pMidSizer->AddSpacer(GetWindowMetrics(UIBookStoreChartsPageMiddleSpaceIndex));
        m_pMidSizer->Add(chartsSizer, UISizerFlags().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIHorizonMarginIndex)));
    }

    return m_pMidSizer;
}

FetchDataResult UIBookStoreChartsPage::FetchAds()
{
    FetchDataResult fdr = FDR_FAILED;
    BookStoreInfoManager* manager = BookStoreInfoManager::GetInstance();
    if (manager)
    {
        fdr = manager->FetchRankAds();
    }
    return fdr;
}

