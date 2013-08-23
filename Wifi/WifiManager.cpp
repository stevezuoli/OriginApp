#include "Wifi/WifiManager.h"
#include "dkWinError.h"
#include "interface.h"
#include "Wifi/WifiBaseImpl.h"
#include "Wifi/WifiCfg.h"
#include "Utility/ThreadHelper.h"
#include "Framework/CNativeThread.h"
#include "Framework/INativeMessageQueue.h"
#include "SQM.h"
#include "drivers/DeviceInfo.h"
#include "GUI/EventSet.h"
#include "PersonalUI/PushedMessagesManager.h"
#include "Utility/SystemUtil.h"

#include <iostream>
#include <fstream>

using namespace std;
using dk::utility::SystemUtil;

class CJoinAPAsyncParam
{
public:
    FuncPointer BetweenFunc;
    FPWifiConnection AfterFunc;
    string m_strSSID;
    string m_strPassword;
    string m_strIdentity;
    int m_iSecurityTypes;
    string m_strIP;
    string m_strMask;
    string m_strGate;
    string m_strDNS;
    CJoinAPAsyncParam()
    {
        BetweenFunc = NULL;
        AfterFunc = NULL;
        m_strSSID.clear();
        m_strPassword.clear();
        m_strIdentity.clear();
        m_iSecurityTypes = 0;
        m_strIP.clear();
        m_strMask.clear();
        m_strGate.clear();
        m_strDNS.clear();
    };
};

class CWifiCallbackFuncs
{
    public:

        FuncPointer BetweenCalling;
        FuncPointer AfterCalling;

        CWifiCallbackFuncs( FuncPointer between,  FuncPointer after):
            BetweenCalling(between)
        ,   AfterCalling(after)
        {}
};

eWifiManagerWorking WifiManager::m_eWorking = Nothing;
AccessPoint** WifiManager::m_ppAPList = NULL;
int WifiManager::m_iAPCount = 0;
int WifiManager::m_iAutoJoinSleepTime = 0;
int WifiManager::m_iAutoRefreshTime = 0;

bool WifiManager::m_fAutoJoinRunning = false;
bool WifiManager::m_fAutoRefreshing = false;

pthread_t WifiManager::m_pThreadPowerOn = 0;
pthread_t WifiManager::m_pThreadPowerOff = 0;
pthread_t WifiManager::m_pThreadRefresh = 0;
pthread_t WifiManager::m_pThreadJoinAP = 0;
pthread_t WifiManager::m_pThreadAutoJoin = 0;
pthread_t WifiManager::m_pThreadAutoRefresh = 0;

pthread_mutex_t WifiManager::mutex_wifi_scan;
pthread_mutex_t WifiManager::mutex_wifi_join;

int WifiManager::m_wifi_status = 0;

SINGLETON_CPP(WifiManager);

const char* WifiManager::WifiAvailableEvent = "WifiAvailable";
const char* WifiManager::WifiUnavailableEvent = "WifiUnavailable";
/*const char* WifiManager::WifiAutoJoinEvent = "WifiAutoJoin";
const char* WifiManager::WifiRefreshEvent = "WifiRefresh";
const char* WifiManager::WifiShowRefreshTipEvent = "WifiShowRefreshTip";
*/

WifiManager::WifiManager()
{
    m_bCurConnectedStatus = false;
    m_iSignalStrength = 0;
    m_pThreadDaemon = 0;
	pthread_mutex_init(&mutex_wifi_scan, NULL);
	pthread_mutex_init(&mutex_wifi_join, NULL);
}


WifiManager::~WifiManager()
{
    SafeDeleteArrayEx(m_ppAPList);
		
	pthread_mutex_destroy(&mutex_wifi_scan);	
	pthread_mutex_destroy(&mutex_wifi_join);
}
IWifiBase* WifiManager::GetWifiBase()
{
    return CWifiBaseImpl::GetInstance();
}

int WifiManager::GetActiveSignalLevel()
{
    
    if(Connecting == m_eWorking)
    {
#ifdef KINDLE_FOR_TOUCH
        return 4;
#else
        return 5;//Connecting state, caller will use it to display icon for connecting
#endif
    }
    else if(PowerOffIsWorking == m_eWorking)
    {
#ifdef KINDLE_FOR_TOUCH
        return 5;
#else
        return 6;//Powering off state, caller will use it to display icon for powering off wifi
#endif
    }
    else
    {
         return m_iSignalStrength;
    }
}

// 说明： iwlist扫描出来的信号, Noise Level为-95dBm
// Signal level 等于或低于Noise Level则信号被淹没
// 信号强度的范围是(Quality= 1 to 94), 对应 （Signal level -94 dBm to - 1 dBm）
// 
// 802.11n 典型值
// 某个产品典型值
// 300Mbps：-65dBm
// 135Mbps：-74dBm
// 11Mbps：-84dBm
// 无线速率：300Mbps
// 工作频段：2.4-2.4835GHz
// 灵敏度（丢包率）：
// 270M：-68dBm@10% PER
// 130M：-68dBm@10% PER
// 108M：-68dBm@10% PER
// 54M：-68dBm@10% PER
// 11M：-85dBm@8% PER
// 6M：-88dBm@10% PER
// 1M：-90dBm@8% PER


int WifiManager::QualityToLevel(int iQuality)
{
    if(0 >= iQuality|| iQuality > 100)
    {
        return 0;
    }

#ifdef KINDLE_FOR_TOUCH

	//  upper -60 dBm
    if(iQuality >= 35)
    {
        return 3;
    }
	
	//  upper -68 dBm
	if(iQuality >= 27)
    {
        return 2;
    }
	
	// upper -74 dBm
	if(iQuality >= 21)
    {
        return 1;
    }

    // upper -84 dBm
    if(iQuality >= 11)
    {
        return 1;
    }

	// -85 dBm to - 94dBm
    return 0;

#else
	//  upper -60 dBm
    if(iQuality >= 35)
    {
        return 4;
    }
	
	//  upper -68 dBm
	if(iQuality >= 27)
    {
        return 3;
    }
	
	// upper -74 dBm
	if(iQuality >= 21)
    {
        return 2;
    }

    // upper -84 dBm
    if(iQuality >= 11)
    {
        return 1;
    }

	// -85 dBm to - 94dBm
    return 0;
#endif

}
void* WifiManager::AutoJoin(void *Func)
{
    pthread_detach(pthread_self());
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, 0);

    if(WifiManager::GetInstance()->IsConnected())
    {
        m_pThreadAutoJoin = 0;
        ThreadHelper::ThreadExit(0);
        return NULL;
    }
	
	int err = pthread_mutex_trylock(&mutex_wifi_join);

	if ( err != 0) 
	{	      
        m_pThreadAutoJoin = 0;
        ThreadHelper::ThreadExit(0);
        return NULL;
	}

	WifiManager::GetInstance()->SetConnectedStatus(false);
	m_eWorking = Connecting;

    WifiManager::GetInstance()->StopWifiDaemon();    

    CJoinAPAsyncParam *cwf = (CJoinAPAsyncParam *) Func;

    FuncPointer between = cwf ? cwf->BetweenFunc: NULL;
    if(between)
    {
        between();
    }
	
	FPWifiConnection after = cwf ? cwf->AfterFunc: NULL;

    vector<WifiAccess*> bestWifis;
    WifiAccess * pConnectedWifi = NULL;
    UINT uLen= 0;

    // GetBestWifis() scanning and sorting the active ssids, based on signal-strength
    for(int i = 0; i < 2; i++)
    {
        bestWifis = DKWifiCfgManager::GetInstance()->GetBestWifis();
        if(bestWifis.size() != 0)
            break;
        usleep(300000);
    }

    uLen = bestWifis.size();
    //DebugPrintf(DLC_DIAGNOSTIC, "WifiManager::AutoJoin(), GetBestWifis(%d)", uLen);

    // if GetBestWifis() get no active ssid,  then connect the ssid(s) in history list
    if(!uLen)
    {
        bestWifis = DKWifiCfgManager::GetInstance()->GetBestUnavailableWifis();
        uLen = bestWifis.size();
        DebugPrintf(DLC_DIAGNOSTIC, "WifiManager::AutoJoin(), GetBestUnavailableWifis(%d)", uLen);
    }

    if(uLen)
    {
        DebugPrintf(DLC_LIUJT, "WifiManager::AutoJoin begin to connect following %d wifis", uLen);
        for(UINT32 i =0 ; i < uLen; i++)
        {
            WifiAccess* wifi = bestWifis[i];

            if(!wifi || wifi->GetSSID().empty())
            {
                SafeDeletePointerEx(wifi);
                continue ;
            }
            if(pConnectedWifi)//如果已连接成功，则删除其后不用的wifi，避免内存泄漏
            {
                SafeDeletePointerEx(wifi);
                continue;
            }
			
			// 自动连接时，过滤掉CMCC开头的，空密码AP， 比如： CMCC, CMCC-HEMC
			if(wifi->GetPassword().empty() && wifi->GetSSID().find("CMCC") == 0)
			{
				DebugPrintf(DLC_DIAGNOSTIC, "WifiManager::AutoJoin(), skip (%s)", wifi->GetSSID().c_str());
                SafeDeletePointerEx(wifi);
                continue;
			}

            if (WifiManager::GetInstance()->IsManualJoinAP())
            {
                SafeDeletePointerEx(wifi);
                continue ;
            }

			WifiManager::GetInstance()->SetConnectedStatus(false);

			m_wifi_status = (int) CONNECT_WIFI_JOINAP;
            HRESULT result = CWifiBaseImpl::GetInstance()->JoinAP(
                wifi->GetSSID(), 
                wifi->GetPassword(), 
                wifi->GetIdentity(),
                wifi->GetSecurityType(), 
                wifi->GetStaticIPAddress(), 
                wifi->GetMask(), 
                wifi->GetGateway(), 
                wifi->GetDNS());

            if (WifiManager::GetInstance()->IsManualJoinAP())
            {
                SafeDeletePointerEx(wifi);
                continue;
            }

			// 自动连接过程中，不发DHCP消息。分配IP成功时，不需要跳到WiFi管理界面
			if( S_OK == result)
			{
				m_wifi_status = (int) CONNECT_WIFI_DHCP;
				
				eIPStatus IPstatus =  CWifiBaseImpl::GetInstance()->SetIP(
					wifi->GetStaticIPAddress(),
					wifi->GetMask(), 
					wifi->GetGateway(),
					wifi->GetDNS());

				switch(IPstatus)
				{
				case DHCP_STAITC_IP:
					m_wifi_status = (int) CONNECT_WIFI;
					WifiManager::GetInstance()->SetConnectedStatus(true);
					result = S_OK;
					break;
				case DHCP_OK:
					m_wifi_status = (int) CONNECT_WIFI;
					WifiManager::GetInstance()->SetConnectedStatus(true);
					result = S_OK;
					break;
				case DHCP_CONNECT_LOST:
					m_wifi_status = (int) CONNECT_WIFI_DHCP_LOST;
					WifiManager::GetInstance()->SetConnectedStatus(false);
					result = E_FAIL;
					break;
				case DHCP_NO_DNS:					
					m_wifi_status = (int) CONNECT_WIFI_NO_DNS;
					WifiManager::GetInstance()->SetConnectedStatus(false);
					result = E_FAIL;
					break;
				case DHCP_NO_GATEWAY:
					m_wifi_status = (int) CONNECT_WIFI_NO_GW;
					WifiManager::GetInstance()->SetConnectedStatus(false);
					result = E_FAIL;
					break;
				case DHCP_NO_LEASE:
					m_wifi_status = (int) CONNECT_WIFI_DHCP_FAIL;
					WifiManager::GetInstance()->SetConnectedStatus(false);
					result = E_FAIL;
					break;
				case DHCP_OFFER_WRONG:					
					m_wifi_status = (int) CONNECT_WIFI_DHCP_OFFER;
					WifiManager::GetInstance()->SetConnectedStatus(false);
					result = E_FAIL;
					break;
				}

				if(after)
				{
					after((result == S_OK), wifi->GetSSID(), wifi->GetPassword(), wifi->GetIdentity());
				}
				
			}
            
            if (WifiManager::GetInstance()->IsManualJoinAP())
            {
                SafeDeletePointerEx(wifi);
                continue;
            }

            if(S_OK == result)
            {
                AccessPoint* ap = CWifiBaseImpl::GetInstance()->GetCurAP();
                int iSecrityType = UNKNOWN_WIFI_SECURITY_TYPE;
                if(ap && ap->SSID == wifi->GetSSID())
                {
                    iSecrityType = ap->SecurityType;
                }
                else
                {
                    DebugPrintf(DLC_LIUJT, "WifiManager::AutoJoin Get Wifi Connected, but can't get it from GetCurAP()!!!!!!!!!");
                }

                DebugPrintf(DLC_LIUJT, "WifiManager::AutoJoin Save SSID: %s, password: %s, securitytype : %d, unknowntype=%d", wifi->GetSSID().c_str(),wifi->GetPassword().c_str(), iSecrityType, UNKNOWN_WIFI_SECURITY_TYPE);
                WifiAccess* wa = new WifiAccess(wifi->GetSSID(), wifi->GetPassword(), wifi->GetIdentity(), 1, iSecrityType);
                if(wa)
                {
                    wa->SetStaticIP(wifi->IsStaticIP());
                    if(wifi->IsStaticIP())
                    {
                        wa->SetStaticIPAddress(wifi->GetStaticIPAddress());
                        wa->SetMask(wifi->GetMask());
                        wa->SetGateway(wifi->GetGateway());
                        wa->SetDNS(wifi->GetDNS());
                    }

                    wa->SetLastAccess(1);
                    DKWifiCfgManager::GetInstance()->SaveWifiConfig(wa);
                }
                
                pConnectedWifi = wifi;
                DebugPrintf(DLC_LIUJT, "WifiManager::AutoJoin successfully connect ssid: %s with password: %s", wifi->GetSSID().c_str(), wifi->GetPassword().c_str());

            }
            else
            {
                DebugPrintf(DLC_LIUJT, "WifiManager::AutoJoin failed to connect ssid: %s with password: %s", wifi->GetSSID().c_str(), wifi->GetPassword().c_str());
                SafeDeletePointerEx(wifi);
            }

        }
    }
    else
    {
        DebugPrintf(DLC_LIUJT, "WifiManager::AutoJoin gets no wifis");
    }

    m_eWorking = Nothing;
    if(after)
    {
        if(pConnectedWifi)
        {
            after(TRUE, pConnectedWifi->GetSSID(), pConnectedWifi->GetPassword(), pConnectedWifi->GetIdentity());
            WifiManager* pManager =  WifiManager::GetInstance();
            if(pManager)
            {
                pManager->InitConnectedStatus();
                pManager->StartWifiDaemon();
            }
        }
        else
        {
            after(FALSE, "", "", "");
        }
        
    }
    
    SafeDeletePointerEx(pConnectedWifi);
    SafeDeletePointerEx(cwf);

	m_eWorking = Nothing;
	pthread_mutex_unlock(&mutex_wifi_join);
    m_pThreadAutoJoin = 0;
    ThreadHelper::ThreadExit(0);
    return NULL;
}


HRESULT WifiManager::StartAutoJoin(FuncPointer between, FPWifiConnection after)
{
    
    CJoinAPAsyncParam *pParam = new CJoinAPAsyncParam();
    if(!pParam)
    {
        return E_FAIL;
    }
    pParam->BetweenFunc = between;
    pParam->AfterFunc= after;

    ThreadHelper::CreateThread(&m_pThreadAutoJoin,AutoJoin,(void*)pParam, "WifiManager @ AutoJoin", true, 51200);

    return S_OK;
}
HRESULT WifiManager::EndAutoJoin()
{
    //DebugPrintf(DLC_DIAGNOSTIC, "WifiManager::EndAutoJoin ");
    SystemUtil::ExecuteCommand("killall -9 wpa_supplicant;killall -9 udhcpc");
    if(m_pThreadAutoJoin)
    {
        ThreadHelper::CancelThread(m_pThreadAutoJoin);
    }
	m_pThreadAutoJoin = 0;
	m_eWorking = Nothing;
    WifiManager::GetInstance()->SetConnectedStatus(false);
    return S_OK;
}

HRESULT WifiManager::Refresh()
{
    return GetWifiBase()->Refresh();
}

HRESULT WifiManager::PowerOn()
{
    //DebugPrintf(DLC_DIAGNOSTIC, "WifiManager::PowerOn Begin");
    HRESULT hr = GetWifiBase()->PowerOn();
    //DebugPrintf(DLC_DIAGNOSTIC, "WifiManager::PowerOn End");
    return hr;
}

HRESULT WifiManager::PowerOff()
{
    //DebugPrintf(DLC_DIAGNOSTIC, "WifiManager::PowerOff Begin");
    PushedMessagesManager* pPushedMessagesMgr = PushedMessagesManager::GetInstance();
    if (pPushedMessagesMgr)
    {
        pPushedMessagesMgr->StopGetMessages();
    }
    HRESULT hr = GetWifiBase()->PowerOff();
    StopWifiDaemon();
    //由于UpdateStatus只在WiFiDaemon线程运行时执行，因此当WiFiDaemon停止时，应直接发送WiFi不可用消息。
    
    SNativeMessage msg;
    msg.iType = KMessageWifiChange;
    msg.iParam1 = (UINT32)ALLRECEIVER;
    msg.iParam2 = UPDATE_WIFI_STATUS;
    msg.iParam3 = WIFIUNAVAILABLE;
    DebugPrintf(DLC_LIUJT, "WifiManager::PowerOff send wifi unavailable message!!!");
    INativeMessageQueue::GetMessageQueue()->Send(msg);

    FireWifiUnavailableEvent();

    m_bCurConnectedStatus = false;
    m_iSignalStrength = 0;
    //DebugPrintf(DLC_DIAGNOSTIC, "WifiManager::PowerOff End");
    return hr;
}
HRESULT WifiManager::GetAPList(AccessPoint*** pppAPList,int *iAPCount)
{
    return GetWifiBase()->GetAPList(pppAPList,iAPCount);
}

#if 0
HRESULT WifiManager::JoinAP(std::string strSSID)
{
    return GetWifiBase()->JoinAP(strSSID);
}
HRESULT WifiManager::JoinAP(std::string strSSID,std::string strPassword,int securityTypes ,std::string strIP,std::string strMask,std::string strGate,std::string strDNS)
{
    return GetWifiBase()->JoinAP( strSSID, strPassword, securityTypes , strIP , strMask , strGate , strDNS);
}

#endif

void* WifiManager::PowerOnAsync(void *pFunc)
{
    if(m_eWorking != Nothing)
    {
        ThreadHelper::ThreadExit(0);
        return NULL;
    }
    
    pthread_detach(pthread_self());
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);

    
    CWifiCallbackFuncs * cwf = (CWifiCallbackFuncs *)pFunc;
    
    m_eWorking = PowerOnIsWorking;

    FuncPointer between = cwf ? cwf->BetweenCalling: NULL;
    
    if(between)
    {
        between();
    }
    
    WifiManager::GetInstance()->PowerOn();

    m_eWorking = Nothing;

    if(GetInstance()->IsPowerOn())
    {
        SafeDeleteArrayEx(m_ppAPList);
        m_iAPCount = 0;
        CWifiBaseImpl::GetInstance()->GetAPList(&m_ppAPList,&m_iAPCount);
    }
    

    FuncPointer after = cwf ? cwf->AfterCalling: NULL;
    if(after)
    {
        after();
    }
    SafeDeletePointerEx(cwf);
    ThreadHelper::ThreadExit(0);
    return NULL;
}
HRESULT WifiManager::StartPowerOnAsync (FuncPointer between, FuncPointer after)
{
    CWifiCallbackFuncs* cwf = new CWifiCallbackFuncs(between, after);
    if(!cwf)
    {
        return E_FAIL;
    }

    bool detached = true;
    ThreadHelper::CreateThread(&m_pThreadPowerOn, PowerOnAsync, (void*)cwf, "WifiManager @ PowerOnAsync" , detached);
    return S_OK;
}
void* WifiManager::PowerOffAsync(void *pFunc)
{
    if(m_eWorking == PowerOffIsWorking)
    {
        ThreadHelper::ThreadExit(0);
        return NULL;
    }

    pthread_detach(pthread_self());
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    
    m_eWorking = PowerOffIsWorking;
    CWifiCallbackFuncs * cwf = (CWifiCallbackFuncs *)pFunc;
    
    FuncPointer between = cwf ? cwf->BetweenCalling: NULL;
    if(between)
    {
        between();
    }
    
    WifiManager::GetInstance()->PowerOff();
    m_eWorking = Nothing;

    FuncPointer after = cwf ? cwf->AfterCalling: NULL;
    if(after)
    {
        after();
    }
    SafeDeletePointerEx(cwf);
    ThreadHelper::ThreadExit(0);
    return NULL;
}
HRESULT WifiManager::StartPowerOffAsync(FuncPointer between, FuncPointer after)
{
    CWifiCallbackFuncs* cwf = new CWifiCallbackFuncs(between, after);
    if(!cwf)
    {
        return E_FAIL;
    }

	bool detached = true;
    ThreadHelper::CreateThread(&m_pThreadPowerOff, PowerOffAsync, (void*)cwf , "WifiManager @ PowerOffAsync" , detached);
    return S_OK;
}

void* WifiManager::RefreshAsync(void* pFunc)
{
	//DebugPrintf(DLC_DIAGNOSTIC, "WifiManager::RefreshAsync(), begin");	
	int err = pthread_mutex_trylock(&mutex_wifi_scan);

	if ( err != 0) 
	{	
		DebugPrintf(DLC_DIAGNOSTIC, "WifiManager::RefreshAsync(), LOCK Err(%d)", err);	
        ThreadHelper::ThreadExit(0);
        return NULL;
	}

    CWifiCallbackFuncs * cwf = (CWifiCallbackFuncs *)pFunc;
    
    FuncPointer between = cwf ? cwf->BetweenCalling: NULL;
    if(between)
    {
        between();
    }

    SafeDeleteArrayEx(m_ppAPList);
    m_iAPCount = 0;
    CWifiBaseImpl::GetInstance()->GetAPList(&m_ppAPList,&m_iAPCount);

    FuncPointer after = cwf ? cwf->AfterCalling: NULL;
    if(after)
    {
        after();
    }

    SafeDeletePointerEx(cwf);	
	pthread_mutex_unlock(&mutex_wifi_scan);

	//DebugPrintf(DLC_DIAGNOSTIC, "WifiManager::RefreshAsync(), end");	

    ThreadHelper::ThreadExit(0);
    return NULL;
}


HRESULT WifiManager::StartRefreshAsync(FuncPointer between, FuncPointer after)
{
    CWifiCallbackFuncs* cwf = new CWifiCallbackFuncs(between, after);
    if(!cwf)
    {
        return E_FAIL;
    }
	
	bool detached = true;
    ThreadHelper::CreateThread(&m_pThreadRefresh, RefreshAsync, (void*)cwf, "WifiManager @ RefreshAsync" , detached);
    return S_OK;
}


void* WifiManager::JoinApAsync(void* AsynCallback)
{	
	//DebugPrintf(DLC_DIAGNOSTIC, "WifiManager::JoinApAsync(), begin");	

	CJoinAPAsyncParam *pParam = (CJoinAPAsyncParam*)AsynCallback;
    if(NULL == pParam)
    {
		DebugPrintf(DLC_DIAGNOSTIC, "WifiManager::JoinApAsync(), AsynCallback(NULL)");
        m_pThreadJoinAP = 0;
        ThreadHelper::ThreadExit(0);
        return NULL;
    }

#if 0
	int err = pthread_mutex_trylock(&mutex_wifi_join);
	if ( err != 0) 
	{	
		DebugPrintf(DLC_DIAGNOSTIC, "WifiManager::JoinApAsync(), LOCK Err(%d)", err);	
        m_pThreadJoinAP = 0;
        ThreadHelper::ThreadExit(0);
        return NULL;
	}
#endif

    pthread_detach(pthread_self());
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    WifiManager::GetInstance()->StopWifiDaemon();
	
    m_eWorking = Connecting;
	WifiManager::GetInstance()->SetConnectedStatus(false);

    FuncPointer between = pParam->BetweenFunc;

    if(between)
    {
        between();
    }
	
	// JoinAP ，包括连接SSID和分配IP， 耗时最长可达10秒以上
        
	m_wifi_status = (int) CONNECT_WIFI_JOINAP;
    HRESULT result = CWifiBaseImpl::GetInstance()->JoinAP(
                                    pParam->m_strSSID,
                                    pParam->m_strPassword,
                                    pParam->m_strIdentity,
                                    pParam->m_iSecurityTypes,
                                    pParam->m_strIP,
                                    pParam->m_strMask,
                                    pParam->m_strGate,
                                    pParam->m_strDNS);

    pthread_testcancel();
	if(result == S_OK)
	{
		m_wifi_status = (int) CONNECT_WIFI_DHCP;
	
		if(pParam->AfterFunc) 
		{
			pParam->AfterFunc(FALSE,pParam->m_strSSID,pParam->m_strPassword, pParam->m_strIdentity);
		}
		
		eIPStatus IPstatus =  CWifiBaseImpl::GetInstance()->SetIP(
                                    pParam->m_strIP,
                                    pParam->m_strMask,
                                    pParam->m_strGate,
                                    pParam->m_strDNS);

        pthread_testcancel();
		switch(IPstatus)
		{
		case DHCP_STAITC_IP:
			m_wifi_status = (int) CONNECT_WIFI;
			WifiManager::GetInstance()->SetConnectedStatus(true);
			result = S_OK;
			break;
		case DHCP_OK:
			m_wifi_status = (int) CONNECT_WIFI;
			WifiManager::GetInstance()->SetConnectedStatus(true);
			result = S_OK;
			break;
		case DHCP_CONNECT_LOST:
			m_wifi_status = (int) CONNECT_WIFI_DHCP_LOST;
			WifiManager::GetInstance()->SetConnectedStatus(false);
			result = E_FAIL;
			break;
		case DHCP_NO_DNS:					
			m_wifi_status = (int) CONNECT_WIFI_NO_DNS;
			WifiManager::GetInstance()->SetConnectedStatus(false);
			result = E_FAIL;
			break;
		case DHCP_NO_GATEWAY:
			m_wifi_status = (int) CONNECT_WIFI_NO_GW;
			WifiManager::GetInstance()->SetConnectedStatus(false);
			result = E_FAIL;
			break;
		case DHCP_NO_LEASE:
			m_wifi_status = (int) CONNECT_WIFI_DHCP_FAIL;
			WifiManager::GetInstance()->SetConnectedStatus(false);
			result = E_FAIL;
			break;
		case DHCP_OFFER_WRONG:					
			m_wifi_status = (int) CONNECT_WIFI_DHCP_OFFER;
			WifiManager::GetInstance()->SetConnectedStatus(false);
			result = E_FAIL;
			break;
		}

		if(pParam->AfterFunc) 
		{
			pParam->AfterFunc( (result == S_OK),pParam->m_strSSID,pParam->m_strPassword, pParam->m_strIdentity);
		}

	}

	m_eWorking = Nothing;
		
	//DebugPrintf(DLC_DIAGNOSTIC, "WifiManager::JoinApAsync  after posting, m_eWorking=%d", m_eWorking);
    
    pthread_testcancel();

    SQM::GetInstance()->IncCounter(SQM_ACTION_WIFI_CONNECT_END);

    if(pParam->AfterFunc)
    {
        if(S_OK == result)
        {
            AccessPoint* ap = CWifiBaseImpl::GetInstance()->GetCurAP();
            int securityType = UNKNOWN_WIFI_SECURITY_TYPE;
            if(ap && ap->SSID == pParam->m_strSSID)
            {
                securityType = ap->SecurityType;
            }
            else
            {
                DebugPrintf(DLC_LIUJT, "Get Wifi Connected, but can't get it from GetCurAP()!!!!!!!!!");
            }
            DebugPrintf(DLC_LIUJT, "Save SSID: %s, password: %s, securitytype : %d, unknowntype=%d", pParam->m_strSSID.c_str(),pParam->m_strPassword.c_str(), securityType,UNKNOWN_WIFI_SECURITY_TYPE);
            WifiAccess* wa = new WifiAccess(pParam->m_strSSID,pParam->m_strPassword, pParam->m_strIdentity, 1, securityType);
            if(wa)
            {
                BOOL bStaticIP = pParam->m_strIP.size() > 0;
                wa->SetStaticIP(bStaticIP);
                if(bStaticIP)
                {
                    wa->SetStaticIPAddress(pParam->m_strIP);
                    wa->SetMask(pParam->m_strMask);
                    wa->SetGateway(pParam->m_strGate);
                    wa->SetDNS(pParam->m_strDNS);
                }

                wa->SetLastAccess(1);
                DKWifiCfgManager::GetInstance()->SaveWifiConfig(wa);
            }

            pParam->AfterFunc(TRUE,pParam->m_strSSID,pParam->m_strPassword, pParam->m_strIdentity);
            WifiManager* pManager =  WifiManager::GetInstance();
            if(pManager)
            {
                pManager->InitConnectedStatus();
                pManager->StartWifiDaemon();
            }
        }
        else
        {
            /*WifiAccess* wa = new WifiAccess(pParam->m_strSSID,pParam->m_strPassword);
            if(wa)
            {
                DKWifiCfgManager::GetInstance()->RemoveWifiConfig(wa);
            }*/
            pParam->AfterFunc(FALSE,pParam->m_strSSID,pParam->m_strPassword, pParam->m_strIdentity);
        }
    }

    delete pParam;
    pParam = NULL;
  
#if 0
	pthread_mutex_unlock(&mutex_wifi_join);
#endif

	//DebugPrintf(DLC_DIAGNOSTIC, "WifiManager::JoinApAsync(), end");	
    m_pThreadJoinAP = 0;
    ThreadHelper::ThreadExit(0);
    return NULL;
}

HRESULT WifiManager::StartJoinAPAsync(FuncPointer between, FPWifiConnection after, std::string strSSID)
{
    if(strSSID.empty())
    {
        return S_OK;
    }
    int securityType = NONE;
    return StartJoinAPAsync(between , after,strSSID,"", "",securityType);
}

HRESULT WifiManager::StartJoinAPAsync(FuncPointer between, FPWifiConnection after, std::string strSSID,std::string strPassword, std::string strIdentity, int securityTypes ,string strIP,string strMask,string strGate,string strDNS) 
{
    if(strSSID.empty())
    {
        return S_OK;
    }

    if (!WifiManager::GetInstance()->IsPowerOn() )
    {
        WifiManager::GetInstance()->PowerOn();
    }        

    SQM::GetInstance()->IncCounter(SQM_ACTION_WIFI_CONNECT_START);      
    
    CJoinAPAsyncParam *pParam = new CJoinAPAsyncParam();
    if(!pParam)
    {
        return E_FAIL;
    }
    pParam->BetweenFunc = between;
    pParam->AfterFunc= after;
    pParam->m_strSSID = strSSID;
    pParam->m_strPassword = strPassword;
    pParam->m_strIdentity = strIdentity;
    pParam->m_strDNS = strDNS;
    pParam->m_strGate = strGate;
    pParam->m_strIP = strIP;
    pParam->m_strMask = strMask;
    pParam->m_iSecurityTypes = securityTypes;

    EndAutoJoin();
    EndJoinAP();
    usleep(500000);
    ThreadHelper::CreateThread(&m_pThreadJoinAP, JoinApAsync,(void*)pParam, "WifiManager @ JoinApAsync", true, 51200);
    return S_OK;
}


HRESULT WifiManager::EndJoinAP()
{
    //DebugPrintf(DLC_DIAGNOSTIC,"WifiManager::EndJoinAP() ");
    SystemUtil::ExecuteCommand("killall -9 wpa_supplicant;killall -9 udhcpc");
    if(m_pThreadJoinAP)
    {
        ThreadHelper::CancelThread(m_pThreadJoinAP);
    }
	m_pThreadJoinAP = 0;
	m_eWorking= Nothing;
    WifiManager::GetInstance()->SetConnectedStatus(false);
    return S_OK;
}

HRESULT WifiManager::GetAsyncAPList(AccessPoint*** pppAPList,int *iAPCount)
{
    string ConnectedSSID = CWifiBaseImpl::GetInstance()->FindESSID();
	bool bConnected = CWifiBaseImpl::GetInstance()->IsConnectedtoESSID();

    AccessPoint** tmp = new AccessPoint*[m_iAPCount];
    if(!tmp)
    {
        return E_OUTOFMEMORY;
    }
    for(int i=0;i<this->m_iAPCount;i++)
    {
        tmp[i] = new AccessPoint(*(m_ppAPList[i]));
        if(!tmp[i])
        {
            return E_OUTOFMEMORY;
        }
        if(tmp[i]->SSID.empty())
        {
            continue;
        }
        if(tmp[i]->SSID == ConnectedSSID) //Here we must check it, otherwise it will display wrong/mismatched info on UI
        {
            tmp[i]->fIsConnected= bConnected;
        }
        else
        {
            tmp[i]->fIsConnected= FALSE;
        }
    }
    *iAPCount = m_iAPCount;
    *pppAPList = tmp;
    return S_OK;
}

eWifiManagerWorking WifiManager::GetCurWorking()
{
    return this->m_eWorking;
}
bool WifiManager::IsPowerOn()
{
    bool bPowerOn = CWifiBaseImpl::GetInstance()->IsPowerOn();
    DebugPrintf(DLC_LIUJT, "WifiManager::IsPowerOn return %d", bPowerOn);
    return bPowerOn;
}
bool WifiManager::IsConnected()
{
#ifdef _X86_LINUX
    return true;
#else
    return m_bCurConnectedStatus;
#endif
}

void WifiManager::StartWifiDaemon()
{
	//DebugPrintf(DLC_DIAGNOSTIC, "WifiManager::StartWifiDaemon begin");

	if (m_pThreadDaemon != 0)
	{	
		DebugPrintf(DLC_DIAGNOSTIC, "WifiManager::StartWifiDaemon already started");
		return;
	}

    int err  = ThreadHelper::CreateThread(&m_pThreadDaemon, WifiManager::UpdateStatusThread, (void*)this, "WifiManager @ UpdateStatusThread", true, 51200);
    if ( err != 0)
    {
		m_pThreadDaemon = 0;
    }
	
	DebugPrintf(DLC_DIAGNOSTIC, "WifiManager::StartWifiDaemon end, Err(%d), threadid(%d)", err, m_pThreadDaemon);

}


void WifiManager::StopWifiDaemon()
{
    //DebugPrintf(DLC_DIAGNOSTIC, "WifiManager::StopWifiDaemon begin");
	if(m_pThreadDaemon == 0)
	{
		DebugPrintf(DLC_DIAGNOSTIC, "WifiManager::StopWifiDaemon already stopped");
		return;
	}
	
	ThreadHelper::CancelThread(m_pThreadDaemon);
	
	// detached thread 不需要使用join
	// ThreadHelper::JoinThread(m_pThreadDaemon, NULL);
	m_pThreadDaemon = 0;
	//DebugPrintf(DLC_DIAGNOSTIC, "WifiManager::StopWifiDaemon end");
}

void* WifiManager::UpdateStatusThread(void * pParam)
{
	//DebugPrintf(DLC_DIAGNOSTIC, "WifiManager::UpdateStatusThread");
	
	int count;
    pthread_detach(pthread_self());
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, 0);
    WifiManager* pWiFiManager = (WifiManager*)pParam;
    if(pWiFiManager)
    {
		count = 0;
        while(true)
        {
            pthread_testcancel();
            pWiFiManager->UpdateStatus();
			pthread_testcancel();

			count++;
			if(count < 5)
				sleep(5);
			else
				sleep(30);
        }
    }
    return NULL;
}
void WifiManager::InitConnectedStatus()
{
	bool connected =  CWifiBaseImpl::GetInstance()->IsConnectedtoLAN();

	if (connected)
	{
		int quality =  CWifiBaseImpl::GetInstance()->GetConnectedQuality();
		m_iSignalStrength = QualityToLevel(quality);
		m_bCurConnectedStatus = true;
	}
	else
	{
		m_iSignalStrength = 0;
		m_bCurConnectedStatus = false;
	}


    SNativeMessage msg;
    msg.iType = KMessageWifiChange;
    msg.iParam1 = (UINT32)ALLRECEIVER;
    msg.iParam2 = UPDATE_WIFI_STATUS;
    msg.iParam3 = WIFIAVAILABLE;
    INativeMessageQueue::GetMessageQueue()->Send(msg);
    
    FireWifiAvailableEvent();

    DebugPrintf(DLC_LIUJT, "WifiManager::InitConnectedStatus send wifi available message!!!");

}
void WifiManager::UpdateStatus()
{
    bool bCurrentStatus;
	static bool bNewStatus;
    static int  iCount = 0;

	if (DeviceInfo::IsK4NT() || DeviceInfo::IsK4Touch())//K4上连接WiFi后设置的default网关很快就失效了，原因未明。因此这里针对k4需要更新一下网关。
	{
		CWifiBaseImpl::GetInstance()->UpdateGateway();
	}
	
	bool connected =  CWifiBaseImpl::GetInstance()->IsConnectedtoLAN();

	if (connected)
	{
		int quality =  CWifiBaseImpl::GetInstance()->GetConnectedQuality();
		m_iSignalStrength = QualityToLevel(quality);
		bCurrentStatus = true;
	}
	else
	{
		m_iSignalStrength = 0;
		bCurrentStatus = false;
	}

	
    bNewStatus = bCurrentStatus;

#if 0
    if(false == bCurrentStatus)//如果诊断网络是断开状态，则需多次确认，目前确认3次。
    {
        if(0 == iCount)
        {
            bNewStatus = false;
            iCount++;
            return;
        }
        else if(false == bNewStatus)
        {
            iCount++;
            if(iCount < 3)
            {
                DebugPrintf(DLC_LIUJT, "WifiManager::UpdateStatus return count=%d, bCurrentStatus=%d, bNewStatus=%d", iCount, bCurrentStatus, bNewStatus);
                return;
            }
        }
        else//新状态为连接
        {
            iCount = 0;
        }
    }
    else//新状态为连接
    {
        iCount = 0;
        bNewStatus =  true;
    }
#endif

    DebugPrintf(DLC_LIUJT, "WifiManager::UpdateStatus count=%d, bCurrentStatus=%d, bNewStatus=%d, m_bCurConnectedStatus=%d", iCount, bCurrentStatus, bNewStatus, m_bCurConnectedStatus);

    if( m_bCurConnectedStatus != bNewStatus )
    {
        SNativeMessage msg;
        msg.iType = KMessageWifiChange;
        msg.iParam1 = (UINT32)ALLRECEIVER;
        msg.iParam2 = UPDATE_WIFI_STATUS;

        if(bNewStatus)
        {
                msg.iParam3 = WIFIAVAILABLE;
                DebugPrintf(DLC_LIUJT, "WifiManager::UpdateStatus send wifi available message!!!");
        }
        else
        {
                msg.iParam3 = WIFIUNAVAILABLE;
                DebugPrintf(DLC_LIUJT, "WifiManager::UpdateStatus send wifi unavailable message!!!");
        }
        DebugPrintf(DLC_LIUJT, "WifiManager::UpdateStatus Send KMessageWifiChange message, param1=%d, param2=%d, param3=%d", msg.iParam1, msg.iParam2, msg.iParam3);
        INativeMessageQueue::GetMessageQueue()->Send(msg);

        bNewStatus ? FireWifiAvailableEvent() : FireWifiUnavailableEvent();

        m_bCurConnectedStatus = bNewStatus;
    }
}

void WifiManager::GetWifiInformation(vector<string>& wifistrings)
{
    wifistrings.clear();
    fstream ifs("/tmp/wlan0");
    string str;

    while (ifs.good())
    {
        string sOldStr(str);
        DebugPrintf(DLC_LIUJT, "WifiManager::GetWifiInformation ing...");
        ifs >> str;
        if(str != sOldStr)
        {
            DebugPrintf(DLC_LIUJT, "WifiManager::GetWifiInformation old wifi=%s, new wifi=%s",
                sOldStr.c_str(), str.c_str());
        }
        wifistrings.push_back(str);
    }

    ifs.close();
}


bool WifiManager::FireWifiEvent(const char* eventName, WifiEventArgs& args)
{
    FireEvent(eventName, (EventArgs&)args);
    return true;
}

bool WifiManager::FireWifiAvailableEvent()
{
    WifiEventArgs args(ALLRECEIVER, UPDATE_WIFI_STATUS, WIFIAVAILABLE);
    return FireWifiEvent(WifiAvailableEvent, args);
}

bool WifiManager::FireWifiUnavailableEvent()
{
    WifiEventArgs args(ALLRECEIVER, UPDATE_WIFI_STATUS, WIFIUNAVAILABLE);
    return FireWifiEvent(WifiUnavailableEvent, args);
}

/*
bool WifiManager::FireWifiAutoJoinEvent()
{

}

bool WifiManager::FireWifiRefreshEvent()
{
    WifiEventArgs args(ALLRECEIVER, AUTO_REFRESH, DEFAULT);
    return FireWifiEvent(WifiRefreshEvent, args);
}

bool WifiManager::FireWifiShowRefreshTipEvent()
{
    WifiEventArgs args(ALLRECEIVER, AUTO_REFRESH_SHOWTIP, DEFAULT);
    return FireWifiEvent(WifiShowRefreshTipEvent, args);
}
*/

int WifiManager::GetWiFiStatus()
{
	return m_wifi_status;
}

void  WifiManager::SetWiFiStatus(int status)
{
	m_wifi_status = status;	
}

void  WifiManager::SetConnectedStatus(bool bStatus)
{    
	m_bCurConnectedStatus = bStatus;    
}

bool WifiManager::IsManualJoinAP()
{
    return (m_pThreadJoinAP != 0);
}

