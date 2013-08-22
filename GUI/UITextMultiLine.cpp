#include "GUI/UIContainer.h"
#include "GUI/UITextMultiLine.h"
#include "GUI/CTpGraphics.h"
#include "GUI/GUIHelper.h"
#include "Utility/ColorManager.h"
#include "Common/SystemSetting_DK.h"
#include "FontManager/DKFont.h"
#include "BMPProcessor/IBMPProcessor.h"
#include "DKTAPI.h"
#include "KernelMacro.h"
#include "KernelEncoding.h"

UITextMultiLine::UITextMultiLine()
    : UIWindow(NULL, IDSTATIC)
    , m_leftMargin(10)
    , m_topMargin(10)
    , m_rightMargin(10)
    , m_bottomMargin(10)
    , m_fontSize(20)
    , m_lineGap(1)
    , m_indent(0)
    , m_alignType(IDKTRender::LAYOUT_ALIGN_JUSTIFY)
    , m_endWithEllipsis(true)
    , m_textColor(DK_ARGBCOLOR(255, 0, 0, 0))
    , m_pDKTRender(NULL)
    , m_pCharInfo(NULL)
    , m_charInfoCount(0)
    , m_pLineInfo(NULL)
    , m_lineInfoCount(0)
{
}

UITextMultiLine::UITextMultiLine(UIContainer* pParent, const DWORD dwId)
    : UIWindow(pParent, dwId)
    , m_leftMargin(10)
    , m_topMargin(10)
    , m_rightMargin(10)
    , m_bottomMargin(10)
    , m_fontSize(20)
    , m_lineGap(1)
    , m_indent(0)
    , m_alignType(IDKTRender::LAYOUT_ALIGN_JUSTIFY)
    , m_endWithEllipsis(true)
    , m_textColor(DK_ARGBCOLOR(255, 0, 0, 0))
    , m_pDKTRender(NULL)
    , m_pCharInfo(NULL)
    , m_charInfoCount(0)
    , m_pLineInfo(NULL)
    , m_lineInfoCount(0)
{
    if (NULL != pParent)
    {
        pParent->AppendChild(this);
    }
}

UITextMultiLine::~UITextMultiLine()
{
    ClearRenderInfo();
    if (m_pDKTRender)
    {
        DKT_ReleaseRender(m_pDKTRender);
    }
}

bool UITextMultiLine::RenderText()
{
    DebugPrintf(DLC_UITEXTMULTILINE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (m_text.empty())
    {
        DebugPrintf(DLC_UITEXTMULTILINE, "%s, %d, %s, %s m_text.empty()", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return true;
    }

    if (NULL == m_pDKTRender)
    {
        m_pDKTRender = (IDKTRender*)DKT_CreateRender();
        if (NULL == m_pDKTRender)
        {
            DebugPrintf(DLC_UITEXTMULTILINE, "%s, %d, %s, %s NULL == m_pDKTRender", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            return false;
        }
    }
    ClearRenderInfo();
    if (m_AnsiFontFile.empty())
    {
        wstring ansiFontName = L"Lingoes Unicode";
        DKFont* pAnsiFont = g_pFontManager->GetFontForFaceName(ansiFontName);
        if (NULL == pAnsiFont)
        {
            DebugPrintf(DLC_UITEXTMULTILINE, "%s, %d, %s, %s NULL == pAnsiFont", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            return false;
        }
        wchar_t* pAnsiFontFile = EncodingUtil::CharToWChar(pAnsiFont->GetFontPath());
        if (NULL == pAnsiFontFile)
        {
            DebugPrintf(DLC_UITEXTMULTILINE, "%s, %d, %s, %s NULL == pAnsiFontFile", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            return false;
        }
        m_AnsiFontFile = pAnsiFontFile;
        SafeFreePointer(pAnsiFontFile);
    }

    if (m_GbFontFile.empty())
    {
        wchar_t* pChineseFontName = EncodingUtil::CharToWChar(SystemSettingInfo::GetInstance()->GetConfigValue(CKI_DefaultChineseFont));
        if (NULL == pChineseFontName)
        {
            DebugPrintf(DLC_UITEXTMULTILINE, "%s, %d, %s, %s NULL == pChineseFontName", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            return false;
        }
        wstring chineseFontName = pChineseFontName;
        SafeFreePointer(pChineseFontName);
        DKFont* pChineseFont = g_pFontManager->GetFontForFaceName(chineseFontName);
        if (NULL == pChineseFont)
        {
            DebugPrintf(DLC_UITEXTMULTILINE, "%s, %d, %s, %s NULL == pChineseFont", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            return false;
        }
        wchar_t* pChineseFontFile = EncodingUtil::CharToWChar(pChineseFont->GetFontPath());
        if (NULL == pChineseFontFile)
        {
            DebugPrintf(DLC_UITEXTMULTILINE, "%s, %d, %s, %s NULL == pChineseFontFile", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            return false;
        }
        m_GbFontFile = pChineseFontFile;
        SafeFreePointer(pChineseFontFile);
    }

    IDKTRender::LayoutOption layoutOption;
    layoutOption.fontSize  = m_fontSize;
    layoutOption.lineGap   = m_lineGap;
    layoutOption.indent    = m_indent;
    layoutOption.alignType = m_alignType;
    DK_BOOL followAfter = false;
    if (DK_FAILED(m_pDKTRender->CreateRenderInfoByHeight((const DK_WCHAR*)m_text.c_str(),
        layoutOption,
        (const DK_WCHAR*)m_GbFontFile.c_str(),
        (const DK_WCHAR*)m_AnsiFontFile.c_str(),
        (DK_DOUBLE)m_iWidth - m_leftMargin - m_rightMargin,
        (DK_DOUBLE)m_iHeight - m_topMargin - m_bottomMargin,
        m_endWithEllipsis,
        &followAfter,
        &m_pCharInfo,
        &m_charInfoCount,
        &m_pLineInfo,
        &m_lineInfoCount)))
    {
        DebugPrintf(DLC_UITEXTMULTILINE, "%s, %d, %s, %s DK_FAILED(CreateRenderInfoByHeight)", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return false;
    }
    DebugPrintf(DLC_UITEXTMULTILINE, "%s, %d, %s, %s END", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return true;
}

void UITextMultiLine::ClearRenderInfo()
{
    DebugPrintf(DLC_UITEXTMULTILINE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (NULL != m_pCharInfo || NULL != m_pLineInfo)
        m_pDKTRender->ReleaseRenderInfo(m_pCharInfo, m_pLineInfo);
    m_pCharInfo = NULL;
    m_pLineInfo = NULL;
    m_charInfoCount = 0;
    m_lineInfoCount = 0;
    DebugPrintf(DLC_UITEXTMULTILINE, "%s, %d, %s, %s END", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

HRESULT UITextMultiLine::Draw(DK_IMAGE drawingImg)
{
    DebugPrintf(DLC_UITEXTMULTILINE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (!m_bIsVisible)
    {
        DebugPrintf(DLC_UITEXTMULTILINE,"if (!m_bIsVisible)");
        return S_OK;
    }
    if (drawingImg.pbData == NULL)
    {
        DebugPrintf(DLC_UITEXTMULTILINE,"if (drawingImg.pbData == NULL)");
        return E_FAIL;
    }

    if (!RenderText())
    {
        DebugPrintf(DLC_UITEXTMULTILINE,"if (!RenderText())");
        return E_FAIL;
    }

    if (NULL == m_pCharInfo || NULL == m_pLineInfo || 0 == m_charInfoCount || 0 == m_lineInfoCount)
    {
        DebugPrintf(DLC_UITEXTMULTILINE,"if (!m_pCharInfo || !m_pLineInfo || 0 == m_charInfoCount || 0 == m_lineInfoCount)");
        return S_OK;
    }

    DK_IMAGE imgDraw;
    imgDraw.iColorChannels = 4;
    imgDraw.iWidth         = m_iWidth;
    imgDraw.iHeight        = m_iHeight;
    imgDraw.iStrideWidth   = imgDraw.iWidth * imgDraw.iColorChannels;
    long iDataLen = imgDraw.iStrideWidth * imgDraw.iHeight;
    imgDraw.pbData         = DK_MALLOCZ_OBJ_N(BYTE8, iDataLen);
    if (NULL == imgDraw.pbData)
    {
        DebugPrintf(DLC_UITEXTMULTILINE,"if (NULL == imgDraw.pbData)");
        return E_FAIL;
    }
    memset(imgDraw.pbData, 0xff, iDataLen); 

    DK_RENDERGAMMA  gma;
    gma.dTextGamma = 1.0f;

    DK_BITMAPBUFFER_DEV dev;
    dev.lWidth       = imgDraw.iWidth;
    dev.lHeight      = imgDraw.iHeight;
    dev.nPixelFormat = DK_PIXELFORMAT_RGB32;
    dev.lStride      = imgDraw.iStrideWidth;
    dev.nDPI         = RenderConst::SCREEN_DPI;
    dev.pbyData      = imgDraw.pbData;

    DK_FLOWRENDEROPTION renderOption;
    renderOption.nDeviceType = DK_OUTPUTDEV_BITMAPBUFFER;
    renderOption.pDevice     = &dev;
    renderOption.gamma       = gma;
    renderOption.bBinaryText = DK_FALSE;

    const IDKTRender::RenderCharInfo* firstCharInfo = m_pCharInfo;
    const IDKTRender::RenderCharInfo* endCharInfo   = m_pCharInfo + m_charInfoCount;// - 1;
    if (DK_SUCCEEDED(m_pDKTRender->RenderByInfo(renderOption,
        false,
        m_leftMargin,
        m_topMargin,
        m_textColor,
        (DK_WCHAR*)m_GbFontFile.c_str(),
        (DK_WCHAR*)m_AnsiFontFile.c_str(),
        m_fontSize,
        firstCharInfo,
        endCharInfo)))
    {
        DebugPrintf(DLC_UITEXTMULTILINE,"DKT_RenderTextFromRenderInfo SUCCESSED");
        DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight };
        CopyRenderImage(&drawingImg,&imgDraw,rcSelf);
    }
    SafeFreePointer(imgDraw.pbData);
    DebugPrintf(DLC_UITEXTMULTILINE, "%s, %d, %s, %s END", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

HRESULT UITextMultiLine::CopyRenderImage(DK_IMAGE* _TargetImg, DK_IMAGE* _pCopyImage, DK_RECT _rcSelf)
{
    DebugPrintf(DLC_UITEXTMULTILINE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    HRESULT hr(S_OK);
    if(0 == (_TargetImg && _pCopyImage && _pCopyImage->pbData))
    {
        DebugPrintf(DLC_UITEXTMULTILINE, "if(0 == (_TargetImg && _pCopyImage && _pCopyImage->pbData))");
        return E_FAIL;
    }
    IBMPProcessor* pProcessor = BMPProcessFactory::CreateInstance(IBMPProcessor::DK_BMPPROCESSOR_GRAYSCALE);
    DK_ASSERT(pProcessor);
    if (NULL == pProcessor)
    {
        DebugPrintf(DLC_UITEXTMULTILINE, "if (NULL == pProcessor)");
        return E_FAIL;
    }
    if(FALSE == pProcessor->Process(_pCopyImage))
    {
        DebugPrintf(DLC_UITEXTMULTILINE, "if(FALSE == pProcessor->Process(_pCopyImage))");
        return E_FAIL;
    }

    DK_IMAGE imgSelf;
    RTN_HR_IF_FAILED(CropImage( *_TargetImg,  _rcSelf, &imgSelf ));

    if(S_OK != CopyImage(imgSelf, *_pCopyImage))
    {
        DebugPrintf(DLC_UITEXTMULTILINE, "if (S_OK != CopyImage(imgSelf, *_pCopyImage))");
        return E_FAIL;
    }
    DebugPrintf(DLC_UITEXTMULTILINE, "%s, %d, %s, %s END", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return S_OK;
}

void UITextMultiLine::SetMargin(int left, int top, int right, int bottom)
{
    DebugPrintf(DLC_UITEXTMULTILINE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (left >= 0)
    {
        m_leftMargin = left;
    }

    if (top >= 0)
    {
        m_topMargin = top;
    }

    if (right >= 0)
    {
        m_rightMargin = right;
    }

    if (bottom >= 0)
    {
        m_bottomMargin = bottom;
    }
    DebugPrintf(DLC_UITEXTMULTILINE, "m_leftMargin   = %d", m_leftMargin);
    DebugPrintf(DLC_UITEXTMULTILINE, "m_topMargin    = %d", m_topMargin);
    DebugPrintf(DLC_UITEXTMULTILINE, "m_rightMargin  = %d", m_rightMargin);
    DebugPrintf(DLC_UITEXTMULTILINE, "m_bottomMargin = %d", m_bottomMargin);
    DebugPrintf(DLC_UITEXTMULTILINE, "%s, %d, %s, %s END", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

void UITextMultiLine::SetFontSize(int fontSize)
{
    DebugPrintf(DLC_UITEXTMULTILINE, "%s, %d, %s, %s fontSize = %d", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, fontSize);
    if (fontSize < 4)
        fontSize = 4;
    m_fontSize = fontSize;
}

void UITextMultiLine::SetLineGap(int lineGap)
{
    DebugPrintf(DLC_UITEXTMULTILINE, "%s, %d, %s, %s lineGap = %d", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, lineGap);
    if (lineGap < 0)
        lineGap = 0;
    m_lineGap = lineGap;
}

void UITextMultiLine::SetIndent(int indent)
{
    DebugPrintf(DLC_UITEXTMULTILINE, "%s, %d, %s, %s indent = %d", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, indent);
    if (indent < 0)
        indent = 0;
    m_indent = indent;
}

void UITextMultiLine::SetAlign(int alignType)
{
    DebugPrintf(DLC_UITEXTMULTILINE, "%s, %d, %s, %s alignType = %d", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, alignType);
    m_alignType = IDKTRender::LAYOUT_ALIGN_JUSTIFY;
    switch(alignType)
    {
    case ALIGN_LEFT:
        m_alignType = IDKTRender::LAYOUT_ALIGN_LEFT;
        break;
    case ALIGN_CENTER:
        m_alignType = IDKTRender::LAYOUT_ALIGN_CENTER;
        break;
    case ALIGN_RIGHT:
        m_alignType = IDKTRender::LAYOUT_ALIGN_RIGHT;
        break;
    default:
        break;
    }
    DebugPrintf(DLC_UITEXTMULTILINE, "%s, %d, %s, %s m_alignType = %d", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, m_alignType);
}

void UITextMultiLine::SetEndWithEllipsis(bool endWithEllipsis)
{
    DebugPrintf(DLC_UITEXTMULTILINE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    DebugPrintf(DLC_UITEXTMULTILINE, "endWithEllipsis = %s", endWithEllipsis ? "TRUE" : "FALSE");
    m_endWithEllipsis = endWithEllipsis;
}

void UITextMultiLine::SetGbFontFile(const wchar_t* pFontFile)
{
    DebugPrintf(DLC_UITEXTMULTILINE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    m_GbFontFile = pFontFile;
}

void UITextMultiLine::SetAnsiFontFile(const wchar_t* pFontFile)
{
    DebugPrintf(DLC_UITEXTMULTILINE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    m_AnsiFontFile = pFontFile;
}

void UITextMultiLine::SetTextColor(int textColor)
{
    DebugPrintf(DLC_UITEXTMULTILINE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    m_textColor.rgbRed = GUIHelper::ExtractRedComponent(textColor);
    m_textColor.rgbGreen = GUIHelper::ExtractGreenComponent(textColor);
    m_textColor.rgbBlue = GUIHelper::ExtractBlueComponent(textColor);
    m_textColor.rgbAlpha = GUIHelper::ExtractAlphaComponent(textColor);
}

void UITextMultiLine::SetText(const char* pText)
{
    DebugPrintf(DLC_UITEXTMULTILINE, "%s, %d, %s, %s start : %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, (NULL != pText) ? pText : "NULL");
    wchar_t* pWstrText = EncodingUtil::CharToWChar(pText);
    m_text = pWstrText;
    SafeFreePointer(pWstrText);
    DebugPrintf(DLC_UITEXTMULTILINE, "%s, %d, %s, %s END", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

void UITextMultiLine::SetText(const wchar_t* pText)
{
    DebugPrintf(DLC_UITEXTMULTILINE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    m_text = pText;
}
