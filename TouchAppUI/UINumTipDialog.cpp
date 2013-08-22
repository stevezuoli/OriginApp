////////////////////////////////////////////////////////////////////////
// Contact: wulf
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "TouchAppUI/UINumTipDialog.h"
#include "GUI/CTpGraphics.h"
#include "Utility/ImageManager.h"
#include "GUI/FontManager.h"
#include "Framework/CDisplay.h"
#include <stdio.h>
#include <pthread.h>

UINumTipDialog* UINumTipDialog::m_Instance = NULL;

UINumTipDialog::UINumTipDialog()
    : UIDialog(NULL)
    , m_pEventHandler(NULL)
{
    Init();
}

UINumTipDialog:: UINumTipDialog(UIContainer* pParent, const DWORD dwId)
    : UIDialog(pParent)
    , m_pEventHandler(pParent)
{
    m_dwId = dwId;
}

UINumTipDialog::~ UINumTipDialog()
{

}

UINumTipDialog* UINumTipDialog::GetInstance()
{
    if(m_Instance == NULL)
    {
        m_Instance = new UINumTipDialog();
    }
    return m_Instance;
}

CString UINumTipDialog::GetLetterOfNum(int num)
{
    CString str;
    switch (num)
    {
        case 1:
            str = CString("Q");
            break;
        case 2:
            str = CString("W");
            break;
        case 3:
            str = CString("E");
            break;
        case 4:
            str = CString("R");
            break;
        case 5:
            str = CString("T");
            break;
        case 6:
            str = CString("Y");
            break;
        case 7:
            str = CString("U");
            break;
        case 8:
            str = CString("I");
            break;
        case 9:
            str = CString("O");
            break;
        case 0:
            str = CString("P");
            break;
        default:
            break;
    }
    return str;
}
void UINumTipDialog::OnNotify(UIEvent rEvent)
{
    UIDialog::OnNotify(rEvent);
}

INT32 UINumTipDialog::DoModal()
{
    DebugPrintf(DLC_CX,"here in DoModal()");
    INT res = UIDialog::DoModal();
    return res;
}


HRESULT UINumTipDialog::DrawBackground(DK_IMAGE drawingImg)
{
    CTpGraphics prg(drawingImg);
    SPtr<ITpImage> spImg = ImageManager::GetImage(IMAGE_MENU_BACK);
    if (spImg)
    {
        prg.DrawImageStretchBlend(spImg.Get(), 0, 0, m_iWidth, m_iHeight);
    }
    prg.DrawLine(0, m_iHeight /2, m_iWidth, 2, SOLID_STROKE);
    return S_OK;
}

HRESULT UINumTipDialog::Draw(DK_IMAGE drawingImg)
{
    if (!m_bIsVisible)
    {
        return S_OK;
    }
    if (drawingImg.pbData == NULL)
    {
           return E_FAIL;
    }
    
    HRESULT hr(S_OK);
    DK_IMAGE imgSelf;
    DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
    RTN_HR_IF_FAILED(CropImage(
            drawingImg,
            rcSelf,
            &imgSelf
        ));
    
    DrawBackground(imgSelf);

    CTpGraphics prg(imgSelf);
    DKFontAttributes iFontAttribute;
    iFontAttribute.SetFontAttributes(0, 0, 24);
    INT32 iColor = ColorManager::GetColor(COLOR_SOFTKEY_TXT);
    ITpFont *pFont = FontManager::GetInstance()->GetFont(iFontAttribute, iColor);
    if(pFont == NULL)
    {
        return 0;
    }
    
    int i = 0;
    int iWidth = 0;
    int iHeight = 0;
    int iLeft = 0;
    int iTop = 0;
    CString lable = CString("");
    for(i=0; i < 10; i++)
    {
        lable = GetLetterOfNum((i+1) % 10);
        iWidth = pFont->StringWidth(lable);
        iHeight = pFont->GetHeight();
        iLeft = (m_iWidth / 10 - iWidth ) / 2 + i * m_iWidth / 10; 
        iTop = m_iHeight / 2 + (m_iHeight / 2 - iHeight) / 2;
        prg.DrawStringUtf8(lable.GetBuffer(), iLeft, iTop, pFont);

        iTop = (m_iHeight / 2 - iHeight) / 2;
        lable = CString((i+1) % 10);
        prg.DrawStringUtf8(lable, iLeft, iTop, pFont);
    }
    return S_OK;
}

BOOL UINumTipDialog::OnKeyPress(INT32 iKeyCode)
{
    if(m_pParent != NULL)
        return m_pParent->OnKeyPress(iKeyCode);
    return FALSE;
        
}

void UINumTipDialog::Show()
{
    this->SetVisible(TRUE);
    this->m_pParent->AppendChild(this);
}

void UINumTipDialog::Hide()
{
    this->SetVisible(FALSE);
    this->m_pParent->RemoveChild(this, false);
}
