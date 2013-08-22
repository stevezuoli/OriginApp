#include "BookStoreUI/UIBookStoreBookInCartItem.h"
#include "BookStoreUI/UIBookDetailedInfoPage.h"
#include "CommonUI/CPageNavigator.h"
#include "I18n/StringManager.h"
#include "Utility/ImageManager.h"
#include "GUI/CTpGraphics.h"
#include "interface.h"
#include "BookStore/BookStoreInfoManager.h"
#include <tr1/functional>
#include "Common/CAccountManager.h"
#include "GUI/UIMessageBox.h"
#include "GUI/GUISystem.h"
#include "GUI/UISizer.h"
#include "../Common/FileManager_DK.h"
#include "Common/WindowsMetrics.h"
#include "Framework/CDisplay.h"
#include "Utility/PathManager.h"
#ifdef KINDLE_FOR_TOUCH
#include "TouchAppUI/BookOpenManager.h"
#else
#include "AppUI/BookOpenManager.h"
#endif
#include "Framework/CNativeThread.h"
#include "CommonUI/UIUtility.h"

using dk::utility::PathManager;
using dk::bookstore::model::BookInfo;
using namespace WindowsMetrics;

using namespace dk::bookstore;
using namespace dk::account;


const char* UIBookStoreBookInCartItemStatusListener::EventBookInCartItemLongTapped = "EventBookInCartItemLongTapped";
const char* UIBookStoreBookInCartItemStatusListener::EventBookInCartDelete = "EventBookInCartDelete";

UIBookStoreBookInCartItemStatusListener* UIBookStoreBookInCartItemStatusListener::GetInstance()
{
    static UIBookStoreBookInCartItemStatusListener listner;
    return &listner;
}

void UIBookStoreBookInCartItemStatusListener::FireBookInCartItemLongTappedEvent(dk::bookstore::model::BookInfoSPtr info)
{
    BookInCartItemEventArgs args;
    args.bookInfo = info;
    FireEvent(EventBookInCartItemLongTapped, args);
}

void UIBookStoreBookInCartItemStatusListener::FireBookInCartItemDeleteEvent(dk::bookstore::model::BookInfoSPtr info)
{
    BookInCartItemEventArgs args;
    args.bookInfo = info;
    FireEvent(EventBookInCartDelete, args);
}


UIBookStoreBookInCartItem::UIBookStoreBookInCartItem(bool showScore)
{
    SetLongClickable(true);
    SetUpdateOnPress(false);
    SetUpdateOnUnPress(false);
    Init();
}

UIBookStoreBookInCartItem::~UIBookStoreBookInCartItem()
{

}

void UIBookStoreBookInCartItem::Init()
{
    //const int authorFontSize = GetWindowFontSize(UIListItemBaseMidFontSizeIndex);
    const int titleFontSize = GetWindowFontSize(UIListItemBaseTitleFontSizeIndex);
    const int priceFontSize = GetWindowFontSize(FontSize16Index);
    const int authorFontSize = GetWindowFontSize(FontSize16Index);
    const int originFontSize = GetWindowFontSize(FontSize14Index);
    m_bookCover.Initialize(SMALL);
    m_bookCover.SetEnable(false);
    AppendChild(&m_bookCover);

    m_bookTitle.SetFontSize(titleFontSize);
    m_bookTitle.SetEndWithEllipsis(true);
    AppendChild(&m_bookTitle);

    m_bookAuthor.SetFontSize(authorFontSize);
    AppendChild(&m_bookAuthor);

    m_bookPrice.SetFontSize(priceFontSize);
    AppendChild(&m_bookPrice);

    m_bookOriginPrice.SetFontSize(originFontSize);
    m_bookOriginPrice.SetForeGroundColor(0x999999);
    AppendChild(&m_bookOriginPrice);

    //m_bookPaperPrice.SetFontSize(originFontSize);
    //m_bookPaperPrice.SetForeGroundColor(0x999999);
    //AppendChild(&m_bookPaperPrice);

    //m_disCountFromPaper.SetFontSize(GetWindowFontSize(priceFontSize));
    //AppendChild(&m_disCountFromPaper);
    
#ifdef KINDLE_FOR_TOUCH
    m_responseWnd.Initialize(ID_BTN_DELETE, StringManager::GetPCSTRById(BOOK_DELETE), priceFontSize);
    m_responseWnd.SetTouchSlot(GetWindowMetrics(UIPixelValue15Index));
#else
    m_responseWnd.SetFontSize(priceFontSize);
    m_responseWnd.SetVisible(FALSE);
#endif
    AppendChild(&m_responseWnd);

    if (!m_windowSizer)
    {
        const int margin = GetWindowMetrics(UIPixelValue15Index);
        const int border = GetWindowMetrics(UIPixelValue10Index);
#ifdef KINDLE_FOR_TOUCH
        m_responseWnd.SetMinSize(dkSize(GetWindowMetrics(UIBookStoreRecommendListItemButtonWidthIndex), GetWindowMetrics(UIBookStoreRecommendListItemButtonHeightIndex)));
#else
        m_bookCover.SetMinSize(dkSize(m_bookCover.GetWidth(), m_bookCover.GetHeight()));
#endif
        UISizer* pOutmostSizer = new UIBoxSizer(dkVERTICAL);
        UISizer* pMainSizer = new UIBoxSizer(dkHORIZONTAL);
        if (pOutmostSizer != 0 && pMainSizer != 0)
        {
            pMainSizer->Add(&m_bookCover, UISizerFlags().Border(dkTOP, margin));
            pMainSizer->AddSpacer(margin);

            UISizer* bookContentInfoSizer = new UIBoxSizer(dkVERTICAL);
            if (bookContentInfoSizer != 0)
            {
                bookContentInfoSizer->Add(&m_bookTitle, UISizerFlags().Expand().Border(dkBOTTOM, border));
                bookContentInfoSizer->Add(&m_bookAuthor, UISizerFlags().Expand().Border(dkBOTTOM, border));

                UISizer* bookPriceSizer = new UIBoxSizer(dkHORIZONTAL);
                bookPriceSizer->Add(&m_bookPrice, UISizerFlags().Expand());
                bookPriceSizer->Add(&m_bookOriginPrice, UISizerFlags().Expand().Border(dkLEFT, GetWindowMetrics(UIPixelValue12Index)));
                //bookPriceSizer->Add(&m_bookPaperPrice, UISizerFlags().Expand().Border(dkLEFT, GetWindowMetrics(UIPixelValue12Index)));
                //bookPriceSizer->Add(&m_disCountFromPaper, UISizerFlags().Expand().Border(dkLEFT, GetWindowMetrics(UIPixelValue12Index)));

                bookContentInfoSizer->Add(bookPriceSizer, UISizerFlags().Expand().Border(dkBOTTOM, border));
                pMainSizer->Add(bookContentInfoSizer, UISizerFlags(1).Border(dkTOP, margin).Expand().Reduce(1));
            }

            pMainSizer->Add(&m_responseWnd, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Border(dkTOP, border).Border(dkBOTTOM, margin));

            pOutmostSizer->AddStretchSpacer();
            pOutmostSizer->Add(pMainSizer, UISizerFlags().Expand());
            pOutmostSizer->AddStretchSpacer();
            SetSizer(pOutmostSizer);
        }
        else
        {
            SafeDeletePointer(pOutmostSizer);
            SafeDeletePointer(pMainSizer);
        }
    }
}


void UIBookStoreBookInCartItem::SetInfoSPtr(dk::bookstore::model::BasicObjectSPtr info, int index)
{
    m_bookInfo.reset((model::BookInfo*)info.get()->Clone());
    model::BookInfo* bookInfo = (model::BookInfo*)(m_bookInfo.get());
    if(0 == bookInfo)
    {
        return;
    }
    
    if (index > 0)
    {
        string bookTitle = bookInfo->GetTitle();
        int length = bookTitle.size();
        char *pTmpTitle = new char[length + 64];
        sprintf(pTmpTitle,"%d. %s", index, bookTitle.c_str());
        m_bookTitle.SetText(pTmpTitle);
        delete [] pTmpTitle;
    }
    else
    {
        m_bookTitle.SetText(bookInfo->GetTitle().c_str());
    }

    {
        string author = bookInfo->GetAuthorsOrEditors();
        int length = author.size();
        char *pTmpAuthor = new char[length + 64];
        sprintf(pTmpAuthor, "%s", author.c_str());
        m_bookAuthor.SetText(pTmpAuthor);
        delete [] pTmpAuthor;
    }

    {
        char priceBuf[32] = {0};
        float price = static_cast<float>(bookInfo->GetPrice());
        if (bookInfo->GetDiscountPrice() >= 0)
        {
            float originPrice = price;
            price = bookInfo->GetDiscountPrice();

            sprintf(priceBuf, StringManager::GetPCSTRById(BOOKSTORE_MY_CART_ORIGIN_PRICE), originPrice/100);
            m_bookOriginPrice.SetVisible(TRUE);
            m_bookOriginPrice.SetText(priceBuf);
        }
        else
        {
            m_bookOriginPrice.SetVisible(FALSE);
        }

        sprintf(priceBuf, "%s%.2f", StringManager::GetPCSTRById(BOOKSTORE_RMB), price/100);
        //sprintf(priceBuf, StringManager::GetPCSTRById(BOOKSTORE_MY_CART_PRICE), price/100);
        m_bookPrice.SetText(priceBuf);
    }

    //if (bookInfo->GetPaperPrice() > bookInfo->GetPrice())
    //{
    //    char priceBuf[32] = {0};
    //    float paperPrice = static_cast<float>(bookInfo->GetPaperPrice());
    //    sprintf(priceBuf, StringManager::GetPCSTRById(BOOKSTORE_MY_CART_PAPER_PRICE), paperPrice/100);
    //    m_bookPaperPrice.SetText(priceBuf);

    //    float discountPrice = static_cast<float>(bookInfo->GetDiscountPrice() > 0 ? bookInfo->GetDiscountPrice() : bookInfo->GetPrice());
    //    int discount = static_cast<int>((discountPrice * 10) / paperPrice);
    //    sprintf(priceBuf, StringManager::GetPCSTRById(BOOKSTORE_MY_CART_DISCOUNT), discount);
    //    m_disCountFromPaper.SetText(priceBuf);
    //}

    m_bookCover.SetCoverUrl(bookInfo->GetCoverUrl());
}


HRESULT UIBookStoreBookInCartItem::DrawBackGround(DK_IMAGE drawingImg)
{
    CTpGraphics grf(drawingImg);
    HRESULT hr(S_OK);
    RTN_HR_IF_FAILED(grf.EraserBackGround());

#ifdef KINDLE_FOR_TOUCH
    const int lineHeight = 1;
    const StrokeStyle style = DOTTED_STROKE;
#else
    const int lineHeight = IsFocus() ? 3 : 1;
    const StrokeStyle style = IsFocus() ? SOLID_STROKE : DOTTED_STROKE;
#endif
    const int left = m_bookTitle.GetX();
    const int lineTop = m_bookCover.GetY() + m_bookCover.GetHeight() - lineHeight;//m_iHeight - lineHeight;
    RTN_HR_IF_FAILED(grf.DrawLine(left, lineTop, m_iWidth - left, lineHeight, style));
    return hr;
}

bool UIBookStoreBookInCartItem::ResponseTouchTap()
{
    UIBookDetailedInfoPage* bookInfoPage = new UIBookDetailedInfoPage(m_bookInfo->GetBookId(),m_bookInfo->GetTitle()); 
    bookInfoPage->MoveWindow(0,0,CDisplay::GetDisplay()->GetScreenWidth(),CDisplay::GetDisplay()->GetScreenHeight());
    CPageNavigator::Goto(bookInfoPage);
    return true;
}

bool UIBookStoreBookInCartItem::ResponseOperation()
{
    UIBookStoreBookInCartItemStatusListener::GetInstance()->FireBookInCartItemLongTappedEvent(m_bookInfo);
    return true;
}

void UIBookStoreBookInCartItem::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    switch (dwCmdId)
    {
    case ID_BTN_DELETE:
        UIBookStoreBookInCartItemStatusListener::GetInstance()->FireBookInCartItemDeleteEvent(m_bookInfo);
        break;
    default:
        UIListItemBase::OnCommand(dwCmdId, pSender, dwParam);
        break;
    }
}
