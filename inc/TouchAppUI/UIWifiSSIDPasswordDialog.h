///////////////////////////////////////////////////////////////////////
// UIWifiSSIDPasswordDialog.h
// Contact: liuhj
// Copyright (c) Duokan Corporation. All rights reserved.
//
///////////////////////////////////////////////////////////////////////

#ifndef __UIWIfISSIDPASSWORDDIALOG_H__
#define __UIWIfISSIDPASSWORDDIALOG_H__

#include "GUI/UIButton.h"
#include "GUI/UITouchComplexButton.h"
#include "GUI/UIModalDialog.h"
#include "GUI/UIImage.h"
#include "Utility/ImageManager.h"
#include "GUI/UITextBox.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UIRadioButtonGroup.h"
#include "Utility/ColorManager.h"
#include "DkSPtr.h"
#include "CommonUI/UIIPAddressTextBox.h"

class UIWifiSSIDPasswordDialog : public UIModalDlg
{
public:
    virtual LPCSTR GetClassName() const
    {
        return "UIWifiSSIDPasswordDialog";
    }

    UIWifiSSIDPasswordDialog(UIContainer* pParent);
    virtual ~UIWifiSSIDPasswordDialog();
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    virtual bool OnChildClick(UIWindow* child);
    bool isDynamicIP() const
    {
        return m_dynamicIP;
    }
    std::string GetSSID() const
    {
        return m_ssid;
    }
    std::string GetPasword() const
    {
        return m_password;
    }
    std::string GetIP() const 
    {
        return m_IP;
    }
    std::string GetIPMask() const
    {
        return m_ipMask;
    }
    std::string GetIPGate() const
    {
        return m_ipGate;
    }
    std::string GetIPDNS() const
    {
        return m_ipDNS;
    }

private:
    enum
    {
        SBI_DYNAMIIP = 0x1000,
        SBI_STATICIP,
    };
    
    void InitUI ();
    void UpdateIPAddress(bool dynamicIP);
    
    UITextSingleLine    m_txtSSIDHintLabel;
    UITextBox          m_tbSSID;
    UITextSingleLine    m_txtPasswordHintLabel;
    UITextBox          m_tbPassword;
    UITextSingleLine m_txtIPType;
    UIRadioButtonGroup m_IPType;

    UITextSingleLine m_inputStaticIPTitle;
    UITextSingleLine m_inputMaskTitle;
    UITextSingleLine m_inputGateTitle;
    UITextSingleLine m_inputDNSTitle;
    UIIPAddressTextBox m_inputStaticIP;
    UIIPAddressTextBox m_inputMask;
    UIIPAddressTextBox m_inputGate;
    UIIPAddressTextBox m_inputDNS;

    UITouchComplexButton    m_btnOK;
    UITouchComplexButton    m_btnCancel;
    
    bool m_dynamicIP;

    std::string m_ssid;
    std::string m_password;
    std::string m_IP;
    std::string m_ipMask;
    std::string m_ipGate;
    std::string m_ipDNS;
};

#endif //__UIWIfISSIDPASSWORDDIALOG_H__

