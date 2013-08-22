#ifndef __WIFIBASEIMPL_H__
#define __WIFIBASEIMPL_H__

#include <string>
#include "singleton.h"
#include <sys/socket.h>
#include "Wifi/WifiBase.h"
#include <semaphore.h>
#include <pthread.h>

#define WAIT(sem) {int value = -1, newvalue = -1; sem_getvalue(&sem, &value); sem_wait(&sem); sem_getvalue(&sem, &newvalue); if(value != newvalue+1)  DebugPrintf(DLC_LIUJT,"%s %s %d before WAIT, value=%d, after WAIT, value=%d",__FILE__,__FUNCTION__,__LINE__, value, newvalue);}
#define POST(sem) {int value = -1, newvalue = -1; sem_getvalue(&sem, &value); sem_post(&sem); sem_getvalue(&sem, &newvalue);if(value+1 != newvalue)  DebugPrintf(DLC_LIUJT,"%s %s %d before Post, value=%d, after POST, value =%d",__FILE__,__FUNCTION__,__LINE__, value, newvalue);}
#define GETVALUE(sem) {int value=-1; sem_getvalue(&sem, &value); DebugPrintf(DLC_LIUJT,"%s %s %d , sem value=%d",__FILE__,__FUNCTION__,__LINE__, value);}

// DHCP_NO_DNS found at 20120917,  for example,  Share AP via PC/Pad/NoteBook, 
// Realtek usb adapter RTL8191us 11n, 8187B 11g (OK) ;
// Intel 6200AGN failed(bug of Intel_My_WiFi 13.x drivers) 
//

typedef enum IPStatus
{
	DHCP_STAITC_IP,  // Static IP
    DHCP_OK,  // DHCP OK
	DHCP_CONNECT_LOST,  // Weak WiFi  Quality,  AP connection lost, while dhcp client request
    DHCP_NO_DNS,  // DHCP server cannot provide DNS 
	DHCP_NO_LEASE,  // No lease, failing
	DHCP_NO_GATEWAY, // cannot find gateway
	DHCP_OFFER_WRONG,  // dhcp server reply, but "Offer from server" is wrong
}eIPStatus;

struct wlantool_value {
        __uint32_t      cmd;
        __uint32_t      data;
};

class CWifiBaseImpl : public IWifiBase
{

SINGLETON_H(CWifiBaseImpl);
    
public:
    STDMETHOD(Refresh)();
    STDMETHOD(GetAPList)(AccessPoint*** pppAPList,int *piAPCount);
    STDMETHOD(JoinAP)(std::string strSSID);
    STDMETHOD(JoinAP)(std::string strSSID,std::string strPassword, std::string strIdentity, unsigned int securityTypes = WEP_OPEN | WEP_SHARE | WPA | WPA2 ,std::string strIP = "",std::string strMask = "",std::string strGate = "",std::string DNS = "");
    STDMETHOD(PowerOn)() ;
    STDMETHOD(PowerOff)() ;
    virtual bool IsPowerOn();
    virtual bool IsConnected(BOOL bNeedPing = TRUE);
	virtual AccessPoint *GetCurAP(BOOL bNeedPing = TRUE); 
	void UpdateGateway();
	void UpdateGateway(std::string GateWay);
    std::string FindGateWay();
	std::string FindIP();
	std::string FindMacAddr();
	std::string FindESSID();
    eIPStatus SetIP(std::string IP,std::string Mask,std::string Gate,std::string DNS);
	bool IsConnectedtoESSID(int securityType = NONE);
	int  GetConnectedQuality();
	bool IsConnectedtoLAN();
	bool IsConnectedtoWAN();	
    bool IsPingHostOK(std::string IP, int port, int mstimeout=100);
    bool IsPasswordError(int securityType = NONE);
    bool GetSecurityTypeFromSSID(std::string SSID, eSecurityType *SecurityType);
	
private:
    int  Async_connect(int soc, struct sockaddr_in &addr, int ms);
    void GetAPFromString(std::string& APListString,AccessPoint** pAP);
    void GetAPListFromString(std::string& APListString,AccessPoint*** rgAPs,int * APCount);
    std::string GetSystemCallOutput(std::string cmd);
    void RemoveAndSort(AccessPoint*** ppAPs,int *iAPCount);
    bool TryConnectAP(std::string SSID,std::string Password, eSecurityType SecurityType, int timeout, std::string Identity = "");	
    bool isWEPPassword(const std::string& Password);
	std::string GetIPFromString(std::string& str, std::string::size_type index=0);

    std::string     m_strAdapterName;
	std::string     m_strGateway;
    bool         m_fAutoDoing;
    AccessPoint m_CurAP;
    
 	pthread_mutex_t mutex_wifilist;

    CWifiBaseImpl();
    ~CWifiBaseImpl();
};

#endif

