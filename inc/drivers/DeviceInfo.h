#ifndef __DEVICEINFO_H__
#define __DEVICEINFO_H__
#include <map>
#include "string"
using std::map;
using std::string;


class DeviceInfo
{

public:
    enum DEVICETYPE
    {
        UNKNOWN = 0,
        KINDLE_2US,
        KINDLE_2I,
        KINDLE_2DXUS,
        KINDLE_2DXI,
        KINDLE_3WIFI,
        KINDLE_3GUS,
        KINDLE_3GUK,
        KINDLE_3DXG,
        KINDLE_4NT,
        KINDLE_4TOUCH,
        KINDLE_4TOUCH3G,
        KINDLE_4TOUCH3GI,
        KINDLE_4NTBLACK,
        KINDLE_PW,
        KINDLE_PW3G,
    };
	

    static DEVICETYPE GetDeviceType();
    static string GetDeviceID();
    static bool IsK3();
    static bool IsK4Series();   
    static bool IsK4NT();
    static bool IsK4Touch();
    static bool IsKPW();    
    static int GetK4Version();
    static int GetKPVersion();
    static int GetTouchVersion();
    static bool IsTouch500();
    static bool IsTouch510();    
    static string GetKindleVersion();
    static string GetSerialNo();    
    static string SplitSerialNo(string serialNo);
    static int GetTouchScreenWidth();
    static int GetTouchScreenHeight();
    static int GetDisplayScreenWidth();
    static int GetDisplayScreenHeight();
#ifdef _X86_LINUX
    static void SetSerialNo(const char* sn);
    static void SetDeviceType(DEVICETYPE deviceType);
#endif
private:
    static const map<string,string>& GetDeviceInfo();
    static void GetDeviceInfoByString(string strInfo,string& Key,string& Value);
    static void StringTrim(string& strInfo);
private:
    static bool initialized;
    static map<string,string> deviceinfo;	
    static DEVICETYPE devicetype;
    static int version;
};
#endif
