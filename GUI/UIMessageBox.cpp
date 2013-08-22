////////////////////////////////////////////////////////////////////////
// UIMessageBox.cpp
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "GUI/UIMessageBox.h"
#ifdef KINDLE_FOR_TOUCH
#include "GUI/UITouchComplexButton.h"
#else
#include "GUI/UIComplexButton.h"
#endif
#include "Utility/ImageManager.h"
#include "Utility/ColorManager.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UISizer.h"
#include "Framework/CDisplay.h"
#include "I18n/StringManager.h"
#include "Framework/CMessageHandler.h"
#include "Common/WindowsMetrics.h"
#include "CommonUI/UIUtility.h"
#include "Framework/CNativeThread.h"
#include <map>

using namespace WindowsMetrics;

UIMessageBox::UIMessageBox()
    : UIDialog(NULL)
    , m_iType(0)
    , m_bIsDisposed(FALSE)
    , m_textDisplay(this, IDSTATIC)
    , m_textTip(this, IDSTATIC)
    , m_iSelectItems(0)
{
    Init();
}

UIMessageBox::UIMessageBox(UIContainer* pParent, LPCSTR szText, INT32 iType)
    : UIDialog(pParent)
    , m_iType(iType)
    , m_bIsDisposed(FALSE)
    , m_textDisplay(this, IDSTATIC)
    , m_textTip(this, IDSTATIC)
    , m_iSelectItems(0)
{
    m_textDisplay.SetText(szText);
    Init();
}

UIMessageBox::~UIMessageBox()
{
    Dispose();
}

void UIMessageBox::Dispose()
{
    UIDialog::Dispose();
    OnDispose(m_bIsDisposed);
}

void UIMessageBox::OnDispose(BOOL bDisposed)
{
    if (bDisposed)
    {
        return;
    }

    m_bIsDisposed = TRUE;
    Finalize();
}

void UIMessageBox::Init()
{
	SetMinWidth(GetWindowMetrics(UIMessageBoxWidthIndex));
    InitMember();
    InitUI();
    InitSizer();
}

void UIMessageBox::InitMember()
{
    m_bIsDisposed = FALSE;
}

void UIMessageBox::InitUI()
{
    const int fontsize22 = GetWindowFontSize(FontSize22Index);

    m_textDisplay.SetFontSize(fontsize22);
    m_textDisplay.SetMode(UIAbstractText::TextMultiLine);
    m_textDisplay.SetAutoModifyHeight(false);
    AppendChild(&m_textDisplay);

	m_textTip.SetFontSize(GetWindowFontSize(FontSize18Index));
    m_textTip.SetMode(UIAbstractText::TextMultiLine);
    m_textTip.SetAutoModifyHeight(false);
	m_textTip.SetAlign(ALIGN_JUSTIFY);
	m_textTip.SetVisible(false);
    AppendChild(&m_textTip);

    AppendChild(&m_btnItems);
    if (m_iType & MB_NO)
    {
        AddButtonForType(MB_NO);
    }
    if (m_iType & MB_CANCEL)
    {
        AddButtonForType(MB_CANCEL);
    }
    if (m_iType & MB_YES)
    {
        AddButtonForType(MB_YES);
    }
    if ((m_iType & MB_OK) || (m_iType == MB_TIPMSG))
    {
        AddButtonForType(MB_OK);
    }
	if(m_iType & MB_WITHOUTBTN)
	{
		AddButtonForType(MB_WITHOUTBTN);
	}
    InitFocusedObj();
}

void UIMessageBox::InitSizer()
{
    const int textHorzBorder = GetWindowMetrics(UIMessageBoxTextHorzBorderIndex);
    const int textVertBorder = GetWindowMetrics(UIMessageBoxTextVertBorderIndex);
    const int btnHeight = GetWindowMetrics(UIMessageBoxBtnHeightIndex);
    m_btnItems.SetMinHeight(btnHeight);
    m_btnItems.SetSplitLineHeight(btnHeight - (GetWindowMetrics(UIButtonGroupSplitLineTopPaddingIndex) << 1));
    m_btnItems.SetBottomLinePixel(0);
    const int width = GetWindowMetrics(UIMessageBoxWidthIndex);
    m_textDisplay.SetMinWidth(width - (textHorzBorder << 1));
	m_textTip.SetMinWidth(width - (textHorzBorder << 1));
    if (!m_windowSizer)
    {
        UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
        if (mainSizer)
        {
            mainSizer->Add(&m_textDisplay, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, textHorzBorder).Border(dkTOP, textVertBorder).Border(dkBOTTOM, textVertBorder/2));
			mainSizer->Add(&m_textTip, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, textHorzBorder).Border(dkBOTTOM, textVertBorder/2));
            mainSizer->Add(&m_btnItems, UISizerFlags().Expand().Border(dkTOP, textVertBorder/2));
        }
        SetSizer(mainSizer);
    }
}

void UIMessageBox::MakeCenterAndLayout()
{
    if (m_windowSizer)
    {
        int width = GetMinWidth();
        int height = GetMinHeight();
        MakeCenter(width, height);
        Layout();
    }
}

void UIMessageBox::InitFocusedObj()
{
    if ((m_iType & MB_OK) || (m_iType == MB_TIPMSG))
    {
        SetFocusButton(MB_OK);
    }
    else if (m_iType & MB_YES)
    {
        SetFocusButton(MB_YES);
    }
    else if (m_iType & MB_CANCEL)
    {
        SetFocusButton(MB_CANCEL);
    }
    else if (m_iType & MB_NO)
    {
        SetFocusButton(MB_NO);
    }
}

bool UIMessageBox::AddButtonForType(int type)
{
	if(MB_WITHOUTBTN == type)
	{
		m_btnItems.SetVisible(false);
		return true;
	}
    const int fontsize20 = GetWindowFontSize(UIMessageBoxButtonIndex);
    static std::map<int, const int> btnTexts;
    if (btnTexts.empty())
    {
        btnTexts.insert(std::make_pair(MB_OK, ACTION_OK));
        btnTexts.insert(std::make_pair(MB_YES, ACTION_YES));
        btnTexts.insert(std::make_pair(MB_CANCEL, ACTION_CANCEL));
        btnTexts.insert(std::make_pair(MB_NO, ACTION_NO));
    }

    std::map<int, const int>::iterator iter = btnTexts.find(type);
    if (iter == btnTexts.end())
    {
        return false;
    }

#ifdef KINDLE_FOR_TOUCH
    UITouchComplexButton* button = new UITouchComplexButton;
    if (button)
    {
        button->Initialize(type, StringManager::GetPCSTRById((SOURCESTRINGID)(iter->second)), fontsize20);
        m_btnItems.AddButton(button);
        return true;
    }
#else
    static std::map<int, const char> btnShortCutKeys;
    if (btnShortCutKeys.empty())
    {
        btnShortCutKeys.insert(std::make_pair(MB_OK, 'A'));
        btnShortCutKeys.insert(std::make_pair(MB_CANCEL, 'B'));
        btnShortCutKeys.insert(std::make_pair(MB_YES, 'C'));
        btnShortCutKeys.insert(std::make_pair(MB_NO, 'D'));
    }
    std::map<int, const char>::iterator iterShortCutKey = btnShortCutKeys.find(type);
    if (iterShortCutKey == btnShortCutKeys.end())
    {
        return false;
    }
    UIComplexButton* button = new UIComplexButton;
    if (button)
    {
        button->Initialize(type, StringManager::GetPCSTRById((SOURCESTRINGID)(iter->second)), iterShortCutKey->second, fontsize20, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
        m_btnItems.AddButton(button);
        return true;
    }
#endif
    return false;
}

void UIMessageBox::Finalize()
{
    int btnSize = m_btnItems.GetChildrenCount();
    for (int i = btnSize - 1; i >= 0; --i)
    {
        m_btnItems.RemoveChildByIndex(i, true);
    }
}

void UIMessageBox::SetText(LPCSTR szText)
{
    m_textDisplay.SetText(szText);
}

UIText* UIMessageBox::GetTextDisplay()
{
	return &m_textDisplay;
}

UIText* UIMessageBox::GetTextTip()
{
	return &m_textTip;
}

LPCSTR UIMessageBox::GetText() const
{
    return m_textDisplay.GetText();
}

void UIMessageBox::SetTipText(const char* tipText)
{
	m_textTip.SetText(tipText);
	m_textTip.SetVisible(true);
	int textHeight = m_textTip.GetTextHeight();
	m_textTip.SetMinHeight(textHeight);
}

HRESULT UIMessageBox::DrawBackGround(DK_IMAGE drawingImg)
{
    HRESULT hr(S_OK);
    RTN_HR_IF_FAILED(UIUtility::DrawBorder(drawingImg, 0, 0, 0, 0));
    return hr;
}

void UIMessageBox::SetButtonText(INT32 id, LPCSTR szText)
{
    UIButton* pButton = m_btnItems.GetButtonByCommandId(id);
    if(pButton && szText)
    {
        pButton->SetText(szText);
    }
}

void UIMessageBox::OnCommand(DWORD _dwCmdId, UIWindow* _pSender, DWORD _dwParam)
{
    EndDialog(_dwCmdId);
}

BOOL UIMessageBox::OnKeyPress(INT32 iKeyCode)
{
    INT32 iEndCode = 0;
    switch (iKeyCode)
    {
    case KEY_BACK:
        EndDialog(iEndCode);
        return FALSE;
    case KEY_HOME:
        return TRUE;
    }
    return UIDialog::OnKeyPress(iKeyCode);
}

void UIMessageBox::SetEnableHookTouch(bool isEnable)
{
	isEnable ? HookTouch() : UnhookTouch();
}

void UIMessageBox::SetFocusButton(INT32 id)
{
    m_btnItems.SetFocusedButton(m_btnItems.GetButtonByCommandId(id));
}

void UIMessageBox::Popup()
{
	MakeCenterAndLayout();
	UIDialog::Popup();
}

INT32 UIMessageBox::DoModal()
{
    if(m_iType == MB_TIPMSG)
    {
        Popup();
        BOOL bDestoryMsgBox(false);
        BOOL bEnterSaveScreen(false);
        struct timeval  abstv_start;
        struct timeval  abstv_end;
        gettimeofday(&abstv_start, NULL);
        while (!bDestoryMsgBox)
        {
            SNativeMessage msg;
            CMessageHandler::GetMessage(msg);
            switch (msg.iType)
            {
            case KMessageKeyboard:
                if(msg.iParam2 ==(UINT32) DK_KEYTYPE_DOWN)
                {
                    if(KEY_HOME == msg.iParam1)
                    {
                        this->EndDialog(0);
                        if(NULL != m_pParentDialog)
                        {
                            m_pParentDialog->EndDialog(0);
                        }
                        CMessageHandler::DispatchMessage(msg);
                        bDestoryMsgBox = true;
                    }
                    else if(KEY_BACK == msg.iParam1)
                    {
                        EndDialog(0);
                        bDestoryMsgBox = true;
                    }
                    else
                    {
                        EndDialog(0);
                        CMessageHandler::DispatchMessage(msg);
                        bDestoryMsgBox = true;
                    }
                }
                break;
            case KMessageGoHome:
            case KMessageGoPrePage:
            case KMessageUpdateUI:
            case KMessageUsbSingle:
            case KMessageWebBrowserStart:
            case KMessageWebBrowserStop:
                {
                    EndDialog(0);
                    CNativeThread::Send(msg);
                    bDestoryMsgBox = true;
                }
                break;
            case KMessageScreenSaverOut:
                CMessageHandler::DispatchMessage(msg);
                bEnterSaveScreen = false;
                break;
            case KMessageScreenSaverIn:
                 bEnterSaveScreen = true;
                 CMessageHandler::DispatchMessage(msg);
                 break;
            case KMessagePaint:
                CMessageHandler::DispatchMessage(msg);
                break;
            case KMessageBatteryLowAlarm:
                CMessageHandler::DispatchMessage(msg);
                break;
            default:
                break;
            }
            gettimeofday(&abstv_end, NULL);
            if(abstv_end.tv_sec - abstv_start.tv_sec >= 3 && false == bEnterSaveScreen)
            {
                EndDialog(0);
                bDestoryMsgBox = true;
                break;
            }
        }
    }
    else
    {
         return UIDialog::DoModal();
    }
    return 0;
}

