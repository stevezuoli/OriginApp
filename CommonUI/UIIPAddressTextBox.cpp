#include "CommonUI/UIIPAddressTextBox.h"
#include "I18n/StringManager.h"
#include "Framework/CDisplay.h"
#include "GUI/FontManager.h"
#include "GUI/UIMessageBox.h"
#include "drivers/DeviceInfo.h"
#include "TouchAppUI/UINumInput.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "CommonUI/UIUtility.h"
#include <tr1/functional>

using namespace WindowsMetrics;
using namespace std;

UIIPAddressTextBox::UIIPAddressTextBox(UIContainer* pParent, const DWORD dwId)
    : UIContainer(pParent, dwId)
{
    InitUI();
}
UIIPAddressTextBox::UIIPAddressTextBox()
    : UIContainer()
{
    InitUI();
}

void UIIPAddressTextBox::InitUI()
{
    const int elemSpacing = GetWindowMetrics(UIElementMidSpacingIndex);
    UISizer* pMainSizer = new UIGridSizer(1, 4, elemSpacing, elemSpacing);
    if (pMainSizer)
    {
        const int fontsize14 = GetWindowFontSize(FontSize14Index);
        const int borderLine = GetWindowMetrics(UIBtnBorderLineIndex);
        for(int i = 0;i < IPV4DECNUMLENGTH;i++)
        {
            UITextBox& ipBox = m_vIPTexts[i];
            EventListener::SubscribeEvent(
                UIAbstractTextEdit::EventTextEditChange,
                ipBox, 
                std::tr1::bind(
                std::tr1::mem_fn(&UIIPAddressTextBox::OnInputChange),
                this,
                std::tr1::placeholders::_1));

            ipBox.SetFontSize(fontsize14);
            ipBox.SetNumberOnlyMode();
            ipBox.ShowBorder(false);
            ipBox.SetMarginTop(0);
            ipBox.SetMarginBottom(0);
            AppendChild(&ipBox);
            pMainSizer->Add(&ipBox, UISizerFlags(1).Expand().Border(dkLEFT | dkRIGHT, elemSpacing).Border(dkTOP | dkBOTTOM, borderLine));
        }
        SetSizer(pMainSizer);
    }
}
void  UIIPAddressTextBox::SetDefaultIPAddress(const std::string& address)
{
    string strSplit = ".";
    size_t endPos = address.find(strSplit), startPos = 0;
    int i = 0;
    while((endPos != string::npos) && (i < IPV4DECNUMLENGTH))
    {
        string subString = address.substr(startPos, endPos - startPos);
        m_defaultIPs[i] = subString;
        m_vIPTexts[i].SetTipUTF8(subString.c_str());
        startPos = endPos + 1;
        i++;
        endPos = address.find(strSplit, startPos);
    }
    
    if ((endPos == string::npos) && ((i + 1) == IPV4DECNUMLENGTH))
    {
        string subString = address.substr(startPos, address.length() - startPos);
        m_defaultIPs[i] = subString;
        m_vIPTexts[i].SetTipUTF8(subString.c_str());
    }
}

bool UIIPAddressTextBox::OnInputChange(const EventArgs& args)
{
    int i = 0;
    for(i = 0;i < IPV4DECNUMLENGTH;i++)
    {
        if (m_vIPTexts[i].IsFocus())
        {
            break;
        }
    }   
    if (i < IPV4DECNUMLENGTH)
    {
        string ipValue = m_vIPTexts[i].GetTextUTF8();
        int iValue = atoi(ipValue.c_str());
        if (iValue > 25)
        {
            char newValue[8] = {0};
            if (iValue > 255)
            {
                snprintf(newValue, DK_DIM(newValue), "%2d", (iValue / 10));
                m_vIPTexts[i].DoFireEvent(false);
                m_vIPTexts[i].SetTextUTF8(newValue);
                m_vIPTexts[i].DoFireEvent(true);
            }
            i++;
            NextFocus(false);
            if (i < IPV4DECNUMLENGTH)
            {
                if (iValue > 255)
                {
                    snprintf(newValue, DK_DIM(newValue), "%1d", (iValue % 10));
                    m_vIPTexts[i].DoFireEvent(false);
                    m_vIPTexts[i].SetTextUTF8(newValue);
                    m_vIPTexts[i].DoFireEvent(true);
                }
            }
            else if (m_pParent)
            {
                m_pParent->NextFocus(false);
            }
        }
    }
    return true;
}

HRESULT UIIPAddressTextBox::DrawBackGround(DK_IMAGE drawingImg)
{
    CTpGraphics prg(drawingImg);
    HRESULT hr(S_OK); 
    int fontColor = ColorManager::knBlack;
    if (!IsEnable())
    {
        fontColor = (0xff000000 | ColorManager::GetColor(COLOR_DISABLECOLOR));
        const int borderLine = GetWindowMetrics(UIBtnBorderLineIndex);
        RTN_HR_IF_FAILED(prg.FillRect(0, 0, m_iWidth, m_iHeight, fontColor));
        RTN_HR_IF_FAILED(prg.FillRect(borderLine, borderLine
            , m_iWidth - borderLine, m_iHeight - borderLine, ColorManager::knWhite));
    }
    else
    {
        fontColor = (0xff000000 | ColorManager::knBlack);
        RTN_HR_IF_FAILED(UIUtility::DrawBorder(drawingImg, 0, 0, 0, 0, 0, true, true));
    }
    DKFontAttributes fontattr;
    const int fontsize16 = GetWindowFontSize(FontSize16Index);
    fontattr.SetFontAttributes(0,0,fontsize16);
    ITpFont* pFont = FontManager::GetInstance()->GetFont(fontColor);
    string csDot(".");
    const int elemSpacing = GetWindowMetrics(UIElementMidSpacingIndex);
    const int diff = ((m_iWidth - (elemSpacing * 3)) >> 2);
    int left = diff;
    const int top = 0;
    for(int i = 0; i < 3; i++)
    {
        prg.DrawStringUtf8(csDot.c_str(), left, top, pFont);
        left += (elemSpacing + diff);
    }
    return hr;
}

string UIIPAddressTextBox::GetIP()
{
    string ip;
    for(int i = 0; i < IPV4DECNUMLENGTH; i++)
    {
        string sIP = m_vIPTexts[i].GetTextUTF8();
        if (sIP.empty())
        {
            sIP = m_defaultIPs[i];
            if (sIP.empty())
            {
                return "";
            }
        }

        ip.append(sIP);
        if(i < IPV4DECNUMLENGTH-1)
        {
            ip.append(".");
        }
    }
    
    return ip;
}

void UIIPAddressTextBox::MoveWindow(int left, int top, int width, int height)
{
    UIContainer::MoveWindow(left, top, width, height);
    Layout();
}

BOOL UIIPAddressTextBox::OnKeyPress(INT32 iKeyCode)
{
    return FALSE;
}

