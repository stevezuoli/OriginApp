///////////////////////////////////////////////////////////////////////
// UIBookReaderContainer.cpp
// Contact: liuhj
// modified by: zhangjf
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#include "TouchAppUI/UIBookReaderContainer.h"
#include "TouchAppUI/UIAutoPageDownDlg.h"
#include "CommonUI/CPageNavigator.h"
#include "TouchAppUI/UICommentOrSummarySelectDlg.h"
#include "CommonUI/UIWeiboSharePage.h"
#include "CommonUI/UIUtility.h"
#include "CommonUI/UICommentEditDialog.h"
#include "TouchAppUI/UIAudioPlayerDialog.h"
#include "CommonUI/UIFootNoteDialog.h"
#include "TouchAppUI/UIBookContentSearchDlg.h"
#include "CommonUI/UIBookCorrectionDlg.h"
#include "I18n/StringManager.h"
#include "Common/FileManager_DK.h"
#include "BookReader/EpubBookReader.h"
#include "BookReader/MobiBookReader.h"
#include "BookReader/TextBookReader.h"
#include "BookReader/PdfBookReader.h"
#include "BookReader/PageTable.h"
#include "Framework/CHourglass.h"
#include "Framework/CDisplay.h"
#include "Framework/CNativeThread.h"
#include "PowerManager/PowerManager.h"
#include "TTS/TTS.h"
#include "DkStreamFactory.h"
#include "Utility.h"
#include <string.h>
#include "FontManager/DKFont.h"
#include "KernelEncoding.h"
#include "DKXManager/DKX/DKXReadingDataItemFactory.h"
#include "GUI/UIMessageBox.h"
#include "Utility/FileSystem.h"
#include "Utility/Dictionary.h"
#include "Utility/StringUtil.h"
#include "Utility/SystemUtil.h"
#include "Utility/Misc.h"
#include "Utility/HttpUtil.h"
#include "../ImageHandler/DkImageHelper.h"
#include "AudioPlayer/AudioPlayer.h"
#include "TouchAppUI/BookOpenManager.h"
#include "unistd.h"
#include "SQM.h"
#include "drivers/DeviceInfo.h"
#include "KernelVersion.h"
#include "BookStore/ReadingBookInfo.h"
#include "BookStore/BookStoreInfoManager.h"
#include "BookStoreUI/UIBookDetailedInfoPage.h"
#include "BookStoreUI/UIBookStoreAddCommentPage.h"
#include "Common/CAccountManager.h"
#include <tr1/functional>
#include "ImageHandler/ImageHandler.h"
#include "../ImageHandler/DkImageHelper.h"
#include "Common/WindowsMetrics.h"
#include "GUI/CTpGraphics.h"
#include "GUI/GUISystem.h"
#include "Weibo/WeiboFactory.h"
#include "Common/ReadingHistoryStat.h"
#include "CommonUI/UIInteractiveImageDetailPage.h"
#include "CommonUI/UIInteractiveFlexPage.h"
#include "GUI/GUIHelper.h"
#include "GUI/GlobalEventListener.h"
#include "TouchAppUI/UITocDlg.h"
#include "Utility/ReaderUtil.h"
#include "GUI/ImageFullRepaintCalculation.h"
#include "BookStore/OfflineBookStoreManager.h"
#include "Utility/WebBrowserUtil.h"

#ifdef KINDLE_FOR_TOUCH
#include "TouchAppUI/UIBookMenuDlg.h"
#else
#include "AppUI/UIBookMenuDlg.h"
#endif

#ifndef _RELEASE_VERSION
//#define ENABLE_PROFILE
#endif
#include "Utility/Profiler.h"
#define DEFAULT_AUTO_PAGE_DOWN_SECOND_COUNT     (25)
using namespace dk::bookstore;
using namespace dk::bookstore::sync;
using namespace DkFormat;
using namespace dk::account;
using namespace dk::utility;
using namespace WindowsMetrics;

static const unsigned int BOOKMARK_PREVIEW_MAX_LENGTH = 200;
static const int WEB_SEARCH_KEYWORD_MAX_LENGTH = 50;
static const int EXTERNAL_LINK_MESSAGE_MAX_LENGTH = 40;

using namespace DkFormat;

static std::string getWebSearchZoom()
{
    return "zoom=1.5";
}

static std::string getRefinedSearchKeywords(const std::string& selectedContent)
{
    wstring w_raw_keyword = EncodeUtil::ToWString(selectedContent);
    int selectedContentSize = wcslen(w_raw_keyword.c_str());
    if (selectedContentSize > WEB_SEARCH_KEYWORD_MAX_LENGTH)
    {
        w_raw_keyword = w_raw_keyword.substr(0, WEB_SEARCH_KEYWORD_MAX_LENGTH);
    }
    std::string raw_keyword = EncodeUtil::ToString(w_raw_keyword);

    //raw_keyword = HttpUtil::UrlEncode(raw_keyword.c_str());
    std::string ret = "\"";
    ret.append(raw_keyword).append("\"");
    return ret;
}

static CommandList getBaiduSearchUrl(const std::string& keyword)
{
    CommandList commands;
    std::string search_option("\"search=baidu\"");
    commands.push_back(search_option);

    std::string word = getRefinedSearchKeywords(keyword);
    commands.push_back(word);

    commands.push_back(getWebSearchZoom());
    return commands;
}

static CommandList getGoogleSearchUrl(const std::string& keyword)
{
    CommandList commands;
    std::string search_option("\"search=google\"");
    commands.push_back(search_option);

    std::string word = getRefinedSearchKeywords(keyword);
    commands.push_back(word);

    commands.push_back(getWebSearchZoom());
    return commands;
}

static CommandList getCibaTranslateUrl(const std::string& keyword)
{
    CommandList commands;
    std::string search_option("\"search=ciba\"");
    commands.push_back(search_option);

    std::string word = getRefinedSearchKeywords(keyword);
    commands.push_back(word);

    commands.push_back(getWebSearchZoom());
    return commands;
}

static CommandList getWikiUrl(const std::string& keyword)
{
    CommandList commands;
    std::string search_option("\"search=wiki\"");
    commands.push_back(search_option);

    std::string word = getRefinedSearchKeywords(keyword);
    commands.push_back(word);

    //commands.push_back(getWebSearchZoom());
    return commands;
}

static std::string getExternLinkURL(const std::string& origin_url)
{
    std::string target = "\"" + origin_url + "\"";
    return target;
}

static void searchOnWeb(const CommandList& commands)
{
    if(UIUtility::CheckAndReConnectWifi())
    {
        WebBrowserLauncher::GetInstance()->Exec(commands);
    }
}

UIBookReaderContainer::UIBookReaderContainer (UIContainer *pParent)
    : UIContainer(pParent, IDSTATIC)
    , m_pBookReader(NULL)
    , m_pageWidth(-1)
    , m_pageHeight(-1)
    , m_scaleFlag(WAIT_FOR_SCALE)
    , m_curSpan(0)
    , m_pInitBMP(NULL)
    , m_pImgStream(NULL)
    , m_expandToc(false)
    , m_bInialized(false)
    , m_bIsAutoPageDown(false)
    , m_bLastPage(false)
    , m_bFirstPage(false)
    , m_bIsFirstOpen(true)
    , m_isTrialBook(false)
    , m_iBookID(-1)
    , m_iTopMargin(30)
    , m_iBottomMargin(30)
    , m_iHorizontalMargin(30)
    , m_eFormat(DFF_Unknown)
    , m_iAutoPageDownSecondCount(DEFAULT_AUTO_PAGE_DOWN_SECOND_COUNT)
    , m_iAutoPageDownTimerId(-1)
    , m_clsReaderProgressBar(this, IDSTATIC)
    , m_zoomedPdfVerticalPos(this, IDSTATIC)
    , m_zoomedPdfHorizontalPos(this, IDSTATIC)
    , m_eLastReadingMode (PDF_RM_NormalPage)
    , m_bExistBookMarkForCurPage(false)
    , m_uIndexOfBookMarkForCurPage(0)
    , m_bIsPdfTextMode(false)
    , m_pReaderSettingDlg(NULL)
    , m_readingDataChanged(false)
    , m_clsStartSelected(-1, -1, -1, -1)
    , m_clsEndSelected(-1, -1, -1, -1)
    , m_iCurCommentIndex(-1)
    , m_iCurDigestIndex(-1)
    , m_turnPageNum(0)
    , m_isScreenSaverOut(false)
    , m_isGotoBookMarkFromOpenBook(true)
    , m_txtChapterTitle()
    , m_iCurrentInteractivePageObjectIndex(-1)
    , m_dotProgressFocusIndex(-1)
    , m_isSelecting(false)
    , m_rightSelecter(true)
    , m_processMove(true)
    , m_downInSelectionImage(false)
    , m_imageSelectedEnd(this)
    , m_imageSelectedStart(this)
    , m_pdfZoomMoved(false)
    , m_firstDrawPage(true)
    , m_lastPageFullScreen(false)
    , m_dkxBlock(NULL)
    , m_fullRepaint(false)
	, m_isPageTurned(false)
	, m_jumpHistory(0, 0, 0)
	, m_jumpDirection(GO_NONE)
{
    AppendChild(&m_txtChapterTitle);
    m_gestureListener.SetBookReaderContainer(this);
    m_gestureDetector.SetListener(&m_gestureListener);
    m_gestureDetector.SetDoubleTapListener(&m_gestureListener);
    m_gestureDetector.EnableLongPress(true);
    m_gestureDetector.EnableHolding(true);
    m_scaleGestureListener.SetBookReaderContainer(this);
    m_scaleGestureDetector.SetListener(&m_scaleGestureListener);
    DebugPrintf(DLC_GESTURE, "UIBookReaderContainer::Ctor end");
}

UIBookReaderContainer::~UIBookReaderContainer ()
{
    //Don't delete this pointer, it will be released by the reader!!!
    GlobalEventListener::GetInstance()->SetBookReaderContainer(NULL);
    PostLocalReadingDataToCloud();
    ResetContainer();
    m_iTopMargin = 0;
    m_iBottomMargin = 0;
    m_iHorizontalMargin = 0;
    Dispose ();
    DKXManager::DestoryDKXBlock(m_dkxBlock);
}

BOOL UIBookReaderContainer::Initialize (LPCSTR pstrFilePath, LPCSTR strBookName, int iBookId)
{
    DebugLog(DLC_GESTURE, "UIBookReaderContainer::Initialize() Start");
    //读取图书页面 margin
    if (NULL == pstrFilePath || 0 == pstrFilePath[0])
    {
        return FALSE;
    }

    ResetContainer();
    m_strBookFilePath = pstrFilePath;
    m_dkxBlock = DKXManager::CreateDKXBlock(m_strBookFilePath.c_str());
    if (NULL == m_dkxBlock)
    {
        return false;
    }
    m_strBookName = strBookName;
    m_iBookID = iBookId;
    m_isDKBookStoreBook = IsDuoKanBookStoreBook(m_strBookFilePath);

    // TODO:delete it.
    CDKFileManager *pFileManager = CDKFileManager::GetFileManager();
    PCDKFile pDkFile = pFileManager->GetFileById(iBookId);
    if(NULL == pDkFile)
    {
        return FALSE;
    }
    
    m_eFormat = pDkFile->GetFileFormat();
    m_localBookID = pDkFile->GetBookID();
	m_isTrialBook = pDkFile->GetIsTrialBook();
    // end.

    if(!CreateBookReader())
    {
        return FALSE;
    }
	m_bInialized = true;

	GlobalEventListener::GetInstance()->SetBookReaderContainer(this);
    SubscribeMessageEvent(
            BookStoreInfoManager::EventGetReadingProgressUpdate,
            *BookStoreInfoManager::GetInstance(),
            std::tr1::bind(
                std::tr1::mem_fn(
                    &UIBookReaderContainer::OnGetReadingProgressUpdate),
                this,
                std::tr1::placeholders::_1));
    SubscribeMessageEvent(
            BookStoreInfoManager::EventGetReadingDataUpdate,
            *BookStoreInfoManager::GetInstance(),
            std::tr1::bind(
                std::tr1::mem_fn(
                    &UIBookReaderContainer::OnGetReadingDataUpdate),
                this,
                std::tr1::placeholders::_1));
	SubscribeEvent(CAccountManager::EventAccount,
			*(CAccountManager::GetInstance()),
			std::tr1::bind(
			std::tr1::mem_fn(&UIBookReaderContainer::OnAccountLoginEvent),
			this, std::tr1::placeholders::_1));

	SubscribeMessageEvent(UIBookParseStatusListener::EventBookParseFinished,
			*(UIBookParseStatusListener::GetInstance()),
			std::tr1::bind(
			std::tr1::mem_fn(&UIBookReaderContainer::OnParseFinishedEvent),
			this, std::tr1::placeholders::_1));

    SubscribeMessageEvent(WebBrowserLauncher::EventWebBrowserClosed,
        *WebBrowserLauncher::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookReaderContainer::OnWebBrowserClosed),
        this,
        std::tr1::placeholders::_1));
    return TRUE;
}

BOOL UIBookReaderContainer::CreateBookReader()
{
    if (!CUtility::CheckFileExistence(m_strBookFilePath.c_str()))
    {
        return FALSE;
    }

    switch (m_eFormat)
    {
    case DFF_Text:
        {
            SQM::GetInstance()->IncCounter(SQM_ACTION_BOOKREADER_FORMAT_TXT);
            m_pBookReader = new TextBookReader();
        }
        break;
    case DFF_ElectronicPublishing:
        {
            SQM::GetInstance()->IncCounter(SQM_ACTION_BOOKREADER_FORMAT_EPUB);
            m_pBookReader = new EpubBookReader();
        }
        break;
    case DFF_MobiPocket:
        {
            SQM::GetInstance()->IncCounter(SQM_ACTION_BOOKREADER_FORMAT_MOBI);
            m_pBookReader = new MobiBookReader();
        }
        break;
    case DFF_PortableDocumentFormat:
        {
            SQM::GetInstance()->IncCounter(SQM_ACTION_BOOKREADER_FORMAT_PDF);
            m_pBookReader = new PdfBookReader();
        }
        break;
    default:
        break;
    }

    if (NULL == m_pBookReader
            || !m_pBookReader->OpenDocument(m_strBookFilePath))
    {
        return FALSE;
    }

	// 设置行尾标点压缩
    DKTYPESETTING typeSetting;
	typeSetting.ShrinkLineEndPunctuation = DK_TRUE;
	typeSetting.EnableHyphenation = DK_FALSE;
	typeSetting.LineMaxStretchRate = LINEMAXSTRETCHRATE;
	m_pBookReader->SetTypeSetting(typeSetting);
	
    DoDkxUpdate();

    // 添加图像处理器
    IBMPProcessor* pProcessor = BMPProcessFactory::CreateInstance(IBMPProcessor::DK_BMPPROCESSOR_GRAYSCALE);
    if (NULL == pProcessor)
    {
        return FALSE;
    }

    m_pBookReader->AddBMPProcessor(pProcessor);
    
    return TRUE;
}

bool UIBookReaderContainer::IsInstantTurnPageModel()
{
    return SystemSettingInfo::GetInstance()->IsInstantTurnPageModel();
}

bool UIBookReaderContainer::InitBookReader()
{
	if(!m_dkxBlock)
	{
		return false;
	}
    m_dkxBlock->GetPDFModeController(&m_modeController);
    CT_ReadingDataItemImpl readingBookmark;
    m_dkxBlock->GetProgress(&readingBookmark);
    m_bIsPdfTextMode = (DFF_PortableDocumentFormat == m_eFormat
        && PDF_RM_Rearrange == m_modeController.m_eReadingMode);

    // Get smart cutting setting
    if (m_modeController.m_eCuttingEdgeMode == PDF_CEM_Smart)
    {
        m_modeController.m_bIsSmartCutting = true;
    }

    // 加载系统设置数据
    LoadSystemPageSizeInfo(m_pageWidth, m_pageHeight);
    if (!m_pBookReader->SetPageSize(m_iHorizontalMargin, m_iTopMargin, m_pageWidth, m_pageHeight))
    {
        return false;
    }
    m_pBookReader->SetScreenSize(m_iWidth, m_iHeight);

    LoadSystemDarkenLevel();

    DK_FONT_SMOOTH_TYPE fontSmoothType = DK_FONT_SMOOTH_NORMAL;
    if(SystemSettingInfo::GetInstance()->GetFontRender() == 0)
    {
        fontSmoothType = DK_FONT_SMOOTH_SHARP;
    }
    m_pBookReader->SetFontSmoothType(fontSmoothType);
    
    bool bFontnameChanged = false;
    LoadSystemFontnameChange(bFontnameChanged);
    
    bool bIsInstant = false;
    //bIsInstant = IsInstantTurnPageModel();
    m_pBookReader->SetInstantTurnPageModel(bIsInstant);
    
    m_pBookReader->SetFontChange(bFontnameChanged);

    DK_LAYOUTSETTING    clslayout;
    LoadSystemLayoutInfo(&clslayout);
    if (!m_pBookReader->SetLayoutSettings(clslayout))
    {
        return false;
    }
    
    m_pBookReader->SetFamiliarToTraditional(SystemSettingInfo::GetInstance()->GetFontStyle_FamiliarOrTraditional());

    InitUI();

    if (DFF_PortableDocumentFormat == m_eFormat)
    {
        m_pBookReader->SetPdfModeController(&m_modeController);
    }

    m_turnPageNum = 0;
    if(!m_localBookID.empty())
    {
        ReadingHistoryStat::GetInstance()->OnBookOpen(
                m_localBookID.c_str(),
                m_strBookName.c_str(),
                m_eFormat,
                m_isTrialBook);
    }   
    // 跳转到上次位置
    return GotoBookmark(&readingBookmark);
}

void UIBookReaderContainer::OnLoad()
{
    //目前不需要做什么事情
    if (FALSE == CreateBookReader())
    {
        CPageNavigator::BackToEntryPage();
    }

    // 加入从屏保状态退出时提示语    
    CDisplay*     pDisplay = CDisplay::GetDisplay();
    if (pDisplay)
    {
        UITextSingleLine textInfo;
        textInfo.SetFontSize(GetWindowFontSize(FontSize22Index));
        textInfo.SetText(StringManager::GetPCSTRById(SCREENSAVER_OUT_MESSAGE));
        textInfo.SetAlign(ALIGN_CENTER);
        textInfo.SetBackColor(ColorManager::knWhite);
        textInfo.SetBackTransparent(false);
        const int screenWidth = pDisplay->GetScreenWidth();
        const int screenHeight = pDisplay->GetScreenHeight();
        const int width = textInfo.GetTextWidth();
        const int height = textInfo.GetTextHeight();
        const int left = (screenWidth - width) >> 1;
        const int top = (screenHeight - height) >> 1;
        textInfo.MoveWindow(left, top, width, height);
        DK_IMAGE img = pDisplay->GetMemDC();
        img.EraseBackground(ColorManager::knWhite);
        textInfo.Draw(img);
        DkRect rect = DkRect::FromLeftTopWidthHeight(0, 0, screenWidth, screenHeight);
        pDisplay->ForceDraw(rect, PAINT_FLAG_FULL);

        pDisplay->EndCache();
    }

    InitBookReader();
    m_isScreenSaverOut = (GUIHelper::GetScreenSaverStatus() == 1);
	if(m_isScreenSaverOut)
	{
		//如果50秒之内就解除屏保，此时仍然在联网状态
		BeginGetReadingData();
	}
}

void UIBookReaderContainer::OnUnLoad()
{
    if(m_bIsAutoPageDown)
    {
        UpdateAutoPageDownStatus(FALSE);
    }
    SaveReadingInfo();
	if(GUIHelper::GetScreenSaverStatus() == 1)
	{
		PostLocalReadingDataToCloud();
	}
    ResetContainer();
}

void UIBookReaderContainer::OnEnter()
{
    if (NULL == m_pBookReader)
    {
        return;
    }

    InitBookReader();
	if(m_isGotoBookMarkFromOpenBook)
	{
		m_isGotoBookMarkFromOpenBook = false;
		BeginGetReadingData();
	}
}

void UIBookReaderContainer::OnLeave()
{
    if(m_bIsAutoPageDown)
    {
        UpdateAutoPageDownStatus(FALSE);
    }
    SaveReadingInfo();
    StopTTS();
}

bool UIBookReaderContainer::IsPageInfoShowPercent() const
{
    return (DFF_MobiPocket == m_eFormat);
}

bool UIBookReaderContainer::DrawZoomedPdfPosBar(DK_IMAGE drawingImg)
{
    if (!IsZoomedPdf() || !m_pBookReader)
    {
        m_zoomedPdfHorizontalPos.SetVisible(FALSE);
        m_zoomedPdfVerticalPos.SetVisible(FALSE);
        return false;
    }
    char cText[50] = {0};
    long lPageIndex = m_pBookReader->GetCurrentPageIndex();
    long lPageCount = m_pBookReader->GetTotalPageNumber();
    snprintf(cText, sizeof(cText), "%ld / %ld", (lPageIndex + 1), lPageCount);
    m_zoomedPdfHorizontalPos.SetVisible(TRUE);
    m_zoomedPdfVerticalPos.SetVisible(TRUE);
    m_zoomedPdfHorizontalPos.SetProgressText(string(cText));
    return true;
}

bool UIBookReaderContainer::UpdateBottomBar()
{
    // TODO: 进度条的进度文字显示应该放在控件内部
    if (!m_pBookReader || !m_clsReaderProgressBar.IsVisible())
    {
        return false;
    }

    long lPageIndex = m_pBookReader->GetCurrentPageIndex();
    long lPageCount = m_pBookReader->GetTotalPageNumber();
    m_clsReaderProgressBar.SetMaximum(lPageCount);
    m_clsReaderProgressBar.SetProgress(lPageIndex+1);

    string strProgress = "";
    if (IsPageInfoShowPercent())
    {
        char cText[50] = {0};
        double curPercent = (double)(lPageIndex + 1)/lPageCount;
        curPercent *= 100;
        snprintf(cText, sizeof(cText), "%.2f%%", curPercent);
        strProgress = cText;
    }
    else
    {
        if(lPageIndex == -1)
        {
            strProgress = StringManager::GetPCSTRById(PAGINT);
        }
        else
        {
            char cText[50] = {0};
            snprintf(cText, sizeof(cText), "%ld / %ld", (lPageIndex + 1), lPageCount);
            strProgress = cText;
        }
    }

    m_clsReaderProgressBar.SetProgressText(strProgress);
    return true;
}

bool UIBookReaderContainer::DrawSearchData(DK_IMAGE drawingImg)
{
    for (unsigned int i = 0; i < m_vHighlightRects.size(); ++i)
    {
        DK_RECT rect = m_vHighlightRects[i];
        drawingImg.ReverseRect(rect.left, rect.top, rect.GetWidth(), rect.GetHeight());
    }

    return true;
}

void UIBookReaderContainer::SetFullRepaint()
{
    if (m_firstDrawPage)
    {
        m_firstDrawPage = false;
        CDisplay::GetDisplay()->SetFullRepaint(true);
    }
    else if (m_fullRepaint)
    {
        m_lastPageFullScreen = true;
        CDisplay::GetDisplay()->SetFullRepaint(true);
    }
    else if (m_lastPageFullScreen)
    {
        m_lastPageFullScreen = false;
        CDisplay::GetDisplay()->SetFullRepaint(true);
    }
}

HRESULT UIBookReaderContainer::Draw(DK_IMAGE drawingImg)
{
    if (!IsDisplay())
    {
        return S_OK;
    }
    SetFullRepaint();
    m_imageReader = drawingImg;
    m_imageReader.pbData = drawingImg.pbData;

    drawingImg.EraseBackground(ColorManager::knWhite);

    const bool zoomedPdf = IsZoomedPdf();
    m_zoomedPdfHorizontalPos.SetVisible(zoomedPdf);
    m_zoomedPdfVerticalPos.SetVisible(zoomedPdf);

    DrawBookReaderData(drawingImg);
    DrawInteractiveImageSymbol(drawingImg);
    DrawDotProgress(drawingImg);
    bool drawCommentTapImage = true;
    if (zoomedPdf)
    {
        DrawZoomedPdfPosBar(drawingImg);
        drawCommentTapImage = m_modeController.m_zoomX0 > m_pageWidth * (m_modeController.m_dUserZoom - 1);
    }
    else
    {
        DrawSearchData(drawingImg);
        UpdateBottomBar();
    }
    m_clsSelected.ResetDrawMode();
    m_clsSelected.SetDrawMode(UIBookReaderSelected::DRAW_ALL_SELECTION);
    DrawSelected(drawingImg, false);
    DrawSelectionImage(drawingImg);
    DrawBookMark(drawingImg, drawCommentTapImage);
    UIContainer::Draw(drawingImg);
    StopHourGlass();
    return S_OK;
}

BOOL UIBookReaderContainer::IsPageDownFinished()
{
    //翻至最好一页必须满足该书已被解析完成，否则该书不能算读完
    return m_bLastPage && IsParseFinished();
}

BOOL UIBookReaderContainer::IsPageUpFinished()
{
    return m_bFirstPage;
}

void UIBookReaderContainer::SetPageDownFinished()
{
}

void UIBookReaderContainer::SetPageUpFinished()
{
}

BOOL UIBookReaderContainer::GotoLastPage()
{
    return TRUE;
}

BOOL UIBookReaderContainer::OnKeyPress (int iKeyCode)
{
    return FALSE;
}

void UIBookReaderContainer::OnTTSMessage(SNativeMessage* msg)
{
    if (IsScannedPdf() || (msg->iType != KMessageTTSChange))
    {
        return ;
    }

    CTextToSpeech* pTTS = CTextToSpeech::GetInstance();
    if (NULL == pTTS)
        return;
    
    if(m_pBookReader->GetCurrentPageText().length() == 0)
    {
        return;
    }

    switch(msg->iParam2)
    {
    case TTS_MESSAGE_PLAY:
        {
            if (pTTS->TTS_Init())
            {
                pTTS->TTS_Play(m_pBookReader);
            }
        }
        break;
    case TTS_MESSAGE_NEXTPAGE_MANUAL:
        if (pTTS->TTS_IsPlay())
        {
            pTTS->TTS_Stop();
            sleep(1);
            pTTS->TTS_Play(m_pBookReader);
        }
        break;
    case TTS_MESSAGE_NEXTPAGE_AUTO:
        if (pTTS->TTS_IsPlay())
        {

            pTTS->TTS_Stop();
            if (BOOKREADER_GOTOPAGE_SUCCESS == m_pBookReader->GotoNextPage())
            {
                Repaint ();
                pTTS->TTS_Play(m_pBookReader);
            }
            else
            {
                pTTS->TTS_UnInit();
            }
            UpdateAutoPageDownStatus(TRUE);
        }
        break;
    default :
        break;
    }
}

void UIBookReaderContainer::ResetContainer()
{
    StopTTS();
    ClearPageObjectsInCurPage();
    m_bInialized = false;
    m_eLastReadingMode = PDF_RM_NormalPage;
    SafeDeletePointer(m_pReaderSettingDlg);
    SafeDeletePointer(m_pBookReader);
    m_clsSelected.Initialize(NULL);

    m_bFirstPage = true;
    m_bLastPage = false;

    ClearSelection();
    m_vHighlightRects.clear();
    SafeFreePointer(m_pInitBMP);
    SafeFreePointer(m_pImgStream);

    m_gestureDetector.ResetHoldingTimeOut();
    DebugLog(DLC_CHENM, "UIBookReaderContainer::ResetContainer() End");
}


//TODO: 仅用于RAR/Zip中的文件字段
int UIBookReaderContainer::GetCurFileId()
{
    return -1;
}


int  UIBookReaderContainer::GetBookID()
{
    return m_iBookID;
}

bool UIBookReaderContainer::GotoBookmark(const ICT_ReadingDataItem* _bookmark)
{
    DK_PROFILE_FUNCTION;
    if(m_pBookReader && m_pBookReader->GotoBookMark(_bookmark))
    {
        Repaint();
		m_isPageTurned = true;
        OnReadLocationChanged();
        return true;
    }
#ifdef ENABLE_PROFILE
    DebugPrintf(DLC_DIAGNOSTIC, "------%d/%d------", m_pBookReader->GetCurrentPageIndex() + 1, m_pBookReader->GetTotalPageNumber());
    Profiler::DumpAndReset();
#endif

    return false;
}

void UIBookReaderContainer::UpdateAutoPageDownStatus (BOOL fRestartTimer)
{
    DebugLog(DLC_CHENM, "UIBookReaderContainer::UpdateAutoPageDownStatus() Start");
    if (-1 != m_iAutoPageDownTimerId)
    {
        DebugPrintf (DLC_LIUHJ, "UIBookReaderContainer Auto Page Down Timer Stop");

        UnregisterTimer(m_iAutoPageDownTimerId);
        m_iAutoPageDownTimerId = -1;
        PowerManagement::DKPowerManagerImpl::GetInstance()->ReleaseLock(PM_LOCK_SCREEN, MODULE_DKREADER);
    }

    if (m_bIsAutoPageDown && fRestartTimer)
    {
        DebugPrintf (DLC_LIUHJ, "UIBookReaderContainer Auto Page Down Timer (Re)Start");

        PowerManagement::DKPowerManagerImpl::GetInstance()->AccquireLock(PM_LOCK_SCREEN, MODULE_DKREADER);
        m_iAutoPageDownTimerId = RegisterTimer(m_iAutoPageDownSecondCount, FALSE);
    }
    DebugLog(DLC_CHENM, "UIBookReaderContainer::UpdateAutoPageDownStatus() End");
}

void UIBookReaderContainer::UpdateReadingTitle()
{
    m_txtChapterTitle.SetVisible(false);
    if(!m_pBookReader
        || !SystemSettingInfo::GetInstance()->GetReadingLayout()
        || DFF_PortableDocumentFormat == m_eFormat
        || IsComicsChapter()
        || m_pBookReader->IsCurrentPageFullScreen()
        || m_clsSelected.IsSelectionMode())
    {
        return;
    }

    ATOM posCurPageAtom, endAtom;
    m_pBookReader->GetCurrentPageATOM(posCurPageAtom, endAtom);
    const DK_TOCINFO* posCurChapterInfo = m_pBookReader->GetChapterInfo(posCurPageAtom.ToFlowPosition(), CURRENT_CHAPTER, m_dkxBlock);
    if(!posCurChapterInfo)
    {
        return;
    }

    //如果当前页为当前章的第一页时不显示标题头
    if(posCurChapterInfo->GetBeginPos() == posCurPageAtom.ToFlowPosition())
    {
        return;
    }
    m_txtChapterTitle.SetVisible(true);
    m_txtChapterTitle.SetUseTraditionalChinese((bool)SystemSettingInfo::GetInstance()->GetFontStyle_FamiliarOrTraditional());
    m_txtChapterTitle.SetText(posCurChapterInfo->GetChapter().c_str());
    m_txtChapterTitle.MoveWindow (m_iLeft + m_iHorizontalMargin, GetWindowMetrics(UIBookReaderChapterTitleTopMarginIndex),
        m_iWidth - 2*m_iHorizontalMargin, GetWindowMetrics(UIBookReaderChapterTitleHeightIndex));
}

bool UIBookReaderContainer::DrawSelectionImage(DK_IMAGE drawingImg)
{
    if (!m_imageSelectedEnd.IsVisible() || !m_imageSelectedStart.IsVisible())
    {
        return false;
    }
    if (IsWritingRTL())
    {
        return false;
    }
    bool result = false;
    const int offset = GetWindowMetrics(UIPixelValue10Index);
    if (m_pBookReader && m_clsSelected.IsInitialized() && m_clsSelected.IsSelectionMode())
    {
        const DK_RECT rectEnd = m_clsSelected.GetEndRect();
        const DK_RECT rectStart = m_clsSelected.GetStartRect();

        int startSizeX = rectStart.GetHeight();
        int endSizeY = rectEnd.GetHeight();
        if (IsVerticalForms())
        {
            startSizeX = rectStart.GetWidth();
            endSizeY = rectEnd.GetWidth();
        }
        m_imageSelectedStart.SetMaxSize(dkSize(startSizeX, startSizeX));
        m_imageSelectedEnd.SetMaxSize(dkSize(endSizeY, endSizeY));
        UpdateSelectionImage();

        if (CUtility::IsEmptyRect(rectEnd))
        {
            m_imageSelectedEnd.SetVisible(FALSE);
        }
        else
        {
            int left = 0, top = 0;
            dkSize size = m_imageSelectedEnd.GetMinSize();
            const int width = size.GetWidth();
            const int height = size.GetHeight();
            if (IsVerticalForms())
            {
                left = (rectEnd.left + rectEnd.right - width) >> 1;
                if (IsVerticalRTL())
                {
                    top = rectEnd.bottom + offset;
                }
                else
                {
                    top = rectEnd.top - height - offset;
                }
            }
            else
            {
                top = (rectEnd.top + rectEnd.bottom - height) >> 1;
                if (IsHorizontalHTB())
                {
                    left = rectEnd.right + offset;
                }
                else
                {
                    left = rectEnd.left - width - offset;
                }
            }
            if (top < 0)
            {
                top = 0;
            }
            if (left < 0)
            {
                left = 0;
            }
            m_imageSelectedEnd.MoveWindow(left, top, width, height);
            m_imageSelectedEnd.Draw(drawingImg);
            result = true;
        }

        if (CUtility::IsEmptyRect(rectStart))
        {
            m_imageSelectedStart.SetVisible(FALSE);
        }
        else
        {
            int left = 0, top = 0;
            dkSize size = m_imageSelectedStart.GetMinSize();
            const int width = size.GetWidth();
            const int height = size.GetHeight();
            if (IsVerticalForms())
            {
                left = (rectStart.left + rectStart.right - width) >> 1;
                if (IsVerticalRTL())
                {
                    top = rectStart.top - height - offset;
                }
                else
                {
                    top = rectStart.bottom + offset;
                }
            }
            else
            {
                top = (rectStart.top + rectStart.bottom - height) >> 1;
                if (IsHorizontalHTB())
                {
                    left = rectStart.left - width - offset;
                }
                else
                {
                    left = rectStart.right + offset;
                }
            }
            if (top < 0)
            {
                top = 0;
            }
            if (left < 0)
            {
                left = 0;
            }
            m_imageSelectedStart.MoveWindow(left, top, width, height);
            m_imageSelectedStart.Draw(drawingImg);
            result = true;
        }
    }
    return result;
}

bool UIBookReaderContainer::DrawBookReaderData(DK_IMAGE drawingImg)
{
    if (NULL == m_pBookReader)
    {
        return false;
    }

    DK_IMAGE* pImage = m_pBookReader->GetPageBMP();
    if (NULL == pImage || NULL == pImage->pbData)
    {
        return false;
    }

    DK_RECT rcSelf ={m_iLeft+ m_iHorizontalMargin , m_iTop + m_iTopMargin , m_iLeft + m_iWidth - m_iHorizontalMargin, m_iTop + m_iHeight - m_iBottomMargin };
    if (IsZoomedPdf())
    {
        rcSelf.right = m_iLeft+ m_iHorizontalMargin + pImage->iWidth;
        rcSelf.bottom = m_iTop + m_iTopMargin + pImage->iHeight;
    }
    else
    {
        int iSelfWidth = rcSelf.right - rcSelf.left;
        int iSelfHeight = rcSelf.bottom - rcSelf.top;

        if (iSelfWidth > pImage->iWidth)
        {
            int iTmp = iSelfWidth - pImage->iWidth;
            rcSelf.left += iTmp >> 1;
            rcSelf.right -= iTmp - (iTmp >> 1);
        }
        if (pImage->iWidth > iSelfWidth && pImage->iWidth <= m_iWidth)
        {
            int iTmp = m_iWidth - pImage->iWidth;
            rcSelf.left = iTmp / 2;
            rcSelf.right = m_iWidth - rcSelf.left;
        }
        if (iSelfHeight > pImage->iHeight)
        {
            int iTmp = iSelfHeight - pImage->iHeight;
            rcSelf.top += iTmp >> 1;
            rcSelf.bottom -= iTmp - (iTmp >> 1);
        }
        if (pImage->iHeight > iSelfHeight && pImage->iHeight <= m_iHeight)
        {
            int iTmp = m_iHeight - pImage->iHeight;
            rcSelf.top = iTmp / 2;
            rcSelf.bottom = m_iHeight - rcSelf.top;
        }
    }

    HRESULT hr;

    DK_IMAGE imgSelf;
    RTN_HR_IF_FAILED(CropImage( drawingImg,  rcSelf, &imgSelf ));

    CopyImage(imgSelf, *pImage);

    UpdateReadingTitle();
    return DK_TRUE;
}

void UIBookReaderContainer::UpdateSelectionImage()
{
    int startImageId = IMAGE_TOUCH_ICON_SLID_LEFT;
    int endImageId = IMAGE_TOUCH_ICON_SLID_RIGHT;
    if (IsVerticalRTL())
    {
        startImageId = IMAGE_TOUCH_ICON_SLID_UP;
        endImageId = IMAGE_TOUCH_ICON_SLID_DOWN;
    }
    else if (IsVerticalLTR())
    {
        startImageId = IMAGE_TOUCH_ICON_SLID_DOWN;
        endImageId = IMAGE_TOUCH_ICON_SLID_UP;
    }
    else if (IsHorizontalHTB())
    {
        startImageId = IMAGE_TOUCH_ICON_SLID_LEFT;
        endImageId = IMAGE_TOUCH_ICON_SLID_RIGHT;
    }
    else
    {
        startImageId = IMAGE_TOUCH_ICON_SLID_RIGHT;
        endImageId = IMAGE_TOUCH_ICON_SLID_LEFT;
    }

    m_imageSelectedStart.SetImageFile(ImageManager::GetImagePath(startImageId));
    m_imageSelectedEnd.SetImageFile(ImageManager::GetImagePath(endImageId));
}

bool UIBookReaderContainer::InitUI()
{
    int progressBarHeight = GetWindowMetrics(UIBookReaderProgressBarHeightIndex);
    m_clsReaderProgressBar.MoveWindow (m_iLeft, m_iTop + m_iHeight - progressBarHeight, m_iWidth , progressBarHeight);

    m_zoomedPdfHorizontalPos.MoveWindow (m_iLeft, m_iTop + m_iHeight - PDF_ZOOM_POSHEIGHT , m_iWidth , PDF_ZOOM_POSHEIGHT);
    m_zoomedPdfHorizontalPos.SetVisible(FALSE);
    m_zoomedPdfVerticalPos.MoveWindow (m_iLeft + m_iWidth - PDF_ZOOM_POSHEIGHT, m_iTop, PDF_ZOOM_POSHEIGHT , m_iHeight - PDF_ZOOM_POSMARGIN);
    m_zoomedPdfVerticalPos.SetDirection(UIBookReaderProgressBar::PBD_Vertical);
    m_zoomedPdfVerticalPos.SetVisible(FALSE);

    m_imageSelectedStart.SetAutoSize(true);
    m_imageSelectedStart.SetVisible(FALSE);
    m_imageSelectedEnd.SetAutoSize(true);
    m_imageSelectedEnd.SetVisible(FALSE);

    m_txtChapterTitle.SetAlign (ALIGN_LEFT);
    m_txtChapterTitle.SetForeColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
    m_txtChapterTitle.SetFontSize(GetWindowFontSize(FontSize16Index));

    // TODO:修整进度条的接口,过于冗余.
    UpdateProgressBarMode();
    return true;
}

string UIBookReaderContainer::GetUserComment(const CT_RefPos& beginPos, const CT_RefPos& endPos)
{
    string comment;
    if (NULL == m_dkxBlock || m_dkxBlock->GetBookmarkCount() == 0)
    {
        return comment;
    }

    // 从DKX读取数据
    vector<ICT_ReadingDataItem *> vBookmarks;
    if(!m_dkxBlock->GetAllBookmarks(&vBookmarks))
    {
        return comment;
    }

    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "UIBookReaderContainer::GetUserComment() FetchAllBookmarks: %d", vBookmarks.size());
    for (vector<ICT_ReadingDataItem *>::iterator vi = vBookmarks.begin(); vi != vBookmarks.end(); ++vi)
    {
        if (NULL == (*vi))
        {
            continue;
        }

        if ((ICT_ReadingDataItem::COMMENT == (*vi)->GetUserDataType())
            && ((*vi)->GetBeginPos() == beginPos)
            && ((*vi)->GetEndPos()) == endPos)
        {
            comment = (*vi)->GetUserContent();
            break;
        }
    }

    for (DK_AUTO(cur, vBookmarks.begin()); cur != vBookmarks.end(); ++cur)
    {
        DKXReadingDataItemFactory::DestoryReadingDataItem(*cur);
    }
    return comment;
}

void UIBookReaderContainer::InitSelectBasePoint()
{
    DK_POS startPos;
    DK_POS endPos;
    if (DFF_ElectronicPublishing == m_eFormat)
    {
        m_clsSelected.SetBasePoint(0, 0);
        if (IsVerticalRTL())
        {
            startPos = DK_POS(m_iWidth, 0);
            endPos = DK_POS(0, m_iHeight);
        }
        else
        {
            startPos = DK_POS(0, 0);
            endPos = DK_POS(m_iWidth, m_iHeight);
        }
    }
    else if (DFF_PortableDocumentFormat == m_eFormat)
    {
        m_clsSelected.SetBasePoint(m_iHorizontalMargin, m_iTopMargin);
        const int left = m_iHorizontalMargin;
        const int top = m_iTopMargin;
        const int right = m_iLeft + m_iWidth - m_iHorizontalMargin;
        const int bottom = m_iTop + m_iHeight - m_iBottomMargin;
        switch (m_modeController.m_iRotation)
        {
        case 0:
            startPos = DK_POS(left, top);
            endPos = DK_POS(right, bottom);
            break;
        case 180:
            startPos = DK_POS(right, bottom);
            endPos = DK_POS(left, top);
            break;
        case 90:
            startPos = DK_POS(left, bottom);
            endPos = DK_POS(right, top);
            break;
        default: // 270
            startPos = DK_POS(right, top);
            endPos = DK_POS(left, bottom);
            break;
        }
    }
    else
    {
        m_clsSelected.SetBasePoint(m_iHorizontalMargin, m_iTopMargin);
        startPos = DK_POS(m_iHorizontalMargin, m_iTopMargin);
        endPos = DK_POS(m_iWidth - 2 * m_iHorizontalMargin, m_iHeight - m_iTopMargin - m_iBottomMargin);
    }
    m_clsSelected.SetPageUpLeftAndRightDownPoint(startPos, endPos);
}

bool UIBookReaderContainer::DrawSelected(DK_IMAGE drawingImg, bool updateToScreen)
{
    if (NULL == m_pBookReader)
    {
        return false;
    }

	BookTextController *pSelectedControler = m_pBookReader->GetBookTextControlerOfCurPage();
	if(!m_clsSelected.Initialize(pSelectedControler) || !m_clsSelected.IsSelectionMode())
	{
		DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s Initialize error", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
		return false;
	}
	InitSelectBasePoint();

    // 需要局刷的矩形
    DK_RECT rcSelf;
    CDisplay* pDisplay = CDisplay::GetDisplay();
    if (m_clsSelected.Draw(drawingImg, &rcSelf) && updateToScreen && pDisplay)
    {
        CDisplay::CacheDisabler cache;
        UpdateToScreen(rcSelf, PAINT_FLAG_DU);
        DebugPrintf(DLC_UIBOOKREADERCONTAINER, "UIBookReaderContainer::DrawSelected() UpdateToScreen, %d,%d -> %d,%d", rcSelf.left, rcSelf.top, rcSelf.right, rcSelf.bottom);
    }
    m_processMove = true;
    return true;
}

bool UIBookReaderContainer::DrawBookMark(DK_IMAGE drawingImg, bool drawComment)
{
    m_bExistBookMarkForCurPage = false;
    if (NULL == m_dkxBlock)
    {
        return false;
    }
    if (m_dkxBlock->GetBookmarkCount() == 0)
    {
        return true;
    }

    // 从DKX读取数据
    vector<ICT_ReadingDataItem *> vBookmarks;
    if(!m_dkxBlock->GetAllBookmarks(&vBookmarks))
    {
        return false;
    }

	ATOM pageBeginPos, pageEndPos;
	if(!m_pBookReader->GetCurrentPageATOM(pageBeginPos, pageEndPos))
	{
		return false;
	}
	if(IsScannedPdf())
	{
		//TODO: 非重排PDF页末三级索引同页首相同
		pageEndPos.SetChapterIndex(pageEndPos.GetChapterIndex() + 1);
	}

    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "UIBookReaderContainer::DrawBookMark() FetchAllBookmarks: %d", vBookmarks.size());
    m_bExistBookMarkForCurPage = false;
    m_commentTapLocation.clear();
    for (unsigned int i = 0; i < vBookmarks.size(); ++i)
    {
        if (NULL == vBookmarks[i])
        {
            continue;
        }

		const ATOM& markBeginPos = vBookmarks[i]->GetBeginPos();
		const ATOM& markEndPos = vBookmarks[i]->GetEndPos();
		if(pageBeginPos.CompareTo(markEndPos) > 0 || pageEndPos.CompareTo(markBeginPos) <= 0)
		{
			continue;
		}

        switch ((vBookmarks[i])->GetUserDataType())
        {
        case ICT_ReadingDataItem::BOOKMARK:
            // 书签
            {
                if (m_pBookReader->IsPositionInCurrentPage(markBeginPos.ToFlowPosition()))
                {
                    m_bExistBookMarkForCurPage = true;
                    m_uIndexOfBookMarkForCurPage = i;
                    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "UIBookReaderContainer::%s() Exist bookmark for current page", __FUNCTION__);

                    SPtr<ITpImage> spImageCursor = ImageManager::GetImage(IMAGE_BOOKMARK);
                    int rightGap = (m_iHorizontalMargin < (int)(spImageCursor->GetWidth()))? 0: (m_iHorizontalMargin - (int)(spImageCursor->GetWidth())) / 2;
                    DK_RECT rcSelf;
                    if (IsVerticalRTL())
                    {
                        rcSelf.left = rightGap;
                        rcSelf.right = rcSelf.left + spImageCursor->GetWidth();
                    }
                    else
                    {
                        rcSelf.right  = m_iWidth - rightGap;
                        rcSelf.left   = rcSelf.right - spImageCursor->GetWidth();
                    }
                    rcSelf.top    = 0;
                    rcSelf.bottom = rcSelf.top + spImageCursor->GetHeight();
                    DK_IMAGE imgSelf;
                    if (S_OK == CropImage(drawingImg, rcSelf, &imgSelf))
                    {
                        CTpGraphics grf(imgSelf);
                        grf.DrawImageBlend(spImageCursor.Get(), 0, 0, 0, 0, spImageCursor->GetWidth(), spImageCursor->GetHeight());
                    }
                }
                else
                {
                }
            }
            break;
        case ICT_ReadingDataItem::DIGEST:
            // 书摘
            if (m_clsSelected.IsInitialized())
            {
                DebugPrintf(DLC_UIBOOKREADERCONTAINER, "UIBookReaderContainer::%s() DrawLine DIGEST markBeginPos(%d, %d, %d)", __FUNCTION__, markBeginPos.GetChapterIndex(), markBeginPos.GetParaIndex(), markBeginPos.GetAtomIndex());
                DebugPrintf(DLC_UIBOOKREADERCONTAINER, "UIBookReaderContainer::%s() DrawLine DIGEST markEndPos(%d, %d, %d)", __FUNCTION__, markEndPos.GetChapterIndex(), markEndPos.GetParaIndex(), markEndPos.GetAtomIndex());

                DebugPrintf(DLC_UIBOOKREADERCONTAINER, "UIBookReaderContainer::%s() DrawLine DIGEST", __FUNCTION__);
                m_clsSelected.DrawDigest(markBeginPos, markEndPos, drawingImg);
            }
            break;
        case ICT_ReadingDataItem::COMMENT:
            // 批注
            {
                if (m_clsSelected.IsInitialized())
                {
                    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "UIBookReaderContainer::%s() DrawLine COMMENT", __FUNCTION__);
                    UIBookReaderSelected::CommentDrawType commentDrawType = IsVerticalForms() ? UIBookReaderSelected::CDT_GrayRect : UIBookReaderSelected::CDT_WaveLine;
                    DK_RECT lastRect = m_clsSelected.DrawComment(markBeginPos, markEndPos, drawingImg, commentDrawType);
                    int topGap = lastRect.top;
                    if (topGap > 0 && drawComment)
                    {
                        SPtr<ITpImage> spImageCursor = ImageManager::GetImage(IMAGE_TOUCH_ICON_COMMENT);
                        int rightGap = (m_iHorizontalMargin < (int)(spImageCursor->GetWidth()))? 0: (m_iHorizontalMargin - (int)(spImageCursor->GetWidth())) / 2;
                        DK_RECT rcSelf;
                        if (IsVerticalForms())
                        {
                            rcSelf.right = lastRect.left;
                            rcSelf.right = rcSelf.right < spImageCursor->GetWidth() ? spImageCursor->GetWidth() : rcSelf.right;
                            rcSelf.left = rcSelf.right - spImageCursor->GetWidth();
                            rcSelf.top = lastRect.bottom;
                            rcSelf.top = rcSelf.top + spImageCursor->GetHeight() > m_iHeight ? m_iHeight - spImageCursor->GetHeight() : rcSelf.top;
                            rcSelf.bottom = rcSelf.top + spImageCursor->GetHeight();
                        }
                        else
                        {
                            rcSelf.right  = m_iWidth - rightGap;
                            rcSelf.left   = rcSelf.right - spImageCursor->GetWidth();
                            rcSelf.top    = topGap;
                            rcSelf.bottom = rcSelf.top + spImageCursor->GetHeight();
                        }
                        DK_IMAGE imgSelf;
                        if (S_OK == CropImage(drawingImg, rcSelf, &imgSelf))
                        {
                            CTpGraphics grf(imgSelf);
                            grf.DrawImageBlend(spImageCursor.Get(), 0, 0, 0, 0, spImageCursor->GetWidth(), spImageCursor->GetHeight());
                        }

                        const int RECTOFFSET = 30;
                        COMMENT_TAP_LOCATION commentTap;
                        commentTap.imgRect.left = rcSelf.left - RECTOFFSET;
                        commentTap.imgRect.top = (rcSelf.top < RECTOFFSET) ? 0 : (rcSelf.top - RECTOFFSET); 
                        commentTap.imgRect.right = (rcSelf.right + RECTOFFSET) > m_iWidth ? m_iWidth : rcSelf.right + RECTOFFSET;
                        commentTap.imgRect.bottom = ((rcSelf.bottom + RECTOFFSET) > m_iHeight) ? m_iHeight : (rcSelf.bottom + RECTOFFSET);
                        commentTap.startPos = markBeginPos;
                        commentTap.endPos = markEndPos;
                        m_commentTapLocation.push_back(commentTap);
                        DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s() m_commentTapLocation.size() = %d ", __FUNCTION__, m_commentTapLocation.size());
                    }
                }
            }
            break;
        default:
            break;
        }

        delete vBookmarks[i];
    }

    vBookmarks.clear();
    DebugLog(DLC_UIBOOKREADERCONTAINER, "UIBookReaderContainer::DrawBookMark() Succeeded");
    return true;
}

bool UIBookReaderContainer::SaveBookMark(ICT_ReadingDataItem *_pclsBookmark)
{
    if(m_strBookFilePath.empty())
    {
        return false;
    }

    if(NULL == m_dkxBlock)
    {
        return false;
    }

    // add chapter title
    string chapterTitle = m_pBookReader->GetChapterTitle(_pclsBookmark->GetBeginPos().ToFlowPosition(), m_dkxBlock);
    _pclsBookmark->SetChapterTitle(chapterTitle);

    m_dkxBlock->AddBookmark(_pclsBookmark);
    m_dkxBlock->SetReadingDataModified(true);
    return m_dkxBlock->Serialize();
}

bool UIBookReaderContainer::UpdateBookMark(int _iIndex, ICT_ReadingDataItem *_pclsBookmark)
{
    if(m_strBookFilePath.empty() || NULL == m_dkxBlock)
    {
        return false;
    }
    m_readingDataChanged = true;

    ICT_ReadingDataItem*  pclsReadingProgress = DKXReadingDataItemFactory::CreateReadingDataItem();
    if(NULL == pclsReadingProgress)
    {
        return false;
    }

    if (m_dkxBlock->GetBookmarkByIndex(_iIndex, pclsReadingProgress))
    {
        CT_RefPos posPreBegin = pclsReadingProgress->GetBeginPos();
        CT_RefPos posPreEnd   = pclsReadingProgress->GetEndPos();
        CT_RefPos posCurBegin = _pclsBookmark->GetBeginPos();
        CT_RefPos posCurEnd   = _pclsBookmark->GetEndPos();

        if (posCurBegin.GetChapterIndex() < 0 || posPreBegin.CompareTo(posCurBegin) < 0)
        {
            posCurBegin = posPreBegin;
            _pclsBookmark->SetBeginPos(posPreBegin);
        }

        if (posCurEnd.GetChapterIndex() < 0 || posPreEnd.CompareTo(posCurEnd) > 0)
        {
            posCurEnd = posPreEnd;
            _pclsBookmark->SetEndPos(posPreEnd);
        }
        if (m_clsSelected.IsInitialized())
        {
            string strNewContent;
            m_clsSelected.GetStringBetweenPos(posCurBegin, posCurEnd, strNewContent);
            _pclsBookmark->SetBookContent(strNewContent);
        }
        m_dkxBlock->RemoveBookmarkByIndex(_iIndex);
    }
    DKXReadingDataItemFactory::DestoryReadingDataItem(pclsReadingProgress);

    // add chapter title
    string chapterTitle = m_pBookReader->GetChapterTitle(_pclsBookmark->GetBeginPos().ToFlowPosition(), m_dkxBlock);
    _pclsBookmark->SetChapterTitle(chapterTitle);

    m_dkxBlock->AddBookmark(_pclsBookmark);
    m_dkxBlock->SetReadingDataModified(true);
    return m_dkxBlock->Serialize();
}

bool UIBookReaderContainer::RemoveBookMark(int _iIndex)
{
    m_readingDataChanged = true;
    if(m_strBookFilePath.empty() || NULL == m_dkxBlock)
    {
        return false;
    }

    m_dkxBlock->RemoveBookmarkByIndex(_iIndex);
    m_dkxBlock->SetReadingDataModified(true);
    return m_dkxBlock->Serialize();
}

bool UIBookReaderContainer::SaveReadingInfo()
{
    m_vDotProgress.clear();
    if(!m_pBookReader || !m_dkxBlock)
    {
        return false;
    }
    // pdf放大状态恢复原状
    ReadingMode savedReadingMode = m_modeController.m_eReadingMode;
    double userZoom = m_modeController.m_dUserZoom;
    double zoomX0 = m_modeController.m_zoomX0;
    double zoomY0 = m_modeController.m_zoomY0;
    if (m_modeController.m_eReadingMode == PDF_RM_ZoomPage)
    {
        m_modeController.m_eReadingMode = PDF_RM_NormalPage;
        m_modeController.m_dUserZoom = 1.0;
        m_modeController.m_zoomX0 = m_modeController.m_zoomY0 = 0;
        FinishZoomPdf(false);
    }

    CDKFileManager* fileManager = CDKFileManager::GetFileManager();
    int _iFileID = fileManager->GetFileIdByPath(m_strBookFilePath.c_str());
    PCDKFile _pclsFile = fileManager->GetFileById(_iFileID);

    if (DFF_PortableDocumentFormat == m_eFormat)
    {
        m_dkxBlock->SetPDFModeController(m_modeController);
    }

    ICT_ReadingDataItem* _pclsProgress = m_pBookReader->GetCurPageProgress();
	if(!_pclsProgress)
	{
		return false;
	}
    long lPageIndex = m_pBookReader->GetCurrentPageIndex();
	long lPageCount = m_pBookReader->GetTotalPageNumber();
	int percent = 0;
	if(lPageIndex != -1 && lPageCount > 0)
	{
		percent = (lPageIndex + 1)*100/lPageCount;
	}
	_pclsProgress->SetPercent(percent);

    m_dkxBlock->SetProgress(_pclsProgress);
    m_dkxBlock->SetLastReadTime(_pclsProgress->GetLastModifyTime());
    m_dkxBlock->SetReadingOrder(fileManager->GetReadingOrder());

    _pclsFile->SetFileLastReadTime(CUtility::TransferStringToTime(_pclsProgress->GetLastModifyTime()));
    _pclsFile->SetFileReadingOrder(fileManager->GetReadingOrder());
    //int _iProgress = _pclsFileManager->CalcProgress(_pclsProgress);
    //_pclsFile->SetFilePlayProcess(_iProgress);
    _pclsFile->SetFilePlayProcess(percent);
    SafeDeletePointerEx(_pclsProgress);

    m_modeController.m_eReadingMode = savedReadingMode;
    m_modeController.m_dUserZoom = userZoom;
    m_modeController.m_zoomX0 = zoomX0;
    m_modeController.m_zoomY0 = zoomY0;
    
    if(!m_localBookID.empty())
    {
        ReadingHistoryStat::GetInstance()->OnBookClose(m_turnPageNum);
    }
    m_turnPageNum = 0;
    m_dkxBlock->SetCrash(false);
    m_dkxBlock->Serialize();
    DKXManager::GetInstance()->Reset();
    return true;
}

bool UIBookReaderContainer::IsParseFinished()
{
    return m_pBookReader && 
        m_pBookReader->GetParseStatus() == BOOKREADER_PARSE_FINISHED;
}

void UIBookReaderContainer::ShowParseDialog(BOOKREADER_PARSE_STATUS clsStatus)
{
    UIMessageBox messagebox(this,
            StringManager::GetStringById(
                BOOKREADER_PARSE_START == clsStatus ?
                BOOK_PARSE_ISPARSING :
                BOOK_PARSE_ERROR),
            MB_TIPMSG);
    messagebox.DoModal();
}

void UIBookReaderContainer::StopTTS()
{
    CTextToSpeech* pTTS = CTextToSpeech::GetInstance();
    if (pTTS && pTTS->TTS_IsPlay())
    {
        DebugPrintf(DLC_UIBOOKREADERCONTAINER, "UIBookReaderContainer::StopTTS entering");
        SNativeMessage msg2;
        msg2.iType = KMessageTTSChange;
        msg2.iParam2 = TTS_MESSAGE_STOP;
        INativeMessageQueue::GetMessageQueue()->Send(msg2);
    }
}

void UIBookReaderContainer::PlayTTS()
{   
    if (IsScannedPdf())
    {
        return ;
    }

    CTextToSpeech* pTTS = CTextToSpeech::GetInstance();
    if (pTTS && !pTTS->TTS_IsPlay())
    {
        DebugPrintf(DLC_UIBOOKREADERCONTAINER, "UIBookReaderContainer::PlayTTS entering");
        SNativeMessage msg2;
        msg2.iType = KMessageTTSChange;
        msg2.iParam2 = TTS_MESSAGE_PLAY;
        INativeMessageQueue::GetMessageQueue()->Send(msg2);
    }
}

void UIBookReaderContainer::ManualTTS()
{
    CTextToSpeech* pTTS = CTextToSpeech::GetInstance();
    if (NULL != pTTS && pTTS->TTS_IsPlay())
    {
        SNativeMessage msg;
        msg.iType = KMessageTTSChange;
        msg.iParam2 = TTS_MESSAGE_NEXTPAGE_MANUAL; 
        INativeMessageQueue::GetMessageQueue()->Send(msg);
    }
}

bool UIBookReaderContainer::LoadSystemLayoutInfo(DK_LAYOUTSETTING* pLayout)
{
    if (pLayout)
    {
        pLayout->dFontSize    = abs(SystemSettingInfo::GetInstance()->GetFontSize());
        if(SystemSettingInfo::GetInstance()->GetTypeSetting() == LAYOUT_SYSLE_NORMAL)
        {
            pLayout->bOriginalLayout = true;
        }
        else
        {
            pLayout->bOriginalLayout = false;
            pLayout->dLineGap     = abs(SystemSettingInfo::GetInstance()->GetLineGap() * 0.01f);
            pLayout->dParaSpacing = abs(SystemSettingInfo::GetInstance()->GetParaSpacing() * 0.01f);
            pLayout->dTabStop     = abs(SystemSettingInfo::GetInstance()->GetTabStop());
            pLayout->dIndent      = abs(SystemSettingInfo::GetInstance()->GetBookIndent());

        }
        return true;
    }
    return false;
}
void UIBookReaderContainer::LoadSystemFontnameChange(bool &bChanged)
{
    bChanged = SystemSettingInfo::GetInstance()->IsFontNameChanged();
    SystemSettingInfo::GetInstance()->SetFontNameChanged(false);
}

bool UIBookReaderContainer::LoadSystemDarkenLevel()
{
    if (NULL == m_dkxBlock)
    {
        return false;
    }
    int iLevel = 0;
    if(IsScannedPdf())
    {
        m_dkxBlock->GetPDFDarkenLevel(&iLevel);

        switch(iLevel)
        {
        case 0:
            iLevel = 0;
            break;
            //return false;
        case 1:
            iLevel = 3;
            break;
        case 2:
            iLevel = 5;
            break;
        default:
            iLevel = 0;
            return false;
        }

        vector<IBMPProcessor*> processorList;
        m_pBookReader->GetAllBMPProcessor (processorList);

        vector<IBMPProcessor*>::iterator vi;
        for (vi = processorList.begin (); vi < processorList.end (); ++vi)
        {
            if ((*vi) && IBMPProcessor::DK_BMPPROCESSOR_DARKEN == (*vi)->GetType ())
            {
                m_pBookReader->RemoveBMPProcessor (*vi);
            }
        }

        IBMPProcessor* pDarkenProcessor = BMPProcessFactory::CreateInstance(IBMPProcessor::DK_BMPPROCESSOR_DARKEN);
        if (DK_NULL != pDarkenProcessor)
        {
            ((DarkenProcessor*)pDarkenProcessor)->SetDarkenLevel (iLevel);
            m_pBookReader->AddBMPProcessor(pDarkenProcessor);
        }
    }
    else
    {
        iLevel = SystemSettingInfo::GetInstance()->GetTextDarkLevel();

        double dGammaRate = 1.0f;
        switch(iLevel)
        {
        case 0:
            dGammaRate = 1.0f;
            break;
        case 1:
            dGammaRate = 1.5f;
            break;
        case 2:
            dGammaRate = 2.0f;
            break;
        default:
            dGammaRate = 1.0f;
            break;
        }

        m_pBookReader->SetTextGrayScaleLevel(dGammaRate);
    }

    return true;
}

bool UIBookReaderContainer::LoadSystemPageSizeInfo(int& pageWidth, int& pageHeight)
{
    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "UIBookReaderContainer::LoadSystemPageSizeInfo() Start");
    int rightMargin = 0;
    int readingMode = SystemSettingInfo::GetInstance()->GetReadingLayout();
    if (DFF_PortableDocumentFormat != m_eFormat || m_bIsPdfTextMode)
    {
        m_iTopMargin        = SystemSettingInfo::GetInstance()->GetPageTopMargin(readingMode);
        m_iBottomMargin     = SystemSettingInfo::GetInstance()->GetPageBottomMargin(readingMode);
        m_iHorizontalMargin = SystemSettingInfo::GetInstance()->GetPageHorizontalMargin(readingMode);
    }
#ifdef KINDLE_FOR_TOUCH
    else if (IsZoomedPdf())
    {
        m_iTopMargin = 0;
        m_iBottomMargin = PDF_ZOOM_POSMARGIN;
        m_iHorizontalMargin = 0;
        rightMargin = PDF_ZOOM_POSMARGIN;
    }
#endif
    else
    {
        m_iTopMargin = 0;
        m_iBottomMargin = 1;
        m_iHorizontalMargin = 0;
    }

    m_clsReaderProgressBar.SetLeftMargin(m_iHorizontalMargin);
    pageWidth  = m_iWidth - 2 * m_iHorizontalMargin - rightMargin;
    pageHeight = m_iHeight - m_iTopMargin - m_iBottomMargin;
    InitSelectBasePoint();
    return true;
}

void UIBookReaderContainer::StopHourGlass()
{
    CHourglass* pIns = CHourglass::GetInstance();
    if(pIns)
    {
        pIns->Stop();
    }
}

bool UIBookReaderContainer::MoveToPage(long _lPageIndex)
{
    StopTTS();
    if (!m_pBookReader)
    {
        return false;
    }

    int iPageEnd = m_pBookReader->GetTotalPageNumber() - 1; 
    if (iPageEnd < 0)
    {
        return false;
    }

    _lPageIndex = _lPageIndex < 0 ? 0 : _lPageIndex;
    _lPageIndex = (iPageEnd > 0 && _lPageIndex > iPageEnd) ? iPageEnd : _lPageIndex;

    if (BOOKREADER_GOTOPAGE_SUCCESS != m_pBookReader->MoveToPage(_lPageIndex))
    {
        return false;
    }
    OnReadLocationChanged();

    return true;
}

int UIBookReaderContainer::GotoPrePage()
{
    DK_PROFILE_FUNCTION;
    int iGotoPageStatus = m_pBookReader->GotoPrevPage();
    if (BOOKREADER_GOTOPAGE_SUCCESS == iGotoPageStatus)
    {
        ++m_turnPageNum;
        m_bFirstPage = false;
        if (IsInSelectionProgress() && m_isSelecting)
        {
            m_clsSelected.ResetDrawMode();
            m_clsSelected.SetDrawMode(UIBookReaderSelected::DRAW_ALL_SELECTION);
        }
        Repaint();
		m_isPageTurned = true;
        ManualTTS();
        OnReadLocationChanged();
        m_fullRepaint = GetFullRepaintFlag();
    }
    else if (BOOKREADER_GOTOPAGE_WAIT == iGotoPageStatus && !IsParseFinished())
    {
        ShowParseDialog(BOOKREADER_PARSE_START);
    }
    else
    {
        m_bFirstPage = true;
        m_bLastPage = false;
    }
#ifdef ENABLE_PROFILE
    DebugPrintf(DLC_DIAGNOSTIC, "------%d/%d------", m_pBookReader->GetCurrentPageIndex() + 1, m_pBookReader->GetTotalPageNumber());
    Profiler::DumpAndReset();
#endif
    return iGotoPageStatus;
}

bool UIBookReaderContainer::GetFullRepaintFlag()
{
    ImageFullRepaintCalculation* pFullRepaint = ImageFullRepaintCalculation::GetInstance();
    if (NULL == pFullRepaint || NULL == m_pBookReader)
    {
        return false;
    }
    if (m_pBookReader->IsCurrentPageFullScreen()
        || IsComicsChapter())
    {
        return true;
    }
    for (unsigned int i=0; i<m_vPageInteractiveObjects.size(); ++i)
    {
        const PageObjectInfo& object = m_vPageInteractiveObjects[i];
        switch (object.m_objectType)
        {
        case DKE_PAGEOBJ_INTERACTIVEIMAGE:
        case DKE_PAGEOBJ_IMAGE:
        case DKE_PAGEOBJ_STATICIMAGE:
        case DKE_PAGEOBJ_GALLERY:
            {
                const DK_RECT& bounding = object.m_bounding;
                pFullRepaint->SetRect(bounding);
            }
            break;
        default:
            break;
        }
    }
    return pFullRepaint->GetFullRepaintFlag();
}

int UIBookReaderContainer::GotoNextPage()
{
    DK_PROFILE_FUNCTION;
	int result = m_pBookReader->GotoNextPage();
	//DebugPrintf(DLC_DIAGNOSTIC, "GotoNextPage return value = %d", result);
    if (BOOKREADER_GOTOPAGE_SUCCESS == result)
    {
        ++m_turnPageNum;
        m_bLastPage = false;
        if (IsInSelectionProgress() && m_isSelecting)
        {
            m_clsSelected.ResetDrawMode();
            m_clsSelected.SetDrawMode(UIBookReaderSelected::DRAW_ALL_SELECTION);
        }
        Repaint();
        m_isPageTurned = true;
        ManualTTS();
        OnReadLocationChanged();
        m_fullRepaint = GetFullRepaintFlag();
		int totalPageCount = m_pBookReader->GetTotalPageNumber();
		int curPageIndex = m_pBookReader->GetCurrentPageIndex();
		if(totalPageCount > 0 && curPageIndex > 0 && curPageIndex >= totalPageCount - 3)
		{
			ReadingHistoryStat::GetInstance()->SetBookCompleted(m_localBookID.c_str(), m_strBookName.c_str(), m_eFormat, m_isTrialBook);
		}
    }
    else if(BOOKREADER_GOTOPAGE_WAIT == result)
	{
		UIUtility::SetScreenTips(StringManager::GetPCSTRById(BOOK_PARSE_ISPARSING));
	}
	else
    {
        m_bLastPage = true;
        m_bFirstPage = false;
        HandleNextPageInLast();
    }
#ifdef ENABLE_PROFILE
    DebugPrintf(DLC_DIAGNOSTIC, "------%d/%d------", m_pBookReader->GetCurrentPageIndex() + 1, m_pBookReader->GetTotalPageNumber());
    Profiler::DumpAndReset();
#endif
    return result;
}

void UIBookReaderContainer::GotoChapter(GotoChapterType _ChapterType)
{
    if(!m_pBookReader)
    {
        return;
    }

    //没有章节信息并且是第一次打开的时候提示
    const vector<DK_TOCINFO*>* pTocList = m_pBookReader->GetTOC(m_dkxBlock);
    if(!pTocList || pTocList->empty())
    {
        if(m_bIsFirstOpen)
        {
            UIMessageBox messagebox(this, StringManager::GetStringById(TOUCH_HAVE_NO_CHAPTER), MB_OK);
            messagebox.DoModal();
            m_bIsFirstOpen = false;
        }
        return;
    }

    //向前翻章时按页首的索引来判断
    //向后翻章时按页尾的索引来判断
    CT_RefPos posBegin, posEnd;
    m_pBookReader->GetCurrentPageATOM(posBegin, posEnd);
    DK_FLOWPOSITION posCurPageBegin = posBegin.ToFlowPosition();
    DK_FLOWPOSITION posCurPageEnd = posEnd.ToFlowPosition();
    
    DK_FLOWPOSITION posShouldMoveChapter;
    switch(_ChapterType)
    {
    case GotoPrevChapter:
        {
            const DK_TOCINFO* pCurChapterInfo = m_pBookReader->GetChapterInfo(posCurPageBegin, CURRENT_CHAPTER, m_dkxBlock);
            if(!pCurChapterInfo)
            {
                return;
            }
            DK_FLOWPOSITION posCurChapter = pCurChapterInfo->GetBeginPos();
            if(posCurPageBegin != posCurChapter)
            {
                posShouldMoveChapter = posCurChapter;
            }
            else
            {
                const DK_TOCINFO* pPrevChapterInfo = m_pBookReader->GetChapterInfo(posCurPageBegin, PRIVIOUS_CHAPTER, m_dkxBlock);
                if(pPrevChapterInfo)
                {
                    posShouldMoveChapter = pPrevChapterInfo->GetBeginPos();
                }
                else
                {
                    UIMessageBox messagebox(this, StringManager::GetStringById(TOUCH_HAVE_NO_PRE_CHAPTER), MB_OK);
                    messagebox.DoModal();
                    return;
                }
            }
            break;
        }
    case GotoNextChapter:
        {
            const DK_TOCINFO* pNextChapterInfo = m_pBookReader->GetChapterInfo(posCurPageEnd, NEXT_CHAPTER, m_dkxBlock);
            if(pNextChapterInfo)
            {
                posShouldMoveChapter = pNextChapterInfo->GetBeginPos();
            }
            else
            {
                int iCurrentPageIndex = m_pBookReader->GetCurrentPageIndex();
                if(iCurrentPageIndex < (int)m_pBookReader->GetTotalPageNumber() - 1)
                {
                    NavigateToPage(m_pBookReader->GetTotalPageNumber() - 1);
                    if(iCurrentPageIndex != m_pBookReader->GetCurrentPageIndex())  //txt与mobi不是准确页码，所以判断跳页前后页索引是否相等
                    {
                        Repaint();
                        return;
                    }
                }
                UIMessageBox messagebox(this, StringManager::GetStringById(TOUCH_HAVE_NO_NEXT_PAGE), MB_OK);
                messagebox.DoModal();
                return;
            }
            break;
        }
    }

    int iGotoChapterStatus = MoveToFlowPosition(posShouldMoveChapter);
    if (BOOKREADER_GOTOPAGE_SUCCESS == iGotoChapterStatus)
    {
		m_isPageTurned = true;
        Repaint();
    }
    else if (BOOKREADER_GOTOPAGE_WAIT == iGotoChapterStatus && !IsParseFinished())
    {
        ShowParseDialog(BOOKREADER_PARSE_START);
    }
    OnReadLocationChanged();
}

bool UIBookReaderContainer::HitTestAndMove(GESTURE_EVENT ge, int x, int y)
{
    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "@@@@@@: UIBookReaderContainer::%s, %d, %d", __FUNCTION__, x, y);
	if(x < 0 || y < 0 || x > m_iWidth || y > m_iHeight)
	{
		return false;
	}
	int pointX = x;
    int pointY = y;
    switch(ge)
    {
    case GESTURE_MOVE_RIGHT:
    case GESTURE_MOVE_UP:
    case GESTURE_MOVE_DOWN:
    case GESTURE_MOVE_LEFT:
        if (m_clsSelected.IsSelectionMode())
        {
            DebugPrintf(DLC_UIBOOKREADERCONTAINER, "UIBookReaderContainer::%s GESTURE_MOVE", __FUNCTION__);
            return m_clsSelected.Move(pointX, pointY, m_rightSelecter);
        }
        break;
    case GESTURE_LONG_TAP:
        {
            DebugPrintf(DLC_UIBOOKREADERCONTAINER, "UIBookReaderContainer::%s GESTURE_LONG_TAP", __FUNCTION__);
            return m_clsSelected.SetStartPos(pointX, pointY, 15, IsVerticalForms());  //SELECTEDCONTROLER_DIFFDISTANCE
        }
        break;
    default:
        break;
    }
    
    return false;
}

bool UIBookReaderContainer::OnTouchTap(GESTURE_EVENT _ge, int _x, int _y)
{
    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s : %d", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, m_isSelecting);
    if (m_pReaderSettingDlg && m_pReaderSettingDlg->IsVisible())
    {
        return true;
    }

    if (m_isSelecting)
    {
        if (IsInSelectionProgress() && (m_downInSelectionImage || PointInSelectionRects(DK_POS(_x, _y))))
        {
            DebugPrintf(DLC_UIBOOKREADERCONTAINER, "UIBookReaderContainer::OnTouchTap IsInSelectionProgress true");
        }
        else
        {
            DebugPrintf(DLC_UIBOOKREADERCONTAINER, "UIBookReaderContainer::OnTouchTap IsInSelectionProgress false");
            ClearSelection();
            Repaint();
        }
        return true;
    }

    if (m_clsSelected.IsInitialized() && !m_clsSelected.IsSelectionMode())
    {
        DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s m_clsSelected", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        if(DFF_ElectronicPublishing == m_eFormat
            && (HandleFootNote(_x, _y) || HandleInteractiveImage(_x, _y)))
        {
            return true;
        }
        if (m_commentTapLocation.size() > 0 && HandleCommentTouch(_x, _y))
        {
            return true;
        }
        DK_POS curPoint(_x, _y);
        if(HandleLinkJump(curPoint))
        {
            return true;
        }
    }

    if (IsZoomedPdf())
    {
        return true;
    }
    if (HitOnBookMarkShortCutArea(_x, _y))
    {
        return true;
    }

    int flag = SystemSettingInfo::GetInstance()->GetTurnPageFlag();
    RecognizerAction action = RecognizeTouchArea(_x, _y);

    switch(action)
    {
    case ActionPagedown:
        {
            //DebugPrintf(DLC_GESTURE, "TAP:, Goto next Page: x:%d, y:%d", _x, _y);
            GotoNextPage();
        }
        break;
    case ActionPageup:
        if (flag==0) 
        {
            //DebugPrintf(DLC_GESTURE, "TAP:, Goto Pre Page: x:%d, y:%d", _x, _y);
            GotoPrePage();
        }
        else
        {
            GotoNextPage();
        }
        break;
    case ActionMenu: 
        {
            //DebugPrintf(DLC_GESTURE, "TAP:, Menu: x:%d, y:%d", _x, _y);
            //SaveReadingInfo();
            if (!m_pReaderSettingDlg)
            {
                m_pReaderSettingDlg = new UIReaderSettingDlg(
                        this,
                        m_pBookReader,
                        m_dkxBlock,
                        m_strBookName.c_str(),
                        m_strBookFilePath.c_str(), 
                        m_eFormat);
            }
            m_pReaderSettingDlg->SetBookMarkOn(m_bExistBookMarkForCurPage);
            
            bool isTTSPlay = false;
            CTextToSpeech* pTTS = CTextToSpeech::GetInstance();
            if (pTTS && pTTS->TTS_IsPlay())
            {
                isTTSPlay = true;
            }
            
            m_pReaderSettingDlg->SetIsTTSPlay(isTTSPlay);
            m_pReaderSettingDlg->DoModal();
            switch (m_pReaderSettingDlg->GetEndFlag()) 
            {
            case UIReaderSettingDlg::SHOWTOCDLG:
                TocHandler(UITocDlg::SHOWTOC);
                break;
            case UIReaderSettingDlg::GotoPage:
                {
                    ReaderSettingChangedHandler();
                }
                break;
            case UIReaderSettingDlg::Normal:
                {
                    ReaderSettingChangedHandler();
                    OnReadLocationChanged();
                }
                break;
            case UIReaderSettingDlg::BackToBookShelf:
                {
                    StopTTS();
                    CPageNavigator::BackToPrePage();
                    return true;
                }
            case UIReaderSettingDlg::TTS:
                if ((DFF_PortableDocumentFormat != m_eFormat)
                    || (m_bIsPdfTextMode && DFF_PortableDocumentFormat == m_eFormat))
                {
                    CTextToSpeech* pTTS = CTextToSpeech::GetInstance();
                    if (NULL != pTTS && NULL != m_pBookReader)
                    {
                        if (pTTS->TTS_IsPlay())
                        {
                            StopTTS();
                        }
                        else
                        {
                            SQM::GetInstance()->IncCounter(SQM_ACTION_BOOKREADER_TTS);
                            PlayTTS();
                        }
                    }
                }
                break;
            case UIReaderSettingDlg::SHOWSEARCHDLG:
                {
                    ShowBookContentSearchDlg();
                }
                break;
            case UIReaderSettingDlg::SwitchComicsFrameMode:
                {
                    StopTTS();
                    SwitchComicsFrameMode();
                }
                break;
            case UIReaderSettingDlg::NoChange:
                {
                    //do nothing
                }
                break;
            }

        UpdateWindow();
        }
        break;
    default:
        break;
    }
    return true;
}

void UIBookReaderContainer::ShowBookContentSearchDlg()
{
    if (DFF_Text == m_eFormat || DFF_ElectronicPublishing == m_eFormat || DFF_MobiPocket == m_eFormat || DFF_PortableDocumentFormat == m_eFormat)
    {
        UIBookContentSearchDlg dlgContentSearch(this, m_strBookName, DFF_PortableDocumentFormat==m_eFormat);
        dlgContentSearch.MoveWindow(m_iLeft, m_iTop, m_iWidth, m_iHeight);
        dlgContentSearch.DoModal();
    }
}

void UIBookReaderContainer::TocHandler(int showTable)
{
    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    UITocDlg dlgTOC(
        this,
        m_iBookID,
        m_dkxBlock,
        m_pBookReader,
        m_strBookName.c_str());
    dlgTOC.MoveWindow(m_iLeft, m_iTop, m_iWidth, m_iHeight);
    if (!m_expandToc)
    {
        DK_TOCINFO::InitTocListExpandStatus(m_pBookReader->GetTOC(m_dkxBlock));
        m_expandToc = true;
    }
    dlgTOC.SetFamiliarToTraditional(SystemSettingInfo::GetInstance()->GetFontStyle_FamiliarOrTraditional());
    dlgTOC.SetShowDialog(showTable);
    dlgTOC.DoModal();
    switch(dlgTOC.GetEndFlag())
    {
    case UITocDlg::GOTOTOC:
        DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s UITocDlg::GOTOTOC", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        StopTTS();
        NavigateToToc(dlgTOC.GetCurTocInfo());
        break;
    case UITocDlg::GOTOBOOKMARK:
        DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s UITocDlg::GOTOBOOKMARK", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        StopTTS();
        NavigateToBookMark();
        break;
    default:
        break;
    }
    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return;
}

void UIBookReaderContainer::ProcessComment(DWORD cmdID, const std::string& comment)
{
    switch(cmdID)
    {
    case SAVE_COMMENT:
        if (!comment.empty())
        {
            ICT_ReadingDataItem *pclsBookMark = DKXReadingDataItemFactory::CreateReadingDataItem(
                ICT_ReadingDataItem::COMMENT, 
                m_clsStartSelected, m_clsEndSelected, 
                m_strSelected, comment.c_str());
            UpdateBookMark(m_iCurCommentIndex, pclsBookMark);
            DKXReadingDataItemFactory::DestoryReadingDataItem(pclsBookMark);
            Repaint();
        }
        break;
    case DELETE_COMMENT:
        {
            RemoveBookMark(m_iCurCommentIndex);
            Repaint();
        }
        break;
    case GOTO_COMMENTORDIGEST:
        {
            TocHandler(UITocDlg::SHOWBOOKCOMMENTANDDIGEST);
        }
        break;
    default:
        break;
    }
}

void UIBookReaderContainer::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    UICommentOrSummary* pCommentOrSummary = UICommentOrSummary::GetInstance();
    if (pCommentOrSummary && pCommentOrSummary->IsVisible())
    {
        pCommentOrSummary->SetVisible(false);
        HideSelectionImage(false);
    }
    switch(dwCmdId)
    {
    case ID_BTN_TOUCH_ADD_COMMENT:
        {
            DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s ID_BTN_TOUCH_ADD_COMMENT start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            UICommentEditDialog commentDlg(this);

            if (IDCANCEL != commentDlg.DoModal())
            {
                ProcessComment(commentDlg.GetCmdID(), commentDlg.GetComment());
            }
            DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s ID_BTN_TOUCH_ADD_COMMENT end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        }
        break;
    case ID_BTN_TOUCH_EDIT_COMMENT:
        {
            DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s ID_BTN_TOUCH_EDIT_COMMENT start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            ICT_ReadingDataItem* pclsReadingProgress = DKXReadingDataItemFactory::CreateReadingDataItem();
            if(m_dkxBlock && pclsReadingProgress && !m_strBookFilePath.empty())
            {
                DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s pComment", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
                m_dkxBlock->GetBookmarkByIndex(m_iCurCommentIndex, pclsReadingProgress);
                UICommentEditDialog commentDlg(this, m_iCurCommentIndex + 1, pclsReadingProgress->GetUserContent());
                if (IDCANCEL != commentDlg.DoModal())
                {
                    ProcessComment(commentDlg.GetCmdID(), commentDlg.GetComment());
                }
            }
            
            DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s ID_BTN_TOUCH_EDIT_COMMENT end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        }
        break;
    case ID_BTN_TOUCH_DELETE_COMMENT:
        {
            DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s ID_BTN_TOUCH_DELETE_COMMENT start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            RemoveBookMark(m_iCurCommentIndex);
            Repaint();
            DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s ID_BTN_TOUCH_DELETE_COMMENT end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        }
        break;
    case ID_BTN_TOUCH_ADD_DIGEST:
        {
            DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s ID_BTN_TOUCH_ADD_DIGEST start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            AddDigest();
            Repaint();
            DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s ID_BTN_TOUCH_ADD_DIGEST end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        }
        break;
    case ID_BTN_TOUCH_DELETE_DIGEST:
        {
            DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s ID_BTN_TOUCH_DELETE_DIGEST start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            RemoveBookMark(m_iCurDigestIndex);
            Repaint();
            DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s ID_BTN_TOUCH_DELETE_DIGEST end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        }
        break;
    case ID_BTN_TOUCH_MORE_DICTIONARY:
        {
            std::string str_tmp = "";
            UICommentOrSummary* pCommentOrSummary = UICommentOrSummary::GetInstance();
            str_tmp = pCommentOrSummary->GetWords();
            UIDictionaryDialog dictDlg(this, str_tmp.c_str());
            dictDlg.MoveWindow(m_iLeft, m_iTop , m_iWidth, m_iHeight);
            dictDlg.DoModal();
        }
        break;
    case ID_BTN_BOOK_CORRECTION:
        {
            std::string str_tmp = "";
            UIBookCorrectionDlg bookCorrection(this, m_pBookReader);
            bookCorrection.SetSelectedText(m_strSelected.c_str(), &m_clsStartSelected, &m_clsEndSelected);
            bookCorrection.MoveWindow(m_iLeft, m_iTop , m_iWidth, m_iHeight);
            bookCorrection.DoModal();
        }
        break;
    case ID_BTN_SINAWEIBO_SHARE:
        {
            std::string sharedContent = UIWeiboSharePage::PruneStringForDKComment(m_strBookName, m_strSelected);
            UIWeiboSharePage* pPage = new UIWeiboSharePage(sharedContent.c_str());
            if (pPage)
            {
                CPageNavigator::Goto(pPage);
            }
        }
        break;
    case ID_BTN_WEB_SEARCH:
        {
            showWebSearchMenu();
        }
        break;
    case ID_BTN_WEB_SEARCH_BAIDU:
        {
            searchOnWeb(getBaiduSearchUrl(m_strSelected));
        }
        break;
    case ID_BTN_WEB_SEARCH_GOOGLE:
        {
            searchOnWeb(getGoogleSearchUrl(m_strSelected));
        }
        break;
    case ID_BTN_WEB_SEARCH_CIBA:
        {
            searchOnWeb(getCibaTranslateUrl(m_strSelected));
        }
        break;
    case ID_BTN_WEB_SEARCH_WIKI:
        {
            searchOnWeb(getWikiUrl(m_strSelected));
        }
        break;
    case ID_BTN_TOUCH_COMMENT_LINK:
        {
            HandleLinkJump(m_posJumpLink);
        }
        break;
    default :
        {
            DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s default", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            Repaint();
        }
        break;
    }
    ClearSelection();
    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

void UIBookReaderContainer::showWebSearchMenu()
{
    std::vector<int> btnIDs;
    btnIDs.push_back(ID_BTN_WEB_SEARCH_BAIDU);
    btnIDs.push_back(ID_BTN_WEB_SEARCH_GOOGLE);
    btnIDs.push_back(ID_BTN_WEB_SEARCH_CIBA);
    btnIDs.push_back(ID_BTN_WEB_SEARCH_WIKI);
    btnIDs.push_back(ID_INVALID);

    std::vector<int> strIDs;
    strIDs.push_back(WEB_SEARCH_BAIDU);
    strIDs.push_back(WEB_SEARCH_GOOGLE);
    strIDs.push_back(WEB_SEARCH_CIBA);
    strIDs.push_back(WEB_SEARCH_WIKI);
    strIDs.push_back(-1);
#ifdef KINDLE_FOR_TOUCH
    UIBookMenuDlg dlgMenu(this, btnIDs, strIDs); 
    dlgMenu.MakeCenter(dlgMenu.GetWidth(), dlgMenu.GetHeight());
#else
    vector<char> customizedShortKeys;
    customizedShortKeys.push_back('B');
    customizedShortKeys.push_back('G');
    customizedShortKeys.push_back('C');
    customizedShortKeys.push_back('W');
    UIBookMenuDlg dlgMenu(this, "",  btnIDs, strIDs, customizedShortKeys);
#endif
    if (dlgMenu.DoModal() == IDOK)
    {
        OnCommand(dlgMenu.GetCommandId(), NULL, 0);
    }
}

void UIBookReaderContainer::AddDigest()
{
    SQM::GetInstance()->IncCounter(SQM_ACTION_DKREADERPAGE_BOOKNOTE_ADD);
    string strUserComment = "";
    ICT_ReadingDataItem *pclsBookMark = DKXReadingDataItemFactory::CreateReadingDataItem(
            ICT_ReadingDataItem::DIGEST, 
            m_clsStartSelected, m_clsEndSelected, 
            m_strSelected, strUserComment);
    UpdateBookMark(m_iCurDigestIndex, pclsBookMark);
    DKXReadingDataItemFactory::DestoryReadingDataItem(pclsBookMark);
}

bool UIBookReaderContainer::OnScale(ScaleGestureDetector* detector)
{
    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (WAIT_FOR_SCALE == m_scaleFlag)
    {
        DebugPrintf(DLC_UIBOOKREADERCONTAINER, "return false: m_modeController.m_iRotation = %d", m_modeController.m_iRotation);
        return false;
    }

    if (NULL == detector)
    {
        DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s NULL == detector", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return false;
    }

    const int curSpan = detector->GetCurrentSpan();
    const DK_DOUBLE zoomSpan = ((DK_DOUBLE)(curSpan - m_curSpan)) / 100.0;
    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "curSpan = %d, m_curSpan = %d, zoomSpan = %f", curSpan,  m_curSpan, zoomSpan);
    if (abs(zoomSpan) < MOVE_MINZOOMSPAN || (m_modeController.m_dUserZoom == PDF_ZOOM_MAXVALUE && zoomSpan > 0))
    {
        DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s diffSpan < MOVE_MINDISTANCE", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return false;
    }

    if (IsScannedPdf())
    {
        DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s IsScannedPdf()", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        OnTouchZoomPdf(zoomSpan, detector->GetFocusX(), detector->GetFocusY());
    }
    m_scaleFlag = SCALING;
    m_curSpan = curSpan;
    return true;
}

bool UIBookReaderContainer::OnScaleBegin(ScaleGestureDetector* detector)
{
    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (!IsSupportedZoomedPara())
    {
        return false;
    }
    if (NULL == detector || !IsScannedPdf())
    {
        return false;
    }
    m_scaleFlag = START_SCALE;
    m_curSpan = detector->GetCurrentSpan();
	InitZoomedPdfPara();
    DebugPrintf(DLC_UIBOOKREADERCONTAINER,"m_curSpan: %d", m_curSpan);
    return true;
}

bool UIBookReaderContainer::OnScaleEnd(ScaleGestureDetector* detector)
{
    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (WAIT_FOR_SCALE == m_scaleFlag)
    {
        DebugPrintf(DLC_UIBOOKREADERCONTAINER, "return false: m_modeController.m_iRotation = %d", m_modeController.m_iRotation);
        return false;
    }
    m_scaleFlag = STOP_SCALE;
    return FinishZoomPdf();
}

bool UIBookReaderContainer::OnTouchZoomPdf(DK_DOUBLE zoomSpan, int focusX, int focusY)
{
    DK_DOUBLE& userZoom = m_modeController.m_dUserZoom;
    DK_DOUBLE& zoomX0 = m_modeController.m_zoomX0;
    DK_DOUBLE& zoomY0 = m_modeController.m_zoomY0;
    const DK_DOUBLE preUserZoom = userZoom;
    userZoom += zoomSpan;
    if (userZoom <= 1)
    {
        m_modeController.m_eReadingMode = PDF_RM_NormalPage;
        userZoom = 1.0;
        zoomX0 = zoomY0 = 0;
        return true;
    }
    else if (userZoom > PDF_ZOOM_MAXVALUE)
    {
        userZoom = PDF_ZOOM_MAXVALUE;
    }

    if (userZoom == preUserZoom)
    {
        return true;
    }
    
    zoomX0 = focusX * (userZoom - 1);
    zoomY0 = focusY * (userZoom - 1);
    if (!IsZoomedPdf())
    {
        m_modeController.m_eReadingMode = PDF_RM_ZoomPage;
        LoadSystemPageSizeInfo(m_pageWidth, m_pageHeight);
        m_pBookReader->SetPageSize(m_iHorizontalMargin, m_iTopMargin, m_pageWidth, m_pageHeight);
    }

    UpdateZoomedPdfProgressBar();
    ZoomBMP();

    return true;
}

bool UIBookReaderContainer::OnTouchMoveZoomPdf(int distanceX, int distanceY)
{
    DebugPrintf(DLC_GESTURE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (m_pBookReader && ((abs(distanceX) > PDF_ZOOM_MOVE_MINVALUE) || (abs(distanceY) > PDF_ZOOM_MOVE_MINVALUE)))
    {
        DK_DOUBLE& zoomX0 = m_modeController.m_zoomX0;
        DK_DOUBLE& zoomY0 = m_modeController.m_zoomY0;
        DebugPrintf(DLC_GESTURE, "zoomX0 = %f, zoomY0 = %f", zoomX0,  zoomY0);
        DebugPrintf(DLC_GESTURE, "distanceX = %d,   distanceY = %d", distanceX,  distanceY);
        zoomX0 += distanceX;
        zoomY0 += distanceY;
        zoomX0 = dk_max(0.0, zoomX0);
        zoomY0 = dk_max(0.0, zoomY0);

        const DK_DOUBLE maxZoomX0 = (m_pageWidth * (m_modeController.m_dUserZoom - 1));
        const DK_DOUBLE maxZoomY0 = (m_pageHeight * (m_modeController.m_dUserZoom - 1));
        zoomX0 = dk_min(maxZoomX0, zoomX0);
        zoomY0 = dk_min(maxZoomY0, zoomY0);

        DebugPrintf(DLC_GESTURE, "UpdateZoomedPdfProgressBar begin");
        m_pdfZoomMoved = true;
        UpdateZoomedPdfProgressBar();
        DebugPrintf(DLC_GESTURE, "ZoomBMP begin");
        ZoomBMP();
    }
    DebugPrintf(DLC_GESTURE, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return true;
}

void UIBookReaderContainer::ZoomBMP()
{
    CDisplay* pDisplay = CDisplay::GetDisplay();
    if (NULL == pDisplay || (NULL == m_pImgStream) || (NULL == m_pBookReader))
    {
        return;
    }

    ImageHandler* pImgHandler = DkImageHelper::CreateImageHandlerInstance(m_pImgStream);
    if (pImgHandler)
    {
        const int imgWidth = pImgHandler->GetWidth();
        const int imgHeight = pImgHandler->GetHeight();

        const DK_DOUBLE& userZoom = m_modeController.m_dUserZoom;
        const double& zoomX0 = m_modeController.m_zoomX0;
        const double& zoomY0 = m_modeController.m_zoomY0;
        double zoomX1 = zoomX0 + m_pageWidth;
        double zoomY1 = zoomY0 + m_pageHeight;


        DK_RECT srcRect = {
            dk_max(0, (int)(zoomX0 / userZoom - 1)), dk_max(0, (int)(zoomY0/ userZoom - 1)),
            dk_min((int)(zoomX1 / userZoom + 1), imgWidth), dk_min((int)(zoomY1 / userZoom + 1), imgHeight)
        };
        int iNewWidth = (int)((srcRect.right - srcRect.left) * userZoom);
        int iNewHeight = (int)((srcRect.bottom - srcRect.top) * userZoom);
        if(pImgHandler->CropAndResize(srcRect, iNewWidth, iNewHeight))
        {
            DK_RECT rcSelf = {0, 0,  m_pageWidth, m_pageHeight};

            DK_IMAGE srcImg, DestImg;
            DK_IMAGE memDC = pDisplay->GetMemDC();
            ResetImage(&srcImg, 1, iNewWidth, iNewHeight);
            if((pImgHandler->GetImage(&srcImg)) && SUCCEEDED(CropImage(srcImg, rcSelf, &DestImg)))
            {
                CDisplay::CacheDisabler cache;
                CTpImage img(&DestImg);
                CTpGraphics prg(memDC); 
                prg.DrawImage(&img, 0, 0);
                pDisplay->ForceDraw(PAINT_FLAG_FULL);
            }
            SafeFreePointer(srcImg.pbData);
        }
        SafeDeletePointer(pImgHandler);
    }
}

void UIBookReaderContainer::UpdateZoomedPdfProgressBar()
{
    const DK_DOUBLE& userZoom = m_modeController.m_dUserZoom;
    const DK_DOUBLE& zoomX0 = m_modeController.m_zoomX0;
    const DK_DOUBLE& zoomY0 = m_modeController.m_zoomY0;

    const int x  = (int)zoomX0;
    const int maxHorizontal = (int)(m_pageWidth * userZoom);
    const int startPosHorizontal = (x < 0) ? 0 : ((x > (maxHorizontal - m_pageWidth)) ? (maxHorizontal - m_pageWidth) : x);
    const int endPosHorizontal = startPosHorizontal + m_pageWidth;
    m_zoomedPdfHorizontalPos.SetMaximum(maxHorizontal);
    m_zoomedPdfHorizontalPos.SetProgress(endPosHorizontal);
    m_zoomedPdfHorizontalPos.SetStartPos(startPosHorizontal);
    m_zoomedPdfHorizontalPos.UpdateWindow();

    const int y = (int)zoomY0;
    const int maxVertical = (int)(m_pageHeight * userZoom);
    const int startPosVertical = (y < 0) ? 0 : ((y > (maxVertical - m_pageHeight)) ? (maxVertical - m_pageHeight) : y);
    const int endPosVertical = startPosVertical + m_pageHeight;
    m_zoomedPdfVerticalPos.SetMaximum(maxVertical);
    m_zoomedPdfVerticalPos.SetProgress(endPosVertical);
    m_zoomedPdfVerticalPos.SetStartPos(startPosVertical);
    m_zoomedPdfVerticalPos.UpdateWindow();
}

int UIBookReaderContainer::GetCommentDigestIndex(const CT_RefPos& beginPos, const CT_RefPos& endPos, ICT_ReadingDataItem::UserDataType dataType, bool& isContained)
{
	int dataIndex = -1;
    if(m_strBookFilePath.empty())
    {
        return dataIndex;
    }

    if(!m_dkxBlock)
    {
        return dataIndex;
    }

    int _iCount = m_dkxBlock->GetBookmarkCount();
    for( int i = 0; i < _iCount; i++)
    {
        ICT_ReadingDataItem *pclsReadingProgress = DKXReadingDataItemFactory::CreateReadingDataItem();
        if(!pclsReadingProgress)
        {
            break;
        }

        m_dkxBlock->GetBookmarkByIndex(i, pclsReadingProgress);
        if (dataType == pclsReadingProgress->GetUserDataType())
        {
            CT_RefPos clsRecordBegin = pclsReadingProgress->GetBeginPos();
            CT_RefPos clsRecordEnd   = pclsReadingProgress->GetEndPos();
			if(clsRecordBegin.CompareTo(beginPos) <=0 && clsRecordEnd.CompareTo(endPos) >= 0)
			{
				isContained = true;
				dataIndex = i;
				DKXReadingDataItemFactory::DestoryReadingDataItem(pclsReadingProgress);
                return dataIndex;
			}
			else if(clsRecordBegin.CompareTo(endPos) >= 0 || clsRecordEnd.CompareTo(beginPos) <= 0)
			{
			}
			else
			{
				dataIndex = i;
				DKXReadingDataItemFactory::DestoryReadingDataItem(pclsReadingProgress);
				return dataIndex;
			}
        }
        DKXReadingDataItemFactory::DestoryReadingDataItem(pclsReadingProgress);
    }
    return dataIndex;
}

void UIBookReaderContainer::PDFFormatSettingChangedHandler()
{
    m_modeController = m_pReaderSettingDlg->GetPdfModeController();

    //处理智能重排&关闭智能重排
    if (m_pReaderSettingDlg->RearRangeStatusChanged()) 
    {
        SaveReadingInfo();
        m_pBookReader->SetPdfModeController (&m_modeController);
        bool ret = InitBookReader();
        m_bIsPdfTextMode = (m_modeController.m_eReadingMode==PDF_RM_Rearrange);
        if (m_bIsPdfTextMode && !ret)
        {
            // PDF 进入重排模式失败
            UIMessageBox messagebox(this, StringManager::GetStringById(PDF_REARRANGE_FAILED), MB_TIPMSG);
            messagebox.DoModal();

            m_bIsPdfTextMode = false;
            m_modeController = *(m_pBookReader->GetPdfModeController());
            m_pBookReader->SetPdfModeController (&m_modeController);
            m_pReaderSettingDlg->SetPdfModeController(&m_modeController);
            SaveReadingInfo();
            InitBookReader();
        }
    } 
    else if (!m_bIsPdfTextMode)
    {
        //仍在流式下，重新设置PDFModeController即可
        m_pBookReader->SetPdfModeController (&m_modeController);
		m_isPageTurned = true;
    }
    else 
    {
        //仍在智能重排模式下，设置修改同其他类文档
        OtherFormatsSettingChangedHandler();
    }
}

void UIBookReaderContainer::OtherFormatsSettingChangedHandler()
{
    ICT_ReadingDataItem* pReadingProcess = m_pBookReader->GetCurPageProgress();            
    if (pReadingProcess)
    {
        GotoBookmark(pReadingProcess);
        SafeDeletePointer(pReadingProcess);
    }
}

void UIBookReaderContainer::UpdateProgressBarMode()
{
    if (IsScannedPdf())
    {
        m_clsReaderProgressBar.SetProgressBarMode(1);
    }
    else
    {
        m_clsReaderProgressBar.SetProgressBarMode(SystemSettingInfo::GetInstance ()->GetProgressBarMode ());
        m_clsReaderProgressBar.SetDirection(IsVerticalRTL() ? UIBookReaderProgressBar::PBD_RTL_Horizontal : UIBookReaderProgressBar::PBD_LTR_Horizontal);
    }
}

void UIBookReaderContainer::ReaderSettingChangedHandler()
{
    if (!m_pReaderSettingDlg)
    {
        return;
    }

    UpdateProgressBarMode();
    // 加载系统设置数据
    LoadSystemPageSizeInfo(m_pageWidth, m_pageHeight);

    if (m_pBookReader)
    {
        DK_FONT_SMOOTH_TYPE fontSmoothType = DK_FONT_SMOOTH_NORMAL;
        if(SystemSettingInfo::GetInstance()->GetFontRender() == 0)
        {
            fontSmoothType = DK_FONT_SMOOTH_SHARP;
        }
        m_pBookReader->SetFontSmoothType(fontSmoothType);
    }

    //对于非pdf文档，设置页面即时开始更新设置
    if (DFF_PortableDocumentFormat != m_eFormat)
    {
        return;
    }

    m_bIsPdfTextMode = (DFF_PortableDocumentFormat == m_eFormat
        && PDF_RM_Rearrange == m_modeController.m_eReadingMode);

    if (!m_pBookReader->SetPageSize(m_iHorizontalMargin, m_iTopMargin, m_pageWidth, m_pageHeight))
    {
        return ;
    }

    LoadSystemDarkenLevel();
    bool bFontnameChanged = false;
    LoadSystemFontnameChange(bFontnameChanged);
    m_pBookReader->SetFontChange(bFontnameChanged);

    DK_LAYOUTSETTING    clslayout;
    LoadSystemLayoutInfo(&clslayout);
    if (!m_pBookReader->SetLayoutSettings(clslayout))
    {
        return;
    }

    m_pBookReader->SetFamiliarToTraditional(SystemSettingInfo::GetInstance()->GetFontStyle_FamiliarOrTraditional());
    
    //if (DFF_PortableDocumentFormat==m_eFormat) 
    //{
        PDFFormatSettingChangedHandler();
    //}
    //else 
    //{
        //OtherFormatsSettingChangedHandler();
    //}
    Repaint ();
}

UIBookReaderContainer::RecognizerAction UIBookReaderContainer::RecognizeTouchArea(int _x, int _y)
{
    RecognizerAction action = ActionInvalid;
    //record the responsing positon to recognize
    //transform into the situation when m_iRotation is zero
    DK_INT curRotation = 0;
    double heightPercent = 0.2;
    double widthPercent  = 0.2;

    if (IsScannedPdf())
    {
        /*const PdfModeController* pdfModeCtrl = m_pBookReader->GetPdfModeController();
        curRotation = pdfModeCtrl->m_iRotation;*/
        curRotation = m_modeController.m_iRotation;
    }
    else
    {
        curRotation = 0;
    }

    if (curRotation==0)
    {
        if (_y>=0 && _y<=(m_iHeight*heightPercent + 50))
        {
            action = ActionMenu;
        }
        else if (IsVerticalRTL())
        {
            if (_x>=0 && _x<=m_iWidth*(1.0 - widthPercent))
            {
                action = ActionPagedown;
            } 
            else
            {
                action = ActionPageup;
            }
        }
        else
        {
            if (_x>=0 && _x<=m_iWidth*widthPercent)
            {
                action = ActionPageup;
            } 
            else
            {
                action = ActionPagedown;
            }
        }
    }
    else if (curRotation==180) 
    {
        if (_y>=(m_iHeight*(1.0 - heightPercent) - 50) && _y<=m_iHeight)
        {
            action = ActionMenu;
        } 
        else if (_x>=m_iWidth*(1.0 - widthPercent) && _x<=m_iWidth)
        {
            action = ActionPageup;
        } 
        else
        {
            action = ActionPagedown;
        }
    }
    else if (curRotation==90)
    {
        if ((_y>=m_iHeight*(1.0-heightPercent) && _y<=m_iHeight)
            ||  (((_x>=0 && _x<=m_iWidth*widthPercent)) && (_y>=0&&_y<=m_iHeight*(1.0-heightPercent))))
        {
                action = ActionPageup;
        } 
        else if(_x>widthPercent*m_iWidth && _x<=(1.0 - widthPercent)*m_iWidth
            &&  _y>heightPercent*m_iHeight&& _y<=(1.0 - heightPercent)*m_iHeight)
        {
                action = ActionMenu;
        } 
        else
        {
            action = ActionPagedown;
        }
    }
    else if (curRotation==270)
    {
        if ((_y>=m_iHeight*(1.0-heightPercent) && _y<=m_iHeight)
            ||  (((_x>=0 && _x<=m_iWidth*widthPercent)) && (_y>=0&&_y<=m_iHeight*(1.0-heightPercent))))
        {
                action = ActionPagedown;
        }
        else if 
        (_x>widthPercent*m_iWidth && _x<=(1.0 - widthPercent)*m_iWidth
            &&  _y>heightPercent*m_iHeight&& _y<=(1.0 - heightPercent)*m_iHeight)
        {
                action = ActionMenu;
        } 
        else
        {
            action = ActionPageup;
        }
    }
    
    if (action==ActionInvalid)
    {
        DebugPrintf(DLC_ZHY, "error! %s::%s, %s(%d)", GetClassName(), __FUNCTION__, __FILE__, __LINE__);
    }
    return action;
}

GESTURE_EVENT UIBookReaderContainer::TransformGestureDirections(GESTURE_EVENT ge)
{
    if (IsZoomedPdf())
    {
        return ge;
    }
    DK_INT curRotation = 0;

    if (IsScannedPdf()) 
    {
        /*const PdfModeController* pdfModeCtrl = m_pBookReader->GetPdfModeController();
        curRotation = pdfModeCtrl->m_iRotation;*/
        curRotation = m_modeController.m_iRotation;
        DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s curRotation = %d", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, curRotation);
    }
    else
    {
        curRotation = 0;
    }

    // int TransformedGesture;
    //int RelativeStartIndex = ge;
    //int R0GestureDirections[]   = {GESTURE_MOVE_UP, GESTURE_MOVE_DOWN, GESTURE_MOVE_LEFT, GESTURE_MOVE_RIGHT};
    GESTURE_EVENT iGesture = ge;
    GESTURE_EVENT R90GestureDirections[]  = {GESTURE_NONE, GESTURE_START, GESTURE_LONG_TAP, GESTURE_MOVE_RIGHT, GESTURE_MOVE_LEFT, GESTURE_MOVE_UP, GESTURE_MOVE_DOWN, GESTURE_END};
    GESTURE_EVENT R180GestureDirections[] = {GESTURE_NONE, GESTURE_START, GESTURE_LONG_TAP, GESTURE_MOVE_DOWN, GESTURE_MOVE_UP, GESTURE_MOVE_RIGHT, GESTURE_MOVE_LEFT,GESTURE_END};
    GESTURE_EVENT R270GestureDirections[] = {GESTURE_NONE, GESTURE_START, GESTURE_LONG_TAP, GESTURE_MOVE_LEFT, GESTURE_MOVE_RIGHT, GESTURE_MOVE_DOWN, GESTURE_MOVE_UP,GESTURE_END};
    switch (curRotation)
    {
    case 0:
        iGesture = ge;
        break;
    case 90:
        iGesture = R90GestureDirections[ge];
        break;
    case 180:
        iGesture = R180GestureDirections[ge];
        break;
    case 270:
        iGesture = R270GestureDirections[ge];
        break;
    default:
        DebugPrintf(DLC_ERROR, "rotation error! %d", ge);
    }

    if(ShouldTreatUpDownAsPageSwitch())
    {
        if(iGesture == GESTURE_MOVE_UP)
        {
            iGesture = GESTURE_MOVE_LEFT;
        }
        else if(iGesture == GESTURE_MOVE_DOWN)
        {
            iGesture = GESTURE_MOVE_RIGHT;
        }
    }
    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return iGesture;
}

void UIBookReaderContainer::SwitchBookMarkOffOn()
{
    if(NULL == m_dkxBlock)
    {
        return;
    }
    m_readingDataChanged = true;
    if (m_bExistBookMarkForCurPage)
    {

        m_dkxBlock->SetReadingDataModified(true);
        m_dkxBlock->RemoveBookmarkByIndex(m_uIndexOfBookMarkForCurPage);
    }
    else
    {
        time_t _tTime;
        time(&_tTime);
        string _strTime  = CUtility::TransferTimeToString(_tTime);

        ATOM posCurPageAtom, endAtom;
        m_pBookReader->GetCurrentPageATOM(posCurPageAtom, endAtom);
        CT_ReadingDataItemImpl clsBookMark(ICT_ReadingDataItem::BOOKMARK);
        string text = GetCurPageTextPreview(BOOKMARK_PREVIEW_MAX_LENGTH);
        clsBookMark.SetCreateTime(_strTime);
        clsBookMark.SetLastModifyTime(_strTime);
        clsBookMark.SetCreator("DuoKan");
        clsBookMark.SetBookContent(text);
        clsBookMark.SetBeginPos(posCurPageAtom);
        clsBookMark.SetEndPos(endAtom);
        SaveBookMark(&clsBookMark);
    }
    UpdateWindow();

    SQM::GetInstance()->IncCounter(SQM_ACTION_DKREADERPAGE_BOOKMARK_ADD);
}

DK_FLOWPOSITION UIBookReaderContainer::GetJumpHistory()
{
	return m_jumpHistory;
}

JumpDirection UIBookReaderContainer::GetJumpDirection()
{
	return m_jumpDirection;
}

int UIBookReaderContainer::MoveToFlowPosition(const DK_FLOWPOSITION &pos, bool isRollBack)
{
	StopTTS();
	ATOM beginPos, endPos;
	if(m_pBookReader->GetCurrentPageATOM(beginPos, endPos))
	{
		if(isRollBack)
		{
			m_jumpDirection = (m_jumpDirection == GO_LEFT ? GO_RIGHT : GO_LEFT);
		}
    	else
		{
			m_jumpDirection = GO_LEFT;
		}
		m_jumpHistory = beginPos.ToFlowPosition();
	}
	return m_pBookReader->MoveToFlowPosition(pos);
}

void UIBookReaderContainer::NavigateToPage(int iPageNo)
{
	StopTTS();
	if(m_pBookReader)
	{
		if (iPageNo<0)
        iPageNo = 0;
	
		ATOM beginPos, endPos;
		if(m_pBookReader->GetCurrentPageATOM(beginPos, endPos))
		{
			m_jumpDirection = GO_LEFT;
			m_jumpHistory = beginPos.ToFlowPosition();
		}
	    if (!MoveToPage(iPageNo) && !IsParseFinished())
	    {
	        ShowParseDialog(m_pBookReader->GetParseStatus());
	    }
	    SQM::GetInstance()->IncCounter(SQM_ACTION_DKREADERPAGE_PAGE_JUMP);
	}
}

void UIBookReaderContainer::NavigateToToc(DK_TOCINFO* pTocInfo)
{
    StopTTS();
    if(!pTocInfo)
    {
        return;
    }
    if ((BOOKREADER_GOTOPAGE_SUCCESS != m_pBookReader->MoveToTOC(pTocInfo)) && !IsParseFinished())
    {
        ShowParseDialog(m_pBookReader->GetParseStatus());
    }
    OnReadLocationChanged();
}

void UIBookReaderContainer::NavigateToBookMark()
{
    if (NULL == m_pBookReader)
    {
        return;
    }
    if (DFF_PortableDocumentFormat == m_eFormat)
    {
        m_dkxBlock->SetPDFModeController(m_modeController);
    }
    InitBookReader();
}

std::string UIBookReaderContainer::GetCurPageTextPreview(unsigned int maxLength)
{
    if (!m_pBookReader || 0 == maxLength)
    {
        return string("");
    }
    
    string textPreview = m_pBookReader->GetCurrentPageText();
    if (textPreview.empty())
    {
        // 如果当前页不包含文字，且分页已完成，则返回"x / y 页"表示书签位置，否则返回不包含内容的字符串。
        long lPageIndex = m_pBookReader->GetCurrentPageIndex();
        long lPageCount = m_pBookReader->GetTotalPageNumber();
        if (lPageCount > 0 && lPageIndex >= 0 && lPageCount >= lPageIndex)
        {
            char text[256] = {0};
            snprintf (text, sizeof(text), StringManager::GetPCSTRById(CURRENTPAGE_IS_PICTURE), lPageIndex + 1, lPageCount);
            return string(text);
        }
        else
        {
            return string("");
        }
    }
    else if (textPreview.length() <= maxLength)
    {
        // 小于要求的最大长度，直接返回该页全部文本。
        return textPreview;
    }

    // 该页文本长度大于要求的最大长度 maxLength，计算在 maxLength 内，包含合法UTF-8字符串的最大长度。
    unsigned int subLen(maxLength);
    unsigned int index(maxLength - 1);
    unsigned int charSize(0);
    while (index)
    {
        charSize = EncodingUtil::GetCharSizeForUtf8LeadingByte(textPreview[index]);
        if (0 != charSize && (subLen = index + charSize) <= maxLength)
        {
            break;
        }
        --index;
    }
    return textPreview.substr(0, subLen);
}

void UIBookReaderContainer::DoDkxUpdate()
{
    if(!m_dkxBlock)
    {
        return;
    }
    
    std::string dkxBookRevision = m_dkxBlock->GetBookRevision();
    std::string localFileRevision = m_pBookReader->GetBookRevision();
    if (dkxBookRevision.empty() && !localFileRevision.empty())
    {
        m_dkxBlock->SetBookRevision(localFileRevision.c_str());
    }
    
    vector<ICT_ReadingDataItem *> vOldBookmarks;
     // 不判断返回值，因为可能没有书签但有进度
    m_dkxBlock->GetAllBookmarks(&vOldBookmarks);
   
    ICT_ReadingDataItem *pclsReadingProgress = DKXReadingDataItemFactory::CreateReadingDataItem();
    if (pclsReadingProgress && m_dkxBlock->GetProgress(pclsReadingProgress))
    {
        // 自动生成的初始dkx不做转换
        CT_RefPos end = pclsReadingProgress->GetEndPos();
        if (0 != end.GetChapterIndex() 
            || 0 != end.GetParaIndex() 
            || 0 != end.GetAtomIndex())
        {
            vOldBookmarks.push_back(pclsReadingProgress);
        }
        else
        {
            DKXReadingDataItemFactory::DestoryReadingDataItem(pclsReadingProgress);
            pclsReadingProgress = NULL;
        }
    }

    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "UIBookReaderContainer::DoDkxUpdate() vOldBookmarks.size() %d", vOldBookmarks.size());
    bool needUpdate = false;
    int iCurFirstVersionNum = 0, iCurSecondVersionNum = 0;
    sscanf(DK_GetKernelVersion(), "%d.%d.", &iCurFirstVersionNum, &iCurSecondVersionNum);
    for (unsigned int i = 0; i < vOldBookmarks.size(); ++i)
    {
        int iDkxFirstVersionNum = 0, iDkxSecondVersionNum = 0;
        sscanf(vOldBookmarks[i]->GetKernelVersion().c_str(), "%d.%d.", &iDkxFirstVersionNum, &iDkxSecondVersionNum);
         //if (iDkxFirstVersionNum < iCurFirstVersionNum || iDkxSecondVersionNum < iCurSecondVersionNum)
        // 内核版本太低需要升级
        if (iDkxFirstVersionNum < 1 || (1 == iDkxFirstVersionNum && 0 == iDkxSecondVersionNum))
        {
            needUpdate = true;
            break;
        }
    }
    if (DFF_ElectronicPublishing == m_eFormat)
    {
        if (vOldBookmarks.size() > 0)
        {
            for (unsigned int i = 0; i < vOldBookmarks.size(); ++i)
            {
                // 如果没有offsetInChapter或者版本号不对，则需要更新
                // 没有offsetInChapter则直接转换，算出offsetInChapter
                // 如果有offsetInChapter但版本号不对则根据offsetInChapter重新计算flow position
                if (!vOldBookmarks[i]->GetBeginPos().HasOffsetInChapter()
                        || !vOldBookmarks[i]->GetBeginPos().HasChapterId()
                        || strcmp(DK_GetKernelVersion(), vOldBookmarks[i]->GetKernelVersion().c_str()) != 0)
                {
                    needUpdate = true;
                    break;
                }
            }
            if (!needUpdate)
            {
                // 检查书籍是否升级
                if (!dkxBookRevision.empty() && !localFileRevision.empty() && dkxBookRevision!= localFileRevision)
                {
                    needUpdate = true;
                }
            }
        }
    }

    if (needUpdate)
    {
        vector<ICT_ReadingDataItem *> vNewBookmarks;
        if (m_pBookReader->UpdateReadingData(dkxBookRevision.c_str(), &vOldBookmarks, localFileRevision.c_str(), &vNewBookmarks))
        {
            int iBookMarkNumber = m_dkxBlock->GetBookmarkCount();
            for (int i = 0; i < iBookMarkNumber; ++i)
            {
                m_dkxBlock->RemoveBookmarkByIndex(0);
            }

            for (unsigned int i = 0; i < vNewBookmarks.size(); ++i)
            {
                if (ICT_ReadingDataItem::PROGRESS == vNewBookmarks[i]->GetUserDataType())
                {
                    m_dkxBlock->SetProgress(vNewBookmarks[i]);
                }
                else
                {
                    // add chapter title
                    string chapterTitle = m_pBookReader->GetChapterTitle(vNewBookmarks[i]->GetBeginPos().ToFlowPosition(), m_dkxBlock);
                    vNewBookmarks[i]->SetChapterTitle(chapterTitle);
                    m_dkxBlock->AddBookmark(vNewBookmarks[i]);
                }
            }
        }

        DeleteObjInRange(vNewBookmarks.begin(), vNewBookmarks.end());
    }
    if (dkxBookRevision != localFileRevision)
    {
        m_dkxBlock->SetBookRevision(localFileRevision.c_str());
    }

    for (unsigned int i = 0; i < vOldBookmarks.size(); ++i)
    {
        SafeDeletePointer(vOldBookmarks[i]);
    }
}

HRESULT UIBookReaderContainer::HandleBookContentSearch(
        ICT_ReadingDataItem* searchResultTable[],
        UINT HightLightStatPosTable[],
        UINT HightLightEndPosTable[],
        INT locationsInBook[],
        int iTableSize,
        PINT piUsedTableSize,
        PINT piNeedTableSize,
        DK_FLOWPOSITION loadPos,
        PCCH pcchUTF8Keyword,
        int iTextLength)
{
    if(!piUsedTableSize || 
            !piNeedTableSize || 
            !pcchUTF8Keyword || 
            !m_pBookReader || 
            iTableSize <= 0 || 
            iTextLength <= 0)
    {
        return E_POINTER;
    }

    string _strSearch(pcchUTF8Keyword);
    unsigned int _uResultCount = 0;

    SEARCH_RESULT_DATA resultData;
    resultData.pSearchResultTable = searchResultTable;
    resultData.pHighlightStartPosTable = HightLightStatPosTable;
    resultData.pHighlightEndPosTable = HightLightEndPosTable;
    resultData.pLocationTable = locationsInBook;

    m_pBookReader->SearchInCurrentBook(_strSearch, loadPos, iTableSize, &resultData, &_uResultCount);
    *piNeedTableSize = _uResultCount;
    if((int)_uResultCount > iTableSize)
    {
        *piUsedTableSize = iTableSize;
    }
    else
    {
        *piUsedTableSize = (int)_uResultCount;
    }


    SQM::GetInstance()->IncCounter(SQM_ACTION_DKREADERPAGE_SEARCH_OPEN);
    return S_OK;
}

bool UIBookReaderContainer::GotoBookmarkAndHighlightKeyword(const ICT_ReadingDataItem* pBookmark)
{
    StopTTS();
    if(m_pBookReader && m_pBookReader->GotoBookMark(pBookmark))
    {
        m_pBookReader->GetPageBMP(); // 这里需要获取页面光标位置，所以必须确保当前页面已绘制完成。
        OnReadLocationChanged();

        CT_RefPos start = pBookmark->GetBeginPos();
        CT_RefPos end = pBookmark->GetEndPos();
        DK_FLOWPOSITION startPos = start.ToFlowPosition();
        DK_FLOWPOSITION endPos = end.ToFlowPosition();
        m_clsSelected.GetRectsBetweenPos(startPos, endPos, &m_vHighlightRects);
        Repaint();
        return true;
    }
    return false;
}

using namespace dk::bookstore::sync;

//只有在关闭书和开启屏保时才上传数据
void UIBookReaderContainer::PostLocalReadingDataToCloud()
{
    if (!m_pBookReader || !m_dkxBlock)
    {
        return;
    }

    if(m_localBookID.empty())
    {
        return;
    }

    // 从DKX读取数据
    vector<ICT_ReadingDataItem *> vBookmarks;
    if(!dk::utility::ReaderUtil::GetAllBookmarksForUpgrade(m_dkxBlock, &vBookmarks, m_eFormat))
    {
        return;
    }

    BookStoreInfoManager* bookStoreInfoManager = BookStoreInfoManager::GetInstance();
    ReadingBookInfo readingProgressInfo;
    readingProgressInfo.SetKernelVersion(DK_GetKernelVersion());
    readingProgressInfo.SetDeviceId(DeviceInfo::GetDeviceID().c_str());
    readingProgressInfo.SetVersion(ReadingBookInfo::CURRENT_VERSION);
    readingProgressInfo.SetBookId(m_localBookID.c_str());
    readingProgressInfo.SetBookRevision(m_pBookReader->GetBookRevision().c_str());

    ReadingBookInfo readingDataInfo;
    readingDataInfo.SetKernelVersion(DK_GetKernelVersion());
    readingDataInfo.SetVersion(ReadingBookInfo::CURRENT_VERSION);
    readingDataInfo.SetBookId(m_localBookID.c_str());
    readingDataInfo.SetBookRevision(m_pBookReader->GetBookRevision().c_str());

    int dataVersion = m_dkxBlock->GetDataVersion();

    for (size_t i = 0; i < vBookmarks.size(); ++i)
    {
        ICT_ReadingDataItem* localReadingDataItem = vBookmarks[i];
        if (NULL == localReadingDataItem)
        {
            continue;
        }
        ReadingDataItem* readingDataItem = CreateReadingDataItemFromICTReadingDataItem(*localReadingDataItem);
        if (NULL != readingDataItem)
        {
            switch (readingDataItem->GetType())
            {
                case ReadingDataItem::PROGRESS:
                    readingProgressInfo.AddReadingDataItem(*readingDataItem);
                    break;
                default:
                    readingDataInfo.AddReadingDataItem(*readingDataItem);
                    break;
            }
            delete readingDataItem;
        }
        DKXReadingDataItemFactory::DestoryReadingDataItem(localReadingDataItem);
    }

    if (readingProgressInfo.GetReadingDataItemCount() == 0)
    {
        ICT_ReadingDataItem* localReadingDataItem = DKXReadingDataItemFactory::CreateReadingDataItem();
        if (NULL != localReadingDataItem)
        {
            if (m_pBookReader->GetDKXProgressForUpgrade(m_dkxBlock, localReadingDataItem))
            {
                ReadingDataItem* readingDataItem = CreateReadingDataItemFromICTReadingDataItem(*localReadingDataItem);
                if (NULL != readingDataItem)
                {
                    readingProgressInfo.AddReadingDataItem(*readingDataItem);
                    delete readingDataItem;
                }
            }
            DKXReadingDataItemFactory::DestoryReadingDataItem(localReadingDataItem);
        }
    }

    if (WifiManager::GetInstance()->IsConnected() && CAccountManager::GetInstance()->IsLoggedIn())
    {
        bookStoreInfoManager->PutReadingProgress(m_localBookID.c_str(), m_strBookName.c_str(), readingProgressInfo, dataVersion, m_isDKBookStoreBook);
    }

    if (m_dkxBlock->IsReadingDataModified())
    {
        if (WifiManager::GetInstance()->IsConnected() && CAccountManager::GetInstance()->IsLoggedIn())
        {
            bookStoreInfoManager->PutReadingData(m_localBookID.c_str(), m_strBookName.c_str(), dataVersion, readingDataInfo, m_isDKBookStoreBook);
        }
        else
        {
            //store in local file wait uploaded
            OfflineBookStoreManager::GetInstance()->AddBookWaitUpload(m_localBookID.c_str(), m_strBookName.c_str(), m_pBookReader->GetBookRevision().c_str());
        }
    }
}

bool UIBookReaderContainer::UpdateReadingData(const ReadingBookInfo& readingBookInfo, ReadingBookInfo* localInfo)
{
    std::string cloudBookRevision = readingBookInfo.GetBookRevision();
    std::string localBookRevision = m_pBookReader->GetBookRevision();
    // 1. 云端数据结构转化为本地数据结构
    std::vector<ICT_ReadingDataItem*> cloudItems;
    for (size_t i = 0; i < readingBookInfo.GetReadingDataItemCount(); ++i)
    {
        ICT_ReadingDataItem *readingDataItem = CreateICTReadingDataItemFromReadingDataItem(readingBookInfo.GetReadingDataItem(i));
        if (NULL == readingDataItem)
        {
            continue;
        }
        cloudItems.push_back(readingDataItem);
    }
    // 本地数据结构升级
    std::vector<ICT_ReadingDataItem*> localItems;
    if (!m_pBookReader->UpdateReadingData(cloudBookRevision.c_str(), &cloudItems, localBookRevision.c_str(), &localItems))
    {
        for (size_t i = 0; i < cloudItems.size(); ++i)
        {
            DKXReadingDataItemFactory::DestoryReadingDataItem(cloudItems[i]);
        }
        DebugPrintf(DLC_LIWEI, "OnGetReadingDataUpdate false .......");
        return false;
    }
    for (size_t i = 0; i < localItems.size(); ++i)
    {
        ReadingDataItem* readingDataItem = CreateReadingDataItemFromICTReadingDataItem(*localItems[i]);
        if (NULL == readingDataItem)
        {
            delete localItems[i];
            continue;
        }
        localInfo->AddReadingDataItem(*readingDataItem);
        delete readingDataItem;
        delete localItems[i];
    }
    for (size_t i = 0; i < cloudItems.size(); ++i)
    {
        delete cloudItems[i];
    }
    return true;
}

bool UIBookReaderContainer::OnGetReadingDataUpdate(const EventArgs& args)
{
    if (NULL == m_pBookReader || NULL == m_dkxBlock)
    {
        return false;
    }
    const ReadingDataSyncUpdateArgs& readingDataSyncUpdateArgs = (const ReadingDataSyncUpdateArgs&)args;
    if (!readingDataSyncUpdateArgs.succeeded)
    {
        return false;
    }
    const ReadingDataSyncResult& result = readingDataSyncUpdateArgs.result;
    if (!result.HasReadingData())
    {
        return false;
    }
    const ReadingBookInfo* readingBookInfo = &(result.GetReadingData());
    if (readingBookInfo->HasSyncResult())
    {
        const SyncResult& syncResult = readingBookInfo->GetSyncResult();
        // 只有在成功或冲突时merge
        if (syncResult.GetResultCode() != SRC_SUCCEEDED && syncResult.GetResultCode() != SRC_CONFLICT)
        {
            return false;
        }
    }
    std::string cloudBookRevision = readingBookInfo->GetBookRevision();
    std::string localBookRevision = m_pBookReader->GetBookRevision();
    int revisionCmpResult = strcmp(cloudBookRevision.c_str(), localBookRevision.c_str());
    if (revisionCmpResult > 0)
    {
        // >0 云端比较新，无法处理，忽略
        return false;
    }
    int localDataVersion = m_dkxBlock->GetDataVersion();
    int cloudDataVersion = localDataVersion;
    // 如果有冲突
    if (readingBookInfo->HasSyncResult())
    {
        cloudDataVersion = readingBookInfo->GetSyncResult().GetLatestVersion();
    }
    // == 数据版本一致，无需更新
    // < 则云端或本地处理版本出错
    if (cloudDataVersion <= localDataVersion)
    {
        return false;
    }

    ReadingBookInfo updatedReadingBookInfo;
    size_t itemCount = readingBookInfo->GetReadingDataItemCount();
    for(size_t i = 0; i < itemCount; ++i)
    {
        updatedReadingBookInfo.AddReadingDataItem(readingBookInfo->GetReadingDataItem(i));
    }
    
    //不同格式的书籍的云端数据需要不同的计算方法        
    if(!m_pBookReader->ProcessCloudBookmarks(updatedReadingBookInfo))
    {
        return false;
    }
    
    if (revisionCmpResult < 0) // 云端书籍版本较低，先升级到本地版本
    {
        ReadingBookInfo localInfo;
        if(!UpdateReadingData(updatedReadingBookInfo, &localInfo))
        {
            return false;
        }
        m_dkxBlock->MergeCloudBookmarks(localInfo, cloudDataVersion);
    }
    else
    {
        m_dkxBlock->MergeCloudBookmarks(updatedReadingBookInfo, cloudDataVersion);
    }
    m_dkxBlock->SetReadingDataModified(true);
    m_dkxBlock->Serialize();
    return true;
}

bool UIBookReaderContainer::OnGetReadingProgressUpdate(const EventArgs& args)
{
    if (NULL == m_pBookReader)
    {
        return false;
    }
    const FetchReadingProgressUpdateArgs& fetchReadingProgressUpdateArgs =
        (const FetchReadingProgressUpdateArgs&)args;
    if (!fetchReadingProgressUpdateArgs.succeeded)
    {
        return false;
    }
    const ReadingProgressGetResult& result = fetchReadingProgressUpdateArgs.result;
    //printf("succeded: %d\n", result.IsSuccessful());
    if (!result.HasReadingProgress())
    {
        return false;
    }
    const ReadingBookInfo& readingBookInfo = result.GetReadingProgress();
    if (readingBookInfo.GetReadingDataItemCount() != 1)
    {
        return false;
    }
    const ReadingDataItem& readingDataItem = readingBookInfo.GetReadingDataItem(0);
    std::string cloudBookRevision = readingBookInfo.GetBookRevision();
    std::string localBookRevision = m_pBookReader->GetBookRevision();
    int revisionCmpResult = strcmp(cloudBookRevision.c_str(), localBookRevision.c_str());
    if (revisionCmpResult > 0)
    {
        // 云端比较新，无法处理，忽略
        return false;
    }
	if(readingBookInfo.GetDeviceId() == DeviceInfo::GetDeviceID())
	{
		//如果是本设备上传的数据，不提示
		return false;
	}
    ICT_ReadingDataItem *readingProgress = DKXReadingDataItemFactory::CreateReadingDataItem();
    if (NULL == readingProgress)
    {
        return false;
    }
    readingProgress->SetUserDataType(ICT_ReadingDataItem::PROGRESS);
    readingProgress->SetBeginPos(readingDataItem.GetRefPos());
    readingProgress->SetEndPos(readingDataItem.GetRefPos());
    
    //不同格式的云端数据需要不同的计算方法  
    m_pBookReader->ProcessCloudDKXProgress(readingProgress);
    
    if (revisionCmpResult < 0) // 云端版本较低，先升级到本地版本
    {
        std::vector<ICT_ReadingDataItem*> cloudItems;
        cloudItems.push_back(readingProgress);
        std::vector<ICT_ReadingDataItem*> localItems;
        if (!m_pBookReader->UpdateReadingData(
                    cloudBookRevision.c_str(),
                    &cloudItems,
                    localBookRevision.c_str(),
                    &localItems)
                || localItems.size() != 1)
        {
            delete readingProgress;
            return false;
        }
        delete readingProgress;
        readingProgress = localItems[0];
    }

    ATOM posCurPageAtom, endAtom;
    m_pBookReader->GetCurrentPageATOM(posCurPageAtom, endAtom);
	if (m_pBookReader->IsPositionInCurrentPage(readingProgress->GetBeginPos().ToFlowPosition()) || (readingProgress->GetBeginPos() == m_getServerProgressPos))
	{
        delete readingProgress;
        return false;
    }

    long cloudPageIndex = m_pBookReader->GetPageIndex(readingProgress->GetBeginPos());
    long currentPageIndex = m_pBookReader->GetCurrentPageIndex();
    char buf[512];
    if(currentPageIndex == -1 || cloudPageIndex == -1 || DFF_MobiPocket == m_eFormat)
    {
        string curChapterTitle = m_pBookReader->GetChapterTitle(posCurPageAtom.ToFlowPosition(), m_dkxBlock);
        string cloudChapterTitle = m_pBookReader->GetChapterTitle(readingProgress->GetBeginPos().ToFlowPosition(), m_dkxBlock);
        if(curChapterTitle.empty())
        {
            curChapterTitle = StringManager::GetPCSTRById(BOOKSTORE_USERDATA_SYNC_BOOKCOVER);
        }
        if(cloudChapterTitle.empty())
        {
            cloudChapterTitle = StringManager::GetPCSTRById(BOOKSTORE_USERDATA_SYNC_BOOKCOVER);
        }
        readingProgress->GetBookContent();
        snprintf(buf, DK_DIM(buf), StringManager::GetPCSTRById(BOOKSTORE_USERDATA_SYNC_PAGING), curChapterTitle.c_str(), cloudChapterTitle.c_str());
    }
    else
    {
        snprintf(buf, DK_DIM(buf), StringManager::GetPCSTRById(BOOKSTORE_USERDATA_SYNC_GOTO_PAGE), currentPageIndex + 1, cloudPageIndex + 1);
    }

    // TODO: message box and jump
    ClearSelection();
    UIMessageBox msgBox(GetParent(), buf, MB_OK | MB_CANCEL);
    if (msgBox.DoModal() == MB_OK)
    {
        GotoBookmark(readingProgress);
    }
    delete readingProgress;
    return true;
}

bool UIBookReaderContainer::OnAccountLoginEvent(const EventArgs& args)
{
    //关闭屏保50 秒后才会真正断网，此时重新开启后会重新连接网络并自动登录
	if(m_isScreenSaverOut)
	{
		BeginGetReadingData();
	}
    return true;
}

bool UIBookReaderContainer::OnParseFinishedEvent(const EventArgs& args)
{
	CDisplay* pDisplay = CDisplay::GetDisplay();
	pDisplay->StopFullRepaintOnce();
    if (pDisplay)
    {
        pDisplay->Repaint(PAINT_FLAG_REPAINT);
    }
	return true;
}

//只有在打开书或者关闭屏保时才获取阅读数据
void UIBookReaderContainer::BeginGetReadingData()
{
	if(!m_localBookID.empty() && WifiManager::GetInstance()->IsConnected() && CAccountManager::GetInstance()->IsLoggedIn())
	{
        m_isScreenSaverOut = false;
        ATOM endAtom;
		m_pBookReader->GetCurrentPageATOM(m_getServerProgressPos, endAtom);
		//获取进度
		BookStoreInfoManager::GetInstance()->FetchReadingProgress(m_localBookID.c_str(), m_isDKBookStoreBook);
		//获取阅读数据
		BookStoreInfoManager::GetInstance()->FetchReadingData(
                m_localBookID.c_str(),
                m_dkxBlock->GetDataVersion(),
                m_isDKBookStoreBook);
	}
}

bool UIBookReaderContainer::IsVerticalRTL() const
{
    return (m_pBookReader && (m_pBookReader->GetBookDirection() & BOOK_MODE_VRL));
}

bool UIBookReaderContainer::IsVerticalLTR() const
{
    return (m_pBookReader && (m_pBookReader->GetBookDirection() & BOOK_MODE_VLR));
}

bool UIBookReaderContainer::IsVerticalForms() const
{
    return IsVerticalRTL() || IsVerticalLTR();
}

bool UIBookReaderContainer::IsHorizontalHTB() const
{
    return (m_pBookReader && (m_pBookReader->GetBookDirection() & BOOK_MODE_HTB));
}

bool UIBookReaderContainer::IsWritingLTR() const
{
    return (m_pBookReader && (m_pBookReader->GetBookDirection() & BOOK_DIRECTION_LTR));
}

bool UIBookReaderContainer::IsWritingRTL() const
{
    return (m_pBookReader && (m_pBookReader->GetBookDirection() & BOOK_DIRECTION_RTL));
}

bool UIBookReaderContainer::IsScannedPdf() const
{
    return ((DFF_PortableDocumentFormat == m_eFormat) && !m_bIsPdfTextMode);
}

bool UIBookReaderContainer::IsZoomedPdf() const
{
    if (IsScannedPdf())
    {
        return (m_modeController.m_eReadingMode == PDF_RM_ZoomPage) && (m_modeController.m_dUserZoom > 1);
    }
    return false;
}

bool UIBookReaderContainer::InitZoomedPdfPara()
{
    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (IsScannedPdf())
    {
        assert (NULL != m_pBookReader);
		if(!m_isPageTurned)
		{
			return false;
		}
		else
		{
			m_isPageTurned= false;
		}
        DK_IMAGE* pInitImage = m_pBookReader->GetPageBMP();
        assert (NULL != pInitImage);
        
        const int pbDataLength = ((pInitImage->iWidth + 3) / 4 * 4) * pInitImage->iHeight;
        const int bmpFileHeaderSize = sizeof(BITMAPFILEHEADER);
        const int bmpInfoHeaderSize = sizeof(BITMAPINFOHEADER);
        const int plusSize = 256 * sizeof(int);
        const int bmpSize = bmpFileHeaderSize + bmpInfoHeaderSize + plusSize + pbDataLength;

        SafeFreePointer(m_pInitBMP);
        SafeDeletePointer(m_pImgStream);

        m_pInitBMP = DK_MALLOCZ_OBJ_N(char, bmpSize);
        if (NULL == m_pInitBMP)
        {
            DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s NULL == pInitBMP", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            return false;
        }
        ConvertImageToBMP(pInitImage, m_pInitBMP);

        m_pImgStream = DkStreamFactory::GetMemoryStream(m_pInitBMP, bmpSize, bmpSize);
        m_modeController.m_zoomX0 = 0;
        m_modeController.m_zoomY0 = 0;
        m_modeController.m_dUserZoom = 0;
        DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s return true", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return (NULL != m_pImgStream);
    }
    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s return false", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return false;
}

namespace
{
    struct BookStoreBookData
    {
        std::string bookId;
        std::string bookTitle;
    };
}

static void ShowBuyPage(void* data)
{
    BookStoreBookData* buyPageData = (BookStoreBookData*)data;
    UIBookDetailedInfoPage* bookInfoPage = new UIBookDetailedInfoPage(buyPageData->bookId, buyPageData->bookTitle);
    delete buyPageData;
    CDisplay* pDisplay = CDisplay::GetDisplay();
    bookInfoPage->MoveWindow(0, 0, pDisplay->GetScreenWidth(), pDisplay->GetScreenHeight());
    CPageNavigator::Goto(bookInfoPage);
    
}

bool UIBookReaderContainer::HandleNextPageInLast()
{
    if(!m_clsSelected.IsSelectionMode() && !m_localBookID.empty())
    {
        if(m_isTrialBook)
        {
            UIMessageBox msgBox(this, StringManager::GetPCSTRById(BOOKSTORE_BUY_WHOLE), MB_OK | MB_CANCEL);
            msgBox.SetButtonText(MB_OK, StringManager::GetPCSTRById(BOOKSTORE_BUY));
            //msgBox.ShowIcon(true);
            if (msgBox.DoModal() == MB_OK)
            {
                if(UIUtility::CheckAndReLogin())
                {
                    BookStoreBookData* data = new BookStoreBookData();
                    if (NULL != data)
                    {
                        data->bookId = m_localBookID;
                        data->bookTitle = m_pBookReader->GetBookInfo().title;
                        //CPageNavigator::BackToPrePage();
                        CNativeThread::AsyncCall(ShowBuyPage, data);
                        return true;
                    }
                }
            }
        }
        else if(m_isDKBookStoreBook)
        {
            CDisplay* pDisplay = CDisplay::GetDisplay();
            UIBookStoreAddCommentPage* pAddCommentPage = new UIBookStoreAddCommentPage(m_localBookID, m_strBookName);
            if (pDisplay && pAddCommentPage)
            {
                pAddCommentPage->MoveWindow(0, 0, DeviceInfo::GetDisplayScreenWidth(),DeviceInfo::GetDisplayScreenHeight());
                CPageNavigator::Goto(pAddCommentPage);
                return true;
            }
		}
    }
    return false;
}

bool UIBookReaderContainer::IsDotProgressIndexValid() const
{
    return m_dotProgressFocusIndex >= 0 && m_dotProgressFocusIndex < (int)m_vDotProgress.size();
}

bool UIBookReaderContainer::ConvertImageToBMP(const DK_IMAGE* pSrcImg, char* pDstBMP)
{
    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    assert ((NULL != pSrcImg) && (NULL != pDstBMP));
    const int bmpFileHeaderSize = sizeof(BITMAPFILEHEADER);
    const int bmpInfoHeaderSize = sizeof(BITMAPINFOHEADER);
    const int plusSize = 256 * sizeof(int);

    int biBitCount = 8;
    const int imageWidth = pSrcImg->iWidth;
    const int imageHeight = pSrcImg->iHeight;
    const int initWidth = (imageWidth * biBitCount / 8 + 3) / 4 * 4; 
    const int initHeight = imageHeight;
    const int pbDataLength = initWidth * initHeight;

    BITMAPFILEHEADER bmp_file_header;
    memset(&bmp_file_header, 0, bmpFileHeaderSize);
    bmp_file_header.bfOffBits =  bmpFileHeaderSize + bmpInfoHeaderSize + plusSize;
    bmp_file_header.bfSize = pbDataLength + bmp_file_header.bfOffBits;
    bmp_file_header.bfType = 0x4d42;

    BITMAPINFOHEADER bmp_info_header;
    memset(&bmp_info_header, 0, bmpInfoHeaderSize);
    bmp_info_header.biSize = bmpInfoHeaderSize;
    bmp_info_header.biWidth = initWidth;
    bmp_info_header.biHeight = imageHeight;
    bmp_info_header.biPlanes = 1;
    bmp_info_header.biBitCount = biBitCount;
    bmp_info_header.biSizeImage = initWidth * imageHeight;
    bmp_info_header.biClrUsed = 256;

    char* pTemp = pDstBMP;
    memcpy(pTemp, &bmp_file_header, bmpFileHeaderSize);
    pTemp += bmpFileHeaderSize;
    memcpy(pTemp, &bmp_info_header, bmpInfoHeaderSize);
    pTemp += bmpInfoHeaderSize;
    memset(pTemp, 0 , plusSize);
    for(int i=0; i < 256; i++)
    {
        memset(pTemp + (i << 2), 255 - i, 4);
    }
    pTemp += plusSize;

    memset(pTemp, 0, pbDataLength);
    
    int i = initHeight;
    int tempOffset = (initHeight - 1) * initWidth;
    int pbDataOffset = 0;
    while (i > 0)
    {
        memcpy(pTemp + tempOffset, pSrcImg->pbData + pbDataOffset, imageWidth);
        for (int j = 0; j < imageWidth; j++)
        {
            char& gray256 = pTemp[tempOffset + j];
            gray256 = (gray256 >= 0x80)? 0xFF: 0x00;
        }
        i--;
        tempOffset -= initWidth;
        pbDataOffset += imageWidth;
    }
    
    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s return true", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return true;
}

bool UIBookReaderContainer::ResetImage(DK_IMAGE *pImage, int colorChannels, int width, int height)
{
    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (NULL == pImage || colorChannels < 0 || width < 0 || height < 0)
    {
        DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s return false", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return false;
    }
    memset(pImage, 0, sizeof(DK_IMAGE));
    pImage->iColorChannels = colorChannels;
    pImage->iWidth = width;
    pImage->iStrideWidth = pImage->iWidth * pImage->iColorChannels;
    pImage->iHeight = height;

    SafeFreePointer(pImage->pbData);
    if (pImage->iStrideWidth * pImage->iHeight > 0)
    {
        pImage->pbData = DK_MALLOCZ_OBJ_N(BYTE, pImage->iStrideWidth * pImage->iHeight);
        if (NULL == pImage->pbData)
        {
            DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s return false", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            return false;
        }
    }

    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s return true", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return true;
}

bool UIBookReaderContainer::UIBookReaderContainerGestureListener::OnDown(MoveEvent* moveEvent)
{
    DebugPrintf(DLC_GESTURE, "UIBookReaderContainer::OnDown");
    return m_pBookReaderContainer->OnDown(moveEvent);
}

bool UIBookReaderContainer::UIBookReaderContainerGestureListener::OnSingleTapUp(MoveEvent* moveEvent)
{
    DebugPrintf(DLC_GESTURE, "UIBookReaderContainer::OnSingleTapUp");
    return false;
}

bool UIBookReaderContainer::UIBookReaderContainerGestureListener::OnScroll(MoveEvent* moveEvent1, MoveEvent* moveEvent2, int distanceX, int distanceY)
{
    return m_pBookReaderContainer->OnScroll(moveEvent1, moveEvent2, distanceX, distanceY);
}

bool UIBookReaderContainer::OnScroll(MoveEvent* moveEvent1, MoveEvent* moveEvent2, int distanceX, int distanceY)
{
    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "UIBookReaderContainer::OnScroll m_isSelecting = %d", m_isSelecting);
    if (WAIT_FOR_SCALE != m_scaleFlag)
    {
        DebugPrintf(DLC_GESTURE, "return m_scaleFlag = %d", m_scaleFlag);
        return true;
    }

    if (m_isSelecting)
    {
        if (IsInSelectionProgress() && (abs(distanceX) > 0 || abs(distanceY) > 0))
        {
            DebugPrintf(DLC_UIBOOKREADERCONTAINER, "UIBookReaderContainer::OnScroll IsInSelectionProgress()");
            OnSelectMove(moveEvent2->GetX(), moveEvent2->GetY());
        }
        return true;
    }
    else if (IsZoomedPdf())
    {
        return OnTouchMoveZoomPdf(-distanceX, -distanceY);
    }

    return true;
}

void UIBookReaderContainer::UIBookReaderContainerGestureListener::OnLongPress(MoveEvent* moveEvent)
{
    DebugPrintf(DLC_GESTURE, "UIBookReaderContainer::OnLongPress");
    m_pBookReaderContainer->OnLongPress(moveEvent);
}

void UIBookReaderContainer::UIBookReaderContainerGestureListener::OnSingleTapConfirmed(MoveEvent* moveEvent)
{
    DebugPrintf(DLC_GESTURE, "UIBookReaderContainer::OnSingleTapConfirmed");
    m_pBookReaderContainer->OnTouchTap(GESTURE_END, moveEvent->GetX(), moveEvent->GetY());
}

bool UIBookReaderContainer::UIBookReaderContainerGestureListener::OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy)
{
    DebugPrintf(DLC_GESTURE, "UIBookReaderContainer::OnFling(%d,%d)-(%d,%d), dir: %d", moveEvent1->GetX(), moveEvent1->GetY(), moveEvent2->GetX(), moveEvent2->GetY(), direction);
    
    int flingDistance = abs(moveEvent1->GetX() - moveEvent2->GetX());
    int minFlingDistance = DeviceInfo::GetDisplayScreenWidth() / 10;

    if (direction == FD_UP || direction == FD_DOWN)
    {
        flingDistance = abs(moveEvent1->GetY() - moveEvent2->GetY());
        minFlingDistance = DeviceInfo::GetDisplayScreenHeight() / 10;
    }
    if (flingDistance < minFlingDistance || FD_UNKNOWN == direction)
    {
        DebugPrintf(DLC_GESTURE, "Tread min fling as tap");
        return m_pBookReaderContainer->OnTouchTap(GESTURE_END, moveEvent2->GetX(), moveEvent2->GetY());
    }
    else
    {
        return m_pBookReaderContainer->OnFling(moveEvent1, moveEvent2, direction, vx, vy);
    }
}

bool UIBookReaderContainer::UIBookReaderContainerGestureListener::OnDoubleTap(MoveEvent* moveEvent)
{
    DebugPrintf(DLC_GESTURE, "UIBookReaderContainer::OnDoubleTap");
    return (NULL != m_pBookReaderContainer) ? m_pBookReaderContainer->OnDoubleTapEvent(moveEvent) : false;
}

bool UIBookReaderContainer::UIBookReaderContainerGestureListener::OnDoubleTapEvent(MoveEvent* moveEvent)
{
    DebugPrintf(DLC_GESTURE, "UIBookReaderContainer::OnDoubleTapEvent");
    return false;
}

bool UIBookReaderContainer::OnDoubleTapEvent(MoveEvent* moveEvent)
{
    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "UIBookReaderContainer::OnDoubleTapEvent");
    if (IsZoomedPdf())
    {
        m_modeController.m_eReadingMode = PDF_RM_NormalPage;
        m_modeController.m_dUserZoom = 1.0;
        return FinishZoomPdf();
    }
    else if (IsComicsChapter() && moveEvent)
    {
        DK_POS pos(moveEvent->GetX(), moveEvent->GetY());
        return SwitchComicsFrameMode(&pos);
    }
    return false;
}

bool UIBookReaderContainer::OnTouchEvent(MoveEvent* moveEvent)
{
    DebugPrintf(DLC_GESTURE, "%s, %d, %s, %s start----------------- time = %d", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, (int)SystemUtil::GetUpdateTimeInMs());
    if (moveEvent->GetActionMasked() == MoveEvent::ACTION_UP)
    {
        OnUp(moveEvent);
    }
    m_scaleGestureDetector.OnTouchEvent(moveEvent);
    return m_gestureDetector.OnTouchEvent(moveEvent);
}

bool UIBookReaderContainer::UIBookReaderContainerScaleGestureListener::OnScale(ScaleGestureDetector* detector)
{
    DebugPrintf(DLC_GESTURE, "UIBookReaderContainer::OnScale");
    if (NULL == m_pBookReaderContainer)
    {
        DebugPrintf(DLC_GESTURE, "UIBookReaderContainer::OnScale return false");
        return false;
    }
    return m_pBookReaderContainer->OnScale(detector);
}

bool UIBookReaderContainer::UIBookReaderContainerScaleGestureListener::OnScaleBegin(ScaleGestureDetector* detector)
{
    if (NULL == m_pBookReaderContainer)
    {
        return false;
    }
    return m_pBookReaderContainer->OnScaleBegin(detector);
}

bool UIBookReaderContainer::UIBookReaderContainerScaleGestureListener::OnScaleEnd(ScaleGestureDetector* detector)
{
    if (NULL == m_pBookReaderContainer)
    {
        return false;
    }
    return m_pBookReaderContainer->OnScaleEnd(detector);
}

bool UIBookReaderContainer::IsInSelectionProgress() const
{
    return m_clsSelected.IsInitialized() && m_clsSelected.IsSelectionMode();
}

bool UIBookReaderContainer::SwitchComicsFrameMode(const DK_POS* pPos)
{
    if (IsComicsChapter() && m_pBookReader)
    {
        if (m_pBookReader->IsComicsFrameMode())
        {
            m_pBookReader->SetComicsFrameMode(false);
        }
        else if (pPos)
        {
            DKE_HITTEST_OBJECTINFO objInfo;
            const DK_POS& pos = *pPos;
            m_clsSelected.GetHitObject(pos, &objInfo);
            if (objInfo.objType == DKE_PAGEOBJ_IMAGE)
            {
                DK_LONG frameIndex = m_clsSelected.HitTestComicsFrame(pos);
                if (frameIndex >= 0)
                {
                    m_pBookReader->SetComicsFrameMode(true, frameIndex);
                }
            }
            m_clsSelected.FreeHitObject(&objInfo);
        }
        else
        {
            m_pBookReader->SetComicsFrameMode(true);
        }
        UpdateWindow();
        return true;
    }
    return false;
}

bool UIBookReaderContainer::OnLongPress(MoveEvent* moveEvent)
{
    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "UIBookReaderContainer::OnLongPress(), %d", m_clsSelected.IsInitialized());
    //if (IsScannedPdf())
    //{
        //return true;
    //}
    if (m_pReaderSettingDlg && m_pReaderSettingDlg->IsVisible())
    {
        return true;
    }

    int x = moveEvent->GetX();
    int y = moveEvent->GetY();
    if (m_clsSelected.IsInitialized())
    {
        if(x < m_iHorizontalMargin)
        {
            return false;
        }
        y = y < m_iTopMargin ? m_iTopMargin : y;

        if (IsInSelectionProgress())
        {
            return true;
        }

        m_isSelecting = true;
        if(!HitTestAndMove(GESTURE_LONG_TAP, x, y))
        {
            return false;
        }
        m_clsSelected.SetSelectionMode(true);
        m_clsSelected.ResetDrawMode();
        m_clsSelected.SetDrawMode(UIBookReaderSelected::DRAW_ALL_SELECTION);
        DrawSelected(m_imageReader);

        return true;
    }
    return false;
}

void UIBookReaderContainer::ClearSelection()
{
    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "UIBookReaderContainer::ClearSelection()");
    m_iCurCommentIndex    = -1;
    m_iCurDigestIndex     = -1;
    m_clsStartSelected    = CT_RefPos(-1, -1, -1, -1);
    m_clsEndSelected      = CT_RefPos(-1, -1, -1, -1);
    m_strSelected.clear();
    m_clsSelected.SetSelectionMode(false);
    m_clsSelected.ResetDrawMode();
    m_imageSelectedEnd.SetVisible(FALSE);
    m_imageSelectedStart.SetVisible(FALSE);
    m_isSelecting = false;
    UICommentOrSummary* pComment = UICommentOrSummary::GetInstance();
    if (pComment && pComment->IsVisible())
    {
        pComment->SetVisible(false);
    }
}

void UIBookReaderContainer::OnSelectMove(int x, int y)
{
    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "UIBookReaderContainer::OnSelectMove");
    if (!m_processMove || !HitTestAndMove(GESTURE_MOVE_LEFT, x, y))
    {
        return;
    }
    m_clsSelected.SetSelectionMode(true);
    m_clsSelected.SetDrawMode(UIBookReaderSelected::DRAW_NEW_SELECTION);
    DebugPrintf(DLC_GESTURE, "UIBookReaderContainer::OnSelectionMove(%d,%d,%d)~(%d,%d,%d)",
            m_clsStartSelected.GetChapterIndex(),
            m_clsStartSelected.GetParaIndex(),
            m_clsStartSelected.GetAtomIndex(),
            m_clsEndSelected.GetChapterIndex(),
            m_clsEndSelected.GetParaIndex(),
            m_clsEndSelected.GetAtomIndex()
            );
    CDisplay* pDisplay = CDisplay::GetDisplay();
    if (pDisplay)
    {
        pDisplay->StopFullRepaintOnce();
    }
    DrawSelected(m_imageReader);
}

bool UIBookReaderContainer::ShouldTreatUpDownAsPageSwitch()
{
    return SystemSettingInfo::GetInstance()->ShouldTreatUpDownGestureAsPageSwitch();
}

int UIBookReaderContainer::GetRotationDegree() const
{
    return IsScannedPdf() ? m_modeController.m_iRotation : 0;
}

bool UIBookReaderContainer::FinishZoomPdf(bool updateWindow)
{
    DebugPrintf(DLC_GESTURE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (NULL == m_pBookReader)
    {
        return false;
    }

    m_pBookReader->SetPdfModeController(&m_modeController);
    LoadSystemPageSizeInfo(m_pageWidth, m_pageHeight);
    DebugPrintf(DLC_GESTURE, "%s, %d, %s, %s m_pageWidth = %d m_pageHeight = %d", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, m_pageWidth, m_pageHeight);
    m_pBookReader->SetPageSize(m_iHorizontalMargin, m_iTopMargin, m_pageWidth, m_pageHeight);
    m_curSpan = 0;
    if (updateWindow)
    {
        UpdateWindow();
    }
    DebugPrintf(DLC_GESTURE, "%s, %d, %s, %s return true", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return true;
}

bool UIBookReaderContainer::ActionOfGallery(MoveEvent* moveEvent1, MoveEvent* moveEvent2, DK_POS& pos)
{
    //DebugPrintf(DLC_DIAGNOSTIC, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (NULL == moveEvent1 || NULL == moveEvent2)
    {
        return false;
    }
    DK_POS start(moveEvent1->GetX(), moveEvent1->GetY());
    m_dotProgressFocusIndex = -1;
    for (size_t i = 0; i < m_vGalleryObjects.size(); ++i)
    {
        const GalleryObjectInfo& object = m_vGalleryObjects[i];
        const DK_RECT& bounding = object.m_boundingImage;
        if (UIUtility::PosInRect(bounding, start))
        {
            const int X0 = bounding.left;
            const int Y0 = bounding.top;
            const int X1 = bounding.right;
            const int Y1 = bounding.bottom;
            pos = DK_POS(X0 + ((X1 - X0) >> 1), Y0 + ((Y1 - Y0) >> 1));
            m_dotProgressFocusIndex = i;
            return true;
        }
    }
    return false;
}

void UIBookReaderContainer::UpdateDotProgress()
{
    if (IsDotProgressIndexValid())
    {
        UIDotProgressSPtr dot = m_vDotProgress[m_dotProgressFocusIndex];
        const GalleryObjectInfo& object = m_vGalleryObjects[m_dotProgressFocusIndex];
        if (dot)
        {
            dot->SetProgress(object.m_activeCellIndex, object.m_cellCount);
        }
    }
}

bool UIBookReaderContainer::GotoPreFrameInGallery(const DK_POS& pos)
{
    //DebugPrintf(DLC_DIAGNOSTIC, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (IsDotProgressIndexValid() && m_pBookReader && m_pBookReader->GotoPreFrameInGallery(pos))
    {
        GalleryObjectInfo& galleryObject = m_vGalleryObjects[m_dotProgressFocusIndex];
        int& activeCellIndex = galleryObject.m_activeCellIndex;
        const int& cellCount = galleryObject.m_cellCount;
        activeCellIndex--;
        if (activeCellIndex < 0)
        {
            activeCellIndex = cellCount - 1;
        }
        UpdateDotProgress();
        UpdateWindow();
        return true;
    }
    return false;
}

bool UIBookReaderContainer::GotoNextFrameInGallery(const DK_POS& pos)
{
    //DebugPrintf(DLC_DIAGNOSTIC, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (IsDotProgressIndexValid() && m_pBookReader && m_pBookReader->GotoNextFrameInGallery(pos))
    {
        GalleryObjectInfo& galleryObject = m_vGalleryObjects[m_dotProgressFocusIndex];
        int& activeCellIndex = galleryObject.m_activeCellIndex;
        const int& cellCount = galleryObject.m_cellCount;
        activeCellIndex++;
        if (activeCellIndex >= cellCount)
        {
            activeCellIndex = 0;
        }
        UpdateDotProgress();
        UpdateWindow();
        return true;
    }
    return false;
}

static bool ResponseInteractiveImageTouch = true;
bool UIBookReaderContainer::HandleInteractiveImage(int _x, int _y)
{
    DK_POS pos(_x, _y);
    bool ret = false;
    for (unsigned int i=0; i<m_vPageInteractiveObjects.size(); ++i)
    {
        const DK_RECT& bounding = m_vPageInteractiveObjects[i].m_bounding;
        if (UIUtility::PosInRect(bounding, pos))
        {
            if(!ResponseInteractiveImageTouch)
            {
                return true;
            }
            ResponseInteractiveImageTouch = false;
            DKE_HITTEST_OBJECTINFO objInfo;
            m_clsSelected.GetHitObject(pos, &objInfo);
            switch(objInfo.objType)
            {
            case DKE_PAGEOBJ_INTERACTIVEIMAGE:
                {
                    UIInteractiveImageDetailPage* pPage = new UIInteractiveImageDetailPage(m_strBookName);
                    if (pPage)
                    {
                        pPage->SetHitObjectInfo(objInfo);
                        CPageNavigator::Goto(pPage);
                        ret = true;
                    }
                }
                break;
            case DKE_PAGEOBJ_GALLERY:
                {
                    IDKEGallery* pGallery = NULL;
                    if (m_clsSelected.HitTestGallery(pos, &pGallery) && pGallery)
                    {
                        UIInteractiveImageDetailPage* pPage = new UIInteractiveImageDetailPage(m_strBookName);
                        if (pPage)
                        {
                            pPage->SetGallery(pGallery);
                            CPageNavigator::Goto(pPage);
                            ret = true;
                        }
                        m_clsSelected.FreeHitTestGallery(pGallery);
                    }
                }
                break;
            case DKE_PAGEOBJ_PREBLOCK:
                {
                    DKE_PREBLOCK_INFO preBlock;
                    if (m_clsSelected.HitTestPreBlock(pos, &preBlock) && (NULL != preBlock.pPreFlexPage))
                    {
                        UIInteractiveFlexPage* pPage = new UIInteractiveFlexPage(m_strBookName);
                        if (pPage)
                        {
                            pPage->SetPreBlockInfo(preBlock);
                            CPageNavigator::Goto(pPage);
                            ret = true;
                        }
                        m_clsSelected.FreeHitTestPreBlock(&preBlock);
                    }
                }
                break;
            default:
                break;
            }
            ResponseInteractiveImageTouch = true;
            m_clsSelected.FreeHitObject(&objInfo);
        }
    }
    return ret;
}

bool UIBookReaderContainer::HandleFootNote(int _x, int _y)
{
    DKE_FOOTNOTE_INFO footNoteInfo;
    const DK_POS pos((DK_DOUBLE)_x, (DK_DOUBLE)_y);
    if (m_clsSelected.HitTestFootnote(pos, &footNoteInfo))
    {
        DKE_SHOWCONTENT_HANDLE pHandle;
        UIFootNoteDialog dlgFootNote(this);
        if(DKE_FOOTNOTECONTENT_ALTTEXT == footNoteInfo.noteType)
        {
            string footNote = EncodeUtil::ToString(footNoteInfo.noteText);
            dlgFootNote.SetText(footNote.c_str());
        }
        else
        {
            m_clsSelected.GetFootnoteContentHandle(footNoteInfo.noteId, DKE_SHOWCONTENT_FIXEDPAGE, &pHandle);
            dlgFootNote.SetSection(pHandle.pSection);
        }

        const int    offset      = 9;
        const int    dlgHeight   = dlgFootNote.GetMaxHeight();
        const DK_BOX boundingBox = footNoteInfo.boundingBox;
        const int    boxX0       = (int)boundingBox.X0;
        const int    boxX1       = (int)boundingBox.X1;
        const int    boxY0       = (int)boundingBox.Y0;
        const int    boxY1       = (int)boundingBox.Y1;
        DebugPrintf(DLC_UIBOOKREADERCONTAINER, "[%f, %f, %f, %f]", boundingBox.X0, boundingBox.Y0, boundingBox.X1, boundingBox.Y1);
        const int    arrowX      = boxX0 + ((boxX1 - boxX0) >> 1);
        DebugPrintf(DLC_UIBOOKREADERCONTAINER, "arrowX   = %d", arrowX);
        bool arrowUp = true;
        int  top     = boxY1 + offset;
        int  arrowY  = boxY1;

        if ((top + dlgHeight) > m_iHeight)
        {
            top = boxY0 - dlgHeight - offset;
            arrowY = boxY0;
            arrowUp = false;
        }
        dlgFootNote.SetArrowPara(arrowUp, arrowX, arrowY);
        dlgFootNote.MoveWindow(0, top, 0, dlgHeight);
        dlgFootNote.DoModal();

        if(DKE_FOOTNOTECONTENT_RICHTEXT == footNoteInfo.noteType)
        {
            m_clsSelected.FreeContentHandle(DKE_SHOWCONTENT_FIXEDPAGE, &pHandle);
        }
        m_clsSelected.FreeHitTestFootnote(&footNoteInfo);
        return true;
    }
    m_clsSelected.FreeHitTestFootnote(&footNoteInfo);
    return false;
}

bool UIBookReaderContainer::HandleCommentTouch(int _x, int _y)
{
    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "m_commentTapLocation.size() = %d", m_commentTapLocation.size());
    for (vector<COMMENT_TAP_LOCATION>::iterator vi = m_commentTapLocation.begin(); vi != m_commentTapLocation.end(); ++vi)
    {
        const DK_RECT rect = (*vi).imgRect;
        DebugPrintf(DLC_UIBOOKREADERCONTAINER, "rect[%d, %d, %d, %d]", rect.left, rect.top, rect.right, rect.bottom);
        DebugPrintf(DLC_UIBOOKREADERCONTAINER, "pos [%d, %d]", _x, _y);
        if (rect.left <= _x && _x <= rect.right && rect.top <= _y && _y <= rect.bottom)
        {
            DebugPrintf(DLC_UIBOOKREADERCONTAINER, "rect contain (x, y)");
            string comment = GetUserComment((*vi).startPos, (*vi).endPos);
            if (comment.length() > 0)
            {
                DebugPrintf(DLC_UIBOOKREADERCONTAINER, "comment = %s", comment.c_str());
                UIFootNoteDialog dlgFootNote(this);
                dlgFootNote.SetText(comment.c_str());

                const int    offset      = 9;
                const int    dlgHeight   = dlgFootNote.GetMaxHeight();
                const int    arrowX      = _x;
                DebugPrintf(DLC_UIBOOKREADERCONTAINER, "arrowX   = %d", arrowX);
                int top     = ((rect.bottom + offset + dlgHeight) > m_iHeight) ? (rect.top - dlgHeight - offset) : (rect.bottom + offset);
                dlgFootNote.ShowArrow(false);
                dlgFootNote.MoveWindow(0, top, 0, dlgHeight);
                dlgFootNote.DoModal();
                return true;
            }
        }
    }
    return false;
}

bool UIBookReaderContainer::HandleLinkJump(const DK_POS& curPoint)
{
    if(DFF_ElectronicPublishing == m_eFormat || DFF_MobiPocket == m_eFormat)
    {
        bool isInternalLink = true;
        if(m_clsSelected.CheckIsLink(curPoint, isInternalLink))
        {
            DebugPrintf(DLC_DIAGNOSTIC, "after CheckIsLink:%s", isInternalLink ? "true" : "false");
            if (isInternalLink)
            {
                DK_FLOWPOSITION posLink;
                if (m_clsSelected.GetLinkFlowPosition(curPoint, &posLink))
                {
                    if (BOOKREADER_GOTOPAGE_SUCCESS == MoveToFlowPosition(posLink))
                    {
                        m_bLastPage = false;
                        Repaint();
                    }
                    else
                    {
                        m_bLastPage = true;
                        m_bFirstPage = false;
                    }
                    OnReadLocationChanged();
                    return true;
                }
            }
            else
            {
                std::string url;
                if (m_clsSelected.GetExternelLink(curPoint, url))
                {
                    char strMessage[256] = {0};
                    std::string urlMessage = url;
                    if (urlMessage.length() > EXTERNAL_LINK_MESSAGE_MAX_LENGTH)
                    {
                        urlMessage = urlMessage.substr(0, EXTERNAL_LINK_MESSAGE_MAX_LENGTH);
                        urlMessage.append("...");
                    }
                    snprintf(strMessage,
                             sizeof(strMessage)/sizeof(char),
                             StringManager::GetPCSTRById(ASK_OPEN_EXTERNAL_LINK),
                             urlMessage.c_str());
                    
                    UIMessageBox messagebox(m_pParent, strMessage, MB_OK | MB_CANCEL);
                    if(MB_OK == messagebox.DoModal())
                    {
                        std::string external_url = getExternLinkURL(url);
                        CommandList commands;
                        commands.push_back(external_url);
                        if(UIUtility::CheckAndReConnectWifi())
                        {
                            WebBrowserLauncher::GetInstance()->Exec(commands);
                        }
                    }
                    return true;
                }
            }
        }
    }
    return false;
}

bool UIBookReaderContainer::OnWebBrowserClosed(const EventArgs& args)
{
    Repaint();
    return true;
}

bool UIBookReaderContainer::OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy)
{
    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "UIBookReaderContainer::OnFling(%d)", direction);
    if (IsZoomedPdf() || (STOP_SCALE == m_scaleFlag))
    {
        DebugPrintf(DLC_GESTURE, "%s, %d, %s, %s IsZoomedPdf", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return true;
    }

    if (m_isSelecting)
    {
        if (!IsInSelectionProgress())
        {
            ClearSelection();
            Repaint();
        }
        return true;
    }

    DK_POS pos(0, 0);
    bool actionOfGallery = ActionOfGallery(moveEvent1, moveEvent2, pos);

    direction = TransformFlingDirection(direction, GetRotationDegree());
    switch (direction)
    {
    case FD_UP:
        if (ShouldTreatUpDownAsPageSwitch())
        {
            if (!(actionOfGallery && GotoNextFrameInGallery(pos)))
            {
                GotoNextPage();
            }
        }
        else
        {
            //DebugPrintf(DLC_GESTURE, "UIBookReaderContainer::OnFling up, goto next chapter");
            GotoChapter(GotoNextChapter);
        }
        break;
    case FD_DOWN:
        if (ShouldTreatUpDownAsPageSwitch())
        {
            if (!(actionOfGallery && GotoPreFrameInGallery(pos)))
            {
                GotoPrePage();
            }
        }
        else
        {
            //DebugPrintf(DLC_GESTURE, "UIBookReaderContainer::OnFling down, goto prev chapter");
            GotoChapter(GotoPrevChapter);
        }
        break;
    case FD_LEFT:
        //DebugPrintf(DLC_GESTURE, "UIBookReaderContainer::OnFling left, goto next page");
        if (IsVerticalRTL())
        {
            if (!(actionOfGallery && GotoPreFrameInGallery(pos)))
            {
                IsScannedPdf() ? GotoNextPage() : GotoPrePage();
            }
        }
        else
        {
            if (!(actionOfGallery && GotoNextFrameInGallery(pos)))
            {
                GotoNextPage();
            }
        }
        break;
    case FD_RIGHT:
        //DebugPrintf(DLC_GESTURE, "UIBookReaderContainer::OnFling right, goto prev page");
        if (IsVerticalRTL())
        {
            if (!(actionOfGallery && GotoNextFrameInGallery(pos)))
            {
                IsScannedPdf() ? GotoPrePage() : GotoNextPage();
            }
        }
        else
        {
            if (!(actionOfGallery && GotoPreFrameInGallery(pos)))
            {
                GotoPrePage();
            }
        }
        break;
    default:
        return false;
    }
    return true;
}

bool UIBookReaderContainer::MoveSelectionImage(int x, int y)
{
    const int offset = GetWindowMetrics(UIPixelValue15Index);
    m_rightSelecter = true;
    if (m_imageSelectedStart.IsVisible())
    {
        const int downX = m_imageSelectedStart.GetAbsoluteX();
        const int downY = m_imageSelectedStart.GetAbsoluteY();
        const int downLeft = (downX > offset) ? downX - offset : 0;
        const int downRight = downX + m_imageSelectedStart.GetWidth() + offset;
        const int downTop = (downY > offset) ? downY - offset : 0;
        const int downBottom = downY + m_imageSelectedStart.GetHeight() + offset;
        if (downLeft <= x && x < downRight && downTop <= y && y < downBottom)
        {
            m_rightSelecter = false;
            return true;
        }
    }

    if (m_imageSelectedEnd.IsVisible())
    {
        const int upX = m_imageSelectedEnd.GetAbsoluteX();
        const int upY = m_imageSelectedEnd.GetAbsoluteY();
        const int upLeft = (upX > offset) ? upX - offset : 0;
        const int upRight = upX + m_imageSelectedEnd.GetWidth() + offset;
        const int upTop = (upY > offset) ? upY - offset : 0;
        const int upBottom = upY + m_imageSelectedEnd.GetHeight() + offset;
        if (upLeft <= x && x < upRight && upTop <= y && y < upBottom)
        {
            return true;
        }
    }
    return false;
}

DK_RECT UIBookReaderContainer::GetRectOfSelection()
{
    DK_RECT rect;
    rect.left = rect.top = rect.right = rect.bottom = 0;
    if (m_isSelecting && IsInSelectionProgress())
    {
        DK_FLOWPOSITION startPos = m_clsSelected.GetStartPos();
        DK_FLOWPOSITION endPos = m_clsSelected.GetEndPos();
        std::vector<DK_RECT> rects;
        m_clsSelected.GetRectsBetweenPos(startPos, endPos, &rects);
        int length = rects.size();
        if (length > 0)
        {
            rect = rects[0];
            for (int i = 1; i < length; ++i)
            {
                DK_RECT tmpRect = rects[i];
                rect.left = dk_min(rect.left, tmpRect.left);
                rect.top = dk_min(rect.top, tmpRect.top);
                rect.right = dk_max(rect.right, tmpRect.right);
                rect.bottom = dk_max(rect.bottom, tmpRect.bottom);
            }
        }
    }
    return rect;
}

int UIBookReaderContainer::PointInSelectionRects(const DK_POS& point)
{
    if (m_isSelecting && IsInSelectionProgress())
    {
        DK_FLOWPOSITION startPos = m_clsSelected.GetStartPos();
        DK_FLOWPOSITION endPos = m_clsSelected.GetEndPos();
        std::vector<DK_RECT> rects;
        m_clsSelected.GetRectsBetweenPos(startPos, endPos, &rects);
        for (size_t i = 0; i < rects.size(); ++i)
        {
            const DK_RECT& rect = rects[i];
            if (UIUtility::PosInRect(rect, point))
            {
                return SELECT_INAREA;
            }
        }
    }
    return SELECT_OUTAREA;
}

int UIBookReaderContainer::PointInSelection(const DK_POS& point)
{
    int result = SELECT_OUTAREA;

    if (!IsWritingLTR())
    {
        return result;
    }
    if (m_isSelecting && IsInSelectionProgress())
    {
        const int pointX = (int)point.X;
        const int pointY = (int)point.Y;
        const DK_RECT& startRect = m_clsSelected.GetStartRect();
        const DK_RECT& endRect = m_clsSelected.GetEndRect();
        if (CUtility::IsEmptyRect(startRect))
        {
            if ((IsVerticalRTL() && (pointX >= endRect.right || (pointX >= endRect.left && pointY < endRect.bottom)))
                || (IsVerticalLTR() && (pointX < endRect.left || (pointX < endRect.right && pointY >= endRect.bottom)))
                || (IsHorizontalHTB() && (pointY < endRect.top || (pointY < endRect.bottom && pointX < endRect.right)))
                || (!IsVerticalForms() && !IsHorizontalHTB() && (pointY >= endRect.bottom || (pointY >= endRect.top && pointX >= endRect.left))))
            {
                result |= SELECT_INAREA | SELECT_RIGHT;
            }
        }
        else if (CUtility::IsEmptyRect(endRect))
        {
            if ((IsVerticalRTL() && (pointX < startRect.left || (pointX < startRect.right && pointY >= startRect.top)))
                || (IsVerticalLTR() && (pointX >= startRect.right || (pointX >= startRect.left && pointY < startRect.top)))
                || (IsHorizontalHTB() && (pointY >= startRect.bottom || (pointY >= startRect.top && pointX >= startRect.left)))
                || (!IsVerticalForms() && !IsHorizontalHTB() && (pointY < startRect.top || (pointY < startRect.bottom && pointX <= startRect.right))))
            {
                result |= SELECT_INAREA | SELECT_LEFT;
            }
        }
        else
        {
            DK_RECT rect = GetRectOfSelection();
            bool startEqualEnd = UIUtility::IsEqual(startRect, endRect);
            bool posInRect = UIUtility::PosInRect(rect, point);
            if (IsVerticalRTL())
            {
                if (posInRect)
                {
                    if (startEqualEnd || (pointX >= endRect.right && pointX < startRect.left))
                    {
                        const int mid = startEqualEnd ? ((rect.bottom + rect.top) >> 1) : ((endRect.right + startRect.left) >> 1);
                        const int left = startEqualEnd ? pointY : pointX;
                        result |= SELECT_INAREA;
                        result |= (left >= mid) ? SELECT_LEFT : SELECT_RIGHT;
                    }
                    else if (pointX < endRect.right && pointY < endRect.bottom)
                    {
                        result |= SELECT_INAREA | SELECT_RIGHT;
                    }
                    else if (pointX >= startRect.left && pointY >= startRect.top)
                    {
                        result |= SELECT_INAREA | SELECT_LEFT;
                    }
                }
                else if (!startEqualEnd)
                {
                    if (/*DkFloatEqual(rect.top, m_iTop) && */pointY < rect.top)
                    {
                        result |= SELECT_INAREA | SELECT_RIGHT;
                    }
                    else if (/*DkFloatEqual(rect.bottom, GetBottom()) && */pointY > rect.bottom)
                    {
                        result |= SELECT_INAREA | SELECT_LEFT;
                    }
                }
            }
            else if (IsVerticalLTR())
            {
                if (posInRect)
                {
                    if (startEqualEnd || (pointX >= startRect.right && pointX < endRect.left))
                    {
                        const int mid = startEqualEnd ? ((rect.bottom + rect.top) >> 1) : ((startRect.right + endRect.left) >> 1);
                        const int left = startEqualEnd ? pointY : pointX;
                        result |= SELECT_INAREA;
                        result |= (left <= mid) ? SELECT_LEFT : SELECT_RIGHT;
                    }
                    else if (pointX < startRect.right && pointY < startRect.bottom)
                    {
                        result |= SELECT_INAREA | SELECT_LEFT;
                    }
                    else if (pointX >= endRect.left && pointY >= endRect.top)
                    {
                        result |= SELECT_INAREA | SELECT_RIGHT;
                    }
                }
                else if (!startEqualEnd)
                {
                    if (/*DkFloatEqual(rect.top, m_iTop) && */pointY < rect.top)
                    {
                        result |= SELECT_INAREA | SELECT_RIGHT;
                    }
                    else if (/*DkFloatEqual(rect.bottom, GetBottom()) && */pointY > rect.bottom)
                    {
                        result |= SELECT_INAREA | SELECT_LEFT;
                    }
                }
            }
            else
            {
                bool isHTB = IsHorizontalHTB();
                if (posInRect)
                {
                    if (startEqualEnd
                        || (isHTB 
                            ? (pointY >= startRect.bottom && pointY < endRect.top)
                            : (pointY >= endRect.bottom && pointY < startRect.top)))
                    {
                        const int mid = startEqualEnd ? ((rect.left + rect.right) >> 1) : ((startRect.bottom + endRect.top) >> 1);
                        const int left = startEqualEnd ? pointX : pointY;
                        result |= SELECT_INAREA;
                        if (isHTB)
                        {
                            result |= (left <= mid) ? SELECT_LEFT : SELECT_RIGHT;
                        }
                        else
                        {
                            result |= (left <= mid) ? SELECT_RIGHT : SELECT_LEFT;
                        }
                    }
                    else if (isHTB
                        ? (pointY < startRect.bottom && pointX >= startRect.left)
                        : (pointY >= startRect.top && pointX <= startRect.right))
                    {
                        result |= SELECT_INAREA | SELECT_LEFT;
                    }
                    else if (isHTB
                        ? (pointY >= endRect.top && pointX < endRect.right)
                        : (pointY < endRect.bottom && pointX >= endRect.left))
                    {
                        result |= SELECT_INAREA;
                        result |= isHTB ? SELECT_RIGHT : SELECT_LEFT;
                    }
                }
                else if (!startEqualEnd)
                {
                    if (/*DkFloatEqual(rect.left, m_iLeft) && */pointX < rect.left)
                    {
                        result |= SELECT_INAREA;
                        result |= isHTB ? SELECT_LEFT : SELECT_RIGHT;
                    }
                    else if (/*DkFloatEqual(rect.right, GetRight()) && */pointX > rect.right)
                    {
                        result |= SELECT_INAREA;
                        result |= isHTB ? SELECT_RIGHT : SELECT_LEFT;
                    }
                }
            }
        }
    }
    return result;
}

void UIBookReaderContainer::HideSelectionImage(bool updateWindow)
{
    m_imageSelectedEnd.SetVisible(FALSE);
    m_imageSelectedStart.SetVisible(FALSE);
    if (updateWindow)
    {
        CDisplay* pDisplay = CDisplay::GetDisplay();
        if (pDisplay)
        {
            pDisplay->StopFullRepaintOnce();
        }
        CDisplay::CacheDisabler cache;
        UpdateWindow();
    }
}

bool UIBookReaderContainer::OnDown(MoveEvent* moveEvent)
{
    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "UIBookReaderContainer::OnDown m_isSelecting = %d", m_isSelecting);
    m_scaleFlag = WAIT_FOR_SCALE;
    m_vHighlightRects.clear();
    m_pdfZoomMoved = false;
    m_gestureDetector.ResetHoldingTimeOut();
    if (IsInSelectionProgress())
    {
        DebugPrintf(DLC_UIBOOKREADERCONTAINER, "UIBookReaderContainer::OnDown IsInSelectionProgress()");
        int result = PointInSelection(DK_POS(moveEvent->GetX(), moveEvent->GetY()));
        m_downInSelectionImage = MoveSelectionImage(moveEvent->GetX(), moveEvent->GetY());
        if ((result & SELECT_INAREA) || m_downInSelectionImage)
        {
            DebugPrintf(DLC_UIBOOKREADERCONTAINER, "UIBookReaderContainer::OnDown MoveSelectionImage true");
            m_clsSelected.ResetDrawMode();
            m_clsSelected.SetDrawMode(UIBookReaderSelected::DRAW_ALL_SELECTION);
            if (result & SELECT_INAREA)
            {
                m_rightSelecter = (result & SELECT_RIGHT);
            }
        }
        else
        {
            m_clsSelected.SetSelectionMode(false);
        }
        HideSelectionImage();
    }
    return true;
}

bool UIBookReaderContainer::OnUp(MoveEvent* moveEvent)
{
    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "UIBookReaderContainer::OnUp m_isSelecting = %d", m_isSelecting);

    if (IsInSelectionProgress() && m_isSelecting)
    {
        ShowCommentOrSummary();
        return true;
    }
    else if (IsZoomedPdf())
    {
        DebugPrintf(DLC_GESTURE, "%s, %d, %s, %s IsZoomedPdf", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return (WAIT_FOR_SCALE != m_scaleFlag) || (m_pdfZoomMoved && FinishZoomPdf());
    }
    return false;
}

bool UIBookReaderContainer::ValidCommentDlgTopAndLeft(int& left, int& top)
{
    UICommentOrSummary* pCommentOrSummary = UICommentOrSummary::GetInstance();
    if (pCommentOrSummary)
    {
        const dkSize minSize = pCommentOrSummary->GetMinSize();
        const DK_RECT startRect = m_clsSelected.GetStartRect();
        const DK_RECT endRect = m_clsSelected.GetEndRect();
        bool dlgVertical = pCommentOrSummary->IsDirectionVertical();
        if (IsVerticalForms())
        {
            if (dlgVertical)
            {
                if (IsVerticalRTL())
                {
                    const int dlgWidth = minSize.GetWidth();
                    const int offset = GetWindowMetrics(UIPixelValue10Index);
                    if (CUtility::IsEmptyRect(startRect))
                    {
                        left = endRect.left - offset - dlgWidth;
                        if (left <= 0)
                        {
                            left = endRect.right + offset;
                        }
                    }
                    else
                    {
                        left = startRect.right + offset;
                        if (left + dlgWidth > GetRight())
                        {
                            if (CUtility::IsEmptyRect(endRect))
                            {
                                left = startRect.left - offset - dlgWidth;
                            }
                            else
                            {
                                left = endRect.left - offset - dlgWidth;
                                if (left <= 0)
                                {
                                    left = (startRect.right + endRect.left) >> 1;
                                }
                            }
                        }
                    }
                }
                else //if (IsVerticalLTR())
                {
                    const int dlgWidth = minSize.GetWidth();
                    const int offset = GetWindowMetrics(UIPixelValue10Index);
                    if (CUtility::IsEmptyRect(startRect))
                    {
                        left = endRect.right + offset;
                        if (left + dlgWidth > GetRight())
                        {
                            left = endRect.left - offset - dlgWidth;
                        }
                    }
                    else
                    {
                        left = startRect.left - offset - dlgWidth;
                        if (left <= 0)
                        {
                            if (CUtility::IsEmptyRect(endRect))
                            {
                                left = startRect.right + offset;
                            }
                            else
                            {
                                left = endRect.right + offset;
                                if (left + dlgWidth > GetRight())
                                {
                                    left = (startRect.left + endRect.right) >> 1;
                                }
                            }
                        }
                    }
                }
            }
            else 
            {
                const int dlgHeight = minSize.GetHeight();
                const int offset = GetWindowMetrics(UIPixelValue60Index);
                if (CUtility::IsEmptyRect(endRect))
                {
                    top = startRect.top - offset - dlgHeight;
                    if (top <= 0)
                    {
                        top = startRect.bottom + offset;
                    }
                }
                else 
                {
                    top = endRect.bottom + offset;
                    if (top + dlgHeight >= GetBottom())
                    {
                        top = endRect.top - offset - dlgHeight;
                    }
                }
            }
        }
        else
        {
            if (IsHorizontalHTB())
            {
                const int dlgHeight = minSize.GetHeight();
                const int offset = GetWindowMetrics(UIPixelValue10Index);
                if (CUtility::IsEmptyRect(startRect))
                {
                    top = endRect.bottom + offset;
                    if (top + dlgHeight >= GetBottom())
                    {
                        top = endRect.top - offset - dlgHeight;
                    }
                }
                else
                {
                    top = startRect.top - offset - dlgHeight;
                    if (top <= 0)
                    {
                        if (CUtility::IsEmptyRect(endRect))
                        {
                            top = startRect.bottom + offset;
                        }
                        else
                        {
                            top = endRect.bottom + offset;
                            if (top + dlgHeight >= GetBottom())
                            {
                                top = (startRect.bottom + endRect.top) >> 1;
                            }
                        }
                    }
                }
            }
            else
            {
                const int dlgHeight = minSize.GetHeight();
                const int offset = GetWindowMetrics(UIPixelValue10Index);
                if (CUtility::IsEmptyRect(startRect))
                {
                    top = endRect.top - offset - dlgHeight;
                    if (top < 0)
                    {
                        top = endRect.bottom + offset;
                    }
                }
                else
                {
                    top = startRect.bottom + offset;
                    if (top + dlgHeight >= GetBottom())
                    {
                        if (CUtility::IsEmptyRect(endRect))
                        {
                            top = startRect.top - offset - dlgHeight;
                        }
                        else
                        {
                            top = endRect.top - offset - dlgHeight;
                            if (top <= 0)
                            {
                                top = (startRect.bottom + endRect.top) >> 1;
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}

bool UIBookReaderContainer::ShowCommentOrSummary()
{
    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "UIBookReaderContainer::ShowCommentOrSummary");
    if (IsInSelectionProgress())
    {
        DebugPrintf(DLC_UIBOOKREADERCONTAINER, "UIBookReaderContainer::ShowCommentOrSummary IsInSelectionProgress()");
        m_clsSelected.ValidPositionAndRects();
        DK_POS pos = m_clsSelected.GetEndPoint();
        const int x = (int)(pos.X + 0.5);
        const int y = (int)(pos.Y + 0.5);
        string strUserComment = "";
        if (m_clsSelected.GetSelection(&m_clsStartSelected, &m_clsEndSelected, m_strSelected))
        {
            m_posJumpLink = pos;
            m_pBookReader->CompletePosRange(&m_clsStartSelected, &m_clsEndSelected);
            UICommentOrSummary* pCommentOrSummary = UICommentOrSummary::GetInstance();
            if (pCommentOrSummary)
            {
                pCommentOrSummary->SetInGallery(m_clsSelected.CheckInGallery(m_clsSelected.GetStartPoint()));
                bool isInternalLink = true;
                pCommentOrSummary->SetIsLink(m_clsSelected.CheckIsLink(m_posJumpLink, isInternalLink));
                pCommentOrSummary->SetIsDuokanBook(m_pBookReader->IsPurchasedDuoKanBook());
                AppendChild(pCommentOrSummary);

                string strTranslate = StringUtil::Trim(m_strSelected.c_str());
                bool hasComment = false;
                bool hasDigest = false;
                m_iCurCommentIndex = GetCommentDigestIndex((CT_RefPos)m_clsStartSelected, (CT_RefPos)m_clsEndSelected, ICT_ReadingDataItem::COMMENT, hasComment);
                hasComment = (m_iCurCommentIndex != -1);
                m_iCurDigestIndex = GetCommentDigestIndex((CT_RefPos)m_clsStartSelected, (CT_RefPos)m_clsEndSelected, ICT_ReadingDataItem::DIGEST, hasDigest);
                pCommentOrSummary->SetShowStatusAndText( hasComment, hasDigest, strTranslate.c_str());
                pCommentOrSummary->SetVerticalDirection(IsVerticalForms());
                pCommentOrSummary->SetVisible(true);
                int dlgX = x, dlgY = y;
                ValidCommentDlgTopAndLeft(dlgX, dlgY);
                pCommentOrSummary->SetOffsetXY(dlgX, dlgY, m_iWidth, m_iHeight);

                m_imageSelectedEnd.SetVisible(TRUE);
                m_imageSelectedStart.SetVisible(TRUE);
                CDisplay* pDisplay = CDisplay::GetDisplay();
                if (pDisplay)
                {
                    pDisplay->StopFullRepaintOnce();
                    m_clsSelected.ResetDrawMode();
                    m_clsSelected.SetDrawMode(UIBookReaderSelected::DRAW_ALL_SELECTION);
                }
                Repaint();
            }
        }
        return true;
    }
    return false;
}

void UIBookReaderContainer::UIBookReaderContainerGestureListener::OnHolding(int holdingX, int holdingY)
{
    m_pBookReaderContainer->OnHolding(holdingX, holdingY);
}

void UIBookReaderContainer::OnHolding(int holdingX, int holdingY)
{
    DebugPrintf(DLC_UIBOOKREADERCONTAINER, "UIBookReaderContainer::OnHolding");
    if (WAIT_FOR_SCALE != m_scaleFlag || !m_pBookReader)
    {
        return;
    }

    if (IsInSelectionProgress()
        && m_isSelecting
        && !m_clsSelected.SelectingInGallery())
    {
        DebugPrintf(DLC_UIBOOKREADERCONTAINER, "UIBookReaderContainer::OnHolding IsInSelectionProgress() && m_isSelecting");
        m_gestureDetector.SetHoldingTimeOut(2000);
        const int DISTANCE_CHECK = 100;
        int prevX = 0, prevY = 0;
        int nextX = m_iWidth, nextY = m_iHeight;
        if (IsVerticalRTL())
        {
            prevX = m_iWidth;
            nextX = 0;
        }
        else if (IsVerticalLTR())
        {
            prevY = m_iHeight;
            nextY = 0;
        }
        else if (!IsHorizontalHTB())
        {
            nextX = 0, nextY = 0;
            prevX = m_iWidth, prevY = m_iHeight;
        }
        bool process = false;
        if (IsPointNeighbour(holdingX, holdingY, prevX, prevY, DISTANCE_CHECK)
            && !m_pBookReader->IsFirstPageInChapter())
        {
            DebugPrintf(DLC_GESTURE, "Near GotoPrePage %d, %d", prevX, prevY);
            if (BOOKREADER_GOTOPAGE_SUCCESS == GotoPrePage())
            {
                m_clsSelected.MoveToEndOfPage();
                process = true;
            }
        }
        else if (IsPointNeighbour(holdingX, holdingY, nextX, nextY, DISTANCE_CHECK)
            && !m_pBookReader->IsLastPageInChapter())
        {
            DebugPrintf(DLC_GESTURE, "Near GotoNextPage %d, %d", nextX, nextY);
            if (BOOKREADER_GOTOPAGE_SUCCESS == GotoNextPage())
            {
                m_clsSelected.MoveToBeginOfPage();
                process = true;
            }
        }
        if (process)
        {
            HitTestAndMove(GESTURE_MOVE_LEFT, holdingX, holdingY);
            m_processMove = false;
        }
    }
}

bool UIBookReaderContainer::IsSupportedZoomedPara() const
{
    return IsScannedPdf() 
        && (0 == m_modeController.m_iRotation) 
        && (PDF_CEM_Normal == m_modeController.m_eCuttingEdgeMode) 
        && (PDF_RM_NormalPage == m_modeController.m_eReadingMode || PDF_RM_ZoomPage == m_modeController.m_eReadingMode);
}

bool UIBookReaderContainer::DrawInteractiveImageSymbol(DK_IMAGE drawingImg)
{
    SPtr<ITpImage> zoomedOut = ImageManager::GetImage(IMAGE_ICON_ZOOMOUT);
    const int symbolWidth = zoomedOut->GetWidth();
    const int symbolHeight = zoomedOut->GetHeight();
    for (unsigned int i=0; i<m_vPageInteractiveObjects.size(); ++i)
    {
        const PageObjectInfo& interactiveObject = m_vPageInteractiveObjects[i];
        switch(interactiveObject.m_objectType)
        {
        case DKE_PAGEOBJ_INTERACTIVEIMAGE:
        case DKE_PAGEOBJ_GALLERY:
        case DKE_PAGEOBJ_PREBLOCK:
            {
                DK_IMAGE imgSelf;
                DK_RECT bounding = interactiveObject.m_bounding;
                DK_RECT rcSelf = 
                {bounding.right - symbolWidth, bounding.bottom - symbolHeight, bounding.right, bounding.bottom};
                if (S_OK == CropImage(drawingImg, rcSelf, &imgSelf))
                {
                    CTpGraphics grf(imgSelf);
                    grf.DrawImageBlend(zoomedOut.Get(), 0, 0, 0, 0, symbolWidth, symbolHeight);
                }
            }
            break;
        default:
            break;
        }
    }
    return true;
}

bool UIBookReaderContainer::DrawDotProgress(DK_IMAGE drawingImg)
{
    for (size_t i = 0; i < m_vDotProgress.size(); ++i)
    {
        if (m_vDotProgress[i])
        {
            m_vDotProgress[i]->Draw(drawingImg);
        }
    }
    return true;
}

void UIBookReaderContainer::InitPageObjectsInCurPage()
{
    ClearPageObjectsInCurPage();
	BookTextController *pSelectedControler = m_pBookReader->GetBookTextControlerOfCurPage();
	if (!m_clsSelected.Initialize(pSelectedControler))
    {
        DebugPrintf(DLC_UIBOOKREADERCONTAINER, "%s, %d, %s, %s Initialize error", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return;
    }
	InitSelectBasePoint();
	if (m_eFormat == DFF_ElectronicPublishing)
	{
        m_clsSelected.InitInteractiveObjects(m_vPageInteractiveObjects);
        m_clsSelected.InitGalleryObjects(m_vGalleryObjects);
        InitDotProgress();
    }
}

void UIBookReaderContainer::InitDotProgress()
{
    m_vDotProgress.clear();
    for (size_t i = 0; i < m_vGalleryObjects.size(); ++i)
    {
        const GalleryObjectInfo& galleryObj = m_vGalleryObjects[i];
        UIDotProgress* dotProgress = new UIDotProgress;
        if (dotProgress)
        {
            const DK_RECT& bounding = galleryObj.m_bounding;
            const int cellCount = galleryObj.m_cellCount;
            const int cellIndex = galleryObj.m_activeCellIndex;
            dotProgress->SetProgress(cellIndex, cellCount);
            const int width = bounding.right - bounding.left;
            const int height = dotProgress->GetMinHeight();
            const int left = bounding.left;
            const int top = bounding.bottom - height;
            dotProgress->MoveWindow(left, top, width, height);
            m_vDotProgress.push_back(UIDotProgressSPtr(dotProgress));
        }
        else
        {
            m_vDotProgress.push_back(UIDotProgressSPtr());
        }
    }
}

void UIBookReaderContainer::ClearPageObjectsInCurPage()
{
    m_vPageInteractiveObjects.clear();
    m_iCurrentInteractivePageObjectIndex = -1;

    m_vGalleryObjects.clear();
    m_vDotProgress.clear();
    m_dotProgressFocusIndex = -1;
}

bool UIBookReaderContainer::HitOnBookMarkShortCutArea(int x, int y)
{
    const int bookMarkAreaSize = WindowsMetrics::GetWindowMetrics(BookMarkShortCutAreaSizeIndex);
    DK_INT curRotation = 0;

    //TODO:@zhangying，rotation不为0时,书签的位置仍在右上角
    if (IsScannedPdf())
    {
        curRotation = m_modeController.m_iRotation;
    }
    bool isInBookMarkShortCutArea = false;
    switch (curRotation)
    {
        case 0:
            if (IsVerticalRTL())
            {
                isInBookMarkShortCutArea = (x >= 0 && x <= bookMarkAreaSize && y >= 0 && y <= bookMarkAreaSize);
            }
            else
            {
                isInBookMarkShortCutArea = (x >= (m_iWidth - bookMarkAreaSize) && x <= m_iWidth && y >= 0 && y <= bookMarkAreaSize);
            }
            break;
        case 180:
            isInBookMarkShortCutArea = (x >= 0 && x <= bookMarkAreaSize && y >= (m_iHeight - bookMarkAreaSize) && y <= m_iHeight);
            break;
        case 270:
            isInBookMarkShortCutArea = (x >= 0 && x <= bookMarkAreaSize && y >= 0 && y <= bookMarkAreaSize);
            break;
        case 90:
            isInBookMarkShortCutArea = (x >= (m_iWidth - bookMarkAreaSize) && x <= m_iWidth && y >= (m_iHeight - bookMarkAreaSize) && y <= m_iHeight);
            break;
        default:
            break;
    }

    if (isInBookMarkShortCutArea)
    {
        SwitchBookMarkOffOn();
    }

    return isInBookMarkShortCutArea;
}

bool UIBookReaderContainer::IsComicsChapter() const
{
    ATOM posCurPageAtom, endAtom;
    if (m_pBookReader && m_pBookReader->GetCurrentPageATOM(posCurPageAtom, endAtom))
    {
        return m_pBookReader->IsComicsChapter(posCurPageAtom.GetChapterIndex());
    }

    return false;
}

void UIBookReaderContainer::OnReadLocationChanged()
{
    InitPageObjectsInCurPage();
    m_clsReaderProgressBar.SetVisible(!IsComicsChapter() && m_pBookReader && !m_pBookReader->IsCurrentPageFullScreen());
}

bool UIBookReaderContainer::SyncDKXBlock()
{
	if(m_dkxBlock)
	{
		m_dkxBlock->UnSerialize();
	}
	return true;
}

