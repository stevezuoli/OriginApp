#include <tr1/functional>
#include "PersonalUI/UIPersonalFavouritesPage.h"
#include "Common/CAccountManager.h"
#include "BookStore/DataUpdateArgs.h"
#include "BookStore/BookStoreOrderManager.h"
#include "CommonUI/UIUtility.h"
#include "CommonUI/CPageNavigator.h"
#include "I18n/StringManager.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "Framework/CDisplay.h"
#include "BookStoreUI/UIBookStoreListBoxWithBtn.h"
#include "BookStoreUI/UIBookStoreBookItem.h"
#include "BookStoreUI/UIBookDetailedInfoPage.h"
#include "BookStoreUI/UIBookStoreCartPage.h"
#include "TouchAppUI/UIBookMenuDlg.h"
#include "drivers/DeviceInfo.h"

using namespace dk::bookstore;
using namespace dk::account;
using namespace WindowsMetrics;

UIBookStorePersonalFavouritesPage::UIBookStorePersonalFavouritesPage()
    : UIBookStoreListBoxPage(StringManager::GetPCSTRById(PERSONAL_FAVOURITES))
{
    m_fetchInfo.CustomizeNoElementInfo(StringManager::GetPCSTRById(PERSONAL_FAVOURITES_NO_ELEMENT_NOTICE),
                                       StringManager::GetPCSTRById(PERSONAL_CART_NO_ELEMENT_STROLL));
    SubscribeMessageEvent(BookStoreOrderManager::EventGetFavouriteUpdate, 
        *BookStoreOrderManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreListBoxPage::OnMessageListBoxUpdate),
        this,
        std::tr1::placeholders::_1)
        );

    SubscribeMessageEvent(BookStoreOrderManager::EventAddFavouriteUpdate, 
        *BookStoreOrderManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStorePersonalFavouritesPage::OnAddFavouriteUpdate),
        this,
        std::tr1::placeholders::_1)
        );

    SubscribeMessageEvent(BookStoreOrderManager::EventRemoveFavouriteUpdate, 
        *BookStoreOrderManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStorePersonalFavouritesPage::OnRemoveFavouriteUpdate),
        this,
        std::tr1::placeholders::_1)
        );

    SubscribeMessageEvent(BookStoreOrderManager::EventAddToCartUpdate, 
        *BookStoreOrderManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStorePersonalFavouritesPage::OnAddCartUpdate),
        this,
        std::tr1::placeholders::_1)
        );

    SubscribeMessageEvent(BookStoreOrderManager::EventRemoveFromCartUpdate, 
        *BookStoreOrderManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStorePersonalFavouritesPage::OnRemoveCartUpdate),
        this,
        std::tr1::placeholders::_1)
        );

    SubscribeEvent(UIBookStoreBookItemStatusListener::EventBookItemLongTapped, 
        *UIBookStoreBookItemStatusListener::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStorePersonalFavouritesPage::OnBookItemLongTapped),
        this,
        std::tr1::placeholders::_1)
        );
}

UIBookStorePersonalFavouritesPage::~UIBookStorePersonalFavouritesPage()
{
}

FetchDataResult UIBookStorePersonalFavouritesPage::FetchInfo() 
{
    SetFetchStatus(FETCHING);

    FetchDataResult fdr = FDR_FAILED;
    BookStoreOrderManager* manager = BookStoreOrderManager::GetInstance();
    if (manager && m_pListBoxPanel)
    {
        fdr = manager->FetchFavouriteBooks();
    }
    else
    {
        SetFetchStatus(FETCH_FAIL);
    }
    return fdr;
}

bool UIBookStorePersonalFavouritesPage::OnBookItemLongTapped(const EventArgs& args)
{
    if (!IsDisplay())
    {
        // Only handle the long tap message when this page is activated
        return false;
    }

    DebugPrintf(DLC_DIAGNOSTIC, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);

    const BookItemEventArgs& itemArgs = (const BookItemEventArgs&)args;
    m_currentBookInfo = itemArgs.bookInfo;    
    int status = BookStoreInfoManager::GetInstance()->GetBookStatus(m_currentBookInfo);
    std::vector<int> btnIDs;
    std::vector<int> strIDs;
    if (m_currentBookInfo->GetPrice() != 0 &&
        m_currentBookInfo->GetDiscountPrice() != 0 &&
        (BS_UNPURCHASED & status))
    {
        if (!BookStoreOrderManager::GetInstance()->IsBookAddedToCart(m_currentBookInfo->GetBookId().c_str()))
        {
            btnIDs.push_back(ID_BTN_ADD_BOOK_TO_CART);
            strIDs.push_back(ADD_BOOK_TO_CART);
        }
    }
    btnIDs.push_back(ID_BTN_REMOVE_FAVOURITE);
    btnIDs.push_back(ID_BTN_OPEN_BOOK);
    btnIDs.push_back(ID_INVALID);
 
    strIDs.push_back(REMOVE_FAVOURITE);
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

bool UIBookStorePersonalFavouritesPage::CreateListBoxPanel()
{
    if (NULL == m_pListBoxPanel)
    {
        m_pListBoxPanel = new UIBookStoreListBoxWithBtn(GetWindowMetrics(UIBookStoreListBoxPageItemCountIndex), 
            GetWindowMetrics(UIBookStoreListBoxPageItemHeightIndex));
        m_pListBoxPanel->SetOnceAllItems(true);
        m_pListBoxPanel->SetItemEnableLongClick(true);
        m_pListBoxPanel->SetItemEnableHighlight(false);
    }
    return (NULL != m_pListBoxPanel);
}

void UIBookStorePersonalFavouritesPage::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    switch (dwCmdId)
    {
    case ID_BTN_ADD_BOOK_TO_CART:
        {
            if (m_currentBookInfo != 0)
            {
                // TODO: Refine this code
                model::BookInfoList cartBooks;
                cartBooks.push_back(m_currentBookInfo);
                //if (BookStoreOrderManager::GetInstance()->MoveFavouriteBooksToCart(cartBooks)
                if (BookStoreOrderManager::GetInstance()->AddBooksToCart(cartBooks)
                    == FDR_FAILED)
                {
                    break;
                }
            }
        }
        break;
    case ID_BTN_REMOVE_FAVOURITE:
        {
            if (m_currentBookInfo != 0)
            {
                // TODO: Refine this code
                model::BookInfoList favouriteBooks;
                favouriteBooks.push_back(m_currentBookInfo);
                if (BookStoreOrderManager::GetInstance()->RemoveFavouriteBooks(favouriteBooks)
                    == FDR_FAILED)
                {
                    break;
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

void UIBookStorePersonalFavouritesPage::FetchLocalFavourites()
{
    DataUpdateArgs updatedData;
    if (BookStoreOrderManager::GetInstance()->GetCachedFavouriteList(updatedData))
    {
        OnMessageListBoxUpdate(updatedData);
    }
}

void UIBookStorePersonalFavouritesPage::OnEnter()
{
    FetchLocalFavourites();
}

bool UIBookStorePersonalFavouritesPage::OnAddFavouriteUpdate(const EventArgs& args)
{
    const BookStoreOrderArgs& addFavouriteArgs = (const BookStoreOrderArgs&)args;
    if (addFavouriteArgs.succeeded)
    {
        //ShowTips(StringManager::GetPCSTRById(BOOKSTORE_ADD_FAVOURITE_SUCCEEDED));
    }
    else if (addFavouriteArgs.resultCode == FAVOURITES_IS_FULL)
    {
        ShowTips(StringManager::GetPCSTRById(BOOKSTORE_FAVOURITES_FULL));
    }
    else
        ShowTips(StringManager::GetPCSTRById(BOOKSTORE_ADD_FAVOURITE_FAILED));
    FetchLocalFavourites();
    return true;
}

bool UIBookStorePersonalFavouritesPage::OnRemoveFavouriteUpdate(const EventArgs& args)
{
    const BookStoreOrderArgs& removeFavouriteArgs = (const BookStoreOrderArgs&)args;
    if (!removeFavouriteArgs.succeeded)
        ShowTips(StringManager::GetPCSTRById(BOOKSTORE_REMOVE_FAVOURITE_FAILED));
    FetchLocalFavourites();
    return true;
}

bool UIBookStorePersonalFavouritesPage::OnAddCartUpdate(const EventArgs& args)
{
    const BookStoreOrderArgs& addCartArgs = (const BookStoreOrderArgs&)args;
    if (addCartArgs.succeeded)
    {
        //ShowTips(StringManager::GetPCSTRById(BOOKSTORE_ADD_TO_CART_SUCCEEDED));
    }
    else if (addCartArgs.resultCode == CART_IS_FULL)
    {
        ShowTips(StringManager::GetPCSTRById(BOOKSTORE_CART_FULL));
        if (IsDisplay())
        {
            UIBookStoreCartPage *pPage = new UIBookStoreCartPage();
            if (pPage)
            {
                pPage->MoveWindow(0, 0, DeviceInfo::GetDisplayScreenWidth(), DeviceInfo::GetDisplayScreenHeight());
                CPageNavigator::Goto(pPage);
            }
        }
    }
    else
        ShowTips(StringManager::GetPCSTRById(BOOKSTORE_ADD_TO_CART_FAILED));
    return true;
}

bool UIBookStorePersonalFavouritesPage::OnRemoveCartUpdate(const EventArgs& args)
{
    const BookStoreOrderArgs& removeCartArgs = (const BookStoreOrderArgs&)args;
    if (!removeCartArgs.succeeded)
        ShowTips(StringManager::GetPCSTRById(BOOKSTORE_REMOVE_FROM_CART_FAILED));
    return true;
}

void UIBookStorePersonalFavouritesPage::ShowTips(const std::string& tips)
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