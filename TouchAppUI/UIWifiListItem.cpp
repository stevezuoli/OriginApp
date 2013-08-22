////////////////////////////////////////////////////////////////////////
// UIWifiListItem.h
// Contact: chenm
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "Framework/CDisplay.h"
#include "Framework/CHourglass.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UISizer.h"
#include "TouchAppUI/UIWifiListItem.h"
#include "DkSPtr.h"
#include "I18n/StringManager.h"
#include "Common/WindowsMetrics.h"

using namespace DkFormat;
using namespace WindowsMetrics;

UIWifiListItem::UIWifiListItem()
    : UIListItemBase(NULL, IDSTATIC)
    , m_pSSIDLabel(NULL)
    , m_pProtecedAPImg(NULL)    
    , m_pSelectedAPImg(NULL)    
    , m_pStatusLabel(NULL)
    , m_bIsDisposed(FALSE)
	, m_iConnectStatus(NoConnect)
{
    InitItem();
}

UIWifiListItem::UIWifiListItem(UICompoundListBox* pParent, const DWORD rstrId)
    : UIListItemBase(pParent, rstrId)
    , m_pSSIDLabel(NULL)
    , m_pProtecedAPImg(NULL)
    , m_pSelectedAPImg(NULL)    
    , m_pStatusLabel(NULL)
    , m_bIsDisposed(FALSE)
{
    if (pParent != NULL)
    {
        pParent->AppendChild(this);
    }
    if(S_OK != InitItem())
    {
        DebugPrintf(DLC_LIUJT, "UIWifiListItem::UIWifiListItem Something is wrong with InitItem() method: May be out of memory!!!");
    }
}

UIWifiListItem::~UIWifiListItem()
{
    if (m_pSSIDLabel != NULL)
    {
        delete m_pSSIDLabel;
        m_pSSIDLabel = NULL;
    }

    if (m_pStatusLabel != NULL)
    {
        delete m_pStatusLabel;
        m_pStatusLabel = NULL;
    }

    if (m_pProtecedAPImg != NULL)
    {
        delete m_pProtecedAPImg;
        m_pProtecedAPImg = NULL;
    }
    if (m_pSelectedAPImg != NULL)
    {
        delete m_pSelectedAPImg;
        m_pSelectedAPImg = NULL;
    }
}

BOOL UIWifiListItem::OnKeyPress(INT32 iKeyCode)
{
    return TRUE;
}

HRESULT UIWifiListItem::Draw(DK_IMAGE drawingImg)
{
    return UIListItemBase::Draw(drawingImg);
}

HRESULT UIWifiListItem::DrawBackGround(DK_IMAGE drawingImg)
{
    HRESULT hr(S_OK);
    CTpGraphics grf(drawingImg);
	RTN_HR_IF_FAILED(grf.EraserBackGround());

    SPtr<ITpImage> pimgSignalAP = ImageManager::GetImage(IMAGE_TOUCH_ICON_WIFI_LEVEL);

    const int width = pimgSignalAP->GetWidth();
    const int height = pimgSignalAP->GetHeight()/5;
    const int top = m_fIsProtected ? m_pProtecedAPImg->GetY() : (m_iHeight - height) >> 1;
    RTN_HR_IF_FAILED(grf.DrawImageBlend(pimgSignalAP.Get(),
        m_iLeft + m_iWidth - pimgSignalAP->GetWidth(),
        top,
        0,
        height * (m_iQualityLv - 1),
        width,
        height));

    RTN_HR_IF_FAILED(grf.DrawLine(m_pSSIDLabel->GetX(), m_iHeight - 1, m_iWidth - m_pSSIDLabel->GetX(), 1, DOTTED_STROKE));
    return hr;
}

HRESULT UIWifiListItem::InitItem()
{    
    m_strSSID.clear();
    m_iQualityLv = 0;
    m_fIsProtected = FALSE;
    m_fIsConn = FALSE;
    m_strStateMess.clear();
    SetLongClickable(true);
    DebugLog(DLC_GUI, "UIWifiListItem::InitItem===========\n");

	SPtr<ITpImage> pimgSelectedAP = ImageManager::GetImage(IMAGE_TOUCH_ICON_SELECT);
    if (NULL == m_pSSIDLabel)
    {
        m_pSSIDLabel = new UITextSingleLine(this, IDSTATIC);
        if(NULL == m_pSSIDLabel)
        {
            return E_OUTOFMEMORY;
        }
        m_pSSIDLabel->SetFontSize(GetWindowFontSize(UIWifiDialogListSSIDIndex));
    }


    if (NULL == m_pProtecedAPImg)
    {
        m_pProtecedAPImg = new UIImage(this, IDSTATIC);
        if(NULL == m_pProtecedAPImg)
        {
            return E_OUTOFMEMORY;
        }
		SPtr<ITpImage> pimgProtectedAP = ImageManager::GetImage(IMAGE_PROCTED_AP);
		m_pProtecedAPImg->SetImage(pimgProtectedAP);
    }

	if (NULL == m_pSelectedAPImg)
	{
        m_pSelectedAPImg = new UIImage(this, IDSTATIC);
        if(NULL == m_pSelectedAPImg)
        {
            return E_OUTOFMEMORY;
        }
        m_pSelectedAPImg->SetMinSize(dkSize(pimgSelectedAP->GetWidth(), pimgSelectedAP->GetHeight()));
	}

    if (!m_windowSizer)
    {
        UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
		SPtr<ITpImage> pimgSignalAP = ImageManager::GetImage(IMAGE_TOUCH_ICON_WIFI_LEVEL);
        if (mainSizer)
        {
            mainSizer->AddStretchSpacer();

            UISizer* ssidSizer = new UIBoxSizer(dkHORIZONTAL);
            if (ssidSizer)
            {
                ssidSizer->Add(m_pSelectedAPImg, UISizerFlags().Border(dkRIGHT, GetWindowMetrics(UIWifiListItemStatusImgRightMarginIndex)).Align(dkALIGN_CENTER_VERTICAL).ReserveSpaceEvenIfHidden());
                ssidSizer->Add(m_pSSIDLabel, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
                ssidSizer->Add(m_pProtecedAPImg, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Border(dkRIGHT, 14+pimgSignalAP->GetWidth()));
                mainSizer->Add(ssidSizer, UISizerFlags().Expand());
            }

            mainSizer->AddSpacer(GetWindowMetrics(UIWifiListItemSSIDBottomMarginIndex));
            SetSizer(mainSizer);
        }
    }
    DebugLog(DLC_GUI, "UIWifiListItem::InitItem exit ========\n");
    return S_OK;
}


void UIWifiListItem::ChangeWIFIStatus(const CHAR* _status)
{
    if(NULL == _status || NULL == m_pStatusLabel) 
    {
        return;
    }    
    DebugPrintf(DLC_LIUJT, "UIWifiListItem::ChangeWIFIStatus  _status = %s", _status);

    string str(_status);
    this ->SetStateMess(str);
    UpdateWindow();
}

void UIWifiListItem::ChangeWIFIStatus(ConnectStatus status)
{
	m_iConnectStatus = status;
	if (CHourglass::GetInstance()->IsRunning())
		CHourglass::GetInstance()->Stop();
	switch (status) {
	case NoConnect:
		{
            if (m_pSelectedAPImg)
            {
                m_pSelectedAPImg->SetVisible(false);
            }
			string str("");
			SetStateMess(str);
			SetIsConn(false);
		}
		break;
	case Connecting:
        {
            if (m_pSelectedAPImg)
            {
                m_pSelectedAPImg->SetVisible(false);
                if (!CHourglass::GetInstance()->IsRunning())
                    CHourglass::GetInstance()->Start(m_pSelectedAPImg->GetAbsoluteX(), m_pSelectedAPImg->GetAbsoluteY());
            }
			string str(StringManager::GetPCSTRById(CONNECT_WORKING));
			SetStateMess(str);
			SetIsConn(false);
		}
		break;
	case Connected:
        {
            if (m_pSelectedAPImg)
            {
                m_pSelectedAPImg->SetVisible(true);
            }
			string str(StringManager::GetPCSTRById(CONNECT_ALREADY));
			SetStateMess(str);
			SetIsConn(true);
		}
		break;
	case DHCP:
        {
            if (m_pSelectedAPImg)
            {
                m_pSelectedAPImg->SetVisible(false);
                if (!CHourglass::GetInstance()->IsRunning())
                    CHourglass::GetInstance()->Start(m_pSelectedAPImg->GetAbsoluteX(), m_pSelectedAPImg->GetAbsoluteY());
            }
			string str(StringManager::GetPCSTRById(CONNECT_ASSIGN_IP));
			SetStateMess(str);
			SetIsConn(false);
		}
		break;
	case DHCPFail:
        {
            if (m_pSelectedAPImg)
            {
                m_pSelectedAPImg->SetVisible(false);
            }
			string str(StringManager::GetPCSTRById(CONNECT_ASSIGN_IP_FAIL));
			SetStateMess(str);
			SetIsConn(false);
		}
		break;
	case DHCPLost:
        {
            if (m_pSelectedAPImg)
            {
                m_pSelectedAPImg->SetVisible(false);
            }
			string str(StringManager::GetPCSTRById(CONNECT_ASSIGN_IP_LOST));
			SetStateMess(str);
			SetIsConn(false);
		}
		break;
    default:
        break;
	}
	UpdateWindow();
}

void UIWifiListItem::SetSSID(string& strSSID)
{
    m_strSSID = strSSID;
    if (m_pSSIDLabel)
    {
        m_pSSIDLabel->SetText(CString(m_strSSID.c_str()));
    }
}

string& UIWifiListItem::GetSSID()
{
    return m_strSSID;
}

void UIWifiListItem::SetQualityLv(int value)
{
    m_iQualityLv = value;
}
int UIWifiListItem::GetQualityLv()
{
    return m_iQualityLv;
}
void UIWifiListItem::SetIsProtected(BOOL fIsProtected)
{
    m_fIsProtected = fIsProtected;
    if (m_pProtecedAPImg)
    {
        m_pProtecedAPImg->Show(m_fIsProtected);
    }
}
BOOL UIWifiListItem::GetIsProtected()
{
    return m_fIsProtected;
}

void UIWifiListItem::SetIsConn(BOOL fIsConn)
{
    m_fIsConn = fIsConn;
}
BOOL UIWifiListItem::GetIsConn()
{
    return m_fIsConn;
}

void UIWifiListItem::SetStateMess(string& strMess )
{
    m_strStateMess = strMess;
}
string& UIWifiListItem::GetStateMess()
{
    return m_strStateMess;
}

