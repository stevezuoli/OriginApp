
#include <sys/types.h> 
#include <unistd.h> 
#include <string>
#include <vector>
#include "Wifi/WifiManager.h"
#include "interface.h"
#include "Wifi/WifiCfg.h"
#include "IniFile.h"
#include "Utility/EncodeUtil.h"

using namespace std;
using dk::utility::EncodeUtil;

#define WIFI_CFG_FILE "/mnt/us/system/WifiCfg.ini"
#define WIFI_ADDED_FILE "/mnt/us/system/UserAddedWifi.ini"

#define WIFILASTACCESS "LastAccess"
#define WIFIIDENTITY "Identity"
#define WIFIPASSWORDHEX "PasswordHex"
#define WIFIPASSWORD "Password"
#define WIFISECURITYTYPE "SecurityType"
#define WIFIACCESSACOUNT "AccessCount"
#define WIFIPUBLIC "WifiPublic"
#define WIFISTATICIP "StaticIP"

#define WIFISTATICIPADDRESS "StaticIPAddress"
#define WIFISTATICIPMASK "StaticIPMask"
#define WIFISTATICIPGATEWAY "StaticIPGateway"
#define WIFISTATICIPDNS "StaticIPDNS"

SINGLETON_CPP(DKWifiCfgManager)


string DKWifiCfgManager::GetIdentity(const string ssid)
{
    string Identity;
    if(ssid.empty())
    {
        return Identity;
    }
    vector<WifiAccess*> reservedWifiList = GetHistoryWifis();
    for(vector<WifiAccess*>::size_type i = 0; i< reservedWifiList.size(); i++)
    {
        WifiAccess * wifi = reservedWifiList[i];
        if (!wifi || wifi->GetSSID().empty())
        {
            SafeDeletePointerEx(wifi);
            continue;
        }

        if(wifi->GetSSID() == ssid)
        {
            Identity = wifi->GetIdentity();
        }

        SafeDeletePointerEx(wifi);
    }

    return Identity;
}

string DKWifiCfgManager::GetPassword(const string ssid)
{
    string password;
    if(ssid.empty())
    {
        return password;
    }
    vector<WifiAccess*> reservedWifiList = GetHistoryWifis();
    for(vector<WifiAccess*>::size_type i = 0; i< reservedWifiList.size(); i++)
    {
        WifiAccess * wifi = reservedWifiList[i];
        if (!wifi || wifi->GetSSID().empty())
        {
            SafeDeletePointerEx(wifi);
            continue;
        }

        if(wifi->GetSSID() == ssid)
        {
            password = wifi->GetPassword();
        }

        SafeDeletePointerEx(wifi);
    }

    return password;
}
/*
*The caller should be responsible for deleting the memory referenced by returned pointer.
*/
WifiAccess* DKWifiCfgManager::GetHistoryWifiBySSID(string ssid)
{
    if(ssid.empty())
    {
        return NULL;
    }
    WifiAccess * target = NULL;
    vector<WifiAccess*> reservedWifiList = GetHistoryWifis();
    UINT uLen = reservedWifiList.size();
    for(UINT i = 0; i< uLen; i++)
    {
        WifiAccess * wifi = reservedWifiList[i];
        if (!wifi || wifi->GetSSID().empty())
        {
            SafeDeletePointerEx(wifi);
            continue;
        }

        if(wifi->GetSSID() == ssid)
        {
            target = wifi;
        }
        else
        {
            SafeDeletePointerEx(wifi);
        }
    }

    return target;
    
}

/*
*The caller should be responsible for deleting the memory referenced by each element in the returned vector.
*/
vector<WifiAccess *> DKWifiCfgManager::GetHistoryWifis()
{
    CIniFile inifile;
    vector<WifiAccess *> wifiLists;
    inifile.SetPath(WIFI_CFG_FILE);

    IniKeyMap::iterator keymapiter;

    if (inifile.ReadFile())
    {
        for(keymapiter=inifile.keys.begin();keymapiter!=inifile.keys.end();++keymapiter)
            {
                string sWifiSSID = keymapiter->first;
                if(sWifiSSID.empty())
                {
                    continue;
                }

                string sIdentity = inifile.GetStringValue(sWifiSSID, WIFIIDENTITY,"");
                string sPassword = inifile.GetStringValue(sWifiSSID, WIFIPASSWORD,"");
                
                int iSecurityType =inifile.GetIntValue(sWifiSSID, WIFISECURITYTYPE,UNKNOWN_WIFI_SECURITY_TYPE);
                int iAccessCount =inifile.GetIntValue(sWifiSSID, WIFIACCESSACOUNT,1);
                BOOL bStaticIP = inifile.GetBoolValue(sWifiSSID, WIFISTATICIP, FALSE);
                int iLastAcess = inifile.GetIntValue(sWifiSSID, WIFILASTACCESS, 0);
				
				string sPasswordHex = inifile.GetStringValue(sWifiSSID, WIFIPASSWORDHEX,"");

				if(sPasswordHex.size() > 2 * sPassword.size())
				{
					std::vector<unsigned char> buf;

					if(EncodeUtil::HexToBin(sPasswordHex.c_str(), &buf))
					{
						string sPasswordOK(buf.begin(), buf.end());
						sPassword = sPasswordOK;
					}
				}

                WifiAccess * pWA = new WifiAccess(sWifiSSID, sPassword, sIdentity, iAccessCount, iSecurityType);
                if(!pWA) 
                {
                    continue;
                }
                
                if(bStaticIP)
                {
                    string sIP = inifile.GetStringValue(sWifiSSID, WIFISTATICIPADDRESS, "");
                    string sIPMask = inifile.GetStringValue(sWifiSSID, WIFISTATICIPMASK, "");
                    string sIPGateway = inifile.GetStringValue(sWifiSSID, WIFISTATICIPGATEWAY, "");
                    string sIPDNS = inifile.GetStringValue(sWifiSSID, WIFISTATICIPDNS, "");
                    pWA->SetStaticIP(TRUE);
                    pWA->SetStaticIPAddress(sIP);
                    pWA->SetMask(sIPMask);
                    pWA->SetGateway(sIPGateway);
                    pWA->SetDNS(sIPDNS);
                }
                
                pWA->SetLastAccess(iLastAcess);

                wifiLists.push_back(pWA);
          }
    }
    DebugPrintf(DLC_LIUJT, "DKWifiCfgManager:: history wifilist size()=%d", wifiLists.size());
    return wifiLists;

}

vector<WifiAccess*> DKWifiCfgManager::GetBestUnavailableWifis()
{
    vector<WifiAccess*> historyWifis = GetHistoryWifis();
    UINT uLen = historyWifis.size();
    if(uLen <= 3)
    {
        return historyWifis;
    }
    
    vector<WifiAccess*> bestwifis;    
    for(UINT i = 0; i < uLen-1; i++)//按照访问次数对记录过WiFi进行排序
    {
        WifiAccess* pWA = historyWifis[i];
        if(!pWA || pWA->GetSSID().empty())
        {
            continue;
        }
        UINT uIndex = i;
        INT  iCount = pWA->GetAccessCount();
        for(UINT j = i+1; j < uLen; j++)
        {
            WifiAccess* pTemp = historyWifis[j];
            if(!pTemp || pTemp->GetSSID().empty())
            {
                continue;
            }
            if(pTemp->GetAccessCount() > iCount)
            {
                iCount = pTemp->GetAccessCount();
                uIndex = j;
            }
        }
        if(uIndex != i)
        {
            WifiAccess* pExchange = historyWifis[uIndex];
            historyWifis[uIndex] = historyWifis[i];
            historyWifis[i] = pExchange;
        }
        
    }
    int iReturnCount = 0;
    for(UINT i = 0; i < uLen; i++) // 取访问次数最多的1个 WiFi作为候选，同时删除其余的WiFi
    {
        WifiAccess* pTemp = historyWifis[i];
        if(!pTemp || pTemp->GetSSID().empty())
        {
            SafeDeletePointerEx(historyWifis[i]);
            continue;
        }
        if(iReturnCount >= 1)
        {
            SafeDeletePointerEx(historyWifis[i]);
        }
        else
        {
            bestwifis.push_back(pTemp);
            iReturnCount++;
        }
    }
    return bestwifis;
}

/*
*The caller should be responsible for deleting the memory referenced by each element in the returned vector.
*/
vector<WifiAccess*> DKWifiCfgManager::GetBestWifis()
{
    vector<WifiAccess*> bestwifis;

    WifiManager* wifiBase = WifiManager::GetInstance();
    AccessPoint** _AP_list = NULL;
    int _AP_num = 0;

    wifiBase->GetAPList(&_AP_list , &_AP_num);      //Get the wifi list after refreshing

    if (!_AP_num || !_AP_list)
    {
        SafeDeleteArrayEx(_AP_list);
        
        return bestwifis;
    }

    vector<WifiAccess*> historyWifis = GetHistoryWifis();
    UINT uLen = historyWifis.size();

    if(uLen) //If user has connected wifis before
    {
        for(int i = 0; i< _AP_num; i++)
        {
            AccessPoint *AP = _AP_list[i];
            if(!AP || AP->SSID.empty())
            {
                continue;
            }
            for(UINT jj = 0; jj < uLen; jj++)
            {
                WifiAccess *CurWA = historyWifis[jj];
                
                if(!CurWA || CurWA->GetSSID().empty())
                {
                    continue;
                }
                string tmp = CurWA->GetSSID();
                if(AP->SSID == tmp)
                {
                    CurWA->SetSignalStrength(AP->Quality);

                    if(CurWA->GetLastAccess() == 1)  // 上次连接的AP，优先排序
                         CurWA->SetSignalStrength(100);
                }

            }
        }

        for(UINT i = 0; i< uLen-1; i++)
        {
            WifiAccess * pWA = historyWifis[i];
            if(!pWA || pWA->GetSSID().empty())
            {
                continue;
            }
            INT32 iSignalStrength = pWA->GetSignalStrength();
            UINT uIndex = i;
            for(UINT j = i+1; j< uLen; j++)
            {
                WifiAccess * pHistory = historyWifis[j];
                if(!pHistory || pHistory->GetSSID().empty())
                {
                    continue;
                }
                
                if(pHistory->GetSignalStrength() > iSignalStrength)
                {
                    iSignalStrength = pHistory->GetSignalStrength();
                    uIndex = j;
                }
            }
            if(uIndex == i)
            {
                continue;
            }

            //exchange i and index's WifiAccess
            WifiAccess * pTmp = historyWifis[i];
            
            historyWifis[i] = historyWifis[uIndex];
            
            historyWifis[uIndex] = pTmp;

        }

        for(INT32 i = uLen -1 ; i >= 0; i--)     //Remove the invalid wifi networks via signal
        {
            WifiAccess * pWA = historyWifis[i];
            if(!pWA || pWA->GetSSID().empty())
            {
                SafeDeletePointerEx(pWA);
                continue;
            }
            INT32 signal = pWA->GetSignalStrength();
            
            if(signal > 0 && signal <= 100)
            {
                break;
            }

            SafeDeletePointerEx(pWA);
            historyWifis.pop_back();//popup it to update the vector's size()!
        }

    }

    SafeDeleteArrayEx(_AP_list);

    return historyWifis;
}

/*
* The caller should be responsible for deleting the returned pointer!
*/
WifiAccess* DKWifiCfgManager::GetBestWifi()
{
    vector<WifiAccess*> activeWifis = GetBestWifis();
    
    if(activeWifis.empty())
    {
        return NULL;
    }

    INT32 index = -1;
    for(UINT32 i = 0; i < activeWifis.size(); i++)
    {
        if(!activeWifis[i] || activeWifis[i]->GetSSID().empty())
        {
            SafeDeletePointerEx(activeWifis[i]);
            continue;
        }
        index = i;
        break;
    }
    
    if(index == -1)
    {
        activeWifis.clear();
        return NULL;
    }
    
    WifiAccess* bestWifi = new WifiAccess(activeWifis[index]->GetSSID(), activeWifis[index]->GetPassword(), activeWifis[index]->GetIdentity(), activeWifis[index]->GetAccessCount(), activeWifis[index]->GetSecurityType());

    if(!bestWifi)
    {
        return NULL;
    }
    bestWifi->SetSignalStrength(activeWifis[index]->GetSignalStrength());
    
    DebugPrintf(DLC_LIUJT, "DKWifiCfgManager::GetBestWifi Get %d best wifi works", activeWifis.size());

    for(UINT i = 0; i < activeWifis.size(); i++)
    {
        WifiAccess * tmp = activeWifis[i];
        SafeDeletePointerEx(tmp);
    }
    activeWifis.clear();
    DebugPrintf(DLC_LIUJT, "DKWifiCfgManager::GetBestWifi Best wifi: %s", !bestWifi ? "NONE": "HAS ONE");
    return bestWifi;

}
/*
*This method will delete memory referrenced by each element in wifi;
*/
void DKWifiCfgManager::SaveWifiConfig(WifiAccess* wifi)
{
    if(!wifi || wifi->GetSSID().empty()) 
    {
        SafeDeletePointerEx(wifi);
        return;
    }
    vector<WifiAccess *> wifiFromHistory = GetHistoryWifis();
    vector<WifiAccess *> newWifiList(wifiFromHistory);
    BOOL findIt = FALSE;    
    for(UINT i = 0; i < newWifiList.size(); i++)
    {
        WifiAccess * history = newWifiList[i];
        if (!history || history->GetSSID().empty())
        {
            continue;
        }
        
        if(history->GetSSID() == wifi->GetSSID())
        {
            history->SetIdentity(wifi->GetIdentity());
            history->SetPassword(wifi->GetPassword());
            history->SetSecurityType(wifi->GetSecurityType());
            history->SetAccessCount(history->GetAccessCount()+1);            
            history->SetLastAccess(wifi->GetLastAccess());
            history->SetStaticIP(wifi->IsStaticIP());
            if(wifi->IsStaticIP())
            {
                history->SetStaticIPAddress(wifi->GetStaticIPAddress());
                history->SetMask(wifi->GetMask());
                history->SetGateway(wifi->GetGateway());
                history->SetDNS(wifi->GetDNS());
            }
            findIt = TRUE;
            delete wifi;
            wifi = NULL;
            break;
        }
    }
    
    if(FALSE == findIt)
    {
        newWifiList.push_back(wifi);
    }

    WriteWifiConfigToFile(newWifiList);
}

/*
*This method will delete memory referrenced by wifi;
*/
void DKWifiCfgManager::RemoveWifiConfig(WifiAccess* wifi)
{
    if(!wifi || wifi->GetSSID().empty()) 
    {
        SafeDeletePointerEx(wifi);
        return;
    }
    
    vector<WifiAccess *> wifiFromHistory = GetHistoryWifis();
    vector<WifiAccess *> newWifiList;
    UINT uLen = wifiFromHistory.size();
    for(UINT i = 0; i < uLen; i++)
    {
        WifiAccess * history = wifiFromHistory[i];
        if (!history  || history->GetSSID().empty() || history->GetSSID()== wifi->GetSSID())//remove the target
        {
            SafeDeletePointerEx(history);
            continue;
        }
        
        //keep the other ones in the list
        newWifiList.push_back(history);
    }

    delete wifi;
    wifi = NULL;
    
    WriteWifiConfigToFile(newWifiList);
}
/*
*This method will delete memory referrenced by each element in apList;
*/
void DKWifiCfgManager::WriteWifiConfigToFile(vector<WifiAccess*> wifiList)
{
    CIniFile iniFile;
    iniFile.SetPath(WIFI_CFG_FILE);
    iniFile.Reset();

    for(vector<WifiAccess*>::size_type i=0; i< wifiList.size(); i++)
    {
        WifiAccess* pWA = wifiList.at(i);
        if (!pWA || pWA->GetSSID().empty())
        {
            SafeDeletePointerEx(pWA);
            
            continue;
        }

        iniFile.StoreStringValue(pWA->GetSSID(), WIFIIDENTITY, pWA->GetIdentity(), TRUE);

        string sPasswordHex = EncodeUtil::BinToHex( (const unsigned char*)(pWA->GetPassword()).c_str(), pWA->GetPassword().length());
        iniFile.StoreStringValue(pWA->GetSSID(), WIFIPASSWORDHEX, sPasswordHex, TRUE);
        iniFile.StoreIntValue(pWA->GetSSID(), WIFISECURITYTYPE, pWA->GetSecurityType(), TRUE);
        iniFile.StoreIntValue(pWA->GetSSID(), WIFIACCESSACOUNT, pWA->GetAccessCount(), TRUE);
        iniFile.StoreBoolValue(pWA->GetSSID(), WIFISTATICIP, pWA->IsStaticIP(), TRUE);
        iniFile.StoreIntValue(pWA->GetSSID(), WIFILASTACCESS, pWA->GetLastAccess(), TRUE);

        if(pWA->IsStaticIP())
        {
            iniFile.StoreStringValue(pWA->GetSSID(), WIFISTATICIPADDRESS, pWA->GetStaticIPAddress(), TRUE);
            iniFile.StoreStringValue(pWA->GetSSID(), WIFISTATICIPMASK, pWA->GetMask(), TRUE);
            iniFile.StoreStringValue(pWA->GetSSID(), WIFISTATICIPGATEWAY, pWA->GetGateway(), TRUE);
            iniFile.StoreStringValue(pWA->GetSSID(), WIFISTATICIPDNS, pWA->GetDNS(), TRUE);
        }
        delete pWA;
        pWA = NULL;
    }
    DebugPrintf(DLC_LIUJT, "DKWifiCfgManager::WriteWifiConfigToFile  Begin to write file!!!!");
    iniFile.WriteFile();
    DebugPrintf(DLC_LIUJT, "DKWifiCfgManager::WriteWifiConfigToFile  DONE!!!!");
}

/*
*This method will delete memory referrenced by each element in apList;
*/
void DKWifiCfgManager::AddUserAPToFile(const vector<AccessPoint*>& apList)
{
    DebugPrintf(DLC_LIUJT, "DKWifiCfgManager::AddUserAPToFile  Entering");
    CIniFile iniFile;
    iniFile.SetPath(WIFI_ADDED_FILE); 
    iniFile.Reset();

    for(vector<AccessPoint*>::size_type i=0; i< apList.size(); i++)
    {
        AccessPoint* ap = apList[i];
        if (!ap || ap->SSID.empty())
        {
            continue;
        }
        
        iniFile.StoreBoolValue(ap->SSID, WIFIPUBLIC, ap->fPublic, TRUE);
    }    
    iniFile.WriteFile();
}

/*This method will be responsible to release memory for input pointer.
*/
void DKWifiCfgManager::AddUserAPToFile(AccessPoint* ap)
{
    if(!ap || ap->SSID.empty()) 
    {
        SafeDeletePointerEx(ap);
        return;
    }
    
    vector<AccessPoint *> NewAPList = GetUserAPFromFile();
    BOOL findIt = FALSE;
    for(UINT i = 0; i < NewAPList.size(); i++)
    {
        AccessPoint * CurAP = NewAPList[i];
        if (!CurAP  || CurAP->SSID.empty())
        {
            continue;
        }
        
        if(CurAP->SSID == ap->SSID) //remove the target
        {
            findIt = TRUE;

            CurAP->fPublic = ap->fPublic;
            
            delete ap;
            ap = NULL;
            
            break;
        }
    }
    
    if(!findIt)
    {
        NewAPList.push_back(ap);
    }
        
    AddUserAPToFile(NewAPList);
    for (vector<AccessPoint *>::iterator vi = NewAPList.begin(); vi != NewAPList.end(); vi++)
    {
        delete (*vi);
        (*vi) = NULL;
    }
}

vector<AccessPoint*> DKWifiCfgManager::GetUserAPFromFile()
{
    CIniFile inifile;
    vector<AccessPoint*> apLists;
    inifile.SetPath(WIFI_ADDED_FILE);

    IniKeyMap::iterator keymapiter;

    if (inifile.ReadFile())
    {
        for(keymapiter=inifile.keys.begin();keymapiter!=inifile.keys.end();++keymapiter)
        {
            string APSSID = keymapiter->first;
            if(APSSID.empty())
            {
                continue;
            }
            bool bPublic = inifile.GetBoolValue(APSSID, WIFIPUBLIC, FALSE);
            AccessPoint* ap = new AccessPoint(APSSID, bPublic);
            if(!ap)
            {
                continue;
            }
            apLists.push_back(ap);
      }
    }
    DebugPrintf(DLC_LIUJT, "DKWifiCfgManager::GetUserAPFromFile get AP list size()=%d", apLists.size());
    
    return apLists;
}

