#include <tr1/functional>
#include "BookStore/BookStoreInfoManager.h"
#include "BookStore/BookStoreErrno.h"
#include "BookStoreUI/UIBookStoreBookInfo.h"
#include "BookStoreUI/UIBookStoreBookAbstractPage.h"
#include "BookStoreUI/UIBookStorePostAddCartPage.h"
#include "BookStoreUI/UIBookStoreCartPage.h"
#include "BookStoreUI/UIBookStoreTagListPage.h"
#include "PersonalUI/UIPersonalFavouritesPage.h"
#include "drivers/DeviceInfo.h"
#include "I18n/StringManager.h"
#include "Utility/ImageManager.h"
#include "Utility/PathManager.h"
#include "GUI/CTpGraphics.h"
#include "CommonUI/CPageNavigator.h"
#include "CommonUI/UIUtility.h"
#include "CommonUI/UIWeiboSharePage.h"
#include "GUI/GUISystem.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "Common/CAccountManager.h"
#include "Framework/CDisplay.h"

using namespace WindowsMetrics;
using namespace dk::bookstore;
using namespace dk::utility;
using namespace dk::account;

UIBookStoreBookInfo::UIBookStoreBookInfo(UIContainer* pParent, const std::string& bookId, const std::string& bookName, const std::string& duokanKey)
    : UITablePanel()
    , m_tagBtnLst(this)
    , m_bookId(bookId)
    , m_bookName(bookName)
    , m_buttonController(pParent, NULL, &m_buyButton, &m_readPartContentButton, &m_addFavouriteButton, &m_addToCartButton, duokanKey)
    , m_btnSizer(0)
    , m_midSizer(0)
    , m_middleBar(0)
{
#ifdef KINDLE_FOR_TOUCH
    m_gestureListener.SetBookStoreBookInfo(this);
    m_gestureDetector.SetListener(&m_gestureListener);
#endif
    SubscribeEvent(UIBookStorePriceButtonListener::EventPriceButtonUpdate, 
        *UIBookStorePriceButtonListener::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreBookInfo::OnMessagePriceButtonUpdate),
        this,
        std::tr1::placeholders::_1)
        );
    Init();
}

UIBookStoreBookInfo::~UIBookStoreBookInfo()
{
}

bool UIBookStoreBookInfo::OnMessagePriceButtonUpdate(const EventArgs& args)
{
    const PriceButtonEventArgs& msgArgs = (const PriceButtonEventArgs&)args;
    if (m_bookId == msgArgs.bookId)
    {
        switch (msgArgs.updateStatus)
        {
        case BookInCartUpdate:
            {
                if (msgArgs.resultCode == 0)
                {
                    UIBookStorePostAddCartPage *page = new UIBookStorePostAddCartPage(m_bookDetailedInfo);
                    if (page)
                    {
                        page->MoveWindow(0, 0, DeviceInfo::GetDisplayScreenWidth(), DeviceInfo::GetDisplayScreenHeight());
                        CPageNavigator::Goto(page);
                    }
                }
                else if (msgArgs.resultCode == CART_IS_FULL)
                {
                    {
                        CDisplay::CacheDisabler forceDraw;
                        UIUtility::SetScreenTips(StringManager::GetPCSTRById(BOOKSTORE_CART_FULL));
                    }
                    UIBookStoreCartPage *pPage = new UIBookStoreCartPage();
                    if (pPage)
                    {
                        pPage->MoveWindow(0, 0, DeviceInfo::GetDisplayScreenWidth(), DeviceInfo::GetDisplayScreenHeight());
                        CPageNavigator::Goto(pPage);
                    }
                }
            }
            break;
        case FavouriteBookStatusUpdate:
        {
            if (msgArgs.handled == 0)
            {
                UpdateSectionUI();
            }
            else if (msgArgs.resultCode == FAVOURITES_IS_FULL)
            {
                {
                    CDisplay::CacheDisabler forceDraw;
                    UIUtility::SetScreenTips(StringManager::GetPCSTRById(BOOKSTORE_FAVOURITES_FULL));
                }
                // Here is a bug of dead loop caused by adding event handler in slot function.
                /*UIBookStorePersonalFavouritesPage *pPage = new UIBookStorePersonalFavouritesPage();
                if (pPage)
                {
                    pPage->MoveWindow(0, 0, DeviceInfo::GetDisplayScreenWidth(), DeviceInfo::GetDisplayScreenHeight());
                    CPageNavigator::Goto(pPage);
                }*/
            }
        }
        default:
            UpdateSectionUI();
            break;
        }
    }
    return true;
}

bool UIBookStoreBookInfo::OnMessageBookDetailedInfoUpdate(const EventArgs& args)
{
    const BookInfoUpdateArgs& msgArgs = (const BookInfoUpdateArgs&)args;
    if (m_bookId == msgArgs.bookId)
    {
        if(msgArgs.succeeded)
        {
            m_bookDetailedInfo.reset((model::BookInfo*)(msgArgs.bookInfo->Clone()));
            UpdateMyBookDetailedInfo();
            m_buttonController.SetBookInfo(m_bookDetailedInfo);
        }
    }
    return true;
}

void UIBookStoreBookInfo::Init()
{
    m_tagList.clear();
    const int bookInfoFontSize = GetWindowFontSize(FontSize16Index);
    const int btnFontSize = GetWindowFontSize(FontSize18Index);
#ifdef KINDLE_FOR_TOUCH

    m_addFavouriteButton.Initialize(ID_BOOKSTORE_ADD_FAVOURITE, StringManager::GetStringById(BOOKSTORE_ADD_FAVOURITE), btnFontSize);
    m_addFavouriteButton.SetIcons(ImageManager::GetImage(IMAGE_TOUCH_FAVOUR),
                                  ImageManager::GetImage(IMAGE_TOUCH_FAVOUR),
                                  UIButton::ICON_LEFT);
    m_addFavouriteButton.SetUpdateOnPress(false);
    m_addFavouriteButton.SetUpdateOnUnPress(false);

    m_addToCartButton.Initialize(ID_BOOKSTORE_ADD_TO_CART, StringManager::GetStringById(BOOKSTORE_ADD_TO_CART), btnFontSize);
    m_addToCartButton.SetIcons(ImageManager::GetImage(IMAGE_BOOKSTORE_DETAIL_CART),
                               ImageManager::GetImage(IMAGE_BOOKSTORE_DETAIL_CART),
                               UIButton::ICON_LEFT);
    m_addToCartButton.SetUpdateOnPress(false);
    m_addToCartButton.SetUpdateOnUnPress(false);

    m_shareButton.Initialize(ID_BOOKSTORE_SHARE, StringManager::GetStringById(BOOKSTORE_SHARE), btnFontSize);
    m_shareButton.SetIcons(ImageManager::GetImage(IMAGE_BOOKSTORE_SHARE),
                           ImageManager::GetImage(IMAGE_BOOKSTORE_SHARE),
                           UIButton::ICON_LEFT);
    m_shareButton.SetUpdateOnPress(false);
    m_shareButton.SetUpdateOnUnPress(false);

    m_readPartContentButton.Initialize(ID_BOOKSTORE_READAPART,StringManager::GetStringById(BOOKSTORE_FREECHAPTERS),btnFontSize);
    m_readPartContentButton.SetBackgroundImage(IMAGE_TOUCH_WEIBO_SHARE);
    m_buyButton.Initialize(ID_BOOKSTORE_BUY,"",btnFontSize);
    m_buyButton.SetBackgroundImage(IMAGE_TOUCH_WEIBO_SHARE);
    m_abstruct.SetLineSpace(1);

    //const int btnWidth = GetWindowMetrics(UIBookStoreBookInfoBtnWidthIndex);
    const int btnHeight = GetWindowMetrics(UIPixelValue45Index);

    m_buyButton.SetMinHeight(btnHeight);
    m_buyButton.SetLeftMargin(0);
    m_buyButton.SetTopMargin(0);

    m_readPartContentButton.SetMinHeight(btnHeight);
    m_readPartContentButton.SetLeftMargin(0);
    m_readPartContentButton.SetTopMargin(0);
#else
    m_addFavouriteButton.Initialize(ID_BOOKSTORE_ADD_FAVOURITE, StringManager::GetPCSTRById(BOOKSTORE_ADD_FAVOURITE), KEY_RESERVED, btnFontSize, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_addFavouriteButton.SetVAlign(ALIGN_CENTER);
    m_addFavouriteButton.SetRightIcon(ImageManager::GetImage(IMAGE_TOUCH_FAVOUR));

    m_addToCartButton.Initialize(ID_BOOKSTORE_ADD_TO_CART, StringManager::GetPCSTRById(BOOKSTORE_ADD_TO_CART), KEY_RESERVED, btnFontSize, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_addToCartButton.SetVAlign(ALIGN_CENTER);
    m_addToCartButton.SetRightIcon(ImageManager::GetImage(IMAGE_BOOKSTORE_DETAIL_CART));

    m_shareButton.Initialize(ID_BOOKSTORE_SHARE, StringManager::GetPCSTRById(BOOKSTORE_SHARE), KEY_RESERVED, btnFontSize, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_shareButton.SetVAlign(ALIGN_CENTER);
    m_shareButton.SetRightIcon(ImageManager::GetImage(IMAGE_BOOKSTORE_SHARE));
    
    m_readPartContentButton.Initialize(ID_BOOKSTORE_READAPART,StringManager::GetPCSTRById(BOOKSTORE_FREECHAPTERS), KEY_RESERVED, btnFontSize, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_buyButton.Initialize(ID_BOOKSTORE_BUY, NULL, KEY_RESERVED, btnFontSize, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));

    m_moreAbstruct.Initialize(ID_BOOKSTORE_VIEW_ALL, StringManager::GetStringById(BOOKSTORE_VIEW_ALL), KEY_RESERVED, bookInfoFontSize, ImageManager::GetImage(IMAGE_RIGHT_ARROW));
    m_moreAbstruct.SetAlign(ALIGN_RIGHT);
    AppendChild(&m_moreAbstruct);
    m_bookCover.SetEnable(false);
#endif
    m_bookCover.Initialize(LARGE);
    m_abstruct.SetFontSize(bookInfoFontSize);
    m_abstruct.SetMode(UIAbstractText::TextMultiLine);
    m_abstruct.SetVAlign(ALIGN_LEFT);
    m_authorLabel.SetFontSize(bookInfoFontSize);;
    m_authorLabel.SetEnglishGothic();

    m_scoring.SetScore(0.0);
    m_scoring.SetEnable(false);
    m_scoreCount.SetFontSize(bookInfoFontSize);
    m_scoreCount.SetEnglishGothic();

    m_sizeLabel.SetFontSize(bookInfoFontSize);
    m_sizeLabel.SetEnglishGothic();
    m_tagLabel.SetFontSize(bookInfoFontSize);
    string tag = StringManager::GetPCSTRById(BOOK_TAG);
    tag += ":";
    m_tagLabel.SetText(tag.c_str());

    m_priceTitle.SetFontSize(bookInfoFontSize);
    m_price.SetFontSize(bookInfoFontSize);
    m_price.SetEnglishGothic();

    m_paperPriceTitle.SetFontSize(bookInfoFontSize);
    m_paperPrice.SetFontSize(bookInfoFontSize);
    m_paperPrice.SetEnglishGothic();

    AppendChild(&m_relatedBook);
    AppendChild(&m_readPartContentButton);
    AppendChild(&m_buyButton);
    AppendChild(&m_authorLabel);
    AppendChild(&m_sizeLabel);
    AppendChild(&m_tagLabel);
    AppendChild(&m_priceTitle);
    AppendChild(&m_price);
    AppendChild(&m_paperPriceTitle);
    AppendChild(&m_paperPrice);
    AppendChild(&m_abstruct);
    AppendChild(&m_bookCover);
    AppendChild(&m_scoring);
    AppendChild(&m_scoreCount);
}

void UIBookStoreBookInfo::InitUI()
{
    UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
    UISizer* topSizer = new UIBoxSizer(dkHORIZONTAL);
    UISizer* infoRightSizer = new UIBoxSizer(dkVERTICAL);
    UISizer* starSizer = new UIBoxSizer(dkHORIZONTAL);
    UISizer* sizeSizer = new UIBoxSizer(dkHORIZONTAL);
    UISizer* tagSizer = new UIBoxSizer(dkHORIZONTAL);

    m_middleBar = new UIButtonGroup(this);
    m_middleBar->SetMinHeight(GetWindowMetrics(UIMiddleBarHeightIndex));
    m_middleBar->SetSplitLineHeight(GetWindowMetrics(UIBottomBarSplitLineHeightIndex));

    m_midSizer = new UIBoxSizer(dkVERTICAL);
#ifdef KINDLE_FOR_TOUCH
    m_btnSizer = new UIGridSizer(1, 2, GetWindowMetrics(UIPixelValue22Index), 0);
#else
    m_btnSizer = new UIBoxSizer(dkHORIZONTAL);
#endif
    if (mainSizer && topSizer && sizeSizer && tagSizer && m_btnSizer && infoRightSizer && starSizer && m_midSizer)
    {
        const int controlSpacing = GetWindowMetrics(UIPixelValue15Index);

        m_bookCover.SetMinSize(dkSize(m_bookCover.GetWidth(), m_bookCover.GetHeight()));
        m_abstruct.SetMaxWidth(m_iWidth);

        starSizer->Add(&m_scoring, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
        starSizer->Add(&m_scoreCount, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));

        const int englishGothicOffset = GetWindowMetrics(UIPixelValue3Index);
        sizeSizer->Add(&m_sizeLabel, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
        sizeSizer->AddStretchSpacer();
        sizeSizer->Add(&m_priceTitle, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Border(dkLEFT, controlSpacing));
        sizeSizer->Add(&m_price, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Border(dkBOTTOM, englishGothicOffset));
        sizeSizer->Add(&m_paperPriceTitle, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Border(dkLEFT, controlSpacing));
        sizeSizer->Add(&m_paperPrice, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Border(dkBOTTOM, englishGothicOffset));

        tagSizer->Add(&m_tagLabel, UISizerFlags().Expand().Align(dkALIGN_CENTER_VERTICAL).Border(dkTOP, GetWindowMetrics(UIBookStoreBookInfoTagTitleTopMarginIndex)).ReserveSpaceEvenIfHidden());
        tagSizer->Add(&m_tagBtnLst, UISizerFlags().Expand().Align(dkALIGN_CENTER_VERTICAL).ReserveSpaceEvenIfHidden());

#ifdef KINDLE_FOR_TOUCH
        const int bookInfoMargin = GetWindowMetrics(UIPixelValue3Index);
        const int tagMargin = GetWindowMetrics(UIPixelValue13Index);
        dkAlignment moreAbstructAlign = dkALIGN_CENTER;
        const int btnMargin = controlSpacing >> 1;
        m_btnSizer->Add(&m_readPartContentButton, UISizerFlags(1).Expand().Border(dkRIGHT, btnMargin).ReserveSpaceEvenIfHidden());
        m_btnSizer->Add(&m_buyButton, UISizerFlags(1).Expand().Border(dkLEFT, btnMargin));
#else
        dkAlignment moreAbstructAlign = dkALIGN_RIGHT;
        const int bookInfoMargin = GetWindowMetrics(UIPixelValue8Index);
        const int tagMargin = GetWindowMetrics(UIPixelValue20Index);
        m_btnSizer->AddStretchSpacer();
        m_btnSizer->Add(&m_readPartContentButton);
        m_btnSizer->Add(&m_buyButton, UISizerFlags().Border(dkLEFT, controlSpacing));
#endif

        // Order Buttons Bar
        m_middleBar->AddButton(&m_addFavouriteButton, UISizerFlags(1).Expand().Center().Border());
        if (m_bookDetailedInfo->GetPrice() != 0 && m_bookDetailedInfo->GetDiscountPrice() != 0)
            m_middleBar->AddButton(&m_addToCartButton, UISizerFlags(1).Expand().Center().Border());
        else
            m_addToCartButton.SetEnable(FALSE);
        m_middleBar->AddButton(&m_shareButton, UISizerFlags(1).Expand().Center().Border());

        infoRightSizer->Add(&m_authorLabel, UISizerFlags().Expand());
        infoRightSizer->Add(starSizer, UISizerFlags().Expand().Border(dkTOP, bookInfoMargin));
        infoRightSizer->Add(sizeSizer, UISizerFlags().Expand().Border(dkTOP, bookInfoMargin));
        infoRightSizer->Add(tagSizer, UISizerFlags().Expand().Border(dkTOP | dkBOTTOM, tagMargin).ReserveSpaceEvenIfHidden());
        infoRightSizer->Add(m_btnSizer, UISizerFlags(1).Expand());

        m_midSizer->AddSpacer(GetWindowMetrics(UIPixelValue13Index));
        m_midSizer->Add(&m_abstruct, UISizerFlags(1).Expand());
#ifndef KINDLE_FOR_TOUCH
        m_midSizer->Add(&m_moreAbstruct, UISizerFlags().Align(moreAbstructAlign).Border(dkTOP, GetWindowMetrics(UIPixelValue14Index)));
#endif

        topSizer->Add(&m_bookCover);
        topSizer->Add(infoRightSizer, UISizerFlags(1).Expand().Border(dkLEFT, GetWindowMetrics(UIPixelValue22Index)));

        mainSizer->Add(topSizer, UISizerFlags().Expand().Border(dkTOP | dkBOTTOM, GetWindowMetrics(UIPixelValue16Index)));
        mainSizer->Add(m_middleBar, UISizerFlags().Expand());
        mainSizer->Add(m_midSizer,UISizerFlags(1).Expand().Border(dkBOTTOM, GetWindowMetrics(UIPixelValue8Index)));
        mainSizer->Add(&m_relatedBook,UISizerFlags().Expand().Border(dkBOTTOM, GetWindowMetrics(UIPixelValue13Index)));
        SetSizer(mainSizer);
    }
    else
    {
        SafeDeletePointer(mainSizer);
        SafeDeletePointer(topSizer);
        SafeDeletePointer(sizeSizer);
        SafeDeletePointer(tagSizer);
        SafeDeletePointer(m_btnSizer);
        SafeDeletePointer(infoRightSizer);
        SafeDeletePointer(m_midSizer);
        SafeDeletePointer(starSizer);
    }
}

void UIBookStoreBookInfo::GotoBookAbstructPage()
{
    if (!m_bookDetailedInfo)
    {
        return;
    }
    UIBookStoreBookAbstractPage *page = new UIBookStoreBookAbstractPage(m_bookDetailedInfo->GetContent());
    if (page)
    {
        page->MoveWindow(0, 0, DeviceInfo::GetDisplayScreenWidth(), DeviceInfo::GetDisplayScreenHeight());
        CPageNavigator::Goto(page);
    }
}

void UIBookStoreBookInfo::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    switch (dwCmdId)
    {
    case ID_BOOKSTORE_VIEW_ALL:
        GotoBookAbstructPage();
        break;
    case ID_BOOKSTORE_SHARE:
        {
            string displayText = StringManager::GetPCSTRById(ELEGANT_READING_TIME);
            displayText.append(StringManager::GetPCSTRById(SHAREWEIBO_HINT));
            char buf[1024] = {0};
            snprintf(buf, sizeof(buf)/sizeof(char), displayText.c_str(), m_bookDetailedInfo->GetTitle().c_str());
            UIWeiboSharePage* pPage = new UIWeiboSharePage(buf);
            if (pPage)
            {
                CPageNavigator::Goto(pPage);
            }
        }
        break;
    case ID_BOOKINFO_TAG_0:
    case ID_BOOKINFO_TAG_1:
    case ID_BOOKINFO_TAG_2:
    case ID_BOOKINFO_TAG_3:
    case ID_BOOKINFO_TAG_4:
    case ID_BOOKINFO_TAG_5:
    case ID_BOOKINFO_TAG_6:
    case ID_BOOKINFO_TAG_7:
    case ID_BOOKINFO_TAG_8:
    case ID_BOOKINFO_TAG_9:
        {
            int index = dwCmdId - ID_BOOKINFO_TAG_0;
            if (index >= 0 && index < (int)m_tagList.size())
            {
                string tag = m_tagList.at(index);
                if (!tag.empty())
                {
                    UIBookStoreTagListPage* pPage = new UIBookStoreTagListPage(tag.c_str());
                    if (pPage)
                    {
                        pPage->MoveWindow(0, 0, DeviceInfo::GetDisplayScreenWidth(),DeviceInfo::GetDisplayScreenHeight());
                        CPageNavigator::Goto(pPage);
                    }
                }
            }
        }
        break;
    default:
		break;
	}
}

std::string UIBookStoreBookInfo::GetBookTitle() const
{
    return m_bookName;
}

void UIBookStoreBookInfo::UpdateMyBookDetailedInfo()
{
    if(!m_bookDetailedInfo)
    {
        return;
    }
    string tmpValue = StringManager::GetPCSTRById(BOOK_AUTHOR);
    string author = m_bookDetailedInfo->GetAuthors();
    if (author.empty())
    {
        tmpValue = StringManager::GetPCSTRById(BOOK_EDITORS);
        author = m_bookDetailedInfo->GetEditors();
    }
    string bookTitle = m_bookDetailedInfo->GetTitle();
    if (!bookTitle.empty())
    {
        m_bookName = bookTitle;
    }
    
    tmpValue +=  (" " + author);
    m_authorLabel.SetText(tmpValue.c_str());

    char szTmp[64] = {0};
    tmpValue = StringManager::GetPCSTRById(BOOK_SIZE);
    if (m_bookDetailedInfo->GetSize() >= 1024768)
    {
        snprintf(szTmp, DK_DIM(szTmp), "%s %.2f MB",tmpValue.c_str(),m_bookDetailedInfo->GetSize() / 1048576.0);
    }
    else
    {
        snprintf(szTmp, DK_DIM(szTmp), "%s %.2f KB", tmpValue.c_str(), m_bookDetailedInfo->GetSize() / 1024.0);
    }
    m_sizeLabel.SetText(szTmp);

    int discountPrice = m_bookDetailedInfo->GetDiscountPrice();
    int paperPrice = m_bookDetailedInfo->GetPaperPrice();
    int price = m_bookDetailedInfo->GetPrice();
    if (discountPrice >= 0 && price >= 0)
    {
        m_priceTitle.SetVisible(true);
        char priceTitle[128] = {0};
        snprintf(priceTitle, DK_DIM(priceTitle), "%s ", StringManager::GetPCSTRById(BOOK_ORIGIN_PRICE));
        m_priceTitle.SetText(priceTitle);

        m_price.SetVisible(true);
        char szPrice[128] = {0};
        snprintf(szPrice, DK_DIM(szPrice), "%s%.2f", StringManager::GetPCSTRById(BOOKSTORE_RMB), (float)price/100);
        m_price.SetText(szPrice);
    }
    else
    {
        m_priceTitle.SetVisible(false);
        m_price.SetVisible(false);
    }

    if (paperPrice > 0)
    {
        m_paperPriceTitle.SetVisible(true);
        char paperPriceTitle[128] = {0};
        snprintf(paperPriceTitle, DK_DIM(paperPriceTitle), "%s ", StringManager::GetPCSTRById(BOOK_PAPER_PRICE));
        m_paperPriceTitle.SetText(paperPriceTitle);

        m_paperPrice.SetVisible(true);
        char szPaperPrice[128] = {0};
        snprintf(szPaperPrice, DK_DIM(szPaperPrice), "%s%.2f", StringManager::GetPCSTRById(BOOKSTORE_RMB), (float)paperPrice/100);
        m_paperPrice.SetText(szPaperPrice);
    }
    else
    {
        m_paperPriceTitle.SetVisible(false);
        m_paperPrice.SetVisible(false);
    }

    m_tagList = m_bookDetailedInfo->GetTagList();
    printf("m_tagList.size = %d\n", m_tagList.size());
    int tagSize = (int)m_tagList.size();
    const int minSize = dk_min(tagSize, 10);
    if (minSize > 0)
    {
        //m_tagBtnLst.SetSplitLineWidth(GetWindowMetrics(UIPixelValue3Index));
        m_tagBtnLst.SetMinHeight(GetWindowMetrics(UIPixelValue16Index));
        m_tagBtnLst.SetTopLinePixel(0);
        m_tagBtnLst.SetBottomLinePixel(0);
        m_tagBtnLst.SetSplitLineHeight(GetWindowMetrics(UIPixelValue15Index));

        m_tagLabel.SetVisible(true);
        const int tagBtnSize = GetWindowFontSize(FontSize16Index);
        for (int i = 0; i < minSize; ++i)
        {
            string tag = m_tagList.at(i);
#ifdef KINDLE_FOR_TOUCH
            m_tagBtn[i].Initialize(ID_BOOKINFO_TAG_0 + i, tag.c_str(), tagBtnSize);
            m_tagBtn[i].SetDo2Gray(false);
            m_tagBtnLst.AddButton(m_tagBtn + i, UISizerFlags().Expand().Center().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIPixelValue13Index)));
#else
            m_tagBtn[i].Initialize(ID_BOOKINFO_TAG_0 + i, tag.c_str(), KEY_RESERVED, tagBtnSize, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
            m_tagBtn[i].SetFontSize(tagBtnSize);
            m_tagBtnLst.AddButton(m_tagBtn + i, UISizerFlags().Center().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIPixelValue13Index)));
#endif
        }
    }
    else
    {
        m_tagLabel.SetVisible(false);
    }
    
    m_scoring.SetScore(m_bookDetailedInfo->GetScore());
    snprintf(szTmp, DK_DIM(szTmp), " (%d)", m_bookDetailedInfo->GetScoreCount());
    m_scoreCount.SetText(szTmp);

    m_abstruct.SetText(m_bookDetailedInfo->GetContent().c_str());
#ifndef KINDLE_FOR_TOUCH
    m_moreAbstruct.SetVisible(true);
#endif

    m_bookCover.SetCoverUrl(m_bookDetailedInfo->GetCoverUrl());
    m_bookCover.SetVisible(true);

    // Do Not display cart button if it's free book
    if (m_bookDetailedInfo->GetPrice() == 0 || m_bookDetailedInfo->GetDiscountPrice() == 0)
    {
        m_addToCartButton.SetVisible(FALSE);
        m_addToCartButton.SetEnable(FALSE);
    }

    m_relatedBook.SetBookInfo(m_bookDetailedInfo);
    InitUI();

#ifndef KINDLE_FOR_TOUCH
    m_buyButton.SetFocus(true);
    OnChildSetFocus(&m_buyButton);
#endif
}

HRESULT UIBookStoreBookInfo::DrawBackGround(DK_IMAGE drawingImg)
{
    HRESULT hr(S_OK);
    CTpGraphics grf(drawingImg);
    RTN_HR_IF_FAILED(grf.EraserBackGround());

    if (m_midSizer)
    {
        const int topMargin = m_midSizer->GetPosition().y;
        const int lineHeight = 1;
        grf.DrawLine(0, topMargin, m_iWidth, lineHeight, SOLID_STROKE);
    }
    return hr;
}

BOOL UIBookStoreBookInfo::OnKeyPress(INT32 iKeyCode)
{
#ifndef KINDLE_FOR_TOUCH
    //TODO(JUGH)
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (!SendKeyToChildren(iKeyCode))
    {
        DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s if (!SendKeyToChildren(iKeyCode))", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return FALSE;
    }

    switch (iKeyCode)
    {
    case KEY_UP:
        if (m_tagBtnLst.IsFocus() || ((m_readPartContentButton.IsFocus() || m_buyButton.IsFocus()) && !m_tagLabel.IsVisible()))
        {
            SetFocus(false);
            return TRUE;
        }
        else if (m_relatedBook.IsFocus())
        {
            m_relatedBook.SetFocus(FALSE);
            m_moreAbstruct.SetFocus(TRUE);
        }
        else if (m_moreAbstruct.IsFocus())
        {
            m_moreAbstruct.SetFocus(FALSE);
            if (m_middleBar)
            {
                m_middleBar->SetFocus(TRUE);
            }
            else
            {
                m_buyButton.SetFocus(TRUE);
            }
        }
        else if (m_middleBar && m_middleBar->IsFocus())
        {
            m_buyButton.SetFocus(TRUE);
        }
        else
        {
            MoveFocus(DIR_TOP);
        }
        break;
    case KEY_DOWN:
        if (!IsFocus())
        {
            if (m_tagLabel.IsVisible())
            {
                m_tagBtnLst.SetFocus(TRUE);
            }
            else
            {
                m_buyButton.SetFocus(TRUE);
            }
        }
        else if (m_readPartContentButton.IsFocus() || m_buyButton.IsFocus())
        {
            if (m_middleBar)
            {
                m_middleBar->SetFocus(TRUE);
            }
            else
            {
                m_moreAbstruct.SetFocus(TRUE);
            }
        }
        else if (m_middleBar && m_middleBar->IsFocus())
        {
            m_moreAbstruct.SetFocus(TRUE);
        }
        else if (m_relatedBook.IsFocus())
        {
            m_relatedBook.SetFocus(FALSE);
            SetFocus(false);
            return TRUE;
        }
        else
        {
            MoveFocus(DIR_DOWN);
        }
        break;
    default:
        if(SendHotKeyToChildren(iKeyCode))
        {
            DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s default", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            return UIContainer::OnKeyPress(iKeyCode);
        }
        break;
    }
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
#endif
    return FALSE;
}

void UIBookStoreBookInfo::UpdateSectionUI(bool bIsFull)
{
    if (m_btnSizer && m_middleBar)
    {
        m_buttonController.UpdateBookStatus();
        m_buttonController.UpdateTrialBookStatus();
        m_buttonController.UpdateFavouriteBookStatus();
        m_buttonController.UpdateBookInCartStatus();
        m_btnSizer->Layout();
        m_middleBar->Layout();
        if (GUISystem::GetInstance()->GetTopFullScreenContainer())
        {
            GUISystem::GetInstance()->GetTopFullScreenContainer()->UpdateWindow();
        }
        else
        {
            UpdateWindow();
        }
    }
}

#ifdef KINDLE_FOR_TOUCH
bool UIBookStoreBookInfo::OnTouchEvent(MoveEvent* moveEvent)
{
    m_gestureDetector.OnTouchEvent(moveEvent);
    return true;
}

bool UIBookStoreBookInfo::OnSingleTapUp(MoveEvent* moveEvent)
{
    if (moveEvent)
    {
        int y = moveEvent->GetY();
        if (y >= m_abstruct.GetY() && y < (m_abstruct.GetY() + m_abstruct.GetHeight()))
        {
            GotoBookAbstructPage();
        }
    }
    return true;
}
#endif

