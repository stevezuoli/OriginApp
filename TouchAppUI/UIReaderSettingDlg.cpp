#include "SQM.h"
#include "Utility.h"
#include "DkStreamFactory.h"
#include "BookReader/TextBookReader.h"
#include "BookReader/MobiBookReader.h"
#include "BookReader/EpubBookReader.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UISizer.h"
#include "CommonUI/CPageNavigator.h"
#include "TouchAppUI/UIButtonDlg.h"
#include "TouchAppUI/UIBookReaderContainer.h"
#include "TouchAppUI/UINavigateToDlg.h"
#include "TouchAppUI/UIReaderSettingDlg.h"
#include "TouchAppUI/UIPdfTrimColumnedDlg.h"
#include "TouchAppUI/DkReaderPage.h"
#include "I18n/StringManager.h"
#include "Framework/CDisplay.h"
#include "Framework/KernelGear.h"
#include "TTS/TTS.h"
#include "Utility/ThreadHelper.h"
#include "drivers/DeviceInfo.h"
#include "Common/WindowsMetrics.h"
#include "TouchAppUI/UIFrontLightManager.h"
#include "GUI/GUISystem.h"
#include "CommonUI/UIIMEManager.h"
#include "TouchAppUI/UINumInput.h"
#include <tr1/functional>

using namespace WindowsMetrics;
using namespace dk::utility;

#define UIREADERSETTING_BORDER          1
static const int maxBoldenValue = 2;

UIReaderSettingDlg::UIReaderSettingDlg(
        UIContainer* pContainer,
        IBookReader* pBookReader,
        DKXBlock* dkxBlock,
        const char* title,
        const char* strFilePath,
        DkFileFormat format) :
    UIDialog(NULL) ,
    m_pBookReader(pBookReader) ,
    m_bIsScannedText(DFF_PortableDocumentFormat==format) ,
    m_bIsImage(NULL == pBookReader) ,
    m_bBookMarkExists(false) ,
    m_strBookName(title) ,
    m_strFilePath(strFilePath) ,
    m_btnBack(this),
    m_jumpPageNum(this, ID_BTN_TOUCH_NAVIGATETO),
    m_splitLine(this),
    m_clsReaderProgressBar(this, IDSTATIC),
    m_pPdfTrimColumnedDlg(NULL) ,
    m_pNavigateToDlg(NULL) ,
    m_iSelectedCNFontIndex(-1) ,
    m_iSelectedENFontIndex(-1) ,
    m_bIsTTSPlaying(FALSE) ,
    m_bIsPdfTextMode(FALSE) ,
    m_iCurPdfEmBoldenLevel(0) ,
    m_curFontStyle(0),
    m_iCurReadingLayout(0),
    m_eLastReadingMode(PDF_RM_NormalPage) ,
    m_bIsRearRangeStatusChanged(false) ,
    m_iGotoPageNum(0) ,
    m_bIsNothingChanged(true) ,
    m_endFlag(Invalid) ,
    //TODO, for imagereader, initiliazation of the var below may be wrong.
    m_eFormat(format) ,
    m_fontsize20(GetWindowFontSize(FontSize20Index)) ,
    m_tidParseBook(0) ,
    m_bIsPreviewImageReady(true) ,
    m_pReadingProcess(NULL),
    m_bIsPdfRearRanging(false),
    m_dkxBlock(dkxBlock),
    m_pContainer(pContainer),
    m_imageController(NULL)
{
	SubscribeMessageEvent(UIBookParseStatusListener::EventBookParseFinished,
			*(UIBookParseStatusListener::GetInstance()),
			std::tr1::bind(
			std::tr1::mem_fn(&UIReaderSettingDlg::OnParseFinishedEvent),
			this, std::tr1::placeholders::_1));
    SetClickable(true);
	if(m_bIsImage && m_pContainer)
	{
		UIImageReaderContainer* imageReadContainer = dynamic_cast<UIImageReaderContainer*>(m_pContainer);
		m_imageController = imageReadContainer->GetImgControler();
	}
    LoadReaderSettingInfo();
    InitUI();//child window initialized
    InitFont();
}

UIReaderSettingDlg::~UIReaderSettingDlg()
{
    SafeDeletePointer(m_pPdfTrimColumnedDlg);
    SafeDeletePointer(m_pNavigateToDlg);
    SafeDeletePointer(m_pReadingProcess);
}

void UIReaderSettingDlg::InitTopWindows()
{
    AppendChild(&m_titleBar);

    if(DeviceInfo::IsKPW())
    {
        m_btnLight.Initialize(ID_BTN_TOUCH_READER_SETTING_LIGHT, "", m_fontsize20);
        m_btnLight.SetIcon(ImageManager::GetImage(IMAGE_KPW_ICON_FRONTLIGHT), UIButton::ICON_LEFT);
        m_btnLight.SetElemSpacing(0);
        if(IsComicsChapter())
        {
            m_btnLight.SetCornerStyle(true, true, true, true);
        }
        else
        {
            m_btnLight.SetCornerStyle(false, true, true, false);
        }
    }

    m_btnTOC.Initialize(ID_BTN_TOUCH_TOC, "", m_fontsize20);
    m_btnTOC.SetIcon(ImageManager::GetImage(IMAGE_TOUCH_ICON_TOC), UIButton::ICON_LEFT);
    m_btnTOC.SetElemSpacing(0);
    m_btnTOC.SetCornerStyle(true, true, true, true);

    m_btnComicsFrame.Initialize(ID_BTN_COMICSFRAMEMODE, "", m_fontsize20);
    m_btnComicsFrame.SetElemSpacing(0);
    m_btnComicsFrame.SetCornerStyle(false, true, true, false);
    if (IsComicsFrameMode())
    {
        m_btnComicsFrame.SetIcon(ImageManager::GetImage(IMAGE_ICON_COMICSFRAME_OFF), UIButton::ICON_LEFT);
    }
    else 
    {
        m_btnComicsFrame.SetIcon(ImageManager::GetImage(IMAGE_ICON_COMICSFRAME_ON), UIButton::ICON_LEFT);
    }

    m_btnSearch.Initialize(ID_BTN_TOUCH_CONTENT_SEARCH, "", m_fontsize20);
    m_btnSearch.SetIcon(ImageManager::GetImage(IMAGE_ICON_SEARCH), UIButton::ICON_LEFT);
    m_btnSearch.SetDisabledIcon(ImageManager::GetImage(IMAGE_ICON_SEARCH_DISABLE));
    m_btnSearch.SetElemSpacing(0);

    if (m_bIsImage)
    {
        m_btnSearch.SetEnable(false);
    }

    if(DeviceInfo::IsKPW() || IsComicsChapter())
    {
        m_btnSearch.SetCornerStyle(true, true, true, true);
    }
    else
    {
        m_btnSearch.SetCornerStyle(false, true, true, false);
    }

    m_btnBookMark.Initialize(ID_BTN_TOUCH_ADD_BOOKMARK, "", m_fontsize20);
    m_btnBookMark.SetIcon(ImageManager::GetImage(IMAGE_TOUCH_ICON_BOOKMARK), UIButton::ICON_LEFT);
    m_btnBookMark.SetElemSpacing(0);
    m_btnBookMark.SetCornerStyle(true, false, false, true);

	m_btnBack.SetId(ID_BTN_TOUCH_BOOKSHELF);
	m_btnBack.SetMinHeight(GetWindowMetrics(UIBackButtonHeightIndex));

    AppendChild(&m_btnBack);
    if(DeviceInfo::IsKPW())
    {
        AppendChild(&m_btnLight);
    }
    if(IsComicsChapter())
    {
        AppendChild(&m_btnComicsFrame);
    }
    AppendChild(&m_btnTOC);
    AppendChild(&m_btnSearch);
    AppendChild(&m_btnBookMark);
    
    const int minWidth = GetWindowMetrics(UIReaderSettingDlgButtonWidthIndex);
    const int minHeight = GetWindowMetrics(UIPixelValue40Index);
    if(DeviceInfo::IsKPW())
    {
        m_btnLight.SetMinSize(minWidth, minHeight);
    }
    m_btnComicsFrame.SetMinSize(minWidth, minHeight);
    m_btnSearch.SetMinSize(minWidth, minHeight);
    m_btnTOC.SetMinSize(minWidth, minHeight);
    m_btnBookMark.SetMinSize(minWidth, minHeight);

    m_topSizer->Add(&m_btnBack, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
	m_topSizer->AddStretchSpacer();
    if(IsComicsChapter())
    {
        m_topSizer->Add(&m_btnComicsFrame, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL | dkALIGN_RIGHT));
    }
    if(DeviceInfo::IsKPW())
    {
        m_topSizer->Add(&m_btnLight, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL | dkALIGN_RIGHT));
    }
    m_topSizer->Add(&m_btnSearch, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL | dkALIGN_RIGHT));
    m_topSizer->Add(&m_btnTOC, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL | dkALIGN_RIGHT));
    m_topSizer->Add(&m_btnBookMark, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL | dkALIGN_RIGHT));
}

void UIReaderSettingDlg::InitMiddleVolumeWindows()
{
    m_btnModifyVolume[0].Initialize(ID_BTN_TOUCH_DECRETTSVOLUME, "", m_fontsize20);
    m_btnModifyVolume[0].SetIcons(ImageManager::GetImage(IMAGE_TOUCH_ICON_VOLUME_LESS), ImageManager::GetImage(IMAGE_TOUCH_ICON_VOLUME_LESS_DISABLE),UIButton::ICON_LEFT);
    m_btnModifyVolume[0].SetDisabledIcon(ImageManager::GetImage(IMAGE_TOUCH_ICON_VOLUME_LESS_DISABLE));
    m_btnModifyVolume[1].Initialize(ID_BTN_TOUCH_INCRETTSVOLUME, "", m_fontsize20);
    m_btnModifyVolume[1].SetIcons(ImageManager::GetImage(IMAGE_TOUCH_ICON_VOLUME_ADD), ImageManager::GetImage(IMAGE_TOUCH_ICON_VOLUME_ADD_DISABLE),UIButton::ICON_LEFT);
    m_btnModifyVolume[1].SetDisabledIcon(ImageManager::GetImage(IMAGE_TOUCH_ICON_VOLUME_ADD_DISABLE));

    for (int i = 0; i < 2; ++i)
    {
        AppendChild(m_btnModifyVolume+i);
        m_btnModifyVolume[i].SetMinHeight(GetWindowMetrics(UIPixelValue40Index));
    }
    m_middleSizers[MID_WINDOWS_VOLUME]->Add(m_btnModifyVolume, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
    m_middleSizers[MID_WINDOWS_VOLUME]->AddSpacer(GetWindowMetrics(UIPixelValue32Index));
    m_middleSizers[MID_WINDOWS_VOLUME]->Add(m_btnModifyVolume+1, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
}

void UIReaderSettingDlg::InitMiddleChapterWindows()
{
	const int verticalMargin = GetWindowMetrics(UIReaderSettingDlgMiddleButtonsVerticalMarginIndex);
	const int horizonMargin = GetWindowMetrics(UIHorizonMarginIndex);
	const int btnWidth = GetWindowMetrics(UIReaderSettingDlgButtonWidthIndex);
	const int btnHeight = GetWindowMetrics(UIPixelValue40Index);
	const int textHeight = m_fontsize20 + 3;
    m_btnRollBack.Initialize(ID_BTN_TOUCH_ROLLBACK, "", m_fontsize20);
	m_btnRollBack.SetIcons(ImageManager::GetImage(IMAGE_ICON_GO_TO_LEFT), ImageManager::GetImage(IMAGE_ICON_GO_TO_LEFT), UIButton::ICON_CENTER);
	m_btnRollBack.SetDisabledIcon(ImageManager::GetImage(IMAGE_ICON_GO_TO_LEFT_GREY));
	m_btnRollBack.ShowBorder(false);
	
    m_textCurChapterInfo.SetFontSize(m_fontsize20);
	m_textCurChapterInfo.SetMinSize(dkSize(CDisplay::GetDisplay()->GetScreenWidth() - horizonMargin*2, textHeight));
	
	m_textBookName.SetText(m_strBookName);
    m_textBookName.SetAlign(ALIGN_CENTER);
	m_textBookName.SetMinSize(dkSize(CDisplay::GetDisplay()->GetScreenWidth() - horizonMargin*2, textHeight));
    m_textBookName.SetFontSize(m_fontsize20);

	m_prevChapter.Initialize(ID_BTN_TOUCH_PREVCHAPTER, StringManager::GetStringById(TOUCH_BOOKSETTING_PREV_CHAPTER), m_fontsize20);
	m_prevChapter.SetIcons(ImageManager::GetImage(IMAGE_ICON_GO_LEFT), ImageManager::GetImage(IMAGE_ICON_GO_LEFT), UIButton::ICON_RIGHT);
	m_prevChapter.ShowBorder(false);
	m_prevChapter.SetEnable(!m_bIsImage);
	
	m_nextChapter.Initialize(ID_BTN_TOUCH_NEXTCHAPTER, StringManager::GetStringById(TOUCH_BOOKSETTING_NEXT_CHAPTER), m_fontsize20);
	m_nextChapter.SetIcons(ImageManager::GetImage(IMAGE_ICON_GO_RIGHT), ImageManager::GetImage(IMAGE_ICON_GO_RIGHT), UIButton::ICON_LEFT);
	m_nextChapter.ShowBorder(false);
	m_nextChapter.SetEnable(!m_bIsImage);
	
	m_jumpPageNum.SetFontSize(m_fontsize20);
	m_jumpPageNum.SetMarginRight(10);
	m_jumpPageNum.SetMinWidth(GetWindowMetrics(UIReaderSettingDlgJumpTextBoxWidthIndex));
	if (IsPageInfoShowPercent())
    {
        m_jumpPageNum.SetTextBoxStyle(PERCENTAGE);
    }
    else
    {
        m_jumpPageNum.SetTextBoxStyle(NUMBER_ONLY);
    }
	
	m_totalPageNum.SetFontSize(m_fontsize20);
	m_totalPageNum.SetAlign(ALIGN_LEFT);
	m_totalPageNum.SetFirstLineIndent(-1);
	m_splitLine.SetMinHeight(1);
	
    AppendChild(&m_textCurChapterInfo);
    AppendChild(&m_clsReaderProgressBar);
    AppendChild(&m_btnRollBack);
	AppendChild(&m_textBookName);
	AppendChild(&m_prevChapter);
	AppendChild(&m_nextChapter);
	AppendChild(&m_jumpPageNum);
	AppendChild(&m_totalPageNum);
	AppendChild(&m_splitLine);

    const int spacing = GetWindowMetrics(UIPixelValue20Index);
	m_clsReaderProgressBar.SetEnableScroll(true);
	m_clsReaderProgressBar.SetTransparent(false);
	m_clsReaderProgressBar.SetBarHeight(GetWindowMetrics(UIProgressBarLineHeightIndex)*2);
	m_clsReaderProgressBar.SetTopMargin((btnHeight - m_clsReaderProgressBar.GetBarHeight())/2);
	m_clsReaderProgressBar.SetCallBackFun(RefreshProgress, this);
	int progressBarWidth = CDisplay::GetDisplay()->GetScreenWidth() - btnWidth - horizonMargin*2 - spacing;
	m_clsReaderProgressBar.SetMinSize(dkSize(progressBarWidth, btnHeight));
    m_btnRollBack.SetMinSize(dkSize(btnWidth, btnHeight));

	UISizer* jumpSizer = new UIBoxSizer(dkHORIZONTAL);
	jumpSizer->Add(&m_prevChapter, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL | dkALIGN_LEFT).Border(dkLEFT, horizonMargin));
	jumpSizer->AddStretchSpacer();
	jumpSizer->Add(&m_jumpPageNum, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
	jumpSizer->Add(&m_totalPageNum, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
	jumpSizer->AddStretchSpacer();
	jumpSizer->Add(&m_nextChapter, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL | dkALIGN_RIGHT).Border(dkRIGHT, horizonMargin));
	UISizer* progressSizer = new UIBoxSizer(dkHORIZONTAL);
	progressSizer->Add(&m_clsReaderProgressBar, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
	progressSizer->Add(&m_btnRollBack, UISizerFlags().Border(dkLEFT, spacing).Align(dkALIGN_CENTER_VERTICAL));
	m_middleSizers[MID_WINDOWS_CHAPTER]->Add(jumpSizer, UISizerFlags().Expand().Border(dkDOWN, verticalMargin - 2).Align(dkALIGN_CENTER_VERTICAL));
	m_middleSizers[MID_WINDOWS_CHAPTER]->Add(&m_splitLine, UISizerFlags().Expand());
	m_middleSizers[MID_WINDOWS_CHAPTER]->Add(&m_textCurChapterInfo, UISizerFlags()
		.Border(dkLEFT | dkRIGHT, horizonMargin).Align(dkALIGN_BOTTOM)
		.Border(dkUP, verticalMargin).ReserveSpaceEvenIfHidden());
	m_middleSizers[MID_WINDOWS_CHAPTER]->Add(progressSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizonMargin).Align(dkALIGN_CENTER_VERTICAL));
	m_middleSizers[MID_WINDOWS_CHAPTER]->Add(&m_textBookName, UISizerFlags().Border(dkLEFT | dkRIGHT, horizonMargin).Align(dkALIGN_TOP | dkALIGN_CENTER_HORIZONTAL));
}

void UIReaderSettingDlg::InitMiddleFontSettingWindows()
{
    const int minHeight = GetWindowMetrics(UIPixelValue40Index);
    const int titleFontSize = GetWindowFontSize(FontSize28Index);
    const int tagFontSize = GetWindowFontSize(FontSize24Index);
    
    // LANG SETTING
    m_textLangSettingTitle.SetText(StringManager::GetPCSTRById(TOUCH_BOOKSETTING_MAINTEXT_FONTSETTING));
    m_textLangSettingTitle.SetFontSize(titleFontSize);
    m_textLangSettingTitle.SetAlign(ALIGN_LEFT);
    AppendChild(&m_textLangSettingTitle);
        
    m_textLangChoices[0].SetText(StringManager::GetPCSTRById(CHINESE));
    m_textLangChoices[0].SetFontSize(tagFontSize);
    m_textLangChoices[1].SetText(StringManager::GetPCSTRById(ENGLISH));
    m_textLangChoices[1].SetFontSize(tagFontSize);
    m_btnLangChoices[0].Initialize(ID_BTN_TOUCH_CN, "", m_fontsize20);
    m_btnLangChoices[1].Initialize(ID_BTN_TOUCH_EN, "", m_fontsize20);
    for ( int i=0; i<2; ++i) {
        m_btnLangChoices[i].SetIcons(ImageManager::GetImage(IMAGE_TOUCH_ARROW_BLACK), ImageManager::GetImage(IMAGE_TOUCH_ARROW_WHITE), UIButton::ICON_RIGHT);
        m_btnLangChoices[i].SetMinSize(dkSize(GetWindowMetrics(UIReaderSettingDlgFontSettingButtonWidthIndex), minHeight));
        AppendChild(m_btnLangChoices+i);
        AppendChild(m_textLangChoices+i);
    }

    m_textFontOtherSettingTitle.SetText(StringManager::GetPCSTRById(TOUCH_BOOKSETTING_OTHER_FONTSETTING));
    m_textFontOtherSettingTitle.SetFontSize(titleFontSize);
    m_textFontOtherSettingTitle.SetAlign(ALIGN_LEFT);
    AppendChild(&m_textFontOtherSettingTitle);

    // FONT SIZE SETTING
    m_textFontSize.SetText(StringManager::GetPCSTRById(TOUCH_BOOKSETTING_FONTSIZE));
    m_textFontSize.SetFontSize(tagFontSize);
    m_textFontSize.SetAlign(ALIGN_LEFT);
    AppendChild(&m_textFontSize);

    m_btnModifyFontSize[0].Initialize(ID_BTN_TOUCH_DECREFONTSIZE, "", m_fontsize20);
    m_btnModifyFontSize[0].SetIcons(ImageManager::GetImage(IMAGE_TOUCH_ICON_FONTSIZE_DECRE), ImageManager::GetImage(IMAGE_TOUCH_ICON_FONTSIZE_DECRE_WHITE),UIButton::ICON_LEFT);
    m_btnModifyFontSize[0].SetDisabledIcon(ImageManager::GetImage(IMAGE_TOUCH_ICON_FONTSIZE_DECRE_DISABLE));
    m_btnModifyFontSize[1].Initialize(ID_BTN_TOUCH_INCREFONTSIZE, "", m_fontsize20);
    m_btnModifyFontSize[1].SetIcons(ImageManager::GetImage(IMAGE_TOUCH_ICON_FONTSIZE_INCRE), ImageManager::GetImage(IMAGE_TOUCH_ICON_FONTSIZE_INCRE_WHITE), UIButton::ICON_LEFT);
    m_btnModifyFontSize[1].SetDisabledIcon(ImageManager::GetImage(IMAGE_TOUCH_ICON_FONTSIZE_INCRE_DISABLE));
    for (int i = 0; i < 2; ++i)
    {
        AppendChild(m_btnModifyFontSize+i);
        m_btnModifyFontSize[i].SetMinHeight(minHeight);
    }

    // FONT STYLE SETTING
    m_textFontStyle.SetText(StringManager::GetPCSTRById(FONT_STYLE_GBK_OR_BIG5));
    m_textFontStyle.SetFontSize(tagFontSize);
    m_textFontStyle.SetAlign(ALIGN_LEFT);
    AppendChild(&m_textFontStyle);

    m_btnModifyFontStyle[0].Initialize(ID_BTN_TOUCH_FONTSTYLE_FAMILIARFONT,StringManager::GetPCSTRById(FONT_STYLE_GBK), m_fontsize20);
    m_btnModifyFontStyle[1].Initialize(ID_BTN_TOUCH_FONTSTYLE_TRADITIONALFONT,StringManager::GetPCSTRById(FONT_STYLE_BIG5), m_fontsize20);

    for ( int i=0; i<2; i++) {
        m_btnModifyFontStyle[i].SetMinHeight(minHeight);
        AppendChild(m_btnModifyFontStyle + i);
    }
    m_btnModifyFontStyle[0].SetCornerStyle(false,true,true,false);
    m_btnModifyFontStyle[1].SetCornerStyle(true,false,false,true);
    RefreshFontStyleFocus();

    // FONT BOLDEN SETTING
    m_textBolden.SetText(StringManager::GetPCSTRById(TOUCH_BOOKSETTING_BOLD));
    m_textBolden.SetFontSize(tagFontSize);
    m_textBolden.SetAlign(ALIGN_LEFT);
    AppendChild(&m_textBolden);

    m_btnModifyEmbolden[0].Initialize(ID_BTN_TOUCH_DECREEMBOLDEN, "",  m_fontsize20);
    m_btnModifyEmbolden[1].Initialize(ID_BTN_TOUCH_INCREEMBOLDEN, "",  m_fontsize20);
    m_btnModifyEmbolden[0].SetIcons(ImageManager::GetImage(IMAGE_TOUCH_ICON_BLACK_DECRE), ImageManager::GetImage(IMAGE_TOUCH_ICON_BLACK_DECRE_WHITE), UIButton::ICON_LEFT);
    m_btnModifyEmbolden[1].SetIcons(ImageManager::GetImage(IMAGE_TOUCH_ICON_BLACK_INCRE), ImageManager::GetImage(IMAGE_TOUCH_ICON_BLACK_INCRE_WHITE), UIButton::ICON_LEFT);
    m_btnModifyEmbolden[0].SetDisabledIcon(ImageManager::GetImage(IMAGE_TOUCH_ICON_BLACK_DECRE_DISABLE));
    m_btnModifyEmbolden[1].SetDisabledIcon(ImageManager::GetImage(IMAGE_TOUCH_ICON_BLACK_INCRE_DISABLE));
    for (int i = 0; i < 2; ++i)
    {
        AppendChild(m_btnModifyEmbolden+i);
        m_btnModifyEmbolden[i].SetMinHeight(minHeight);
    }
}

void UIReaderSettingDlg::UpdateFontSettingWindows()
{
    m_middleSizers[MID_WINDOWS_FONTSETTING]->Clear();
    const int itemSpacing = GetWindowMetrics(UIPixelValue23Index);
    const int lineSpacing = GetWindowMetrics(UIPixelValue10Index);
    const int btnSpacing = GetWindowMetrics(UIPixelValue32Index);
    bool textValid = !m_bIsImage && !IsComicsChapter() && (!m_bIsScannedText || m_bIsPdfTextMode);
    m_textLangSettingTitle.SetVisible(textValid);
    m_textFontOtherSettingTitle.SetVisible(textValid);
    m_textFontStyle.SetVisible(!m_bIsScannedText);
    m_textFontSize.SetVisible(textValid);
    m_textBolden.SetVisible(textValid);
    for (int i = 0; i < 2; ++i)
    {
        m_textLangChoices[i].SetVisible(textValid);
        m_btnLangChoices[i].SetVisible(textValid);
        m_btnModifyFontStyle[i].SetVisible(!m_bIsScannedText);
        m_btnModifyFontSize[i].SetVisible(textValid);
    }

    if (textValid)
    {
        // LANG SETTING
        m_middleSizers[MID_WINDOWS_FONTSETTING]->Add(&m_textLangSettingTitle, UISizerFlags().Expand().Border(dkBOTTOM, lineSpacing));
        UISizer* cnFontSizer = new UIBoxSizer(dkHORIZONTAL);
        if (cnFontSizer)
        {
            cnFontSizer->Add(m_textLangChoices, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
            cnFontSizer->Add(m_btnLangChoices, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL).Border(dkLEFT, btnSpacing));
            m_middleSizers[MID_WINDOWS_FONTSETTING]->Add(cnFontSizer, UISizerFlags().Expand().Border(dkLEFT, btnSpacing).Border(dkBOTTOM, lineSpacing));
        }
        UISizer* enFontSizer = new UIBoxSizer(dkHORIZONTAL);
        if (enFontSizer)
        {
            enFontSizer->Add(m_textLangChoices+1, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
            enFontSizer->Add(m_btnLangChoices+1, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL).Border(dkLEFT, btnSpacing));
            m_middleSizers[MID_WINDOWS_FONTSETTING]->Add(enFontSizer, UISizerFlags().Expand().Border(dkLEFT, btnSpacing));
        }
        m_middleSizers[MID_WINDOWS_FONTSETTING]->AddSpacer(itemSpacing);

        m_middleSizers[MID_WINDOWS_FONTSETTING]->Add(&m_textFontOtherSettingTitle, UISizerFlags().Expand().Border(dkBOTTOM, lineSpacing));

        if (!m_bIsScannedText)
        {
            // FONT STYLE SETTING
            UISizer* fontStyleSizer = new UIBoxSizer(dkHORIZONTAL);
            if (fontStyleSizer)
            {
                fontStyleSizer->Add(&m_textFontStyle, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Border(dkRIGHT, btnSpacing));
                fontStyleSizer->Add(m_btnModifyFontStyle, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
                fontStyleSizer->Add(m_btnModifyFontStyle+1, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
                m_middleSizers[MID_WINDOWS_FONTSETTING]->Add(fontStyleSizer, UISizerFlags().Expand().Border(dkLEFT, btnSpacing).Border(dkBOTTOM, lineSpacing));
            }
        }

        // FONT SIZE SETTING
        UISizer* fontSizeSizer = new UIBoxSizer(dkHORIZONTAL);
        if (fontSizeSizer)
        {
            fontSizeSizer->Add(&m_textFontSize, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Border(dkRIGHT, btnSpacing));
            fontSizeSizer->Add(m_btnModifyFontSize, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
            fontSizeSizer->AddSpacer(btnSpacing);
            fontSizeSizer->Add(m_btnModifyFontSize+1, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));

            m_middleSizers[MID_WINDOWS_FONTSETTING]->Add(fontSizeSizer, UISizerFlags().Expand().Border(dkLEFT, btnSpacing).Border(dkBOTTOM, lineSpacing));
            RefreshFontSizeEnable(m_iCurFontSizeIndex);
        }
    }
    // FONT BOLDEN SETTING
    UISizer* fontBoldenSizer = new UIBoxSizer(dkHORIZONTAL);
    if (fontBoldenSizer)
    {
        if (textValid)
        {
            fontBoldenSizer->Add(&m_textBolden, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Border(dkRIGHT, btnSpacing));
        }
        fontBoldenSizer->Add(m_btnModifyEmbolden, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
        fontBoldenSizer->AddSpacer(btnSpacing);
        fontBoldenSizer->Add(m_btnModifyEmbolden+1, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));

        m_middleSizers[MID_WINDOWS_FONTSETTING]->Add(fontBoldenSizer, UISizerFlags().Expand().Border(dkLEFT, btnSpacing).Border(dkBOTTOM, lineSpacing));
        RefreshFontBoldenEnable((m_bIsScannedText && !m_bIsPdfTextMode) ? m_iCurPdfEmBoldenLevel : m_iCurEmBoldenLevel);
    }
}

void UIReaderSettingDlg::InitMiddleCuttingEdge()
{
    m_middleSizers[MID_WINDOWS_CUTTINGEDGE]->Clear();

    // Add title
    m_textEdgeCuttingTitle.SetText(StringManager::GetPCSTRById(TOUCH_BOOKSETTING_TRIM_SETTING));
    m_textEdgeCuttingTitle.SetFontSize(GetWindowFontSize(FontSize28Index));
    m_textEdgeCuttingTitle.SetAlign(ALIGN_LEFT);
    AppendChild(&m_textEdgeCuttingTitle);
    m_middleSizers[MID_WINDOWS_CUTTINGEDGE]->Add(&m_textEdgeCuttingTitle);
    m_middleSizers[MID_WINDOWS_CUTTINGEDGE]->AddSpacer(GetWindowMetrics(UIPixelValue23Index));

    // Add smart cutting button
    UISizer* smartCuttingSizer = new UIBoxSizer(dkHORIZONTAL);

    m_textSmartCuttingTitle.SetText(StringManager::GetPCSTRById(TOUCH_BOOKSETTING_SMART_TRIM));
    m_textSmartCuttingTitle.SetFontSize(m_fontsize20);
    m_textSmartCuttingTitle.SetAlign(ALIGN_LEFT);
    AppendChild(&m_textSmartCuttingTitle);
    smartCuttingSizer->Add(&m_textSmartCuttingTitle);
    smartCuttingSizer->AddSpacer(GetWindowMetrics(UIPixelValue23Index));

    m_btnSmartCuttingSwitch.Initialize(ID_BTN_TOUCH_CUTTINGEDGE_SMART, "", m_fontsize20);
    m_btnSmartCuttingSwitch.ShowBorder(false);
    m_btnSmartCuttingSwitch.SetLeftMargin(0);
    m_btnSmartCuttingSwitch.SetTopMargin(0);
    m_btnSmartCuttingSwitch.SetIcon(m_PdfModeController.m_bIsSmartCutting ?
                                    ImageManager::GetImage(IMAGE_TOUCH_SWITCH_ON)
                                    : ImageManager::GetImage(IMAGE_TOUCH_SWITCH_OFF));
    m_btnSmartCuttingSwitch.SetAlign(ALIGN_CENTER);
    AppendChild(&m_btnSmartCuttingSwitch);
    smartCuttingSizer->Add(&m_btnSmartCuttingSwitch, UISizerFlags().Center());
    m_middleSizers[MID_WINDOWS_CUTTINGEDGE]->Add(smartCuttingSizer, UISizerFlags().Expand());
    m_middleSizers[MID_WINDOWS_CUTTINGEDGE]->AddSpacer(GetWindowMetrics(UIPixelValue23Index));

    // Add manual cutting button
    m_textManualCuttingTitle.SetText(StringManager::GetPCSTRById(TOUCH_BOOKSETTING_MANUAL_TRIM));
    m_textManualCuttingTitle.SetFontSize(m_fontsize20);
    m_textManualCuttingTitle.SetAlign(ALIGN_LEFT);
    AppendChild(&m_textManualCuttingTitle);
    m_middleSizers[MID_WINDOWS_CUTTINGEDGE]->Add(&m_textManualCuttingTitle);
    m_middleSizers[MID_WINDOWS_CUTTINGEDGE]->AddSpacer(GetWindowMetrics(UIPixelValue23Index));

    UISizer* manualCuttingSizer = new UIBoxSizer(dkHORIZONTAL);
    m_btnModifyCuttingEdge[0].Initialize(ID_BTN_TOUCH_CUTTINGEDGE_NORMAL, StringManager::GetPCSTRById(TOUCH_CUTTINGEDGE_NORMAL),  m_fontsize20);
    m_btnModifyCuttingEdge[1].Initialize(ID_BTN_TOUCH_CUTTINGEDGE_ODDEVEN, StringManager::GetPCSTRById(TOUCH_CUTTINGEDGE_ODDEVEN),  m_fontsize20);
    for ( int i = 0; i < 2; ++i) { 
        AppendChild(m_btnModifyCuttingEdge+i);
        m_btnModifyCuttingEdge[i].SetMinHeight(GetWindowMetrics(UIPixelValue40Index));
    }
    manualCuttingSizer->Add(m_btnModifyCuttingEdge, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
    manualCuttingSizer->AddSpacer(GetWindowMetrics(UIPixelValue32Index));
    manualCuttingSizer->Add(m_btnModifyCuttingEdge+1, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
    m_middleSizers[MID_WINDOWS_CUTTINGEDGE]->Add(manualCuttingSizer, UISizerFlags().Expand());
    m_middleSizers[MID_WINDOWS_CUTTINGEDGE]->AddSpacer(GetWindowMetrics(UIPixelValue23Index));
}

void UIReaderSettingDlg::InitMiddleLayoutWindows()
{
    m_textReadingLayout.SetText(StringManager::GetPCSTRById(READERLAYOUT));
    m_textReadingLayout.SetFontSize(GetWindowFontSize(FontSize22Index));
    m_textReadingLayout.SetAlign(ALIGN_LEFT);
    AppendChild(&m_textReadingLayout);

    m_textLayoutStyle.SetText(StringManager::GetPCSTRById(LAYOUT));
    m_textLayoutStyle.SetFontSize(GetWindowFontSize(FontSize22Index));
    m_textLayoutStyle.SetAlign(ALIGN_LEFT);
    AppendChild(&m_textLayoutStyle);

    m_btnReadingLayout[0].Initialize(ID_BTN_TOUCH_READERLAYOUT_FULLSCREEN, StringManager::GetPCSTRById(READERLAYOUT_FULLSCREEN), m_fontsize20);
    m_btnReadingLayout[1].Initialize(ID_BTN_TOUCH_READERLAYOUT_SHOWREADERMESSAGE, StringManager::GetPCSTRById(READERLAYOUT_SHOWREADERMESSAGE), m_fontsize20);
    switch (m_iCurReadingLayout)
    {
    case 0:
        m_btnReadingLayout[0].SetFocus(true);
        break;
    case 1:
        m_btnReadingLayout[1].SetFocus(true);
        break;
    default:
        break;
    }

    m_btnLayoutStyleChoices[0].Initialize(ID_BTN_TOUCH_LAYOUTSTYLE_ELEGANT,StringManager::GetPCSTRById(LAYOUT_ELEGANT), m_fontsize20);
    m_btnLayoutStyleChoices[1].Initialize(ID_BTN_TOUCH_LAYOUTSTYLE_STANDARD,StringManager::GetPCSTRById(LAYOUT_STANDARD), m_fontsize20);
    m_btnLayoutStyleChoices[2].Initialize(ID_BTN_TOUCH_LAYOUTSTYLE_COMPACT,StringManager::GetPCSTRById(LAYOUT_COMPACT), m_fontsize20);
    m_btnLayoutStyleChoices[3].Initialize(ID_BTN_TOUCH_LAYOUTSTYLE_NORMAL,StringManager::GetPCSTRById(LAYOUT_ORIGINAL), m_fontsize20);

    UISizer* readerLayoutSizer = new UIBoxSizer(dkHORIZONTAL);
    for ( int i=0; i<2; i++) {
        AppendChild(m_btnReadingLayout + i);
        m_btnReadingLayout[i].SetMinHeight(GetWindowMetrics(UIPixelValue40Index));
        readerLayoutSizer->Add(m_btnReadingLayout+i, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
    }
    m_btnReadingLayout[0].SetCornerStyle(false,true,true,false);
    m_btnReadingLayout[1].SetCornerStyle(true,false,false,true);

    UISizer* layoutStyletSizer = new UIBoxSizer(dkHORIZONTAL);
    for ( int i=0; i<4; i++) {
        AppendChild(m_btnLayoutStyleChoices + i);
        m_btnLayoutStyleChoices[i].SetMinHeight(GetWindowMetrics(UIPixelValue40Index));
        m_btnLayoutStyleChoices[i].SetCornerStyle(true,true,true,true);
        layoutStyletSizer->Add(m_btnLayoutStyleChoices+i, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
    }
    m_btnLayoutStyleChoices[0].SetCornerStyle(false,true,true,false);
    m_btnLayoutStyleChoices[3].SetCornerStyle(true,false,false,true);

    switch (m_iCurLayoutStyle) {
    case LAYOUT_STYLE_ELEGANT:
        m_btnLayoutStyleChoices[0].SetFocus(true);
        break;
    case LAYOUT_STYLE_STANDARD:
        m_btnLayoutStyleChoices[1].SetFocus(true);
        break;
    case LAYOUT_STYLE_COMPACT:
        m_btnLayoutStyleChoices[2].SetFocus(true);
        break;
    case LAYOUT_SYSLE_NORMAL:
        m_btnLayoutStyleChoices[3].SetFocus(true);
        break;
    default:
        break;
    }

    UISizer* layoutSizer = new UIBoxSizer(dkVERTICAL);
    layoutSizer->Add(&m_textReadingLayout, UISizerFlags().Expand());
    layoutSizer->Add(readerLayoutSizer, UISizerFlags().Expand().Border(dkTOP | dkBOTTOM , GetWindowMetrics(UIPixelValue23Index)/2));
    layoutSizer->AddSpacer(GetWindowMetrics(UIPixelValue23Index)/2);
    layoutSizer->Add(&m_textLayoutStyle, UISizerFlags().Expand());
    layoutSizer->Add(layoutStyletSizer, UISizerFlags().Expand().Border(dkTOP, GetWindowMetrics(UIPixelValue23Index)/2));

    m_middleSizers[MID_WINDOWS_LAYOUT]->Add(layoutSizer, UISizerFlags(1).Expand());
}

void UIReaderSettingDlg::InitMiddleRotateWindows()
{
    m_btnRotate[0].Initialize(ID_BTN_TOUCH_ROTATE_0,   "", m_fontsize20);
    m_btnRotate[0].SetIcons(ImageManager::GetImage(IMAGE_TOUCH_ICON_ROTATE01), ImageManager::GetImage(IMAGE_TOUCH_ICON_ROTATE01_WHITE), UIButton::ICON_LEFT);
    m_btnRotate[1].Initialize(ID_BTN_TOUCH_ROTATE_90,  "", m_fontsize20);
    m_btnRotate[1].SetIcons(ImageManager::GetImage(IMAGE_TOUCH_ICON_ROTATE02), ImageManager::GetImage(IMAGE_TOUCH_ICON_ROTATE02_WHITE), UIButton::ICON_LEFT);
    m_btnRotate[2].Initialize(ID_BTN_TOUCH_ROTATE_180, "", m_fontsize20); 
    m_btnRotate[2].SetIcons(ImageManager::GetImage(IMAGE_TOUCH_ICON_ROTATE03), ImageManager::GetImage(IMAGE_TOUCH_ICON_ROTATE03_WHITE), UIButton::ICON_LEFT);
    m_btnRotate[3].Initialize(ID_BTN_TOUCH_ROTATE_270, "", m_fontsize20);
    m_btnRotate[3].SetIcons(ImageManager::GetImage(IMAGE_TOUCH_ICON_ROTATE04), ImageManager::GetImage(IMAGE_TOUCH_ICON_ROTATE04_WHITE), UIButton::ICON_LEFT);

    for ( int i=0; i<4; ++i) {
        m_btnRotate[i].SetFocus(false);
        AppendChild(m_btnRotate+i);
        m_btnRotate[i].SetCornerStyle(true,true,true,true);
    }
    m_btnRotate[0].SetCornerStyle(false,true,true,false);
    m_btnRotate[3].SetCornerStyle(true,false,false,true);

    int index = m_PdfModeController.m_iRotation / 90;
    if (index < 0 || index > 4)
    {
        index = 0;
    }
    m_btnRotate[index].SetFocus(true);

    for ( int i=0; i<4; ++i)
    {
        m_btnRotate[i].SetMinHeight(GetWindowMetrics(UIPixelValue40Index));
        m_middleSizers[MID_WINDOWS_ROTATE]->Add(m_btnRotate+i, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
    }
}

void UIReaderSettingDlg::InitMiddleReaderSettingWindows()
{
    const int minHeight = GetWindowMetrics(UIPixelValue40Index);
    const int titleFontSize = GetWindowFontSize(FontSize28Index);
    const int tagFontSize = GetWindowFontSize(FontSize24Index);

    m_textReaderSettingTitle.SetText(StringManager::GetPCSTRById(TOUCH_BOOKSETTING_READERSETTING));
    m_textReaderSettingTitle.SetFontSize(titleFontSize);
    m_textReaderSettingTitle.SetAlign(ALIGN_LEFT);    
    AppendChild(&m_textReaderSettingTitle);
    
    m_textTurnPageCustom.SetText(StringManager::GetPCSTRById(TOUCH_BOOKSETTING_TAP_LEFT));
    if (!m_bIsScannedText)
    {
        UIBookReaderContainer* pBookReadContainer = dynamic_cast<UIBookReaderContainer*>(m_pContainer);
        if (pBookReadContainer && pBookReadContainer->IsVerticalRTL())
        {
            m_textTurnPageCustom.SetText(StringManager::GetPCSTRById(TOUCH_BOOKSETTING_TAP_RIGHT));
        }
    }
    m_textTurnPageCustom.SetFontSize(tagFontSize);
    m_textTurnPageCustom.SetAlign(ALIGN_LEFT);
    AppendChild(&m_textTurnPageCustom);
    
    m_btnTurnPageCustom[0].Initialize(ID_BTN_TOUCH_PAGEUP_CUSTOM, StringManager::GetPCSTRById(TOUCH_BOOKSETTING_TURN_UP), m_fontsize20);
    m_btnTurnPageCustom[1].Initialize(ID_BTN_TOUCH_PAGEDOWN_CUSTOM, StringManager::GetPCSTRById(TOUCH_BOOKSETTING_TURN_DOWN), m_fontsize20);
    for ( int i=0; i<2; ++i) {
        m_btnTurnPageCustom[i].SetFontSize(m_fontsize20);
        m_btnTurnPageCustom[i].SetAlign(ALIGN_CENTER);
        m_btnTurnPageCustom[i].SetMinHeight(minHeight);
        AppendChild(m_btnTurnPageCustom+i);
    }

    m_textReadingUpDownGestureCustom.SetText(StringManager::GetPCSTRById(TOUCH_BOOKSETTING_READINGUPDOWN_GUSTURE_CUSTOM));
    m_textReadingUpDownGestureCustom.SetFontSize(tagFontSize);
    m_textReadingUpDownGestureCustom.SetAlign(ALIGN_LEFT);
    AppendChild(&m_textReadingUpDownGestureCustom);

    m_btnReadingUpDownGestureCustom[0].Initialize(ID_BTN_TOUCH_READINGUPDOWN_GESTURE_CHAPTER, StringManager::GetPCSTRById(TOUCH_BOOKSETTING_READINGUPDOWN_GESTURE_CHAPTER), m_fontsize20);
    m_btnReadingUpDownGestureCustom[1].Initialize(ID_BTN_TOUCH_READINGUPDOWN_GESTURE_PAGE, StringManager::GetPCSTRById(TOUCH_BOOKSETTING_READINGUPDOWN_GESTURE_PAGE), m_fontsize20);
    for ( int i=0; i<2; ++i) {
        m_btnReadingUpDownGestureCustom[i].SetFontSize(m_fontsize20);
        m_btnReadingUpDownGestureCustom[i].SetAlign(ALIGN_CENTER);
        m_btnReadingUpDownGestureCustom[i].SetMinHeight(minHeight);
        AppendChild(m_btnReadingUpDownGestureCustom+i);
    }

    m_btnTurnPageCustom[0].SetCornerStyle(false,true,true,false);
    m_btnTurnPageCustom[1].SetCornerStyle(true,false,false,true);
    
    m_btnReadingUpDownGestureCustom[0].SetCornerStyle(false,true,true,false);
    m_btnReadingUpDownGestureCustom[1].SetCornerStyle(true,false,false,true);

    m_textContentDisplay.SetText(StringManager::GetPCSTRById(TOUCH_BOOKSETTING_DISPLAY_CONTENT));
    m_textContentDisplay.SetFontSize(tagFontSize);
    m_textContentDisplay.SetAlign(ALIGN_LEFT);
    AppendChild(&m_textContentDisplay);

    m_btnContentDisplay[0].Initialize(ID_BTN_TOUCH_NORMAL_DISPLAY, StringManager::GetPCSTRById(TOUCH_BOOKSETTING_DISPLAY_NORMAL), m_fontsize20);
    m_btnContentDisplay[1].Initialize(ID_BTN_TOUCH_ADAPTWIDE_DISPLAY, StringManager::GetPCSTRById(TOUCH_BOOKSETTING_DISPLAY_ADAPTWIDE), m_fontsize20);
    for ( int i=0; i<2; ++i) {
        m_btnContentDisplay[i].SetFontSize(m_fontsize20);
        m_btnContentDisplay[i].SetAlign(ALIGN_CENTER);
        m_btnContentDisplay[i].SetMinHeight(minHeight);
        AppendChild(m_btnContentDisplay+i);
    }
    m_btnContentDisplay[0].SetCornerStyle(false,true,true,false);
    m_btnContentDisplay[1].SetCornerStyle(true,false,false,true);
    
    // 进度条
    m_textProgressBar.SetText(StringManager::GetPCSTRById(PROGRESSBAR_SELECT_TYPE));
    m_textProgressBar.SetFontSize(tagFontSize);
    m_textProgressBar.SetAlign(ALIGN_LEFT);
    AppendChild(&m_textProgressBar);

    m_btnProgressBar[0].Initialize(ID_BTN_TOUCH_NORMAL_PROGRESSBAR, StringManager::GetPCSTRById(TOUCH_PROGRESSBAR_NORMAL_TYPE), m_fontsize20);
    m_btnProgressBar[1].Initialize(ID_BTN_TOUCH_MINI_PROGRESSBAR, StringManager::GetPCSTRById(TOUCH_PROGRESSBAR_MINI_TYPE), m_fontsize20);
    for ( int i=0; i<2; ++i) {
        m_btnProgressBar[i].SetFontSize(m_fontsize20);
        m_btnProgressBar[i].SetAlign(ALIGN_CENTER);
        m_btnProgressBar[i].SetMinHeight(minHeight);
        AppendChild(m_btnProgressBar+i);
    }
    m_btnProgressBar[0].SetCornerStyle(false,true,true,false);
    m_btnProgressBar[1].SetCornerStyle(true,false,false,true);

    // 字体渲染
    m_textFontRender.SetText(StringManager::GetPCSTRById(FONT_RENDER));
    m_textFontRender.SetFontSize(tagFontSize);
    m_textFontRender.SetAlign(ALIGN_LEFT);
    AppendChild(&m_textFontRender);

    m_btnFontRender[0].Initialize(ID_BTN_TOUCH_FONT_RENDER_SHARP, StringManager::GetPCSTRById(FONT_RENDER_SHARP), m_fontsize20);
    m_btnFontRender[1].Initialize(ID_BTN_TOUCH_FONT_RENDER_SMOOTH, StringManager::GetPCSTRById(FONT_RENDER_SMOOTH), m_fontsize20);
    for ( int i=0; i<2; ++i) {
        m_btnFontRender[i].SetFontSize(m_fontsize20);
        m_btnFontRender[i].SetAlign(ALIGN_CENTER);
        m_btnFontRender[i].SetMinHeight(minHeight);
        AppendChild(m_btnFontRender+i);
    }
    m_btnFontRender[0].SetCornerStyle(false,true,true,false);
    m_btnFontRender[1].SetCornerStyle(true,false,false,true);
}

void UIReaderSettingDlg::UpdateReaderSettingWindows()
{
    m_middleSizers[MID_WINDOWS_READERSETTING]->Clear();

    const int itemSpacing = GetWindowMetrics(UIPixelValue23Index);
    const int lineSpacing = GetWindowMetrics(UIPixelValue10Index);
    const int btnSpacing = GetWindowMetrics(UIPixelValue32Index);
    const int titleMinWidth = GetWindowMetrics(UIReaderSettingDlgReaderSettingSubTitleWidthIndex);
    bool isPdfMode = m_bIsScannedText && !m_bIsPdfTextMode;
    m_textContentDisplay.SetVisible(isPdfMode);
    m_btnContentDisplay[0].SetVisible(isPdfMode);
    m_btnContentDisplay[1].SetVisible(isPdfMode);
    bool isTextMode = !m_bIsImage && !IsComicsChapter() && (!m_bIsScannedText || m_bIsPdfTextMode);
    m_textProgressBar.SetVisible(isTextMode);
    m_btnProgressBar[0].SetVisible(isTextMode);
    m_btnProgressBar[1].SetVisible(isTextMode);

    m_middleSizers[MID_WINDOWS_READERSETTING]->Add(&m_textReaderSettingTitle, UISizerFlags().Expand().Border(dkBOTTOM, itemSpacing));
    if (isPdfMode)
    {
        UISizer* contentDisplaySizer = new UIBoxSizer(dkHORIZONTAL);
        if (contentDisplaySizer)
        {
            m_textContentDisplay.SetMinWidth(titleMinWidth);
            contentDisplaySizer->Add(&m_textContentDisplay, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
            contentDisplaySizer->Add(m_btnContentDisplay, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
            contentDisplaySizer->Add(m_btnContentDisplay + 1, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
            m_middleSizers[MID_WINDOWS_READERSETTING]->Add(contentDisplaySizer, UISizerFlags().Expand().Border(dkLEFT, btnSpacing).Border(dkBOTTOM, lineSpacing));
        }
    }
    if (isTextMode)
    {
        UISizer* textProgressBarSizer = new UIBoxSizer(dkHORIZONTAL);
        if (textProgressBarSizer)
        {
            m_textProgressBar.SetMinWidth(titleMinWidth);
            textProgressBarSizer->Add(&m_textProgressBar, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
            textProgressBarSizer->Add(m_btnProgressBar, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
            textProgressBarSizer->Add(m_btnProgressBar + 1, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
            m_middleSizers[MID_WINDOWS_READERSETTING]->Add(textProgressBarSizer, UISizerFlags().Expand().Border(dkLEFT, btnSpacing).Border(dkBOTTOM, lineSpacing));
        }
    }

    // 字体渲染
    UISizer* textFontRenderSizer = new UIBoxSizer(dkHORIZONTAL);
    if (textFontRenderSizer)
    {
        m_textFontRender.SetMinWidth(titleMinWidth);
        textFontRenderSizer->Add(&m_textFontRender, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
        textFontRenderSizer->Add(m_btnFontRender, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
        textFontRenderSizer->Add(m_btnFontRender + 1, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
        m_middleSizers[MID_WINDOWS_READERSETTING]->Add(textFontRenderSizer, UISizerFlags().Expand().Border(dkLEFT, btnSpacing).Border(dkBOTTOM, lineSpacing));
    }

    UISizer* turnPageCurstomSizer = new UIBoxSizer(dkHORIZONTAL);
    if (turnPageCurstomSizer)
    {
        m_textTurnPageCustom.SetMinWidth(titleMinWidth);
        turnPageCurstomSizer->Add(&m_textTurnPageCustom, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
        turnPageCurstomSizer->Add(m_btnTurnPageCustom, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
        turnPageCurstomSizer->Add(m_btnTurnPageCustom + 1, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
        m_middleSizers[MID_WINDOWS_READERSETTING]->Add(turnPageCurstomSizer, UISizerFlags().Expand().Border(dkLEFT, btnSpacing).Border(dkBOTTOM, lineSpacing));
    }

    UISizer* readingUpDownGestureCustomSizer = new UIBoxSizer(dkHORIZONTAL);
    if (readingUpDownGestureCustomSizer)
    {
        m_textReadingUpDownGestureCustom.SetMinWidth(titleMinWidth);
        readingUpDownGestureCustomSizer->Add(&m_textReadingUpDownGestureCustom, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
        readingUpDownGestureCustomSizer->Add(m_btnReadingUpDownGestureCustom, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
        readingUpDownGestureCustomSizer->Add(m_btnReadingUpDownGestureCustom + 1, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
        m_middleSizers[MID_WINDOWS_READERSETTING]->Add(readingUpDownGestureCustomSizer, UISizerFlags().Expand().Border(dkLEFT, btnSpacing).Border(dkBOTTOM, lineSpacing));
    }
}

void UIReaderSettingDlg::InitMiddleColumnedWindows()
{
    m_textColumned.SetText(StringManager::GetPCSTRById(LABEL_FENLAN));
    m_textColumned.SetFontSize(GetWindowFontSize(FontSize28Index));
    m_textComic.SetText(StringManager::GetPCSTRById(IMAGE_CARTOON_MODE));
    m_textComic.SetFontSize(GetWindowFontSize(FontSize28Index));

    m_btnColumns[0].Initialize(ID_BTN_TOUCH_PDF_COLUMNED_ORDER_1,       "", GetWindowFontSize(FontSize28Index));
    m_btnColumns[0].SetIcons(ImageManager::GetImage(IMAGE_TOUCH_ICON_PDF_ORDER_1), ImageManager::GetImage(IMAGE_TOUCH_ICON_PDF_ORDER_1_PRESSED), UIButton::ICON_LEFT);
    m_btnColumns[1].Initialize(ID_BTN_TOUCH_PDF_COLUMNED_ORDER_2,       "", GetWindowFontSize(FontSize28Index));
    m_btnColumns[1].SetIcons(ImageManager::GetImage(IMAGE_TOUCH_ICON_PDF_ORDER_2), ImageManager::GetImage(IMAGE_TOUCH_ICON_PDF_ORDER_2_PRESSED), UIButton::ICON_LEFT);
    m_btnColumns[2].Initialize(ID_BTN_TOUCH_PDF_COLUMNED_ORDER_3,       "", GetWindowFontSize(FontSize28Index));
    m_btnColumns[2].SetIcons(ImageManager::GetImage(IMAGE_TOUCH_ICON_PDF_ORDER_3), ImageManager::GetImage(IMAGE_TOUCH_ICON_PDF_ORDER_3_PRESSED), UIButton::ICON_LEFT);
    m_btnColumns[3].Initialize(ID_BTN_TOUCH_PDF_COLUMNED_ORDER_4,       "", GetWindowFontSize(FontSize28Index));
    m_btnColumns[3].SetIcons(ImageManager::GetImage(IMAGE_TOUCH_ICON_PDF_ORDER_4), ImageManager::GetImage(IMAGE_TOUCH_ICON_PDF_ORDER_4_PRESSED), UIButton::ICON_LEFT);
    
    m_btnComics[0].Initialize(ID_BTN_TOUCH_COMICS_ORDER_1,      "", m_fontsize20);
    m_btnComics[0].SetIcons(ImageManager::GetImage(IMAGE_TOUCH_ICON_PDF_ORDER_6), ImageManager::GetImage(IMAGE_TOUCH_ICON_PDF_ORDER_6), UIButton::ICON_LEFT);
    m_btnComics[1].Initialize(ID_BTN_TOUCH_COMICS_ORDER_2,      "", m_fontsize20);
    m_btnComics[1].SetIcons(ImageManager::GetImage(IMAGE_TOUCH_ICON_PDF_ORDER_7), ImageManager::GetImage(IMAGE_TOUCH_ICON_PDF_ORDER_7), UIButton::ICON_LEFT);

    m_middleSizers[MID_WINDOWS_COLUMNED]->Add(&m_textColumned, UISizerFlags().Border(dkTOP, GetWindowMetrics(UIReaderSettingDlgMiddleButtonsVerticalMarginIndex)));
    UISizer* splitSizer = new UIBoxSizer(dkHORIZONTAL);
    for ( int i=0; i<4; ++i)
    {
        m_btnColumns[i].SetMinSize(dkSize(GetWindowMetrics(UIReaderSettingDlgSplitButtonWidthIndex),
            GetWindowMetrics(UIReaderSettingDlgSplitButtonHeightIndex)));
        splitSizer->Add(m_btnColumns+i);
    }
    m_middleSizers[MID_WINDOWS_COLUMNED]->Add(splitSizer, UISizerFlags().Border(dkTOP, GetWindowMetrics(UIReaderSettingDlgMiddleButtonsVerticalMarginIndex)));
    m_middleSizers[MID_WINDOWS_COLUMNED]->Add(&m_textComic, UISizerFlags().Border(dkTOP, GetWindowMetrics(UIReaderSettingDlgMiddleButtonsVerticalMarginIndex)));
    UISizer* comicsSizer = new UIBoxSizer(dkHORIZONTAL);
    for ( int i=0; i<2; ++i)
    {
        m_btnComics[i].SetMinSize(dkSize(GetWindowMetrics(UIReaderSettingDlgSplitButtonWidthIndex),
            GetWindowMetrics(UIReaderSettingDlgSplitButtonHeightIndex)));
        comicsSizer->Add(m_btnComics+i);
    }
    m_middleSizers[MID_WINDOWS_COLUMNED]->Add(comicsSizer, UISizerFlags().Border(dkTOP | dkBOTTOM, GetWindowMetrics(UIReaderSettingDlgMiddleButtonsVerticalMarginIndex)));

    for ( int i=0; i<2; ++i) {
        AppendChild(m_btnComics + i);
    }
    m_btnComics[0].SetCornerStyle(false,true,true,false);
    m_btnComics[1].SetCornerStyle(true,false,false,true);

    AppendChild(&m_textComic);

    size_t columns = sizeof(m_btnColumns)/sizeof(m_btnColumns[0]);
    for ( size_t i=0; i<columns; ++i) {
        AppendChild(m_btnColumns + i);
        m_btnColumns[i].SetCornerStyle(true,true,true,true);
    }
    m_btnColumns[0].SetCornerStyle(false,true,true,false);
    m_btnColumns[columns-1].SetCornerStyle(true,false,false,true);

    AppendChild(&m_textColumned);
}

DWORD UIReaderSettingDlg::GetCmdIdOfRotationOrTTS()
{
    if (IsComicsChapter())
    {
        return IDSTATIC;
    }
    if (m_bIsImage || (m_bIsScannedText && !m_bIsPdfTextMode))
    {
        return ID_BTN_TOUCH_PDFROTATE;
    }
    return DeviceInfo::IsK4Touch() ? ID_BTN_TOUCH_START_TTS : IDSTATIC;
}

DWORD UIReaderSettingDlg::GetCmdIdOfVolume()
{
    if (m_bIsImage || IsComicsChapter() || (m_bIsScannedText && !m_bIsPdfTextMode))
    {
        return IDSTATIC;
    }
    return (DeviceInfo::IsK4Touch() && m_bIsTTSPlaying) ? ID_BTN_TOUCH_VOLUME : IDSTATIC;
}

DWORD UIReaderSettingDlg::GetCmdIdOfBottomMidBtns(int index)
{
    switch(index)
    {
    case 0: // 切边 / 字体
        return (m_bIsImage || (m_bIsScannedText && !m_bIsPdfTextMode)) ? ID_BTN_TOUCH_PDFTRIM : ID_BTN_TOUCH_FONTSETTING;
    case 1: // 分栏 / 版式
        return (m_bIsImage || (m_bIsScannedText && !m_bIsPdfTextMode)) ? ID_BTN_TOUCH_PDFCOLUMN : ID_BTN_TOUCH_LAYOUTSTYLE;
    case 2: // 重排
        if (m_bIsScannedText)
        {
            return ID_BTN_TOUCH_PDFREARRANGE;
        }
        break;
    case 3:// 加黑
        if (m_bIsScannedText && !m_bIsPdfTextMode)
        {
            return ID_BTN_TOUCH_BOLD;
        }
    default:
        break;
    }
    return IDSTATIC;
}

const char* UIReaderSettingDlg::GetStringOfRotationOrTTS()
{
    if (m_bIsImage || (m_bIsScannedText && !m_bIsPdfTextMode))
    {
        return StringManager::GetPCSTRById(TOUCH_BOOKSETTING_ROTATE);
    }
    return StringManager::GetPCSTRById(m_bIsTTSPlaying ? TOUCH_BOOKSETTING_CLOSETTS : TOUCH_BOOKSETTING_TTS);
}

const char* UIReaderSettingDlg::GetStringOfVolume()
{
    return StringManager::GetPCSTRById(TOUCH_BOOKSETTING_VOLUME);
}

const char* UIReaderSettingDlg::GetStringOfBottomMidBtns(int index)
{
    switch(index)
    {
    case 0:// 切边 / 字体
        {
            if (m_bIsImage && m_imageController)
            {
                return StringManager::GetPCSTRById(TOUCH_BOOKSETTING_TRIM);
            }
            else if (m_bIsScannedText && !m_bIsPdfTextMode)
            {
                return StringManager::GetPCSTRById((m_PdfModeController.m_eCuttingEdgeMode == PDF_CEM_Custom) ? TOUCH_BOOKSETTING_CLOSETRIM : TOUCH_BOOKSETTING_TRIM);
            }
            return StringManager::GetPCSTRById(TOUCH_BOOKSETTING_FONTSETTING);
        }
        break;
    case 1:// 分栏 / 版式
        {
            if (m_bIsImage && m_imageController)
            {
                return StringManager::GetPCSTRById(
                    (m_imageController->m_eReadingMode != PDF_RM_Split2Page && m_imageController->m_eReadingMode != PDF_RM_Split4Page) ? LABEL_FENLAN : LABEL_FENLAN_CLOSE
                    );
            }
            else if (m_bIsScannedText && !m_bIsPdfTextMode)
            {
                return StringManager::GetPCSTRById(
                    (m_PdfModeController.m_eReadingMode != PDF_RM_Split2Page && m_PdfModeController.m_eReadingMode != PDF_RM_Split4Page) ? LABEL_FENLAN : LABEL_FENLAN_CLOSE
                    );
            }
            return StringManager::GetPCSTRById(TOUCH_BOOKSETTING_FORMAT);
        }
        break;
    case 2:// 重排
        if (m_bIsScannedText)
        {
            return StringManager::GetPCSTRById((PDF_RM_Rearrange == m_PdfModeController.m_eReadingMode) ? TOUCH_BOOKSETTING_CLOSEREARRANGE : TOUCH_BOOKSETTING_REARRANGE);
        }
        break;
    case 3:// 加黑
        if (m_bIsScannedText && !m_bIsPdfTextMode)
        {
            return StringManager::GetPCSTRById(TOUCH_BOOKSETTING_BOLD);
        }
        break;
    default:
        break;
    }
    return NULL;
}

SPtr<ITpImage> UIReaderSettingDlg::GetImageOfRotationOrTTS(ICON_TYPE type)
{
    static const int textImageId[] = 
    {
        IMAGE_TOUCH_ICON_READ,         // ICON_UNFOCUSED
        IMAGE_TOUCH_ICON_READ_WHITE,   // ICON_FOCUSED
        IMAGE_TOUCH_ICON_READ_DISABLE  // ICON_DISABLE
    };

    static const int pdfRotateOnImageId[] = 
    {
        IMAGE_TOUCH_ICON_ROTATE,         // ICON_UNFOCUSED
        IMAGE_TOUCH_ICON_ROTATE_WHITE,   // ICON_FOCUSED
        IMAGE_TOUCH_ICON_ROTATE          // ICON_DISABLE
    };

    if ((m_bIsImage && m_imageController) || (m_bIsScannedText && !m_bIsPdfTextMode))
    {
        return ImageManager::GetImage(pdfRotateOnImageId[type]);
    }
    return ImageManager::GetImage(textImageId[type]);
}

SPtr<ITpImage> UIReaderSettingDlg::GetImageOfVolume(ICON_TYPE type)
{
    static const int textImageId[] = 
    {
        IMAGE_TOUCH_ICON_READ_VOLUME,         // ICON_UNFOCUSED
        IMAGE_TOUCH_ICON_READ_VOLUME_FOCUS,   // ICON_FOCUSED
        IMAGE_TOUCH_ICON_READ_VOLUME          // ICON_DISABLE
    };

    return ImageManager::GetImage(textImageId[type]);
}

SPtr<ITpImage> UIReaderSettingDlg::GetImageOfFontsettingOrTrim(ICON_TYPE type)
{
    static const int textImageId[] = 
    {
        IMAGE_TOUCH_ICON_TEXT,         // ICON_UNFOCUSED
        IMAGE_TOUCH_ICON_TEXT_WHITE,   // ICON_FOCUSED
        IMAGE_TOUCH_ICON_TEXT_DISABLE  // ICON_DISABLE
    };

    static const int pdfTrimOnImageId[] = 
    {
        IMAGE_TOUCH_ICON_TRIM,         // ICON_UNFOCUSED
        IMAGE_TOUCH_ICON_TRIM_WHITE,   // ICON_FOCUSED
        IMAGE_TOUCH_ICON_TRIM_DISABLE  // ICON_DISABLE
    };

    static const int pdfTrimOffImageId[] = 
    {
        IMAGE_TOUCH_ICON_TRIM_QUIT,         // ICON_UNFOCUSED
        IMAGE_TOUCH_ICON_TRIM_QUIT_WHITE,   // ICON_FOCUSED
        IMAGE_TOUCH_ICON_TRIM_QUIT  // ICON_DISABLE
    };


    if (m_bIsImage && m_imageController)
    {
        return ImageManager::GetImage(pdfTrimOnImageId[type]);
    }
    else if (m_bIsScannedText && !m_bIsPdfTextMode)
    {
        return ImageManager::GetImage((m_PdfModeController.m_eCuttingEdgeMode != PDF_CEM_Custom) ? pdfTrimOnImageId[type] : pdfTrimOffImageId[type]);
    }
    return ImageManager::GetImage(textImageId[type]);
}

SPtr<ITpImage> UIReaderSettingDlg::GetImageOfFormatOrColumn(ICON_TYPE type)
{
    static const int textImageId[] = 
    {
        IMAGE_TOUCH_ICON_LAYOUT,         // ICON_UNFOCUSED
        IMAGE_TOUCH_ICON_LAYOUT_WHITE,   // ICON_FOCUSED
        IMAGE_TOUCH_ICON_LAYOUT_DISABLE  // ICON_DISABLE
    };

    static const int pdfColumnOnImageId[] = 
    {
        IMAGE_TOUCH_ICON_COLUME,         // ICON_UNFOCUSED
        IMAGE_TOUCH_ICON_COLUME_WHITE,   // ICON_FOCUSED
        IMAGE_TOUCH_ICON_COLUME_DISABLE  // ICON_DISABLE
    };

    static const int pdfColumnOffImageId[] = 
    {
        IMAGE_TOUCH_ICON_COLUME_QUIT,         // ICON_UNFOCUSED
        IMAGE_TOUCH_ICON_COLUME_QUIT_WHITE,   // ICON_FOCUSED
        IMAGE_TOUCH_ICON_COLUME_QUIT          // ICON_DISABLE
    };

    if (m_bIsImage && m_imageController)
    {
        return ImageManager::GetImage(
            (m_imageController->m_eReadingMode != PDF_RM_Split2Page && m_imageController->m_eReadingMode != PDF_RM_Split4Page) 
                ? pdfColumnOnImageId[type]
                : pdfColumnOffImageId[type]
            );
    }
    else if (m_bIsScannedText && !m_bIsPdfTextMode)
    {
        return ImageManager::GetImage(
            (m_PdfModeController.m_eReadingMode != PDF_RM_Split2Page && m_PdfModeController.m_eReadingMode != PDF_RM_Split4Page) 
                ? pdfColumnOnImageId[type]
                : pdfColumnOffImageId[type]
        );
    }
    return ImageManager::GetImage(textImageId[type]);
}

SPtr<ITpImage> UIReaderSettingDlg::GetImageOfRearrange(ICON_TYPE type)
{
    // TODO: 取消智能重排没有
    static const int pdfRearrangeOnImageId[] = 
    {
        IMAGE_TOUCH_ICON_REFILL,         // ICON_UNFOCUSED
        IMAGE_TOUCH_ICON_REFILL_WHITE,   // ICON_FOCUSED
        IMAGE_TOUCH_ICON_REFILL_DISABLE, // ICON_DISABLE
    };

    static const int pdfRearrangeOffImageId[] = 
    {
        IMAGE_TOUCH_ICON_REFILL_QUIT,         // ICON_UNFOCUSED
        IMAGE_TOUCH_ICON_REFILL_QUIT_WHITE,   // ICON_FOCUSED
        IMAGE_TOUCH_ICON_REFILL_QUIT          // ICON_DISABLE
    };

    return ImageManager::GetImage(m_bIsPdfTextMode ? pdfRearrangeOffImageId[type] : pdfRearrangeOnImageId[type]);
}

SPtr<ITpImage> UIReaderSettingDlg::GetImageOfBolden(ICON_TYPE type)
{
    static const int imageId[] = 
    {
        IMAGE_TOUCH_ICON_BLACK,         // ICON_UNFOCUSED
        IMAGE_TOUCH_ICON_BLACK_WHITE,   // ICON_FOCUSED
        IMAGE_TOUCH_ICON_BLACK_DISABLE  // ICON_DISABLE
    };
    return ImageManager::GetImage(imageId[type]);
}

void UIReaderSettingDlg::UpdateBtnStatusOfRotationOrTTS()
{
    DWORD cmdID = GetCmdIdOfRotationOrTTS();
    bool visible = (cmdID != IDSTATIC);
    m_bottomLeftBtns[0].SetVisible(visible);
    if (visible)
    {
        m_bottomLeftBtns[0].Initialize(cmdID, GetStringOfRotationOrTTS(), m_fontsize20);
        m_bottomLeftBtns[0].SetIcons(GetImageOfRotationOrTTS(ICON_UNFOCUSED), GetImageOfRotationOrTTS(ICON_FOCUSED));
        m_bottomLeftBtns[0].SetDisabledIcon(GetImageOfRotationOrTTS(ICON_DISABLE));
    }
}

void UIReaderSettingDlg::UpdateBtnStatusOfVolume()
{
    DWORD cmdID = GetCmdIdOfVolume();
    bool visible = (cmdID != IDSTATIC);
    m_bottomLeftBtns[1].SetVisible(visible);
    if (visible)
    {
        m_bottomLeftBtns[1].Initialize(cmdID, GetStringOfVolume(), m_fontsize20);
        m_bottomLeftBtns[1].SetIcons(GetImageOfVolume(ICON_UNFOCUSED), GetImageOfVolume(ICON_FOCUSED));
        m_bottomLeftBtns[1].SetDisabledIcon(GetImageOfVolume(ICON_DISABLE));
    }
}

void UIReaderSettingDlg::UpdateBottomBtnsStatus()
{
    UpdateBtnStatusOfRotationOrTTS();
    UpdateBtnStatusOfVolume();

    UpdateBtnStatusOfFontsettingOrTrim();
    UpdateBtnStatusOfFormatOrColumn();
    UpdateBtnStatusOfRearrange();
    UpdateBtnStatusOfBolden();

    UpdateBtnStatusOfSetting();

    UpdateBottomLeftWndsCornerStyle();
    UpdateBottomMidWndsCornerStyle();
}

void UIReaderSettingDlg::UpdateBtnStatusOfFontsettingOrTrim()
{
    DWORD cmdID = GetCmdIdOfBottomMidBtns(0);
    bool visible = (cmdID != IDSTATIC);
    m_bottomMidBtns[0].SetVisible(visible);
    if (visible)
    {
        m_bottomMidBtns[0].Initialize(cmdID, GetStringOfBottomMidBtns(0), m_fontsize20);
        m_bottomMidBtns[0].SetIcons(GetImageOfFontsettingOrTrim(ICON_UNFOCUSED), GetImageOfFontsettingOrTrim(ICON_FOCUSED));
        m_bottomMidBtns[0].SetDisabledIcon(GetImageOfFontsettingOrTrim(ICON_DISABLE));
        UpdateEnableOfFontsettingOrTrim();
    }
}

void UIReaderSettingDlg::UpdateEnableOfFontsettingOrTrim()
{
    m_bottomMidBtns[0].SetEnable(true);
    if (m_bIsImage
        || IsComicsFrameMode()
        || (m_bIsScannedText 
            && !m_bIsPdfTextMode 
            && (PDF_RM_NormalPage != m_PdfModeController.m_eReadingMode
            || 90 == m_PdfModeController.m_iRotation
            || 270 == m_PdfModeController.m_iRotation
            || PDF_TPM_SinglePage != m_PdfModeController.m_eTurnPageMode)
            )
        )
    {
        m_bottomMidBtns[0].SetEnable(false);
    }
}

void UIReaderSettingDlg::UpdateBtnStatusOfFormatOrColumn()
{
    DWORD cmdID = GetCmdIdOfBottomMidBtns(1);
    bool visible = (cmdID != IDSTATIC);
    m_bottomMidBtns[1].SetVisible(visible);
    if (visible)
    {
        m_bottomMidBtns[1].Initialize(cmdID, GetStringOfBottomMidBtns(1), m_fontsize20);
        m_bottomMidBtns[1].SetIcons(GetImageOfFormatOrColumn(ICON_UNFOCUSED), GetImageOfFormatOrColumn(ICON_FOCUSED));
        m_bottomMidBtns[1].SetDisabledIcon(GetImageOfFormatOrColumn(ICON_DISABLE));
        UpdateEnableOfFormatOrColumn();
    }
}

void UIReaderSettingDlg::UpdateEnableOfFormatOrColumn()
{
    m_bottomMidBtns[1].SetEnable(true);
    if (IsComicsFrameMode()
        || (m_bIsScannedText 
            && !m_bIsPdfTextMode 
            && (m_PdfModeController.m_iRotation != 0
                || m_PdfModeController.m_eReadingMode == PDF_RM_ZoomPage
                || m_PdfModeController.m_eReadingMode == PDF_RM_AdaptiveWidth
                || m_PdfModeController.m_eReadingMode == PDF_RM_Rearrange
                || m_PdfModeController.m_eTurnPageMode != PDF_TPM_SinglePage)
            )
        )
    {
        m_bottomMidBtns[1].SetEnable(false);
    }
}

void UIReaderSettingDlg::UpdateBtnStatusOfRearrange()
{
    DWORD cmdID = GetCmdIdOfBottomMidBtns(2);
    bool visible = (cmdID != IDSTATIC);
    m_bottomMidBtns[2].SetVisible(visible);
    if (visible)
    {
        m_bottomMidBtns[2].Initialize(cmdID, GetStringOfBottomMidBtns(2), m_fontsize20);
        m_bottomMidBtns[2].SetIcons(GetImageOfRearrange(ICON_UNFOCUSED), GetImageOfRearrange(ICON_FOCUSED));
        m_bottomMidBtns[2].SetDisabledIcon(GetImageOfRearrange(ICON_DISABLE));
        m_bottomMidBtns[2].SetFontColor(ColorManager::knBlack);
    }
}

void UIReaderSettingDlg::UpdateBtnStatusOfBolden()
{
    DWORD cmdID = GetCmdIdOfBottomMidBtns(3);
    bool visible = (cmdID != IDSTATIC);
    m_bottomMidBtns[3].SetVisible(visible);
    if (visible)
    {
        m_bottomMidBtns[3].Initialize(cmdID, GetStringOfBottomMidBtns(3), m_fontsize20);
        m_bottomMidBtns[3].SetIcons(GetImageOfBolden(ICON_UNFOCUSED), GetImageOfBolden(ICON_FOCUSED));
        m_bottomMidBtns[3].SetDisabledIcon(GetImageOfBolden(ICON_DISABLE));
        m_bottomMidBtns[3].SetFontColor(ColorManager::knBlack);
    }
}

void UIReaderSettingDlg::UpdateBtnStatusOfSetting()
{
    m_btnSetting.Initialize(ID_BTN_TOUCH_READERSETTING, StringManager::GetPCSTRById(TOUCH_BOOKSETTING_SETTING), m_fontsize20);
    m_btnSetting.SetIcons(ImageManager::GetImage(IMAGE_TOUCH_ICON_SETTING), ImageManager::GetImage(IMAGE_TOUCH_ICON_SETTING_WHITE));
    m_btnSetting.SetDisabledIcon(ImageManager::GetImage(IMAGE_TOUCH_ICON_SETTING_DISABLE));
    m_btnSetting.SetEnable(!m_bIsImage);
}

void UIReaderSettingDlg::UpdateBottomLeftWndsCornerStyle()
{
    if(m_bIsTTSPlaying)
    {
        m_bottomLeftBtns[0].SetCornerStyle(false, true, true, false);
        m_bottomLeftBtns[1].SetCornerStyle(true, false, false, true);
    }
    else
    {
        m_bottomLeftBtns[0].SetCornerStyle(false, false, false, false);
    }
}

void UIReaderSettingDlg::UpdateBottomMidWndsCornerStyle()
{
    int beginIndex = -1;
    for (int i = 0; i < 4; ++i)
    {
        if (m_bottomMidBtns[i].IsVisible())
        {
            beginIndex = i;
            break;
        }
    }
    
    if (beginIndex >= 0)
    {
        int endIndex = -1;
        for (int i = 3; i >= 0; --i)
        {
            if (m_bottomMidBtns[i].IsVisible())
            {
                endIndex = i;
                break;
            }
        }

        if (beginIndex == endIndex)
        {
            m_bottomMidBtns[beginIndex].SetCornerStyle(false, false, false, false);
        }
        else
        {
            m_bottomMidBtns[beginIndex].SetCornerStyle(false,true,true,false);
            m_bottomMidBtns[endIndex].SetCornerStyle(true,false,false,true);
            for (int i = beginIndex + 1; i < endIndex; ++i)
            {
                m_bottomMidBtns[i].SetCornerStyle(true,true,true,true);
            }
        }
    }
}

void UIReaderSettingDlg::InitBottomWindows()
{
    UpdateBottomBtnsStatus();

    AppendChild(&m_bottomLeftBtns[0]);
    AppendChild(&m_bottomLeftBtns[1]);
    AppendChild(&m_bottomMidBtns[0]);
    AppendChild(&m_bottomMidBtns[1]);
    AppendChild(&m_bottomMidBtns[2]);
    AppendChild(&m_bottomMidBtns[3]);
    AppendChild(&m_btnSetting);

    const int minWidth = GetWindowMetrics(UIReaderSettingDlgButtonWidthIndex);
    const int minHeight = GetWindowMetrics(UIPixelValue60Index);
    m_bottomLeftBtns[0].SetMinSize(minWidth, minHeight);
    m_bottomLeftBtns[1].SetMinSize(minWidth, minHeight);
    m_bottomMidBtns[0].SetMinHeight(minHeight);
    m_bottomMidBtns[1].SetMinHeight(minHeight);
    m_bottomMidBtns[2].SetMinHeight(minHeight);
    m_bottomMidBtns[3].SetMinHeight(minHeight);
    m_btnSetting.SetMinSize(minWidth, minHeight);

    m_btnsBottom.clear();
    m_btnsBottom.push_back(&m_bottomLeftBtns[0]);
    m_btnsBottom.push_back(&m_bottomLeftBtns[1]);
    m_btnsBottom.push_back(&m_bottomMidBtns[0]);
    m_btnsBottom.push_back(&m_bottomMidBtns[1]);
    m_btnsBottom.push_back(&m_bottomMidBtns[2]);
    m_btnsBottom.push_back(&m_bottomMidBtns[3]);
    m_btnsBottom.push_back(&m_btnSetting);

    const int margin = GetWindowMetrics(UIReaderSettingDlgBottomBarButtonMarginIndex);
    m_bottomSizer->Clear();
    m_bottomSizer->Add(&m_bottomLeftBtns[0], UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
    m_bottomSizer->Add(&m_bottomLeftBtns[1], UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
    m_bottomSizer->AddSpacer(margin);
    m_bottomSizer->Add(&m_bottomMidBtns[0], UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
    m_bottomSizer->Add(&m_bottomMidBtns[1], UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
    m_bottomSizer->Add(&m_bottomMidBtns[2], UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
    m_bottomSizer->Add(&m_bottomMidBtns[3], UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
    m_bottomSizer->Add(&m_btnSetting, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Border(dkLEFT, margin));
}

void UIReaderSettingDlg::InitUI()
{
    SetSize(dkSize(GetScreenWidth(),GetScreenHeight()));
    if (!m_windowSizer)
    {
        //主布局采用竖直方向
        UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
        //添加标题栏
        mainSizer->Add(&m_titleBar);
        //上方采用水平方向布局
        m_topSizer = new UIBoxSizer(dkHORIZONTAL);
        m_topSizer->SetMinHeight(GetWindowMetrics(UIBackButtonHeightIndex));
        mainSizer->Add(m_topSizer, UISizerFlags().Expand().Border(dkRIGHT, GetWindowMetrics(UIHorizonMarginIndex)));
        mainSizer->AddStretchSpacer();
        //逐个添加中间的窗口组
        for( int i = 0; i < MID_WINDOWS_COUNTER; ++i)
        {
            if (i==MID_WINDOWS_COLUMNED || i==MID_WINDOWS_FONTSETTING || i==MID_WINDOWS_READERSETTING || i==MID_WINDOWS_CUTTINGEDGE || i == MID_WINDOWS_CHAPTER)
            {
                m_middleSizers[i] = new UIBoxSizer(dkVERTICAL);
            }
            else
            {
                m_middleSizers[i] = new UIBoxSizer(dkHORIZONTAL);
            }
            if(i == MID_WINDOWS_CHAPTER)
        	{
        		mainSizer->Add(m_middleSizers[i], UISizerFlags().Expand()
					.Border(dkTOP | dkBOTTOM, GetWindowMetrics(UIReaderSettingDlgMiddleButtonsVerticalMarginIndex)));
        	}
			else
			{
				mainSizer->Add(m_middleSizers[i], UISizerFlags().Expand()
                	.Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIHorizonMarginIndex))
                	.Border(dkTOP | dkBOTTOM, GetWindowMetrics(UIReaderSettingDlgMiddleButtonsVerticalMarginIndex)));
			}
            
        }
        //底部栏采用水平方向布局
        m_bottomSizer = new UIBoxSizer(dkHORIZONTAL);
        m_bottomSizer->SetMinHeight(GetWindowMetrics(UIReaderSettingDlgBottomBarHeightIndex));
        mainSizer->Add(m_bottomSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIHorizonMarginIndex)));
        SetSizer(mainSizer);
    }


    InitTopWindows();
    if (!m_bIsImage)
    {
        InitMiddleVolumeWindows();
        InitMiddleLayoutWindows();

        InitMiddleFontSettingWindows();
        UpdateFontSettingWindows();

        InitMiddleReaderSettingWindows();
        UpdateReaderSettingWindows();
    }
    InitMiddleCuttingEdge();
    InitMiddleChapterWindows();
    InitMiddleColumnedWindows();

    if (m_bIsScannedText || m_bIsImage) {
        InitMiddleRotateWindows();
    }
    InitBottomWindows();
    SetMiddlePosWins(MID_WINDOWS_CHAPTER);
}

void UIReaderSettingDlg::InitFont()
{
    DebugPrintf(DLC_UISYSTEMSETTING, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (m_bIsImage)
    {
        return;
    }

    /* 自定义支持中日韩字符集的字体 */
    BOOL fIsUserFontFound (FALSE);
    vector<DKFont*> lstFont;
    vector<string>::iterator viName;
    m_vCNFontName.clear();
    m_vENFontName.clear(); 
    
    if ((g_pFontManager->GetFontListByCharSet (DK_CHARSET_BIG5, lstFont) && GetFontNameList (lstFont, &m_vCNFontName)) |
        (g_pFontManager->GetFontListByCharSet (DK_CHARSET_SHITJIS, lstFont) && GetFontNameList (lstFont, &m_vCNFontName)) |
        (g_pFontManager->GetFontListByCharSet (DK_CHARSET_JOHAB, lstFont) && GetFontNameList (lstFont, &m_vCNFontName)) |
        (g_pFontManager->GetFontListByCharSet (DK_CHARSET_GB, lstFont) && GetFontNameList (lstFont, &m_vCNFontName)))
    {    
        sort(m_vCNFontName.begin(), m_vCNFontName.end());
        vector<string>::iterator vi = std::unique(m_vCNFontName.begin(), m_vCNFontName.end());
        m_vCNFontName.erase(vi, m_vCNFontName.end()); 

            viName = m_vCNFontName.begin();    // 默认显示第一个字体名
            m_btnLangChoices[0].SetText((*viName).c_str ());

            int iIndex = 0;
            for(viName = m_vCNFontName.begin (); viName != m_vCNFontName.end (); ++viName)
            {
                // 当前使用字体设置选中状态
                if (!fIsUserFontFound &&
                    0 == strcmp (SystemSettingInfo::GetInstance()->GetFontByCharset(DK_CHARSET_GB), (*viName).c_str ()))
                {
                    m_btnLangChoices[0].SetText((*viName).c_str ());
                    fIsUserFontFound = TRUE;
                    m_iSelectedCNFontIndex = iIndex;
                }
                iIndex++;
            }
    }

    lstFont.clear ();

    /* 自定义英文字体初始化 */
    fIsUserFontFound = FALSE;

    if (g_pFontManager->GetFontListByCharSet (DK_CHARSET_ANSI, lstFont) && GetFontNameList (lstFont, &m_vENFontName))
    {
        sort(m_vENFontName.begin(), m_vENFontName.end());
        vector<string>::iterator vi = unique(m_vENFontName.begin(), m_vENFontName.end());
        m_vENFontName.erase(vi, m_vENFontName.end());
        
        viName = m_vENFontName.begin ();    // 默认显示第一个字体名
        m_btnLangChoices[1].SetText((*viName).c_str ());

        int iIndex = 0;
        for(viName = m_vENFontName.begin (); viName != m_vENFontName.end (); ++viName)
        {
            if (!fIsUserFontFound &&
                0 == strcmp (SystemSettingInfo::GetInstance()->GetFontByCharset(DK_CHARSET_ANSI), (*viName).c_str ()))
            {
                m_btnLangChoices[1].SetText((*viName).c_str ());
                fIsUserFontFound = TRUE;
                m_iSelectedENFontIndex = iIndex;
            }
            iIndex++;
        }
    }
    
    m_iSelectedCNFontIndex = m_iSelectedCNFontIndex >= 0 ? m_iSelectedCNFontIndex : 0;
    m_iSelectedENFontIndex = m_iSelectedENFontIndex >= 0 ? m_iSelectedENFontIndex : 0;
    lstFont.clear ();
    
    DebugPrintf(DLC_UISYSTEMSETTING, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

//-------------------------------------------
// Summary:
//      获取可供UI界面显示的字体名称
//  [in]lstFont      - 存放DKFont指针的容器
//  [out]lstFontName - 从@lstFont中提取到的字体名字
//-------------------------------------------
bool UIReaderSettingDlg::GetFontNameList (std::vector<DKFont*> &lstFont, std::vector<string>* pLstFontName)
{
    DK_CHAR* pFontName (DK_NULL);
    std::vector<DKFont*>::iterator viFont;
    for (viFont = lstFont.begin (); viFont != lstFont.end (); ++viFont)
    {
        vector<wstring>* lstFullName = (*viFont)->GetFullFontName();
        if (lstFullName && lstFullName->size() > 0)
        {
            // 取该字体的最后一个字体名(中文字体的中文名排在英文名之后)，压入出参pLstFontName
            pFontName = EncodingUtil::WCharToChar (lstFullName->at (lstFullName->size () - 1).c_str ());
            if (pFontName && strlen (pFontName) > 0)
            {
                (*pLstFontName).push_back (string (pFontName));
            }
            SafeFreePointer (pFontName);
        }
    }
    return true;
}

void UIReaderSettingDlg::ENLanguageChangedHandler(int iIndex)
{
    SQM::GetInstance()->IncCounter(SQM_ACTION_SETTINGPAGE_SET_ENGLISH_FONT);
    CString font_name =m_btnLangChoices[1].GetText();

    if(0 == strcmp(SystemSettingInfo::GetInstance()->GetFontByCharset(DK_CHARSET_ANSI), font_name.GetBuffer()))
    {
        return; // 字体未改变
    }

    DK_WCHAR* pName = EncodingUtil::CharToWChar (font_name.GetBuffer ());
    if (!pName)
    {
        return;
    }

    // 更新fontmanager
    wstring strFontName = pName;
    SafeFreePointer (pName);
    g_pFontManager->SetDefaultFontFaceNameForCharSet (DK_CHARSET_ANSI, strFontName);

    // Kernel 设置默认字体
    SetKernelDefaultFonts ();

#if 0
    // 更新最近使用字体
    if(0 == strcmp(SystemSettingInfo::GetInstance()->GetConfigValue(CKI_EnglishFont_1), font_name.GetBuffer()))
    {
        SystemSettingInfo::GetInstance()->SetConfigValue(CKI_EnglishFont_1, SystemSettingInfo::GetInstance()->GetConfigValue(CKI_EnglishFont));
    }
    else
    {
        SystemSettingInfo::GetInstance()->SetConfigValue(CKI_EnglishFont_2, SystemSettingInfo::GetInstance()->GetConfigValue(CKI_EnglishFont_1));
        SystemSettingInfo::GetInstance()->SetConfigValue(CKI_EnglishFont_1, SystemSettingInfo::GetInstance()->GetConfigValue(CKI_EnglishFont));
    }
#endif
    SystemSettingInfo::GetInstance()->SetFontNameChanged(true);
    SystemSettingInfo::GetInstance()->SetFontByCharset(DK_CHARSET_ANSI, font_name.GetBuffer());
}

void UIReaderSettingDlg::CNLanguageChangedHandler(int iIndex)
{
    SQM::GetInstance()->IncCounter(SQM_ACTION_SETTINGPAGE_SET_CHINESE_FONT);
    CString font_name = m_btnLangChoices[0].GetText();

    if(strcmp(SystemSettingInfo::GetInstance()->GetFontByCharset(DK_CHARSET_GB), font_name.GetBuffer()) == 0)
    {
        return; // 字体未改变
    }

    DK_WCHAR* pName = EncodingUtil::CharToWChar (font_name.GetBuffer ());
    if (!pName)
    {
        return;
    }

    // 更新fontmanager
    wstring strFontName = pName;
    SafeFreePointer (pName);

    DKFont* pFont = g_pFontManager->GetFontForFaceName (strFontName);
    assert(pFont);
    if (!pFont)
    {
        return;
    }

    bool bSupportPRC (false);
    vector<unsigned short> lstCharSet;
    vector<unsigned short>::iterator it;
    pFont->GetSupportedCharSet (lstCharSet);
    for (it = lstCharSet.begin(); it != lstCharSet.end(); ++it)
    {
        if (DK_CHARSET_GB == *it || DK_CHARSET_BIG5 == *it)
        {
            bSupportPRC = true;
            break;
        }
    }

    // 针对该字体是否支持中文字符集做不同的默认字体设置
    // 当要设置的字体支持中文时，将所有字符集的默认字体均设置为该字体
    // 当要设置的字体不支持中文时，将中文字符集的默认字体设置为系统默认字体
    if (bSupportPRC)
    {
        g_pFontManager->SetDefaultFontFaceNameForCharSet (DK_CHARSET_GB, strFontName);
        g_pFontManager->SetDefaultFontFaceNameForCharSet (DK_CHARSET_SHITJIS, strFontName);
        g_pFontManager->SetDefaultFontFaceNameForCharSet (DK_CHARSET_JOHAB, strFontName);
        g_pFontManager->SetDefaultFontFaceNameForCharSet (DK_CHARSET_BIG5, strFontName);
    }
    else
    {
        g_pFontManager->SetDefaultFontFaceNameForCharSet (DK_CHARSET_SHITJIS, strFontName);
        g_pFontManager->SetDefaultFontFaceNameForCharSet (DK_CHARSET_JOHAB, strFontName);

        wstring strSysDefaultFont (L"");
        g_pFontManager->GetSystemDefaultFontFaceName(&strSysDefaultFont);
        g_pFontManager->SetDefaultFontFaceNameForCharSet (DK_CHARSET_GB, strSysDefaultFont);
        g_pFontManager->SetDefaultFontFaceNameForCharSet (DK_CHARSET_BIG5, strSysDefaultFont);
    }

    // Kernel 设置默认字体
    SetKernelDefaultFonts ();

#if 0
    // 更新最近使用字体
    if(0 == strcmp(SystemSettingInfo::GetInstance()->GetConfigValue(CKI_ChineseFont_1), font_name.GetBuffer()))
    {
        SystemSettingInfo::GetInstance()->SetConfigValue(CKI_ChineseFont_1, SystemSettingInfo::GetInstance()->GetConfigValue(CKI_ChineseFont));
    }
    else
    {
        SystemSettingInfo::GetInstance()->SetConfigValue(CKI_ChineseFont_2, SystemSettingInfo::GetInstance()->GetConfigValue(CKI_ChineseFont_1));
        SystemSettingInfo::GetInstance()->SetConfigValue(CKI_ChineseFont_1, SystemSettingInfo::GetInstance()->GetConfigValue(CKI_ChineseFont));
    }
#endif
    SystemSettingInfo::GetInstance()->SetFontNameChanged(true);
    SystemSettingInfo::GetInstance()->SetFontByCharset(DK_CHARSET_GB, font_name.GetBuffer());

}

void UIReaderSettingDlg::ShowCNLangSettingDlg()
{
    if (m_bIsImage)
        return ;
    
    if (m_vCNFontName.size() > 0)
    {
        UIButtonDlg dlgCNFont(this); 
        dlgCNFont.SetData(m_vCNFontName, m_iSelectedCNFontIndex);

        int iHeight = dlgCNFont.GetTotalHeight();
        int iTop = m_btnLangChoices[0].GetY() - iHeight;
        dlgCNFont.MoveWindow(m_btnLangChoices[0].GetX(), 
            iTop,
            GetWindowMetrics(UIReaderSettingDlgFontSettingButtonWidthIndex),
            iHeight);
        m_btnLangChoices[0].SetFocus(TRUE);
        AppendChild(&dlgCNFont);
        dlgCNFont.DoModal();
        m_btnLangChoices[0].SetFocus(FALSE);
        int iSelectIndex = dlgCNFont.GetSelectedButtonID() - UIButtonDlg::BI_Base;
        if (iSelectIndex >= 0 && iSelectIndex != m_iSelectedCNFontIndex)
        {
            vector<string>::iterator viSelectedName = m_vCNFontName.begin() + iSelectIndex;
            m_btnLangChoices[0].SetText(viSelectedName->c_str());
            m_iSelectedCNFontIndex = iSelectIndex;
            CNLanguageChangedHandler(iSelectIndex);
            OnBookParseAttributesChanged();
        }
        //Repaint();
    }

}

void UIReaderSettingDlg::ShowENLangSettingDlg()
{
    if (m_bIsImage)
        return ;
    
    if (m_vENFontName.size() > 0)
    {
        UIButtonDlg dlgENFont(this); 
        dlgENFont.SetData(m_vENFontName, m_iSelectedENFontIndex);
        int iHeight = dlgENFont.GetTotalHeight();
        int iTop = m_btnLangChoices[1].GetY() - iHeight;
        dlgENFont.MoveWindow(m_btnLangChoices[1].GetX(), 
            iTop,
            GetWindowMetrics(UIReaderSettingDlgFontSettingButtonWidthIndex),
            iHeight);
        m_btnLangChoices[1].SetFocus(TRUE);
        dlgENFont.DoModal();
        m_btnLangChoices[1].SetFocus(FALSE);
        int iSelectIndex = dlgENFont.GetSelectedButtonID() - UIButtonDlg::BI_Base;
        if (iSelectIndex >= 0 && iSelectIndex != m_iSelectedENFontIndex)
        {
            vector<string>::iterator viSelectedName = m_vENFontName.begin() + iSelectIndex;
            m_btnLangChoices[1].SetText(CString(viSelectedName->c_str()));
            m_iSelectedENFontIndex = iSelectIndex;
            ENLanguageChangedHandler(iSelectIndex);
            OnBookParseAttributesChanged();
        }
        //Repaint();
    }
}

void UIReaderSettingDlg::LoadReaderSettingInfo()
{
    if (m_bIsImage)
    {
        return;
    }
    
    SystemSettingInfo* systemSettingInfo = SystemSettingInfo::GetInstance();
    if(systemSettingInfo)
    {
        m_iCurEmBoldenLevel = systemSettingInfo->GetTextDarkLevel();
        m_iCurFontSizeIndex = LayoutHelper::GetFontSizeIndex();
        m_iCurLayoutStyle = (LayoutStyle)systemSettingInfo->GetTypeSetting();
        m_curFontStyle = systemSettingInfo->GetFontStyle_FamiliarOrTraditional();
        m_iCurReadingLayout = systemSettingInfo->GetReadingLayout();
    }
    else
    {
        m_iCurEmBoldenLevel= 2;//TODO: get from table the same as fontsize
        m_iCurFontSizeIndex = 1;
        m_iCurLayoutStyle = LAYOUT_STYLE_COMPACT;
        m_curFontStyle = 0;
        m_iCurReadingLayout = 0;
    }

    if (m_bIsScannedText) {
        const PdfModeController* pModeController = m_pBookReader->GetPdfModeController();
        if (pModeController)
        {
            m_PdfModeController = *pModeController;
            m_bIsPdfTextMode = (PDF_RM_Rearrange == m_PdfModeController.m_eReadingMode);
        }
        m_dkxBlock->GetPDFDarkenLevel(&m_iCurPdfEmBoldenLevel);
    }
}

void UIReaderSettingDlg::OnChildSetFocus(UIWindow* pWindow)
{
    bool findFlag = false;
    for ( size_t i=0; i<m_btnsBottom.size(); ++i)
        if (m_btnsBottom[i]==pWindow)
        {
            findFlag = true;
            break;
        }
    if (findFlag) {
        for ( size_t i=0; i<m_btnsBottom.size(); ++i)
            if (m_btnsBottom[i]!=pWindow)
                m_btnsBottom[i]->SetFocus(false);
    }
}

int UIReaderSettingDlg::GetCurFontSize() const
{
    const uint32_t *fontSizeTable = LayoutHelper::GetFontSizeTable();
    if (m_iCurFontSizeIndex<0 || m_iCurFontSizeIndex>=(int)LayoutHelper::GetFontSizeTableSize()) 
    {
        DebugPrintf(DLC_ERROR, "error font size index %s::%s", GetClassName(), __FUNCTION__);
        return 22;
    }
    return fontSizeTable[m_iCurFontSizeIndex];
}

LayoutStyle UIReaderSettingDlg::GetCurLayoutStyle() const
{
    return m_iCurLayoutStyle;
}

int UIReaderSettingDlg::GetCurFontStyle() const
{
    return m_curFontStyle;
}

int UIReaderSettingDlg::GetCurEmBoldenLevel() const
{
    return m_iCurEmBoldenLevel;
}

int UIReaderSettingDlg::GetCurReadingLayoutMode() const
{
    return m_iCurReadingLayout;
}

void UIReaderSettingDlg::RefreshRotateFocus()
{
    for ( int i=0; i<4; i++)
    {
        m_btnRotate[i].SetFocus(false);
    }

    int iRotation = 0;

    if (m_bIsImage && m_imageController)
    {
        iRotation = m_imageController->m_iRotation;
    }
    else if(m_bIsScannedText && !m_bIsPdfTextMode)
    {
        iRotation = m_PdfModeController.m_iRotation;
    }

    int index = iRotation / 90;
    if (index < 0 || index > 4)
    {
        index = 0;
    }

    m_btnRotate[index].SetFocus(true);
    return;
}

void UIReaderSettingDlg::RefreshContentDisplayFocus()
{
    if(m_bIsScannedText && !m_bIsPdfTextMode)
    {
        bool isAdaptiveWidth = m_PdfModeController.m_eReadingMode == PDF_RM_AdaptiveWidth;
        m_btnContentDisplay[0].SetFocus(!isAdaptiveWidth);
        m_btnContentDisplay[1].SetFocus(isAdaptiveWidth);
    }
}

void UIReaderSettingDlg::RefreshTurnPageCustomFocus()
{
    //翻页习惯
    int flagTurnPage = SystemSettingInfo::GetInstance()->GetTurnPageFlag();
    m_btnTurnPageCustom[0].SetFocus(flagTurnPage == 0);
    m_btnTurnPageCustom[1].SetFocus(flagTurnPage == 1);
}

void UIReaderSettingDlg::RefreshReadingUpDownGestureCustomFocus()
{
    //换章
    int flagMoveGusture = SystemSettingInfo::GetInstance()->ShouldTreatUpDownGestureAsPageSwitch();
    m_btnReadingUpDownGestureCustom[0].SetFocus(flagMoveGusture == 0);
    m_btnReadingUpDownGestureCustom[1].SetFocus(flagMoveGusture == 1);
}

void UIReaderSettingDlg::RefreshProgressBarFocus()
{
    if (!m_bIsImage && !IsComicsChapter() && (!m_bIsScannedText || m_bIsPdfTextMode))
    {
        int progressBar = SystemSettingInfo::GetInstance()->GetProgressBarMode();
        m_btnProgressBar[0].SetFocus(progressBar == 0);
        m_btnProgressBar[1].SetFocus(progressBar == 1);
    }
}

void UIReaderSettingDlg::RefreshFontRenderFocus()
{
    int fontRender = SystemSettingInfo::GetInstance()->GetFontRender();
    m_btnFontRender[0].SetFocus(fontRender == 0);
    m_btnFontRender[1].SetFocus(fontRender == 1);
}

void UIReaderSettingDlg::RefreshReaderSettingFocus()
{
    RefreshTurnPageCustomFocus();
    RefreshReadingUpDownGestureCustomFocus();
    RefreshContentDisplayFocus();
    RefreshProgressBarFocus();
    RefreshFontRenderFocus();
}

void UIReaderSettingDlg::RefreshFontStyleFocus()
{
    for (int i=0; i<2; ++i) {
        m_btnModifyFontStyle[i].SetFocus(false);
    }
    m_btnModifyFontStyle[m_curFontStyle%2].SetFocus(true);
}

void UIReaderSettingDlg::InitWindowForRearRange()
{
    if (m_bIsImage || !m_bIsScannedText) {
        return;
    }
    UpdateReaderSettingWindows();
    UpdateFontSettingWindows();
    RefreshFontBoldenEnable((m_bIsScannedText && !m_bIsPdfTextMode) ? m_iCurPdfEmBoldenLevel : m_iCurEmBoldenLevel);
    SetMiddlePosWins(m_curMidWins);
}

void UIReaderSettingDlg::SetPdfModeController(PdfModeController* pdfModeController)
{
    if (!m_bIsScannedText) {
        return;
    }

    m_PdfModeController = *pdfModeController;
    if (PDF_RM_Rearrange != m_PdfModeController.m_eReadingMode)
    {
        m_bIsPdfTextMode = false;
    }
    else
    {
        m_bIsPdfTextMode = true;
    }

}

const PdfModeController UIReaderSettingDlg::GetPdfModeController() const
{
    return m_PdfModeController;
}

HRESULT UIReaderSettingDlg::DrawBackGround(DK_IMAGE drawingImg)
{
    DK_IMAGE* pDemoImage = NULL;
    //LayoutReader layoutReader(this);
    if(m_bIsImage && m_pContainer)
    {
        pDemoImage = ((UIImageReaderContainer*)(m_pContainer))->GetCurImage();
    }
    else if (!m_bIsPdfRearRanging)
    {
        if(m_bIsScannedText && !m_bIsPdfTextMode)
        {
            pDemoImage = m_pBookReader->GetPageBMP();
        }
        else if (m_bIsPreviewImageReady)
        {
            pDemoImage = GetPreviewImage();
        }
    }
    
    HRESULT hr(S_OK);
    DK_IMAGE imgSelf;
    CTpGraphics grf(drawingImg);
	grf.EraserBackGround(ColorManager::knWhite);

    if (pDemoImage)
    {
        const int readingLayoutMode = m_iCurReadingLayout;
        int iTopMargin = SystemSettingInfo::GetInstance()->GetPageTopMargin(readingLayoutMode);
        int iBottomMargin  = SystemSettingInfo::GetInstance()->GetPageBottomMargin(readingLayoutMode);
        int iHorizontalMargin = SystemSettingInfo::GetInstance()->GetPageHorizontalMargin(readingLayoutMode);
        if (m_bIsScannedText && !m_bIsPdfTextMode)
        {
            iTopMargin = 0;
            iBottomMargin = 1;
            iHorizontalMargin = 0;
        }
        DK_RECT rcSelf = {m_iLeft+ iHorizontalMargin , m_iTop + iTopMargin 
            , m_iLeft + m_iWidth - iHorizontalMargin, m_iTop + m_iHeight - iBottomMargin };
        int iSelfWidth = rcSelf.right - rcSelf.left;
        int iSelfHeight = rcSelf.bottom - rcSelf.top;

        if (iSelfWidth > pDemoImage->iWidth)
        {
            int iTmp = iSelfWidth - pDemoImage->iWidth;
            rcSelf.left += iTmp >> 1;
            rcSelf.right -= iTmp - (iTmp >> 1);
        }
        if (pDemoImage->iWidth > iSelfWidth && pDemoImage->iWidth <= m_iWidth)
        {
            int iTmp = m_iWidth - pDemoImage->iWidth;
            rcSelf.left = iTmp / 2;
            rcSelf.right = m_iWidth - rcSelf.left;
        }
        if (iSelfHeight > pDemoImage->iHeight)
        {
            int iTmp = iSelfHeight - pDemoImage->iHeight;
            rcSelf.top += iTmp >> 1;
            rcSelf.bottom -= iTmp - (iTmp >> 1);
        }
        if (pDemoImage->iHeight > iSelfHeight && pDemoImage->iHeight <= m_iHeight)
        {
            int iTmp = m_iHeight - pDemoImage->iHeight;
            rcSelf.top = iTmp / 2;
            rcSelf.bottom = m_iHeight - rcSelf.top;
        }
        RTN_HR_IF_FAILED(CropImage(drawingImg, rcSelf, &imgSelf));
        CopyImage(imgSelf, *pDemoImage);
    }
    else
    {
        DKFontAttributes fontattr;
        fontattr.SetFontAttributes(0,0,GetWindowFontSize(FontSize32Index));
        ITpFont* pFont = FontManager::GetInstance()->GetFont(fontattr, ColorManager::knBlack);
        const char* tipText = StringManager::GetPCSTRById(SCREENSAVER_OUT_MESSAGE);
        const int tipHeight = pFont->GetHeight();
        const int tipWidth = pFont->StringWidth(tipText);
        if (m_topSizer && m_curMidWins < MID_WINDOWS_COUNTER && m_middleSizers[m_curMidWins])
        {
            const int top = m_topSizer->GetPosition().y + m_topSizer->GetSize().GetHeight();
            const int bottom = m_middleSizers[m_curMidWins]->GetPosition().y;
            if (NULL != pFont)
            {
                grf.DrawStringUtf8(tipText, (m_iWidth - tipWidth) >> 1, (top + bottom - tipHeight) >> 1, pFont);
            }
        }
    }
    return hr;
}

HRESULT UIReaderSettingDlg::Draw(DK_IMAGE drawingImg)
{
    if (!m_bIsVisible)
    {
        return S_OK;
    }

    if (drawingImg.pbData == NULL)
    {
        return E_FAIL;
    }
    HRESULT hr(S_OK);
    DK_IMAGE imgSelf;
    DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
    RTN_HR_IF_FAILED(CropImage(drawingImg, rcSelf, &imgSelf));

    RTN_HR_IF_FAILED(DrawBackGround(imgSelf));

    CTpGraphics grf(imgSelf);
    int iLeft = m_iLeft;
    int iTop1 = m_iTop + GetWindowMetrics(UITitleBarHeightIndex);
    int iTop2 = m_iTop + m_iHeight - GetWindowMetrics(UIReaderSettingDlgBottomBarHeightIndex) - m_middleSizers[m_curMidWins]->GetSize().GetHeight();
    iTop2 -= (GetWindowMetrics(UIReaderSettingDlgMiddleButtonsVerticalMarginIndex)<<1);
    int iTop3 = m_iTop + m_iHeight - GetWindowMetrics(UIReaderSettingDlgBottomBarHeightIndex);

    int iRight = m_iLeft + m_iWidth - (UIREADERSETTING_BORDER << 1);
    RTN_HR_IF_FAILED(grf.FillRect(iLeft, iTop1, iRight, iTop1 + GetWindowMetrics(UIBackButtonHeightIndex) + 3, ColorManager::knWhite));
    RTN_HR_IF_FAILED(grf.FillRect(iLeft, iTop2, iRight, m_iTop + m_iHeight - UIREADERSETTING_BORDER, ColorManager::knWhite));

    RTN_HR_IF_FAILED(grf.DrawLine(iLeft, iTop1 + GetWindowMetrics(UIBackButtonHeightIndex), m_iWidth, 2, SOLID_STROKE));
    RTN_HR_IF_FAILED(grf.DrawLine(iLeft, iTop2, m_iWidth, 2, SOLID_STROKE));
    RTN_HR_IF_FAILED(grf.DrawLine(iLeft, iTop3, m_iWidth, 1, DOTTED_STROKE));
    //drawing child wins
    UINT32 iSize = GetChildrenCount();
    for (UINT32 i = 0; i < iSize; i++)
    {
        UIWindow* pWin = GetChildByIndex(i);
        if (pWin)
        {
            pWin->Draw(imgSelf);
        }
    }
    return (hr == S_OK);
}

void UIReaderSettingDlg::VolumeChangedHandler(DWORD dwCmdId)
{
    CTextToSpeech* pTTS = CTextToSpeech::GetInstance();
    int volume = pTTS->TTS_GetVolume();
    if (dwCmdId == ID_BTN_TOUCH_INCRETTSVOLUME)
    {
        volume++;
    }
    else if (dwCmdId == ID_BTN_TOUCH_DECRETTSVOLUME)
    {
        volume--;
    }
    volume = (volume <= 0 ? 0 : volume);
    volume = (volume >= 14 ? 14 : volume);
    
    pTTS->TTS_SetVolume(volume);
    
    if(volume == 14)
    {
        m_btnModifyVolume[1].SetEnable(false);
    }
    else
    {
        m_btnModifyVolume[1].SetEnable(true);
    }
    if(volume == 0)
    {
        m_btnModifyVolume[0].SetEnable(false);
    }
    else
    {
        m_btnModifyVolume[0].SetEnable(true);
    }

    UpdateWindow(m_iLeft, m_topSizer->GetSize().GetHeight(), m_iWidth, m_iHeight - m_topSizer->GetSize().GetHeight() - GetWindowMetrics(UIReaderSettingDlgBottomBarHeightIndex));
}

void UIReaderSettingDlg::RefreshFontSizeEnable(int fontSize)
{
    if (!m_bIsImage && !IsComicsChapter() && (!m_bIsScannedText || m_bIsPdfTextMode))
    {
        const int tableSize = (int)LayoutHelper::GetFontSizeTableSize();
        m_btnModifyFontSize[0].SetEnable(fontSize > 0);
        m_btnModifyFontSize[1].SetEnable(fontSize < (tableSize - 1));
    }
}

void UIReaderSettingDlg::FontSizeChangedHandler(DWORD dwCmdId)
{
    if (m_bIsImage)
        return ;
    
    if (dwCmdId==ID_BTN_TOUCH_INCREFONTSIZE)
        m_iCurFontSizeIndex++;
    else if (dwCmdId==ID_BTN_TOUCH_DECREFONTSIZE)
        m_iCurFontSizeIndex--;
    else
        return;
    const int tableSize = (int)LayoutHelper::GetFontSizeTableSize();
    m_iCurFontSizeIndex = dk_max(0, m_iCurFontSizeIndex);
    m_iCurFontSizeIndex = dk_min(m_iCurFontSizeIndex, tableSize - 1);

    RefreshFontSizeEnable(m_iCurFontSizeIndex);
    OnBookParseAttributesChanged();
    UpdateWindow(m_iLeft, m_topSizer->GetSize().GetHeight(), m_iWidth, m_iHeight - m_topSizer->GetSize().GetHeight() - GetWindowMetrics(UIReaderSettingDlgBottomBarHeightIndex));
}

void UIReaderSettingDlg::RefreshFontBoldenEnable(int bolden)
{
    m_btnModifyEmbolden[0].SetEnable(bolden > 0);
    m_btnModifyEmbolden[1].SetEnable(bolden < maxBoldenValue);
}

void UIReaderSettingDlg::EmBoldenChangedHandler(DWORD dwCmdId)
{
    switch (dwCmdId) {
    case ID_BTN_TOUCH_INCREEMBOLDEN:
        m_iCurEmBoldenLevel++;
        break;
    case ID_BTN_TOUCH_DECREEMBOLDEN:
        m_iCurEmBoldenLevel--;
        break;
    default:
        break;
    }
    m_iCurEmBoldenLevel = dk_max(0, m_iCurEmBoldenLevel);
    m_iCurEmBoldenLevel = dk_min(m_iCurEmBoldenLevel, maxBoldenValue);
    RefreshFontBoldenEnable(m_iCurEmBoldenLevel);

    OnBookParseAttributesChanged();
    UpdateWindow(m_iLeft, m_topSizer->GetSize().GetHeight(), m_iWidth, m_iHeight - m_topSizer->GetSize().GetHeight() - GetWindowMetrics(UIReaderSettingDlgBottomBarHeightIndex));
}

void UIReaderSettingDlg::CuttingEdgeChangedHandler(DWORD dwCmdId)
{
    if (ID_BTN_TOUCH_CUTTINGEDGE_SMART == dwCmdId)
    {
        m_PdfModeController.m_bIsSmartCutting = !m_PdfModeController.m_bIsSmartCutting; 
        m_btnSmartCuttingSwitch.SetIcon(m_PdfModeController.m_bIsSmartCutting ?
            ImageManager::GetImage(IMAGE_TOUCH_SWITCH_ON) :
            ImageManager::GetImage(IMAGE_TOUCH_SWITCH_OFF));

        if (m_PdfModeController.m_bIsSmartCutting)
        {
            m_PdfModeController.m_eCuttingEdgeMode = PDF_CEM_Smart;
        }
        else
        {
            m_PdfModeController.m_eCuttingEdgeMode = PDF_CEM_Normal;
        }
        UpdateWindow();
    }
    
    if (ID_BTN_TOUCH_CUTTINGEDGE_NORMAL == dwCmdId || ID_BTN_TOUCH_CUTTINGEDGE_ODDEVEN == dwCmdId)
    {
        if (!m_pPdfTrimColumnedDlg) 
        {
            m_pPdfTrimColumnedDlg = new UIPdfTrimColumnedDlg(this, m_pBookReader, UIPdfTrimColumnedDlg::PdfTrimAction);
        }
        if (!m_pPdfTrimColumnedDlg)
        {
            return;
        }
        PdfModeController newModeController = m_PdfModeController;
        if (m_pBookReader)
        {
            m_pBookReader->SetPdfModeController(&newModeController);
        }

        m_pPdfTrimColumnedDlg->SetAction(UIPdfTrimColumnedDlg::PdfTrimAction);

        if (m_pPdfTrimColumnedDlg->DoModal() > 0)
        {
            m_pPdfTrimColumnedDlg->GetTrimColumnedChangedArgs(newModeController);
            newModeController.m_bIsNormalCutting = (ID_BTN_TOUCH_CUTTINGEDGE_NORMAL == dwCmdId);
            int iCurPageIndex = GetCurPageIndex();
            if ((ID_BTN_TOUCH_CUTTINGEDGE_NORMAL != dwCmdId) && ((iCurPageIndex % 2) != 0))
            {
                swap(newModeController.m_dLeftEdge, newModeController.m_dRightEdge);
            }
            m_PdfModeController = newModeController;
        }
        else if (m_pBookReader)
        {
            m_pBookReader->SetPdfModeController(&m_PdfModeController);
        }
        m_btnSmartCuttingSwitch.SetIcon(m_PdfModeController.m_bIsSmartCutting ?
            ImageManager::GetImage(IMAGE_TOUCH_SWITCH_ON) :
            ImageManager::GetImage(IMAGE_TOUCH_SWITCH_OFF));
        m_endFlag = UIReaderSettingDlg::Normal;
        EndDialog(0);
    }
    UpdateBtnStatusOfFontsettingOrTrim();
}

void UIReaderSettingDlg::PdfEmBoldenChangedHandler(DWORD dwCmdId)
{
    switch (dwCmdId) {
    case ID_BTN_TOUCH_INCREEMBOLDEN:
        m_iCurPdfEmBoldenLevel++;
        break;
    case ID_BTN_TOUCH_DECREEMBOLDEN:
        m_iCurPdfEmBoldenLevel--;
        break;
    default:
        break;
    }

    m_iCurPdfEmBoldenLevel = dk_max(0, m_iCurPdfEmBoldenLevel);
    m_iCurPdfEmBoldenLevel = dk_min(m_iCurPdfEmBoldenLevel, maxBoldenValue);
    RefreshFontBoldenEnable(m_iCurPdfEmBoldenLevel);

    for (int i=0; i<2; i++)
        m_btnModifyEmbolden[i].UpdateWindow();
}

void UIReaderSettingDlg::FontStyleChangedHandler(DWORD dwCmdId)
{
    if (m_bIsImage)
        return ;
    int oldFontStyle = m_curFontStyle;

    switch (dwCmdId) {
    case ID_BTN_TOUCH_FONTSTYLE_FAMILIARFONT:
        m_curFontStyle = 0;
        break;
    case ID_BTN_TOUCH_FONTSTYLE_TRADITIONALFONT:
        m_curFontStyle = 1;
        break;
    default:
        break;
    }
    RefreshFontStyleFocus();

    if (m_curFontStyle == oldFontStyle)
        return;

    OnBookParseAttributesChanged();
    UpdateWindow(m_iLeft, GetWindowMetrics(UIBackButtonHeightIndex), m_iWidth, m_iHeight - GetWindowMetrics(UIReaderSettingDlgBottomBarHeightIndex) - GetWindowMetrics(UIBackButtonHeightIndex));
}

void UIReaderSettingDlg::ReadingLayoutChangeHandler(DWORD dwCmdId)
{
    if (m_bIsImage)
        return;
    
    for ( int i=0; i<2; ++i) {
        m_btnReadingLayout[i].SetFocus(false);
    }
    
    switch (dwCmdId) {
    case ID_BTN_TOUCH_READERLAYOUT_FULLSCREEN:
        m_iCurReadingLayout = 0;
        m_btnReadingLayout[0].SetFocus(true);
        break;
    case ID_BTN_TOUCH_READERLAYOUT_SHOWREADERMESSAGE:
        m_iCurReadingLayout = 1;
        m_btnReadingLayout[1].SetFocus(true);
        break;
    default:
        break;
    }

    OnBookParseAttributesChanged();
    UpdateWindow(m_iLeft, m_topSizer->GetSize().GetHeight(), m_iWidth, m_iHeight - m_topSizer->GetSize().GetHeight() - GetWindowMetrics(UIReaderSettingDlgBottomBarHeightIndex));
}

void UIReaderSettingDlg::LayoutStyleChangedHandler(DWORD dwCmdId)
{
    if (m_bIsImage)
        return ;
    
    for ( int i=0; i<4; ++i) {
        m_btnLayoutStyleChoices[i].SetFocus(false);
    }
    LayoutStyle m_lastLayoutStyle = m_iCurLayoutStyle;
    switch (dwCmdId) {
    case ID_BTN_TOUCH_LAYOUTSTYLE_ELEGANT:
        m_iCurLayoutStyle = LAYOUT_STYLE_ELEGANT;
        m_btnLayoutStyleChoices[0].SetFocus(true);
        break;
    case ID_BTN_TOUCH_LAYOUTSTYLE_STANDARD:
        m_iCurLayoutStyle = LAYOUT_STYLE_STANDARD;
        m_btnLayoutStyleChoices[1].SetFocus(true);
        break;
    case ID_BTN_TOUCH_LAYOUTSTYLE_COMPACT:
        m_iCurLayoutStyle = LAYOUT_STYLE_COMPACT;
        m_btnLayoutStyleChoices[2].SetFocus(true);
        break;
    case ID_BTN_TOUCH_LAYOUTSTYLE_NORMAL:
        m_iCurLayoutStyle = LAYOUT_SYSLE_NORMAL;
        m_btnLayoutStyleChoices[3].SetFocus(true);
        break;
    default:
        break;
    }

    if (m_iCurLayoutStyle==m_lastLayoutStyle)
        return;
    
    OnBookParseAttributesChanged();
    UpdateWindow(m_iLeft, m_topSizer->GetSize().GetHeight(), m_iWidth, m_iHeight - GetWindowMetrics(UIReaderSettingDlgBottomBarHeightIndex) - m_topSizer->GetSize().GetHeight());
}

void UIReaderSettingDlg::TurnPageCustomChangedHandler(DWORD dwCmdId)
{
    SystemSettingInfo::GetInstance()->SetTurnPageFlag(dwCmdId == ID_BTN_TOUCH_PAGEDOWN_CUSTOM);
    RefreshTurnPageCustomFocus();

    for ( int i=0; i<2; ++i)
        m_btnTurnPageCustom[i].UpdateWindow();
}

void UIReaderSettingDlg::ReadingUpDownGestureCustomChangedHandler(DWORD dwCmdId)
{
    SystemSettingInfo::GetInstance()->SetTreatUpDownGestureAsPageSwitch(dwCmdId == ID_BTN_TOUCH_READINGUPDOWN_GESTURE_PAGE);
    RefreshReadingUpDownGestureCustomFocus();

    for ( int i=0; i<2; ++i)
        m_btnReadingUpDownGestureCustom[i].UpdateWindow();
}


void UIReaderSettingDlg::RotateScreenHandler(DWORD dwCmdId)
{

    switch (dwCmdId) {
    case ID_BTN_TOUCH_ROTATE_0:
    case ID_BTN_TOUCH_ROTATE_180:
        if (m_bIsImage && m_imageController)
        {
            m_imageController->m_iRotation = (dwCmdId == ID_BTN_TOUCH_ROTATE_0) ? 0 : 180;
        }
        else
        {
            m_PdfModeController.m_iRotation =  (dwCmdId==ID_BTN_TOUCH_ROTATE_0) ? 0 :180;
            //m_btnRotate[((dwCmdId==ID_BTN_TOUCH_ROTATE_0) ? 0 :2)].SetFocus(true);
            m_PdfModeController.m_eReadingMode = PDF_RM_NormalPage;
            m_PdfModeController.m_eTurnPageMode = PDF_TPM_SinglePage;
        }
        break;
    case ID_BTN_TOUCH_ROTATE_90:
    case ID_BTN_TOUCH_ROTATE_270:
        if (m_bIsImage && m_imageController)
        {
            m_imageController->m_iRotation = (dwCmdId == ID_BTN_TOUCH_ROTATE_90) ? 90 : 270;
        }
        else
        {
            m_PdfModeController.m_iRotation = (dwCmdId==ID_BTN_TOUCH_ROTATE_90) ? 90 : 270;
            //m_btnRotate[((dwCmdId==ID_BTN_TOUCH_ROTATE_90) ? 1 : 3)].SetFocus(true);
            m_PdfModeController.m_eReadingMode = PDF_RM_AdaptiveWidth;
            m_PdfModeController.m_eTurnPageMode = PDF_TPM_ScrollPage;
            m_PdfModeController.m_uCurPageStartPos = 0;
        }
        break;
    default:
        break;
    }
    RefreshRotateFocus();
    UpdateEnableOfFontsettingOrTrim();
    UpdateEnableOfFormatOrColumn();
    UpdateWindow();
}

void UIReaderSettingDlg::ContentDisplayChangedHandler(DWORD dwCmdId)
{
    if (dwCmdId==ID_BTN_TOUCH_NORMAL_DISPLAY) {
        //m_PdfModeController.m_iRotation =  0;
        m_PdfModeController.m_eReadingMode = PDF_RM_NormalPage;
        m_PdfModeController.m_eTurnPageMode = PDF_TPM_SinglePage;
    } else if (dwCmdId==ID_BTN_TOUCH_ADAPTWIDE_DISPLAY) {
        m_PdfModeController.m_eReadingMode = PDF_RM_AdaptiveWidth;
        m_PdfModeController.m_eTurnPageMode = PDF_TPM_ScrollPage;
        m_PdfModeController.m_uCurPageStartPos = 0;
    } else {
        return;
    }
    RefreshContentDisplayFocus();

    for ( int i=0; i<2; ++i)
        m_btnContentDisplay[i].UpdateWindow();
}

bool UIReaderSettingDlg::RearRangeStatusChanged() const
{
    return m_bIsRearRangeStatusChanged;
}

void UIReaderSettingDlg::PDFRearRangeHandler()
{
    m_bIsRearRangeStatusChanged = !m_bIsRearRangeStatusChanged;
    if (PDF_RM_Rearrange != m_PdfModeController.m_eReadingMode)  // 开启文本模式
    {
        m_eLastReadingMode = m_PdfModeController.m_eReadingMode;
        m_PdfModeController.m_eReadingMode = PDF_RM_Rearrange;
        m_bIsPdfTextMode = true;
    }
    else    // 由文本模式返回
    {
        m_PdfModeController.m_eReadingMode = m_eLastReadingMode;
        m_bIsPdfTextMode = false;
    }
    m_bIsPdfRearRanging = true;
    CDisplay* pDisplay = CDisplay::GetDisplay();
    if (pDisplay)
    {
        DK_IMAGE img = pDisplay->GetMemDC();
        CTpGraphics grf(img);
        DKFontAttributes fontattr;
        fontattr.SetFontAttributes(0,0,GetWindowFontSize(FontSize32Index));
        ITpFont* pFont = FontManager::GetInstance()->GetFont(fontattr, ColorManager::knBlack);
        const char* tipText = StringManager::GetPCSTRById(SCREENSAVER_OUT_MESSAGE);
        const int tipHeight = pFont->GetHeight();
        const int tipWidth = pFont->StringWidth(tipText);
        if (m_topSizer && m_curMidWins < MID_WINDOWS_COUNTER && m_middleSizers[m_curMidWins])
        {
            const int top = m_topSizer->GetPosition().y + m_topSizer->GetSize().GetHeight();
            const int bottom = m_middleSizers[m_curMidWins]->GetPosition().y;
            if (NULL != pFont)
            {
                grf.DrawStringUtf8(tipText, (m_iWidth - tipWidth) >> 1, (top + bottom - tipHeight) >> 1, pFont);
            }
            DkRect rect = DkRect::FromLeftTopWidthHeight(0, top, m_iWidth, bottom);
            pDisplay->ForceDraw(rect, PAINT_FLAG_FULL);
            pDisplay->EndCache();
        }
    }

    UIBookReaderContainer* bookReaderContainer = static_cast<UIBookReaderContainer*>(m_pContainer);
    if (bookReaderContainer)
    {
        bookReaderContainer->ReaderSettingChangedHandler();
    }
    m_bIsRearRangeStatusChanged = false;
    m_bIsPdfRearRanging = false;
    InitWindowForRearRange();
    UpdateBottomBtnsStatus();
    UpdateFontSettingWindows();

    if (!m_bIsPdfTextMode)
    {
        StopTTSAndUpdate();
    }
    SetMiddlePosWins(MID_WINDOWS_CHAPTER);
    SafeDeletePointer(m_pReadingProcess);
    if (m_pBookReader)
    {
        m_pReadingProcess = m_pBookReader->GetCurPageProgress();
    }
    UpdateWindow();
}

void UIReaderSettingDlg::CuttingEdgeCommand()
{
    if (m_PdfModeController.m_eCuttingEdgeMode != PDF_CEM_Custom) 
    {
        MiddlePosWindsChangeHandler(ID_BTN_TOUCH_PDFTRIM);
    }
    else 
    {
        m_PdfModeController.m_eCuttingEdgeMode = m_PdfModeController.m_bIsSmartCutting ? PDF_CEM_Smart : PDF_CEM_Normal;
        m_PdfModeController.m_uCurPageStartPos = 0; 
        m_endFlag = UIReaderSettingDlg::Normal;
        EndDialog(0);
    }
    UpdateBtnStatusOfFontsettingOrTrim();
}

void UIReaderSettingDlg::ColumnedChangedHandler(DWORD dwCmdId)
{
    if (m_bIsImage && m_imageController)
    {
        switch (dwCmdId)
        {   
        case ID_BTN_TOUCH_PDF_COLUMNED_ORDER_1:
        case ID_BTN_TOUCH_PDF_COLUMNED_ORDER_2:
        case ID_BTN_TOUCH_PDF_COLUMNED_ORDER_3:
        case ID_BTN_TOUCH_PDF_COLUMNED_ORDER_4:
            {
                m_imageController->m_eReadingMode  = PDF_RM_Split4Page;
                m_imageController->m_uSubPageOrder = dwCmdId - ID_BTN_TOUCH_PDF_COLUMNED_ORDER_1;
                m_imageController->m_uCurSubPage   = 0;
            }
            break;
        case ID_BTN_TOUCH_COMICS_ORDER_1:
        case ID_BTN_TOUCH_COMICS_ORDER_2:
            {
                m_imageController->m_eReadingMode  = PDF_RM_Split2Page;
                m_imageController->m_uSubPageOrder = dwCmdId - ID_BTN_TOUCH_COMICS_ORDER_1;
                m_imageController->m_uCurSubPage   = 0;
            }
            break;
        default:
            return;
        }
    }
    else
    {
        UIPdfTrimColumnedDlg::PdfTrimColumnedAction action;
        switch (dwCmdId) {
        case ID_BTN_TOUCH_PDF_COLUMNED_ORDER_AUTO:
            action = UIPdfTrimColumnedDlg::PdfColumnedOrderAuto;
            break;
        case ID_BTN_TOUCH_PDF_COLUMNED_ORDER_1:
            action = UIPdfTrimColumnedDlg::PdfColumnedOrder1;
            break;
        case ID_BTN_TOUCH_PDF_COLUMNED_ORDER_2:
            action = UIPdfTrimColumnedDlg::PdfColumnedOrder2;
            break;
        case ID_BTN_TOUCH_PDF_COLUMNED_ORDER_3:
            action = UIPdfTrimColumnedDlg::PdfColumnedOrder3;
            break;
        case ID_BTN_TOUCH_PDF_COLUMNED_ORDER_4:
            action = UIPdfTrimColumnedDlg::PdfColumnedOrder4;
            break;
        case ID_BTN_TOUCH_COMICS_ORDER_1:
            action = UIPdfTrimColumnedDlg::PdfColumnedComicOrderLR;
            break;
        case ID_BTN_TOUCH_COMICS_ORDER_2:
            action = UIPdfTrimColumnedDlg::PdfColumnedComicOrderRL;
            break;
        default:
            return;
        }
        if (!m_pPdfTrimColumnedDlg) {
            m_pPdfTrimColumnedDlg = new UIPdfTrimColumnedDlg(this, m_pBookReader);
        }
        m_pPdfTrimColumnedDlg->SetAction(action);

        PdfModeController newModeController = m_PdfModeController;
        if (m_pBookReader)
        {
            m_pBookReader->SetPdfModeController(&newModeController);
        }

        if (m_pPdfTrimColumnedDlg->DoModal() > 0) {
            m_pPdfTrimColumnedDlg->GetTrimColumnedChangedArgs(newModeController);
            m_PdfModeController = newModeController;
        }
        else
        {
            m_pBookReader->SetPdfModeController(&m_PdfModeController);
        }
    }

    UpdateBtnStatusOfFormatOrColumn();
    SetMiddlePosWins(MID_WINDOWS_CHAPTER);
    m_endFlag = UIReaderSettingDlg::Normal;
    EndDialog(0);
}

void UIReaderSettingDlg::BookMarkAddHandler()
{
    if (m_bIsImage)
    {
        if (m_pContainer)
            m_pContainer->OnCommand(ID_BTN_TOUCH_ADD_BOOKMARK, NULL, 0);
    }
    else
    {
        UIBookReaderContainer* pBookReadContainer = dynamic_cast<UIBookReaderContainer*>(m_pContainer);
        if (pBookReadContainer)
            pBookReadContainer->SwitchBookMarkOffOn();
    }
    m_endFlag = UIReaderSettingDlg::Normal;
    EndDialog(0);
}

void UIReaderSettingDlg::CloumnedCommand()
{
    if (m_bIsImage && m_imageController)
    {
        if (PDF_RM_NormalPage == m_imageController->m_eReadingMode) {
            MiddlePosWindsChangeHandler(ID_BTN_TOUCH_PDFCOLUMN);
        } else {
            m_imageController->m_eReadingMode = PDF_RM_NormalPage;
            m_imageController->m_uCurSubPage  = 0;
            m_imageController->m_uSubPageOrder = 0;
            SystemSettingInfo::GetInstance()->SetImageCartoon(IMG_CARTOON_ALL);
            m_endFlag = UIReaderSettingDlg::Normal;
            EndDialog(0);
        }
    } else {
        if (m_PdfModeController.m_eReadingMode!=PDF_RM_Split2Page && m_PdfModeController.m_eReadingMode!=PDF_RM_Split4Page) {
            MiddlePosWindsChangeHandler(ID_BTN_TOUCH_PDFCOLUMN);
        } else {     
            m_PdfModeController.m_eReadingMode = PDF_RM_NormalPage;
            m_PdfModeController.m_uCurPageStartPos = 0;
            m_endFlag = UIReaderSettingDlg::Normal;
            EndDialog(0);
        }
    }
    UpdateBtnStatusOfFormatOrColumn();
}

void UIReaderSettingDlg::ProgressBarChangedHandler(DWORD dwCmdId)
{
    SystemSettingInfo* settingInfo = SystemSettingInfo::GetInstance();
    int newMode = dwCmdId - ID_BTN_TOUCH_NORMAL_PROGRESSBAR;
    if (settingInfo && (newMode != settingInfo->GetProgressBarMode()))
    {
        settingInfo->SetProgressBarMode(newMode);
        RefreshProgressBarFocus();

        UIBookReaderContainer* bookReaderContainer = static_cast<UIBookReaderContainer*>(m_pContainer);
        if (bookReaderContainer)
        {
            bookReaderContainer->ReaderSettingChangedHandler();
        }
        m_btnProgressBar[0].UpdateWindow();
        m_btnProgressBar[1].UpdateWindow();
    }
}

void UIReaderSettingDlg::FontRenderChangedHandler(DWORD dwCmdId)
{
    SystemSettingInfo* settingInfo = SystemSettingInfo::GetInstance();
    int newMode = dwCmdId - ID_BTN_TOUCH_FONT_RENDER_SHARP;
    if (settingInfo && (newMode != settingInfo->GetFontRender()))
    {
        settingInfo->SetFontRender(newMode);
        RefreshFontRenderFocus();
        if (m_bIsScannedText && !m_bIsPdfTextMode)
        {
            UIBookReaderContainer* bookReaderContainer = static_cast<UIBookReaderContainer*>(m_pContainer);
            if (bookReaderContainer)
            {
                bookReaderContainer->ReaderSettingChangedHandler();
            }
            m_btnProgressBar[0].UpdateWindow();
            m_btnProgressBar[1].UpdateWindow();
        }
        else
        {
            OnBookParseAttributesChanged();
            UpdateWindow();
        }
    }
}

void UIReaderSettingDlg::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    m_bIsNothingChanged = false;
    
    switch (dwCmdId)
    {
        //视图改变
    case ID_BTN_TOUCH_BOOKSHELF:
        m_endFlag = UIReaderSettingDlg::BackToBookShelf;
        EndDialog(0, false);
        return;
    case ID_BTN_TOUCH_TOC: 
        m_endFlag = UIReaderSettingDlg::SHOWTOCDLG;
        EndDialog(0);
        return;
    case ID_BTN_TOUCH_CONTENT_SEARCH:
        m_endFlag = UIReaderSettingDlg::SHOWSEARCHDLG;
        EndDialog(0);
        return;
    case ID_BTN_TOUCH_ADD_BOOKMARK:
        BookMarkAddHandler();
        break;
    case ID_BTN_TOUCH_PDFTRIM:
        CuttingEdgeCommand();
        break;
    case ID_BTN_TOUCH_PDFCOLUMN:
        CloumnedCommand();
        break;
    case ID_BTN_TOUCH_PDFREARRANGE:
        PDFRearRangeHandler();
        break;
    case ID_BTN_COMICSFRAMEMODE:
        m_endFlag = UIReaderSettingDlg::SwitchComicsFrameMode;
        EndDialog(0);
        return;
    case ID_BTN_TOUCH_START_TTS:
        {
            m_endFlag = UIReaderSettingDlg::TTS;
            EndDialog(0);
        }
        return;
    case ID_BTN_TOUCH_VOLUME:
    case ID_BTN_TOUCH_FONTSETTING:
    case ID_BTN_TOUCH_LAYOUTSTYLE:
    case ID_BTN_TOUCH_BOLD:
    case ID_BTN_TOUCH_PDFROTATE:
    case ID_BTN_TOUCH_READERSETTING:
        MiddlePosWindsChangeHandler(dwCmdId);
        break;

    case ID_BTN_TOUCH_INCRETTSVOLUME:
    case ID_BTN_TOUCH_DECRETTSVOLUME:
        VolumeChangedHandler(dwCmdId);
        break;
    case ID_BTN_TOUCH_CUTTINGEDGE_NORMAL:
    case ID_BTN_TOUCH_CUTTINGEDGE_ODDEVEN:
    case ID_BTN_TOUCH_CUTTINGEDGE_SMART:
        CuttingEdgeChangedHandler(dwCmdId);
        break;
    case ID_BTN_TOUCH_INCREFONTSIZE: 
    case ID_BTN_TOUCH_DECREFONTSIZE:
        FontSizeChangedHandler(dwCmdId);
        break;
    case ID_BTN_TOUCH_INCREEMBOLDEN:
    case ID_BTN_TOUCH_DECREEMBOLDEN:
        if (m_bIsScannedText && !m_bIsPdfTextMode)
            PdfEmBoldenChangedHandler(dwCmdId);
        else
            EmBoldenChangedHandler(dwCmdId);
        break;
    case ID_BTN_TOUCH_PAGEDOWN_CUSTOM:
    case ID_BTN_TOUCH_PAGEUP_CUSTOM:
        TurnPageCustomChangedHandler(dwCmdId);
        break;
    case ID_BTN_TOUCH_READINGUPDOWN_GESTURE_CHAPTER:
    case ID_BTN_TOUCH_READINGUPDOWN_GESTURE_PAGE:
        ReadingUpDownGestureCustomChangedHandler(dwCmdId);
        break;
    case ID_BTN_TOUCH_NORMAL_DISPLAY:
    case ID_BTN_TOUCH_ADAPTWIDE_DISPLAY:
        ContentDisplayChangedHandler(dwCmdId);
        break;
    case ID_BTN_TOUCH_NORMAL_PROGRESSBAR:
    case ID_BTN_TOUCH_MINI_PROGRESSBAR:
        ProgressBarChangedHandler(dwCmdId);
        break;
    case ID_BTN_TOUCH_FONT_RENDER_SHARP:
    case ID_BTN_TOUCH_FONT_RENDER_SMOOTH:
        FontRenderChangedHandler(dwCmdId);
        break;
    case ID_BTN_TOUCH_LAYOUTSTYLE_NORMAL:
    case ID_BTN_TOUCH_LAYOUTSTYLE_ELEGANT:
    case ID_BTN_TOUCH_LAYOUTSTYLE_STANDARD:
    case ID_BTN_TOUCH_LAYOUTSTYLE_COMPACT:
        LayoutStyleChangedHandler(dwCmdId);
        break;
    case ID_BTN_TOUCH_READERLAYOUT_FULLSCREEN:
    case ID_BTN_TOUCH_READERLAYOUT_SHOWREADERMESSAGE:
        ReadingLayoutChangeHandler(dwCmdId);
        break;
    case ID_BTN_TOUCH_FONTSTYLE_FAMILIARFONT:
    case ID_BTN_TOUCH_FONTSTYLE_TRADITIONALFONT:
        FontStyleChangedHandler(dwCmdId);
        break;
    case ID_BTN_TOUCH_ROTATE_0:
    case ID_BTN_TOUCH_ROTATE_90:
    case ID_BTN_TOUCH_ROTATE_180:
    case ID_BTN_TOUCH_ROTATE_270:
        RotateScreenHandler(dwCmdId);
        break;
    case ID_BTN_TOUCH_PDF_COLUMNED_ORDER_AUTO:
    case ID_BTN_TOUCH_PDF_COLUMNED_ORDER_1:
    case ID_BTN_TOUCH_PDF_COLUMNED_ORDER_2:
    case ID_BTN_TOUCH_PDF_COLUMNED_ORDER_3:
    case ID_BTN_TOUCH_PDF_COLUMNED_ORDER_4:
    case ID_BTN_TOUCH_COMICS_ORDER_1:
    case ID_BTN_TOUCH_COMICS_ORDER_2:
        ColumnedChangedHandler(dwCmdId);
        break;
    case ID_BTN_TOUCH_CN:
        ShowCNLangSettingDlg();
        break;
    case ID_BTN_TOUCH_EN:
        ShowENLangSettingDlg();
        break;
    case ID_BTN_TOUCH_NAVIGATETO:
        NavigateToHandler();
        break;
	case ID_BTN_TOUCH_ROLLBACK:
		RollBackHandler();
		break;
  	case ID_BTN_TOUCH_PREVCHAPTER:
		PrevChapterHandler();
		break;
	case ID_BTN_TOUCH_NEXTCHAPTER:
		NextChapterHandler();
		break;
    case ID_BTN_TOUCH_READER_SETTING_LIGHT:
        OnFrontLightClicked();
        break;
    default:
        break;
    }
}

void UIReaderSettingDlg::MiddlePosWindsChangeHandler(DWORD dwCmdId)
{
    MidWinsIndex m_midContainer = MID_WINDOWS_CHAPTER;
    UITouchComplexButton* curButton;

    switch (dwCmdId) {
    case ID_BTN_TOUCH_VOLUME:
        m_midContainer = MID_WINDOWS_VOLUME;
        curButton = &m_bottomLeftBtns[1];
        break;
    case ID_BTN_TOUCH_FONTSETTING:
        m_midContainer = MID_WINDOWS_FONTSETTING;
        curButton = &m_bottomMidBtns[0];
        break;
    case ID_BTN_TOUCH_LAYOUTSTYLE:
        m_midContainer = MID_WINDOWS_LAYOUT;
        curButton = &m_bottomMidBtns[1];
        break;
    case ID_BTN_TOUCH_BOLD:
        m_midContainer = MID_WINDOWS_FONTSETTING;
        curButton = &m_bottomMidBtns[3];
        break;
    case ID_BTN_TOUCH_PDFROTATE:
        m_midContainer = MID_WINDOWS_ROTATE;
        curButton = &m_bottomLeftBtns[0];
        RefreshRotateFocus();
        break;
    case ID_BTN_TOUCH_READERSETTING:
        m_midContainer = MID_WINDOWS_READERSETTING;
        curButton = &m_btnSetting;
        RefreshReaderSettingFocus();
        break;
    case ID_BTN_TOUCH_PDFCOLUMN:
        m_midContainer = MID_WINDOWS_COLUMNED;
        curButton = &m_bottomMidBtns[1];
        break;
    case ID_BTN_TOUCH_PDFTRIM:
        m_midContainer = MID_WINDOWS_CUTTINGEDGE;
        curButton = &m_bottomMidBtns[0];
        break;
    default:
        return;
    }


    int middleBarHeightStore = m_middleSizers[m_curMidWins]->GetSize().GetHeight() + (GetWindowMetrics(UIReaderSettingDlgMiddleButtonsVerticalMarginIndex)<<1);
    if (m_midContainer == m_curMidWins) {
        curButton->SetFocus(false);
        SetMiddlePosWins(MID_WINDOWS_CHAPTER);
    } else {
        curButton->SetFocus(true);
        SetMiddlePosWins(m_midContainer);
    }


    int middleBarHeight = m_middleSizers[m_curMidWins]->GetSize().GetHeight() + (GetWindowMetrics(UIReaderSettingDlgMiddleButtonsVerticalMarginIndex)<<1);
    middleBarHeight = middleBarHeight > middleBarHeightStore ? middleBarHeight : middleBarHeightStore;
    UpdateWindow(m_iLeft,m_iTop + m_iHeight - GetWindowMetrics(UIReaderSettingDlgBottomBarHeightIndex) - middleBarHeight , m_iWidth, middleBarHeight + GetWindowMetrics(UIReaderSettingDlgBottomBarHeightIndex));
}

void UIReaderSettingDlg::ConfigReadProcessInfo()
{
	m_clsReaderProgressBar.SetEnableScroll(true);
    if (!m_bIsImage && m_pContainer && m_pBookReader->GetCurrentPageIndex() == -1) 
    {
    	m_btnRollBack.SetEnable(false);
    	m_jumpPageNum.SetVisible(false);
        m_totalPageNum.SetText(StringManager::GetPCSTRById(PAGINT));
        m_totalPageNum.SetUseTraditionalChinese((bool)SystemSettingInfo::GetInstance()->GetFontStyle_FamiliarOrTraditional());
		m_clsReaderProgressBar.SetEnableScroll(false);
    }
	else
	{
		m_btnRollBack.SetEnable(true);
		m_jumpPageNum.SetVisible(m_curMidWins == MID_WINDOWS_CHAPTER);
		UpdateJumpDirection();

		long lPageIndex = GetCurPageIndex();
	    long lPageCount = GetTotalPageNum();
	    m_clsReaderProgressBar.SetMaximum(lPageCount);
	    m_clsReaderProgressBar.SetProgress(lPageIndex+1);
	    m_clsReaderProgressBar.SetProgressColor(ColorManager::knBlack);
		UpdatePageNum(lPageIndex + 1, lPageCount);
	}
	UpdateChapterInfo();
	m_middleSizers[MID_WINDOWS_CHAPTER]->Layout();
}

void UIReaderSettingDlg::UpdateJumpDirection()
{
	JumpDirection jumpDirection = GO_NONE;
	if(m_bIsImage)
	{
		UIImageReaderContainer* iamgeReadContainer = dynamic_cast<UIImageReaderContainer*>(m_pContainer);
		jumpDirection = iamgeReadContainer->GetJumpDirection();
	}
	else
	{
		UIBookReaderContainer* bookReadContainer = dynamic_cast<UIBookReaderContainer*>(m_pContainer);
		jumpDirection = bookReadContainer->GetJumpDirection();
	}

	switch(jumpDirection)
	{
	case GO_RIGHT:
		{
			m_btnRollBack.SetIcons(ImageManager::GetImage(IMAGE_ICON_GO_TO_RIGHT), ImageManager::GetImage(IMAGE_ICON_GO_TO_RIGHT), UIButton::ICON_CENTER);
		}
		break;
	case GO_LEFT:
		{
			m_btnRollBack.SetIcons(ImageManager::GetImage(IMAGE_ICON_GO_TO_LEFT), ImageManager::GetImage(IMAGE_ICON_GO_TO_LEFT), UIButton::ICON_CENTER);
		}
		break;
	default:
		m_btnRollBack.SetEnable(false);
		break;
	}
}

void UIReaderSettingDlg::UpdatePageNum(int curPageNum, int pageCount)
{
	char cText[50] = {0};
    if (IsPageInfoShowPercent())
    {
        double curPercent = curPageNum*1.0/pageCount;
        curPercent *= 100;
        snprintf(cText, sizeof(cText), "%.2f", curPercent);
		m_jumpPageNum.SetTextUTF8(cText);
		m_totalPageNum.SetText(" %");
    }
    else
    {
    	snprintf(cText, DK_DIM(cText), "%d", curPageNum);
		m_jumpPageNum.SetTextUTF8(cText);
        snprintf(cText, DK_DIM(cText), " / %d", pageCount);
		m_totalPageNum.SetText(cText);
    }
}

int UIReaderSettingDlg::GetTotalPageNum()
{
	if (m_pContainer && m_bIsImage)
    {
    	UIImageReaderContainer* imageReadContainer = dynamic_cast<UIImageReaderContainer*>(m_pContainer);
    	return imageReadContainer->GetTotalPageNumber();
    }
	else if(m_pBookReader)
	{
		return m_pBookReader->GetTotalPageNumber();
	}
	return 0;
}

int UIReaderSettingDlg::GetCurPageIndex()
{
	if (m_pContainer && m_bIsImage)
    {
    	UIImageReaderContainer* imageReadContainer = dynamic_cast<UIImageReaderContainer*>(m_pContainer);
    	return imageReadContainer->GetCurrentPageIndex();
    }
	else if(m_pBookReader)
	{
		return m_pBookReader->GetCurrentPageIndex();
	}
	return 0;
}

void UIReaderSettingDlg::Popup()
{
    m_endFlag = Invalid;

    if (m_pPdfTrimColumnedDlg)
        m_pPdfTrimColumnedDlg->SetVisible(false);
    if (m_pNavigateToDlg)
        m_pNavigateToDlg->SetVisible(false);
    m_bIsRearRangeStatusChanged = false;
    m_bIsNothingChanged = true;

    UpdateBtnStatusOfRotationOrTTS();
    UpdateBtnStatusOfVolume();
    UpdateBottomLeftWndsCornerStyle();

    InitWindowForRearRange();
    SetMiddlePosWins(MID_WINDOWS_CHAPTER);
	ConfigReadProcessInfo();
    SafeDeletePointer(m_pReadingProcess);
    if (m_pBookReader)
    {
        m_pReadingProcess = m_pBookReader->GetCurPageProgress();
    }
    UIDialog::Popup();
}

void UIReaderSettingDlg::EndDialog(INT32 iEndCode,BOOL fRepaintIt)
{
    if (m_endFlag != NoChange)
    {
        SaveReaderSettingInfo();
    }

    UIDialog::EndDialog(iEndCode,fRepaintIt);
}

void UIReaderSettingDlg::SaveReaderSettingInfo()
{
    if (m_bIsImage)
        return ;
    
    // 设置版式
    UINT uLineSpacing = 0;
    UINT uParaSpacing = 0;
    UINT uIndent = 0;
    UINT uTopMargin = 0;
    UINT uLeftMargin = 0;
    if (!LayoutHelper::LayoutParaFromStyle(m_iCurLayoutStyle,&uLineSpacing,&uParaSpacing,&uIndent,&uTopMargin,&uLeftMargin)) {
        //由于设计修改,暂时去掉对自定义格式的调整
        //uLineSpacing = m_pReaderSettingDlg->GetCurLineGap();
        //uParaSpacing = m_pReaderSettingDlg->GetCurPageMargin();
        //uIndent = m_pReaderSettingDlg->GetCurIndent();
    }

    SystemSettingInfo::GetInstance()->SetTypeSetting(m_iCurLayoutStyle);
    const uint32_t *fontSizeTable = LayoutHelper::GetFontSizeTable();
    SystemSettingInfo::GetInstance()->SetFontSize(fontSizeTable[m_iCurFontSizeIndex]);
    SystemSettingInfo::GetInstance()->SetLineGap(uLineSpacing);
    SystemSettingInfo::GetInstance()->SetParaSpacing(uParaSpacing);
    //SystemSettingInfo::GetInstance()->SetTabStop(clslayout.dTabStop);
    SystemSettingInfo::GetInstance()->SetBookIndent(uIndent);
    SystemSettingInfo::GetInstance()->SetTextDarkLevel(m_iCurEmBoldenLevel);
    SystemSettingInfo::GetInstance()->SetFontStyle_FamiliarOrTraditional(m_curFontStyle);
    SystemSettingInfo::GetInstance()->SetReadingLayout(m_iCurReadingLayout);
    if (m_bIsScannedText && NULL != m_dkxBlock)
    {
        m_dkxBlock->SetPDFDarkenLevel(m_iCurPdfEmBoldenLevel);
    }
    //SystemSettingInfo::GetInstance()->SetPageTopMargin(uTopMargin);
    //SystemSettingInfo::GetInstance()->SetPageHorizontalMargin(uLeftMargin);
}

void UIReaderSettingDlg::SetMiddlePosWins(MidWinsIndex m_midWins)
{
    for ( int i=0; i<MID_WINDOWS_COUNTER; ++i) { 
        m_middleSizers[i]->Show(i == m_midWins);
    }
    if (m_midWins==MID_WINDOWS_CHAPTER)
    {
        for ( size_t i=0; i<m_btnsBottom.size(); ++i)
            m_btnsBottom[i]->SetFocus(false);
        m_jumpPageNum.SetVisible(m_bIsImage || (m_pContainer && m_pBookReader->GetCurrentPageIndex() != -1));
    }
    m_curMidWins = m_midWins;
    Layout();
}

void UIReaderSettingDlg::SetIsTTSPlay(bool isTTSPlay)
{
    m_bIsTTSPlaying = isTTSPlay;
}

void UIReaderSettingDlg::SetBookMarkOn(bool existBookMark)
{
    m_bBookMarkExists = existBookMark;
    if (existBookMark) {
        m_btnBookMark.SetIcon(ImageManager::GetImage(IMAGE_TOUCH_ICON_BOOKMARK_ON), UIButton::ICON_LEFT);
    } else {
        m_btnBookMark.SetIcon(ImageManager::GetImage(IMAGE_TOUCH_ICON_BOOKMARK), UIButton::ICON_LEFT);
    }
}

bool UIReaderSettingDlg::IsPageInfoShowPercent() const
{
    return (DFF_MobiPocket == m_eFormat);
}

void UIReaderSettingDlg::NavigateToHandler()
{
	double navigatePage = atof(m_jumpPageNum.GetTextUTF8().c_str());
    if (IsPageInfoShowPercent())
    {
        navigatePage = navigatePage*this->m_pBookReader->GetTotalPageNumber()/100;
    }
    if (m_bIsImage)
    {
        m_iGotoPageNum = navigatePage;
    }
    else
    {
        UIBookReaderContainer* pBookReadContainer = dynamic_cast<UIBookReaderContainer*>(m_pContainer);
        if (pBookReadContainer)
            pBookReadContainer->NavigateToPage(navigatePage - 1);
    }
    m_endFlag = UIReaderSettingDlg::GotoPage;
	EndDialog(0);
}

void UIReaderSettingDlg::NextChapterHandler()
{
	StopTTSAndUpdate();
	UIBookReaderContainer* pBookReadContainer = dynamic_cast<UIBookReaderContainer*>(m_pContainer);
    if (pBookReadContainer)
        pBookReadContainer->GotoChapter(UIBookReaderContainer::GotoNextChapter);
	SafeDeletePointer(m_pReadingProcess);
    if (m_pBookReader)
    {
        m_pReadingProcess = m_pBookReader->GetCurPageProgress();
    }
	ConfigReadProcessInfo();
}

void UIReaderSettingDlg::PrevChapterHandler()
{
	StopTTSAndUpdate();
	UIBookReaderContainer* pBookReadContainer = dynamic_cast<UIBookReaderContainer*>(m_pContainer);
    if (pBookReadContainer)
        pBookReadContainer->GotoChapter(UIBookReaderContainer::GotoPrevChapter);
	SafeDeletePointer(m_pReadingProcess);
    if (m_pBookReader)
    {
        m_pReadingProcess = m_pBookReader->GetCurPageProgress();
    }
	ConfigReadProcessInfo();
}

void UIReaderSettingDlg::NavigateToPage(int pageIndex)
{
    if (m_pContainer)
	{
		if(m_bIsImage)
		{
			UIImageReaderContainer* imageReadContainer = dynamic_cast<UIImageReaderContainer*>(m_pContainer);
			imageReadContainer->NavigateToPage(pageIndex);
		}
		else if(m_pBookReader)
		{
			StopTTSAndUpdate();
			UIBookReaderContainer* bookReadContainer = dynamic_cast<UIBookReaderContainer*>(m_pContainer);
			bookReadContainer->NavigateToPage(pageIndex);
			SafeDeletePointer(m_pReadingProcess);
		    if (m_pBookReader)
		    {
		        m_pReadingProcess = m_pBookReader->GetCurPageProgress();
		    }
		}
	}
	ConfigReadProcessInfo();
}

void UIReaderSettingDlg::RollBackHandler()
{
	if(m_pContainer)
	{
		if(m_bIsImage)
		{
			UIImageReaderContainer* imageReadContainer = dynamic_cast<UIImageReaderContainer*>(m_pContainer);
			imageReadContainer->NavigateToPage(imageReadContainer->GetJumpHistory(), true);
		}
		else if(m_pBookReader)
		{
			StopTTSAndUpdate();
			UIBookReaderContainer* bookreaderContainer = dynamic_cast<UIBookReaderContainer*>(m_pContainer);
			bookreaderContainer->MoveToFlowPosition(bookreaderContainer->GetJumpHistory(), true);
			SafeDeletePointer(m_pReadingProcess);
		    if (m_pBookReader)
		    {
		        m_pReadingProcess = m_pBookReader->GetCurPageProgress();
		    }
		}
		ConfigReadProcessInfo();
	}
}

void UIReaderSettingDlg::UpdateChapterInfo()
{
    if (m_bIsImage || (NULL == m_pBookReader) || m_bIsScannedText) 
    {
        return;
    }

    ATOM posCurPageAtom, endAtom;
    if (m_pBookReader->GetCurrentPageATOM(posCurPageAtom, endAtom))
    {
        std::string chapterTitle = m_pBookReader->GetChapterTitle(posCurPageAtom.ToFlowPosition(), m_dkxBlock);
        m_textCurChapterInfo.SetText(chapterTitle.c_str());
        m_textCurChapterInfo.SetUseTraditionalChinese((bool)SystemSettingInfo::GetInstance()->GetFontStyle_FamiliarOrTraditional());
		m_textCurChapterInfo.UpdateWindow();
    }
}

void UIReaderSettingDlg::PerformClick(MoveEvent* moveEvent)
{
    int middleBarHeight = m_middleSizers[m_curMidWins]->GetSize().GetHeight() + (GetWindowMetrics(UIReaderSettingDlgMiddleButtonsVerticalMarginIndex)<<1);
    int y = moveEvent->GetY();
    if (y>m_topSizer->GetSize().GetHeight() && y<m_iHeight - GetWindowMetrics(UIReaderSettingDlgBottomBarHeightIndex) - middleBarHeight) 
    {
        if (m_bIsNothingChanged && m_endFlag == UIReaderSettingDlg::Invalid)
        {
            m_endFlag = NoChange;
        }
        else
        {
            m_endFlag = UIReaderSettingDlg::Normal;
        }
        EndDialog(0);
    }
}

void UIReaderSettingDlg::OnFrontLightClicked()
{
    UIFrontLightManager* lightManager = UIFrontLightManager::GetInstance();
    GUISystem::GetInstance()->GetTopFullScreenContainer()->AppendChild(lightManager);
	int topY = m_iTop + GetWindowMetrics(UITitleBarHeightIndex) + GetWindowMetrics(UIBackButtonHeightIndex) + 2;
    lightManager->MoveWindow(0, topY, m_iWidth, lightManager->GetHeight());

    lightManager->SetVisible(true);
}

DK_IMAGE* UIReaderSettingDlg::GetPreviewImage()
{
    return m_pBookReader ? m_pBookReader->GetPageBMP() : NULL;
}

void UIReaderSettingDlg::RenewBookReaderSettings()
{
    if (m_pBookReader)
    {
        //AutoLock lock(&m_mutexParseResult);
        LayoutStyle eLayoutIndex = m_iCurLayoutStyle;

        // 设置版式
        UINT uLineSpacing = 0;
        UINT uParaSpacing = 0;
        UINT uIndent = 0;
        UINT uTopMargin = 0;
        UINT uLeftMargin = 0;
        if (!LayoutHelper::LayoutParaFromStyle(eLayoutIndex,&uLineSpacing,&uParaSpacing,&uIndent,&uTopMargin,&uLeftMargin)) {
        //由于设计修改,暂时去掉对自定义格式的调整
            //uLineSpacing = m_pReaderSettingDlg->GetCurLineGap();
            //uParaSpacing = m_pReaderSettingDlg->GetCurPageMargin();
            //uIndent = m_pReaderSettingDlg->GetCurIndent();
        }

        DK_LAYOUTSETTING    clslayout;
        clslayout.dFontSize    = GetCurFontSize();
        clslayout.dLineGap     = uLineSpacing * 0.01f;
        clslayout.dParaSpacing = uParaSpacing * 0.01f;
        clslayout.dIndent      = uIndent;
        clslayout.dTabStop     = SystemSettingInfo::GetInstance()->GetTabStop();
        clslayout.bOriginalLayout = (eLayoutIndex==LAYOUT_SYSLE_NORMAL);
        m_pBookReader->SetFontChange(true);
        m_pBookReader->SetLayoutSettings(clslayout);

        // 设置页面大小
        int readingLayoutMode = m_iCurReadingLayout;
        int iTopMargin = SystemSettingInfo::GetInstance()->GetPageTopMargin(readingLayoutMode);
        int iBottomMargin  = SystemSettingInfo::GetInstance()->GetPageBottomMargin(readingLayoutMode);
        int iHorizontalMargin = SystemSettingInfo::GetInstance()->GetPageHorizontalMargin(readingLayoutMode);
        int iBookWidth = GetScreenWidth() - 2 * iHorizontalMargin;
        int iBookHeigth = GetScreenHeight() - iTopMargin - iBottomMargin;
        m_pBookReader->SetPageSize(iHorizontalMargin, iTopMargin, iBookWidth,iBookHeigth);

        //设置字体渲染
        DK_FONT_SMOOTH_TYPE fontSmoothType = DK_FONT_SMOOTH_NORMAL;
        if(SystemSettingInfo::GetInstance()->GetFontRender() == 0)
        {
            fontSmoothType = DK_FONT_SMOOTH_SHARP;
        }
        m_pBookReader->SetFontSmoothType(fontSmoothType);

        // 设置简繁转换
        m_pBookReader->SetFamiliarToTraditional(m_curFontStyle);

        // 设置加黑
        double dGammaRate = 1.0f;
        switch(m_iCurEmBoldenLevel)
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
        }

        m_pBookReader->SetTextGrayScaleLevel(dGammaRate);
        SaveReaderSettingInfo();
    }
}

void* UIReaderSettingDlg::ReparseBook(void* dummy)
{
    UIReaderSettingDlg* dummyReaderSettingDlg = static_cast<UIReaderSettingDlg*>(dummy);
    if (!dummyReaderSettingDlg)
    {
        return NULL;
    }
    dummyReaderSettingDlg->m_bIsPreviewImageReady = false;
    dummyReaderSettingDlg->RenewBookReaderSettings();
    IBookReader* dummyBookReader = dummyReaderSettingDlg->m_pBookReader;
    if (!dummyBookReader)
    {
        return NULL;
    }
    if (dummyBookReader)
    {
        bool gotoBookMarkSuccessed = false;
        if (dummyReaderSettingDlg->m_pReadingProcess && dummyBookReader)
        {
            gotoBookMarkSuccessed = dummyBookReader->GotoBookMark(dummyReaderSettingDlg->m_pReadingProcess);
        }

        if (gotoBookMarkSuccessed)
        {
            dummyReaderSettingDlg->m_tidParseBook = 0;
            dummyReaderSettingDlg->m_bIsPreviewImageReady = true;
            UIBookParseStatusListener::GetInstance()->FireBookParseFinishedEvent();
        }
    }
    //don't care about the return value
    return NULL;
}

void UIReaderSettingDlg::StartBookParseThread()
{
    if (0 != ThreadHelper::CreateThread(&m_tidParseBook, ReparseBook, this, "UIReaderSettingDlg @ ReparseBook", false, 1024 * 1024, SERIALIZEDBOOK_THREAD_PRIORITY))
    {
        DebugPrintf(DLC_DIAGNOSTIC, "CreateThread Error");
    }
}

void UIReaderSettingDlg::StopBookParseThread()
{
    if (m_tidParseBook != 0)
    {
        switch (m_eFormat)
        {
            case DFF_ElectronicPublishing:
                {
                    EpubBookReader* reader = static_cast<EpubBookReader*>(m_pBookReader);
                    if (reader)
                        reader->SetStopParsingFlag();
                    break;
                }
            case DFF_MobiPocket:
                break;
            case DFF_Text:
                {
                    TextBookReader* reader = static_cast<TextBookReader*>(m_pBookReader);
                    if (reader)
                        reader->StopParseThread();
                    break;
                }
            case DFF_PortableDocumentFormat:
                break;
            default:
                break;
        }
        ThreadHelper::JoinThread(m_tidParseBook, NULL);
        m_tidParseBook = 0;
        m_bIsPreviewImageReady = false;
    }
}

void UIReaderSettingDlg::OnBookParseAttributesChanged()
{
    if (m_pBookReader)
    {
		StopTTSAndUpdate();
        StopBookParseThread();
        StartBookParseThread();
        usleep(300000);
    }
}

void UIReaderSettingDlg::StopTTSAndUpdate()
{
	if(m_pContainer && !m_bIsImage)
	{
		CTextToSpeech* pTTS = CTextToSpeech::GetInstance();
		if(pTTS && pTTS->TTS_IsPlay())
		{
			UIBookReaderContainer* bookreaderContainer = dynamic_cast<UIBookReaderContainer*>(m_pContainer);
			bookreaderContainer->StopTTS();
			m_bIsTTSPlaying = false;
		}
	}
}

bool UIReaderSettingDlg::IsComicsChapter() const
{
    ATOM posCurPageAtom, endAtom;
    if (m_pBookReader && m_pBookReader->GetCurrentPageATOM(posCurPageAtom, endAtom))
    {
        return m_pBookReader->IsComicsChapter(posCurPageAtom.GetChapterIndex());
    }

    return false;
}

bool UIReaderSettingDlg::IsComicsFrameMode() const
{
    return IsComicsChapter() && m_pBookReader->IsComicsFrameMode();
}

bool UIReaderSettingDlg::OnParseFinishedEvent(const EventArgs& args)
{
	ConfigReadProcessInfo();
	UpdateWindow();
	return true;
}

void UIReaderSettingDlg::RefreshProgress(void* parm, double percentage, bool isStop)
{
	if(parm)
	{
		UIReaderSettingDlg* settingDlg = (UIReaderSettingDlg*)parm;
		(percentage > 1) && (percentage = 1);
		(percentage < 0) && (percentage = 0);
		int totalPageNum = settingDlg->GetTotalPageNum();
		int curPageNum = (int)(totalPageNum * percentage);
		(curPageNum < 1) && (curPageNum = 1); 
		settingDlg->UpdatePageNum(curPageNum, totalPageNum);
		string strToc = "";
		if(!settingDlg->m_bIsImage && !settingDlg->m_bIsScannedText)
		{
			DK_FLOWPOSITION jumpPos(0, 0, 0);
			if(settingDlg->m_pBookReader->GetPosByPageIndex(curPageNum - 1, &jumpPos))
			{
				strToc = settingDlg->m_pBookReader->GetChapterTitle(jumpPos, settingDlg->m_dkxBlock);
			}
		}
		settingDlg->m_textCurChapterInfo.SetText(strToc);
		settingDlg->m_textCurChapterInfo.UpdateWindow();
		if(isStop)
		{
			settingDlg->NavigateToPage(curPageNum - 1);
		}
	}
}

