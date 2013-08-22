////////////////////////////////////////////////////////////////////////
// UIAutoPageDownDlg.cpp
// Contact: liuhj
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "TouchAppUI/UIAutoPageDownDlg.h"
#include "I18n/StringManager.h"

#define AUTO_PAGE_DOWN_MAX_VALUE        (180)
#define AUTO_PAGE_DOWN_MIN_VALUE         (5)

UIAutoPageDownDlg::UIAutoPageDownDlg()
    : UIComplexDialog()
    , m_fInited (FALSE)
{
}

UIAutoPageDownDlg::UIAutoPageDownDlg(UIContainer* pParent, const DWORD dwId)
    : UIComplexDialog(pParent, dwId)
    , m_fInited (FALSE)
{
}

void UIAutoPageDownDlg::Init()
{
}

void UIAutoPageDownDlg::Initialize()
{
    if (this->m_fInited)
    {
        return;
    }

    INT iFontSize = 20;
    INT iTextTop = (m_iHeight - iFontSize) / 2 - 2;

    if( NULL == m_pTextBox)
    {
        m_pTextBox = new UITextBox(this,m_dwId);
    }
    if(m_pTextBox)
    {
        m_pTextBox->SetFontSize(iFontSize);
        m_pTextBox->MoveWindow(65, 12, 96, 31);
    }

    this->m_txtHeadTitle.SetFontSize(iFontSize);
    this->m_txtHeadTitle.SetText(StringManager::GetInstance()->GetStringById(AUTO_PAGE_DOWN_EVERY));
    this->m_txtHeadTitle.MoveWindow(15, iTextTop, 60, m_iHeight - iTextTop);

    this->m_txtTailTitle.SetFontSize(iFontSize);
    this->m_txtTailTitle.SetText(StringManager::GetInstance()->GetStringById(DO_AUTO_PAGE_DOWN));
    this->m_txtTailTitle.MoveWindow(170, iTextTop, m_iWidth - 170, m_iHeight - iTextTop);

    AppendChild(this->m_pTextBox);
    AppendChild(&this->m_txtHeadTitle);
    AppendChild(&this->m_txtTailTitle);

    m_pTextBox->SetFocus(TRUE);
    m_pBtnEnter->ShowIcon(FALSE);
    RemoveChild(m_pBtnEnter, false);
    SetNumberOnlyMode ();

    m_fInited = TRUE;
    return;
}

UIAutoPageDownDlg::~UIAutoPageDownDlg()
{
}

HRESULT UIAutoPageDownDlg::Draw(DK_IMAGE image)
{
    return UIDialog::Draw(image);
}

HRESULT UIAutoPageDownDlg::DrawBackGround(DK_IMAGE drawingImg)
{
    return UIComplexDialog::DrawBackGround(drawingImg);
}

BOOL UIAutoPageDownDlg::OnKeyPress(INT32 iKeyCode)
{
    SetChildWindowFocus (GetChildIndex(this->m_pTextBox), FALSE);

    UIWindow* pWin = GetChildByIndex(m_iFocusedChild);
    if(strcmp(pWin->GetClassName(),"UITextBox")==0 && !pWin->OnKeyPress(iKeyCode))
    {
        return FALSE;
    }

    BOOL bRet = UIComplexDialog::OnKeyPress(iKeyCode);
    if (!bRet && KEY_OKAY == iKeyCode)
    {
        if (!this->m_pTextBox->IsEmpty())
        {
            INT iNum = atoi(this->m_pTextBox->GetTextUTF8().c_str());
            if (AUTO_PAGE_DOWN_MIN_VALUE > iNum)
            {
                iNum = AUTO_PAGE_DOWN_MIN_VALUE;
            }
            else if (AUTO_PAGE_DOWN_MAX_VALUE < iNum)
            {
                iNum = AUTO_PAGE_DOWN_MAX_VALUE;
            }
            EndDialog(iNum);
        }
    }

    return bRet;
}


