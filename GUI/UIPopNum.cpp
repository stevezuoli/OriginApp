#include "GUI/UIPopNum.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "Utility/ColorManager.h"
#include "FontManager/DKFont.h"

using namespace WindowsMetrics;

UIPopNum::UIPopNum(UIContainer* pParent)
    : UIAbstractText(pParent, IDSTATIC)
    , m_image(NULL)
{
    Init();
}

UIPopNum::~UIPopNum()
{

}

void UIPopNum::Init()
{
    m_image.SetEnable(false);
    m_image.SetImageFile(ImageManager::GetImagePath(IMAGE_ICON_POP));

    m_bIsTabStop = FALSE;
    m_TextDrawer.SetMode(TextDrawer::TDM_FIXED_LINES);
    m_TextDrawer.SetMaxLines(1);
    m_TextDrawer.SetEndWithEllipsis(true);
    m_TextDrawer.SetMaxWidth(m_image.GetImageWidth());
    m_TextDrawer.SetFontSize(GetWindowFontSize(FontSize16Index));
    m_TextDrawer.SetTextColor(IColor2ARGBColor(0xFF000000 | ColorManager::knBlack));
    m_TextDrawer.SetAlign(ALIGN_CENTER);
    SetFontPath();
}

void UIPopNum::MoveWindow(int iLeft, int iTop, int iWidth, int iHeight)
{
    UIWindow::MoveWindow(iLeft, iTop, iWidth, iHeight);
    m_image.MoveWindow(0, 0, iWidth, iHeight);
}

HRESULT UIPopNum::Draw(DK_IMAGE drawingImg)
{
    if (!IsDisplay())
    {
        return S_OK;
    }
    if (drawingImg.pbData == NULL)
    {
        return E_FAIL;
    }

    DK_IMAGE imgSelf;
    DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
    CropImage(drawingImg, rcSelf, &imgSelf);
    m_image.Draw(imgSelf);
    return m_TextDrawer.Render(imgSelf, 0, 0, m_iWidth, m_iHeight);
}

dkSize UIPopNum::GetMinSize() const
{
    int width = m_minWidth;
    int height = m_minHeight;
    if (width == dkDefaultCoord)
    {
        width = m_image.GetImageWidth();
    }
    if (height == dkDefaultCoord)
    {
        height = m_image.GetImageHeight();
    }
    return dkSize(width, height);
}

void UIPopNum::SetNum(int num)
{
    bool visible = (num > 0);
    if (visible)
    {
        char str[8] = {0};
        if (num > 99)
        {
            snprintf(str, DK_DIM(str), "n");
        }
        else
        {
            snprintf(str, DK_DIM(str), "%d", num);
        }
        m_TextDrawer.SetText(str);
    }
    SetVisible(visible);
}

void UIPopNum::SetFontPath()
{
    std::wstring fontPath = L"Century Gothic";
    DKFont* englishFont (NULL);
    if(g_pFontManager)
    {
        englishFont = g_pFontManager->GetFontForFaceName (fontPath);
        if(englishFont)
        {
            m_TextDrawer.SetAnsiFontFile(englishFont->GetFontPath());
        }
    }
}
