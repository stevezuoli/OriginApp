///////////////////////////////////////////////////////////////////////
// UIWifiSSIDPasswordDialog.cpp
// Contact: liuhj
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#include "Framework/CDisplay.h"
#include "TouchAppUI/UIWifiSSIDPasswordDialog.h"
#include "CommonUI/UIIMEManager.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UIMessageBox.h"
#include "I18n/StringManager.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"

using namespace WindowsMetrics;

static const char* DEFAUTL_IP = "192.168.1.2";
static const char* DEFAUTL_MASK = "255.255.255.0";
static const char* DEFAUTL_GATE = "192.168.1.1";

UIWifiSSIDPasswordDialog::UIWifiSSIDPasswordDialog(
    UIContainer* pParent)
    : UIModalDlg(pParent, StringManager::GetPCSTRById(ADD_WIFI_MANUALLY))
    , m_dynamicIP(false)
{

    InitUI();
}

UIWifiSSIDPasswordDialog::~UIWifiSSIDPasswordDialog ()
{
    Dispose ();
}

void UIWifiSSIDPasswordDialog::InitUI ()
{
    const int fontsize16 = GetWindowFontSize(FontSize16Index);
    const int fontsize18 = GetWindowFontSize(FontSize18Index);
    m_txtSSIDHintLabel.SetText(StringManager::GetStringById(NAME_OF_NETWORK));
    m_txtSSIDHintLabel.SetFontSize(fontsize18); 
    m_txtSSIDHintLabel.SetAlign(ALIGN_RIGHT);
    m_tbSSID.SetFontSize(fontsize18);
    m_tbSSID.SetTextBoxStyle(ENGLISH_NORMAL);

    m_txtPasswordHintLabel.SetText(StringManager::GetStringById(PASSWORD_WITH_COLON));
    m_txtPasswordHintLabel.SetFontSize(fontsize18); 
    m_txtPasswordHintLabel.SetAlign(ALIGN_RIGHT);
    m_tbPassword.SetFontSize(fontsize18);

    m_txtIPType.SetText(StringManager::GetStringById(WIFIIPTYPE));
    m_txtIPType.SetFontSize(fontsize18);
    m_txtIPType.SetForeColor(ColorManager::knBlack);
    m_txtIPType.SetAlign(ALIGN_RIGHT);

    m_IPType.AddButton(SBI_DYNAMIIP, StringManager::GetPCSTRById(WIFIDYNAMICIP), fontsize16, true);
    m_IPType.AddButton(SBI_STATICIP, StringManager::GetPCSTRById(WIFISTATICIP), fontsize16);
    UpdateIPAddress(true);

    m_inputStaticIPTitle.SetText(StringManager::GetPCSTRById(WIFIIPADDRESS));
    m_inputStaticIPTitle.SetFontSize(fontsize18);
    m_inputStaticIPTitle.SetAlign(ALIGN_RIGHT);
    m_inputMaskTitle.SetText(StringManager::GetPCSTRById(WIFIIPMASK));
    m_inputMaskTitle.SetFontSize(fontsize18);
    m_inputMaskTitle.SetAlign(ALIGN_RIGHT);
    m_inputGateTitle.SetText(StringManager::GetPCSTRById(WIFIGATEWAY));
    m_inputGateTitle.SetFontSize(fontsize18);
    m_inputGateTitle.SetAlign(ALIGN_RIGHT);
    m_inputDNSTitle.SetText(StringManager::GetPCSTRById(WIFIDNSIP));
    m_inputDNSTitle.SetFontSize(fontsize18);
    m_inputDNSTitle.SetAlign(ALIGN_RIGHT);

    m_inputStaticIP.SetDefaultIPAddress(DEFAUTL_IP);
    m_inputMask.SetDefaultIPAddress(DEFAUTL_MASK);
    m_inputGate.SetDefaultIPAddress(DEFAUTL_GATE);

    m_btnCancel.Initialize(IDCANCEL,  StringManager::GetStringById(ACTION_CANCEL), fontsize18, SPtr<ITpImage>());
    m_btnOK.Initialize(IDOK,  StringManager::GetStringById(WIFI_CONNECT), fontsize18, SPtr<ITpImage>());

	AppendChild(&m_txtSSIDHintLabel);
	AppendChild(&m_tbSSID);
	AppendChild(&m_txtPasswordHintLabel);
    AppendChild(&m_tbPassword);
    AppendChild(&m_txtIPType);
    AppendChild(&m_IPType);
    AppendChild(&m_inputStaticIPTitle);
    AppendChild(&m_inputStaticIP);
    AppendChild(&m_inputMaskTitle);
    AppendChild(&m_inputMask);
    AppendChild(&m_inputGateTitle);
    AppendChild(&m_inputGate);
    AppendChild(&m_inputDNSTitle);
    AppendChild(&m_inputDNS);
    AppendChild(&m_btnOK);
    AppendChild(&m_btnCancel);

    const int lineMargin = GetWindowMetrics(UIElementMidSpacingIndex);
    UISizer* pMainSizer = new UIGridSizer(8, 1, lineMargin, lineMargin);
    UISizer* pSSIDSizer = new UIBoxSizer(dkHORIZONTAL);
    UISizer* pPWDSizer = new UIBoxSizer(dkHORIZONTAL);
    UISizer* pIPTypeSizer = new UIBoxSizer(dkHORIZONTAL);
    UISizer* pIPSizer = new UIBoxSizer(dkHORIZONTAL);
    UISizer* pMaskSizer = new UIBoxSizer(dkHORIZONTAL);
    UISizer* pGateSizer = new UIBoxSizer(dkHORIZONTAL);
    UISizer* pDNSSizer = new UIBoxSizer(dkHORIZONTAL);
    UISizer* bottomSizer = new UIBoxSizer(dkHORIZONTAL);

    if (m_windowSizer && pMainSizer && pSSIDSizer && pPWDSizer && pIPTypeSizer
         && pIPSizer && pMaskSizer && pGateSizer && pDNSSizer && bottomSizer)
    {
        const int textWidth = GetWindowMetrics(UIWifiSSIDPasswordDialogTextWidthIndex);
        const int margin = GetWindowMetrics(UIModalDialogHorizonMarginIndex);
        const int btnBtnHeight = GetWindowMetrics(UINormalBtnHeightIndex);
        const int width = GetWindowMetrics(UIWifiSSIDPasswordDialogWidthIndex);
        m_txtSSIDHintLabel.SetMinWidth(textWidth);
        m_txtPasswordHintLabel.SetMinWidth(textWidth);
        m_txtIPType.SetMinWidth(textWidth);
        m_inputStaticIPTitle.SetMinWidth(textWidth);
        m_inputMaskTitle.SetMinWidth(textWidth);
        m_inputGateTitle.SetMinWidth(textWidth);
        m_inputDNSTitle.SetMinWidth(textWidth);
        m_btnCancel.SetMinHeight(btnBtnHeight);
        m_btnOK.SetMinHeight(btnBtnHeight);
        
        pSSIDSizer->Add(&m_txtSSIDHintLabel, UISizerFlags().Border(dkRIGHT,margin).Align(dkALIGN_CENTER_VERTICAL));
        pSSIDSizer->Add(&m_tbSSID, UISizerFlags(1).Expand());

        pPWDSizer->Add(&m_txtPasswordHintLabel, UISizerFlags().Border(dkRIGHT, margin).Align(dkALIGN_CENTER_VERTICAL));
        pPWDSizer->Add(&m_tbPassword, UISizerFlags(1).Expand());

        pIPTypeSizer->Add(&m_txtIPType, UISizerFlags().Border(dkRIGHT, margin).Align(dkALIGN_CENTER_VERTICAL));
        pIPTypeSizer->Add(&m_IPType, UISizerFlags(1).Expand());

        pIPSizer->Add(&m_inputStaticIPTitle, UISizerFlags().Border(dkRIGHT, margin).Align(dkALIGN_CENTER_VERTICAL));
        pIPSizer->Add(&m_inputStaticIP, UISizerFlags(1).Expand());

        pMaskSizer->Add(&m_inputMaskTitle, UISizerFlags().Border(dkRIGHT, margin).Align(dkALIGN_CENTER_VERTICAL));
        pMaskSizer->Add(&m_inputMask, UISizerFlags(1).Expand());

        pGateSizer->Add(&m_inputGateTitle, UISizerFlags().Border(dkRIGHT, margin).Align(dkALIGN_CENTER_VERTICAL));
        pGateSizer->Add(&m_inputGate, UISizerFlags(1).Expand());

        pDNSSizer->Add(&m_inputDNSTitle, UISizerFlags().Border(dkRIGHT, margin).Align(dkALIGN_CENTER_VERTICAL));
        pDNSSizer->Add(&m_inputDNS, UISizerFlags(1).Expand());

        bottomSizer->Add(&m_btnCancel, 1);
        bottomSizer->AddSpacer(margin);
        bottomSizer->Add(&m_btnOK, 1);

        pMainSizer->Add(pSSIDSizer, UISizerFlags().Expand());
        pMainSizer->Add(pPWDSizer, UISizerFlags().Expand());
        pMainSizer->Add(pIPTypeSizer, UISizerFlags().Expand());
        pMainSizer->Add(pIPSizer, UISizerFlags().Expand());
        pMainSizer->Add(pMaskSizer, UISizerFlags().Expand());
        pMainSizer->Add(pGateSizer, UISizerFlags().Expand());
        pMainSizer->Add(pDNSSizer, UISizerFlags().Expand());
        pMainSizer->Add(bottomSizer, UISizerFlags().Expand());
        m_windowSizer->Add(pMainSizer, UISizerFlags().Border(dkALL, margin).Expand());

        int height = GetWindowMetrics(UIWifiSSIDPasswordDialogHeightIndex);
        MakeCenterAboveIME(width, height);
        Layout();
    }
    else
    {
        SafeDeletePointer(pMainSizer);
        SafeDeletePointer(pSSIDSizer);
        SafeDeletePointer(pPWDSizer);
        SafeDeletePointer(pIPTypeSizer);
        SafeDeletePointer(pIPSizer);
        SafeDeletePointer(pMaskSizer);
        SafeDeletePointer(pGateSizer);
        SafeDeletePointer(pDNSSizer);
        SafeDeletePointer(bottomSizer);
    }

    UIIME* pIME = UIIMEManager::GetIME(IUIIME_ENGLISH_LOWER, &m_tbSSID);
    if (pIME)
    {
        m_tbSSID.SetFocus(true);
        pIME->SetShowDelay(true);
    }
}

bool UIWifiSSIDPasswordDialog::OnChildClick(UIWindow* child)
{
    const DWORD& cmdId = child->GetId();
    switch (cmdId)
    {
    case SBI_DYNAMIIP:
    case SBI_STATICIP:
        {
            UpdateIPAddress(cmdId == SBI_DYNAMIIP);
        }
        break; 
    default:
        return true;
    }
    return true;
}

void UIWifiSSIDPasswordDialog::UpdateIPAddress(bool dynamicIP)
{
    if (m_dynamicIP != dynamicIP)
    {
        int selected = 1;
        int color = ColorManager::knBlack;
        if (dynamicIP)
        {
            selected = 0;
            color = ColorManager::GetColor(COLOR_KERNEL_DISABLECOLOR);
        }
        m_IPType.SelectChild(selected);
        const bool enable = !dynamicIP;
        m_inputStaticIP.SetEnable(enable);
        m_inputMask.SetEnable(enable);
        m_inputGate.SetEnable(enable);
        m_inputDNS.SetEnable(enable);
        m_inputStaticIP.SetEnable(!dynamicIP);
        m_inputMask.SetEnable(!dynamicIP);
        m_inputGate.SetEnable(!dynamicIP);
        m_inputDNS.SetEnable(!dynamicIP);
        m_inputStaticIPTitle.SetForeColor(color);
        m_inputMaskTitle.SetForeColor(color);
        m_inputGateTitle.SetForeColor(color);
        m_inputDNSTitle.SetForeColor(color);
        m_dynamicIP = dynamicIP;
        UpdateWindow();
    }
}

void UIWifiSSIDPasswordDialog::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    DebugPrintf(DLC_MESSAGE, "UIWifiSSIDPasswordDialog::OnCommand(dwCmdId=%d, pSender=%X, dwParam=%d)", dwCmdId, pSender, dwParam); 
    switch (dwCmdId)
    {
    case IDOK:
        {
            m_ssid = m_tbSSID .GetTextUTF8();
            m_password = m_tbPassword .GetTextUTF8();
            if (m_ssid.empty())
            {
                m_tbSSID.SetTipUTF8(StringManager::GetPCSTRById(NETWORK_NAME_CANNOT_BE_EMPTY),0);
                m_tbSSID.SetFocus(TRUE);
                m_tbSSID.UpdateWindow();
                break;
            }

            if (!m_dynamicIP)
            {
                m_IP = m_inputStaticIP.GetIP();
                m_ipMask = m_inputMask.GetIP();
                m_ipGate = m_inputGate.GetIP();
                m_ipDNS = m_inputDNS.GetIP();
                if (m_IP.empty() || m_ipMask.empty() || m_ipGate.empty() || m_ipDNS.empty())
                {
                    UIMessageBox messagebox(CDisplay::GetDisplay()->GetCurrentPage(), StringManager::GetStringById(WIFI_EMPTYIP), MB_OK);
                    messagebox.DoModal();
                    break;
                }
            }
            EndDialog(1);
            break;
        }
        break;
    case IDCANCEL:
        EndDialog(0);
        break;
    default:
        EndDialog(0);
        break;
    }
}

