#include <tr1/functional>
#include "BookStoreUI/UIDKBookStoreIndexPage.h"
#include "BookStoreUI/UIBookStoreTopicPage.h"
#include "BookStoreUI/UIBookStoreKeyPage.h"
#include "BookStoreUI/UIBookStoreDuokanRecommendPage.h"
#include "BookStoreUI/UIBookStoreCategoryPage.h"
#include "BookStoreUI/UIBookStoreDiscountPage.h"
#include "BookStoreUI/UIBookStoreChartsPage.h"
#include "BookStoreUI/UIBookStoreLatestPage.h"
#include "BookStoreUI/UIBookStorePublishPage.h"
#include "BookStoreUI/UIBookStoreCartPage.h"
#include "CommonUI/CPageNavigator.h"
#include "CommonUI/UIUtility.h"
#include "CommonUI/UIIMEManager.h"
#include "BookStoreUI/UIBookStoreSearchPage.h"
#include "BookStoreUI/UIBookDetailedInfoPage.h"
#include "I18n/StringManager.h"
#include "GUI/CTpGraphics.h"
#include "GUI/GUISystem.h"
#include "GUI/UISizer.h"
#include "GUI/UIMessageBox.h"
#include "BookStore/BookStoreInfoManager.h"
#include "BookStore/BookStoreOrderManager.h"
#include "BookStore/OfflineBookStoreManager.h"
#include "DownloadManager/DownloadManager.h"
#include "BookStore/DataUpdateArgs.h"
#include "Framework/CDisplay.h"
#include "Common/WindowsMetrics.h"
#include "drivers/DeviceInfo.h"
#include "Common/CAccountManager.h"

using namespace dk::bookstore;
using namespace std;
using namespace WindowsMetrics;
using namespace dk::account;

UIDKBookStoreIndexPage::UIDKBookStoreIndexPage()
#ifndef KINDLE_FOR_TOUCH
    : UIBookStorePage()
#endif
{
    SubscribeMessageEvent(BookStoreInfoManager::EventAdsListUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIDKBookStoreIndexPage::OnMessageTopicUpdate),
        this,
        std::tr1::placeholders::_1)
        );

    SubscribeMessageEvent(BookStoreOrderManager::EventGetCartUpdate, 
        *BookStoreOrderManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIDKBookStoreIndexPage::OnGetBooksInCartUpdate),
        this,
        std::tr1::placeholders::_1)
        );

    for (int i = 0; i < TOPICCOUNT; i++ )
    {
#ifdef KINDLE_FOR_TOUCH
        SubscribeEvent(UIWindow::EventClick,
#else
		SubscribeEvent(UIBookCoverView::EventBookCoverViewTouched,
#endif
            m_topicCoverList[i],
            std::tr1::bind(
            std::tr1::mem_fn(&UIDKBookStoreIndexPage::OnMessageTopicTouch),
            this,
            std::tr1::placeholders::_1)
            );
    }

    Init();
}

UIDKBookStoreIndexPage::~UIDKBookStoreIndexPage()
{
}

dk::bookstore::FetchDataResult UIDKBookStoreIndexPage::FetchSpecialEvents()
{
    return BookStoreOrderManager::GetInstance()->FetchSpecialEvents();
}

FetchDataResult UIDKBookStoreIndexPage::FetchAds()
{
    BookStoreInfoManager*pManager = BookStoreInfoManager::GetInstance();
    FetchDataResult fdr = pManager->FetchAds();
    return fdr;
}

bool UIDKBookStoreIndexPage::OnMessageTopicUpdate(const EventArgs& args)
{
    const DataUpdateArgs& msgArgs = dynamic_cast<const DataUpdateArgs&>(args);
    m_topicInfoList.clear();
    if(msgArgs.succeeded && msgArgs.startIndex == 0)
    {
        int adsCount = msgArgs.dataList.size();
        if (adsCount > TOPICCOUNT)
        {
            adsCount = TOPICCOUNT;
        }

        for (int i = 0; i < adsCount; ++i)
        {
            dk::bookstore::model::AdInfo* adInfo = (dk::bookstore::model::AdInfo*)msgArgs.dataList[i]->Clone();
            m_topicInfoList.push_back(model::AdInfoSPtr(adInfo));
        }
        UpdateTopicCoverList();
        return true;
    }
    return false;
}

static bool GetCartTextArea(DK_RECT& area)
{
    if (DeviceInfo::IsKPW())
    {
        area.left = 28;
        area.top = 3;
        area.right = 55;
        area.bottom = 30;
    }
    else
    {
        area.left = 23;
        area.top = 2;
        area.right = 45;
        area.bottom = 24;   
    }
    return true;
}

bool UIDKBookStoreIndexPage::OnGetBooksInCartUpdate(const EventArgs& args)
{
    const DataUpdateArgs& dataUpdateArgs = (const DataUpdateArgs&)args;
    if (dataUpdateArgs.succeeded)
    {
        int count = dataUpdateArgs.dataList.size();
        if (count > 0)
        {
            char buf[32] = {0};
            if (count < 100)
            {
                sprintf(buf, "%d", count);
            }
            else
            {
                sprintf(buf, "%s", "N");
            }
            m_btnCart.SetText(buf);

            DK_RECT txt_area;
            GetCartTextArea(txt_area);
            m_btnCart.SetCustomizedIcons(ImageManager::GetImage(IMAGE_BOOKSTORE_CART),
                                         ImageManager::GetImage(IMAGE_BOOKSTORE_CART),
                                         txt_area);
        }
        else
        {
            m_btnCart.SetIcons(ImageManager::GetImage(IMAGE_BOOKSTORE_CART_EMPTY),
                               ImageManager::GetImage(IMAGE_BOOKSTORE_CART_EMPTY),
                               UIButton::ICON_CENTER);
        }
        CDisplay* display = CDisplay::GetDisplay();
        if(display)
        {
            DK_IMAGE pGraphics = display->GetMemDC();
            Draw(pGraphics);
            display->StopFullRepaintOnce();
            Repaint();
        }
    }
    return true;
}

bool UIDKBookStoreIndexPage::OnMessageTopicTouch(const EventArgs& args)
{
    if(!UIUtility::CheckAndReConnectWifi())
    {
        return false;
    }
#ifdef KINDLE_FOR_TOUCH
    const ClickArgs& clickArgs = (const ClickArgs&)(args);
    UIBookCoverView* coverView = (UIBookCoverView*)clickArgs.window;
    if (0 == m_topicInfoList.size() || !coverView)
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
    int itemCount = (int)m_topicInfoList.size() < TOPICCOUNT ? m_topicInfoList.size() : TOPICCOUNT;
    for (int i = 0;i < itemCount; i++)
    {
        if(m_topicInfoList[i]->GetPictureUrl() == coverUrl)
        {
            CDisplay *pDisplay = CDisplay::GetDisplay();
            model::ObjectType type = m_topicInfoList[i]->GetReferenceType();
            UIPage *pPage = NULL;
            if (type == model::OT_BOOK)
            {
                pPage = new UIBookDetailedInfoPage(m_topicInfoList[i]->GetReferenceId(),m_topicInfoList[i]->GetName());
            }
            else if(type == model::OT_TOPIC)
            {
                string referenceId = m_topicInfoList[i]->GetReferenceId();
                if (!referenceId.empty())
                {
                    int topicId = atoi(referenceId.c_str());
                    pPage = new UIBookStoreTopicPage(topicId, m_topicInfoList[i]->GetName());
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


void UIDKBookStoreIndexPage::UpdateTopicCoverList()
{
    int i = 0;
    for(; i < (int)m_topicInfoList.size(); i++)
    {
        string url = m_topicInfoList[i]->GetPictureUrl();
        m_topicCoverList[i].SetCoverUrl(url);
        m_topicCoverList[i].SetVisible(TRUE);
    }
    for(; i < TOPICCOUNT; i++)
    {
        m_topicCoverList[i].SetVisible(FALSE);
    }
#ifndef KINDLE_FOR_TOUCH
    if (m_topicCoverList[0].IsVisible() && (m_iFocusedChild < 0))
    {
        m_topicCoverList[0].SetFocus(true);
    }
#endif
    UpdateWindow();
}

void UIDKBookStoreIndexPage::Init()
{
#ifdef KINDLE_FOR_TOUCH
    AppendChild(&m_titleBar);
    m_searchBox.SetImage(IMAGE_ICON_SEARCH_DISABLE, IMAGE_ICON_SEARCH);

    m_btnCart.Initialize(ID_BTN_BOOK_OPEN_CART, "", GetWindowFontSize(FontSize16Index));
    m_btnCart.SetIcons(ImageManager::GetImage(IMAGE_BOOKSTORE_CART_EMPTY),
                       ImageManager::GetImage(IMAGE_BOOKSTORE_CART_EMPTY),
                       UIButton::ICON_CENTER);
    m_btnCart.SetFontColor(ColorManager::knWhite);
    m_btnCart.SetUpdateOnPress(false);
    m_btnCart.SetUpdateOnUnPress(false);
    m_btnCart.ShowBorder(false);

    m_btnSendDuokanKey.Initialize(ID_BTN_BOOK_DUOKANKEY_ENTRY, "", GetWindowFontSize(FontSize18Index));
    m_btnSendDuokanKey.SetIcons(ImageManager::GetImage(IMAGE_BOOKSTORE_EXCHANGE),
                                ImageManager::GetImage(IMAGE_BOOKSTORE_EXCHANGE),
                                UIButton::ICON_CENTER);
    m_btnSendDuokanKey.ShowBorder(false);
#else
    m_btnSendDuokanKey.Initialize(ID_BTN_BOOK_DUOKANKEY_ENTRY, StringManager::GetStringById(BOOKSTORE_BOOK_DUOKANKEY_ENTRY), KEY_RESERVED, GetWindowFontSize(FontSize18Index), ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_btnSendDuokanKey.SetVAlign(ALIGN_CENTER);
    m_btnSendDuokanKey.SetRightIcon(ImageManager::GetImage(IMAGE_ARROW_STRONG_RIGHT));
#endif
    m_btnCart.SetLeftMargin(0);
    m_btnSendDuokanKey.SetLeftMargin(0);
    m_orderButtonsGroup.AddButton(&m_btnSendDuokanKey, UISizerFlags(1).Bottom().Border(dkBOTTOM, GetWindowMetrics(UIPixelValue16Index)));
    m_orderButtonsGroup.AddButton(&m_btnCart,
        UISizerFlags(1).Bottom().Border(dkLEFT, GetWindowMetrics(UIPixelValue3Index)).Border(dkBOTTOM, GetWindowMetrics(UIPixelValue16Index)));
    
    m_orderButtonsGroup.SetMinHeight(GetWindowMetrics(UIAddCategoryDlgBtnGroupHeightIndex));
    m_orderButtonsGroup.SetSplitLineHeight(GetWindowMetrics(UIAddCategoryDlgBtnHeightIndex) - (GetWindowMetrics(UIButtonGroupSplitLineTopPaddingIndex) << 1));

    m_searchBox.SetId(ID_EDIT_SEARCH_BOOK);
    m_searchBox.SetFontSize(GetWindowFontSize(FontSize20Index));
    m_searchBox.SetTipUTF8(StringManager::GetStringById(BOOK_NAME_KEY_WORD));

    //AppendChild(&m_btnCart);
    //AppendChild(&m_btnSendDuokanKey);
    AppendChild(&m_orderButtonsGroup);
#ifdef KINDLE_FOR_TOUCH
    UIIMEManager::GetIME(IUIIME_CHINESE_LOWER, &m_searchBox);
#endif
    for(int i = 0; i < TOPICROWCOUNT; i++)
    {
        for(int j = 0;j < TOPICLINECOUNT; j++)
        {
            int item = i * TOPICLINECOUNT + j;
            m_topicCoverList[item].Initialize(CUSTOMIZED, IMAGE_BOOKSTORE_DEFAULTADCOVER);
            m_topicCoverList[i].SetMinSize(dkSize(GetWindowMetrics(UIDKBookStoreIndexPageItemWidthIndex), 
                GetWindowMetrics(UIDKBookStoreIndexPageCoverHeightIndex)));
            AppendChild(&m_topicCoverList[item]);
        }
    }

    const SOURCESTRINGID titelId[CHANNEL_COUNT] = 
    {
        BOOKSTORE_NEWBOOK, 
        BOOKSTORE_RECOMMEND,  
        BOOKSTORE_CHARTS, 
        BOOKSTORE_DISCOUNT,
        BOOKSTORE_CATEGORYVIEW,
        BOOKSTORE_PUBLISH
    };
	const SOURCESTRINGID abstructId[CHANNEL_COUNT] = 
    {
        BOOKSTORE_NEWBOOK_ABUSTRUCT, 
        BOOKSTORE_RECOMMEND_ABUSTRUCT, 
        BOOKSTORE_CHARTS_ABUSTRUCT, 
        BOOKSTORE_DISCOUNT_ABUSTRUCT, 
        BOOKSTORE_CATEGORY_ABUSTRUCT, 
        BOOKSTORE_PUBLISH_ABUSTRUCT
    };
    const DWORD dwItemId[CHANNEL_COUNT] = 
    {
        ID_BOOKSTORE_NEWBOOKS, 
        ID_BOOKSTORE_RECOMMEND, 
        ID_BOOKSTORE_CHARTS, 
        ID_BOOKSTORE_DISCOUNT, 
        ID_BOOKSTORE_CATEGORY, 
        ID_BOOKSTORE_BOOKRIGHTS
    };
    for (int i = 0; i < CHANNEL_COUNT; i++)
    {
        m_itemList[i].SetTopMargin(GetWindowMetrics(UIDKBookStoreIndexPageItemTopMarginIndex));
        m_itemList[i].SetMinSize(dkSize(GetWindowMetrics(UIDKBookStoreIndexPageItemWidthIndex), 
            GetWindowMetrics(UIDKBookStoreIndexPageItemHeightIndex)));
        m_itemList[i].InitItem(dwItemId[i], StringManager::GetPCSTRById(titelId[i]), StringManager::GetPCSTRById(abstructId[i]));
        AppendChild(&m_itemList[i]);
    }

    AppendChild(&m_searchBox);
    if (!m_windowSizer)
    {
        const int searchBoxHeight = GetWindowMetrics(UIPixelValue48Index);
        const int searchBarHeight = GetWindowMetrics(UIPixelValue80Index);
        const int horizonItemMargin = GetWindowMetrics(UIPixelValue12Index);
        const int horizonMargin = GetWindowMetrics(UIHorizonMarginIndex);

        m_searchBox.SetMinHeight(searchBoxHeight);//?????????
        UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);//?sizer????????

        UISizer* topSizer = new UIBoxSizer(dkHORIZONTAL);//???????????
        topSizer->SetMinHeight(searchBarHeight);//??????????
        //m_btnSendDuokanKey.SetMinSize(dkSize(m_btnSendDuokanKey.GetMinWidth(), searchBarHeight));
        topSizer->Add(&m_searchBox, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
        //topSizer->Add(&m_btnCart, UISizerFlags().Border(dkLEFT, horizonMargin).Align(dkALIGN_CENTER_VERTICAL));
        //topSizer->Add(&m_btnSendDuokanKey, UISizerFlags().Border(dkLEFT, horizonMargin).Align(dkALIGN_CENTER_VERTICAL));
        topSizer->Add(&m_orderButtonsGroup, 
            UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, 0).Border(dkTOP | dkBOTTOM, 0));
        m_orderButtonsGroup.SetTopLinePixel(0);
        m_orderButtonsGroup.SetBottomLinePixel(0);

        //UIGridSizer for covers
        UISizer* coverSizer = new UIGridSizer(TOPICROWCOUNT, TOPICLINECOUNT, horizonItemMargin, horizonItemMargin);
        for ( int i=0; i<TOPICCOUNT; ++i)
        {
            //m_topicCoverList[i].SetMinHeight(coverHeight);//?????????
            coverSizer->Add(m_topicCoverList+i, UISizerFlags(1).Expand());//expand()?????????,proportion?1,????item??1:1????????
        }

        UISizer* itemSizer = new UIGridSizer(CHANNEL_COUNT/CHANNEL_LINE_COUNT, 
            CHANNEL_LINE_COUNT, 
#ifdef KINDLE_FOR_TOUCH
            0, 
#else
            horizonItemMargin,
#endif
            horizonItemMargin);
        for ( int i=0; i<CHANNEL_COUNT; ++i)
        {
            itemSizer->Add(m_itemList+i, UISizerFlags(1).Expand());
        }

        mainSizer->Add(&m_titleBar);
        const int pixelValue8 = GetWindowMetrics(UIPixelValue8Index);
        mainSizer->Add(topSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizonMargin).Border(dkTOP | dkBOTTOM, pixelValue8));
        mainSizer->Add(coverSizer, UISizerFlags().Border(dkLEFT | dkRIGHT, horizonMargin));
        mainSizer->Add(itemSizer, UISizerFlags(1).Border(dkLEFT | dkRIGHT, horizonMargin).Border(dkTOP | dkBOTTOM, pixelValue8));
        //mainSizer->Add(itemSizer, UISizerFlags(1).Border(dkLEFT | dkRIGHT, horizonMargin).Align(dkALIGN_CENTER_VERTICAL));

#ifdef KINDLE_FOR_TOUCH
        mainSizer->Add(UIBottomBar::GetInstance());
        AppendChild(m_pBottomBar);
#else
        mainSizer->AddSpacer(pixelValue8);
#endif
        SetSizer(mainSizer);
        Layout();
    }
}

void UIDKBookStoreIndexPage::OnEnter()
{
    FetchSpecialEvents();
    FetchAds();
    FetchBooksInCart();
}

dk::bookstore::FetchDataResult UIDKBookStoreIndexPage::FetchBooksInCart()
{
    return BookStoreOrderManager::GetInstance()->FetchBooksInCart();
}

void UIDKBookStoreIndexPage::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    switch (dwCmdId)
    {
    case ID_EDIT_SEARCH_BOOK:
    case ID_BTN_SEARCH:
        if (m_searchBox.IsVisible() && UIUtility::CheckAndReConnectWifi())
        {
            string searchKey = m_searchBox.GetTextUTF8();
            UIBookStoreSearchPage *pPage = new UIBookStoreSearchPage(searchKey);
            if (pPage)
            {
                CDisplay::GetDisplay()->SetFullRepaint(true);
                pPage->MoveWindow(0, 0, DeviceInfo::GetDisplayScreenWidth(), DeviceInfo::GetDisplayScreenHeight());
                CPageNavigator::Goto(pPage);
            }
        }
        break;
    case ID_BTN_BOOK_DUOKANKEY_ENTRY:
        {
            if (UIUtility::CheckAndReLogin())
            {
                UIBookStoreKeyPage *pPage = new UIBookStoreKeyPage();
                if (pPage)
                {
                    pPage->MoveWindow(0, 0, DeviceInfo::GetDisplayScreenWidth(), DeviceInfo::GetDisplayScreenHeight());
                    CPageNavigator::Goto(pPage);
                }
            }
        }
        break;
    case ID_BTN_BOOK_OPEN_CART:
        {
            if (UIUtility::CheckAndReConnectWifi())
            {
                UIBookStoreCartPage *pPage = new UIBookStoreCartPage();
                if (pPage)
                {
                    pPage->MoveWindow(0, 0, DeviceInfo::GetDisplayScreenWidth(), DeviceInfo::GetDisplayScreenHeight());
                    CPageNavigator::Goto(pPage);
                }
            }
        }
        break;
    case ID_BOOKSTORE_RECOMMEND:
        if (UIUtility::CheckAndReConnectWifi())
        {
            CDisplay::GetDisplay()->SetFullRepaint(true);
            UIBookStoreDuokanRecommendPage *pRecommendPage = new UIBookStoreDuokanRecommendPage();
            pRecommendPage->MoveWindow(0,0,CDisplay::GetDisplay()->GetScreenWidth(),CDisplay::GetDisplay()->GetScreenHeight());
            CPageNavigator::Goto(pRecommendPage);
        }
        break;
    case ID_BOOKSTORE_CATEGORY:
        if (UIUtility::CheckAndReConnectWifi())
        {
            CDisplay::GetDisplay()->SetFullRepaint(true);
            UIBookStoreCategoryPage *pcategoryPage = new UIBookStoreCategoryPage();
            pcategoryPage->MoveWindow(0,0,CDisplay::GetDisplay()->GetScreenWidth(),CDisplay::GetDisplay()->GetScreenHeight());
            CPageNavigator::Goto(pcategoryPage);
        }
        break;
    case ID_BOOKSTORE_DISCOUNT:
        if (UIUtility::CheckAndReConnectWifi())
        {
            CDisplay::GetDisplay()->SetFullRepaint(true);
            UIBookStoreDiscountPage *discountPage = new UIBookStoreDiscountPage();
            discountPage->MoveWindow(0,0,CDisplay::GetDisplay()->GetScreenWidth(),CDisplay::GetDisplay()->GetScreenHeight());
            CPageNavigator::Goto(discountPage);
        }
        break;
    case ID_BOOKSTORE_CHARTS:
        if (UIUtility::CheckAndReConnectWifi())
        {
            CDisplay::GetDisplay()->SetFullRepaint(true);
            UIBookStoreChartsPage *pChartsPage = new UIBookStoreChartsPage();
            pChartsPage->MoveWindow(0,0,CDisplay::GetDisplay()->GetScreenWidth(),CDisplay::GetDisplay()->GetScreenHeight());
            CPageNavigator::Goto(pChartsPage);
        }
        break;
    case ID_BOOKSTORE_NEWBOOKS:
        if (UIUtility::CheckAndReConnectWifi())
        {
            CDisplay::GetDisplay()->SetFullRepaint(true);
            UIBookStoreLatestPage *pLatestPage = new UIBookStoreLatestPage();
            pLatestPage->MoveWindow(0,0,CDisplay::GetDisplay()->GetScreenWidth(),CDisplay::GetDisplay()->GetScreenHeight());
            CPageNavigator::Goto(pLatestPage);
        }
        break;
    case ID_BOOKSTORE_BOOKRIGHTS:
        if (UIUtility::CheckAndReConnectWifi())
        {
            CDisplay::GetDisplay()->SetFullRepaint(true);
            UIBookStorePublishPage *pPublishPage = new UIBookStorePublishPage();
            pPublishPage->MoveWindow(0,0,CDisplay::GetDisplay()->GetScreenWidth(),CDisplay::GetDisplay()->GetScreenHeight());
            CPageNavigator::Goto(pPublishPage);
        }
        break;
    default:
#ifndef KINDLE_FOR_TOUCH
        UIBookStorePage::OnCommand(dwCmdId, pSender, dwParam);
#endif
        break;
    }
}

#ifndef KINDLE_FOR_TOUCH
HRESULT UIDKBookStoreIndexPage::DrawBackGround(DK_IMAGE drawingImg)
{
    UIPage::DrawBackGround(drawingImg);
    CTpGraphics grf(drawingImg);
    HRESULT hr(S_OK);
    const int lineTop = m_title.GetHeight();
    const int lineHeight = 2;
    grf.FillRect(0, lineTop, m_iWidth, lineTop + lineHeight, ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
    return hr;
}

void UIDKBookStoreIndexPage::OnClick()
{
    if (m_searchBox.IsFocus())
    {
        OnCommand(ID_BTN_SEARCH, this, 0);
    }
}

#endif
