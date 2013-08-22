#include "GUI/UIDotProgress.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UISizer.h"
#include "GUI/FontManager.h"
#include "Common/WindowsMetrics.h"

using namespace WindowsMetrics;

static const wchar_t EMPTYDOT = (wchar_t)0x25cb;
static const wchar_t SPACE = (wchar_t)0xa0;
static const wchar_t FILLEDDOT = (wchar_t)0x25cf;

UIDotProgress::UIDotProgress()
    : m_selected(false)
{
#ifdef KINDLE_FOR_TOUCH
    SetEnable(FALSE);
#else
    SetEnable(TRUE);
#endif
}

UIDotProgress::~UIDotProgress()
{

}

void UIDotProgress::SetSelected(bool selected)
{
    if (m_selected != selected)
    {
        m_selected = selected;
    }
}

void UIDotProgress::UpdateText()
{
    if (m_selected)
    {
        m_strShowText = std::wstring(L"<   ").append(m_strText).append(L"   >");
    }
    else
    {
        m_strShowText = m_strText;
    }
}

void UIDotProgress::SetProgress(int index, int count)
{
    if (index >= 0 && index < count)
    {
        m_strText.clear();

        int i = 0;
        while (i < index)
        {
            m_strText.append(&EMPTYDOT, 1);
            m_strText.append(&SPACE, 1);
            i++;
        }
        m_strText.append(&FILLEDDOT, 1);
        i++;
        while (i < count)
        {
            m_strText.append(&SPACE, 1);
            m_strText.append(&EMPTYDOT, 1);
            i++;
        }
        UpdateText();
    }
}

dkSize UIDotProgress::GetMinSize() const
{
    int minWidth  = m_minWidth;
    int minHeight = m_minHeight;

    if (minWidth == dkDefaultCoord || minHeight == dkDefaultCoord)
    {
        DKFontAttributes iFontAttribute;
        iFontAttribute.SetFontAttributes(0, 0, GetWindowFontSize(FontSize16Index));
        int color = ColorManager::knBlack;
        ITpFont *pFont = FontManager::GetInstance()->GetFont(iFontAttribute, color);
        if (pFont)
        {
            minWidth = pFont->StringWidth(m_strShowText.c_str(), m_strShowText.length()) + GetWindowMetrics(UIPixelValue10Index);
            minHeight = pFont->GetHeight() + GetWindowMetrics(UIPixelValue10Index);
        }
    }
    return dkSize(minWidth, minHeight);
}

HRESULT UIDotProgress::Draw(DK_IMAGE drawingImg)
{
    UpdateText();
    if (!m_strShowText.empty())
    {
        DK_IMAGE imgSelf;
        DK_RECT rcSelf = {m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop +m_iHeight};
        if (S_OK == CropImage(drawingImg, rcSelf, &imgSelf))
        {
            CTpGraphics grf(imgSelf);
            //grf.EraserBackGround();
            DKFontAttributes iFontAttribute;
            iFontAttribute.SetFontAttributes(0, 0, GetWindowFontSize(FontSize16Index));
            int color = ColorManager::knBlack;
            ITpFont *pFont = FontManager::GetInstance()->GetFont(iFontAttribute, color);
            if (pFont)
            {
                const bool enable = IsEnable();
                const int lineHeight = enable ? GetWindowMetrics(UIDialogBorderLineIndex) : 0;
                const int stringWidth = pFont->StringWidth(m_strShowText.c_str(), m_strShowText.length());
                const int stringHeight = pFont->GetHeight();
                const int left = (m_iWidth > stringWidth) ? ((m_iWidth - stringWidth) >> 1) : 0;
                const int top = (m_iHeight > (lineHeight + stringHeight)) ? (m_iHeight - lineHeight - stringHeight) : 0;
                grf.DrawStringUnicode(m_strShowText.c_str(), left, top, pFont, false);
                if (enable)
                {
                    if (IsFocus())
                    {
                        grf.DrawLine(left, m_iHeight - lineHeight, stringWidth, lineHeight, SOLID_STROKE);
                    }
                    else
                    {
                        grf.DrawLine(left, m_iHeight - lineHeight, stringWidth, lineHeight, DOTTED_STROKE);
                    }
                }
            }
        }
    }
    return S_OK;
}
