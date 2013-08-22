///////////////////////////////////////////////////////////////////////
// UISingleTextInputDialog.cpp
// Contact: liuhj
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#include "TouchAppUI/UISingleTextInputDialog.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UIMessageBox.h"
#include "I18n/StringManager.h"
#include "CommonUI/UIIMEManager.h"
#include "Common/File_DK.h"
#include "Framework/CDisplay.h"

const int UISingleTextInputDialog::m_DEFAULT_MARGIN = 30;
const int UISingleTextInputDialog::m_DEFAULT_WIDTH = 400;
const int UISingleTextInputDialog::m_DEFAULT_HEIGHT = 210;

const int UISingleTextInputDialog::m_DEFAULT_ELEMHEIGHT = 40;
const int UISingleTextInputDialog::m_DEFAULT_FONTSIZE = 22;
const int UISingleTextInputDialog::m_DEFAULT_ELEMSPACING = 20;

UISingleTextInputDialog::UISingleTextInputDialog(
        UIContainer* pParent,
        DialogType type,
        BOOL bSupportFont)
    : UIDialog(pParent)
    , m_bIsDisposed(FALSE)
    , m_bSupportFont(bSupportFont)
    , m_iDialogType(type)

{
    InitUI();
}

UISingleTextInputDialog::~UISingleTextInputDialog ()
{
    Dispose ();
}

void UISingleTextInputDialog::InitUI ()
{
    m_txtPasswordHintLabel.SetText(StringManager::GetStringById(INPUT_PASSWORD_NO_COLON));
    m_txtPasswordHintLabel.SetFont (0, 0, m_DEFAULT_FONTSIZE);
    m_txtPasswordHintLabel.SetForeColor (ColorManager::knBlack);

    m_tbPassword.SetTipUTF8(StringManager::GetStringById(INPUT_NUMBER_SYMBOL_SYM_PRESS));
    m_tbPassword.SetFont (0, 0, m_DEFAULT_FONTSIZE);
    m_tbPassword.SetFocus(TRUE);
	m_tbPassword.SetId(ID_TEXTBOX_PASSWORD);

    AppendChild(&m_txtPasswordHintLabel);
    AppendChild(&m_tbPassword);

    if(m_iDialogType == Type_Default)
    {
        m_btnOK.Initialize(IDOK, StringManager::GetStringById(ACTION_OK), m_DEFAULT_FONTSIZE, SPtr<ITpImage>());
        m_btnCancel.Initialize(IDCANCEL,  StringManager::GetStringById(ACTION_CANCEL), m_DEFAULT_FONTSIZE, SPtr<ITpImage>());
        
        AppendChild(&m_btnOK);
        AppendChild(&m_btnCancel);
    }
    else if(m_iDialogType == Type_AddLabel)
    {
        m_btnOK.Initialize(IDOK, StringManager::GetStringById(ADD), m_DEFAULT_FONTSIZE, SPtr<ITpImage>());
        m_btnCancel.Initialize(IDCANCEL,  StringManager::GetStringById(ACTION_CANCEL), m_DEFAULT_FONTSIZE, SPtr<ITpImage>());

        m_txtLabelHint.SetText(StringManager::GetStringById(PUT_SPACE_BETWEEN_LABEL));
        m_txtLabelHint.SetFont (0, 0, m_DEFAULT_FONTSIZE);
        m_txtLabelHint.SetForeColor (ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
        
        AppendChild(&m_btnOK);
        AppendChild(&m_btnCancel);
        AppendChild(&m_txtLabelHint);
    }

    UIIME* pIME = UIIMEManager::GetIME(IUIIME_CHINESE_LOWER, &m_tbPassword);
    if (pIME)
    {
        m_tbPassword.SetFocus(true);
        pIME->SetShowDelay(true);
    }
}

void UISingleTextInputDialog::MoveWindow(int _iLeft, int _iTop, int _iWidth, int _iHeight)
{
    DebugPrintf(DLC_UISINGLETEXTINPUTDLG, "%s, %d, %s, %s START", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    CDisplay* pDisplay = CDisplay::GetDisplay();
    if (pDisplay)
    {
        _iWidth = m_DEFAULT_WIDTH;
        _iHeight = m_DEFAULT_HEIGHT;
        _iLeft = (pDisplay->GetScreenWidth() - _iWidth) >> 1;
        _iTop = 200;
    }
    UIWindow::MoveWindow(_iLeft, _iTop, _iWidth, _iHeight);

    int iTop = m_DEFAULT_MARGIN;
    int iLeft = m_DEFAULT_MARGIN;
    int iWidth = m_iWidth - (m_DEFAULT_MARGIN << 1);
    m_txtPasswordHintLabel.MoveWindow (iLeft, iTop, iWidth, m_DEFAULT_ELEMHEIGHT);
    iTop += m_DEFAULT_ELEMHEIGHT + m_DEFAULT_ELEMSPACING;
    
    m_tbPassword.MoveWindow(iLeft, iTop, iWidth, m_DEFAULT_ELEMHEIGHT);
    iTop += (m_DEFAULT_ELEMHEIGHT + m_DEFAULT_ELEMSPACING);
    
    if(m_iDialogType == Type_Default)
    {
        const int iBtnWidth = (iWidth - m_DEFAULT_ELEMSPACING) >> 1;
        m_btnCancel.MoveWindow(iLeft, iTop, iBtnWidth, m_DEFAULT_ELEMHEIGHT);
        iLeft += iBtnWidth + m_DEFAULT_ELEMSPACING;

        m_btnOK.MoveWindow(iLeft, iTop, iBtnWidth, m_DEFAULT_ELEMHEIGHT);
    }
    else if(m_iDialogType == Type_AddLabel)
    {
        const int iBtnWidth = (iWidth - (m_DEFAULT_ELEMSPACING << 1)) >> 2;
        m_txtLabelHint.MoveWindow(iLeft, iTop, iBtnWidth << 1, m_DEFAULT_ELEMHEIGHT);
        iLeft += (iBtnWidth << 1) + m_DEFAULT_ELEMSPACING;

        m_btnCancel.MoveWindow(iLeft, iTop, iBtnWidth, m_DEFAULT_ELEMHEIGHT);
        iLeft += iBtnWidth + m_DEFAULT_ELEMSPACING;
        
        m_btnOK.MoveWindow(iLeft, iTop, iBtnWidth, m_DEFAULT_ELEMHEIGHT);
    }
    DebugPrintf(DLC_UISINGLETEXTINPUTDLG, "%s, %d, %s, %s END", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

void UISingleTextInputDialog::SetDialogTitle (const char * dialogText)
{
    if(NULL != dialogText)
    {
        m_txtPasswordHintLabel.SetText(dialogText);
    }
}

void UISingleTextInputDialog::InitPassword (CString text)
{
    if(!text.Length())
    {
        return;
    }
    m_tbPassword.SetTextUTF8(text.GetBuffer());
    m_tbPassword.SelectAll();
}

void UISingleTextInputDialog::SetMaxInputLen (INT32 iMaxLen)
{
    m_tbPassword.SetMaxDataLen(iMaxLen);
}


HRESULT UISingleTextInputDialog::DrawBackGround (DK_IMAGE drawingImg)
{
    DebugPrintf(DLC_UISINGLETEXTINPUTDLG, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    HRESULT hr(S_OK);
    DK_IMAGE imgSelf;
    DK_RECT rcSelf={0, 0, m_iWidth, m_iHeight};

    RTN_HR_IF_FAILED(CropImage(
        drawingImg,
        rcSelf,
        &imgSelf
        ));
    CTpGraphics grp(imgSelf);

    SPtr<ITpImage> spImg = ImageManager::GetImage (IMAGE_MENU_BACK);
    if (spImg)
    {
        RTN_HR_IF_FAILED(grp.DrawImageStretchBlend (spImg.Get (), 0, 0, m_iWidth, m_iHeight));
    }

    return S_OK;
}

void UISingleTextInputDialog::Dispose ()
{
    OnDispose (m_bIsDisposed);
}

void UISingleTextInputDialog::OnDispose (BOOL bDisposed)
{
    if (bDisposed)
    {
        return;
    }

    m_bIsDisposed = TRUE;
}

BOOL UISingleTextInputDialog::OnKeyPress(INT32 iKeyCode)
{
    DebugPrintf(DLC_UISINGLETEXTINPUTDLG, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);

    return UIDialog::OnKeyPress(iKeyCode);
}

void UISingleTextInputDialog::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    DebugPrintf(DLC_UISINGLETEXTINPUTDLG, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    switch (dwCmdId)
    {/*
    case ID_TEXTBOX_TOUCH_PASSWORD:
        {
            DebugPrintf(DLC_UISINGLETEXTINPUTDLG, "%s, %d, %s, %s ID_TEXTBOX_TOUCH_PASSWORD", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        }
        break;*/
    case IDOK:
        {
            DebugPrintf(DLC_UISINGLETEXTINPUTDLG, "%s, %d, %s, %s IDOK", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            m_password = m_tbPassword.GetTextUTF8();
            if(m_password.empty())
            {
    			m_tbPassword.SetTipUTF8(StringManager::GetPCSTRById(INPUTISNULL_REPEAT),0);
    			m_tbPassword.SetFocus(TRUE);
    			m_tbPassword.UpdateWindow();
            }
            else
            {
                EndDialog(1);
            }
            break;
        }
    case IDCANCEL:
        EndDialog(0);
        break;
    default: 
        //EndDialog(0);
        break;
    }
    DebugPrintf(DLC_UISINGLETEXTINPUTDLG, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

