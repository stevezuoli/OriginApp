#include "CommonUI/UICoverViewItem.h"
#include "GUI/CTpGraphics.h"
#include "Utility/ColorManager.h"
#include "Common/WindowsMetrics.h"
#include "Framework/CDisplay.h"
#include "Common/BookCoverLoader.h"
#include <tr1/functional>

using namespace WindowsMetrics;
static const int PROGRESS_LEFT = 3;
static const int PROGRESS_TOP_PADDING = 14;
static const int PROGRESS_INTERVAL = 7;
static const int PROGRESS_HEIGHT = 4;
static const int PROGRESS_HEIGHT_SMALL = 2;
static const int TRIAL_TOP_OUTSIDE = 3;
static const int NEW_LEFT = 3;
static const int NEW_TOP_PADDING = 6;
static const int INNER_COVER_LEFT = 1;
static const int INNER_COVER_TOP = 1;
static const int SHADDOW_TOP = 5;
static const int SHADDOW_LEFT = 3;
static const int SHADDOW_RIGHT_EXCEEDED = 2;
static const int SHADDOW_BOTTOM_EXCEEDED = 3;
static const int FORMAT_RIGHT_EXCEEDED = 4;
#ifndef KINDLE_FOR_TOUCH
static const int BORDER_WIDTH = 3;
static const int BORDER_INTERVAL = 2;
#else
static const int BORDER_WIDTH = 0;
static const int BORDER_INTERVAL = 0;
#endif

UICoverViewItem::UICoverViewItem(UIContainer* parent,
                                 BookListUsage usage,
                                 int minWidth,
                                 int minHeight,
                                 int overallWidth,
                                 int overallHeight)
    : UIContainer(parent)
    , m_imgFormat(this)
    , m_imgTrial(this)
    , m_imgSelect(this)
    , m_fileFormat(DFF_ElectronicPublishing)
    , m_isDuoKanBook(false)
    , m_isDir(false)
    , m_selected(false)
    , m_isTrial(false)
    , m_isNew(false)
    , m_usage(usage)
    , m_innerWidth(minWidth)
    , m_innerHeight(minHeight)
    , m_overallWidth(overallWidth)
    , m_overallHeight(overallHeight)
    , m_isLoading(false)
    , m_highlight(false)
{
    InitUI();
    SubscribeMessageEvent(BookCoverLoader::EventBookCoverLoaded,
            *BookCoverLoader::GetInstance(),
            std::tr1::bind(
            std::tr1::mem_fn(&UICoverViewItem::OnCoverLoadeded),
            this,
            std::tr1::placeholders::_1));
}

void UICoverViewItem::InitUI()
{
    m_imgNew.SetAutoSize(false);
    
    m_txtDirNameOrBookName.SetMode(UIAbstractText::TextMultiLine);
    m_txtDirBookNumOrBookAuthor.SetMaxWidth(GetWindowMetrics(UICoverViewItemDirNameMaxWidthIndex));
    m_txtDirNameOrBookName.SetMaxWidth(GetWindowMetrics(UICoverViewItemDirNameMaxWidthIndex));
    m_txtDirNameOrBookName.SetFontSize(GetWindowFontSize(FontSize16Index));
    m_txtDirBookNumOrBookAuthor.SetFontSize(GetWindowFontSize(FontSize12Index));
    m_txtDirNameOrBookName.SetAlign(ALIGN_CENTER);

    m_txtItemNameLine.SetMode(UIAbstractText::TextMultiLine);
    //m_txtItemNameLine.SetEndWithEllipsis(true);
    //m_txtItemNameLine.SetMaxWidth(m_overallWidth - (SHADDOW_LEFT << 1));
    //m_txtItemNameLine.SetMinWidth(m_overallWidth - (SHADDOW_LEFT << 1));
    //m_txtItemNameLine.SetMinHeight(GetWindowMetrics(UICoverViewItemBookNameMaxHeightIndex));
    m_txtItemNameLine.SetFontSize(GetWindowFontSize(FontSize16Index));
    m_txtItemNameLine.SetAutoModifyHeight(false);
    m_txtItemNameLine.SetMaxHeight((GetWindowFontSize(FontSize16Index) << 1) + 16);
    m_txtItemNameLine.SetAlign(ALIGN_LEFT);
    m_txtReadingProgressLine.SetMaxWidth(GetWindowMetrics(UICoverViewItemDirNameMaxWidthIndex));
    m_txtReadingProgressLine.SetFontSize(GetWindowFontSize(FontSize14Index));

    AppendChild(&m_txtDirNameOrBookName);
    AppendChild(&m_txtDirBookNumOrBookAuthor);
    AppendChild(&m_txtItemNameLine);
    AppendChild(&m_txtReadingProgressLine);
    AppendChild(&m_imgNew);
    AppendChild(&m_imgTrial);
    AppendChild(&m_imgCover);
    AppendChild(&m_imgFormat);
    AppendChild(&m_imgSelect);
}

void UICoverViewItem::MoveWindow(int left, int top, int width, int height)
{
    DebugPrintf(DLC_GUI_VERBOSE, "UICoverViewItem::MoveWindow(%d, %d, %d, %d)",
            left, top, width, height);
    UIContainer::MoveWindow(left, top, width, height);
}

#ifdef KINDLE_FOR_TOUCH
void UICoverViewItem::DrawTouchBorder()
{
    if (!IsHighlight())
    {
        return;
    }
    CDisplay* display = CDisplay::GetDisplay();
    // hack
    DK_IMAGE imgScreen = display->GetMemDC();
    int absX = GetAbsoluteX();
    int absY = GetAbsoluteY();
    int margin = 6;
    int coverTop = GetCoverTop();
    int coverLeft = GetCoverLeft();
    int coverWidth = GetCoverWidth();
    int coverHeight = GetCoverHeight();
    DK_RECT rcExtended = {absX + coverLeft- margin, absY + coverTop - margin, absX + coverLeft + coverWidth + margin, absY + coverTop + coverHeight + margin};
    DK_IMAGE imgExtended;
    CropImage(imgScreen, rcExtended, &imgExtended);
    CTpGraphics grf(imgExtended);
    int left = 0;
    int top = 0;
    int right = left + GetCoverWidth() + margin * 2;
    int bottom = top + GetCoverHeight() + margin * 2;
    int borderWidth = 4;
    int borderInterval = 2;
    grf.DrawRectBorder(left, top, right, bottom,
            borderWidth, ColorManager::knBlack);
    grf.DrawRectBorder(left + borderWidth, top + borderWidth, right - borderWidth, bottom - borderWidth,
            borderInterval, ColorManager::knWhite);

}
#endif

void UICoverViewItem::DrawBorder(DK_IMAGE imgSelf)
{
    if (!IsFocus())
    {
        return;
    }
#ifndef KINDLE_FOR_TOUCH
    CTpGraphics grf(imgSelf);
    int left = GetCoverLeft() - GetLeftExtra();
    int top = GetCoverTop() - GetTopExtra();
    int right = left + GetCoverWidth() + GetLeftExtra() * 2;
    int bottom = top + GetCoverHeight() + GetLeftExtra() * 2;
    grf.DrawRectBorder(left, top, right, bottom,
            BORDER_WIDTH, ColorManager::knBlack);
    grf.DrawRectBorder(left + BORDER_WIDTH, top + BORDER_WIDTH, right - BORDER_WIDTH, bottom - BORDER_WIDTH,
            BORDER_INTERVAL, ColorManager::knWhite);

#endif
}
void UICoverViewItem::DrawShaddowAndBorder(DK_IMAGE imgSelf)
{
    CTpGraphics grf(imgSelf);
    int coverTop = GetCoverTop();
    int coverLeft = GetCoverLeft();
    int coverWidth = GetCoverWidth();
    int coverHeight = GetCoverHeight();
    int shaddowLeft = coverLeft + SHADDOW_LEFT;
    int shaddowTop = coverTop + SHADDOW_TOP;
    int shaddowWidth = coverWidth - SHADDOW_LEFT + SHADDOW_RIGHT_EXCEEDED;
    int shaddowHeight = coverHeight - SHADDOW_TOP + SHADDOW_BOTTOM_EXCEEDED;


    grf.EraserBackGround(ColorManager::knWhite);
    grf.FillRect(shaddowLeft, shaddowTop,
            shaddowLeft + shaddowWidth, shaddowTop + shaddowHeight,
            ColorManager::GetColor(COLOR_PROGRESSBAR_BACK));
    grf.FillRect(coverLeft, coverTop,
            coverLeft + coverWidth,
            coverTop + coverHeight,
            ColorManager::knBlack);
}

void UICoverViewItem::DrawTrial(DK_IMAGE imgSelf)
{
    if (IsTrial())
    {
        int trialWidth = GetWindowMetrics(UICoverViewItemImageTrialWidthIndex);
        int trialHeight = GetWindowMetrics(UICoverViewItemImageTrialHeightIndex);
        int trialLeft = GetCoverLeft() + GetCoverWidth() + SHADDOW_RIGHT_EXCEEDED - trialWidth;
#ifdef KINDLE_FOR_TOUCH
        int trialTop = GetCoverTop() - 2;
#else
        int trialTop = GetCoverTop() + 2;
#endif
        m_imgTrial.MoveWindow(trialLeft, trialTop, trialWidth, trialHeight);
        m_imgTrial.Draw(imgSelf);
    }
}

void UICoverViewItem::DrawFormat(DK_IMAGE imgSelf)
{
    if (!IsDir() && !m_isDuoKanBook)
    {
        int formatLeft = GetCoverLeft() + GetCoverWidth() + FORMAT_RIGHT_EXCEEDED - GetWindowMetrics(UICoverViewItemFormatWidthIndex);
        //int formatTop = GetWindowMetrics(UICoverViewItemFormatTopIndex) + GetTopExtra();
        int formatTop = GetCoverTop() + ((GetCoverHeight() * 3) >> 2);
        int formatWidth = GetWindowMetrics(UICoverViewItemFormatWidthIndex);
        int formatHeight = GetWindowMetrics(UICoverViewItemFormatHeightIndex);
        m_imgFormat.MoveWindow(formatLeft, formatTop, formatWidth, formatHeight);
        m_imgFormat.Draw(imgSelf);
    }
}

//void UICoverViewItem::DrawProgress(DK_IMAGE imgSelf)
//{
//    if (IsDir())
//    {
//        return;
//    }
//    int progressLeft = GetCoverLeft() + SHADDOW_LEFT;
//    int coverBottom = GetCoverTop() + GetCoverHeight();
//    if (IsNew())
//    {
//        int newLeft = progressLeft;
//        int newTop = coverBottom + NEW_TOP_PADDING;
//        int newWidth = GetWindowMetrics(UICoverViewItemImageNewWidthIndex);
//        int newHeight = GetWindowMetrics(UICoverViewItemImageNewHeightIndex);
//        int totalHeight = GetWindowMetrics(UIHomePageCoverViewItemHeightIndex);
//        if (newTop + newHeight > totalHeight)
//        {
//            newTop = totalHeight - newHeight;
//        }
//        m_imgNew.MoveWindow(newLeft, newTop, newWidth, newHeight);
//        m_imgNew.Draw(imgSelf);
//        return;
//    }
//
//    int progressTop = GetCoverTop() + GetCoverHeight() + PROGRESS_TOP_PADDING;
//    if (progressTop + PROGRESS_HEIGHT > GetHeight())
//    {
//        progressTop = GetHeight() - PROGRESS_HEIGHT;
//    }
//    CTpGraphics grf(imgSelf);
//    for (int i = 0; i < 100; i += 10)
//    {
//        if (i <= m_progress)
//        {
//            grf.FillRect(progressLeft, progressTop,
//                    progressLeft + PROGRESS_HEIGHT,
//                    progressTop + PROGRESS_HEIGHT,
//                    ColorManager::knBlack);
//        }
//        else
//        {
//            grf.FillRect(progressLeft + 1, progressTop + 1,
//                    progressLeft + 1 + PROGRESS_HEIGHT_SMALL,
//                    progressTop + 1 + PROGRESS_HEIGHT_SMALL,
//                    ColorManager::knBlack);
//        }
//        progressLeft += PROGRESS_INTERVAL;
//    }

void UICoverViewItem::DrawBottomTitle(DK_IMAGE imgSelf)
{
    const int coverLeft = GetCoverLeft();
    int titleWidth = m_overallWidth - ((SHADDOW_LEFT + coverLeft) << 1 );
    int titleLeft = coverLeft + SHADDOW_LEFT;
    int coverBottom = GetCoverTop() + GetCoverHeight();
    int titleTop = coverBottom + NEW_TOP_PADDING;
    
    int titleHeight = m_txtItemNameLine.GetHeightByWidth(titleWidth);
    int maxTitleHeight = (GetWindowFontSize(FontSize16Index) << 1) + 16;
    titleHeight = titleHeight > maxTitleHeight ? maxTitleHeight : titleHeight;
    m_txtItemNameLine.MoveWindow(titleLeft, titleTop, titleWidth, titleHeight);
    m_txtItemNameLine.Draw(imgSelf);
}

void UICoverViewItem::DrawBottomProgress(DK_IMAGE imgSelf)
{
    if (IsDir())
    {
        // Display number of books in directory
        m_txtReadingProgressLine.SetText(m_txtDirBookNumOrBookAuthor.GetText());
    }

    int progressLeft = GetCoverLeft() + SHADDOW_LEFT;
    int coverBottom = GetCoverTop() + GetCoverHeight();
    int progressTop = coverBottom + m_txtItemNameLine.GetHeight() + (NEW_TOP_PADDING << 1) ;
    int progressHeight = m_txtReadingProgressLine.GetTextHeight();
    int overallHeight = m_overallHeight;
    if (progressTop + progressHeight > overallHeight)
    {
        progressTop = overallHeight - progressHeight;
    }

    if (IsNew())
    {
        int newLeft = progressLeft;
        int newTop = progressTop;
        int newWidth = GetWindowMetrics(UICoverViewItemImageNewWidthIndex);
        int newHeight = GetWindowMetrics(UICoverViewItemImageNewHeightIndex);

        m_imgNew.MoveWindow(newLeft, newTop, newWidth, newHeight);
        m_imgNew.Draw(imgSelf);
    }
    else
    {
        int progressWidth = m_txtReadingProgressLine.GetTextWidth();
        if (progressWidth <= 0)
        {
            return;
        }
        m_txtReadingProgressLine.MoveWindow(progressLeft, progressTop, progressWidth, progressHeight);
        m_txtReadingProgressLine.Draw(imgSelf);
    }
}

void UICoverViewItem::DrawCover(DK_IMAGE imgSelf)
{
    if (m_coverFile.empty())
    {
        m_imgCover.SetImage(ImageManager::GetImage(IMAGE_ICON_COVER_BACKGROUND));
    }
    int coverLeft = GetCoverLeft();
    int coverTop = GetCoverTop();

    int imgWidth = m_imgCover.GetWidth();
    int imgHeight = m_imgCover.GetHeight();
    int imgLeft = coverLeft + 1;
    int imgTop = coverTop + 1;
    m_imgCover.MoveWindow(imgLeft, imgTop, imgWidth, imgHeight);
    m_imgCover.Draw(imgSelf);
    if (IsDir())
    {
        m_txtDirNameOrBookName.SetMaxWidth(GetWindowMetrics(UICoverViewItemDirNameMaxWidthIndex));
        m_txtDirNameOrBookName.SetMaxHeight(GetWindowMetrics(UICoverViewItemDirNameMaxHeightIndex));
        int nameWidth = m_txtDirNameOrBookName.GetTextWidth();
        int nameHeight = m_txtDirNameOrBookName.GetHeightByWidth(nameWidth);
        int authorWidth = m_txtDirBookNumOrBookAuthor.GetTextWidth();
        int authorHeight = m_txtDirBookNumOrBookAuthor.GetTextHeight();
        DebugPrintf(DLC_GUI_VERBOSE, "name:(%s,%d,%d), author:(%s,%d,%d)",
                m_txtDirNameOrBookName.GetText(), nameWidth, nameHeight,
                m_txtDirBookNumOrBookAuthor.GetText(), authorWidth, authorHeight);
        int dirNameBookNumberInterval = GetWindowMetrics(UICoverViewItemDirNameBookNumberIntervalIndex);
        int totalSpaceForNameAndNumber = GetWindowMetrics(UICoverViewItemDirNameBookNumberTotalheightIndex);
        int spaceLeft = totalSpaceForNameAndNumber - nameHeight - authorHeight - dirNameBookNumberInterval;
        int topMargin = spaceLeft * 3 / 5;
        int topExtra = GetTopExtra();
        int leftExtra = GetLeftExtra();
        int nameTop = GetWindowMetrics(UICoverViewItemDirNameTopIndex) + topExtra + topMargin;
        int nameLeft = leftExtra + 1 + (m_innerWidth - nameWidth) / 2;

        m_txtDirNameOrBookName.MoveWindow(nameLeft, nameTop, nameWidth, nameHeight);
        //m_txtDirBookNumOrBookAuthor.MoveWindow(authorLeft, authorTop, authorWidth, authorHeight);
        m_txtDirNameOrBookName.Draw(imgSelf);
        //m_txtDirBookNumOrBookAuthor.Draw(imgSelf);
    }
    else if (m_coverFile.empty())
    {
        m_txtDirNameOrBookName.SetMaxWidth(GetWindowMetrics(UICoverViewItemDirNameMaxWidthIndex));
        m_txtDirNameOrBookName.SetMaxHeight(GetWindowMetrics(UICoverViewItemBookNameMaxHeightIndex));
        int nameWidth = m_txtDirNameOrBookName.GetTextWidth();
        int nameHeight = m_txtDirNameOrBookName.GetHeightByWidth(nameWidth);
        int authorWidth = std::min(m_txtDirBookNumOrBookAuthor.GetTextWidth(), 100);

        int authorHeight = m_txtDirBookNumOrBookAuthor.GetTextHeight();
        DebugPrintf(DLC_GUI_VERBOSE, "name:(%s,%d,%d), author:(%s,%d,%d)",
                m_txtDirNameOrBookName.GetText(), nameWidth, nameHeight,
                m_txtDirBookNumOrBookAuthor.GetText(), authorWidth, authorHeight);
        int topExtra = GetTopExtra();
        int leftExtra = GetLeftExtra();
        int nameTop = topExtra + GetWindowMetrics(UICoverViewItemBookNameTopIndex);
        int authorTop = topExtra + GetWindowMetrics(UICoverViewItemAuthorNameTopIndex);
        int nameLeft = leftExtra + 1 + (m_innerWidth - nameWidth) / 2;
        int authorLeft = leftExtra + 1 + (m_innerWidth - authorWidth) / 2;
        m_txtDirNameOrBookName.SetEraseBackground(false);
        m_txtDirNameOrBookName.MoveWindow(nameLeft, nameTop, nameWidth, nameHeight);
        m_txtDirBookNumOrBookAuthor.MoveWindow(authorLeft, authorTop, authorWidth, authorHeight);
        m_txtDirBookNumOrBookAuthor.SetEraseBackground(false);
        m_txtDirNameOrBookName.Draw(imgSelf);
        m_txtDirBookNumOrBookAuthor.Draw(imgSelf);
    }
}

void UICoverViewItem::DrawSelect(DK_IMAGE imgSelf)
{
    if (BLU_SELECT != m_usage)
    {
        return;
    }
    int selectWidth = GetWindowMetrics(UICoverViewItemSelectWidthIndex);
    int selectHeight = GetWindowMetrics(UICoverViewItemSelectHeightIndex);
#ifdef KINDLE_FOR_TOUCH
    int selectLeft = GetCoverLeft() + GetCoverWidth() - selectWidth / 2 ;
    int selectTop = GetCoverTop() + GetCoverHeight() - selectHeight / 2;
#else
    int selectLeft = GetCoverLeft() + GetCoverWidth() - selectWidth + 4;
    int selectTop = GetCoverTop() + GetCoverHeight() - selectHeight + 4;
#endif
    m_imgSelect.MoveWindow(selectLeft, selectTop, selectWidth, selectHeight);
    UpdateSelectImage();
    m_imgSelect.Draw(imgSelf);
}

HRESULT UICoverViewItem::Draw(DK_IMAGE drawingImg)
{
    if (!IsVisible())
    {
        return S_OK;
    }
    DebugPrintf(DLC_GUI_VERBOSE, "UICoverViewItem::Draw(%d,%d)~(%d,%d)",
            m_iLeft, m_iTop, m_iWidth, m_iHeight);
    DK_IMAGE imgSelf;
    DK_RECT rcSelf = {m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
    HRESULT hr = CropImage(drawingImg, rcSelf, &imgSelf);
    if (!SUCCEEDED(hr))
    {
        return hr;
    }
    DrawShaddowAndBorder(imgSelf);
    DrawCover(imgSelf);
    DrawTrial(imgSelf);
    DrawFormat(imgSelf);
    //DrawProgress(imgSelf);
    DrawBottomTitle(imgSelf);
    DrawBottomProgress(imgSelf);
    DrawSelect(imgSelf);
#ifdef KINDLE_FOR_TOUCH
    DrawTouchBorder();
#else
    DrawBorder(imgSelf);
#endif
    return S_OK;
}

void UICoverViewItem::SetCoverImage(const char* coverFile)
{
    m_coverFile = coverFile;
    m_imgCover.SetAutoSize(true);
    m_imgCover.SetImageFile(coverFile);
    if (!m_coverFile.empty() &&
            (m_imgCover.GetWidth() >= 2 * m_imgCover.GetHeight() || 
             m_imgCover.GetWidth() * 2 <= m_imgCover.GetHeight()))
    {
        SetCoverImage("");
    }
}

void UICoverViewItem::SetProgress(const char* progressStr)
{
    m_txtReadingProgressLine.SetText(progressStr);
}

void UICoverViewItem::UpdateFormatText()
{
    if (IsDir() || m_isDuoKanBook)
    {
        return;
    }
    int imgId = 0;
    if (IsLoading())
    {
        imgId = IMAGE_ICON_COVER_FORMAT_LOADING;
    }
    else if (IsDuoKanBook())
    {
        imgId = IMAGE_ICON_COVER_FORMAT_DUOKAN;
    }
    else if (IsLoading())
    {
        imgId = IMAGE_ICON_COVER_FORMAT_LOADING;
    }
    else
    {
        switch (m_fileFormat)
        {
            case DFF_ElectronicPublishing:
                imgId = IMAGE_ICON_COVER_FORMAT_EPUB;
                break;
            case DFF_MobiPocket:
                imgId = IMAGE_ICON_COVER_FORMAT_MOBI;
                break;
            case DFF_RoshalArchive:
                imgId = IMAGE_ICON_COVER_FORMAT_RAR;
                break;
            case DFF_ZipFile:
                imgId = IMAGE_ICON_COVER_FORMAT_ZIP;
                break;
            case DFF_PortableDocumentFormat:
                imgId = IMAGE_ICON_COVER_FORMAT_PDF;
                break;
            case DFF_Text:
                imgId = IMAGE_ICON_COVER_FORMAT_TXT;
                break;
            default:
                break;
        }
    }
    //m_imgFormat.SetAutoSize(true);
    m_imgFormat.SetImageFile(ImageManager::GetImagePath(imgId));
}

void UICoverViewItem::SetBookFormat(DkFileFormat format)
{
    m_fileFormat = format;
    UpdateFormatText();
}

void UICoverViewItem::SetIsDuoKanBook(bool duokanBook)
{
    m_isDuoKanBook = duokanBook;
    UpdateFormatText();
}

void UICoverViewItem::SetIsDir(bool isDir)
{
    m_isDir = isDir;
    if (isDir)
    {
        m_txtDirNameOrBookName.SetMaxHeight(GetWindowMetrics(UICoverViewItemDirNameMaxHeightIndex));
    }
    else
    {
        m_txtDirNameOrBookName.SetMaxHeight(GetWindowMetrics(UICoverViewItemBookNameMaxHeightIndex));
    }
    UpdateFormatText();
}

bool UICoverViewItem::IsDir() const
{
    return m_isDir;
}

void UICoverViewItem::SetDirNameOrBookName(const char* title)
{
    m_txtDirNameOrBookName.SetText(title);
    m_txtItemNameLine.SetText(title);
}

void UICoverViewItem::SetDirBookNumOrBookAuthor(const char* author)
{
    m_txtDirBookNumOrBookAuthor.SetText(author);
}

void UICoverViewItem::SetSelected(bool selected)
{
    if (m_selected != selected)
    {
        m_selected = selected;
    }
}

bool UICoverViewItem::IsSelected() const
{
    return m_selected;
}

void UICoverViewItem::SetIsTrial(bool isTrial)
{
    m_isTrial = isTrial;
    if (IsTrial())
    {
        m_imgTrial.SetImageFile(ImageManager::GetImagePath(IMAGE_ICON_COVER_TRIAL));
    }
    else
    {
        m_imgTrial.SetImageFile("");
    }
}

bool UICoverViewItem::IsTrial() const
{
    return m_isTrial;
}

void UICoverViewItem::SetIsNew(bool isNew)
{
    m_isNew = isNew;
    if (IsNew())
    {
        // m_imgNew.SetImageFile(ImageManager::GetImagePath(IMAGE_ICON_COVER_NEW));
        m_imgNew.SetImageFile(ImageManager::GetImagePath(IMAGE_ICON_COVER_NEW).c_str());
    }
    else
    {
        m_imgNew.SetImageFile("");
    }
}

bool UICoverViewItem::IsNew() const
{
    return m_isNew;
}

bool UICoverViewItem::IsDuoKanBook() const
{
    return m_isDuoKanBook;
}

void UICoverViewItem::UpdateSelectImage()
{
    if (BLU_SELECT != m_usage)
    {
        return;
    }
    m_imgSelect.SetImageFile(
            ImageManager::GetImagePath(IsSelected() 
                ? IMAGE_ICON_COVER_SELECTED_TRANSPARENT
                : IMAGE_ICON_COVER_UNSELECTED_TRANSPARENT));
}

int UICoverViewItem::GetTopExtra() const
{
    return dk_max(BORDER_WIDTH + BORDER_INTERVAL, TRIAL_TOP_OUTSIDE);
}

int UICoverViewItem::GetLeftExtra() const
{
    return BORDER_WIDTH + BORDER_INTERVAL;
}

int UICoverViewItem::GetCoverLeft() const
{
    if (m_coverFile.empty())
    {
        return GetLeftExtra();
    }

    return GetLeftExtra()  
        + ((m_innerWidth - m_imgCover.GetWidth()) >> 1);
}

int UICoverViewItem::GetCoverTop() const
{
    if (m_coverFile.empty())
    {
        return GetTopExtra();
    }
    return GetTopExtra() 
        + (m_innerHeight - m_imgCover.GetHeight());
}

int UICoverViewItem::GetCoverWidth() const
{
    if (m_coverFile.empty())
    {
        return m_innerWidth + 2;
    }
    return m_imgCover.GetWidth() + 2;
}

int UICoverViewItem::GetCoverHeight() const
{
    if (m_coverFile.empty())
    {
        return m_innerHeight + 2;
    }
    return m_imgCover.GetHeight() + 2;
}

void UICoverViewItem::SetIsLoading(bool isLoading)
{
    m_isLoading = isLoading;
    UpdateFormatText();
}

bool UICoverViewItem::IsLoading() const
{
    return m_isLoading;
}
#ifdef KINDLE_FOR_TOUCH
void UICoverViewItem::SetHighlight(bool highlight)
{
    m_highlight = highlight;
}

bool UICoverViewItem::IsHighlight() const
{
    return m_highlight;
}
#endif

bool UICoverViewItem::OnCoverLoadeded(const EventArgs& args)
{
    if (!IsVisible())
    {
        return true;
    }
    const BookCoverLoadedArgs& bookCoverLoadedArgs =
        (const BookCoverLoadedArgs&)args;
    if (m_bookPath != bookCoverLoadedArgs.bookPath
            || bookCoverLoadedArgs.coverPath.empty())
    {
        return true;
    }
    SetCoverImage(bookCoverLoadedArgs.coverPath);
    UpdateWindow();
    return true;
}


void UICoverViewItem::SetBookPath(const char* bookPath)
{
    m_bookPath = bookPath;
}
