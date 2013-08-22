////////////////////////////////////////////////////////////////////////
// UIWifiDialog.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UIWifiDialog_H__
#define __UIWifiDialog_H__

#include "GUI/UIImage.h"
#include "GUI/UIButton.h"
#include "GUI/UIComplexButton.h"
#include "GUI/UITextSingleLine.h"
#include "CommonUI/UITitleBar.h"
#include "GUI/CTpGraphics.h"
#include "TouchAppUI/UIWifiListBox.h"
#include "Model/WifiItemModelImpl.h"
#include "GUI/UIProgressBar.h"
#include "Wifi/WifiBase.h"
#include "GUI/UIDialog.h"
#include <vector>
#include "pthread.h"

class UIWifiAddButton : public UITouchComplexButton
{
public:
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);

};//UIWifiAddButton

#define WIFI_CONNECTING 5
#define WIFI_CLOSING 6
#define WIFI_CLOSED  0

class UISizer;
class UIWifiDialog : public UIDialog, public IEventHook
{
    ////////////////////Constructor Section/////////////////////////
public:

    UIWifiDialog(UIContainer* pParent, bool forTransfer = false);

    static LPCSTR GetClassNameStatic()
    {
        return ("UIWifiDialog");
    }
    virtual LPCSTR GetClassName() const
    {
        return this->GetClassNameStatic();
    }
    virtual ~UIWifiDialog();

    void InitUI();
    virtual void MoveWindow(INT32 _iLeft, INT32 _iTop, INT32 _iWidth, INT32 _iHeight);
    
    HRESULT Draw(DK_IMAGE drawingImg);

    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);

    void OnNotify(UIEvent event);

    void OnLoad();  //When usb was plugged out, call this method to recover previouse page

	void WifiPowerSwitch();
	static string GetStrFixLen(string str, const UINT32 length);

    static string GetPassword(const string ssid);

    virtual void OnWifiMessage(SNativeMessage event);
    static void ShowRefreshTipCallBack();
    static void RefreshCallBack();
	static void ManShowRefreshTipCallBack();
    static void ManRefreshCallBack();
    void OnUnLoad();
	
	std::string GetCurEssid() const { return m_strCurEssid; }
	UIWifiListItem::ConnectStatus GetCurConnectStatus() const { return m_iCurConnectStatus; }

	bool GetWiFiPowerStatus() const { return m_bIsWifiPowerOn; }
protected:
    HRESULT InitMembers();
    void Dispose();
    void UpdateTitleBar();
    virtual void HandleEvent(const UIEvent & event) ;

private:
    void SetWifiItemModel(CWifiItemModelImpl *pModel, AccessPoint* pAp);
    void PowerOnWifi();
    void OnDispose(BOOL bIsDisposed);
    void UpdateNavigationButton();
	void UpdateWifiPowerStatus();
	void UpdateTotalWifiNumber();
	void UpdateWifiConnectStatus(UIWifiListItem::ConnectStatus status, string str);
    void UpdateWifiUI();
    void ManUpdateWifiUI();
    HRESULT UpdateAPList();
    BOOL TryAutoConnectUI();
    BOOL IsWifiAlreadyAdded(string ssid);
    void RemoveUserAddedWifi(string ssid);
    BOOL IsWifiInfoRecorded(string ssid);
    void RemoveWifiInfoRecord(string ssid);
    void ShowRefreshingTip(BOOL showTip);
    void ManShowRefreshingTip(BOOL showTip);
    void Finalize();
    void HandlePageUpDown(BOOL fPageDown);
    void ManualAddWifi();
    void DeleteWifi(int index);
    void RemoveAP(std::vector<AccessPoint*>& APList, std::string ssid);
    void RemoveAllAP(std::vector<AccessPoint*>& APList);
    void DeleteExtraAP(std::vector<AccessPoint*>& APList);
    static bool CompareAPQuality(AccessPoint* AP1, AccessPoint* AP2);

private:
    UISizer* m_pPowerOnSizer;
	UITouchBackButton m_btnBackToSetting;
	UITextSingleLine	 m_textTitle;
	UITextSingleLine	 m_textWifiStatusTitle;
	UITextSingleLine	 m_textWifiStatus;
	UITextSingleLine	 m_textWifiChoices;
	UITouchComplexButton	 m_btnWifiPowerSwitch;
    UITouchComplexButton     m_btnRefresh;
    UIWifiAddButton     m_btnManualAddAP;
    UIWifiListBox           m_lstWifi;
    UITextSingleLine        m_TotalWifi;
    UITextSingleLine        m_txtPageNo;
    UITitleBar         m_titleBar;
    UITextSingleLine    m_refreshingWifiText;
    INT32 m_iTotalPage;
    INT32 m_iCurPage;
    INT32 m_iAPNum;
    INT32 m_iAPNumPerPage;

    std::vector<AccessPoint *> m_rgpAPList;
    std::vector<AccessPoint *> m_userAddedAPs;

    CWifiItemModelImpl **m_ppCurWifiList;
	bool			m_bRefreshThreadOn;
    BOOL m_bIsDisposed;
	BOOL m_bIsWifiPowerOn;

	UIWifiListItem::ConnectStatus m_iCurConnectStatus;
	std::string					  m_strCurEssid;
	bool				m_bIsForTransfer;
	bool				m_bIsReentrancy;
    bool m_refreshing;
};

class ConnectionMessage
{
    BOOL WifiConnected;
    string WifiSSID;
    string WifiPassword;
    string WifiIdentity;
public:
    ConnectionMessage(BOOL connected, string ssid, string password, string strIdentity):
      WifiConnected(connected)
          , WifiSSID(ssid)
          , WifiPassword(password)
          , WifiIdentity(strIdentity)
      {
      }
      BOOL GetWifiConnected()
      {
          return WifiConnected;
      }
      string GetWifiSSID()
      {
          return WifiSSID;
      }
      string GetWifiPassword()
      {
          return WifiPassword;
      }
      string GetWifiIdentity()
      {
          return WifiIdentity;
      }
};

#endif //__UIWifiDialog_H__

