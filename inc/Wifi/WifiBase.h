#ifndef __WIFIBASE_H__
#define __WIFIBASE_H__

#include <string>
#include "dkObjBase.h"

enum eSecurityType
{
    RESET       = 0,
    WEP_OPEN    = 1,
    WEP_SHARE   = 2,
    WPA         = 4,
    WPA2        = 8,
	NONE        = 16,
    IEEE8021X   = 32
};

#define UNKNOWN_WIFI_SECURITY_TYPE (WEP_OPEN|WEP_SHARE|WPA|WPA2|NONE|IEEE8021X)

class AccessPoint
{
public:
    AccessPoint()
    {
        ClearContents();
    }
    
    AccessPoint(AccessPoint * ap)
    {
        SSID = ap->SSID;
        fIsConnected = ap->fIsConnected;
        Quality = ap->Quality;
        SecurityType = ap->SecurityType;
        fPublic = ap->fPublic;
    }
    
    AccessPoint(std::string _ssid, bool _bPublic)
    {
        ClearContents();
        SSID = _ssid;
        fPublic = _bPublic;
    }


    void ClearContents()
    {
        SSID.clear();
        fIsConnected = FALSE;
        Quality = 0;
        SecurityType = NONE;
        fPublic = FALSE;
    }

    std::string SSID;
    bool     fPublic;
    eSecurityType SecurityType;
    int      Quality;   // 1to100  for 1%-100% 
    bool    fIsConnected;
};


class IWifiBase
{
public:
    STDMETHOD(Refresh)() = 0;
    // Refresh×ö¸üÐÂ
    STDMETHOD(GetAPList)(AccessPoint*** pppAPList,int *iAPCount) = 0;

    STDMETHOD(JoinAP)(std::string strSSID) = 0;
    STDMETHOD(JoinAP)(std::string strSSID,std::string strPassword, std::string strIdentity, unsigned int securityTypes = WEP_OPEN | WEP_SHARE | WPA | WPA2 |IEEE8021X ,std::string strIP = "",std::string strMask = "",std::string strGate = "",std::string DNS = "") = 0;
    
    STDMETHOD(PowerOn)() = 0;
    STDMETHOD(PowerOff)() = 0;

    virtual bool IsPowerOn() = 0;
    virtual bool IsConnected(BOOL bNeedPing) = 0;
    virtual AccessPoint *GetCurAP(BOOL bNeedPing) = 0; 
    virtual ~IWifiBase(){};
};



#endif


