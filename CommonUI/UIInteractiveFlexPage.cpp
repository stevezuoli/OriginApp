#include "CommonUI/UIInteractiveFlexPage.h"
#include "CommonUI/CPageNavigator.h"
#include "Utility/RenderUtil.h"
#include "Utility/StringUtil.h"
#include "Utility/EncodeUtil.h"
#include "Utility/FileSystem.h"
#include "Utility/PathManager.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UISizer.h"
#include "DKRAPI.h"
#include "DkStream.h"
#include "DkStreamFactory.h"
#include "Common/WindowsMetrics.h"
#include "I18n/StringManager.h"
#include "Framework/CDisplay.h"
#include "../ImageHandler/DkImageHelper.h"
#include "CommonUI/UIWeiboSharePage.h"
#include "IDKEFlexPage.h"
#include "KernelMacro.h"
using namespace dk::utility;
using namespace WindowsMetrics;

static bool Rotate(DK_IMAGE& image)
{
    if (NULL == image.pbData || 1 != image.iColorChannels)
    {
        return false;
    }

    DK_IMAGE newImage;
    memset(&newImage, 0, sizeof(newImage));

    newImage.iColorChannels = 1;
    newImage.iWidth = image.iHeight;
    newImage.iStrideWidth = newImage.iWidth * newImage.iColorChannels;
    newImage.iHeight = image.iWidth;
    const long dataLen = newImage.iStrideWidth * newImage.iHeight;
    if (dataLen <= 0)
    {
        return false;
    }

    newImage.pbData = DK_MALLOCZ_OBJ_N(BYTE, dataLen);
    if (NULL == newImage.pbData)
    {
        return false;
    }

    for (int i = 0; i < newImage.iHeight; ++i)
    {
        for (int j = 0; j < newImage.iWidth; ++j)
        {
            newImage.pbData[i * newImage.iWidth + j] = 
                image.pbData[(image.iHeight - 1 - j) * image.iWidth + i];
        }
    }

    image.iWidth = newImage.iWidth;
    image.iStrideWidth = newImage.iStrideWidth;
    image.iHeight = newImage.iHeight;
    SafeFreePointer(image.pbData);
    image.pbData = newImage.pbData;

    return true;
}

UIInteractiveFlexPage::UIInteractiveFlexPage(string bookName)
    : m_bookName(bookName)
    , m_isFullScreen(false)
    , m_isRotate(false)
    , m_startPos(0)
{
#ifdef KINDLE_FOR_TOUCH
    m_flingGestureListener.SetInteractiveFlexPage(this);
    m_gestureDetector.SetListener(&m_flingGestureListener);
#endif
    Init();
}

UIInteractiveFlexPage::~UIInteractiveFlexPage()
{
    DestroyImageData();
}

bool UIInteractiveFlexPage::Init()
{
#ifdef KINDLE_FOR_TOUCH
    m_btnClose.Initialize(IDCANCEL, "", GetWindowFontSize(FontSize22Index));
    m_btnClose.SetIcon(ImageManager::GetImage(IMAGE_ICON_CLOSE));
    m_btnClose.ShowBorder(false);
    m_btnClose.SetAlign(ALIGN_CENTER);
    const int size = GetWindowMetrics(UIPixelValue60Index);
    m_btnClose.SetMinSize(size, size);
    m_btnShare.Initialize(ID_BTN_SINAWEIBO_SHARE, "", GetWindowFontSize(FontSize22Index));
    m_btnShare.SetBackground(ImageManager::GetImage(IMAGE_ICON_TRANSMIT));
    m_btnRotate.Initialize(ID_BTN_PREBLOCK_ROTATE, "", GetWindowFontSize(FontSize22Index));
    m_btnRotate.SetBackground(ImageManager::GetImage(IMAGE_TOUCH_ICON_FLIP));
#else
    m_btnClose.Initialize(IDCANCEL, StringManager::GetPCSTRById(SCREENSAVER_USER)
            , 'C', GetWindowFontSize(FontSize22Index), ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_btnShare.Initialize(ID_BTN_SINAWEIBO_SHARE, StringManager::GetPCSTRById(SHARE)
        , 'S', GetWindowFontSize(FontSize22Index), ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_btnRotate.Initialize(ID_BTN_PREBLOCK_ROTATE, StringManager::GetPCSTRById(IMAGE_ROTATE_OPTIONS)
        , 'R', GetWindowFontSize(FontSize22Index), ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_btnFullScreen.Initialize(ID_BTN_INTERACTIVEIMAGE_FULLSCREEN, StringManager::GetPCSTRById(FULLSCREEN)
            , 'F', GetWindowFontSize(FontSize22Index), ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_btnClose.SetVAlign(VALIGN_CENTER);
    m_btnShare.SetVAlign(VALIGN_CENTER);
    m_btnRotate.SetVAlign(VALIGN_CENTER);
    m_btnFullScreen.SetVAlign(VALIGN_CENTER);
    m_btnFullScreen.SetBackground(ImageManager::GetImage(IMAGE_ICON_BUTTON_BG));
    m_btnShare.SetBackground(ImageManager::GetImage(IMAGE_ICON_BUTTON_BG));
    m_btnRotate.SetBackground(ImageManager::GetImage(IMAGE_ICON_BUTTON_BG));
    m_btnClose.SetBackground(ImageManager::GetImage(IMAGE_ICON_BUTTON_BG));
#endif
    const int margin = GetWindowMetrics(UIHorizonMarginIndex);
    m_btnGroup.AddButton(&m_btnRotate, UISizerFlags().Align(dkALIGN_BOTTOM).Border(dkRIGHT, margin));
    m_btnGroup.AddButton(&m_btnShare, UISizerFlags().Align(dkALIGN_BOTTOM).Border(dkLEFT, margin));

    m_btnGroup.SetTopLinePixel(0);
    m_btnGroup.SetBottomLinePixel(0);
    m_btnGroup.SetSplitLineHeight(GetWindowMetrics(UIPixelValue40Index));
    AppendChild(&m_btnGroup);
    
    if (!m_windowSizer)
    {
        UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
        if (mainSizer)
        {
            const int bottomBorder = GetWindowMetrics(UIInteractiveImagePageBottomBorderIndex);

            UISizer* topSizer = new UIBoxSizer(dkHORIZONTAL);
            if (topSizer)
            {
                topSizer->Add(&m_btnClose);
#ifndef KINDLE_FOR_TOUCH
                topSizer->Add(&m_btnFullScreen, UISizerFlags().Border(dkLEFT, GetWindowMetrics(UIInteractiveImagePageTopMarginIndex)));
#endif
                mainSizer->Add(topSizer, UISizerFlags().Border(dkLEFT | dkRIGHT, margin).Border(dkTOP, bottomBorder).Align(dkALIGN_RIGHT));
            }

             UISizer* pBottomSizer = new UIBoxSizer(dkHORIZONTAL);
            if (pBottomSizer)
            {
                pBottomSizer->AddStretchSpacer();
                pBottomSizer->Add(&m_btnGroup, UISizerFlags().Expand().Align(dkALIGN_BOTTOM).Border(dkBOTTOM, bottomBorder));

                mainSizer->AddStretchSpacer();
                mainSizer->Add(pBottomSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, margin).Border(dkTOP, margin).Align(dkALIGN_RIGHT));
            }
            SetSizer(mainSizer);
        }
    }
    AppendChild(&m_btnClose);
#ifndef KINDLE_FOR_TOUCH
    AppendChild(&m_btnFullScreen);
    SetChildWindowFocus(GetChildIndex(&m_btnFullScreen), false);
#endif
    MoveWindow(0, 0, CDisplay::GetDisplay()->GetScreenWidth(), CDisplay::GetDisplay()->GetScreenHeight());
    return true;
}

void UIInteractiveFlexPage::DestroyImageData()
{
    SafeFreePointer(m_image.pbData);
    SafeFreePointer(m_rotateImage.pbData);
}

HRESULT UIInteractiveFlexPage::DrawBackGround(DK_IMAGE drawingImg)
{
    HRESULT hr(S_OK);
    CTpGraphics grf(drawingImg);
    grf.EraserBackGround();
    const DK_IMAGE* pImage = m_isRotate ? &m_rotateImage : &m_image;
    if (pImage->pbData)
    {
        const int imageWidth = pImage->iWidth;
        const int imageHeight = pImage->iHeight;
        const int minWidth = dk_min(m_iWidth, imageWidth);
        const int minHeight = dk_min(m_iHeight, imageHeight);
        int srcX = 0, srcY = 0;
        DK_IMAGE imgSelf;
        DK_RECT rcImgSelf = {m_iLeft, m_iTop, m_iLeft + minWidth, m_iTop + minHeight};
        if (imageHeight < m_iHeight)
        {
            rcImgSelf.top = ((m_iHeight - imageHeight) >> 1);
        }
        else
        {
            srcY = m_isRotate ? 0 : m_startPos;
        }
        if (imageWidth < m_iWidth)
        {
            rcImgSelf.left = ((m_iWidth - imageWidth) >> 1);
        }
        else
        {
            srcX = m_isRotate ? imageWidth - m_iWidth - m_startPos : 0;
        }
        rcImgSelf.bottom = rcImgSelf.top + minHeight;
        rcImgSelf.right = rcImgSelf.left + minWidth;
        RTN_HR_IF_FAILED(CropImage(drawingImg, rcImgSelf, &imgSelf));
        CopyImageEx(imgSelf, 0, 0,
            *pImage, srcX, srcY, minWidth, minHeight);
        return S_OK;
    }

    return E_FAIL;
}

void UIInteractiveFlexPage::FullScreenSwitch()
{
    m_isFullScreen = !m_isFullScreen;

    m_btnClose.SetVisible(!m_isFullScreen);
    m_btnShare.SetVisible(!m_isFullScreen);
    m_btnRotate.SetVisible(!m_isFullScreen);
#ifndef KINDLE_FOR_TOUCH
    m_btnFullScreen.SetVisible(!m_isFullScreen);
#endif
    Layout();
    UpdateWindow();
}

void UIInteractiveFlexPage::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    switch (dwCmdId)
    {
        case IDCANCEL:
            CPageNavigator::BackToPrePage();
            break;
#ifndef KINDLE_FOR_TOUCH
        case ID_BTN_INTERACTIVEIMAGE_FULLSCREEN:
            FullScreenSwitch();
            break;
#endif
        case ID_BTN_SINAWEIBO_SHARE:
            ShareToSinaWeibo();
            break;
        case ID_BTN_PREBLOCK_ROTATE:
            m_isRotate = !m_isRotate;
            m_startPos = 0;
            Repaint();
            break;
        default:
            break;
    }
}

void UIInteractiveFlexPage::ShareToSinaWeibo()
{
    std::string content = StringManager::GetPCSTRById(SHARE_PIC);
    std::string sharedContent = UIWeiboSharePage::PruneStringForDKComment(m_bookName, content);
    UIWeiboSharePage* pPage = new UIWeiboSharePage(sharedContent.c_str());
    if (pPage)
    {
        if (m_image.pbData)
        {
            std::string fileName = PathManager::GetInteractiveImagePath().append("jpeg");
            RenderUtil::SaveBitmapToJpeg(m_image, fileName.c_str());
            pPage->SetSharedPicture(fileName.c_str());
        }
        CPageNavigator::Goto(pPage);
    }
}

void UIInteractiveFlexPage::GetRenderOption(const DK_IMAGE& image, DK_BITMAPBUFFER_DEV& dev, DK_FLOWRENDEROPTION& renderOption)
{
    dev.lWidth       = image.iWidth;
    dev.lHeight      = image.iHeight;
    dev.nPixelFormat = DK_PIXELFORMAT_RGB32;
    dev.lStride      = image.iStrideWidth;
    dev.nDPI         = RenderConst::SCREEN_DPI;
    dev.pbyData      = image.pbData;

    DK_RENDERGAMMA  gma;
    gma.dTextGamma = 0;

    renderOption.nDeviceType = DK_OUTPUTDEV_BITMAPBUFFER;
    renderOption.pDevice     = &dev;
    renderOption.gamma       = gma;
    renderOption.fontSmoothType = DK_FONT_SMOOTH_NORMAL;

    memset(dev.pbyData, 0xFF, image.iStrideWidth * image.iHeight); //画布颜色变为白色
}

bool UIInteractiveFlexPage::SetPreBlockInfo(const DKE_PREBLOCK_INFO& preBlockInfo)
{
    DestroyImageData();
    IDKEFlexPage* pFlexPage = preBlockInfo.pPreFlexPage;
    if (NULL != pFlexPage)
    {
        SystemSettingInfo* systemSetting = SystemSettingInfo::GetInstance();
        if (systemSetting)
        {
            pFlexPage->SetFontSize(abs(systemSetting->GetFontSize()));
            pFlexPage->SetLineGap(systemSetting->GetLineGap() * 0.01f);
            pFlexPage->SetParaSpacing(systemSetting->GetParaSpacing() * 0.01f);
            pFlexPage->SetTabStop(systemSetting->GetTabStop());
            pFlexPage->SetFirstLineIndent(systemSetting->GetBookIndent());
        }
        else
        {
            pFlexPage->SetFontSize(GetWindowFontSize(FontSize16Index));
            pFlexPage->SetLineGap(1);
            pFlexPage->SetParaSpacing(1);
            pFlexPage->SetTabStop(2);
            pFlexPage->SetFirstLineIndent(0);
        }

        DKTYPESETTING typeSetting;
        pFlexPage->SetTypeSetting(typeSetting);
        UpdateImage(pFlexPage, m_image, m_iWidth);
        UpdateImage(pFlexPage, m_rotateImage, m_iHeight);
        Rotate(m_rotateImage);
    }

    Layout();
    return true;
}

bool UIInteractiveFlexPage::UpdateImage(IDKEFlexPage* pFlexPage, DK_IMAGE& image, int initWidth)
{
    if (NULL == pFlexPage || initWidth <= 0)
    {
        return false;
    }
    const int margin = GetWindowMetrics(UIPixelValue30Index);
    const int doubleMargin = (margin << 1);
    pFlexPage->SetPageTopLeft(DK_POS(margin, margin));
    pFlexPage->SetPageWidth(initWidth - doubleMargin);
    pFlexPage->MeasurePageSize();
    int imageWidth = (int)pFlexPage->GetLayoutWidth() + doubleMargin;
    int imageHeight = (int)pFlexPage->GetLayoutHeight() + doubleMargin;

    pFlexPage->CalcPageLayout();

    DK_IMAGE initialImage;
    initialImage.iColorChannels = 4;
    initialImage.iWidth = imageWidth;
    initialImage.iHeight = imageHeight;
    initialImage.iStrideWidth = imageWidth * initialImage.iColorChannels;

    long initialDataLen = initialImage.iStrideWidth * initialImage.iHeight;
    initialImage.pbData = DK_MALLOCZ_OBJ_N(BYTE8, initialDataLen);
    if (NULL == initialImage.pbData)
    {
        return false;
    }
    memset(initialImage.pbData, 0, initialDataLen);

    SafeFreePointer(image.pbData);
    image.iColorChannels = 1;
    image.iWidth = imageWidth;
    image.iHeight = imageHeight;
    image.iStrideWidth = imageWidth * image.iColorChannels;
    long interactiveDataLen = image.iStrideWidth * image.iHeight;
    image.pbData = DK_MALLOCZ_OBJ_N(BYTE8, interactiveDataLen);
    if (0 == m_image.pbData)
    {
        SafeFreePointer(initialImage.pbData);
        return false;
    }
    memset(image.pbData, 0, interactiveDataLen);

    DK_BITMAPBUFFER_DEV dev;
    DK_FLOWRENDEROPTION renderOption;
    GetRenderOption(initialImage, dev, renderOption);

    DK_FLOWRENDERRESULT renderResult;
    if (DK_FAILED(pFlexPage->Render(renderOption, &renderResult))
        || !DkImageHelper::DitherRGB32ToGray16(initialImage, &image))
    {
        SafeFreePointer(initialImage.pbData);
        SafeFreePointer(image.pbData);
        return false;
    }
    SafeFreePointer(initialImage.pbData);
    return true;
}

bool UIInteractiveFlexPage::TurnPageUpDown(bool turnDown)
{
    const int imageLen = m_isRotate ? m_rotateImage.iWidth : m_image.iHeight;
    const int wndLen = m_isRotate ? m_iWidth : m_iHeight;
    if (imageLen > wndLen)
    {
        const int newMargin = GetWindowMetrics(UIPixelValue30Index);
        const int offset = wndLen - newMargin;

        m_startPos += turnDown ? offset : -offset;
        m_startPos = dk_max(0, m_startPos);
        m_startPos = dk_min(m_startPos, (imageLen - wndLen));
        Repaint();
    }
    return true;
}

#ifdef KINDLE_FOR_TOUCH
bool UIInteractiveFlexPage::OnTouchEvent(MoveEvent* moveEvent)
{
    m_gestureDetector.OnTouchEvent(moveEvent);
    return true;
}

bool UIInteractiveFlexPage::OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy)
{
    direction = TransformFlingDirection(direction, m_isRotate ? 270 : 0);
    switch(direction)
    {
    case FD_UP:
        TurnPageUpDown(true);
        break;
    case FD_DOWN:
        TurnPageUpDown(false);
        break;
    default:
        break;
    }
    return true;
}

bool UIInteractiveFlexPage::OnSingleTapUp(MoveEvent* moveEvent)
{
    FullScreenSwitch();
    return true;
}
#else
BOOL UIInteractiveFlexPage::OnKeyPress(INT32 iKeyCode)
{
    if (m_isFullScreen)
    {
        if (iKeyCode == KEY_BACK || iKeyCode == KEY_MENU)
        {
            FullScreenSwitch();
            return FALSE;
        }
    }
    switch(iKeyCode)
    {
    case KEY_LPAGEDOWN:
    case KEY_RPAGEDOWN:
    case KEY_SPACE:
        TurnPageUpDown(true);
        return FALSE;
    case KEY_LPAGEUP:
    case KEY_RPAGEUP:
        TurnPageUpDown(false);
        return FALSE;
    default:
        break;
    }
    return UIPage::OnKeyPress(iKeyCode);
}
#endif
