////////////////////////////////////////////////////////////////////////
// UIBookListItem.h
// Contact: chenm
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UIWIFILISTITEM_H__
#define __UIWIFILISTITEM_H__

#include "GUI/UIListItemBase.h"
#include "Model/IWifiItemModel.h"
#include "GUI/UIImage.h"
#include "GUI/UIComplexButton.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UIProgressBar.h"
#include <string>
using std::string;
class UIWifiListItem : public UIListItemBase
{
public:
	enum ConnectStatus {
		NoConnect,
		Connecting,
		Connected,		
		DHCP,
		DHCPFail,
		DHCPLost,
		DHCPNoDNS,
		DHCPNoGW,
		DHCPOffer
	};

    UIWifiListItem();
    UIWifiListItem(UICompoundListBox* pParent, const DWORD rstrId);
    ~UIWifiListItem();

    virtual LPCSTR GetClassName() const
    {
        return "UIWifiListItem";
    }

    void ChangeWIFIStatus(const CHAR* _status);
    void ChangeWIFIStatus(ConnectStatus status);
    HRESULT Draw(DK_IMAGE drawingImg);
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
    virtual BOOL OnKeyPress(INT32 iKeyCode);
	ConnectStatus GetConnectStatus() { return m_iConnectStatus; }

private:
    string m_strSSID;
    INT32 m_iQualityLv;
    BOOL m_fIsProtected;
    BOOL m_fIsConn;
    string m_strStateMess;

public:
    void SetSSID(string& strSSID);
    string& GetSSID();

    void SetQualityLv(int value);
    int GetQualityLv();

    void SetIsProtected(BOOL fIsProtected);
    BOOL GetIsProtected();

    void SetIsConn(BOOL fIsConn);
    BOOL GetIsConn();

    void SetStateMess(string& strMess );
    string& GetStateMess();


private:
    HRESULT InitItem();

private:
    UITextSingleLine*       m_pSSIDLabel;
    UIImage*                m_pProtecedAPImg;
	UIImage*				m_pSelectedAPImg;
    UITextSingleLine*       m_pStatusLabel;

    BOOL                    m_bIsDisposed;
	ConnectStatus			m_iConnectStatus;
};

#endif  // __UIWIFILISTITEM_H__
