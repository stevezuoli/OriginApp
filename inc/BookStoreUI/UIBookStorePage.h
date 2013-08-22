#ifndef _DKREADER_TOUCHAPPUI_UIBOOKSTOREPAGE_H_
#define _DKREADER_TOUCHAPPUI_UIBOOKSTOREPAGE_H_

#include "GUI/UIPage.h"
#include "GUI/UIMenu.h"
#include "CommonUI/UITitleBar.h"

class UIBookStorePage : public UIPage
{
public:
    static const int MAINMENU_WIDTH = 520;
    static const int MAINMENU_HEIGHT = 48;
    static const int MAINMENU_TOPMARGIN = 35;
    static const int MAINMENU_LEFTMARGIN = 50;

    enum FETCH_STATUS
    {
        FETCHING = 0,
        FETCH_SUCCESS,
        FETCH_FAIL,
        NOFETCH
    };

    static LPCSTR GetClassNameStatic()
    {
        return "UIBookStorePage";
    }
	virtual LPCSTR GetClassName() const
	{
		return GetClassNameStatic();
	}

	UIBookStorePage();
	virtual ~UIBookStorePage();
    
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    virtual BOOL OnKeyPress(INT32 iKeyCode);
    virtual void OnWifiMessage(SNativeMessage event);

protected:
    virtual dk::bookstore::FetchDataResult RequestPage() = 0;

    void SetWifiMenuItem();
    static void WifiStatusCallBack();
    static void WifiPowerOnCallBack();
    static void ConnectWifiCallBack(BOOL connected, std::string ssid, std::string password, string strIdentity);
    void UpdateTitleBar();

    UITitleBar m_title;
    UIMenu m_mnuMain;
    FETCH_STATUS m_fetchStatus;
};

#endif//_DKREADER_TOUCHAPPUI_UIBOOKSTOREPAGE_H_

