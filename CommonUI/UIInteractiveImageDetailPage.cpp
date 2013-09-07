#include "CommonUI/UIInteractiveImageDetailPage.h"
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
#include "IDKEGallery.h"
using namespace dk::utility;
using namespace WindowsMetrics;

UIInteractiveImageDetailPage::UIInteractiveImageDetailPage(string bookName)
    : m_bookName(bookName)
    , m_isFullScreen(false)
    , m_pBottomSizer(NULL)
    , m_dotProgress()
    , m_curIndex(-1)
{
#ifdef KINDLE_FOR_TOUCH
    m_flingGestureListener.SetInteractiveImageDetailPage(this);
    m_gestureDetector.SetListener(&m_flingGestureListener);
#endif
    Init();
}

UIInteractiveImageDetailPage::~UIInteractiveImageDetailPage()
{
    DestroyImageData();
    ClearArray();
}

bool UIInteractiveImageDetailPage::Init()
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
#else
    m_btnClose.Initialize(IDCANCEL, StringManager::GetPCSTRById(SCREENSAVER_USER)
            , 'C', GetWindowFontSize(FontSize22Index), ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_btnShare.Initialize(ID_BTN_SINAWEIBO_SHARE, StringManager::GetPCSTRById(SHARE)
            , 'S', GetWindowFontSize(FontSize22Index), ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_btnFullScreen.Initialize(ID_BTN_INTERACTIVEIMAGE_FULLSCREEN, StringManager::GetPCSTRById(FULLSCREEN)
            , 'F', GetWindowFontSize(FontSize22Index), ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_btnClose.SetVAlign(VALIGN_CENTER);
    m_btnShare.SetVAlign(VALIGN_CENTER);
    m_btnFullScreen.SetVAlign(VALIGN_CENTER);
    m_btnFullScreen.SetBackground(ImageManager::GetImage(IMAGE_ICON_BUTTON_BG));
    m_btnShare.SetBackground(ImageManager::GetImage(IMAGE_ICON_BUTTON_BG));
    m_btnClose.SetBackground(ImageManager::GetImage(IMAGE_ICON_BUTTON_BG));
#endif
    m_textImageMainTitle.SetFontSize(GetWindowFontSize(FontSize24Index));
    m_textImageSubTitle.SetFontSize(GetWindowFontSize(FontSize20Index));
    
    if (!m_windowSizer)
    {
        UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
        if (mainSizer)
        {
            const int margin = GetWindowMetrics(UIHorizonMarginIndex);

            UISizer* topSizer = new UIBoxSizer(dkHORIZONTAL);
            if (topSizer)
            {
                topSizer->Add(&m_btnClose);
#ifndef KINDLE_FOR_TOUCH
                topSizer->Add(&m_btnFullScreen, UISizerFlags().Border(dkLEFT, GetWindowMetrics(UIInteractiveImagePageTopMarginIndex)));
#endif
                mainSizer->Add(topSizer, UISizerFlags().Border(dkLEFT | dkRIGHT, margin).Border(dkTOP, GetWindowMetrics(UIInteractiveImagePageImageBottomBorderIndex)).Align(dkALIGN_RIGHT));
            }

            m_pBottomSizer = new UIBoxSizer(dkVERTICAL);
            if (m_pBottomSizer)
            {
                int widgetsMargin = GetWindowMetrics(UIPixelValue26Index);
                int bottomBorder = GetWindowMetrics(UIInteractiveImagePageBottomBorderIndex);
                const int width = CDisplay::GetDisplay()->GetScreenWidth();
                const int minWidth = width - ((margin + widgetsMargin) << 1) - m_btnShare.GetMinWidth();
                m_textImageMainTitle.SetMinWidth(minWidth);
                m_textImageSubTitle.SetMinWidth(minWidth);

                m_pBottomSizer->Add(&m_dotProgress, UISizerFlags().Expand().Align(dkALIGN_CENTRE));

                UIBoxSizer* infoSizer = new UIBoxSizer(dkHORIZONTAL);
                if (infoSizer)
                {
                    UISizer* titleSizer = new UIBoxSizer(dkVERTICAL);
                    if (titleSizer)
                    {
                        titleSizer->Add(&m_textImageMainTitle, UISizerFlags().Expand());
                        titleSizer->Add(&m_textImageSubTitle, UISizerFlags(1).Expand());
                        infoSizer->Add(titleSizer, UISizerFlags(1).Expand());
                    }
                    infoSizer->Add(&m_seperator, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, widgetsMargin));
                    infoSizer->Add(&m_btnShare, UISizerFlags().Centre());

                    m_pBottomSizer->Add(infoSizer, UISizerFlags().Expand().Border(dkBOTTOM, bottomBorder));
                }

                mainSizer->AddStretchSpacer();
                mainSizer->Add(m_pBottomSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, margin).Border(dkTOP, margin));
            }
            SetSizer(mainSizer);
        }
    }
    m_dotProgress.SetEnable(false);
    m_dotProgress.SetVisible(false);
    AppendChild(&m_btnClose);
    AppendChild(&m_textImageMainTitle);
    AppendChild(&m_textImageSubTitle);
    AppendChild(&m_seperator);
    AppendChild(&m_btnShare);
    AppendChild(&m_dotProgress);
#ifndef KINDLE_FOR_TOUCH
    AppendChild(&m_btnFullScreen);
    SetChildWindowFocus(GetChildIndex(&m_btnFullScreen), false);
#endif
    MoveWindow(0, 0, CDisplay::GetDisplay()->GetScreenWidth(), CDisplay::GetDisplay()->GetScreenHeight());
    return true;
}

bool UIInteractiveImageDetailPage::InitImageData(int index)
{
    if (index == m_curIndex)
    {
        return true;
    }
    if (index < 0 || index >= (int)m_vFileBuffer.size())
    {
        return false;
    }
    UpdateTitleInfo(index);
    m_curIndex = index;
    const std::string& fileBuffer = m_vFileBuffer.at(m_curIndex);
    DestroyImageData();
    bool ret = false;
    if (!fileBuffer.empty())
    {
        int remainWidth = m_iWidth;
        int remainHeight = m_iHeight;
        std::auto_ptr<IDkStream> imgStream( 
                DkStreamFactory::GetMemoryStream((void*)fileBuffer.c_str(),fileBuffer.length(), fileBuffer.length()));
        if (NULL == imgStream.get())
        {
            return false;
        }
        imgStream->Open();
        DK_UINT width = 0, height = 0;
        DKR_AnalyseImageStream(imgStream.get(), &width, &height);
        if (width <= 0 || height <= 0)
        {
            imgStream->Close();
            return false;
        }
        if (remainWidth * height > remainHeight * width)
        {
            remainWidth = remainHeight * width / height;
        }
        else
        {
            remainHeight = remainWidth * height / width;
        }

        DK_BITMAPBUFFER_DEV dev;
        if (!RenderUtil::CreateRenderImageAndDevice32(
                    remainWidth,
                    remainHeight,
                    DK_ARGBCOLOR(0xFF, 0xFF, 0xFF, 0xFF),
                    &m_interactiveImage,
                    &dev))
        {
            imgStream->Close();
            return false;
        }
        DK_FLOWRENDEROPTION renderOption;
        renderOption.nDeviceType = DK_OUTPUTDEV_BITMAPBUFFER;
        renderOption.pDevice = &dev;
        DKR_RenderImageStream(renderOption,
                DK_BOX(0, 0, remainWidth, remainHeight),
                DK_BOX(0, 0, width, height),
                1,
                imgStream.get());

        DK_IMAGE convertedImage;
        convertedImage.iColorChannels = 1;
        convertedImage.iHeight = remainHeight;
        convertedImage.iWidth = remainWidth;
        convertedImage.iStrideWidth = remainWidth*convertedImage.iColorChannels;
        unsigned char  *pConventedData = DK_MALLOCZ_OBJ_N(unsigned char, remainWidth*remainHeight);
        if (0 == pConventedData)
        {
            return false;
        }
        convertedImage.pbData = pConventedData;
        memset(pConventedData, 0, remainWidth*remainHeight);

        ret = DkImageHelper::DitherRGB32ToGray16(m_interactiveImage, &convertedImage);
        if (ret)
        {
            SafeDeletePointer(m_interactiveImage.pbData);
            m_interactiveImage = convertedImage;
        }
        else
        {
            DestroyImageData();
        }

        imgStream->Close();
    }

    return ret;
}

void UIInteractiveImageDetailPage::DestroyImageData()
{
    if (m_interactiveImage.pbData)
    {
        SafeFreePointer(m_interactiveImage.pbData);
    }
}

HRESULT UIInteractiveImageDetailPage::DrawBackGround(DK_IMAGE drawingImg)
{
    HRESULT hr(S_OK);
    CTpGraphics grf(drawingImg);
    grf.EraserBackGround();
    
    if (m_interactiveImage.pbData)
    {
        DK_IMAGE imgSelf;
        DK_RECT rcImgSelf = {m_iLeft, m_iTop, m_iLeft + m_interactiveImage.iWidth, m_iTop + m_interactiveImage.iHeight};
        if (m_interactiveImage.iHeight < m_iHeight)
        {
            rcImgSelf.top = (m_iHeight - m_interactiveImage.iHeight) >> 1;
            rcImgSelf.bottom = rcImgSelf.top + m_interactiveImage.iHeight;
        }
        if (m_interactiveImage.iWidth < m_iWidth)
        {
            rcImgSelf.left = (m_iWidth - m_interactiveImage.iWidth) >> 1;
            rcImgSelf.right = rcImgSelf.left + m_interactiveImage.iWidth;
        }
        RTN_HR_IF_FAILED(CropImage(drawingImg, rcImgSelf, &imgSelf));
        CopyImage(imgSelf, m_interactiveImage);

        int top = m_iHeight;
        if (m_dotProgress.IsVisible())
        {
            top = m_dotProgress.GetY();
        }
        else if (m_pBottomSizer && !m_isFullScreen)
        {
            top = m_pBottomSizer->GetPosition().y;
        }
        if (top != m_iHeight)
        {
            grf.FillRect(0, top, m_iWidth, m_iHeight, ColorManager::knWhite);
        }
        return S_OK;
    }

    return E_FAIL;
}

void UIInteractiveImageDetailPage::FullScreenSwitch()
{
    m_isFullScreen = !m_isFullScreen;

    m_textImageMainTitle.SetVisible(!m_isFullScreen);
    m_textImageSubTitle.SetVisible(!m_isFullScreen);
    m_seperator.SetVisible(!m_isFullScreen);
    m_btnClose.SetVisible(!m_isFullScreen);
    m_btnShare.SetVisible(!m_isFullScreen);
#ifndef KINDLE_FOR_TOUCH
    m_btnFullScreen.SetVisible(!m_isFullScreen);
#endif
    Layout();
    UpdateWindow();
}

void UIInteractiveImageDetailPage::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
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
        default:
            break;
    }
}

void UIInteractiveImageDetailPage::ShareToSinaWeibo()
{
    assert (m_curIndex >= 0 && m_curIndex < (int)m_vMainTitle.size());
    std::string title;
    const std::string& mainTitle = m_vMainTitle.at(m_curIndex);
    if (!mainTitle.empty())
    {
        title.append(mainTitle).append(1, '\n');
    }
    const std::string& subTitle = m_vSubTitle.at(m_curIndex); 
    if (!subTitle.empty())
    {
        title.append(subTitle);
    }
    if (title.empty())
    {
        title = StringManager::GetPCSTRById(SHARE_PIC);
    }
    std::string sharedContent = UIWeiboSharePage::PruneStringForDKComment(m_bookName, title);
    UIWeiboSharePage* pPage = new UIWeiboSharePage(sharedContent.c_str());
    if (pPage)
    {
        const std::string& fileExt = m_vFileExt.at(m_curIndex);
        const std::string& fileBuffer = m_vFileBuffer.at(m_curIndex);
        if ((fileExt.compare("jpg")==0 || fileExt.compare("gif")==0 || fileExt.compare("png")==0)
            && m_interactiveImage.pbData)
        {
            std::string fileName = PathManager::GetInteractiveImagePath().append(fileExt);
            FileSystem::SaveFile(fileBuffer.c_str(), fileBuffer.length(), fileName.c_str());
            pPage->SetSharedPicture(fileName.c_str());
        }
        CPageNavigator::Goto(pPage);
    }
}

void UIInteractiveImageDetailPage::PushBackInfo(DKE_HITTEST_OBJECTINFO objInfo)
{
    std::string fileExt, fileBuffer;
    if (objInfo.srcImageData)
    {
        fileExt = EncodeUtil::ToString(objInfo.srcImageData->strFileExt);
        fileExt = StringUtil::ToLower(fileExt.c_str());

        fileBuffer.assign((const char*)objInfo.srcImageData->pFileDataBuf, (size_t)objInfo.srcImageData->lFileDataLength);
    }
    std::string mainTitle, subTitle;
    if (objInfo.mainTitle)
    {
        mainTitle = EncodeUtil::ToString(objInfo.mainTitle->GetPureText());
    }
    if (objInfo.subTitle)
    {
        subTitle = EncodeUtil::ToString(objInfo.subTitle->GetPureText());
    }
    m_vMainTitle.push_back(mainTitle);
    m_vSubTitle.push_back(subTitle);
    m_vFileExt.push_back(fileExt);
    m_vFileBuffer.push_back(fileBuffer);
}

void UIInteractiveImageDetailPage::ClearArray()
{
    m_vMainTitle.clear();
    m_vSubTitle.clear();
    m_vFileExt.clear();
    m_vFileBuffer.clear();
}

bool UIInteractiveImageDetailPage::SetHitObjectInfo(DKE_HITTEST_OBJECTINFO objInfo)
{
    ClearArray();
    PushBackInfo(objInfo);
    InitImageData();
    Layout();
    return true;
}

bool UIInteractiveImageDetailPage::SetGallery(IDKEGallery* pGallery)
{
    if (NULL != pGallery)
    {
        ClearArray();
        DKE_HITTEST_OBJECTINFO objInfo;
        const int cellCount = pGallery->GetCellCount();
        for (int i = 0; i < cellCount; ++i)
        {
            pGallery->GetCellImageInfo(i, &objInfo);
            PushBackInfo(objInfo);
            pGallery->FreeCellImageInfo(&objInfo);
        }

        const int cell = pGallery->GetCurActiveCell();
        if (cell >= 0 && cell < cellCount)
        {
            m_dotProgress.SetProgress(cell, cellCount);
            m_dotProgress.SetVisible(true);
            InitImageData(cell);
            Layout();
        }
    }
    return NULL != pGallery;
}

void UIInteractiveImageDetailPage::UpdateTitleInfo(int index)
{
    if (index == m_curIndex || index < 0 || index >= (int)m_vFileBuffer.size())
    {
        return;
    }

    std::string mainTitle = m_vMainTitle.at(index);
    if (!mainTitle.empty())
    {
        m_textImageMainTitle.SetText(mainTitle);
    }
    std::string subTitle = m_vSubTitle.at(index);
    if (!subTitle.empty())
    {
        m_textImageSubTitle.SetText(subTitle);
    }
    Layout();
}

bool UIInteractiveImageDetailPage::TurnPageUpDown(bool pageDown)
{
    const int cellCount = m_vFileBuffer.size();
    int newCellIndex = m_curIndex;
    if (pageDown)
    {
        newCellIndex++;
        if (newCellIndex >= cellCount)
        {
            newCellIndex = 0;
        }
    }
    else
    {
        newCellIndex--;
        if (newCellIndex < 0)
        {
            newCellIndex = cellCount - 1;
        }
    }

    if (newCellIndex >= 0 && newCellIndex < cellCount)
    {
        m_dotProgress.SetProgress(newCellIndex, cellCount);
        InitImageData(newCellIndex);
        UpdateWindow();
    }
    return true;
}

#ifdef KINDLE_FOR_TOUCH
bool UIInteractiveImageDetailPage::OnTouchEvent(MoveEvent* moveEvent)
{
    m_gestureDetector.OnTouchEvent(moveEvent);
    return true;
}

bool UIInteractiveImageDetailPage::OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy)
{
    switch(direction)
    {
    case FD_LEFT:
        TurnPageUpDown(true);
        break;
    case FD_RIGHT:
        TurnPageUpDown(false);
        break;
    default:
        break;
    }
    return true;
}

bool UIInteractiveImageDetailPage::OnSingleTapUp(MoveEvent* moveEvent)
{
    FullScreenSwitch();
    return true;
}
#else
BOOL UIInteractiveImageDetailPage::OnKeyPress(INT32 iKeyCode)
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
