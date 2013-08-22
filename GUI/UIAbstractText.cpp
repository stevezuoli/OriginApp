#include "GUI/UIAbstractText.h"
#include "GUI/UIContainer.h"
#include "GUI/GUIHelper.h"
#include "GUI/CTpGraphics.h"
#include "Utility/ColorManager.h"

#include "KernelEncoding.h"
#include "Common/SystemSetting_DK.h"
#include "FontManager/DKFont.h"
#include "I18n/StringManager.h"


UIAbstractText::UIAbstractText() : UIWindow(NULL, IDSTATIC)
    , m_bIsDisposed(FALSE)
    , m_bBackTransparent(FALSE)
    , m_bIsAutoModifyWidth(false)
    , m_bIsAutoModifyHeight(true)
    , m_eraseBackground(true)
{
    Init();
}

UIAbstractText::UIAbstractText(UIContainer* pParent, const DWORD dwId) : UIWindow(pParent, dwId)
    , m_bIsDisposed(FALSE)
    , m_bBackTransparent(FALSE)
    , m_bIsAutoModifyWidth(false)
    , m_bIsAutoModifyHeight(true)
    , m_eraseBackground(true)
{
    if (pParent != NULL)
    {
        pParent->AppendChild(this);
    }
    Init();
}

void UIAbstractText::Init()
{
    m_bIsDisposed = FALSE;
    m_bBackTransparent = TRUE;

    SetFontPath();
}

UIAbstractText::~UIAbstractText()
{
    Dispose();
}

void UIAbstractText::Dispose()
{
    UIWindow::Dispose();
    OnDispose(m_bIsDisposed);
}

void UIAbstractText::OnDispose(BOOL bIsDisposed)
{
    if (bIsDisposed)
    {
        return;
    }

    m_bIsDisposed = TRUE;
}

void UIAbstractText::SetAlign(INT32 iAlign)
{
    m_TextDrawer.SetAlign(iAlign);
}
void UIAbstractText::SetVAlign(INT32 iVAlign)
{
    m_TextDrawer.SetVAlign(iVAlign);
}

void UIAbstractText::SetFirstLineIndent(double firstLineIndent)
{
    m_TextDrawer.SetFirstLineIndent(firstLineIndent);
}

void UIAbstractText::SetTabStop(double tabStop)
{
    m_TextDrawer.SetTabStop(tabStop);
}

void UIAbstractText::SetForeColor(INT32 iColor)
{
    m_TextDrawer.SetTextColor(IColor2ARGBColor(0xFF000000 | ~iColor));
}

void UIAbstractText::SetBackColor(INT32 iColor)
{
    m_TextDrawer.SetBackgroundColor(IColor2ARGBColor(0xFF000000 | ~iColor));
}

void UIAbstractText::SetForeGroundColor(INT32 iColor)
{
    m_TextDrawer.SetTextColor(IColor2ARGBColor(0xFF000000 | ~iColor));
}

void UIAbstractText::SetBackGroundColor(INT32 iColor)
{
    m_TextDrawer.SetBackgroundColor(IColor2ARGBColor(0xFF000000 | ~iColor));
}

void UIAbstractText::SetFontSize(INT32 iSize)
{
    m_TextDrawer.SetFontSize(iSize);
}

void UIAbstractText::SetFont(INT32 iFace, INT32 iStyle, INT32 iSize)
{
    (void(iFace));
    (void(iStyle));
    m_TextDrawer.SetFontSize(iSize);
}

void UIAbstractText::SetEndWithEllipsis(bool endWithEllipsis)
{
    m_TextDrawer.SetEndWithEllipsis(endWithEllipsis);
}

void UIAbstractText::SetStrikeThrough(bool strikeThrough)
{
    m_TextDrawer.SetStrikeThrough(strikeThrough);
}

void UIAbstractText::SetUnderLine(bool underLine)
{
    m_TextDrawer.SetUnderLine(underLine);
}

LPCSTR UIAbstractText::GetText() const
{
    return m_TextDrawer.GetText();
}

void UIAbstractText::SetText(const CString& rstrTxt)
{
    m_TextDrawer.SetText(rstrTxt.GetBuffer());
}

void UIAbstractText::SetText(int strId)
{
    SetText(StringManager::GetPCSTRById((SOURCESTRINGID)strId));
}

void UIAbstractText::SetText(const char* pchTxt)
{
    m_TextDrawer.SetText(pchTxt);
}

void UIAbstractText::SetText(SPtr<DkBufferBase> spText)
{
    CString m_strText;
    if(spText)
    {
        DebugPrintf(DLC_UITEXT,"spText");
        m_strText = CString((const CHAR *)spText->pbBuffer, spText->nLength);
        DebugPrintf(DLC_UITEXT,"spText over");
    }
    else
    {
        DebugPrintf(DLC_UITEXT,"else");
        BYTE8 *p = new BYTE8[4];
        if(p)
        {
            memset(p, 0, 4);
            DkBuffer *pBuf = new DkBuffer(p, 4);
            if(NULL == pBuf)
            {
                delete[] p;
                return;
            }

            SPtr<DkBufferBase>  ptr(pBuf);
            if(ptr)
            {
                m_strText = CString((const CHAR *)ptr->pbBuffer, ptr->nLength);
            }

        }
        DebugPrintf(DLC_UITEXT,"else over");
    }
    DebugPrintf(DLC_UITEXT,"char to wchar");
    DK_WCHAR* pTmpText = EncodingUtil::CharToWChar(m_strText);

    if(NULL == pTmpText)
    {
        return;
    }

    SafeFreePointer(pTmpText);
    m_TextDrawer.SetText(m_strText.GetBuffer());

    DebugPrintf(DLC_UITEXT,"setText over");
}

void UIAbstractText::SetText(const wstring& wstrTxt)
{
    char* strTmpText = EncodingUtil::WCharToChar(wstrTxt.c_str());
    if(NULL == strTmpText)
    {
        return;
    }
    m_TextDrawer.SetText(strTmpText);
    SafeFreePointer(strTmpText);
}

void UIAbstractText::SetKeyword(PCCH _pcchKeyword, UINT _uHightLightStartPos, UINT _uHighLightEndPos)
{
    m_TextDrawer.SetHighlightIndexRange(_uHightLightStartPos, _uHighLightEndPos);
}

HRESULT UIAbstractText::Draw(DK_IMAGE drawingImg)
{
    if (!m_bIsVisible)
    {
        DebugPrintf(DLC_UITEXT, "Draw if invisible");
        return S_OK;
    }
    if (drawingImg.pbData == NULL)
    {
        DebugPrintf(DLC_UITEXT,"Draw if (drawingImg.pbData == NULL)");
        return E_FAIL;
    }
    return DrawBackGround(drawingImg);
}

DK_ARGBCOLOR UIAbstractText::IColor2ARGBColor(INT32 iColor)
{
    DK_ARGBCOLOR m_textColor;
    m_textColor.rgbRed = GUIHelper::ExtractRedComponent(iColor);
    m_textColor.rgbGreen = GUIHelper::ExtractGreenComponent(iColor);
    m_textColor.rgbBlue = GUIHelper::ExtractBlueComponent(iColor);
    m_textColor.rgbAlpha = GUIHelper::ExtractAlphaComponent(iColor);
    
    return m_textColor;
}

void UIAbstractText::SetFontPath()
{
    DKFont* pChineseFont (NULL);
    wchar_t* wcChineseFontName = EncodingUtil::CharToWChar(SystemSettingInfo::GetInstance()->GetDefaultFontByCharset(DK_CHARSET_GB));
    wstring pChineseFontName = wcChineseFontName;
    SafeFreePointer(wcChineseFontName);

    pChineseFont = g_pFontManager->GetFontForFaceName (pChineseFontName);
    m_TextDrawer.SetGbFontFile(pChineseFont->GetFontPath());
    wstring wstrDefaultEnglishFont = L"Lingoes Unicode";
    DKFont* pEnglishFont (NULL);
    pEnglishFont = g_pFontManager->GetFontForFaceName (wstrDefaultEnglishFont);
    if(NULL == pEnglishFont)
    {
        pEnglishFont = pChineseFont;
    }
    m_TextDrawer.SetAnsiFontFile(pEnglishFont->GetFontPath());
}

void UIAbstractText::SetAnsiFontFile(const char* ansiFontPath)
{
	string fontPath = ansiFontPath;
    DKFont* ansiFont = g_pFontManager->GetFont(fontPath);
    if(NULL != ansiFont)
    {
         m_TextDrawer.SetAnsiFontFile(ansiFontPath);
    }
}

void UIAbstractText::SetGbFontFile(const char* gbFontPath)
{
	string fontPath = gbFontPath;
	DKFont* gbFont = g_pFontManager->GetFont(fontPath);
    if(NULL != gbFont)
    {
         m_TextDrawer.SetGbFontFile(gbFontPath);
    }
}

void UIAbstractText::SetBackTransparent(BOOL bIsTransparent)
{
    m_bBackTransparent = bIsTransparent;
}

void UIAbstractText::SetTransparent(BOOL bIsTransparent)
{
    m_bBackTransparent = bIsTransparent;
}

int UIAbstractText::GetTextHeight() const
{
    int width = -1,height = -1;
    if (m_TextDrawer.GetTextBoundingBox(&width, &height))
    {
        return height;
    }
    else
    {
        DebugPrintf(DLC_ERROR, "%s::%s error, return -1.", GetClassName(), __FUNCTION__);
        return -1;
    }
}

int UIAbstractText::GetTextWidth() const
{
    int width = -1,height = -1;
    if (m_TextDrawer.GetTextBoundingBox(&width, &height))
    {
        return width;
    }
    else
    {
        DebugPrintf(DLC_ERROR, "%s::%s error, return -1.", GetClassName(), __FUNCTION__);
        return -1;
    }
}

void UIAbstractText::SetMaxWidth(int maxWidth)
{
    m_TextDrawer.SetMaxWidth(maxWidth);
}

void UIAbstractText::SetMaxHeight(int maxHeight)
{
    m_TextDrawer.SetMaxHeight(maxHeight);
}

bool UIAbstractText::GetAutoModifyWidth() const
{
    return m_bIsAutoModifyWidth;
}

void UIAbstractText::SetAutoModifyWidth(bool autoModifyWidth)
{
    m_bIsAutoModifyWidth = autoModifyWidth;
    m_TextDrawer.SetEndWithEllipsis(!autoModifyWidth);
}

bool UIAbstractText::GetAutoModifyHeight() const
{
    return m_bIsAutoModifyHeight;
}

void UIAbstractText::SetAutoModifyHeight(bool autoModifyHeight)
{
    m_bIsAutoModifyHeight = autoModifyHeight;
}

void UIAbstractText::SetUseTraditionalChinese(bool useTraditionalChinese)
{
    m_TextDrawer.SetUseTraditionalChinese(useTraditionalChinese);
}

HRESULT UIAbstractText::DrawBackGround(DK_IMAGE drawingImg)
{
    if (!m_eraseBackground)
    {
        return S_OK;
    }
    HRESULT hr(S_OK);
    CTpGraphics grf(drawingImg);
    //TODO, should get background color from m_TextDrawer
    const DK_ARGBCOLOR& bgcolor = m_TextDrawer.GetBackgroundColor();
    INT color = ~((bgcolor.rgbRed<<16 & 0xFF0000) | (bgcolor.rgbGreen<<8 & 0xFF00) | (bgcolor.rgbBlue & 0xFF));
    RTN_HR_IF_FAILED(grf.FillRect(0, 0, m_iWidth, m_iHeight, color));
    return hr;
}

void UIAbstractText::SetEraseBackground(bool eraseBackground)
{
    m_eraseBackground = eraseBackground;
}
