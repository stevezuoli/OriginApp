#ifndef __WIFICFG_H__
#define __WIFICFG_H__

#include "singleton.h"
#include <string>
#include "Wifi/WifiBase.h"
#include <vector>
using std::string;
using std::vector;

class WifiAccess
{
public:
    WifiAccess(const string sSSid, const string sPassword, const string sIdentity = "", const int iAccessTimes = 1, const int _securityType  = UNKNOWN_WIFI_SECURITY_TYPE, const BOOL bStaticIP = FALSE) : m_iSignalStrength(-1)
    {
        m_sSSID = sSSid;
        m_sIdentity = sIdentity;
        m_sPassword = sPassword;
        m_iAccessCount = iAccessTimes;
        m_iSecurityType = _securityType;
        m_bStaticIP = bStaticIP;
        m_sStaticIP = "";
        m_sIPMask = "";
        m_sGateway = "";
        m_sDNS = "";
        if(sPassword.empty())
        {
            m_iSecurityType = NONE;
        }
    }
    ~WifiAccess(){}
    
    const string GetSSID()
    {
        return m_sSSID;
    }

    const string GetIdentity()
    {
        return m_sIdentity;
    }

    const string GetPassword()
    {
        return m_sPassword;
    }
    void SetIdentity(const string sIdentity)
    {
        m_sIdentity = sIdentity;
    }

    void SetPassword(const string sPassword)
    {
        m_sPassword = sPassword;
    }

    const INT32  GetSecurityType()
    {
        return m_iSecurityType;
    }
    void SetSecurityType(INT32 iSecurityType)
    {
        m_iSecurityType = iSecurityType;
    }

    const INT32  GetLastAccess()
    {
        return m_iLastAccess;
    }
    
    void SetLastAccess(INT32 iLastAccess)
    {
        m_iLastAccess = iLastAccess;
    }

    const INT32  GetAccessCount()
    {
        return m_iAccessCount;
    }
    
    void SetAccessCount(INT32 iAccessCount)
    {
        m_iAccessCount = iAccessCount;
    }

    const INT32 GetSignalStrength()
    {
        return m_iSignalStrength;
    }

    void SetSignalStrength(INT32 iSignalStrength)
    {
        m_iSignalStrength = iSignalStrength;
    }

    const BOOL IsStaticIP()
    {
        return m_bStaticIP;
    }


    void SetStaticIP(BOOL bStaticIP)
    {
        m_bStaticIP = bStaticIP;
    }

    const string GetStaticIPAddress()
    {
        return m_sStaticIP;
    }

    void SetStaticIPAddress(string sStaticIP)
    {
        m_sStaticIP= sStaticIP;
    }
    

    const string GetMask()
    {
        return m_sIPMask;
    }

    void SetMask(string sMask)
    {
        m_sIPMask= sMask;
    }

    const string GetGateway()
    {
        return m_sGateway;
    }

    void SetGateway(string sGateway)
    {
        m_sGateway= sGateway;
    }
    
    const string GetDNS()
    {
        return m_sDNS;
    }

    void SetDNS(string sDNS)
    {
        m_sDNS = sDNS;
    }
    
    
private:
    
    string  m_sSSID;    
    string  m_sIdentity;
    string  m_sPassword;
    INT32   m_iLastAccess; 
    INT32   m_iAccessCount;
    INT32     m_iSecurityType;
    INT32     m_iSignalStrength;
    BOOL    m_bStaticIP;
    string  m_sStaticIP;
    string  m_sIPMask;
    string  m_sGateway;
    string  m_sDNS;
};

class DKWifiCfgManager
{

    SINGLETON_H(DKWifiCfgManager);
    
    public:
        void SaveWifiConfig(WifiAccess* wifi);
        void RemoveWifiConfig(WifiAccess* wifi);
        
        vector<WifiAccess*> GetBestWifis();

        WifiAccess* GetBestWifi();
        vector<WifiAccess*> GetBestUnavailableWifis();        
        WifiAccess* GetHistoryWifiBySSID(string ssid);
        string GetPassword(const string ssid);
        string GetIdentity(const string ssid);
        
        vector<WifiAccess*> GetHistoryWifis();
        vector<AccessPoint*> GetUserAPFromFile();
        
        void AddUserAPToFile(const vector<AccessPoint*>& apList);
        void AddUserAPToFile(AccessPoint* ap);
        
    private:
        
        DKWifiCfgManager(){}
        ~DKWifiCfgManager(){}
        void    WriteWifiConfigToFile(vector<WifiAccess*> wifiList);
};
#endif


