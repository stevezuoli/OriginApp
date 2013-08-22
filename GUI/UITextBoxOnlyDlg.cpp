#include "GUI/UITextBoxOnlyDlg.h"
#include "GUI/UISizer.h"
#include "CommonUI/UISoftKeyboardIME.h"
#include "CommonUI/UIIMEManager.h"

UITextBoxOnlyDlg::UITextBoxOnlyDlg(UIContainer* parent)
    : UIDialog(parent)
    , m_textBox(this, IDOK)
{
    InitUI();
}

void UITextBoxOnlyDlg::InitUI()
{
    UIBoxSizer* mainSizer = new UIBoxSizer(dkVERTICAL);
    if (mainSizer)
    {
        AppendChild(&m_textBox);
#ifdef KINDLE_FOR_TOUCH
        HookTouch();
#endif

        mainSizer->Add(&m_textBox, UISizerFlags(1).Expand());
        SetSizer(mainSizer);
    }
	UIIME* pIME = UIIMEManager::GetIME(IUIIME_ENGLISH_LOWER, &m_textBox);
	if(pIME)
	{
		pIME->SetShowDelay(true);
	}
}


void UITextBoxOnlyDlg::SetFontSize(int fontSize)
{
    m_textBox.SetFont(0, 0, fontSize);
}

void UITextBoxOnlyDlg::SetTipUTF8(const char* text)
{
    m_textBox.SetTipUTF8(text, strlen(text));
}

std::string UITextBoxOnlyDlg::GetTextUTF8() const
{
    return m_textBox.GetTextUTF8();
}

void UITextBoxOnlyDlg::SetTextBoxStyle(TextBoxStyle eTextBoxStyle)
{
    SetChildWindowFocus(GetChildIndex(&m_textBox), false);
    m_textBox.SetTextBoxStyle(eTextBoxStyle);
    UIIME* pIME = UIIMEManager::GetIME(m_textBox.GetInputMode(), &m_textBox);
    if (pIME)
    {
        pIME->SetShowDelay(true);
    }
}

void UITextBoxOnlyDlg::SetInitInputMode(INPUTMODE inputmode)
{
    UIIME* pIME = UIIMEManager::GetIME(inputmode, &m_textBox);
    if (pIME)
    {
        pIME->SetShowDelay(true);
    }
}

void UITextBoxOnlyDlg::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    if (IDOK == dwCmdId)
    {
        EndDialog(IDOK);
    }
}

void UITextBoxOnlyDlg::SetTextBoxImage(int imageId, int focusedImageId)
{
    m_textBox.SetImage(imageId, focusedImageId);
}

#ifdef KINDLE_FOR_TOUCH
bool UITextBoxOnlyDlg::OnHookTouch(MoveEvent* moveEvent)
{
    if (moveEvent->GetActionMasked() != MoveEvent::ACTION_DOWN)
    {
        return false;
    }
    if (IsVisible() && !PointInWindowAbsolute(moveEvent->GetX(), moveEvent->GetY()))
    {
    	UISoftKeyboardIME* pIME = UISoftKeyboardIME::GetInstance();
		if(pIME->IsShow())
		{
			if(pIME->PointInWindowAbsolute(moveEvent->GetX(), moveEvent->GetY()))
			{
				return false;
			}
		}
      	EndDialog(IDCANCEL);
        return true;
    }
    return false;
}
#endif

BOOL UITextBoxOnlyDlg::OnKeyPress(int keycode)
{
    if (m_textBox.OnKeyPress(keycode))
    {
        if (KEY_OKAY == keycode)
        {
            EndDialog(IDOK);
            return false;
        }
        else
        {
            return UIDialog::OnKeyPress(keycode);
        }
    }
    else
    {
        return false;
    }
}
