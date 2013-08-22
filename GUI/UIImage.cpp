////////////////////////////////////////////////////////////////////////
// UIImage.cpp
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "GUI/UIImage.h"
#include "GUI/CTpGraphics.h"
#include "Utility/ColorManager.h"
#include "interface.h"
#include "ImageHandler/ImageHandler.h"
#include "../ImageHandler/DkImageHelper.h"
#include "Utility/StringUtil.h"
#include "Utility/ImageManager.h"

using dk::utility::StringUtil;

#define IMAGE_FOCUS_PADDING 2

UIImage::UIImage() 
    : UIWindow(NULL, IDSTATIC)
    , m_spImage()
    , m_autoSize(true)
    , m_bIsDisposed(FALSE) 
    , m_iAlign(ALIGN_LEFT)
    , m_iVAlign(VALIGN_TOP)
{
    Init();
}

UIImage::UIImage(UIContainer* pParent, const DWORD dwId)
    : UIWindow(pParent, dwId)
    , m_spImage()
    , m_autoSize(true)
    , m_bIsDisposed(FALSE)
    , m_iAlign(ALIGN_LEFT)
    , m_iVAlign(VALIGN_TOP)
{
    if (pParent != NULL)
    {
        pParent->AppendChild(this);
    }
    Init();
    m_bIsTabStop = FALSE;
}

UIImage::~UIImage()
{
    Dispose();
}

void UIImage::Dispose()
{
    UIWindow::Dispose();
    OnDispose(m_bIsDisposed);
}


void UIImage::Init()
{
    m_bIsDisposed = FALSE;
    m_bIsTabStop = FALSE;
    SetBackgroundColor(ColorManager::knWhite);
}

void UIImage::SetAutoSize(bool autoSize)
{
    m_autoSize = autoSize;
}

HRESULT UIImage::DrawPartial(DK_IMAGE drawingImg, INT32 xInImage, INT32 yInImage )
{
    DebugLog(DLC_GUI, "Entering UIImage::DrawPartial()");
    DebugPrintf(DLC_GUI, "drawimgImg=(%x, %d, %d, %d, %d)", 
        drawingImg.pbData, 
        drawingImg.iColorChannels, 
        drawingImg.iWidth, 
        drawingImg.iHeight, 
        drawingImg.iStrideWidth 
        );

    if (drawingImg.pbData == NULL)
    {
        return E_FAIL;
    }

    HRESULT hr(S_OK);
    RTN_HR_IF_FAILED(DrawImage(drawingImg, xInImage, yInImage));

    DebugLog(DLC_GUI, "Leaving UIImage::DrawPartial()");
    return hr;
}

HRESULT UIImage::Draw(DK_IMAGE drawingImg)
{
    DebugLog(DLC_GUI, "Entering UIImage::Draw()");
    DebugPrintf(DLC_GUI, "drawimgImg=(%x, %d, %d, %d, %d)", 
        drawingImg.pbData, 
        drawingImg.iColorChannels, 
        drawingImg.iWidth, 
        drawingImg.iHeight, 
        drawingImg.iStrideWidth 
        );

    if (!m_bIsVisible)
    {
        return S_OK;
    }

    if (drawingImg.pbData == NULL)
    {
        return E_FAIL;
    }

    HRESULT hr(S_OK);
    RTN_HR_IF_FAILED(DrawImage(drawingImg, 0, 0));
    m_IsChanged = false;

    DebugLog(DLC_GUI, "Leaving UIImage::Draw()");
    return hr;
}

HRESULT UIImage::DrawImage(DK_IMAGE drawingImg, INT32 xInImage, INT32 yInImage)
{
    HRESULT hr(S_OK);
    DK_IMAGE imgSelf;
    DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};

    DebugPrintf(DLC_GUI, "rcSelf=(%d, %d, %d, %d)", 
        rcSelf.left,
        rcSelf.top,
        rcSelf.right,
        rcSelf.bottom
        );

    RTN_HR_IF_FAILED(CropImage(
        drawingImg, 
        rcSelf, 
        &imgSelf
        ));

    CTpGraphics grp(imgSelf);
    RTN_HR_IF_FAILED(grp.FillRect(0,
        0,
        m_iWidth,
        m_iHeight,
        GetBackgroundColor()));
    int left = GetDrawLeftPositon();
    int top = GetDrawTopPositon();
    if (m_spImage)
    {
        RTN_HR_IF_FAILED(grp.DrawImageBlend(m_spImage.Get(), left, top, xInImage, yInImage, m_iWidth, m_iHeight));

    }

    return hr;
}

HRESULT UIImage::DrawFocus(DK_IMAGE drawingImg)
{
    return S_OK;
}

void UIImage::SetImage(int imgId)
{
    return SetImage(ImageManager::GetImage(imgId));
}

void UIImage::SetImage(SPtr<ITpImage> img)
{
    m_imageFile.clear();
    SetImageInternal(img);
}

void UIImage::SetImageInternal(SPtr<ITpImage> spImg)
{
    m_spImage = spImg;

    if (!m_spImage)
    {
        return;
    }

    if (m_autoSize)
    {
        m_iWidth = m_spImage->GetWidth();
        m_iHeight = m_spImage->GetHeight();
    }
    SetMinSize(m_iWidth, m_iHeight);
}

void UIImage::OnResize(INT32 iWidth, INT32 iHeight)
{
    UIWindow::OnResize(iWidth, iHeight);

    if (m_autoSize)
    {
        if (!m_spImage)
        {
            return;
        }

        m_iWidth = m_spImage->GetWidth();
        m_iHeight = m_spImage->GetHeight();
    }
}

void UIImage::OnDispose(BOOL bIsDisposed)
{
    //UIWindow::OnDispose(bDisposed);

    if (bIsDisposed)
    {
        return;
    }

    m_bIsDisposed = TRUE;
    // dispose any resource if needed
}

INT32 UIImage::GetImageWidth()
{
    if (m_spImage)
    {
        return m_spImage->GetWidth();
    }
    else
    {
        return -1;
    }
}

INT32 UIImage::GetImageHeight()
{
    if (m_spImage)
    {
        return m_spImage->GetHeight();
    }
    else
    {
        return -1;
    }
}

void UIImage::SetAlign(INT32 iAlign)
{
    m_iAlign = iAlign;
}

void UIImage::SetVAlign(INT32 iVAlign)
{
    m_iVAlign = iVAlign;
}

int UIImage::GetDrawLeftPositon() const
{
    int left = 0;
    switch (m_iAlign)
    {
    case ALIGN_LEFT:
        break;
    case ALIGN_RIGHT:
        left = m_iWidth - m_minWidth;
        break;
    case ALIGN_CENTER:
        left = (m_iWidth - m_minWidth) >> 1;
        break;
    }

    left = left >= 0 ? left : 0;
    return left;
}

int UIImage::GetDrawTopPositon() const
{
    int top = 0;
    switch (m_iVAlign)
    {
    case VALIGN_TOP:
        break;
    case VALIGN_BOTTOM:
        top = m_iHeight - m_minHeight;
        break;
    case VALIGN_CENTER:
        top = (m_iHeight - m_minHeight)>>1;
    }

    top = top >= 0 ? top : 0;
    return top;
}

void UIImage::SetImageFile(const char* imageFilePath)
{
    if (StringUtil::IsNullOrEmpty(imageFilePath))
    {
        SetImage(SPtr<ITpImage>());
        return;
    }
    if (m_imageFile == imageFilePath)
    {
        return;
    }
    std::auto_ptr<ImageHandler> imageHandler(DkImageHelper::CreateImageHandlerInstance(imageFilePath));
    if (!imageHandler.get())
    {
        return;
    }
    m_imageFile = imageFilePath;
    if (m_autoSize)
    {
        m_iWidth = imageHandler->GetWidth();
        m_iHeight = imageHandler->GetHeight();
    }
    int width = m_iWidth;
    int height = m_iHeight;
    DK_RECT srcRect = {0, 0, imageHandler->GetWidth(), imageHandler->GetHeight()};
    DebugPrintf(DLC_XU_KAI,"imageHandler->GetWidth() : %d; imageHandler->GetHeight() : %d, width : %d, height : %d",imageHandler->GetWidth(),imageHandler->GetHeight(),width,height);
    if(!imageHandler->CropAndResize(srcRect, width, height))
    {
        return;
    }
    DK_IMAGE image;
    memset(&image,0,sizeof(DK_IMAGE));
    image.Reset(1, width, height);
    imageHandler->GetImage(&image);
    SPtr<ITpImage> coverImage(new CTpImage(&image));
    SafeFreePointer(image.pbData);
    SetImageInternal(coverImage);
}

void UIImage::MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight)
{
    bool reloadFromFile = false;
    if (!m_imageFile.empty() && (iWidth != m_iWidth || iHeight != m_iHeight))
    {
        reloadFromFile = true;
    }
    UIWindow::MoveWindow(iLeft, iTop, iWidth, iHeight);
    if (reloadFromFile)
    {
        std::string oldImageFile = m_imageFile;
        m_imageFile.clear();
        SetImageFile(oldImageFile.c_str());
    }
}
