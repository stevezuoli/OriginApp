////////////////////////////////////////////////////////////////////////
// UIBookListBox.cpp
// Contact:
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "TouchAppUI/UIComplexDialog.h"
#include "GUI/CTpGraphics.h"
#define CONTROL_TO_CONTROL_WIDTH (20)
#define CONTROL_TO_PARENTCONTROL_WIDTH (12)
#include "CommonUI/UISoftKeyboardIME.h"
#include "Common/WindowsMetrics.h"
#include "GUI/UISizer.h"
using namespace WindowsMetrics;

UIComplexDialog::UIComplexDialog()
    : UIDialog(NULL)
    , m_pEventHandler(NULL)
    , m_dwId(0)
    , m_pImage(NULL)
    , m_pBtnEnter(NULL)
    , m_pTextBox(NULL)
    , m_ifontAttr()
    , m_iFontColor(0)
    , m_showIME(false)
{
    Init();
}

UIComplexDialog::UIComplexDialog(UIContainer* pParent, const DWORD dwId)
    : UIDialog(pParent)
    , m_pEventHandler(pParent)
    , m_dwId(0)
    , m_pImage(NULL)
    , m_pBtnEnter(NULL)
    , m_pTextBox(NULL)
    , m_ifontAttr()
    , m_iFontColor(0)
    , m_showIME(false)
{
    m_dwId = dwId;
    Init();
}


UIComplexDialog::UIComplexDialog(UIContainer* pParent, const DWORD dwId, bool showIME)
    : UIDialog(pParent)
    , m_pEventHandler(pParent)
    , m_dwId(dwId)
    , m_pImage(NULL)
    , m_pBtnEnter(NULL)
    , m_pTextBox(NULL)
    , m_ifontAttr()
    , m_iFontColor(0)
    , m_showIME(showIME)
{
    Init();
}

void UIComplexDialog::Init()
{
    DebugPrintf(DLC_UICOMPLEXDLG, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    m_ifontAttr.SetFontAttributes(0, 0, 20);
    UIBoxSizer* mainSizer = new UIBoxSizer(dkVERTICAL);

    const int btnLeftPadding = GetWindowMetrics(UIButtonLeftPaddingIndex);
    const int btnTopPadding = GetWindowMetrics(UIButtonTopPaddingIndex);
    if(NULL == m_pBtnEnter)
    {
        m_pBtnEnter = new UITouchComplexButton(this,m_dwId);
        if(m_pBtnEnter)
        {
            m_pBtnEnter->SetTopMargin(btnTopPadding);
            m_pBtnEnter->SetLeftMargin(btnLeftPadding);
            m_pBtnEnter->SetFontSize(m_ifontAttr.m_iSize);
        }
    }
    if(NULL == m_pTextBox)
    {
        m_pTextBox = new UITextBox(this,m_dwId);
        if(m_pTextBox)
        {
            m_pTextBox->SetFont(m_ifontAttr.m_iFace,m_ifontAttr.m_iStyle,m_ifontAttr.m_iSize );
            m_pTextBox->SetFocus(1);
        }
    }
    UIBoxSizer* horzSizer = new UIBoxSizer(dkHORIZONTAL);
    horzSizer->Add(m_pTextBox, 1);
    horzSizer->Add(m_pBtnEnter, UISizerFlags().Border(dkLEFT, btnLeftPadding));
    mainSizer->AddStretchSpacer();
    mainSizer->Add(horzSizer, UISizerFlags().Expand().Border(dkALL, 8));
    mainSizer->AddStretchSpacer();
    SetSizer(mainSizer);
	HookTouch();
    return;
}

void UIComplexDialog::OnIMEChange(BOOL fShow)
{
#ifndef KINDLE_FOR_TOUCH
    DebugPrintf(DLC_UICOMPLEXDLG, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    static INT32 old = 0;
    if(fShow)
    {
        DebugPrintf(DLC_CX,"查找框 处理显示输入法");
        old = m_iTop;
        MoveWindow(m_iLeft, UIIM::GetInstance()->GetY() - m_iHeight - 2,m_iWidth,m_iHeight);
        Repaint();
    }
    else
    {
        DebugPrintf(DLC_CX,"查找框 处理隐藏输入法");
        MoveWindow(m_iLeft,old,m_iWidth,m_iHeight);
        Repaint();
    }
#endif
}

void UIComplexDialog::OnIMEShow(void* pSender)
{
    OnIMEChange(TRUE);
}
void UIComplexDialog::OnIMEHide(void* pSender)
{
    OnIMEChange(FALSE);
}

UIComplexDialog::~UIComplexDialog()
{
#ifndef KINDLE_FOR_TOUCH
    UIIM::GetInstance()->Event_OnShow -= Delegate(UIComplexDialog,OnIMEShow);
    UIIM::GetInstance()->Event_OnHide -= Delegate(UIComplexDialog,OnIMEHide);
    UISoftKeyboardIME::GetInstance()->Event_OnShow -= Delegate(UIComplexDialog,OnIMEShow);
    UISoftKeyboardIME::GetInstance()->Event_OnHide -= Delegate(UIComplexDialog,OnIMEHide);
#endif
    if(m_pBtnEnter != NULL)
    {
        delete m_pBtnEnter;
        m_pBtnEnter = NULL;
    }
    if(m_pTextBox != NULL)
    {
        delete m_pTextBox;
        m_pTextBox = NULL;
    }
	if(m_pImage != NULL)
	{
		delete m_pImage;
		m_pImage = NULL;
	}
}

void UIComplexDialog::SetEnterBtnText(const CString& strText)
{
    DebugPrintf(DLC_UICOMPLEXDLG, "%s, %d, %s, %s->%s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, strText.GetBuffer());
    if( NULL == m_pBtnEnter)
    {
        m_pBtnEnter = new UITouchComplexButton(this,m_dwId);
    }
    if(m_pBtnEnter)
    {
        m_pBtnEnter->SetFontSize(m_ifontAttr.m_iSize);
        m_pBtnEnter->SetText(strText);
    }
}

void UIComplexDialog::SetNumberOnlyMode()
{
    DebugPrintf(DLC_UICOMPLEXDLG, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if(m_pTextBox)
    {
        m_pTextBox->SetNumberOnlyMode();
    }
}
/*
#ifdef KINDLE_FOR_TOUCH
void UIComplexDialog::Popup()
{
	if(!m_pTextBox)
	{
		return;
	}

    UISoftKeyboardIME* pIME = UISoftKeyboardIME::GetInstance();
    if (pIME)
    {
        pIME->ClearAttachedWnd();
        pIME->AddAttachedWnd(this);
		pIME->SetTextBox(m_pTextBox);
		if(m_showIME)
		{
			pIME->ShowIME();
		}
    }
    UIDialog::Popup();
}

void UIComplexDialog::EndDialog(INT32 iEndCode,BOOL fRepaintIt)
{
	UISoftKeyboardIME::GetInstance()->ClearAttachedWnd();
    UIDialog::EndDialog(iEndCode, fRepaintIt);
}
#endif
*/
INT32 UIComplexDialog::DoModal()
{
    DebugPrintf(DLC_UICOMPLEXDLG, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if(m_pTextBox)
    {
        m_pTextBox->SetFocus(TRUE);
        m_pTextBox->SelectAll();
    }

    return UIDialog::DoModal();
}

void UIComplexDialog::SetTipUTF8(const CHAR *strText, INT32 iStrLen)
{
    DebugPrintf(DLC_UICOMPLEXDLG, "%s, %d, %s, %s->%s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, strText);
    if( NULL == m_pTextBox)
    {
        m_pTextBox = new UITextBox(this,m_dwId);
    }
    if(m_pTextBox)
    {
        m_pTextBox->SetFont(m_ifontAttr.m_iFace,m_ifontAttr.m_iStyle,m_ifontAttr.m_iSize );
        m_pTextBox->SetFocus(1);
        m_pTextBox->SetTipUTF8(strText,iStrLen);
    }
}

void UIComplexDialog::SetImage(SPtr<ITpImage> _spImage)
{
    DebugPrintf(DLC_UICOMPLEXDLG, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (NULL == m_pImage)
    {
        m_pImage = new UIImage(this,m_dwId);
    }
    if(m_pImage)
    {
    	SPtr<ITpImage> pimgMainLogo = ImageManager::GetImage(IMAGE_MAIN_LOGO);
    	m_pImage->SetImage(pimgMainLogo);
        m_pImage->MoveWindow(0, 0, pimgMainLogo->GetWidth(), pimgMainLogo->GetHeight());
    }
}

INT32 UIComplexDialog::SetTextUTF8(const CHAR *strText, INT32 iStrLen)
{
    DebugPrintf(DLC_UICOMPLEXDLG, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (m_pTextBox)
    {
        return (m_pTextBox->SetTextUTF8(strText, iStrLen));
    }
    else
    {
        return 0;
    }
}

std::string UIComplexDialog::GetTextUTF8()
{
    if(m_pTextBox)
    {
        return m_pTextBox->GetTextUTF8();
    }
    else
    {
        return "";
    }
}


HRESULT UIComplexDialog::DrawBackGround(DK_IMAGE drawingImg)
{
    DebugPrintf(DLC_UICOMPLEXDLG, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    CTpGraphics prg(drawingImg);
    SPtr<ITpImage> spImg = ImageManager::GetImage(IMAGE_MENU_BACK);
    if (spImg)
    {
        return prg.DrawImageStretchBlend(spImg.Get(), 0, 0, m_iWidth, m_iHeight);
    }
    return S_OK;    //No image needs being drawn, return OK.
}


HRESULT UIComplexDialog::Draw(DK_IMAGE image)
{
    return UIDialog::Draw(image);
#if 0
    DebugPrintf(DLC_UICOMPLEXDLG, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (!m_bIsVisible)
    {
        return S_OK;
    }
    if (image.pbData == NULL)
    {
        return E_FAIL;
    }
    HRESULT hr(S_OK);
    DK_IMAGE imgSelf;
    DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
    RTN_HR_IF_FAILED(CropImage(
        image,
        rcSelf,
        &imgSelf
        ));
    DrawBackground(imgSelf);
    
    if (m_pImage)
    {
        m_pImage->Draw(imgSelf);
    }

    int iLeft = m_pImage ? m_pImage->GetImageWidth() : 0, iTop = 0;
    int iWidth = m_iWidth - iLeft, iHeight = m_iHeight;
    //  计算m_cbtnEnter所占区域
    if(m_pBtnEnter)
    {
        ITpFont* pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, m_iFontColor);
        int icbtnUsedWidth =  pFont->StringWidth(m_pBtnEnter->GetText());
        if(icbtnUsedWidth > (m_iWidth >> 1))
        {
            icbtnUsedWidth = (m_iWidth >> 1);
        }
        int iLeft = m_iWidth - CONTROL_TO_PARENTCONTROL_WIDTH - icbtnUsedWidth;
        int iTop  = ((m_iHeight - m_ifontAttr.m_iSize ) >> 1);
        m_pBtnEnter->MoveWindow(iLeft,iTop,icbtnUsedWidth,m_ifontAttr.m_iSize + 6);
        m_pBtnEnter->ShowIcon(0);
        m_pBtnEnter->Draw(imgSelf);
        iWidth -= icbtnUsedWidth;

    }
    if(m_pTextBox)
    {
        m_pTextBox->MoveWindow(iLeft, iTop, iWidth, iHeight);
        m_pTextBox->Draw(imgSelf);
    }
    return S_OK;
/*
    DebugPrintf(DLC_UICOMPLEXDLG, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (!m_bIsVisible)
    {
        return S_OK;
    }
    if (image.pbData == NULL)
    {
        return E_FAIL;
    }
    HRESULT hr(S_OK);
    DK_IMAGE imgSelf;
    DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
    RTN_HR_IF_FAILED(CropImage(
        image,
        rcSelf,
        &imgSelf
        ));
    DrawBackground(imgSelf);

	const int controlMargin = GetWindowMetrics(UIComplexDlgControlMarginIndex);
	const int verticalMargin = GetWindowMetrics(UIComplexDlgVerticalMarginIndex);
	const int horizonMargin = GetWindowMetrics(UIComplexDlgHorizonMarginIndex);

    //  计算m_cbtnEnter所占区域
    int icbtnUsedWidth = 0;
    if(m_pBtnEnter)
    {
        ITpFont* pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, m_iFontColor);
        icbtnUsedWidth =  pFont->StringWidth(m_pBtnEnter->GetText());
        if(icbtnUsedWidth > (m_iWidth >> 1))
        {
            icbtnUsedWidth = (m_iWidth >> 1);
        }
        int iLeft = m_iWidth - horizonMargin - icbtnUsedWidth;
        int iTop  = ((m_iHeight - m_ifontAttr.m_iSize ) >> 1);
        m_pBtnEnter->MoveWindow(iLeft,iTop,icbtnUsedWidth,m_ifontAttr.m_iSize + 6);
        m_pBtnEnter->ShowIcon(0);
        m_pBtnEnter->Draw(imgSelf);
    }
    if(m_pTextBox)
    {
        int iLeft = horizonMargin;
        int iTop  = verticalMargin;
		int iwidth = m_iWidth - 2 * horizonMargin - icbtnUsedWidth - controlMargin ;
        m_pTextBox->MoveWindow(iLeft, iTop, iwidth, m_iHeight - 2 *verticalMargin);
        m_pTextBox->Draw(imgSelf);
    }
	DebugPrintf(DLC_LIWEI,"m_iLeft = %d,m_iTop = %d",m_iLeft,m_iLeft);
    return S_OK;
*/
#endif
}

BOOL UIComplexDialog::OnKeyPress(INT32 iKeyCode)
{
    DebugPrintf(DLC_UICOMPLEXDLG, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    //  TODO:焦点切换
    switch(iKeyCode)
    {
    case KEY_OKAY:
        if((m_pTextBox && m_pTextBox->IsFocus() && m_pTextBox->IsEnable() && m_pTextBox->OnKeyPress(iKeyCode)) || m_pTextBox->IsFocus() == FALSE)
        {
            if (m_pEventHandler)
            {
                m_pEventHandler->OnEvent(UIEvent(COMMAND_EVENT, this, m_dwId));
            }
        }
        return FALSE;
    default:
        break;
    }
    return UIDialog::OnKeyPress(iKeyCode);
}

void UIComplexDialog::OnCommand(DWORD cmdId, UIWindow* sender, DWORD wParam)
{
    if (sender == m_pTextBox || sender == m_pBtnEnter)
    {
        EndDialog(IDOK);
    }
}


bool UIComplexDialog::OnHookTouch(MoveEvent* moveEvent)
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
        DebugPrintf(DLC_DIAGNOSTIC, "UIComplexDialog::OnHookTouch() return true");
        return true;
    }
    return false;
}

bool UIComplexDialog::OnChildClick(UIWindow* child)
{
    if (child == m_pBtnEnter)
    {
        EndDialog(IDOK);
        return true;
    }
    return UIDialog::OnChildClick(child);
}
