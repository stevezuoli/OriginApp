#include <tr1/functional>
#include "BookStoreUI/UIBookStorePostAddCartPage.h"
#include "BookStoreUI/UIBookStoreBookChangeLogPage.h"
#include "BookStoreUI/UIBookStoreBookAbstractPage.h"
#include "BookStoreUI/UIBookStoreCartPage.h"
#include "drivers/DeviceInfo.h"
#include "I18n/StringManager.h"
#include "Utility/ImageManager.h"
#include "Utility/PathManager.h"
#include "GUI/CTpGraphics.h"
#include "CommonUI/CPageNavigator.h"
#include "CommonUI/UIUtility.h"
#include "GUI/GUISystem.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "BookStore/BookStoreOrderManager.h"

using namespace WindowsMetrics;
using namespace dk::bookstore;
using namespace dk::utility;	


UIBookStorePostAddCartPage::UIBookStorePostAddCartPage(dk::bookstore::model::BookInfoSPtr bookInfo)
    : UIBookStoreNavigation(bookInfo->GetTitle(), false, false, false)
    , m_relatedBook(2, 4, StringManager::GetStringById(TOUCH_ALSOBUY))
    , m_bookDetailedInfo(bookInfo)
{
    SubscribeMessageEvent(BookStoreOrderManager::EventGetCartUpdate, 
        *BookStoreOrderManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStorePostAddCartPage::OnGetBooksInCartUpdate),
        this,
        std::tr1::placeholders::_1)
        );
    Init();
    UpdateMyBookDetailedInfo();
}

UIBookStorePostAddCartPage::~UIBookStorePostAddCartPage()
{
}

void UIBookStorePostAddCartPage::OnEnter()
{
    FetchInfo();
    UIBookStoreNavigation::OnEnter();
}

bool UIBookStorePostAddCartPage::OnGetBooksInCartUpdate(const EventArgs& args)
{
    const DataUpdateArgs& dataUpdateArgs = (const DataUpdateArgs&)args;
    int booksCount = 0;
    int priceCount = 0;
    if (dataUpdateArgs.succeeded)
    {
        booksCount = dataUpdateArgs.dataList.size();
        if (booksCount > 0)
        {
            for (int i = 0; i < booksCount; ++i)
            {
                model::BookInfo* pBookInfo = dynamic_cast<model::BookInfo*>(dataUpdateArgs.dataList[i].get());
                if (pBookInfo->GetDiscountPrice() < 0)
                    priceCount += pBookInfo->GetPrice();
                else
                    priceCount += pBookInfo->GetDiscountPrice();
            }
        }
    }

    char summaryInfo[256];
    if (booksCount == 0)
    {
	    sprintf(summaryInfo, "%s", StringManager::GetPCSTRById(BOOKSTORE_MY_CART_SUMMARY_EMPTY));
    }
    else
    {
	    sprintf(summaryInfo, StringManager::GetPCSTRById(BOOKSTORE_MY_CART_SUMMARY_DETAILS), booksCount, ((float)priceCount) / 100.00);
    }
    m_bookCountLabel.SetText(summaryInfo);
    UpdateMyBookDetailedInfo();
    Layout();
    Repaint();
    return true;
}

void UIBookStorePostAddCartPage::UpdateMyBookDetailedInfo()
{
    if(0 == m_bookDetailedInfo)
    {
        return;
    }

    m_bookCover.SetCoverUrl(m_bookDetailedInfo->GetCoverUrl());
    m_bookCover.SetVisible(true);

    m_relatedBook.SetBookInfo(m_bookDetailedInfo, true);

#ifndef KINDLE_FOR_TOUCH
    OnChildSetFocus(&m_strollButton);
#endif
}

dk::bookstore::FetchDataResult UIBookStorePostAddCartPage::FetchInfo()
{
    SetFetchStatus(FETCHING);

    FetchDataResult fdr = FDR_FAILED;
    BookStoreOrderManager* manager = BookStoreOrderManager::GetInstance();
    if (manager)
    {
        fdr = manager->FetchBooksInCart();
    }
    else
    {
        SetFetchStatus(FETCH_FAIL);
    }
    return fdr;
}

UISizerFlags UIBookStorePostAddCartPage::GetMiddleSizerFlags()
{
    const int titleLineHeight = GetWindowMetrics(UIBookStoreNavigationTitleBottomLineHeightIndex);
    return UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, m_iLeftMargin).Border(dkBOTTOM, titleLineHeight).Border(dkTOP, 2);
}

UISizer* UIBookStorePostAddCartPage::CreateMidSizer()
{
    UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
    UISizer* topSizer = new UIBoxSizer(dkHORIZONTAL);
    UISizer* infoRightSizer = new UIBoxSizer(dkVERTICAL);
#ifdef KINDLE_FOR_TOUCH
    const int bookInfoMargin = 0;
    UISizer* btnSizer = new UIGridSizer(1, 2, GetWindowMetrics(UIPixelValue16Index), 0);
#else
    const int bookInfoMargin = GetWindowMetrics(UIPixelValue5Index);
    UISizer* btnSizer = new UIBoxSizer(dkHORIZONTAL);
#endif
    UISizer* bookInfoSizer = new UIGridSizer(3, 1, bookInfoMargin, bookInfoMargin);
    if (mainSizer && topSizer && btnSizer && infoRightSizer && bookInfoSizer)
    {
        const int controlSpacing = GetWindowMetrics(UIPixelValue8Index);

        m_bookCover.SetMinSize(dkSize(m_bookCover.GetWidth(), m_bookCover.GetHeight()));

        bookInfoSizer->Add(&m_notificationLabel);
        bookInfoSizer->AddSpacer(controlSpacing);
        bookInfoSizer->Add(&m_bookCountLabel);
        bookInfoSizer->Add(&m_bookStoreLabel);

#ifdef KINDLE_FOR_TOUCH
        btnSizer->Add(&m_strollButton, UISizerFlags(1).Expand().Border(dkRIGHT, controlSpacing));
        btnSizer->Add(&m_gotoCartButton, UISizerFlags(1).Expand().Border(dkLEFT, controlSpacing));
#else
        btnSizer->AddStretchSpacer();
        btnSizer->Add(&m_strollButton);
        btnSizer->Add(&m_gotoCartButton);
#endif

        infoRightSizer->Add(bookInfoSizer, UISizerFlags().Expand());
        infoRightSizer->Add(btnSizer, UISizerFlags(1).Expand().Border(dkTOP, GetWindowMetrics(UIPixelValue3Index)));

        topSizer->Add(&m_bookCover);
        topSizer->Add(infoRightSizer, UISizerFlags(1).Expand().Border(dkLEFT, GetWindowMetrics(UIPixelValue22Index)));

        mainSizer->Add(topSizer, UISizerFlags().Expand().Border(dkTOP | dkBOTTOM, GetWindowMetrics(UIPixelValue16Index)));
        mainSizer->Add(&m_relatedBook,UISizerFlags().Expand().Border(dkBOTTOM, GetWindowMetrics(UIPixelValue13Index)));
        //SetSizer(mainSizer);
    }
    else
    {
        SafeDeletePointer(mainSizer);
        SafeDeletePointer(topSizer);
        SafeDeletePointer(bookInfoSizer);
        SafeDeletePointer(btnSizer);
        SafeDeletePointer(infoRightSizer);
    }
    return mainSizer;
}

void UIBookStorePostAddCartPage::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    switch (dwCmdId)
    {
    case ID_BOOKSTORE_STROLL:
        {
            CPageNavigator::BackToEntryPageOnTypeAsync(PAT_BookStore);
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
    default:
        UIBookStoreNavigation::OnCommand(dwCmdId, pSender, dwParam);
        break;
	}
}

BOOL UIBookStorePostAddCartPage::OnKeyPress(INT32 iKeyCode)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (!SendKeyToChildren(iKeyCode))
    {
        DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s if (!SendKeyToChildren(iKeyCode))", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return FALSE;
    }

    switch (iKeyCode)
    {
    case KEY_UP:
        MoveFocus(DIR_TOP);
        break;
    case KEY_DOWN:
        MoveFocus(DIR_DOWN);
        break;
    default:
        if(SendHotKeyToChildren(iKeyCode))
        {
            DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s default", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            return UIBookStoreNavigation::OnKeyPress(iKeyCode);
        }
        break;
    }
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return FALSE;
}


void UIBookStorePostAddCartPage::Init()
{
    const int noticeSize = GetWindowFontSize(FontSize24Index);
    const int cartInfoSize = GetWindowFontSize(FontSize16Index);
    const int btnFontSize = GetWindowFontSize(FontSize20Index);

    m_bookCover.Initialize(LARGE);
    m_notificationLabel.SetFontSize(noticeSize);
    m_notificationLabel.SetText(StringManager::GetPCSTRById(BOOKSTORE_ALREADY_ADDED_TO_CART));
    m_bookCountLabel.SetFontSize(cartInfoSize);
    m_bookStoreLabel.SetFontSize(cartInfoSize);
    m_bookStoreLabel.SetText(StringManager::GetPCSTRById(BOOKSTORE_CART_PURCHASING_NOTICE));

#ifdef KINDLE_FOR_TOUCH
    m_strollButton.Initialize(ID_BOOKSTORE_STROLL, StringManager::GetStringById(PERSONAL_CART_NO_ELEMENT_STROLL), btnFontSize);
    m_gotoCartButton.Initialize(ID_BTN_BOOK_OPEN_CART, StringManager::GetStringById(PERSONAL_GOTO_CART), btnFontSize);
#else
    m_strollButton.Initialize(ID_BOOKSTORE_STROLL, StringManager::GetStringById(PERSONAL_CART_NO_ELEMENT_STROLL),
                              KEY_RESERVED, bookInfoFontSize, ImageManager::GetImage(IMAGE_RIGHT_ARROW));
    m_gotoCartButton.Initialize(ID_BTN_BOOK_OPEN_CART, StringManager::GetStringById(PERSONAL_GOTO_CART),
                                KEY_RESERVED, bookInfoFontSize, ImageManager::GetImage(IMAGE_RIGHT_ARROW));
#endif
    AppendChild(&m_relatedBook);
    AppendChild(&m_notificationLabel);
    AppendChild(&m_bookCover);
    AppendChild(&m_bookCountLabel);
    AppendChild(&m_bookStoreLabel);
    AppendChild(&m_strollButton);
    AppendChild(&m_gotoCartButton);
}
