#include "TouchAppUI/UIUnlockDlg.h"
#include "I18n/StringManager.h"
#include "Framework/CDisplay.h"

using namespace std;

SINGLETON_CPP(UIUnlockDlg)

UIUnlockDlg::UIUnlockDlg(UIContainer* pParent)
    : UIDialog(pParent)
    , m_strRightPasswd("")
    , m_iSelected(0)
    , m_bUnlock(0)
{
    memset(m_szPasswd,0,PASSWORDITEMCOUNT + 1);
    UIInit();
}
UIUnlockDlg::UIUnlockDlg()
    : UIDialog(NULL)
    , m_strRightPasswd("")
    , m_iSelected(0)
    , m_bUnlock(0)
{
    memset(m_szPasswd,0,PASSWORDITEMCOUNT + 1);
    UIInit();
}
UIUnlockDlg::UIUnlockDlg(UIContainer* pParent,string _password)
    : UIDialog(pParent)
    , m_strRightPasswd("")
    , m_iSelected(0)
    , m_bUnlock(0)
{
    int iLen = PASSWORDITEMCOUNT;
    if(_password.length() < PASSWORDITEMCOUNT)
    {
        iLen = _password.length();
    }
    memcpy(m_szPasswd,_password.c_str(),iLen);
    m_szPasswd[iLen] = 0;
    UIInit();
}

void UIUnlockDlg::UIInit()
{
    m_txtInputPasswd.SetFontSize(18);
//    m_txtInputPasswd.SetAlign(ALIGN_CENTER);
    m_txtInputPasswd.SetText(StringManager::GetStringById(INPUT_UNLOCK_PASSWD));
    m_txtInputPasswd.MoveWindow(10,10,150,30);
    AppendChild(&m_txtInputPasswd);

    for(unsigned int i = 0;i < PASSWORDITEMCOUNT;i++)
    {
        m_vctPasswdItem[i].SetFontSize(30);
        m_vctPasswdItem[i].SetAlign(ALIGN_CENTER);
//        m_vctPasswdItem[i].seta(ALIGN_CENTER);
        m_vctPasswdItem[i].MoveWindow(10 + 70 * i,50,70,50);
        AppendChild(&m_vctPasswdItem[i]);
    }
}

HRESULT UIUnlockDlg::DrawBackGround(DK_IMAGE drawingImg)
{
    UIDialog::DrawBackGround(drawingImg);

    if (drawingImg.pbData == NULL)
    {
        return E_FAIL;
    }

    CTpGraphics prg(drawingImg);
    SPtr<ITpImage> spImg = ImageManager::GetImage(IMAGE_MENU_BACK);
    if (spImg)
    {
        prg.DrawImageStretchBlend(spImg.Get(), 0, 0, 300, 120);
        prg.DrawImageStretchBlend(spImg.Get(), 10, 40, 280, 60);
        prg.DrawLine(80,40,2,60,1);
        prg.DrawLine(150,40,2,60,1);
        prg.DrawLine(220,40,2,60,1);
    }
    return S_OK;
}
void UIUnlockDlg::UpdatePasswdItem()
{
    for(unsigned int i = 0;i < PASSWORDITEMCOUNT;i++)
    {
        char sztmp[4] = {m_szPasswd[i],0};
        m_vctPasswdItem[i].SetText(CString(sztmp));
    }
}

HRESULT UIUnlockDlg::DrawCursor(DK_IMAGE drawingImg)
{
    int iOffsetX = 0;
    CTpGraphics prg(drawingImg);
    if(m_szPasswd[m_iSelected] == 0)
    {
         iOffsetX = m_vctPasswdItem[m_iSelected].GetAbsoluteX() + m_vctPasswdItem[m_iSelected].GetWidth() / 2;
    }
    else
    {
        int stringWidth = m_vctPasswdItem[m_iSelected].GetTextWidth();
        if ( stringWidth > 0 )
            iOffsetX = m_vctPasswdItem[m_iSelected].GetAbsoluteX() + (m_vctPasswdItem[m_iSelected].GetWidth() + stringWidth) / 2;
    }
    return prg.DrawLine(iOffsetX,this->GetAbsoluteY() + 50/*m_vctPasswdItem[m_iSelected].GetAbsoluteY()*/,2,40,1);
}

HRESULT UIUnlockDlg::Draw(DK_IMAGE drawingImg)
{
    UpdatePasswdItem();
    HRESULT hr = UIDialog::Draw(drawingImg);
    if(hr != S_OK)
    {
        return hr;
    }
    return DrawCursor(drawingImg);
}

BOOL UIUnlockDlg::OnKeyPress(INT32 iKeyCode)
{    
    BOOL retValue = TRUE;

    char key = VirtualKeyToChar(iKeyCode);
    DebugPrintf(DLC_XU_KAI, "UIUnlockDlg::OnKeyPress: key  is %d",key);

    if(IsAlpha(iKeyCode) || IsDigit(iKeyCode))
    {
        m_szPasswd[m_iSelected] = key;
        m_iSelected++;
        if(m_iSelected >= PASSWORDITEMCOUNT)
        {
            m_iSelected = PASSWORDITEMCOUNT - 1;
        }
        retValue =  FALSE;
        this->UpdateWindow();
//            this->Repaint();
    }
    else
    {
        switch(iKeyCode)
        {
        case KEY_OKAY:
            if(!m_bUnlock)
            {
                if((m_szPasswd[0] == 0 && m_szPasswd[1] == 0 && m_szPasswd[2] == 0 && m_szPasswd[3] == 0)
                    || (m_szPasswd[0] != 0 && m_szPasswd[1] != 0 && m_szPasswd[2] != 0 && m_szPasswd[3] != 0))
                {
                    this->EndDialog(1);
                    retValue =  FALSE;
                }
                else
                {
                    retValue =  FALSE;
                }
            }
            else
            {
                if(!memcmp(m_szPasswd,m_strRightPasswd.c_str(),PASSWORDITEMCOUNT))
                {
                    this->EndDialog(1);
                    retValue =  FALSE;
                }
                else
                {
                    retValue =  FALSE;
                }
            }
            break;
        case KEY_DEL:
            //retValue = this->PressDelete(iKeyCode);
            m_szPasswd[m_iSelected] = 0;
             this->UpdateWindow();
           break;
        case KEY_SPACE:
            //retValue =  this->PressSpace(iKeyCode);
            break;

        case KEY_ENTER:
            //retValue =  this->PressEnter(iKeyCode);
            break;

        case KEY_LEFT:
            m_iSelected--;
            if(m_iSelected <= 0)
            {
                m_iSelected = 0;
            }
            retValue =  FALSE;
            this->UpdateWindow();
//            this->Repaint();
            break;
        case KEY_RIGHT:
            m_iSelected++;
            if(m_iSelected >= PASSWORDITEMCOUNT)
            {
                m_iSelected = PASSWORDITEMCOUNT - 1;
            }
            retValue =  FALSE;
            this->UpdateWindow();
//            this->Repaint();
            break;
        case KEY_FONT:
            break;
        case KEY_BACK:
            break;
        case KEY_HOME:
            if(m_bUnlock)
            {
                retValue =  FALSE;
            }
            break;
        default:
            retValue = TRUE;
            break;
        }
    }
    DebugPrintf(DLC_XU_KAI, "xka UIPage::OnKeyPress: begin  ret is %d", retValue);
    if (!retValue)
    {
        return FALSE;
    }

    return UIDialog::OnKeyPress(iKeyCode);;

}
