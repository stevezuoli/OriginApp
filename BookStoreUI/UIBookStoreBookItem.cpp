#include "BookStoreUI/UIBookStoreBookItem.h"
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


const char* UIBookStoreBookItemStatusListener::EventBookItemLongTapped = "EventBookItemLongTapped";

UIBookStoreBookItemStatusListener* UIBookStoreBookItemStatusListener::GetInstance()
{
    static UIBookStoreBookItemStatusListener listner;
    return &listner;
}

void  UIBookStoreBookItemStatusListener::FireBookItemLongTappedEvent(dk::bookstore::model::BookInfoSPtr info)
{
    BookItemEventArgs args;
    args.bookInfo = info;
    FireEvent(EventBookItemLongTapped, args);
}


UIBookStoreBookItem::UIBookStoreBookItem(bool showScore)
    : m_showScore(showScore)
    , m_buttonController(this, &m_originPriceLine, &m_responseWnd)
{
    SubscribeEvent(UIBookStorePriceButtonListener::EventPriceButtonUpdate, 
        *UIBookStorePriceButtonListener::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreBookItem::OnMessagePriceButtonUpdate),
        this,
        std::tr1::placeholders::_1)
        );
    Init();
}

UIBookStoreBookItem::~UIBookStoreBookItem()
{

}

void UIBookStoreBookItem::Init()
{
    const int briefFontSize = GetWindowFontSize(UIListItemBaseSmallFontSizeIndex);
    const int authorFontSize = GetWindowFontSize(UIListItemBaseMidFontSizeIndex);
    const int titleFontSize = GetWindowFontSize(UIListItemBaseTitleFontSizeIndex);
    const int priceFontSize = GetWindowFontSize(FontSize16Index);
    m_bookCover.Initialize(SMALL);
    m_bookCover.SetEnable(false);
    AppendChild(&m_bookCover);

    m_bookTitle.SetFontSize(titleFontSize);
    m_bookTitle.SetEndWithEllipsis(true);
    AppendChild(&m_bookTitle);

    m_bookAuthor.SetFontSize(authorFontSize);
    AppendChild(&m_bookAuthor);

	m_labelPurchasedTime.SetForeGroundColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
	m_labelPurchasedTime.SetFontSize(authorFontSize);
	AppendChild(&m_labelPurchasedTime);
	
    m_responseWnd.SetFontSize(priceFontSize);
#ifdef KINDLE_FOR_TOUCH
    m_responseWnd.SetTouchSlot(GetWindowMetrics(UIPixelValue15Index));
#endif
    AppendChild(&m_responseWnd);

    m_originPriceLine.SetFontSize(priceFontSize);
    AppendChild(&m_originPriceLine);

    m_scoring.SetScore(0.0);
    m_scoring.SetEnable(false);
    m_scoring.SetVisible(m_showScore);
    AppendChild(&m_scoring);

    m_summary.SetFontSize(briefFontSize);
    m_summary.SetEndWithEllipsis(true);
    AppendChild(&m_summary);

    if (!m_windowSizer)
    {
        const int margin = GetWindowMetrics(UIPixelValue15Index);
        const int border = GetWindowMetrics(UIPixelValue10Index);
#ifdef KINDLE_FOR_TOUCH
        m_responseWnd.SetMinSize(dkSize(GetWindowMetrics(UIBookStoreRecommendListItemButtonWidthIndex), GetWindowMetrics(UIBookStoreRecommendListItemButtonHeightIndex)));
#else
        m_bookCover.SetMinSize(dkSize(m_bookCover.GetWidth(), m_bookCover.GetHeight()));
#endif
        UISizer* pMainSizer = new UIBoxSizer(dkVERTICAL);
        UISizer* mainSizer = new UIBoxSizer(dkHORIZONTAL);
        if (pMainSizer && mainSizer)
        {
            mainSizer->Add(&m_bookCover, UISizerFlags().Border(dkTOP, margin));
            mainSizer->AddSpacer(margin);

            UISizer* bookContentInfoSizer = new UIBoxSizer(dkVERTICAL);
            if (bookContentInfoSizer)
            {
                UISizer* pTitleSizer = new UIBoxSizer(dkHORIZONTAL);
                if (pTitleSizer)
                {
                    pTitleSizer->Add(&m_bookTitle, UISizerFlags().Border(dkRIGHT, border).Reduce(1));
                    if (m_showScore)
                    {
                        pTitleSizer->Add(&m_scoring, UISizerFlags().Border(dkRIGHT, border).Center());
                    }
                    bookContentInfoSizer->Add(pTitleSizer, UISizerFlags().Expand().Border(dkRIGHT, border).Border(dkBOTTOM, border));
                    
                }
                bookContentInfoSizer->Add(&m_bookAuthor, UISizerFlags().Expand().Border(dkBOTTOM, border));
                bookContentInfoSizer->Add(&m_summary, UISizerFlags().Expand().Border(dkBOTTOM, border));
				bookContentInfoSizer->Add(&m_labelPurchasedTime, UISizerFlags().Expand().Border(dkBOTTOM, border));
                mainSizer->Add(bookContentInfoSizer, UISizerFlags(1).Border(dkTOP, margin).Expand().Reduce(1));
            }

            UISizer* pPriceSizer = new UIBoxSizer(dkVERTICAL);
            if (pPriceSizer)
            {
#ifdef KINDLE_FOR_TOUCH
                pPriceSizer->Add(&m_responseWnd, UISizerFlags().Align(dkALIGN_CENTER_HORIZONTAL));
#else
                pPriceSizer->Add(&m_responseWnd, UISizerFlags().Align(dkALIGN_RIGHT));
#endif
                pPriceSizer->Add(&m_originPriceLine, UISizerFlags().Align(dkALIGN_CENTER_HORIZONTAL).Border(dkTOP, border));
                mainSizer->Add(pPriceSizer, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Border(dkTOP, border).Border(dkBOTTOM, margin));
            }

            pMainSizer->AddStretchSpacer();
            pMainSizer->Add(mainSizer, UISizerFlags().Expand());
            pMainSizer->AddStretchSpacer();
            SetSizer(pMainSizer);
        }
        else
        {
            SafeDeletePointer(pMainSizer);
            SafeDeletePointer(mainSizer);
        }
    }
}


void UIBookStoreBookItem::SetInfoSPtr(dk::bookstore::model::BasicObjectSPtr info, int index)
{
    m_bookInfo.reset((model::BookInfo*)info.get()->Clone());
    model::BookInfo* bookInfo = (model::BookInfo*)(m_bookInfo.get());
    if(NULL == bookInfo)
    {
        return;
    }
    m_buttonController.SetBookInfo(m_bookInfo);
    if (index > 0)
    {
        string bookTitle = bookInfo->GetTitle();
        int length = bookTitle.size();
        char *pTmpTitle = new char[length + 64];
        sprintf(pTmpTitle,"%d. %s",index,bookTitle.c_str());
        m_bookTitle.SetText(pTmpTitle);
        delete [] pTmpTitle;
    }
    else
    {
        m_bookTitle.SetText(bookInfo->GetTitle().c_str());
    }
    m_bookAuthor.SetText(bookInfo->GetAuthorsOrEditors().c_str());
    m_summary.SetText(bookInfo->GetSummary().c_str());
    if (m_showScore)
    {
        m_scoring.SetVisible(bookInfo->GetScore() >= 0);
        m_scoring.SetScore(bookInfo->GetScore());
    }
    m_bookCover.SetCoverUrl(bookInfo->GetCoverUrl());

	time_t pruchasedTime = bookInfo->GetPurchasedUTCTime();
	if(pruchasedTime != 0)
	{
		struct tm fullPurchasedTime = *gmtime(&pruchasedTime);
		char buf[128] = {0};
		snprintf(buf, DK_DIM(buf), StringManager::GetPCSTRById(PERSONAL_MYBOOK_PURCHASED),
			fullPurchasedTime.tm_year + 1900, fullPurchasedTime.tm_mon + 1, fullPurchasedTime.tm_mday);
		m_labelPurchasedTime.SetText(buf);
		m_labelPurchasedTime.SetVisible(true);
		m_summary.SetVisible(false);
	}
	else
	{
		m_labelPurchasedTime.SetVisible(false);
		m_summary.SetVisible(true);
	}
}


HRESULT UIBookStoreBookItem::DrawBackGround(DK_IMAGE drawingImg)
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

bool UIBookStoreBookItem::ResponseTouchTap()
{
    dk::bookstore::model::BookInfo* bookInfo = (dk::bookstore::model::BookInfo*)(m_bookInfo.get());
    UIBookDetailedInfoPage* bookInfoPage = new UIBookDetailedInfoPage(bookInfo->GetBookId(),bookInfo->GetTitle()); 
    bookInfoPage->MoveWindow(0,0,CDisplay::GetDisplay()->GetScreenWidth(),CDisplay::GetDisplay()->GetScreenHeight());
    CPageNavigator::Goto(bookInfoPage);
    return true;
}

bool UIBookStoreBookItem::OnMessagePriceButtonUpdate(const EventArgs& args)
{
    const PriceButtonEventArgs& msgArgs = (const PriceButtonEventArgs&)args;
    if (m_bookInfo && (m_bookInfo->GetBookId() == msgArgs.bookId))
    {
        UpdateSectionUI();
    }
    return true;
}

void UIBookStoreBookItem::UpdateSectionUI(bool bIsFull)
{
    m_buttonController.UpdateBookStatus();
    Layout();
    UpdateWindow();
}

bool UIBookStoreBookItem::ResponseOperation()
{
    UIBookStoreBookItemStatusListener::GetInstance()->FireBookItemLongTappedEvent(m_bookInfo);
    return true;
}
