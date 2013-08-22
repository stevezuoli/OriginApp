#include "drivers/DeviceInfo.h"
#include "stdio.h"
#include "interface.h"
#include "stdlib.h"
#include "string.h"
#include "Common/DuoKanServer.h"
#include "Utility/SystemUtil.h"

using dk::utility::SystemUtil;


#define CPU_INFO_FILE "/proc/cpuinfo"
#define VERSION_FILE "/etc/version.txt"
#define PRETTY_VERSION_FILE "/etc/prettyversion.txt"

map<string,string> DeviceInfo::deviceinfo;
bool DeviceInfo::initialized = false;
DeviceInfo::DEVICETYPE DeviceInfo::devicetype;
int DeviceInfo::version = 0;
#ifdef _X86_LINUX
static string s_sn;
#endif

#ifdef _X86_LINUX
void DeviceInfo::SetSerialNo(const char* sn)
{
    s_sn = sn;
    initialized = false;
}

#endif
const map<string,string>& DeviceInfo::GetDeviceInfo()
{
    if (DeviceInfo::initialized)
        return DeviceInfo::deviceinfo;

    FILE *fp = fopen(CPU_INFO_FILE,"r");
    if(!fp)
    {
        return DeviceInfo::deviceinfo;
    }
    char buff[512];
    string strInfo;
    int iBuffCount = 0;
    while(!feof(fp))
    {
        iBuffCount = fread(buff,1,sizeof(buff) - 1,fp);
        if(iBuffCount)
        {
            buff[iBuffCount] = 0;
            strInfo.append(buff);
        }
    }
    fclose(fp);
    unsigned int iStart = 0;
    unsigned int iEnd = string::npos;
    while(1)
    {
        iEnd = strInfo.find("\n",iStart);
        if(iEnd == string::npos)
        {
            break;
        }
        else
        {
            string strKey;
            string strValue;
            string strTmp = strInfo.substr(iStart,iEnd - iStart);
            GetDeviceInfoByString(strTmp,strKey,strValue);
            if(strKey.size() > 0 && strValue.size() > 0)
            {
                DeviceInfo::deviceinfo[strKey] = strValue;
            }
            iStart = iEnd + 1;
            if(iStart >= strInfo.size())
            {
                break;
            }
        }
    }

    DeviceInfo::initialized = true;
    return DeviceInfo::deviceinfo;;
}

void DeviceInfo::GetDeviceInfoByString(string strInfo,string& outstrKey,string& outstrValue)
{
    outstrKey.clear();
    outstrValue.clear();
    unsigned int iIndex = strInfo.find(":");
    if(iIndex == string::npos)
    {
        return;
    }
    outstrKey = strInfo.substr(0,iIndex);
    outstrValue = strInfo.substr(iIndex +1,strInfo.size() - iIndex -1);
    DebugPrintf(DLC_LIUJT, "DeviceInfo::GetDeviceInfoByString strInfo=%s, outstrKey=%s, outstrValue=%s", strInfo.c_str(), outstrKey.c_str(), outstrValue.c_str());
    StringTrim(outstrKey);
    StringTrim(outstrValue);
}
void DeviceInfo::StringTrim(string & str)
{
    if (0 == str.size())
    {
        return;
    }
    unsigned int iEnd = str.size() - 1;
    unsigned int iStart = 0;
    for(int i = iEnd; i>=0;i--)
    {
        if(str[i] >= 33 && str[i] <= 126 && str[i] != '\"')//这个范围是可见字符
        {
            iEnd = i;
            break;
        }
        if(0 == iEnd)
        {
            str.clear();
            return;
        }
    }
    for(unsigned int i = 0; i <=iEnd; i++)
    {
        if(str[i] >= 33 && str[i] <= 126 && str[i] != '\"')//这个范围是可见字符
        {
            iStart = i;
            break;
        }
    }
    str = str.substr(iStart,iEnd - iStart + 1);
}
string DeviceInfo::GetSerialNo()
{
    map<string, string>::const_iterator pos = GetDeviceInfo().find("Serial");
    if (pos != GetDeviceInfo().end())
    {
        return pos->second;
    }
    else
    {
#ifdef _X86_LINUX
        if (!s_sn.empty())
        {
            return s_sn;
        }
        static string deviceId;
        if (deviceId.empty())
        {
            // 去掉结尾回车
            string s = SystemUtil::ReadStringFromShellOutput("ifconfig  |grep -i eth |sed -n -e \"s/.*\\(\\([0-9a-fA-F]\\{2\\}:\\)\\{5\\}\\)/\\1/p\" |sed -n -e \"s/://gp\"");
            for (size_t i = 0; i < s.size(); ++i)
            {
                if (isspace(s[i]))
                {
                    s = s.substr(0, i);
                    break;
                }
            }

            deviceId = "B011" + s;
        }
        return deviceId;
#else
        return "";
#endif 
    }
}

string DeviceInfo::GetDeviceID()
{

#ifdef _X86_LINUX
    if (!s_sn.empty())
    {
        if (IsK4Touch() || IsKPW())
        {
            return "D008" + GetSerialNo();
        }
        else
        {
            return "D001" + GetSerialNo();
        }
    }
#endif
    //产品型号以D开头，后接三位数字，从000开始编号，代表不同厂商：如001--00Z、010--01Z；
    string _dID = string("D");


//#ifdef KindleApp

#ifdef KINDLE_FOR_TOUCH	   		
    _dID += "008" ; //for Kindle;
#else
    _dID += "001" ; //for Kindle;
#endif

    _dID += GetSerialNo();
#ifdef _X86_LINUX
    return _dID;
#else
    return _dID;
#endif
}


bool DeviceInfo::IsK3()
{
	DeviceInfo::DEVICETYPE type = DeviceInfo::GetDeviceType();
    switch (type)
    {
        case KINDLE_3WIFI:
        case KINDLE_3GUS:
        case KINDLE_3GUK:
        case KINDLE_3DXG:
            return true;
        default:
            return false;
    }
}

// 区分K3 与 K4 (在xKindle版本使用)
bool DeviceInfo::IsK4Series()
{
	if ( IsK4NT())
	{
		return true;
	}

	if ( IsK4Touch())
	{
		return true;
	}
	return false;
}

// 区分K4 Non-Touch 与 Touch (在xTouch版本使用)
bool DeviceInfo::IsK4NT()
{
	DeviceInfo::DEVICETYPE type = DeviceInfo::GetDeviceType();
	if ( type == DeviceInfo::KINDLE_4NT || type == DeviceInfo::KINDLE_4NTBLACK )
	{
		return true;
	}
	return false;
}


bool DeviceInfo::IsK4Touch()
{
	DeviceInfo::DEVICETYPE type = DeviceInfo::GetDeviceType();
	if (type == DeviceInfo::KINDLE_4TOUCH || type == DeviceInfo::KINDLE_4TOUCH3G  || type == DeviceInfo::KINDLE_4TOUCH3GI )
	{
		return true;
	}
	return false;
}


bool DeviceInfo::IsKPW()
{
	DeviceInfo::DEVICETYPE type = DeviceInfo::GetDeviceType();
	if (type == DeviceInfo::KINDLE_PW || type == DeviceInfo::KINDLE_PW3G)
	{
		return true;
	}
	return false;
}

DeviceInfo::DEVICETYPE DeviceInfo::GetDeviceType()
{	
    if (DeviceInfo::initialized)
        return DeviceInfo::devicetype;
	
	// 严格前缀匹配B0XX， 避免小概率的误判
    string subsn = GetSerialNo().substr(0, 4);
    DebugPrintf(DLC_DIAGNOSTIC, "GetDeviceType(%s)" , subsn.c_str() );

    if (subsn.find("B002") != string::npos)
		DeviceInfo::devicetype = DeviceInfo::KINDLE_2US;

    else if (subsn.find("B003") != string::npos)
        DeviceInfo::devicetype = DeviceInfo::KINDLE_2I;

    else if (subsn.find("B004") != string::npos)
        DeviceInfo::devicetype = DeviceInfo::KINDLE_2DXUS;

    else if (subsn.find("B005") != string::npos)
        DeviceInfo::devicetype = DeviceInfo::KINDLE_2DXI;

    else if (subsn.find("B006") != string::npos)
        DeviceInfo::devicetype = DeviceInfo::KINDLE_3GUS;

    else if (subsn.find("B008") != string::npos)
        DeviceInfo::devicetype = DeviceInfo::KINDLE_3WIFI;

    else if (subsn.find("B009") != string::npos)
        DeviceInfo::devicetype = DeviceInfo::KINDLE_3DXG;

    else if (subsn.find("B00A") != string::npos)
        DeviceInfo::devicetype = DeviceInfo::KINDLE_3GUK;
	
    else if (subsn.find("B00E") != string::npos)
        DeviceInfo::devicetype = DeviceInfo::KINDLE_4NT;
	
    else if (subsn.find("B011") != string::npos)
        DeviceInfo::devicetype = DeviceInfo::KINDLE_4TOUCH;

    else if (subsn.find("B00F") != string::npos)
        DeviceInfo::devicetype = DeviceInfo::KINDLE_4TOUCH3G;

    else if (subsn.find("B010") != string::npos)
        DeviceInfo::devicetype = DeviceInfo::KINDLE_4TOUCH3GI;
	
    else if (subsn.find("B023") != string::npos)
        DeviceInfo::devicetype = DeviceInfo::KINDLE_4NTBLACK;

    else if (subsn.find("B024") != string::npos)
        DeviceInfo::devicetype = DeviceInfo::KINDLE_PW;
	
    else if (subsn.find("B01B") != string::npos)
        DeviceInfo::devicetype = DeviceInfo::KINDLE_PW3G;

    else if (subsn.find("B01D") != string::npos)
        DeviceInfo::devicetype = DeviceInfo::KINDLE_PW3G;
		
    else if (subsn.find("9023") != string::npos)
        DeviceInfo::devicetype = DeviceInfo::KINDLE_4NTBLACK;

    else if (subsn.find("B01C") != string::npos)
        DeviceInfo::devicetype = DeviceInfo::KINDLE_PW3G;

    else if (subsn.find("B01F") != string::npos)
        DeviceInfo::devicetype = DeviceInfo::KINDLE_PW3G;

    else if (subsn.find("B020") != string::npos)
        DeviceInfo::devicetype = DeviceInfo::KINDLE_PW3G;

    DeviceInfo::initialized = true;
    return DeviceInfo::devicetype;
}


/*  
 *   KT 的版本号从500 到 512 ， 如果不能读取，默认为500 
 *
 *   目前只要区分 <=5.0.4 (属于500系列),  >=5.1.0 (属于510系列)
 *   500系列 与 510系列的区别在于 E-ink 驱动 和 powerd ( 504不用patch ; 510 patch ; 511=512 patch) 做了改动。
 *
 *   5.1.0 version.txt
 *   System Software Version: 049-juno_2_yoshi-155776
 *   Wed Apr  4 20:45:43 PDT 2012
 *
 */

int DeviceInfo::GetTouchVersion()
{
	if(DeviceInfo::version != 0)
		return DeviceInfo::version;

	DeviceInfo::version = 500;
	FILE *fp = fopen(VERSION_FILE,"r");
	if(!fp)
	{
		return DeviceInfo::version;
	}

	const char *yoshi = "yoshi-";
	char *find;
	char buff[128] = {0};

	fread(buff,1,sizeof(buff) - 1,fp);
	fclose(fp);
	find = strstr(buff, yoshi);
	if(find == NULL)
		return DeviceInfo::version;
	
	int iversion = atoi(find + strlen(yoshi));

	if (iversion >= 180429) 
	{
		DeviceInfo::version = 532;
	}
	else if (iversion >= 167953) 
	{
		DeviceInfo::version = 512;
	}
	else if (iversion >= 156819) 
	{
		DeviceInfo::version = 511;
	}
	else if (iversion >= 155776) 
	{
		DeviceInfo::version = 510;
	}
	else if (iversion >= 149604) 
	{
		DeviceInfo::version = 504;
	}
	else if (iversion >= 137371) 
	{
		DeviceInfo::version = 501;
	}
	else if (iversion >= 137028) 
	{
		DeviceInfo::version = 500;
	}
	
	return DeviceInfo::version;
}

bool DeviceInfo::IsTouch500()
{
	return (DeviceInfo::GetTouchVersion() < 510);
}

bool DeviceInfo::IsTouch510()
{
	return (DeviceInfo::GetTouchVersion() >= 510);
}

/*
 * KP 的版本号从520 到 530 ， 如果不能读取，默认为520 
  #cat /etc/version.txt
  System Software Version: 020-juno_3_yoshime3-177470
  Tue Oct 30 18:53:54 PDT 2012

*/

int DeviceInfo::GetKPVersion()
{
	if(DeviceInfo::version != 0)
		return DeviceInfo::version;

	DeviceInfo::version = 520;
	FILE *fp = fopen(VERSION_FILE,"r");
	if(!fp)
	{
		return DeviceInfo::version;
	}

	const char *yoshi = "yoshime3-";
	char *find;
	char buff[128] = {0};

	fread(buff,1,sizeof(buff) - 1,fp);
	fclose(fp);
	find = strstr(buff, yoshi);
	if(find == NULL)
		return DeviceInfo::version;
	
	int iversion = atoi(find + strlen(yoshi));

    if (iversion >= 187929) 
	{
		DeviceInfo::version = 535;
	}
    else if (iversion >= 184890) 
	{
		DeviceInfo::version = 534;
	}
    else if (iversion >= 181203)
    {
		DeviceInfo::version = 533;
    }
    else if (iversion >= 179537)
    {
		DeviceInfo::version = 531;
    }    
	else if (iversion >= 177470) 
	{
		DeviceInfo::version = 530;
	}
	else if (iversion >= 172974) 
	{
		DeviceInfo::version = 520;
	}
	
	return DeviceInfo::version;
}


/* 
 *   2012-06-05 Amazon released: Kindle update 4.1.0 bin 
 *   kernel 2.6.31-rt11-lab126 #5 Fri 2012.05.25 21:29:40 PDT armv71
 *
 *   #cat /etc/version.txt
 *   System Software Version: 055-H2_yoshi-161537
 *   Fri May 25 21:31:18 PDT 2012
 *
 *   本函数区分 400 (4.0.0-4.0.1) 和 410 (4.1.0).
 */

int DeviceInfo::GetK4Version()
{
	if(DeviceInfo::version != 0)
		return DeviceInfo::version;

	DeviceInfo::version = 400;
	FILE *fp = fopen(VERSION_FILE,"r");
	if(!fp)
	{
		return DeviceInfo::version;
	}

	const char *yoshi = "yoshi-";
	char *find;
	char buff[128] = {0};

	fread(buff,1,sizeof(buff) - 1,fp);
	fclose(fp);
	find = strstr(buff, yoshi);
	if(find == NULL)
		return DeviceInfo::version;
	
	int iversion = atoi(find + strlen(yoshi));

	if(iversion >= 161537) 
	{
		DeviceInfo::version = 410;
	}

	return DeviceInfo::version;
}


/*
 * #cat /etc/prettyversion.txt  
 * Kindle 4.1.0 (~~otaVersion~~)
 *
 * #cat /etc/prettyversion.txt  
 * Kindle 5.1.2 (~~otaVersion~~)
 *
 */
 string DeviceInfo::GetKindleVersion()
 {
	string kindleversion;
	string prettyversion = SystemUtil::ReadStringFromFile(PRETTY_VERSION_FILE, false);
	unsigned int end = prettyversion.find(" (");

	if (end == string::npos)
	{
		kindleversion = prettyversion;
	}
	else
	{
		kindleversion = prettyversion.substr(0, end);
	}

	return kindleversion;
}

string DeviceInfo::SplitSerialNo(string serialNo)
{
    // 验证SN号是否正确
    if (16 != serialNo.size())
    {
        return "";
    }
    serialNo.insert(12,1,' ');
    serialNo.insert(8,1,' ');
    serialNo.insert(4,1,' ');
    return serialNo;
}

int DeviceInfo::GetTouchScreenWidth()
{
    if (IsKPW())
    {
        return 758;
    }
    else if (IsK4Touch())
    {
#ifdef _X86_LINUX
        return 600;
#else
        return 4150;
#endif
    }
    else
    {
        return -1;
    }
}

int DeviceInfo::GetTouchScreenHeight()
{
    if (IsKPW())
    {
        return 1024;
    }
    else if (IsK4Touch())
    {
#ifdef _X86_LINUX
        return 800;
#else
        return 4150;
#endif
    }
    else
    {
        return -1;
    }
}

int DeviceInfo::GetDisplayScreenWidth()
{
    if (IsKPW())
    {
        return 758;
    }
    else
    {
        return 600;
    }
}

int DeviceInfo::GetDisplayScreenHeight()
{
    if (IsKPW())
    {
        return 1024;
    }
    else
    {
        return 800;
    }
}


#ifdef _X86_LINUX
void DeviceInfo::SetDeviceType(DEVICETYPE dt)
{
    initialized = true;
    devicetype = dt;
}
#endif
