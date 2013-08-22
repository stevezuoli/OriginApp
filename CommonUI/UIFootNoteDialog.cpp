#include "Framework/CDisplay.h"
#include "GUI/CTpGraphics.h"
#include "Common/WindowsMetrics.h"
#include "GUI/UISizer.h"
#include "I18n/StringManager.h"
#include "IDKESection.h"
#include "IDKEPage.h"
#include "CommonUI/UIFootNoteDialog.h"
#include "../ImageHandler/DkImageHelper.h"
using namespace WindowsMetrics;

UIFootNoteDialog::UIFootNoteDialog(UIContainer* pParent)
    : UIDialog(pParent)
    , m_leftMargin(GetWindowMetrics(UIFootNoteDlgMarginIndex))
    , m_topMargin(GetWindowMetrics(UIFootNoteDlgMarginIndex))
    , m_bottomMargin(GetWindowMetrics(UIFootNoteDlgBottomMarginIndex))
    , m_maxWidth(GetWindowMetrics(UIFootNoteDlgWidthIndex))
    , m_maxHeight(GetWindowMetrics(UIFootNoteDlgHeightIndex))
    , m_bottomHeight(GetWindowMetrics(UIFootNoteDlgBottomHeightIndex))
    , m_elemSpacing(GetWindowMetrics(UIFootNoteDlgSpaceIndex))
#ifdef KINDLE_FOR_TOUCH
    , m_leftPage(this, ID_BTN_TOUCH_PAGEUP_CUSTOM)
    , m_rightPage(this, ID_BTN_TOUCH_PAGEDOWN_CUSTOM)
#endif
    , m_totalPage(-1)
    , m_curPage(-1)
    , m_arrowUp(true)
    , m_isShowArrow(true)
    , m_bottomSizer(NULL)
{
    const int horizontalTotalMargin = (m_leftMargin << 1);
    m_iTextMaxHeight = m_maxHeight - m_bottomHeight - m_topMargin - m_bottomMargin - m_elemSpacing;
    m_iTextWidth = m_maxWidth - horizontalTotalMargin;
    m_iHeight = m_iTextMaxHeight;
    Init();
}

UIFootNoteDialog::~UIFootNoteDialog()
{
    for (size_t i = 0; i < m_vImageOfSection.size(); ++i)
    {
        SafeFreePointer(m_vImageOfSection[i].pbData);
    }
    m_vImageOfSection.clear();
}

void UIFootNoteDialog::SetText(const char* text)
{
	if(text)
	{
		m_footNote.SetFontSize(GetWindowFontSize(UIFootNoteDlgTextIndex));
		m_footNote.SetVAlign(VALIGN_TOP);
	    m_footNote.SetLineSpace(1.2);
		m_footNote.SetText(text);
		AppendChild(&m_footNote);
		
	    m_iHeight = m_footNote.GetHeightByWidth(m_iTextWidth);
	    m_curPage   = 0;
	    m_totalPage = 1;
		int iTextHeight = m_footNote.GetHeightByWidth(m_iTextWidth);
		if (iTextHeight > m_iTextMaxHeight)
	    {
	        iTextHeight = m_iTextMaxHeight;
	        m_totalPage = m_footNote.GetPageNumByWidthHeight(m_iTextWidth, m_iTextMaxHeight);
	    }

        m_iHeight = iTextHeight;
		if(m_totalPage > 1)
		{
			m_iHeight += m_bottomHeight + m_topMargin + m_elemSpacing + m_bottomMargin;
		}
        else
        {
            m_iHeight += m_topMargin << 1;
        }

		if (m_windowSizer)
	    {
	    	m_windowSizer->Insert(m_windowSizer->IndexOf(m_bottomSizer), 
								&m_footNote, UISizerFlags(1).Border(dkLEFT | dkRIGHT, m_leftMargin).Border(dkBOTTOM, m_elemSpacing).Expand());
        }
        UpdatePageNum();
	}
}

void UIFootNoteDialog::SetSection(IDKESection* pSection)
{
	if(pSection)
	{
		m_richTextImage.SetAutoSize(true);
		AppendChild(&m_richTextImage);
		
        pSection->SetFontSize(GetWindowFontSize(UIFootNoteDlgTextIndex));
        pSection->SetParaSpacing(1.2);
		pSection->SetTextColor(DK_ARGBCOLOR(255, 0, 0, 0));
        DK_BOX pageBox(0, 0, m_iTextWidth, m_iTextMaxHeight);
		pSection->SetPageBox(pageBox);
		pSection->ParseSection();
		m_curPage   = 0;
		m_totalPage = pSection->GetPageCount();

        IDKEPage* pPage = pSection->GetPage(0);
        if (pPage)
        {
            pPage->BuildPageElements();
            if(m_totalPage > 1)
            {
                m_iHeight = m_maxHeight;
            }
            else
            {
                m_iTextMaxHeight = (int)pPage->GetLayoutHeight() + 1;
                m_iHeight = m_iTextMaxHeight + m_topMargin << 1;
            }
        }

        for (int i = 0; i < m_totalPage; ++i)
        {
            ProduceBMPPage(pSection, i);
        }

        m_totalPage = m_vImageOfSection.size();
        DK_IMAGE image = m_vImageOfSection[0];
        SPtr<ITpImage> richTextImage(new CTpImage(&image));
        m_richTextImage.SetImage(richTextImage);

		if (m_windowSizer)
	    {
	    	m_windowSizer->Insert(m_windowSizer->IndexOf(m_bottomSizer), 
								&m_richTextImage, UISizerFlags(1).Border(dkLEFT | dkRIGHT, m_leftMargin).Border(dkBOTTOM, m_elemSpacing).Expand());
        }

        UpdatePageNum();
	}
	
}

void UIFootNoteDialog::Init()
{
#ifdef KINDLE_FOR_TOUCH
    m_leftPage.SetIcon(ImageManager::GetImage(IMAGE_TOUCH_ARROW_LEFT), UIButton::ICON_RIGHT);
    m_leftPage.SetDisabledIcon(ImageManager::GetImage(IMAGE_TOUCH_ARROW_LEFT_DISABLED));
    m_leftPage.SetAlign(ALIGN_RIGHT);
    m_leftPage.ShowBorder(false);

    m_rightPage.SetIcon(ImageManager::GetImage(IMAGE_TOUCH_ARROW_RIGHT), UIButton::ICON_LEFT);
    m_rightPage.SetDisabledIcon(ImageManager::GetImage(IMAGE_TOUCH_ARROW_RIGHT_DISABLED));
    m_rightPage.SetAlign(ALIGN_LEFT);
    m_rightPage.ShowBorder(false);
#else
    m_leftPage.SetImage(ImageManager::GetImage(IMAGE_ARROW_LEFT));
    m_rightPage.SetImage(ImageManager::GetImage(IMAGE_ARROW_RIGHT));
    m_leftPage.SetAutoSize(false);
    m_rightPage.SetAutoSize(false);
    m_leftPage.SetAlign(ALIGN_RIGHT);
    m_leftPage.SetVAlign(VALIGN_CENTER);
    m_rightPage.SetAlign(ALIGN_LEFT);
    m_rightPage.SetVAlign(VALIGN_CENTER);
#endif
    m_pageNum.SetFontSize(GetWindowFontSize(UIPageNumIndex));
    m_pageNum.SetAlign(ALIGN_CENTER);
	
    AppendChild(&m_leftPage);
    AppendChild(&m_rightPage);
    AppendChild(&m_pageNum);

    if (!m_windowSizer)
    {
        UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
        mainSizer->AddSpacer(m_topMargin);
        m_bottomSizer = new UIBoxSizer(dkHORIZONTAL);
        m_bottomSizer->Add(&m_leftPage, UISizerFlags(1).Expand().Border(dkLEFT, m_leftMargin));
        m_bottomSizer->Add(&m_pageNum, UISizerFlags(1).Center());
        m_bottomSizer->Add(&m_rightPage, UISizerFlags(1).Expand().Border(dkRIGHT, m_leftMargin));
        mainSizer->Add(m_bottomSizer, UISizerFlags().Expand());
        mainSizer->AddSpacer(m_bottomMargin);
        SetSizer(mainSizer);
    }
#ifdef KINDLE_FOR_TOUCH
    HookTouch();
    m_gestureListener.SetFootNoteDlg(this);
    m_gestureDetector.SetListener(&m_gestureListener);
#endif
}

int UIFootNoteDialog::GetMaxHeight() const
{
    return m_iHeight;
}

#ifdef KINDLE_FOR_TOUCH
void UIFootNoteDialog::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    DebugPrintf(DLC_UIFOOTNOTEDIALOG, "%s:%d:%s  %s start", __FILE__, __LINE__, __FUNCTION__, GetClassName());
    switch(dwCmdId)
    {
    case ID_BTN_TOUCH_PAGEUP_CUSTOM:
    case ID_BTN_TOUCH_PAGEDOWN_CUSTOM:
        HandleDlgPageUpDown(ID_BTN_TOUCH_PAGEUP_CUSTOM == dwCmdId);
        break;
    default:
        break;
    }
    DebugPrintf(DLC_UIFOOTNOTEDIALOG, "%s:%d:%s  %s end", __FILE__, __LINE__, __FUNCTION__, GetClassName());
}
#else
BOOL UIFootNoteDialog::OnKeyPress(int iKeyCode)
{
	DebugPrintf(DLC_UIFOOTNOTEDIALOG, "%s:%d:%s  %s start", __FILE__, __LINE__, __FUNCTION__, GetClassName());
	switch(iKeyCode)
	{
	case KEY_LPAGEUP:
	case KEY_RPAGEUP:
		HandleDlgPageUpDown(true);
		return FALSE;
		break;
	case KEY_LPAGEDOWN:
	case KEY_RPAGEDOWN:
	case KEY_SPACE:
		HandleDlgPageUpDown(false);
		return FALSE;
		break;
	case KEY_BACK:
    case KEY_ENTER:
    case KEY_OKAY:
		EndDialog(0);
		return FALSE;
	default:
		break;
	}

	return UIDialog::OnKeyPress(iKeyCode);

	DebugPrintf(DLC_UIFOOTNOTEDIALOG, "%s:%d:%s  %s end", __FILE__, __LINE__, __FUNCTION__, GetClassName());
}
#endif

void UIFootNoteDialog::SetArrowPara(bool arrowUp, int x, int y)
{
    DebugPrintf(DLC_UIFOOTNOTEDIALOG, "%s:%d:%s  %s start", __FILE__, __LINE__, __FUNCTION__, GetClassName());
    DebugPrintf(DLC_UIFOOTNOTEDIALOG, "arrowUp = [%d, %d], location = [%d, %d]", m_arrowUp, arrowUp, x, y);

    m_arrowUp = arrowUp;
#ifdef KINDLE_FOR_TOUCH
    m_imgArrow.SetImage(ImageManager::GetImage(arrowUp ? IMAGE_TOUCH_ARROW_UP : IMAGE_TOUCH_ARROW_DOWN));
#else
    m_imgArrow.SetImage(ImageManager::GetImage(arrowUp ? IMAGE_ARROW_UP_2 : IMAGE_ARROW_DOWN_2));
#endif

    const int imageWidth  = m_imgArrow.GetImageWidth();
    const int imageHeight = m_imgArrow.GetImageHeight();
    const int imageX      = x - (imageWidth >> 1);
    const int imageY      = arrowUp ? y : (y - imageHeight);
    DebugPrintf(DLC_UIFOOTNOTEDIALOG, "m_imgArrow [%d, %d, %d, %d]", imageX, imageY, imageWidth, imageHeight);
    m_imgArrow.MoveWindow(imageX, imageY, imageWidth, imageHeight);

    DebugPrintf(DLC_UIFOOTNOTEDIALOG, "%s:%d:%s  %s end", __FILE__, __LINE__, __FUNCTION__, GetClassName());
}

void UIFootNoteDialog::MoveWindow(int left, int top, int width, int height)
{
    DebugPrintf(DLC_UIFOOTNOTEDIALOG, "%s:%d:%s  %s start", __FILE__, __LINE__, __FUNCTION__, GetClassName());
    CDisplay* pDisplay = CDisplay::GetDisplay();
    if (pDisplay)
    {
        left = (pDisplay->GetScreenWidth() - m_maxWidth) >> 1;
    }
    UIWindow::MoveWindow(left, top, m_maxWidth, height);
    m_bottomSizer->Show(m_totalPage > 1);

    Layout();
    DebugPrintf(DLC_UIFOOTNOTEDIALOG, "%s:%d:%s  %s end", __FILE__, __LINE__, __FUNCTION__, GetClassName());
}

HRESULT UIFootNoteDialog::DrawBackGround(DK_IMAGE drawingImg)
{
    DebugPrintf(DLC_UIFOOTNOTEDIALOG, "%s:%d:%s  %s start", __FILE__, __LINE__, __FUNCTION__, GetClassName());
    HRESULT hr(S_OK);
    CTpGraphics grp(drawingImg);
    grp.DrawBorder(0, 0, 0, 0);
    DebugPrintf(DLC_UIFOOTNOTEDIALOG, "%s:%d:%s  %s end", __FILE__, __LINE__, __FUNCTION__, GetClassName());
    return hr;
}

bool UIFootNoteDialog::ProduceBMPPage(IDKESection* pSection, int pageIndex)
{
    if (NULL == pSection)
    {
        return false;
    }
    IDKEPage* pPage = pSection->GetPage(pageIndex);
    if (NULL == pPage)
    {
        return false;
    }

    DK_IMAGE imageSection;
    imageSection.iColorChannels = 4;
    imageSection.iWidth         = m_iTextWidth;
    imageSection.iHeight        = m_iTextMaxHeight;
    imageSection.iStrideWidth   = imageSection.iWidth * imageSection.iColorChannels;
    
    long iDataLen = imageSection.iStrideWidth * imageSection.iHeight;
    imageSection.pbData         = DK_MALLOCZ_OBJ_N(BYTE8, iDataLen);
    if (NULL == imageSection.pbData)
    {
        return false;
    }

    // DEV信息根据DK_IMAGE来生成
    DK_BITMAPBUFFER_DEV dev;
    DK_FLOWRENDEROPTION renderOption;
    dev.lWidth       = imageSection.iWidth;
    dev.lHeight      = imageSection.iHeight;
    dev.nPixelFormat = DK_PIXELFORMAT_RGB32;
    dev.lStride      = imageSection.iStrideWidth;
    dev.nDPI         = RenderConst::SCREEN_DPI;
    dev.pbyData      = imageSection.pbData;

    DK_RENDERGAMMA  gma;
    gma.dTextGamma = 1.0;

    renderOption.nDeviceType = DK_OUTPUTDEV_BITMAPBUFFER;
    renderOption.pDevice     = &dev;
    renderOption.gamma       = gma;
    renderOption.fontSmoothType = DK_FONT_SMOOTH_NORMAL;

    memset(dev.pbyData, 0xFF, iDataLen); //画布颜色变为白色

    DK_FLOWRENDERRESULT  clsDKERenderResult;
    if (DK_FAILED(pPage->Render(renderOption, &clsDKERenderResult)))
    {
        return false;
    }

    if (!DkFormat::DkImageHelper::ConvertRGB32ToGray256(&imageSection))
    {
        return false;
    }

    DkFormat::DkImageHelper::ConvertGray256ToGray16(imageSection);

    IBMPProcessor* pProcessor = BMPProcessFactory::CreateInstance(IBMPProcessor::DK_BMPPROCESSOR_GRAYSCALE);
    if (pProcessor)
    {
        pProcessor->Process(&imageSection);
    }

    m_vImageOfSection.push_back(imageSection);
    return true;
}

void UIFootNoteDialog::UpdatePageNum()
{
    if (m_curPage < 0)
    {
        m_curPage = 0;
    }

	char cText[50] = {0};
    snprintf(cText, sizeof(cText), "%d / %d", (m_curPage + 1), m_totalPage);
    m_pageNum.SetText(cText);
#ifdef KINDLE_FOR_TOUCH
    if (m_leftPage.IsVisible())
        m_leftPage.SetEnable(m_curPage != 0);
    if (m_rightPage.IsVisible())
        m_rightPage.SetEnable(m_curPage != (m_totalPage - 1));
#else
    if (m_leftPage.IsVisible())
        m_leftPage.SetImage(m_curPage == 0 ? ImageManager::GetImage(IMAGE_ARROW_LEFT_DISABLED) : ImageManager::GetImage(IMAGE_ARROW_LEFT));
    if (m_rightPage.IsVisible())
        m_rightPage.SetImage(m_curPage == (m_totalPage - 1) ? ImageManager::GetImage(IMAGE_ARROW_RIGHT_DISABLED) : ImageManager::GetImage(IMAGE_ARROW_RIGHT));
#endif
}

HRESULT UIFootNoteDialog::Draw(DK_IMAGE drawingImg)
{
	if (!m_bIsVisible)
        return S_OK;

	if (m_isShowArrow)
    {
        m_imgArrow.Draw(drawingImg);
    }
    return UIDialog::Draw(drawingImg);
}

void UIFootNoteDialog::HandleDlgPageUpDown(bool bPageUp)
{
    DebugPrintf(DLC_UIFOOTNOTEDIALOG, "%s:%d:%s  %s start", __FILE__, __LINE__, __FUNCTION__, GetClassName());
    if (m_totalPage <= 1)
    {
        DebugPrintf(DLC_UIFOOTNOTEDIALOG, "m_totalPage = %d", m_totalPage);
        return ;
    }

    if (bPageUp)
    {
        m_curPage = (m_curPage == 0) ? (m_totalPage - 1) : (m_curPage - 1);
    }
    else
    {
        m_curPage = (m_curPage == (m_totalPage - 1)) ? 0 : (m_curPage + 1);
    }
    if (m_vImageOfSection.size() > 0)
    {
        DK_IMAGE image = m_vImageOfSection[m_curPage];
        SPtr<ITpImage> richTextImage(new CTpImage(&image));
        m_richTextImage.SetImage(richTextImage);
    }
    else
    {
        m_footNote.SetDrawPageNo(m_curPage);
    }

    UpdatePageNum();
    DebugPrintf(DLC_UIFOOTNOTEDIALOG, "m_curPage = %d, m_totalPage = %d", m_curPage, m_totalPage);
    UpdateWindow();
    DebugPrintf(DLC_UIFOOTNOTEDIALOG, "%s:%d:%s  %s end", __FILE__, __LINE__, __FUNCTION__, GetClassName());
}
#ifdef KINDLE_FOR_TOUCH
bool UIFootNoteDialog::OnHookTouch(MoveEvent* moveEvent)
{
    if (moveEvent->GetActionMasked() != MoveEvent::ACTION_DOWN)
    {
        return false;
    }
    if (IsVisible() && !PointInWindowAbsolute(moveEvent->GetX(), moveEvent->GetY()))
    {
        EndDialog(0);
        return true;
    }
    return false;
}

bool UIFootNoteDialog::OnTouchEvent(MoveEvent* moveEvent)
{
    m_gestureDetector.OnTouchEvent(moveEvent);
    return true;
}

bool UIFootNoteDialog::OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy)
{
    switch (direction)
    {
        case FD_UP:
        case FD_LEFT:
            HandleDlgPageUpDown(false);
            break;
        case FD_DOWN:
        case FD_RIGHT:
            HandleDlgPageUpDown(true);
            break;
        default:
            break;
    }
    return true;
}

bool UIFootNoteDialog::OnSingleTapUp(MoveEvent* moveEvent)
{
	double widthPercent  = 0.2;
    int x = moveEvent->GetX();
    HandleDlgPageUpDown(x>=0 && x<= m_iWidth*widthPercent);
    return true;
}
#endif

