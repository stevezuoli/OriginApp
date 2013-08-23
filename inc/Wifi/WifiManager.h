#ifndef __WIFIMANAGER__
#define __WIFIMANAGER__

#include "Wifi/WifiBase.h"
#include <semaphore.h>
#include <string>
#include <vector>
#include "dkObjBase.h"
#include "Wifi/WifiBaseImpl.h"
#include "GUI/EventArgs.h"
#include "GUI/EventSet.h"

typedef void (*FuncPointer)();
typedef void (*FPWifiConnection)(BOOL fIsSuccee,std::string strSSID,std::string strPassword,std::string strIdentity);

enum eWifiManagerWorking
{
    Nothing = 0,
    PowerOnIsWorking,
    PowerOffIsWorking,
    Refreshing,
    Connecting,
    
};
enum WifiMessageReceiver
{
    ALLRECEIVER,
    PAGECUSTOMIZED
};

enum WifiMessageTypes
{
    POWERONOFF_WIFI = 1,
    CONNECT_WIFI,
    AUTO_REFRESH,
    AUTO_REFRESH_SHOWTIP,
    UPDATE_WIFI_STATUS,
	MAN_REFRESH,	
    MAN_REFRESH_SHOWTIP,		
    CONNECT_WIFI_JOINAP,
    CONNECT_WIFI_DHCP,
    CONNECT_WIFI_DHCP_FAIL,
    CONNECT_WIFI_DHCP_LOST,
	CONNECT_WIFI_DHCP_OFFER,
	CONNECT_WIFI_NO_DNS,
	CONNECT_WIFI_NO_GW,
    OTHER
};

enum SpecialWifiMessage
{
    DEFAULT,//common wifi change
    WIFIAVAILABLE,
    WIFIUNAVAILABLE
};

class WifiEventArgs: public EventArgs
{
public:
    WifiEventArgs()
        : m_receiver(ALLRECEIVER)
        , m_msgType(OTHER)
        , m_msg(DEFAULT)
    {

    }


    WifiEventArgs(WifiMessageReceiver receiver, WifiMessageTypes msgType, SpecialWifiMessage msg)
        : m_receiver(receiver)
        , m_msgType(msgType)
        , m_msg(msg)
    {

    }

    virtual EventArgs* Clone() const
    {
        return new WifiEventArgs(*this);
    }
    WifiMessageReceiver m_receiver;
    WifiMessageTypes    m_msgType;
    SpecialWifiMessage  m_msg;
};

class WifiManager : public EventSet
{
SINGLETON_H(WifiManager);

public:
    static const char* WifiAvailableEvent;
    static const char* WifiUnavailableEvent;
    //static const char* WifiAutoJoinEvent;
    //static const char* WifiRefreshEvent;
    //static const char* WifiShowRefreshTipEvent;

    STDMETHOD(Refresh)();
    STDMETHOD(PowerOn)();
    STDMETHOD(PowerOff)();
    STDMETHOD(GetAPList)(AccessPoint*** pppAPList,int *iAPCount);

#if 0
    STDMETHOD(JoinAP)(std::string strSSID);
    STDMETHOD(JoinAP)(std::string strSSID,std::string strPassword,int securityTypes = WEP_OPEN | WEP_SHARE | WPA | WPA2 ,std::string strIP = "",std::string strMask = "",std::string strGate = "",std::string DNS = "");
#endif

    STDMETHOD(StartPowerOnAsync)(FuncPointer between, FuncPointer after);
    STDMETHOD(StartPowerOffAsync)(FuncPointer between, FuncPointer after) ;
    STDMETHOD(StartRefreshAsync)(FuncPointer, FuncPointer) ;
    STDMETHOD(StartJoinAPAsync)(FuncPointer between,  FPWifiConnection after,std::string strSSID);
    STDMETHOD(StartJoinAPAsync)(FuncPointer between,  FPWifiConnection after,std::string strSSID,std::string strPassword, std::string strIdentity = "", int securityTypes = WEP_OPEN | WEP_SHARE | WPA | WPA2 | IEEE8021X ,std::string strIP = "",std::string strMask = "",std::string strGate = "",std::string strDNS = "") ;
    STDMETHOD(GetAsyncAPList)(AccessPoint*** pppAPList,int *iAPCount) ;

    STDMETHOD(StartAutoJoin)(FuncPointer between, FPWifiConnection after);
    STDMETHOD(EndAutoJoin)();
    STDMETHOD(EndJoinAP)();
    
    virtual eWifiManagerWorking GetCurWorking();
    virtual bool IsPowerOn();
    virtual  bool IsConnected();
    void    SetConnectedStatus(bool bStatus); 
    int QualityToLevel(int iQuality);
    int GetActiveSignalLevel();

    void StartWifiDaemon();
    void StopWifiDaemon();
    static void GetWifiInformation(std::vector<std::string>& wifistrings);

    static void* UpdateStatusThread(void * pUseless);

	int GetWiFiStatus();	
	void SetWiFiStatus(int status);	

    bool FireWifiEvent(const char* eventName, WifiEventArgs& args);
    bool FireWifiAvailableEvent();
    bool FireWifiUnavailableEvent();
    //bool FireWifiAutoJoinEvent();
    //bool FireWifiRefreshEvent();
    //bool FireWifiShowRefreshTipEvent();
    bool IsManualJoinAP();

protected:
    IWifiBase *GetWifiBase();   
    WifiManager();
    virtual ~WifiManager();
    static AccessPoint** m_ppAPList;
    static int m_iAPCount;
    static eWifiManagerWorking m_eWorking;
    static pthread_t m_pThreadPowerOn;
    static pthread_t m_pThreadPowerOff;
    static pthread_t m_pThreadRefresh;
    static pthread_t m_pThreadJoinAP;
    static void* PowerOnAsync(void *);
    static void* PowerOffAsync(void *);
    static void* RefreshAsync(void*);
    static void* JoinApAsync(void *);

    //自动加入网络
    static int m_iAutoJoinSleepTime;
    static bool m_fAutoJoinRunning;
    static pthread_t m_pThreadAutoJoin;
    static void* AutoJoin(void*);

    //自动扫描网络
    static int m_iAutoRefreshTime;
    static bool m_fAutoRefreshing;
    static pthread_t m_pThreadAutoRefresh;

	static pthread_mutex_t mutex_wifi_scan;
	static pthread_mutex_t mutex_wifi_join;

	// 连接过程中，wifif状态
	static int m_wifi_status;

    static void* AutoRefresh(void*);
private:
    void UpdateStatus();
    void InitConnectedStatus();
    
    pthread_t m_pThreadDaemon;
    bool     m_bCurConnectedStatus;
    int       m_iSignalStrength;
};
#endif

