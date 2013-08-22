#ifndef _DKREADER_UIBOOKSTORECHARTSPAGE_H_
#define _DKREADER_UIBOOKSTORECHARTSPAGE_H_

#include "BookStore/AdInfo.h"
#include "BookStore/BookStoreTypes.h"
#include "BookStoreUI/UIBookCoverView.h"
#include "BookStoreUI/UIBookStoreNavigation.h"
#include "BookStoreUI/UIBookStoreChartsPageItem.h"
#include "I18n/StringManager.h"
#include <vector>

using namespace dk::bookstore;

class UIBookStoreChartsPage : public UIBookStoreNavigation
{
public:
    static const int s_chartsAdsCounts = 4;
    static const int s_chartsAdsRows = 2;
    static const int s_chartsAdsCols = 2;
    static const int s_chartsRows = 4;
    static const int s_chartsCols = 2;

    UIBookStoreChartsPage();
    ~UIBookStoreChartsPage();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreChartsPage";
    }

    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
private:
    bool OnMessageRankAdsUpdate(const EventArgs& args);
    bool OnMessageRankAdsTouch(const EventArgs& args);
    void UpdateRankAds();
    FetchDataResult FetchAds();
    void InitUI();
    virtual UISizer* CreateMidSizer();

private:
    struct ChartItemAttr
    {
        ChartItemAttr(ChartsRankType rankType, SOURCESTRINGID titleId, DWORD cmdId)
            : m_rankType(rankType)
            , m_titleId(titleId)
            , m_commandId(cmdId)
        {}

        ChartsRankType m_rankType;
        SOURCESTRINGID m_titleId;
        DWORD m_commandId;
    };//ChartItemAttr
    UIBookStoreChartsPageItem m_chartItems[CRT_COUNTS];
    UIBookCoverView m_chartAds[s_chartsAdsCounts];
    dk::bookstore::model::AdInfoList m_adsInfoList;
    std::vector<ChartItemAttr> m_vChartItemAttrs;
};
#endif//_DKREADER_UIBOOKSTORECHARTSPAGE_H_

