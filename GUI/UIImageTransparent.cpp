#include "GUI/CTpGraphics.h"
#include "GUI/UIImageTransparent.h"
#include "Utility/EncodeUtil.h"
#include "Utility/RenderUtil.h"
#include "BMPProcessor/IBMPProcessor.h"
#include "dkWinError.h"
#include "DKRAPI.h"

using namespace dk::utility;

UIImageTransparent::UIImageTransparent(UIContainer* container)
    : UIWindow(container, IDSTATIC)
    , m_imgWidth(0)
    , m_imgHeight(0)
    , m_showBorder(false)
    , m_autoSize(false)

{
}

void UIImageTransparent::SetAutoSize(bool autoSize)
{
	m_autoSize = true;
}

void UIImageTransparent::SetImageFile(const char* imageFile)
{
    DK_AUTO(wImageFile, EncodeUtil::ToWString(imageFile));
    if (wImageFile.empty())
    {
        m_imgWidth = 0;
        m_imgHeight = 0;
    }
    else if (m_imageFile != wImageFile)
    {
        DKR_AnalyseImageFile(wImageFile.c_str(), &m_imgWidth, &m_imgHeight);
    }
    m_imageFile = wImageFile;

	if(m_autoSize)
	{
		double compressionWidth = m_imgWidth;
		double compressionHeight = m_imgHeight;
		//先按宽度压缩
		if(m_maxWidth < compressionWidth && compressionWidth > 0)
		{
			compressionHeight = compressionHeight * m_maxWidth / compressionWidth;
			compressionWidth = m_maxWidth;
		}

		//如果按宽度压缩后仍超高那再按高度压缩
		if(m_maxHeight < compressionHeight && compressionHeight > 0)
		{
			compressionWidth = compressionWidth * m_maxHeight / compressionHeight;
			compressionHeight = m_maxHeight;
		}
		SetMinSize(dkSize((int)compressionWidth, (int)compressionHeight));
	}
}

int UIImageTransparent::GetImageWidth() const
{
    return m_imgWidth;
}

int UIImageTransparent::GetImageHeight() const
{
    return m_imgHeight;
}

void UIImageTransparent::ShowBorder(bool show)
{
	m_showBorder = true;
}

bool UIImageTransparent::HasImage() const
{
    return !m_imageFile.empty() && m_imgWidth > 0 && m_imgHeight > 0;
}

HRESULT UIImageTransparent::Draw(DK_IMAGE drawingImg)
{
    if (!HasImage())
    {
        return E_FAIL;
    }

    CTpGraphics grf(drawingImg);
    grf.DrawImageFile(m_imageFile.c_str(), 
            DK_BOX(0, 0, m_imgWidth, m_imgHeight),
            DK_BOX(m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight));

	if(m_showBorder)
	{
		grf.FillRect(m_iLeft, m_iTop, m_iLeft + 1, m_iTop + m_iHeight, ColorManager::GetColor(COLOR_DISABLECOLOR));
		grf.FillRect(m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + 1, ColorManager::GetColor(COLOR_DISABLECOLOR));
		grf.FillRect(m_iLeft + m_iWidth - 1, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight, ColorManager::GetColor(COLOR_DISABLECOLOR));
		grf.FillRect(m_iLeft, m_iTop + m_iHeight - 1, m_iLeft + m_iWidth, m_iTop + m_iHeight, ColorManager::GetColor(COLOR_DISABLECOLOR));
	}
#if 0
    DK_IMAGE imgDraw;
    DK_BITMAPBUFFER_DEV dev;
    DK_ARGBCOLOR white(0xFF, 0xFF, 0xFF, 0xFF);
    if (!RenderUtil::CreateRenderImageAndDevice32(
                m_iWidth, m_iHeight, white, &imgDraw, &dev))
    {
        return E_FAIL;
    }
    CopyImageEx(imgDraw, 0, 0, drawingImg, m_iLeft, m_iTop, m_iWidth, m_iHeight);
    DK_FLOWRENDEROPTION renderOption;
    renderOption.nDeviceType = DK_OUTPUTDEV_BITMAPBUFFER;
    renderOption.pDevice = &dev;
    DKR_RenderImageFile(renderOption,
            DK_BOX(0, 0, m_iWidth, m_iHeight),
            DK_BOX(0, 0, m_iWidth, m_iHeight),
            1,
            m_imageFile.c_str());
	IBMPProcessor* grayProcessor = BMPProcessFactory::CreateInstance(IBMPProcessor::DK_BMPPROCESSOR_GRAYSCALE);
	if(NULL == grayProcessor || !grayProcessor->Process(&imgDraw))
    {
        DK_FREE(imgDraw.pbData);
		return false;
	}
    CopyImageEx(drawingImg, m_iLeft, m_iTop, imgDraw, 0, 0, m_iWidth, m_iHeight);
    DK_FREE(imgDraw.pbData);
#endif
    return S_OK;
}
