#include <sys/types.h> 
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <string>
#include <vector>
#include <net/if.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include "Wifi/WifiBaseImpl.h"
#include "interface.h"
#include "dkWinError.h"
#include "interface.h"
#include "Utility/SystemUtil.h"
#include "Utility/EncodeUtil.h"


using dk::utility::SystemUtil;
using dk::utility::EncodeUtil;

using namespace std;

#define SYSTEMCALLOUTPUTLENGTH 2048
#define BAIDU                                "www.baidu.com"
#define WPA_FILE_NAME                    "/tmp/wpa.conf"
#define AUTO_JOIN_FILE_NAME       "/mnt/us/wifi.log"

#define WIFI_OFF_COMMOND            "/DuoKan/wifi stop"
#define WIFI_ON_COMMOND             "/DuoKan/wifi start"

#define KINDLE_WIFI_NAME             "wlan0"
#define KINDLE_RESOLV_CONF         "/tmp/resolv.conf"
#define WIFI_LIST_FILE			"/tmp/wifilist"

#define CONNECT_WAIT_TIME          5
#define CONNECT_WAIT_TIME_MAX      300

SINGLETON_CPP(CWifiBaseImpl);

CWifiBaseImpl::CWifiBaseImpl()
{
    m_strAdapterName = KINDLE_WIFI_NAME;

	m_strGateway.clear();
    
    m_fAutoDoing = false;
    m_CurAP.ClearContents();
    
	pthread_mutex_init(&mutex_wifilist, NULL);
}
CWifiBaseImpl::~CWifiBaseImpl()
{
	pthread_mutex_destroy(&mutex_wifilist);
}




/******************************
*
*   从单一AP字符串得到AP结构  
*   这个函数得到的可能不准确
*
*******************************/
void CWifiBaseImpl::GetAPFromString(string& APString,AccessPoint** pAP)
{
    AccessPoint *p =new  AccessPoint();

    if(NULL == p)
    {
        *pAP = NULL;
        return;
    }


    //SSID
    int start =  APString.find("ESSID:\"");
    int end = APString.find("\"\n");
    p->SSID = APString.substr(start+7,end-start-7);
	p->SecurityType = RESET;

    //是否需要密钥
    if(APString.find("Encryption key:on",0)  != string::npos)
    {
        p->fPublic = false;
    }

    if(APString.find("Encryption key:off",0)  != string::npos)
    {
        p->fPublic = true;
        p->SecurityType = NONE;
    }

    //加密方式
    if(APString.find("IE: WPA Version") != string::npos)
    {
        p->SecurityType = WPA;
    }

    if(APString.find("IE: IEEE 802.11i/WPA2 Version") != string::npos)
    {
        p->SecurityType = WPA2;
    }
        
    if(APString.find(": 802.1x") != string::npos)
    {
        p->SecurityType = IEEE8021X;
    }
    
    if(false == p->fPublic && p->SecurityType == RESET)
    {
        p->SecurityType = (eSecurityType)(WEP_OPEN | WEP_SHARE);
    }
    
    //信号
    start = APString.find("Quality=");
    start += 8;
    end = APString.find("/",start);
    if(end - start <=3)
    {
        string level = APString.substr(start ,end - start);
        p->Quality = atoi(level.c_str());
    }
    else
    {
        p->Quality = 0;
    }

    p->fIsConnected = false;
    *pAP = p;

}
/******************************
*
*   将字符串拆分为单个AP的字符串
*
*******************************/
void CWifiBaseImpl::GetAPListFromString(string& APListString,AccessPoint*** rgAPs,int * APCount)
{
    if( NULL == APCount || NULL == rgAPs || APListString.size() == 0)
    {
        return;
    }
    
    unsigned int start = 0;
    vector<int> indexs;
    while(1)
    {
        start = APListString.find("Cell",start+1);
        if(start == string::npos)
        {
            indexs.push_back(APListString.size() + 1);
            break;
        }
        if(APListString.find("Address",start) == start + 10)//确认是不是一个新的AP
        {
            indexs.push_back(start);
        }
    }
    int size = indexs.size();
    *APCount = size - 1;
    if (rgAPs != NULL)
    {
        *rgAPs = new AccessPoint*[size - 1];
        if(NULL == rgAPs)
        {
            return;
        }
        memset(*rgAPs, 0, (size - 1) * sizeof(AccessPoint*));
        for(int i=1;i<size;i++)
        {
            string tmp = APListString.substr(indexs[i-1],indexs[i]-indexs[i-1]);
            GetAPFromString(tmp, &((*rgAPs)[i-1]));
        }
    }
}


void CWifiBaseImpl::RemoveAndSort(AccessPoint*** rgpAPs,int *iCount)
{

}

HRESULT CWifiBaseImpl::Refresh()
{	
	//DebugPrintf(DLC_DIAGNOSTIC,"CWifiBaseImpl::Refresh().");
    string cmd = "iwlist " + m_strAdapterName + " scanning > ";
    cmd.append(WIFI_LIST_FILE);
	SystemUtil::ExecuteCommand(cmd.c_str());	
	return S_OK;
}

/******************************
*
*   获取AP列表
*
*******************************/
HRESULT CWifiBaseImpl::GetAPList(AccessPoint*** pppAPList,int *iAPCount)
{
    //DebugPrintf(DLC_DIAGNOSTIC, "CWifiBaseImpl::GetAPList() begin");

    if(m_strAdapterName.size() == 0 || pppAPList == NULL || iAPCount == NULL)
    {
        DebugPrintf(DLC_DIAGNOSTIC, "CWifiBaseImpl::GetAPList() bad parameter, end.");
        return E_FAIL;
    }

    if (pthread_mutex_trylock(&mutex_wifilist) != 0) 
	{
        DebugPrintf(DLC_DIAGNOSTIC, "CWifiBaseImpl::GetAPList() lock failed, end.");
        return E_FAIL;
	}

    string cmd = "";
    cmd.append("iwlist ");
    cmd.append(m_strAdapterName);
    cmd.append(" scanning");

    string APs;
    FILE   *stream = NULL; 
    char   buf[1024]; 
    int size = -1;	
	int iwlistCount = 2;
		
	while(iwlistCount--)
	{
		memset( buf, 0, sizeof(buf));
		stream = popen(cmd.c_str(), "r" ); 
		if (NULL == stream)
		{
            usleep(500000);
			continue;
		}
		
		while((size = fread( buf, 1, sizeof(buf) - 1, stream)) > 0)
		{
			APs.append(buf);
			memset( buf, 0, sizeof(buf));
		}
		
		pclose( stream ); 
        usleep(200000);		
	}

	if (APs.find("Cell", 0 ) == string::npos) 
	{	
        DebugPrintf(DLC_DIAGNOSTIC, "CWifiBaseImpl::GetAPList() iwlist wlan0 scanning,  find no Cell, end.");
        pthread_mutex_unlock(&mutex_wifilist);
        return E_FAIL;
	}

    GetAPListFromString(APs,pppAPList ,iAPCount);

    //DebugPrintf(DLC_DIAGNOSTIC, "CWifiBaseImpl::GetAPList() , *iAPCount = %d Cell", *iAPCount);

    if (0 == *iAPCount)
    {
        pthread_mutex_unlock(&mutex_wifilist);
        return S_OK;
    }

    AccessPoint **ppAPs = *pppAPList;
    int RefactorAPCount = *iAPCount;
    int j = 0;
    for(int i =0; i<*iAPCount-1; i++)
    {
        AccessPoint * ap = ppAPs[i];
        if (NULL == ap)
        {
            continue;
        }
        for(j = i+1; j<*iAPCount; j++)
        {
            AccessPoint * apLater = ppAPs[j];
            if (NULL == apLater)
            {
                continue;
            }   
            if((ap->SSID == apLater->SSID) && ap->Quality != -1 && apLater->Quality != -1)
            {
                if(ap->Quality <= apLater->Quality)
                {
                    ap->Quality = -1;
                }
                else
                {
                    apLater->Quality = -1;
                }
                RefactorAPCount--;
            }
        }
    }
    
    AccessPoint *tmp = NULL;
    for(int i =0; i<*iAPCount-1; i++)
    {
        for(j = 0; j<*iAPCount - i -1; j++)
        {
            if (NULL == ppAPs[j] || NULL == ppAPs[j+1])
            {
                continue;
            }
            
            if(ppAPs[j]->Quality < ppAPs[j+1]->Quality)
            {
                tmp = ppAPs[j+1];
                ppAPs[j+1] = ppAPs[j];
                ppAPs[j] = tmp;
            }
        }       
    }

    for(int i=RefactorAPCount;i<*iAPCount;i++)
    {
        delete ppAPs[i];
        ppAPs[i] = NULL;
    }

    //DebugPrintf(DLC_DIAGNOSTIC, "CWifiBaseImpl::GetAPList() , RefactorAPCount = %d Cell.", RefactorAPCount);

	// 当扫描到的AP数量过多时，过滤掉信号过弱的AP (-80 dBm to -94 dBm)
	int iAPweak = 0;
	if (RefactorAPCount > 5)
	{
		for(int i = 5; i < RefactorAPCount; i++)
		{
			if (ppAPs[i]->Quality <= 15)
			{
				iAPweak++;
				delete ppAPs[i];
				ppAPs[i] = NULL;
			}
		}
	}

	RefactorAPCount -= iAPweak;

	if(iAPweak != 0)
	{
		DebugPrintf(DLC_DIAGNOSTIC, "CWifiBaseImpl::GetAPList() , delete (%d) weak Cell.", iAPweak);
	}
    
    *iAPCount = RefactorAPCount;

    AccessPoint * CurAP = GetCurAP();
    if(NULL == CurAP || CurAP->SSID.empty())
    {
        pthread_mutex_unlock(&mutex_wifilist);	
        DebugPrintf(DLC_DIAGNOSTIC, "CWifiBaseImpl::GetAPList() CurAP empty, end");
        return S_OK; //没有链接就不用改变链接属性了
    }

    for(int i=0 ;i<*iAPCount;i++)
    {
        if (NULL == ppAPs[i] || ppAPs[i]->SSID.empty())
        {
            continue;
        }
        if(ppAPs[i]->SSID == CurAP->SSID)
        {
			// 修复 bug： 未连接的AP， 显示为已连接
            //DebugPrintf(DLC_DIAGNOSTIC, "CWifiBaseImpl::GetAPList() CurAP(%s),connected(%d)", CurAP->SSID.c_str(), (int)(CurAP->fIsConnected));
            ppAPs[i]->fIsConnected = CurAP->fIsConnected;
        }
        else
        {
            ppAPs[i]->fIsConnected = false;
        }
    }

	pthread_mutex_unlock(&mutex_wifilist);
	//DebugPrintf(DLC_DIAGNOSTIC, "CWifiBaseImpl::GetAPList() end");
    return S_OK;
}


/******************************
*
*   检测当前连接状态
*
*******************************/

bool CWifiBaseImpl::IsConnected(BOOL bNeedPing)
{
    
    AccessPoint* AP = GetCurAP(bNeedPing);

	if(AP == NULL)
	{
		return false;
	}

	if(!AP->fIsConnected) 
	{
		return false;
	}

	if(AP->Quality == 0)
	{
		return false;
	}

	if(AP->SSID.empty())
	{
		return false;
	}

	if (FindIP().empty())
	{
		return false;
	}

	return true;
}


/******************************
*
*   尝试连接AP 
*
*******************************/
bool CWifiBaseImpl::TryConnectAP(string SSID,string Password, eSecurityType SecurityType, int timeout, string Identity)
{
    //DebugPrintf(DLC_DIAGNOSTIC, "CWifiBaseImpl::TryConnectAP begin.");

	std::string ssid = "\"";
	ssid.append(SSID.c_str());
	ssid.append("\"");

    int iCount = 0;
    do
    {
        SystemUtil::ExecuteCommand(
                ("killall -9 wpa_supplicant ; "
                "killall -9 udhcpc ; killall -9 iwconfig ; "
                "iwconfig " +m_strAdapterName + " key off ;" + 
                "iwconfig " +m_strAdapterName + " essid off ;"
                ).c_str());
		// WiFi: sleep 500毫秒，  已等待iwconfig 写入网卡生效, 否则wep share方式易出错
		usleep(500000);
    } while(IsConnectedtoESSID() && ++iCount <= 3 );

    //DebugPrintf(DLC_DIAGNOSTIC, "CWifiBaseImpl::TryConnectAP Closing active wifis DONE");
    
    //判断类别进行连接
    if(SecurityType == NONE)
    {
        if(Password.size() > 0)
        {
            return false;
        }
        SystemUtil::ExecuteCommand(("iwconfig " +m_strAdapterName +" essid " + ssid).c_str());
		//DebugPrintf(DLC_DIAGNOSTIC, "%s", ("iwconfig " +m_strAdapterName +" essid " + ssid).c_str());
    }
    else if(SecurityType  == WEP_OPEN)
    {
        int pwdsize = Password.size();
        if(pwdsize == 5 || pwdsize == 13 || pwdsize == 16)
        {
            SystemUtil::ExecuteCommand(
            ("iwconfig " +m_strAdapterName +" key s:" +Password
            +" ; iwconfig " +m_strAdapterName +" key open"
            +" ; iwconfig " +m_strAdapterName +" essid "+ ssid).c_str());
        }
        else if(pwdsize == 10 || pwdsize == 26 || pwdsize == 32)
        {
            SystemUtil::ExecuteCommand(
            ("iwconfig " +m_strAdapterName +" key " +Password
            +" ; iwconfig " +m_strAdapterName +" key open"
            +" ; iwconfig " +m_strAdapterName +" essid "+ ssid).c_str());
        }
        else
        {
            return false;
        }
        
    }
    else if(SecurityType == WEP_SHARE)
    {
        int pwdsize = Password.size();
        if(pwdsize == 5 || pwdsize == 13 || pwdsize == 16)
        {
            SystemUtil::ExecuteCommand(("iwconfig " +m_strAdapterName +" key restrict s:" +Password
            + " ; iwconfig " +m_strAdapterName +" essid "+ ssid ).c_str());
        }
        else if(pwdsize == 10 || pwdsize == 26 || pwdsize == 32)
        {
            SystemUtil::ExecuteCommand(("iwconfig " +m_strAdapterName +" key restrict " +Password
            + " ; iwconfig " +m_strAdapterName +" essid "+ ssid).c_str());
        }
        else
        {
            return false;
        }
    }
    else if(SecurityType== WPA  || SecurityType == WPA2)
    {
        int pwdsize = Password.size();
        if(pwdsize < 8 || pwdsize > 64)
        {
            return false;
        }
        string filetext;
        filetext.append("network={\n scan_ssid=1\n ssid=" + ssid + "\npsk=\""+Password+"\"\n}\n");
        FILE *fp = fopen(WPA_FILE_NAME,"w+");
        if(!fp)
        {
            DebugPrintf(DLC_LIUJT, "Failed to open file: %s", WPA_FILE_NAME);
            return false;
        }
        fwrite(filetext.c_str(),1,filetext.size(),fp);
        fclose(fp);

		string wpa_log = "/mnt/us/DK_System/xKindle/wifi.log";
		string wpa_cmd = "wpa_supplicant -i"+m_strAdapterName+" -c"+WPA_FILE_NAME+" -B" ;
        SystemUtil::ExecuteCommand("export LD_LIBRARY_PATH=");
        SystemUtil::ExecuteCommand((wpa_cmd + " >> " + wpa_log + " 2>&1" ).c_str());
    }
    else if (SecurityType == IEEE8021X)
    {   
        int pwdsize = Password.size();
        if(pwdsize < 4 || pwdsize > 64)    // CMCC use 6 chars passwd, some  may use 4-digit PIN
        {
            return false;
        }
        if(Identity.size() == 0)
        {
            return false;
        }
        string filetext;
        filetext.append("network={\n ssid=" + ssid + "\n identity=\"" + Identity + "\"\n password=\"" + Password \
            + "\"\n key_mgmt=IEEE8021X\n eap=PEAP\n phase1=\"peapver=0\"\n phase2=\"auth=MSCHAPV2\" \n}\n");
        FILE *fp = fopen(WPA_FILE_NAME,"w+");
        if(!fp)
        {
            DebugPrintf(DLC_LIUJT, "Failed to open file: %s", WPA_FILE_NAME);
            return false;
        }
        fwrite(filetext.c_str(),1,filetext.size(),fp);
        fclose(fp);

		string wpa_log = "/mnt/us/DK_System/xKindle/wifi.log";
		string wpa_cmd = "wpa_supplicant -i"+m_strAdapterName+" -c"+WPA_FILE_NAME+" -B" ;
        SystemUtil::ExecuteCommand("export LD_LIBRARY_PATH=");
        SystemUtil::ExecuteCommand((wpa_cmd + " >> " + wpa_log + " 2>&1" ).c_str());

    }

    bool connected =false;
	if (timeout <= 0  || timeout > CONNECT_WAIT_TIME_MAX) 
	{
		timeout = CONNECT_WAIT_TIME_MAX;
	}

	int passwd_err = 0;
    for(int i = 0; i < timeout; i++)
	{
		usleep(1000000);
				
        connected = IsConnectedtoESSID(SecurityType);
        if(connected)
        {
			if(SSID != FindESSID())
				connected = false;
            //DebugPrintf(DLC_DIAGNOSTIC, "CWifiBaseImpl::TryConnectAP, Connect ssid(%s) type (%d) connected(%d)", SSID.c_str(), SecurityType, (int)connected);
            return connected;
        }

		if (IsPasswordError(SecurityType)) 
		{
			passwd_err++;
		}

		if (passwd_err >= 6)
		{
			DebugPrintf(DLC_DIAGNOSTIC, "CWifiBaseImpl::TryConnectAP, ssid(%s) type (%d) password error!", SSID.c_str(), SecurityType);
            return false;
		}
    }

    DebugPrintf(DLC_DIAGNOSTIC, "CWifiBaseImpl::TryConnectAP, Connect ssid(%s) type (%d) failed!", SSID.c_str(), SecurityType);
    return false;
}

bool CWifiBaseImpl::IsPingHostOK(std::string IP, int port, int mstimeout)
{
    //DebugPrintf(DLC_LIUJT, "CWifiBaseImpl::IsPingHostOK entering");
    struct wlantool_value edata;
    memset(&edata, 0, sizeof(edata));  
    
    int fd = -1, err = 0;
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, "wlan0");
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
            //DebugPrintf(DLC_LIUJT, "CWifiBaseImpl::IsPingHostOK Cannot get control socket");
            return FALSE;
    }

    edata.cmd = 0x0000000a;
    ifr.ifr_data = (caddr_t)&edata;
    err = ioctl(fd, 0x8B18, &ifr);//0x8946

    if(0 != err)
    {
        //DebugPrintf(DLC_LIUJT, "CWifiBaseImpl::IsPingHostOK Cannot get link status, error code=%d", err);
        return FALSE;
    }

    struct sockaddr_in destAddr;
    memset(&destAddr, 0, sizeof(destAddr));
    
    destAddr.sin_family = AF_INET;
    destAddr.sin_addr.s_addr = inet_addr(IP.c_str());
    destAddr.sin_port = htons(port);
   
    int iFailedCount = 0;

	// 50 - 500 ms
	if (mstimeout < 50)
	{
		mstimeout = 50;
	}
	if (mstimeout > 500) 
	{
		mstimeout = 500;
	}

	//  try connect 4 times， if error <=2 , connect ok
    for(int i = 0; i < 4; i++)
    {
        err = Async_connect(fd, destAddr, mstimeout);
        if(err)
        {
            iFailedCount++;
        }
    }

    close(fd); 

    return iFailedCount <= 2;
}

string CWifiBaseImpl::GetSystemCallOutput(string cmd)
{
    DebugPrintf(DLC_LIUJT, "CWifiBaseImpl::GetSystemCallOutput entering with cmd=%s", cmd.c_str());
    if(cmd.empty())
    {
        DebugPrintf(DLC_LIUJT, "CWifiBaseImpl::GetSystemCallOutput return with empty cmd=%s", cmd.c_str());
        return "";
    }
    
    FILE* file;
    file = popen(cmd.c_str(), "r");
    if(NULL == file)
    {
        DebugPrintf(DLC_LIUJT, "CWifiBaseImpl::GetSystemCallOutput return with invalid cmd=%s", cmd.c_str());
        return "";
    }
   char buf[SYSTEMCALLOUTPUTLENGTH] = {0};

   fread(buf, 1,sizeof(buf)-1, file);
   pclose(file);
   return string(buf);;

}

int  CWifiBaseImpl::Async_connect(int soc, struct sockaddr_in &addr, int ms)
{
    int res = 0;
    long arg = 0;
    socklen_t lon;
    int valopt;
    fd_set myset;
    struct timeval tv; 

    if(ms > 1000)
        ms = 999;

    // Set non-blocking
    if( (arg = fcntl(soc, F_GETFL, NULL)) < 0) {
        DebugPrintf(DLC_LIUJT, "WifiBaseImpl::Async_connect fcntl failed: %u", __LINE__);
        return arg;
    }   
    arg |= O_NONBLOCK;
    if( fcntl(soc, F_SETFL, arg) < 0) {
        DebugPrintf(DLC_LIUJT, "WifiBaseImpl::Async_connect fcntl failed: %u", __LINE__);
        return -1; 
    }   

    // Trying to connect with timeout
    res = connect(soc, (struct sockaddr *)&addr, sizeof(addr));
    if (res < 0) {
        if (errno == EINPROGRESS) {
            tv.tv_sec = 0 ; 
            tv.tv_usec = ms * 1000;
            FD_ZERO(&myset);
            FD_SET(soc, &myset);
            res = select(soc+1, NULL, &myset, NULL, &tv);
            if (res < 0 && errno != EINTR) {
                DebugPrintf(DLC_LIUJT, "WifiBaseImpl::Async_connect Error connecting %d - %s/n", errno, strerror(errno));
                return -1; 
            }   
            else if (res > 0) {
                // Socket selected for write
                lon = sizeof(int);
                if (getsockopt(soc, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0) {
                    DebugPrintf(DLC_LIUJT, "WifiBaseImpl::Async_connect Error in getsockopt() %d - %s/n", errno, strerror(errno));
                    return -1; 
                }   
                // Check the value returned...
                if (valopt) {
                    DebugPrintf(DLC_LIUJT, "WifiBaseImpl::Async_connect Error in delayed connection() %d - %s/n", valopt, strerror(valopt) );
                    return -1; 
                }   

                //else success
            }   
            else {
                DebugPrintf(DLC_LIUJT, "WifiBaseImpl::Async_connect Timeout in select() - Cancelling!/n");
                return -1; 
            }   
        }   //end if
        else {
            return -1; 
        }   
    }// end if(res < 0). success
    
    // Set to blocking mode again...
    if( (arg = fcntl(soc, F_GETFL, NULL)) < 0) {
        DebugPrintf(DLC_LIUJT, "WifiBaseImpl::Async_connect Error fcntl(..., F_GETFL) failed: %u", __LINE__);
        return -1;
    }
    arg &= (~O_NONBLOCK);
    if( fcntl(soc, F_SETFL, arg) < 0) {
        DebugPrintf(DLC_LIUJT, "WifiBaseImpl::Async_connect Error fcntl(..., F_SETFL) failed: %u", __LINE__);
        return -1;
    }

    return 0;
}

/******************************
*
*   设置IP 传入IP则设置为传入IP 否则是使用DHCP进行IP获取 
*   
*******************************/
eIPStatus CWifiBaseImpl::SetIP(string IP,string Mask,string Gate,string DNS)
{
	eIPStatus result;
	string Mac;	
	string dhcps;
	string dhcpoffer;
	string::size_type start;
	const char *strok ="obtained";
	const char *strnodns ="without DNS"; 
    const char *stroffer ="Offer from server";
    //const char *strfailing ="No lease, failing";
	char buff[1024];
	FILE *fp = NULL;
	bool dhcpok;
	string dhcp_cmd = "udhcpc -t 3 -n -i "+m_strAdapterName;
	int icount;
	
	//DebugPrintf(DLC_DIAGNOSTIC,"CWifiBaseImpl::SetIP: IP=(%s), Mask=(%s), Gate=(%s), DNS=(%s)", IP.c_str(), Mask.c_str(), Gate.c_str(), DNS.c_str()); 

	// static IP
	if(!IP.empty())
	{
		SystemUtil::ExecuteCommand(("ifconfig "+m_strAdapterName+" "+IP+" netmask "+ Mask).c_str());
		UpdateGateway(Gate);
		// SystemUtil::ExecuteCommand(("echo nameserver "+DNS+" > /etc/resolv.d/resolv.conf.default").c_str());
		SystemUtil::ExecuteCommand(("echo nameserver "+DNS+" > /var/run/resolv.conf").c_str()); 
		result = DHCP_STAITC_IP;
		goto _setip_result;
	}	

	SystemUtil::ExecuteCommand(("echo > /tmp/"+ m_strAdapterName).c_str()); 

	//DebugPrintf(DLC_DIAGNOSTIC,"CWifiBaseImpl::SetIP cmd (%s)", dhcp_cmd.c_str());

	result = DHCP_NO_LEASE;
	dhcpok = false;
	icount = 0;
	while ( icount < 5)
	{
		icount++;
		fp = popen(dhcp_cmd.c_str(),"r");
		if (fp == NULL)
		{
			//DebugPrintf(DLC_DIAGNOSTIC,"CWifiBaseImpl::SetIP cmd (popen NULL %d)", icount);
			usleep(100000);
			continue;
		}

		memset(buff,0,sizeof(buff));
		fread(buff,1,sizeof(buff)-1,fp);
		pclose(fp);
		dhcps.assign(buff);

		// 分配成功，立刻跳出
		if (dhcps.find(strok, 0 ) != string::npos)
		{
			result = DHCP_OK;
			dhcpok = true;
			break;
		}
		
		// 分配不成功，至少尝试两次， 才考虑处理如下错误
		if (icount <= 2)
		{
			continue;
		}
		
		// 无DNS， udhcpc返回错误
		if (dhcps.find(strnodns, 0 ) != string::npos)
		{
			result = DHCP_NO_DNS;
			dhcpok = false;
			break;
		}
		
		// 优化判断： 如果已经尝试两次dhcp, WiFi信号太弱，dhcp过程中，发现已经掉线，则立刻返回
		if (!IsConnectedtoESSID())
		{
			result = DHCP_CONNECT_LOST;
			dhcpok = false;
			break;
		}

		// dhcp no leasing， 未分配到IP， 可继续重试 
		if (dhcps.find(strnodns, 0 ) != string::npos)
		{
			result = DHCP_NO_LEASE;
			dhcpok = false;
			continue;
		}

		// dhcp offer 错误
		start = dhcps.find(stroffer, 0);
		if (start != string::npos)
		{
			dhcpoffer = dhcps.substr(start, dhcps.length() - start);
			result = DHCP_OFFER_WRONG;
			dhcpok = false;
			break;
		}
	}

	if(!dhcpok)
	{
		goto _setip_result;
	}

	Gate = FindGateWay();
	if(Gate.empty())
	{		
		result = DHCP_NO_GATEWAY;
		goto _setip_result;
	}

	UpdateGateway(Gate);
	SystemUtil::ExecuteCommand("cat /tmp/resolv.conf > /var/run/resolv.conf ");

	IP = FindIP();
	Mac = FindMacAddr();

	if (IP.empty())
	{
		result = DHCP_OFFER_WRONG;
		DebugPrintf(DLC_DIAGNOSTIC,"CWifiBaseImpl::SetIP  (IP NULL)");
	}

_setip_result:

	switch(result) 
	{
	case DHCP_STAITC_IP:
		DebugPrintf(DLC_DIAGNOSTIC,"CWifiBaseImpl::SetIP (Static IP)");
		break;
	case DHCP_OK:
		DebugPrintf(DLC_DIAGNOSTIC, "CWifiBaseImpl::SetIP (DHCP_OK) IP(%s),GW(%s), Mac(%s).", IP.c_str(), Gate.c_str(), Mac.c_str());
		break;
	case DHCP_CONNECT_LOST:
		DebugPrintf(DLC_DIAGNOSTIC,"CWifiBaseImpl::SetIP weak wifi (DHCP_CONNECT_LOST)");
		break;
	case DHCP_NO_DNS:
		DebugPrintf(DLC_DIAGNOSTIC,"CWifiBaseImpl::SetIP failed(DHCP_NO_DNS)");
		break;
	case DHCP_NO_GATEWAY:
		DebugPrintf(DLC_DIAGNOSTIC,"CWifiBaseImpl::SetIP failed(DHCP_NO_GATEWAY)");
		break;
	case DHCP_NO_LEASE:
		DebugPrintf(DLC_DIAGNOSTIC,"CWifiBaseImpl::SetIP failed(DHCP_NO_LEASE)");
		break;
	case DHCP_OFFER_WRONG:
		DebugPrintf(DLC_DIAGNOSTIC,"CWifiBaseImpl::SetIP (DHCP_OFFER_WRONG):%s", dhcpoffer.c_str());
		break;
	}

	return result;
}

void CWifiBaseImpl::UpdateGateway()
{
	if(!m_strGateway.empty())
	{
		system(("route add default gw " + m_strGateway).c_str());
	}
}

void CWifiBaseImpl::UpdateGateway(std::string GateWay)
{
	if (!GateWay.empty()) 
	{
		m_strGateway = GateWay;
	}

	if(!m_strGateway.empty())
	{
		system(("route add default gw " + m_strGateway).c_str());
	}

	DebugPrintf(DLC_DIAGNOSTIC, "CWifiBaseImpl::UpdateGateway (%s) End",  m_strGateway.c_str());
}

/******************************
*
*   公开函数 加入AP
*   
*******************************/

HRESULT CWifiBaseImpl::JoinAP(string strSSID)
{
    return JoinAP(strSSID,"", "",NONE);
}
HRESULT CWifiBaseImpl::JoinAP(string strSSID,string strPassword, string strIdentity, unsigned int securityType,string strIP,string strMask,string strGate,string strDNS)
{
	string pwdhex = "";	
	if (strPassword.length() != 0 )
	{
		pwdhex = EncodeUtil::BinToHex( (const unsigned char*)strPassword.c_str(), strPassword.length());
	}

	DebugPrintf(DLC_DIAGNOSTIC,"WifiBaseImpl::JoinAP,ssid(%s),(%02d%s), Type(%d)", strSSID.c_str(), pwdhex.length(), pwdhex.c_str(), securityType);

	bool connected = false;
    eSecurityType thistype = NONE;
    eSecurityType ssidtype = RESET;
	int timeout = CONNECT_WAIT_TIME;
	int timeout_wpa = CONNECT_WAIT_TIME * 2;    
	int timeout_8021x = 120;
	int iscan = 0;
	
	// 扫描获取加密类型， 避免盲目穷举测试； 最多尝试扫描5次

	while (iscan++ < 5)
	{
		if (GetSecurityTypeFromSSID(strSSID, &ssidtype))
		{
			securityType = ssidtype;
			timeout = CONNECT_WAIT_TIME * 2;
			timeout_wpa = CONNECT_WAIT_TIME * 4;
			break;
		}
		usleep(100000);
	}

	// 发现TPLINK WR-2543N 路由器加密方式广播错误，有时候设置为WPA2方式，扫描出来是WEP， PC上也如此
	// 因此， 如果密码不符合WEP方式， 让它尝试WPA连接
	if ((securityType & WEP_SHARE) == WEP_SHARE) 
	{
		if (!isWEPPassword(strPassword))
		{
			securityType |= WPA;
		}
	}
	
	DebugPrintf(DLC_DIAGNOSTIC,"CWifiBaseImpl::GetSecurityTypeFromSSID(), iscan(%d), ssidtype(%d)" , iscan, ssidtype);

	m_CurAP.ClearContents();

    if(!connected && ((securityType & WPA) == WPA  || (securityType & WPA2) == WPA2))
    {
        connected = TryConnectAP(strSSID,strPassword,WPA, timeout_wpa);
        DebugPrintf(DLC_DIAGNOSTIC,"Join WPA/WPA2, connected=%d", connected);
        if(connected)
        {
            thistype = ((securityType & WPA2) == WPA2 ) ? WPA2 : WPA;
        }
    }
	
    if(!connected && ( (securityType & NONE) == NONE || ( securityType & UNKNOWN_WIFI_SECURITY_TYPE) == UNKNOWN_WIFI_SECURITY_TYPE))
    {
        connected = TryConnectAP(strSSID,strPassword,NONE, timeout);
        DebugPrintf(DLC_DIAGNOSTIC,"Join NONE, connected=%d", connected);
        if(connected)
        {
            thistype = NONE;
        }
    }

	if(!connected && ((securityType & WEP_SHARE) == WEP_SHARE))
	{
		connected = TryConnectAP(strSSID,strPassword,WEP_SHARE, timeout);
		DebugPrintf(DLC_DIAGNOSTIC,"Join SHARE, connected=%d", connected);
		if(connected)
		{
			thistype = WEP_SHARE;
		}
	}

    if(!connected && ((securityType & WEP_OPEN) == WEP_OPEN))
    {
        connected = TryConnectAP(strSSID,strPassword,WEP_OPEN, timeout);
        DebugPrintf(DLC_DIAGNOSTIC,"Join OPEN, connected=%d", connected);
        if(connected)
        {
            thistype = WEP_OPEN;
        }
    }

    if(!connected && ((securityType & IEEE8021X) == IEEE8021X))
    {
        connected = TryConnectAP(strSSID,strPassword, IEEE8021X, timeout_8021x, strIdentity);
        DebugPrintf(DLC_DIAGNOSTIC,"Join IEEE8021X, Identity(%s), connected=%d", connected, strIdentity.c_str());
        if(connected)
        {
            thistype = IEEE8021X;
        }
    }

    
	if(!connected) 
	{
		m_CurAP.fIsConnected = false;
		DebugPrintf(DLC_DIAGNOSTIC,"Join failed, SSID(%s), securityType(%d)", strSSID.c_str(), securityType);
		return E_FAIL;
	}
	
	m_CurAP.SSID = FindESSID();
	m_CurAP.Quality = GetConnectedQuality();

	m_CurAP.SecurityType = thistype;
	m_CurAP.fPublic = (thistype == NONE);
	m_CurAP.fIsConnected = true;
	
	if(strSSID != m_CurAP.SSID) 
	{
		DebugPrintf(DLC_DIAGNOSTIC,"JoinAP, BUG! strSSID(%s), FindESSID(%s)", strSSID.c_str(), m_CurAP.SSID.c_str());
	}

	DebugPrintf(DLC_DIAGNOSTIC,"JoinAP OK, SSID(%s),securityType(%d), Link Quality(%d)", strSSID.c_str(), thistype, m_CurAP.Quality);

	return S_OK;
}

/******************************
*
*   公开函数 开启wifi网卡加载驱动
*   
*******************************/
HRESULT CWifiBaseImpl::PowerOn()
{
	DebugPrintf(DLC_DIAGNOSTIC,"CWifiBaseImpl::PowerOn().");

    int result = SystemUtil::ExecuteCommand(("killall -9 wpa_supplicant ; killall -9 udhcpc; killall -9 iwconfig; "+string(WIFI_ON_COMMOND)).c_str());
	
	DebugPrintf(DLC_DIAGNOSTIC, "CWifiBaseImpl::PowerOn(), lsmod:\n%s",
		dk::utility::SystemUtil::ReadStringFromShellOutput("lsmod").c_str());
	DebugPrintf(DLC_DIAGNOSTIC, "CWifiBaseImpl::PowerOn(), wlan0:\n%s",
		dk::utility::SystemUtil::ReadStringFromShellOutput("ifconfig wlan0").c_str());
	
    if (0 == result)
    {
        return S_OK;
    }
    else
    {
        return E_FAIL;
    }
}
/******************************
*
*   公开函数 关闭wifi网卡卸载驱动
*   
*******************************/
HRESULT CWifiBaseImpl::PowerOff()
{
    DebugPrintf(DLC_DIAGNOSTIC,"CWifiBaseImpl::PowerOff().");

    int result = SystemUtil::ExecuteCommand( ("killall -9 wpa_supplicant;killall -9 udhcpc ; iwconfig "+m_strAdapterName+" essid off ; iwconfig " +m_strAdapterName + " key off ;" + WIFI_OFF_COMMOND).c_str());
	
	DebugPrintf(DLC_DIAGNOSTIC, "CWifiBaseImpl::PowerOff(), lsmod:\n%s",
		dk::utility::SystemUtil::ReadStringFromShellOutput("lsmod").c_str());
	DebugPrintf(DLC_DIAGNOSTIC, "CWifiBaseImpl::PowerOff(), wlan0:\n%s",
		dk::utility::SystemUtil::ReadStringFromShellOutput("ifconfig wlan0").c_str());


    if(0 == result)
    {
        return S_OK;
    }
    else
    {
        return E_FAIL;
    }
}
bool CWifiBaseImpl::IsPowerOn()
{
    FILE *fp = fopen("/proc/net/wireless","r");
    if(!fp)
    {
        DebugPrintf(DLC_LIUJT, "CWifiBaseImpl::IsPowerOn return false 00");
        return FALSE;
    }
    char buff[512];
    string strBuff;
    strBuff.clear();
    int count = 0;
    unsigned int start = string::npos;
    while(!feof(fp))
    {
        count = fread(buff,1,sizeof(buff) -1,fp);
        buff[count] = 0;
        strBuff.append(buff);
    }
    fclose(fp);
    fp = NULL;
    start = strBuff.find(m_strAdapterName);
    if(start == string::npos)
    {
        DebugPrintf(DLC_LIUJT, "CWifiBaseImpl::IsPowerOn return false 01");
        return FALSE;
    }
    start = strBuff.find(" ",start);
    if( start == string::npos)
    {
        DebugPrintf(DLC_LIUJT, "CWifiBaseImpl::IsPowerOn return false 02");
        return FALSE;
    }
    while(start < strBuff.size() && strBuff[start] == ' ')//跳过空格
    {
        start++;
    }
    int state = 0;
    while(start < strBuff.size() && strBuff[start] != ' ')//跳过状态0001
    {
        state *= 10;
        state += strBuff[start] - '0';
        start++;
    }
    //DebugPrintf(DLC_DIAGNOSTIC,"WifiBase IsPowerOn  state is : 000%d",state);
    if(1 == state)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


AccessPoint* CWifiBaseImpl::GetCurAP(BOOL bNeedPing)
{
	std::string wireless = SystemUtil::ReadStringFromFile("/proc/net/wireless", false);
	if(wireless.empty())
	{
		return NULL;
	}

    string::size_type start = string::npos;

	// find wlan0 : 0001 
    start = wireless.find(m_strAdapterName);
    if(start == string::npos)
    {
        return NULL;
    }
		
    start = wireless.find("0001", start);
    if(start == string::npos)
    {
        return NULL;
    }
	
	m_CurAP.SSID = FindESSID();
	m_CurAP.Quality = GetConnectedQuality();
	m_CurAP.fIsConnected = IsConnectedtoESSID();

	return &(m_CurAP);
}

bool CWifiBaseImpl::GetSecurityTypeFromSSID(std::string SSID, eSecurityType *Security)
{
	string APs;	
	string APString;
    FILE   *stream; 
    char   buf[1024]; 
	
	DebugPrintf(DLC_DIAGNOSTIC,"CWifiBaseImpl::GetSecurityTypeFromSSID() \"%s\" ." , SSID.c_str());

	Refresh();

	memset( buf, 0, sizeof(buf));
	stream = fopen(WIFI_LIST_FILE, "r"); 
	if (NULL == stream)
	{
		return false;
	}
	
	while (fread( buf, 1, sizeof(buf) - 1, stream) > 0)
	{
		APs.append(buf);
		memset( buf, 0, sizeof(buf));
	}

	fclose(stream);
	
	// 加上一个结尾标志
	APs.append("\nCell End.\n");

	string::size_type start = APs.find(("ESSID:\"" + SSID + "\"").c_str(), 0);
	if (start == string::npos)
	{
		return false;
	}

    string::size_type end = APs.find("Cell" , start+1);
	if (end == string::npos)
	{
		return false;
	}

	APString = APs.substr(start, end-start);

	eSecurityType SecurityType = RESET;
	bool fPublic = false;
	
    //是否需要密钥
    if (APString.find("Encryption key:on",0)  != string::npos)
    {
		fPublic = false;
    }
    if (APString.find("Encryption key:off",0)  != string::npos)
    {
        SecurityType = NONE;
		fPublic = true;
    }

    //加密方式
    if (APString.find("IE: WPA Version") != string::npos)
    {
        SecurityType = WPA;
    }

    if (APString.find("IE: IEEE 802.11i/WPA2 Version") != string::npos)
    {
        SecurityType = WPA2;
    }

    if(APString.find(": 802.1x") != string::npos)
    {
        SecurityType = IEEE8021X;
    }

    if (false == fPublic && SecurityType == RESET)
    {
        SecurityType = (eSecurityType)(WEP_OPEN | WEP_SHARE);
    }
    
	*Security = SecurityType;
	
	DebugPrintf(DLC_DIAGNOSTIC,"CWifiBaseImpl::GetSecurityTypeFromSSID() OK.");

	return true;
}

std::string CWifiBaseImpl::FindGateWay()
{
	FILE *fp;
	char buff[512];
	string gw = "";

	fp = popen(("cat /tmp/"+m_strAdapterName).c_str(),"r");
	if(!fp) 
	{
		return (gw);
	}

	memset(buff, 0, sizeof(buff));
	fread(buff,1,sizeof(buff)-1,fp);
	pclose(fp);

	string str;
	str.append(buff);
	str.append("\n");

	string::size_type start = str.find("gw=", 0);
	if(start == string::npos)
	{
		return gw;
	}

	// 特殊情况下，只查找换行符(\n)，出现重复字段，需过滤。 来源：用户反馈日志 FindGateWay(192.168.11.1 192.168.11.1)
	// 因此改写为稍微严格的IP查找
	gw = GetIPFromString(str, start);

	DebugPrintf(DLC_DIAGNOSTIC, "CWifiBaseImpl::FindGateWay(%s)", gw.c_str());

	return gw;
}

std::string CWifiBaseImpl::FindIP()
{
	FILE *fp;
	char buff[512];
	string IP;

	fp = popen(("ifconfig " + m_strAdapterName).c_str(),"r");
	if(!fp) 
	{
		return IP;
	}

	memset(buff, 0, sizeof(buff));
	fread(buff,1,sizeof(buff)-1,fp);
	pclose(fp);

	string str;
	str.append(buff);
	str.append("\n");

	string::size_type start = str.find("inet addr:", 0);
	if(start == string::npos)
	{
		return IP;
	}

	IP = GetIPFromString(str, start);

	//DebugPrintf(DLC_DIAGNOSTIC, "CWifiBaseImpl::FindIP(%s)", IP.c_str());

	return IP;
}

std::string CWifiBaseImpl::FindMacAddr()
{
	FILE *fp;
	char buff[512];
	string mac;

	mac = SystemUtil::ReadStringFromFile("/proc/mac_addr", false);
	if (mac.size() == 12)
	{
		mac.insert(10, 1, ':');
		mac.insert(8, 1, ':');
		mac.insert(6, 1, ':');
		mac.insert(4, 1, ':');
		mac.insert(2, 1, ':');
		return mac;
	}

	fp = popen(("ifconfig " + m_strAdapterName).c_str(),"r");
	if(!fp) 
	{
		return mac;
	}

	memset(buff, 0, sizeof(buff));
	fread(buff,1,sizeof(buff)-1,fp);
	pclose(fp);

	string str;
	str.append(buff);
	str.append("\n");

	string::size_type start = str.find("HWaddr ", 0);
	if(start == string::npos)
	{
		return mac;
	}

	string::size_type end = str.find(" ", start + 7);
	if(end == string::npos)
	{
		return mac;
	}

	mac = str.substr(start + 7, end - start - 7);
	return mac;
}


std::string CWifiBaseImpl::FindESSID()
{
	string ESSID;	
    SystemUtil::ExecuteCommand("killall -9 iwconfig");
	string str = SystemUtil::ReadStringFromShellOutput(("iwconfig " + m_strAdapterName).c_str(), false);
	if(str.empty())
	{
		return ESSID;
	}
		
	string::size_type start = str.find("ESSID:\"");	
	if(start == string::npos)
	{
		return ESSID;
	}
	start += 7;

	string::size_type end = str.find("\" ", start);
	if(end == string::npos)
	{
		return ESSID;
	}

	ESSID = str.substr(start, end - start);

	return ESSID;
}

bool CWifiBaseImpl::IsConnectedtoESSID(int securityType)
{	
	string str = SystemUtil::ReadStringFromShellOutput(("iwconfig " + m_strAdapterName).c_str(), false);

	if(str.empty())
	{
		return false;
	}
	
	// 当前没有连接信号，或者没有SSID
	if(GetConnectedQuality() == 0)
	{
		return false;
	}

	// 判断密码是否与加密方式矛盾
	string::size_type start = str.find("Encryption key:");
	
	if(start == string::npos)
	{
		return false;
	}
	
	// 加密方式为“不加密”， 连接正确的情况
	// 增加，对K3密码错误的判断
	if(str.find("Encryption key:off", start)  != string::npos)
	{
		if (securityType == RESET  || securityType == NONE) 
		{
			return true;
		}else
		{
			return false;
		}
	}
	
	// 加密方式存在，然而密钥协商失败， 则说明连接错误
	if(str.find("Encryption key:00   Security mode:open", start)  != string::npos)
	{
		return false;
	}

	if(str.find("Encryption key:00   Security mode:restricted", start)  != string::npos)
	{
		return false;
	}
	
	return true;
}

bool CWifiBaseImpl::IsPasswordError(int securityType)
{	
	// 未加密的， 不予判断
	if (securityType == RESET  || securityType == NONE)
	{
		return false;
	}

	int quality = GetConnectedQuality();

	// 信号为0， 无法判断， 不能认定
	if (quality == 0) 
	{
		return false;
	}

	// 在有信号的情况下， 我们可以判断是否密码错误

	string str = SystemUtil::ReadStringFromShellOutput(("iwconfig " + m_strAdapterName).c_str(), false);

	if(str.empty())
	{
		return false;
	}
	
	// 判断密码是否与加密方式矛盾
	string::size_type start = str.find("Encryption key:");
	
	if(start == string::npos)
	{
		return false;
	}
		
	// 有加密， 而 key off了， 说明密码不对
	if(str.find("Encryption key:off", start)  != string::npos)
	{
		return true;
	}
	
	// 有加密， 而Key 00了，说明密码不对
	if(str.find("Encryption key:00   Security mode:open", start)  != string::npos)
	{
		return true;
	}

	if(str.find("Encryption key:00   Security mode:restricted", start)  != string::npos)
	{
		return true;
	}
	
	return false;
}


int CWifiBaseImpl::GetConnectedQuality()
{	
	string str = SystemUtil::ReadStringFromShellOutput(("iwconfig " + m_strAdapterName).c_str(), false);
	const char * link = "Link Quality:";
	
	// 找不到ESSID，当前没有连接上
	string::size_type start = str.find("ESSID:\"");	
	if(start == string::npos)
	{
		return 0;
	}

	// 判断信号强度是否为 0， -1 (-1就是255)
	// Link Quality:255/94 为连接错误
	start = str.find(link);
	if(start == string::npos)
	{
		return 0;
	}

	start += strlen(link);
	string::size_type stop = str.find("/",  start);

	if(stop == string::npos)
	{
		return 0;
	}
	string linkqual = str.substr(start, stop - start);
	int quality = atoi(linkqual.c_str());

	if(quality >= 255 || quality <= 0)
	{
		quality = 0;
	}	

	return quality;
}

bool CWifiBaseImpl::IsConnectedtoLAN()
{
	if (FindESSID().empty())
	{
		return false;

	}

	if (!IsConnectedtoESSID())
	{
		return false;
	}

	if (FindIP().empty())
	{
		return false;
	}

	return true;
}


bool CWifiBaseImpl::IsConnectedtoWAN()
{
	string dns_google("8.8.8.8");
	int dns_port=53;
	int ms = 100;

	return IsPingHostOK(dns_google, dns_port, ms);
}


// 粗略判断密码是否符合WEP
bool CWifiBaseImpl::isWEPPassword(const std::string& Password)
{
	int pwdsize = Password.size();

	if (pwdsize == 5 || pwdsize == 13 || pwdsize == 16)
	{
		return true;
	}

	// 这三种长度要求是HEX字符串
	if (pwdsize == 10 || pwdsize == 26 || pwdsize == 32)
	{
		for (int i=0; i<pwdsize; i++)
		{
			if (!isxdigit(Password[i]))
			{
				return false;
			}
		}
		return true;
	}

	return false;
}

// 从当前字符串查找第一个IP
// 比如: inet addr:192.168.15.244  Bcast:192.168.15.255  Mask:255.255.255.0  返回: 192.168.15.244
std::string  CWifiBaseImpl::GetIPFromString(std::string& str, std::string::size_type index)
{
	string ip = "";

	string::size_type start = str.find_first_of("0123456789",index);
	if(start == string::npos)
	{
		return ip;
	}

	string::size_type end = str.find_first_not_of(".0123456789",start);
	if(end == string::npos)
	{
		return ip;
	}

	ip = str.substr(start, end - start);
	return ip;
}

