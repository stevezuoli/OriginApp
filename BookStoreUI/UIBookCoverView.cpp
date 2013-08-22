#include <tr1/functional>
#include "BookStoreUI/UIBookCoverView.h"
#include "BookStore/BookStoreInfoManager.h"
#include "ImageHandler/ImageHandler.h"
#include "Utility/ImageManager.h"
#include "Utility/PathManager.h"
#include "../ImageHandler/DkImageHelper.h"
#include "GUI/CTpGraphics.h"
#include "Utility/ColorManager.h"
#include "CommonUI/UIUtility.h"
#include "Common/WindowsMetrics.h"

#ifdef KINDLE_FOR_TOUCH
    #include "GUI/GUISystem.h"
    #include "interface.h"
    #include "Utility/SystemUtil.h"
    using dk::utility::SystemUtil;
#else
    #include "Framework/CDisplay.h"
#endif

using namespace WindowsMetrics;
using namespace dk::bookstore;
using dk::utility::PathManager;
using namespace DkFormat;

static const INT32 BORDERHEIGHT = 1;
static const INT32 COVERMAGIN = 6;

#ifndef KINDLE_FOR_TOUCH
const char* UIBookCoverView::EventBookCoverViewTouched = "EventBookCoverViewTouched";
#endif
UIBookCoverView::UIBookCoverView()
    : UIContainer(NULL,IDSTATIC)
    //, m_coverImage(this,IDSTATIC)
    , m_coverImage(this)
    , m_coverStyle(SMALL)
    , m_defaultCoverId(IMAGE_BOOKSTORE_DEFAULTBOOKCOVER)
{
    AppendChild(&m_coverImage);
    ShowBorder(true);
#ifdef KINDLE_FOR_TOUCH
    SetClickable(true);
#endif
    SetEnable(true);
    //m_coverImage.SetAutoSize(false);
    SubscribeMessageEvent(BookStoreInfoManager::EventCoverImageUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookCoverView::OnMessageCoverImageUpdate),
        this,
        std::tr1::placeholders::_1)
        );
    Init();
}

UIBookCoverView::UIBookCoverView(UIContainer* pParent, const DWORD dwId)
    : UIContainer(pParent,dwId)
    , m_coverImage(this)
    , m_coverStyle(SMALL)
    , m_defaultCoverId(IMAGE_BOOKSTORE_DEFAULTBOOKCOVER)
{
    AppendChild(&m_coverImage);
    ShowBorder(true);
#ifdef KINDLE_FOR_TOUCH
    SetClickable(true);
#endif
    SetEnable(true);
    //m_coverImage.SetAutoSize(false);
}

UIBookCoverView::~UIBookCoverView()
{
}

void UIBookCoverView::Initialize(BookCoverFrameStyle coverStyle, int defaultCoverId, const std::string& coverUrl)
{
    SetCoverFrameStyle(coverStyle);
    m_defaultCoverId = defaultCoverId;
    SetCoverUrl(coverUrl);
}

void UIBookCoverView::SetCoverUrl(const std::string& coverUrl)
{
    if (coverUrl.empty())
    {
        m_coverUrl = coverUrl;
        std::string defaultCoverPath = ImageManager::GetImagePath(m_defaultCoverId);
        if (!defaultCoverPath.empty())
        {
            m_coverImage.SetImageFile(defaultCoverPath.c_str());
        }
    }
    else
    {
        std::string normalizedCoverUrl = PathManager::NormalizeCoverImageUrl(coverUrl, m_iWidth, m_iHeight);
        if(m_coverUrl != normalizedCoverUrl && !normalizedCoverUrl.empty())
        {
            m_coverUrl = normalizedCoverUrl;
            string coverImagePath = PathManager::CoverImageUrlToFile(m_coverUrl.c_str());
            DebugPrintf(DLC_XU_KAI,"UIBookCoverView m_coverUrl : %s;coverImagePath is %s\n",m_coverUrl.c_str(), coverImagePath.c_str());
            if((coverImagePath.empty()) || (!PathManager::IsFileExisting(coverImagePath.c_str())))
            {
                RequestCoverFromNetwork();
            }
        }
        m_textBookTitle.SetVisible(false);
    }
}

void UIBookCoverView::RequestCoverFromNetwork()
{
    BookStoreInfoManager* manager = BookStoreInfoManager::GetInstance();
    manager->FetchCoverImage(m_coverUrl.c_str());
}

void UIBookCoverView::SetCoverFrameStyle(BookCoverFrameStyle style)
{
    if(m_coverStyle != style)
    {
        m_coverStyle = style;
        Init();
    }
}

void UIBookCoverView::Init()
{
    switch (m_coverStyle)
    {
    case SMALL:
        m_iWidth = GetWindowMetrics(UIBookCoverViewSmallWidthIndex);
        m_iHeight = GetWindowMetrics(UIBookCoverViewSmallHeightIndex);
        m_minWidth = GetWindowMetrics(UIBookCoverViewSmallWidthIndex);
        m_minHeight = GetWindowMetrics(UIBookCoverViewSmallHeightIndex);
        break;
    case LARGE:
        m_iWidth = GetWindowMetrics(UIBookCoverViewBigWidthIndex);
        m_iHeight = GetWindowMetrics(UIBookCoverViewBigHeightIndex);
        m_minWidth = GetWindowMetrics(UIBookCoverViewBigWidthIndex);
        m_minHeight = GetWindowMetrics(UIBookCoverViewBigHeightIndex);
        break;
    default:
        break;
    }
    AppendChild(&m_textBookTitle);
    m_textBookTitle.SetVisible(false);
    m_textBookTitle.SetMode(UIAbstractText::TextMultiLine);
    m_textBookTitle.SetFontSize(GetWindowFontSize(FontSize12Index));
    m_textBookTitle.SetAlign(ALIGN_CENTER);
    m_textBookTitle.SetVAlign(VALIGN_TOP);
    m_textBookTitle.SetEraseBackground(false);
}

void UIBookCoverView::MoveWindow(INT32 left, INT32 top, INT32 width, INT32 height)
{
    UIContainer::MoveWindow(left, top, width, height);
    int coverLeft = m_iLeftMargin, coverTop = m_iTopMargin;
    int coverWidth = m_iWidth - (m_iLeftMargin << 1);
    int coverHeight = m_iHeight - (m_iLeftMargin << 1);
    m_coverImage.MoveWindow(coverLeft, coverTop, coverWidth, coverHeight);

    std::string defaultCoverPath = ImageManager::GetImagePath(m_defaultCoverId);
    if (!defaultCoverPath.empty())
    {
        m_coverImage.SetImageFile(defaultCoverPath.c_str());
    }

    int horizonMargin = GetWindowMetrics(UIPixelValue10Index);
    int verticalMargin = height / 5.0;
    int textWidth = width - (horizonMargin << 1);
    int textHeight = height * 3/5.0;
    m_textBookTitle.SetMaxWidth(textWidth);
    m_textBookTitle.MoveWindow(horizonMargin, verticalMargin, textWidth, textHeight);
}

HRESULT UIBookCoverView::DrawBackGround(DK_IMAGE drawingImg)
{
    HRESULT hr(S_OK);
    CTpGraphics grf(drawingImg);
    RTN_HR_IF_FAILED(grf.EraserBackGround());
#ifdef KINDLE_FOR_TOUCH
    if (m_showBorder)
    {
        int left = 0, top = 0, right = m_iWidth, bottom = m_iHeight;
        RTN_HR_IF_FAILED(grf.FillRect(left, top, right, bottom, ColorManager::GetColor(COLOR_DISABLECOLOR)));
        RTN_HR_IF_FAILED(grf.FillRect(left + m_iLeftMargin, top + m_iTopMargin, right - m_iLeftMargin, bottom - m_iTopMargin, ColorManager::knWhite));
    }
#else
    if (m_showBorder)
    {
        if (IsFocus())
        {
            UIUtility::DrawBorder(drawingImg, 0, 0, 0, 0);
        }
        else
        {
            const int left = m_iLeftMargin - BORDERHEIGHT;
            const int top = m_iTopMargin - BORDERHEIGHT;
            const int right = m_iWidth - m_iLeftMargin + BORDERHEIGHT;
            const int bottom = m_iHeight - m_iTopMargin + BORDERHEIGHT;
            RTN_HR_IF_FAILED(grf.FillRect(left, top, right, bottom, ColorManager::GetColor(COLOR_DISABLECOLOR)));
            RTN_HR_IF_FAILED(grf.FillRect(left + BORDERHEIGHT, top + BORDERHEIGHT, right - BORDERHEIGHT, bottom - BORDERHEIGHT, ColorManager::knWhite));
        }
    }
#endif
    UpdateMyCoverImage();// 本地加载封面图

    return hr;
}

bool UIBookCoverView::OnMessageCoverImageUpdate(const EventArgs& args)
{
    const CoverImageUpdateArgs& msgArgs = dynamic_cast<const CoverImageUpdateArgs&>(args);
    if(msgArgs.succeeded && msgArgs.imageUrl == m_coverUrl)
    {
        UpdateWindow();
    }
    else if (!msgArgs.succeeded)
    {
        DebugPrintf(DLC_XU_KAI,"ERROR  msgArgs.imageUrl : %s",msgArgs.imageUrl.c_str());
    }
    
    return true;
}

void UIBookCoverView::UpdateMyCoverImage()
{
    if (m_coverUrl.empty())
    {
        return;
    }
    string coverImagePath = PathManager::CoverImageUrlToFile(m_coverUrl.c_str());
    if (!coverImagePath.empty() && PathManager::IsFileExisting(coverImagePath.c_str()))
    {
        m_coverImage.SetImageFile(coverImagePath.c_str());
    }
}

void UIBookCoverView::SetBookTitle(const char* title)
{
    bool displayTitle = title && m_coverUrl.empty();
    m_textBookTitle.SetVisible(displayTitle);
    if (displayTitle)
    {
        m_textBookTitle.SetText(title);
    }
}

void UIBookCoverView::SetEnable(BOOL bEnable)
{
#ifdef KINDLE_FOR_TOUCH
    const int margin = m_showBorder ? BORDERHEIGHT : 0;
#else
    const int margin = m_showBorder ? (bEnable ? COVERMAGIN : BORDERHEIGHT) : 0;
#endif
    SetLeftMargin(margin);
    SetTopMargin(margin);
    UIWindow::SetEnable(bEnable);
}

void UIBookCoverView::ShowBorder(bool showBorder) 
{
    m_showBorder = showBorder; 
#ifdef KINDLE_FOR_TOUCH
    const int margin = m_showBorder ? BORDERHEIGHT : 0;
#else
    const int margin = m_showBorder ? (IsEnable() ? COVERMAGIN : BORDERHEIGHT) : 0;
#endif
    SetLeftMargin(margin);
    SetTopMargin(margin);
}

#ifdef KINDLE_FOR_TOUCH

bool UIBookCoverView::OnChildClick(UIWindow* child)
{
    UIContainer* pParent = GetParent();
    if (child == &m_coverImage && NULL != pParent)
    {
        return pParent->OnChildClick(this);
    }
    return true;
}

#else

BOOL UIBookCoverView::OnKeyPress(INT32 iKeyCode)
{
    switch(iKeyCode)
    {
    case KEY_OKAY:
    case KEY_ENTER:
        {
            DebugPrintf(DLC_JUGH, "UIBookCoverView::OnKeyPress () KEY_OKAY");
            BookCoverViewTouchedEventArgs args;
            args.m_coverUrl = m_coverUrl;
            FireEvent(EventBookCoverViewTouched,args);
        }
        return FALSE;
    default:
        {
            DebugPrintf(DLC_JUGH, "UIBookCoverView::OnKeyPress () default");
        }
    }
    return true;
}

HRESULT UIBookCoverView::UpdateFocus()
{
    return UpdateWindow();
}
#endif


