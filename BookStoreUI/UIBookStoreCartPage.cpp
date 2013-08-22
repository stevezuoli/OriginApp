#include <tr1/functional>
#include "BookStoreUI/UIBookStoreCartPage.h"
#include "Common/CAccountManager.h"
#include "BookStore/DataUpdateArgs.h"
#include "BookStore/BookStoreOrderManager.h"
#include "BookStore/OfflineBookStoreManager.h"
#include "BookStore/SpecialEvent.h"
#include "CommonUI/UIUtility.h"
#include "CommonUI/CPageNavigator.h"
#include "I18n/StringManager.h"
#include "GUI/UISizer.h"
#ifdef KINDLE_FOR_TOUCH
#include "GUI/GUISystem.h"
#endif
#include "Common/WindowsMetrics.h"
#include "Framework/CDisplay.h"
#include "BookStoreUI/UIBookStoreListBoxWithBtn.h"
#include "BookStoreUI/UIBookStoreBookItem.h"
#include "BookStoreUI/UIBookStoreBookInCartItem.h"
#include "BookStoreUI/UIBookStorePayDlg.h"
#include "BookStoreUI/UIBookStorePayInfoDlg.h"
#include "BookStoreUI/UIBookDetailedInfoPage.h"
#include "PersonalUI/UIPersonalFavouritesPage.h"
#include "TouchAppUI/UIBookMenuDlg.h"
#include "drivers/DeviceInfo.h"

using namespace dk::bookstore;
using namespace dk::account;
using namespace WindowsMetrics;
using namespace dk::bookstore::model;


UICartPageBottomBar::UICartPageBottomBar()
    : UIButtonGroup(NULL)
{
    SetBtnsDirection(BGD_Vertical);
    SetDisplaySpliteLine(false);
    InitUI();

    SubscribeMessageEvent(BookStoreOrderManager::EventGetCartUpdate, 
        *BookStoreOrderManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UICartPageBottomBar::OnGetBooksInCartUpdate),
        this,
        std::tr1::placeholders::_1)
        );
    SubscribeMessageEvent(BookStoreOrderManager::EventGetSpecialEventUpdate, 
        *BookStoreOrderManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UICartPageBottomBar::OnGetSpecialEventsUpdate),
        this,
        std::tr1::placeholders::_1)
        );
}

UICartPageBottomBar::~UICartPageBottomBar()
{
}

static bool GetDiscountInfo(const DataUpdateArgs& args, int& originPriceCount, int& discountPriceCount)
{
    int booksCount = args.dataList.size();
    discountPriceCount = 0;
    originPriceCount = 0;
    if (booksCount > 0)
    {
        for (int i = 0; i < booksCount; ++i)
        {
            model::BookInfo* pBookInfo = dynamic_cast<model::BookInfo*>(args.dataList[i].get());
            if (pBookInfo->GetDiscountPrice() < 0)
                discountPriceCount += pBookInfo->GetPrice();
            else
                discountPriceCount += pBookInfo->GetDiscountPrice();
            originPriceCount += pBookInfo->GetPrice();
        }
        return true;
    }
    return false;
}

static SpecialEventSPtr GetSpecialEvent()
{
    double currentTime = time(NULL)*1.0;
    BookStoreOrderManager* orderManager = BookStoreOrderManager::GetInstance();
    SpecialEventList specialEvents = orderManager->GetSpecialEventByTime(currentTime);
    if (!specialEvents.empty())
    {
        for (size_t i = 0; i < specialEvents.size(); ++i)
        {
            if (specialEvents[i] && specialEvents[i]->GetType() == SPECIAL_EVENT_ALL)
            {
                return specialEvents[i];
            }
        }
    }
    return SpecialEventSPtr();
}

bool UICartPageBottomBar::OnGetSpecialEventsUpdate(const EventArgs& args)
{
    DataUpdateArgs updatedData;
    if (BookStoreOrderManager::GetInstance()->GetCachedBooksInCart(updatedData))
    {
        return OnGetBooksInCartUpdate(updatedData);
    }
    return false;
}

void UICartPageBottomBar::ClearAllText()
{
    m_textCartSummary.SetText("");
    m_textDiscountsNotice.SetText("");
    //m_textSpecialEventContent.SetText("");
    //m_textSpecialEventNotice.SetText("");
    m_textSpecialEventContent.SetVisible(FALSE);
    m_textSpecialEventNotice.SetVisible(FALSE);
}

bool UICartPageBottomBar::OnGetBooksInCartUpdate(const EventArgs& args)
{
    if (IsDisplay())
    {
        ClearAllText();
        // Get discount info
        const DataUpdateArgs& dataUpdateArgs = (const DataUpdateArgs&)args;
        int booksCount    = 0;
        int originPrice   = 0;
        int discountPrice = 0;
        double floatDiscountPrice = 0.0f;
        if (dataUpdateArgs.succeeded)
        {
            booksCount = dataUpdateArgs.dataList.size();
            if (GetDiscountInfo(dataUpdateArgs, originPrice, discountPrice))
            {
                floatDiscountPrice = ((float)discountPrice) / 100.00;
            }
        }

        double discountFromSpecialEvent = 0.0f;
        double gapToNextStrategy = 0.0f;

        // Get special events
        SpecialEventSPtr specialEvent = GetSpecialEvent();
        if (specialEvent != 0 && discountPrice > 0)
        {
            std::pair<double, double> activeStrategy(0.0f, 0.0f);
            std::pair<double, double> nextStrategy(0.0f, 0.0f);
            
            if (specialEvent->GetDiscount(floatDiscountPrice, activeStrategy, discountFromSpecialEvent))
            {
                char specialDiscountInfo[256] = {0};
                sprintf(specialDiscountInfo,
                        StringManager::GetPCSTRById(BOOKSTORE_SPECIAL_EVENT_NOTICE_REACHED),
                        activeStrategy.first,
                        activeStrategy.second,
                        floatDiscountPrice - discountFromSpecialEvent);
                m_textSpecialEventContent.SetText(specialDiscountInfo);
                m_textSpecialEventContent.SetVisible(TRUE);
            }

            if (specialEvent->GetGap(floatDiscountPrice, nextStrategy, gapToNextStrategy))
            {
                char specialEventNotice[256] = {0};
                sprintf(specialEventNotice,
                        StringManager::GetPCSTRById(BOOKSTORE_SPECIAL_EVENT_NOTICE_NOT_REACH),
                        gapToNextStrategy,
                        nextStrategy.first,
                        nextStrategy.second);
                m_textSpecialEventNotice.SetText(specialEventNotice);
                m_textSpecialEventNotice.SetVisible(TRUE);
            }
        }

        double finalDiscountPrice = (discountFromSpecialEvent != 0.0f ? discountFromSpecialEvent : floatDiscountPrice);
        if (originPrice > finalDiscountPrice)
        {
            char discountDetails[256] = {0};
            sprintf(discountDetails, StringManager::GetPCSTRById(BOOKSTORE_CART_DISCOUNT_DETAILS),
                ((float)originPrice / 100.0) - finalDiscountPrice);
            m_textDiscountsNotice.SetText(discountDetails);
        }

        char summaryInfo[256] = {0};
        if (booksCount == 0)
        {
	        sprintf(summaryInfo, "%s", StringManager::GetPCSTRById(BOOKSTORE_MY_CART_SUMMARY_EMPTY));
        }
        else if (booksCount >= 50)
        {
            sprintf(summaryInfo, StringManager::GetPCSTRById(BOOKSTORE_MY_CART_SUMMARY_FULL),
                discountFromSpecialEvent != 0.0f ? discountFromSpecialEvent : floatDiscountPrice);
        }
        else
        {
	        sprintf(summaryInfo, StringManager::GetPCSTRById(BOOKSTORE_MY_CART_SUMMARY),
                discountFromSpecialEvent != 0.0f ? discountFromSpecialEvent : floatDiscountPrice);
        }
        m_textCartSummary.SetText(summaryInfo);

        m_btnLogin.SetVisible(!CAccountManager::GetInstance()->IsLoggedIn());
        Layout();
        UpdateWindow();
    }
    return true;
}

void UICartPageBottomBar::InitUI()
{
    m_bottomLinePixel = 0;
    UISizer* specialEventHSizer = new UIBoxSizer(dkHORIZONTAL);
    //UISizer* purchaseInfoHSizer = new UIBoxSizer(dkHORIZONTAL);
    UISizer* specialEventVSizer = new UIBoxSizer(dkVERTICAL);
    int textMargin = GetWindowMetrics(UIBookStoreCartSummaryTextMarginIndex);

    SetSplitLineHeight(GetWindowMetrics(UIBottomBarSplitLineHeightIndex));

    m_textCartSummary.SetAlign(ALIGN_LEFT);
    m_textCartSummary.SetFontSize(GetWindowFontSize(UIBookStoreCartSummaryTextIndex));
    AppendChild(&m_textCartSummary);
    m_pBtnListSizer->Add(&m_textCartSummary,
        UISizerFlags(1).Expand().Center().Border(dkLEFT, textMargin).
        Border(dkTOP, GetWindowMetrics(UIElementMidSpacingIndex)));

    // Special Events
    m_textDiscountTitle.SetAlign(ALIGN_LEFT);
    m_textDiscountTitle.SetFontSize(GetWindowFontSize(UIBookStoreCartDiscountTextIndex));
    m_textDiscountTitle.SetText(StringManager::GetPCSTRById(BOOKSTORE_CART_DISCOUNT_NOTICE));
    AppendChild(&m_textDiscountTitle);
    specialEventHSizer->Add(&m_textDiscountTitle,
        UISizerFlags().Top().Border(dkLEFT, textMargin));

    m_textDiscountsNotice.SetAlign(ALIGN_LEFT);
    m_textDiscountsNotice.SetFontSize(GetWindowFontSize(UIBookStoreCartDiscountTextIndex));
    m_textDiscountsNotice.SetMinWidth(GetWindowMetrics(UICartSpecialEventWidthIndex));
    AppendChild(&m_textDiscountsNotice);
    specialEventVSizer->Add(&m_textDiscountsNotice, UISizerFlags().Left());
    specialEventVSizer->AddSpacer(GetWindowMetrics(UIPixelValue3Index));

    m_textSpecialEventContent.SetAlign(ALIGN_LEFT);
    m_textSpecialEventContent.SetFontSize(GetWindowFontSize(UIBookStoreCartDiscountTextIndex));
    m_textSpecialEventContent.SetMinWidth(GetWindowMetrics(UICartSpecialEventWidthIndex));
    AppendChild(&m_textSpecialEventContent);
    specialEventVSizer->Add(&m_textSpecialEventContent, UISizerFlags().Left());
    specialEventVSizer->AddSpacer(GetWindowMetrics(UIPixelValue3Index));

    m_textSpecialEventNotice.SetAlign(ALIGN_LEFT);
    m_textSpecialEventNotice.SetFontSize(GetWindowFontSize(UIBookStoreCartDiscountTextIndex));
    m_textSpecialEventNotice.SetMinWidth(GetWindowMetrics(UICartSpecialEventWidthIndex));
    AppendChild(&m_textSpecialEventNotice);
    specialEventVSizer->Add(&m_textSpecialEventNotice, UISizerFlags().Left());

    specialEventHSizer->Add(specialEventVSizer, UISizerFlags(1).Top().Border(dkLEFT, GetWindowMetrics(UIPixelValue8Index)));
    m_pBtnListSizer->Add(specialEventHSizer, UISizerFlags().Border(dkTOP, GetWindowMetrics(UIPixelValue8Index)));

    // Purchase info
    UISizer* bottomHSizer = new UIBoxSizer(dkHORIZONTAL);
    m_textPurchaseNotice.SetAlign(ALIGN_LEFT);
    m_textPurchaseNotice.SetFontSize(GetWindowFontSize(UIBookStoreCartDiscountTextIndex));
    m_textPurchaseNotice.SetText(StringManager::GetPCSTRById(BOOKSTORE_CART_PURCHASING_NOTICE));
    AppendChild(&m_textPurchaseNotice);
    bottomHSizer->Add(&m_textPurchaseNotice, UISizerFlags().Center().Left());

    m_btnLogin.Initialize(ID_BTN_PERSONAL_LOGIN,
                          StringManager::GetPCSTRById(ACCOUNT_LOGIN),
                          GetWindowFontSize(UIBookStoreCartPurchaseBtnIndex));
    m_btnLogin.SetAlign(ALIGN_LEFT);
    m_btnLogin.SetFontColor(ColorManager::knBlack);
    AddButton(&m_btnLogin);
    bottomHSizer->Add(&m_btnLogin, UISizerFlags().Center().Border(dkLEFT, GetWindowMetrics(UIPixelValue60Index)));
    if (CAccountManager::GetInstance()->IsLoggedIn())
    {
        m_btnLogin.SetVisible(FALSE);
    }

    m_pBtnListSizer->Add(bottomHSizer,
        UISizerFlags(1).Border(dkLEFT, textMargin).Border(dkBOTTOM, GetWindowMetrics(UIPixelValue8Index)));

    CDisplay *pDisplay = CDisplay::GetDisplay();
    if(pDisplay)
    {
        m_minWidth = pDisplay->GetScreenWidth();
        m_minHeight = GetWindowMetrics(UICartPageBottomBarHeightIndex) + 20;
    }
}

void UICartPageBottomBar::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    if (dwCmdId == ID_BTN_PERSONAL_LOGIN)
    {
        CAccountManager* accountManager = CAccountManager::GetInstance();
        if (!accountManager->IsLoggedIn())
        {
            UIUtility::CheckAndReLogin();
        }
    }

//    if (dwCmdId == ID_BTN_BOOK_CART_PURCHASE)
//    {
//        // Purchase Channels
//        UIBookStorePayDlg bookStorePayDlg(this, FALSE);
//            
//        //CDisplay* pDisplay = CDisplay::GetDisplay();
//		//const int payDlgWidth = bookStorePayDlg.GetWidth();
//		//const int patDlgHeight = bookStorePayDlg.GetHeight();
//        //const int screenWidth = pDisplay->GetScreenWidth();
//        //const int screenHeight = pDisplay->GetScreenHeight();
//		//bookStorePayDlg.MoveWindow((screenWidth - payDlgWidth)/2, (screenHeight - patDlgHeight)/2, payDlgWidth, patDlgHeight);
//        if (bookStorePayDlg.DoModal() == IDOK)
//        {
//            switch (bookStorePayDlg.GetAction())
//            {
//                case UIBookStorePayDlg::SEND_MAIL:
//                    // Do Not support email now
//                    break;
//                case UIBookStorePayDlg::BUY_FROM_OTHER_DEVICE:
//                    {
//                        UIBookStorePayInfoDlg dlg(GetDlgParent(),
//#ifdef KINDLE_FOR_TOUCH
//                            StringManager::GetPCSTRById(BOOKSTORE_TITLE_BUY_FROM_OTHER_DEVICE), 
//#endif
//                            UIBookStorePayInfoDlg::PM_OTHER_DEVICE,
//                            0, 0);
//                        //const int payInfoDlgWidth = dlg.GetWidth();
//                        //const int patInfoDlgHeight = dlg.GetHeight();
//                        //dlg.MoveWindow((screenWidth - payInfoDlgWidth)/2, (screenHeight - patInfoDlgHeight)/2, payInfoDlgWidth, patInfoDlgHeight);
//                        if (dlg.DoModal() == IDOK)
//                        {
//                            // Notify listeners that the purchasing has ended.
//                        }
//                    }
//                    break;
//                case UIBookStorePayDlg::BUY_FROM_WEBSITE:
//                    {
//                        UIBookStorePayInfoDlg dlg(GetDlgParent(),
//#ifdef KINDLE_FOR_TOUCH
//                            StringManager::GetPCSTRById(BOOKSTORE_TITLE_BUY_FROM_WEBSITE), 
//#endif
//                            UIBookStorePayInfoDlg::PM_WEBSITE,
//                            0, 0);
//                        //const int payInfoDlgWidth = dlg.GetWidth();
//                        //const int patInfoDlgHeight = dlg.GetHeight();
//                        //dlg.MoveWindow((screenWidth - payInfoDlgWidth)/2, (screenHeight - patInfoDlgHeight)/2, payInfoDlgWidth, patInfoDlgHeight);
//                        if (dlg.DoModal() == IDOK)
//                        {
//                            // Notify listeners that the purchasing has ended.
//                        }
//                    }
//                    break;
//                default:
//                    break;
//            }
//        }
//    }
}

UIContainer* UICartPageBottomBar::GetDlgParent()
{
#ifdef KINDLE_FOR_TOUCH
    return GUISystem::GetInstance()->GetTopFullScreenContainer();
#else
    return this;
#endif
}



UIBookStoreCartPage::UIBookStoreCartPage()
    : UIBookStoreListBoxPage(StringManager::GetPCSTRById(BOOKSTORE_MY_CART), false)
{
    m_fetchInfo.CustomizeNoElementInfo(StringManager::GetPCSTRById(PERSONAL_CART_NO_ELEMENT_NOTICE),
                                       StringManager::GetPCSTRById(PERSONAL_CART_NO_ELEMENT_STROLL));
    SubscribeMessageEvent(BookStoreOrderManager::EventGetCartUpdate, 
        *BookStoreOrderManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreListBoxPage::OnMessageListBoxUpdate),
        this,
        std::tr1::placeholders::_1)
        );

    SubscribeMessageEvent(BookStoreOrderManager::EventAddToCartUpdate, 
        *BookStoreOrderManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreCartPage::OnAddCartUpdate),
        this,
        std::tr1::placeholders::_1)
        );

    SubscribeMessageEvent(BookStoreOrderManager::EventRemoveFromCartUpdate, 
        *BookStoreOrderManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreCartPage::OnRemoveCartUpdate),
        this,
        std::tr1::placeholders::_1)
        );

    SubscribeMessageEvent(BookStoreOrderManager::EventAddFavouriteUpdate, 
        *BookStoreOrderManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreCartPage::OnAddFavouriteUpdate),
        this,
        std::tr1::placeholders::_1)
        );

    SubscribeMessageEvent(BookStoreOrderManager::EventRemoveFavouriteUpdate, 
        *BookStoreOrderManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreCartPage::OnRemoveFavouriteUpdate),
        this,
        std::tr1::placeholders::_1)
        );

    SubscribeEvent(UIBookStoreBookInCartItemStatusListener::EventBookInCartItemLongTapped, 
        *UIBookStoreBookInCartItemStatusListener::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreCartPage::OnBookItemLongTapped),
        this,
        std::tr1::placeholders::_1)
        );

    SubscribeEvent(UIBookStoreBookInCartItemStatusListener::EventBookInCartDelete, 
        *UIBookStoreBookInCartItemStatusListener::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreCartPage::OnBookInCartDelete),
        this,
        std::tr1::placeholders::_1)
        );
}

UIBookStoreCartPage::~UIBookStoreCartPage()
{
}

FetchDataResult UIBookStoreCartPage::FetchInfo() 
{
    SetFetchStatus(FETCHING);

    FetchDataResult fdr = FDR_FAILED;
    BookStoreOrderManager* manager = BookStoreOrderManager::GetInstance();
    if (manager && m_pListBoxPanel)
    {
        fdr = manager->FetchBooksInCart();
    }
    else
    {
        SetFetchStatus(FETCH_FAIL);
    }
    return fdr;
}

bool UIBookStoreCartPage::OnBookInCartDelete(const EventArgs& args)
{
    if (!IsDisplay())
    {
        // Only handle the long tap message when this page is activated
        return false;
    }

    const BookItemEventArgs& itemArgs = (const BookItemEventArgs&)args;
    char* infoStr = new char[itemArgs.bookInfo->GetTitle().size() + 128];
    memset(infoStr, 0, itemArgs.bookInfo->GetTitle().size() + 128);
    sprintf(infoStr, StringManager::GetStringById(BOOKSTORE_CONFIRM_REMOVE_CART), itemArgs.bookInfo->GetTitle().c_str());
    UIMessageBox messagebox(this, infoStr, MB_OK | MB_CANCEL);
    if(MB_OK == messagebox.DoModal())
    {
        model::BookInfoList favouriteBooks;
        favouriteBooks.push_back(itemArgs.bookInfo);
        if (BookStoreOrderManager::GetInstance()->RemoveBooksFromCart(favouriteBooks)
            == FDR_FAILED)
        {
            return false;
        }
    }
    return true;
}

bool UIBookStoreCartPage::OnBookItemLongTapped(const EventArgs& args)
{
    if (!IsDisplay())
    {
        // Only handle the long tap message when this page is activated
        return false;
    }

    //DebugPrintf(DLC_DIAGNOSTIC, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    const BookItemEventArgs& itemArgs = (const BookItemEventArgs&)args;
    m_currentBookInfo = itemArgs.bookInfo;
    bool isThisBookedAddedToFavourite = BookStoreOrderManager::GetInstance()->IsBookAddedToFavourite(
                                            m_currentBookInfo->GetBookId().c_str());
    std::vector<int> btnIDs;
    btnIDs.reserve(8);
    if (!isThisBookedAddedToFavourite)
        btnIDs.push_back(ID_BTN_ADD_BOOK_TO_FAVOURITE);
    btnIDs.push_back(ID_BTN_REMOVE_FROM_CART);
    btnIDs.push_back(ID_BTN_OPEN_BOOK);
    btnIDs.push_back(ID_INVALID);

    std::vector<int> strIDs;
    strIDs.reserve(8);
    if (!isThisBookedAddedToFavourite)
        strIDs.push_back(ADD_BOOK_TO_FAVOURITE);
    strIDs.push_back(REMOVE_FROM_CART);
    strIDs.push_back(OPEN_BOOK_DETAILS);
    strIDs.push_back(-1);

    CDisplay* pDisplay = CDisplay::GetDisplay();
    UIBookMenuDlg dlgMenu(this, btnIDs, strIDs); 
    int dlgWidth = dlgMenu.GetWidth();
	int dlgHeight = dlgMenu.GetHeight();
    int itemHeight = m_pListBoxPanel->GetItemHeight();
    int selectedItem = m_pListBoxPanel->GetSelectedItemIndex();
    int top = itemHeight * (selectedItem + 2);
    if (top + dlgHeight > m_iHeight)
    {
        top -= itemHeight + dlgHeight;
    }
    dlgMenu.MoveWindow((pDisplay->GetScreenWidth() - dlgWidth)/2, m_iTop + top, dlgWidth, dlgHeight);
	if (dlgMenu.DoModal() == IDOK)
    {
        OnCommand(dlgMenu.GetCommandId(), NULL, 0);
    }
    return true;
}

bool UIBookStoreCartPage::CreateListBoxPanel()
{
    if (NULL == m_pListBoxPanel)
    {
        m_pListBoxPanel = new UIBookStoreListBoxWithBtn(4, //GetWindowMetrics(UIBookStoreListBoxPageItemCountIndex), 
            GetWindowMetrics(UIBookStoreListBoxPageItemHeightIndex));
        m_pListBoxPanel->SetOnceAllItems(true);
        m_pListBoxPanel->SetItemEnableLongClick(true);
        m_pListBoxPanel->SetItemEnableHighlight(false);
    }
    return (NULL != m_pListBoxPanel);
}

void UIBookStoreCartPage::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    switch (dwCmdId)
    {
    case ID_BTN_ADD_BOOK_TO_FAVOURITE:
        {
            if (m_currentBookInfo != 0)
            {
                // TODO: Refine this code
                model::BookInfoList favouriteBooks;
                favouriteBooks.push_back(m_currentBookInfo);
                if (BookStoreOrderManager::GetInstance()->MoveCartBooksToFavourite(favouriteBooks)
                    == FDR_FAILED)
                {
                    break;
                }
            }
        }
        break;
    case ID_BTN_REMOVE_FROM_CART:
        {
            char* infoStr = new char[m_currentBookInfo->GetTitle().size() + 128];
            memset(infoStr, 0, m_currentBookInfo->GetTitle().size() + 128);
            sprintf(infoStr, StringManager::GetStringById(BOOKSTORE_CONFIRM_REMOVE_CART), m_currentBookInfo->GetTitle().c_str());
            UIMessageBox messagebox(this, infoStr, MB_OK | MB_CANCEL);
            if(MB_OK == messagebox.DoModal())
            {
                if (m_currentBookInfo != 0)
                {
                    // TODO: Refine this code
                    model::BookInfoList favouriteBooks;
                    favouriteBooks.push_back(m_currentBookInfo);
                    if (BookStoreOrderManager::GetInstance()->RemoveBooksFromCart(favouriteBooks)
                        == FDR_FAILED)
                    {
                        break;
                    }
                }
            }
        }
        break;
    case ID_BTN_OPEN_BOOK:
        {
            UIBookDetailedInfoPage* bookInfoPage = new UIBookDetailedInfoPage(m_currentBookInfo->GetBookId(),
                                                                              m_currentBookInfo->GetTitle()); 
            bookInfoPage->MoveWindow(0,0,CDisplay::GetDisplay()->GetScreenWidth(),CDisplay::GetDisplay()->GetScreenHeight());
            CPageNavigator::Goto(bookInfoPage);
        }
        break;
    case ID_BOOKSTORE_STROLL:
        {
            CPageNavigator::BackToEntryPageOnTypeAsync(PAT_BookStore);
        }
        break;
    default:
        UIBookStoreListBoxPage::OnCommand(dwCmdId, pSender, dwParam);
        break;
    }
}

void UIBookStoreCartPage::FetchLocalCart()
{
    DataUpdateArgs updatedData;
    if (BookStoreOrderManager::GetInstance()->GetCachedBooksInCart(updatedData))
    {
        OnMessageListBoxUpdate(updatedData);
        UICartPageBottomBar* bottomBar = dynamic_cast<UICartPageBottomBar*>(GetCustomizedBottomBar());
        bottomBar->OnGetBooksInCartUpdate(updatedData);
    }
}

void UIBookStoreCartPage::OnEnter()
{
    // TODO. Let user choose login.
    /*CAccountManager* accountManager = CAccountManager::GetInstance();
    if (!accountManager->IsLoggedIn())
    {
        UIUtility::CheckAndReLogin();
    }*/
    FetchLocalCart();
}

UIButtonGroup* UIBookStoreCartPage::GetCustomizedBottomBar()
{
    static UICartPageBottomBar cartBottomBar;
    return &cartBottomBar;
}

bool UIBookStoreCartPage::OnAddCartUpdate(const EventArgs& args)
{
    const BookStoreOrderArgs& addCartArgs = (const BookStoreOrderArgs&)args;
    if (addCartArgs.succeeded)
    {
        //ShowTips(StringManager::GetPCSTRById(BOOKSTORE_ADD_TO_CART_SUCCEEDED));
    }
    else if (addCartArgs.resultCode == CART_IS_FULL)
    {
        ShowTips(StringManager::GetPCSTRById(BOOKSTORE_CART_FULL));
    }
    else
        ShowTips(StringManager::GetPCSTRById(BOOKSTORE_ADD_TO_CART_FAILED));
    FetchLocalCart();
    return true;
}

bool UIBookStoreCartPage::OnRemoveCartUpdate(const EventArgs& args)
{
    const BookStoreOrderArgs& removeCartArgs = (const BookStoreOrderArgs&)args;
    if (!removeCartArgs.succeeded)
        ShowTips(StringManager::GetPCSTRById(BOOKSTORE_REMOVE_FROM_CART_FAILED));
    FetchLocalCart();
    return true;
}

bool UIBookStoreCartPage::OnAddFavouriteUpdate(const EventArgs& args)
{
    const BookStoreOrderArgs& addFavouriteArgs = (const BookStoreOrderArgs&)args;
    if (addFavouriteArgs.succeeded)
    {
        //ShowTips(StringManager::GetPCSTRById(BOOKSTORE_ADD_FAVOURITE_SUCCEEDED));
    }
    else if (addFavouriteArgs.resultCode == FAVOURITES_IS_FULL)
    {
        ShowTips(StringManager::GetPCSTRById(BOOKSTORE_FAVOURITES_FULL));
        if (IsDisplay())
        {
            UIBookStorePersonalFavouritesPage *pPage = new UIBookStorePersonalFavouritesPage();
            if (pPage)
            {
                pPage->MoveWindow(0, 0, DeviceInfo::GetDisplayScreenWidth(), DeviceInfo::GetDisplayScreenHeight());
                CPageNavigator::Goto(pPage);
            }
        }
    }
    else
        ShowTips(StringManager::GetPCSTRById(BOOKSTORE_ADD_FAVOURITE_FAILED));
    return true;
}

bool UIBookStoreCartPage::OnRemoveFavouriteUpdate(const EventArgs& args)
{
    const BookStoreOrderArgs& removeFavouriteArgs = (const BookStoreOrderArgs&)args;
    if (!removeFavouriteArgs.succeeded)
        ShowTips(StringManager::GetPCSTRById(BOOKSTORE_REMOVE_FAVOURITE_FAILED));
    return true;
}

void UIBookStoreCartPage::ShowTips(const std::string& tips)
{
    if (IsDisplay())
    {
        CDisplay::CacheDisabler forceDraw;
        //UIUtility::ShowTip(m_pTips, true);
        UpdateWindow();
        usleep(500000);
        UIUtility::SetScreenTips(tips.c_str());
    }
}
