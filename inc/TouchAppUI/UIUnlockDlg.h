#ifndef __UIUNLOCKDLG_H_
#define __UIUNLOCKDLG_H_

#include "TouchAppUI/UIWifiListBox.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UIButton.h"
#include "GUI/UIComplexButton.h"
#include "GUI/UIDialog.h"
#include "GUI/UIImage.h"
#include "GUI/UIProgressBar.h"
#include "GUI/UITextSingleLine.h"
#include "Model/WifiItemModelImpl.h"
#include "singleton.h"
#include <vector>
#include "Wifi/WifiBase.h"
#include <vector>
#include <string>

#define PASSWORDITEMCOUNT 4
class UIUnlockDlg : public UIDialog
{
    SINGLETON_H(UIUnlockDlg)
public:
    UIUnlockDlg();
    UIUnlockDlg(UIContainer* pParent);
    UIUnlockDlg(UIContainer* pParent,std::string _password);

    virtual LPCSTR GetClassName() const
    {
        return ("UIUnlockDlg");
    }
    
    void SetPassword(std::string _passwd) {        
        int iLen = PASSWORDITEMCOUNT;
        if(_passwd.length() < PASSWORDITEMCOUNT) {
            iLen = _passwd.length();
        }
        memcpy(m_szPasswd,_passwd.c_str(),iLen);
        m_szPasswd[iLen] = 0;
    };
    std::string GetPassword() {
        return m_szPasswd;
    }
    void SetRightPassword(std::string _RightPasswd) {
        m_strRightPasswd = _RightPasswd;
    }
    void SetUnlockMode() {
        m_bUnlock = 1;
        memset(m_szPasswd,0,PASSWORDITEMCOUNT + 1);
        m_iSelected = 0;
    }
    HRESULT Draw(DK_IMAGE drawingImg);
    HRESULT DrawBackGround(DK_IMAGE drawingImg);
    ~UIUnlockDlg(){};
    void UIInit();
    BOOL OnKeyPress(INT32 iKeyCode);

private:
    void UpdatePasswdItem();
    HRESULT DrawCursor(DK_IMAGE drawingImg);
    char m_szPasswd[PASSWORDITEMCOUNT + 1];
    std::string m_strRightPasswd;
    int m_iSelected;
    UITextSingleLine m_vctPasswdItem[PASSWORDITEMCOUNT];
    UITextSingleLine m_txtInputPasswd;
    bool m_bUnlock;
};


class UIScreenSaverUnlockPage : public UIPage
{
public:
    UIScreenSaverUnlockPage():UIPage(){};

    static LPCSTR GetClassNameStatic()
    {
        return ("UI91SearchPage");
    }
    virtual LPCSTR GetClassName() const
    {
            return this->GetClassNameStatic();
    }

    virtual ~UIScreenSaverUnlockPage(){};
    void OnResize(INT32 iWidth, INT32 iHeight)
    {
    }
};
#endif //__UIUNLOCKDLG_H_
