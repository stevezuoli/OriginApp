#include "../inc/Common/SystemSetting_DK.h"
#include "Framework/INativeMessageQueue.h"
#include "dkDim.h"
#include "FontManager/DKFont.h"
#include "GUI/FCFontManager.h"
#include "Utility/MD5Checker.h"
#include "KernelVersion.h"
#include <sstream>
#include <stdio.h>
#include "Utility/SystemUtil.h"
#include "Utility/Misc.h"
#include "Utility/PathManager.h"
#include "I18n/StringManager.h"

using dk::utility::SystemUtil;
using dk::utility::PathManager;

using namespace std;

SystemSettingInfo* SystemSettingInfo::m_Instance = NULL;

// 磁盘空间
DKDiskSpace::DKDiskSpace()
{
	GetDiskSpaceMiB();
	return;
}

void DKDiskSpace::GetDiskSpaceMiB()
{
	struct statfs ds;	

	memset(&ds, 0, sizeof(struct statfs));
	statfs(Config::GetConfigValueString(CKI_MTDPath), &ds);
	m_TotalMiB = (long long)ds.f_bsize * (long long)ds.f_blocks >> 20;
	m_FreeMiB = (long long)ds.f_bavail * (long long)ds.f_bsize >> 20;
	return;
}

UINT32 DKDiskSpace::GetTotalSize()
{
	GetDiskSpaceMiB();
	return m_TotalMiB;
}

UINT32 DKDiskSpace::GetFreeSize()
{
	GetDiskSpaceMiB();
	return m_FreeMiB;
}

UINT32 DKDiskSpace::GetFreePercent()
{
	GetDiskSpaceMiB();
	return (m_FreeMiB * 100 / m_TotalMiB);
}

void DKDiskSpace::GetFileSpaceInfo()
{    
	book.m_nSize  = FootPrint_Book_Get();
    book.m_nPercent = book.m_nSize * 100 / m_TotalMiB;
    
    video.m_nSize   = FootPrint_Video_Get();
    video.m_nPercent= video.m_nSize * 100 / m_TotalMiB;
    
    audio.m_nSize   = FootPrint_Audio_Get();
    audio.m_nPercent= audio.m_nSize * 100 / m_TotalMiB;
    return;
}

int DKDiskSpace::FootPrint_Book_Get()
{
	return 0;
}
int DKDiskSpace::FootPrint_Video_Get()
{
	return 0;
}

int DKDiskSpace::FootPrint_Audio_Get()
{
	return 0;
}


// IME相关
IME::IME()
{
	Init();
}	

void IME::Init()
{
	m_cIme = 3;
	m_eIme  = GetIme();
	m_OptionIME = GetOptionIME();
}

bool SystemSettingInfo::SetLanguage(const char* language)
{
    SetConfigValueString(CKI_Language, language);
	return true;
}

bool IME::SetDefaultIME(IMETYPE ime)
{
	m_eIme = ime;
    Config::SetConfigValueInt(CKI_DefaultIME, (int)m_eIme);
	return TRUE;
}
    
bool IME::SetOptionIME(int option, BOOL bIsAdd)
{
	if(bIsAdd)	{
		m_OptionIME |= (1 << option);
	}else{
		m_OptionIME &= ~(1 << option);
	}
    Config::SetConfigValueInt(CKI_OptionalIME, m_OptionIME);
	return TRUE;
}


IMETYPE IME::GetIme()
{
    const char* defaultIME = Config::GetConfigValueString(CKI_DefaultIME);
    return defaultIME ? IMETYPE(atoi(defaultIME)) : IME_EN;
}
   
int IME::GetOptionIME()
{
    const char* optionalIME = Config::GetConfigValueString(CKI_OptionalIME);
    return optionalIME ? IMETYPE(atoi(optionalIME)) : IME_EN;
}

// 时钟相关
DKTime::DKTime()
{
	Init();
}


//读取系统时间并初始化
void DKTime::Init()
{
#ifdef _X86_LINUX
	tzeast = 8;
	tzhour = 8;
	tzminute = 0;
#else
    time_t utctm;
    struct tm *info = NULL;
	int rawtz = 0;
	char buf[16] = {0};

	tzeast = 1;
	tzhour = 0;
	tzminute = 0;

	// 如果没有设置时区，则默认设置为东八区
	snprintf(buf, sizeof(buf), "%s",  Config::GetConfigValueString(CKI_TIMEZONE));
	if(strlen(buf) == 5) {
		rawtz = atoi(buf);
		tzeast = (rawtz >= 0) ? 1 :  -1;
		if(rawtz < 0)
			rawtz = -rawtz;
		tzhour = rawtz / 100;
		tzminute = rawtz % 100;

	}else {
		tzeast = 1;
		tzhour = 8;
		tzminute = 0;
	}

	SetTimeZone();

    time(&utctm);

    info = localtime(&utctm);
    if(info == NULL)
		return;

    year    = info->tm_year + 1900;
    month   = info->tm_mon + 1;;
    day     = info->tm_mday;
    hour    = info->tm_hour;
    minute = info->tm_min;
    second = info->tm_sec;

    //从文件中读取是否是24小时制
    bIS24Hour = (bool)(atoi(Config::GetConfigValueString(CKI_TimeSys)));

    if(!bIS24Hour && hour > 12)
	{
        hour -= 12;
    }
#endif
}

//判断闰年
bool DKTime::isLeapYear(int nYear)
{
    if((0 == nYear % 4 && nYear % 100 != 0) || (0 == nYear % 400))
    {
        return TRUE;
    }
    return FALSE;
}

bool DKTime::SetLocalTime(time_t *time)
{
	bool retn;
    struct tm p;

    p.tm_sec  = second;
    p.tm_min  = minute;
    p.tm_hour = hour;
    p.tm_mday = day;
    p.tm_mon  = month - 1;
    p.tm_year = year - 1900;
    DebugPrintf(DLC_MESSAGE, "year %d month %d day %d hour%d minute %d second %d\n",
        year, month, day, hour, minute, second);
    time_t timep  = mktime(&p);
	//stime(&timep);

    struct timeval tv;

    memset(&tv, 0, sizeof(struct timeval));
    tv.tv_sec = timep;
    tv.tv_usec = 0;
	retn = settimeofday(&tv, (struct timezone*)0);
	SystemUtil::ExecuteCommand("hwclock -w -u");

	return retn;
}

/* hwclock 设置rtc时钟，修正以前只设置系统时间 (软时钟) ，复位后时钟信息丢失的bug*/
/* TODO：手动设置时间按用户设置的时区 */ 
bool DKTime::SetSysTime(void)
{
    struct tm p;
    
    p.tm_sec  = second;
    p.tm_min  = minute;
    p.tm_hour = hour;
    p.tm_mday = day;
    p.tm_mon  = month - 1;
    p.tm_year = year - 1900;
    DebugPrintf(DLC_MESSAGE, "year %d month %d day %d hour%d minute %d second %d\n",
        year, month, day, hour, minute, second);

    time_t timep  = mktime(&p);
	return SetLocalTime(&timep);
}

UINT32 DKTime::GetYear()
{
	return year;
}

UINT32 DKTime::GetMonth()
{
	return month;
}

UINT32 DKTime::GetDay()
{
	return day;
}

UINT32 DKTime::GetHour()
{
	return hour;
}

UINT32 DKTime::GetMinute()
{
	return minute;
}

UINT32 DKTime::GetSecond()
{
	return second;
}


// linux time()返回值是一个32 bit整数，合法的时间 0 - 0x7fffffff
// 1970/01/01 00:00:00 - 2038/01/19/ 11:14:07
bool DKTime::SetYear(UINT32 nYear)
{
	if(nYear < 1970)
		nYear = 1970;
	if (nYear > 2037)
		nYear = 2037;
    year = nYear;
	return TRUE;
}

bool DKTime::SetMonth(UINT32 nMonth)
{
    if(nMonth < 1)
		nMonth = 1;
    if(nMonth > 12)
		nMonth = 12;
	month = nMonth;
    return TRUE;
}

int DKTime::GetDaysOfMonth(int nYear, int nMonth)
{
	const int DaysOfMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	int days;

	if(nMonth < 1 || nMonth > 12) {
		return 31;
	}	

	days = DaysOfMonth[nMonth - 1];
	if(nMonth == 2) {
		days = isLeapYear(nYear) ?  29 : days;
	}
	return days;
}

bool DKTime::SetDay(UINT32 nDay)
{
	if(nDay < 1)
		nDay = 1;
	if((int)nDay > GetDaysOfMonth(year, month))
		nDay = GetDaysOfMonth(year, month);
    day = nDay;
    return TRUE;
}

bool DKTime::SetHour(UINT32 nHour)
{
    UINT32 maxvalue = 23;//bIS24Hour ? 23 : 12;
    UINT32 minvalue = 0;//bIS24Hour ? 0 : 1;
    if(nHour < minvalue || nHour > maxvalue)
    {
        return FALSE;
    }
    hour = nHour;
    return TRUE;
}

bool DKTime::SetMinute(UINT32 nMinute)
{
    if(nMinute > 59)
		nMinute = 59;
    minute = nMinute;
    return TRUE;
}

void DKTime::SetTimeSystem(bool bValue)
{
    if(bIS24Hour == bValue)
    {
        return;
    }
    
    bIS24Hour = bValue;
    if(bIS24Hour)
    {
        Config::SetConfigValueString(CKI_TimeSys, "1");
    }
    else
    {
        Config::SetConfigValueString(CKI_TimeSys, "0");
    }
    Init();
}

int DKTime::GetTimeSystem()
{
    return bool(atoi(Config::GetConfigValueString(CKI_TimeSys)));
}


/* 
 * 
 * 从东十二区到西十二区，共分为24个整时区
 * -1200 , +1200 
 * 北京时区为东八区： +0800
 * 印度时区为东五点五区： +0530
 *
 * CST: Chinese Standard Time
 * 
 * 时区为一个四位数，前两位为整时，后两位为分钟。
 * 注意西部时区为负数，东部时区为正数
 */
#define TIME_ZONE_CST  800
int DKTime::GetTimeZoneSeconds()
{	
	int offset_in_seconds = tzhour * 3600 + tzminute * 60;

	if(tzeast < 0)
		offset_in_seconds = - offset_in_seconds;
	return offset_in_seconds;	
}

int DKTime::GetTimeZoneEast()
{	
	return tzeast;	
}

int DKTime::GetTimeZoneFull()
{	
	return tzhour;	
}

int DKTime::GetTimeZoneHalf()
{	
	return tzminute;	
}

void DKTime::SetTimeZone()
{
	int offset_in_seconds = GetTimeZoneSeconds();
	SetTimeZoneSeconds(offset_in_seconds);
}

void DKTime::SetTimeZoneSeconds(int offset_in_seconds)
{
	char buf[128] = {0};
	int east = 	(offset_in_seconds >= 0) ? 1: -1;
	static int cnt = 0;
	
	// 系统初始化时的两次设置，不打印
	cnt++;
	sprintf(buf, "settz %d", offset_in_seconds);
	SystemUtil::ExecuteCommand(buf, (cnt > 2));

	if(offset_in_seconds < 0)
		offset_in_seconds = -offset_in_seconds;
	tzhour = offset_in_seconds / 3600;
	tzminute =  (offset_in_seconds - tzhour * 3600)/60; 
	tzeast = east;

	char sign = east > 0  ?  '+' : '-';
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%c%02d%02d", sign, tzhour, tzminute);
	Config::SetConfigValueString(CKI_TIMEZONE, buf);
}

void DKTime::SetTimeZoneEast(int east)
{	
	tzeast = (east >= 0) ? 1: -1;
}

void DKTime::SetTimeZoneFull(int hour)
{	
	tzhour = hour;
}

void DKTime::SetTimeZoneHalf(int minute)
{	
	tzminute = minute;
}



SystemSettingInfo::SystemSettingInfo()
    : m_eImgRotate(IMG_ROTATE_NONE)
    , m_eImgCartoon(IMG_CARTOON_ALL)
    , m_fontDirSize(-1)

{
    m_Time.Init();
    m_IME.Init();
}
    
SystemSettingInfo* SystemSettingInfo::GetInstance()
{
    if(m_Instance == NULL)
        m_Instance = new SystemSettingInfo();
    return m_Instance; 
}

bool SystemSettingInfo::SetSecsPerPage(int csecs)
{
    if(csecs < 1 || csecs > 99)
    {
        return FALSE;
    }

    SetConfigValueInt(CKI_PageDown_Speed, csecs);
    return TRUE;
}

bool SystemSettingInfo::SetBlackLevel(int nlevel)
{
    return FALSE;
}

bool SystemSettingInfo::SetPdfSharpen(int nlevel)
{
    SetConfigValueInt(CKI_PdfSharpen, nlevel);
    return TRUE;
}

bool SystemSettingInfo::SetIsFavorSysPath(BOOL value)
{
    return FALSE;
} 
    
bool SystemSettingInfo::SetFontSize(int _size)
{
    SetConfigValueInt(CKI_BookFontSize,  _size);
    return TRUE;
}

bool SystemSettingInfo::SetRepaintPage(int iCount)
{
    SetConfigValueInt(CKI_RepaintPage, iCount);
    return TRUE;
}

bool SystemSettingInfo::SetTextDarkLevel(int _iLevel)
{
    SetConfigValueInt(CKI_TextDarkLevel, _iLevel);
    return TRUE;
}

bool SystemSettingInfo::SetFontStyle_FamiliarOrTraditional(int style)
{
    SetConfigValueInt(CKI_FontStyle_FamiliarOrTraditional, MakeInRange(style, 0, 1));
    return TRUE;
}

int SystemSettingInfo::GetFontStyle_FamiliarOrTraditional()
{
    return MakeInRange(GetConfigValueInt(CKI_FontStyle_FamiliarOrTraditional), 0, 1);
}

bool SystemSettingInfo::SetLineGap(int iLineGap)
{
    SetConfigValueInt(CKI_BookLineGap, iLineGap);
    return TRUE;
}

bool SystemSettingInfo::SetParaSpacing(int iParaSpacing)
{
    SetConfigValueInt(CKI_BookParaSpacing, iParaSpacing);
    return TRUE;
}

bool SystemSettingInfo::SetTabStop(int iTabStop)
{
    SetConfigValueInt(CKI_BookTabStop, iTabStop);
    return TRUE;
}

bool SystemSettingInfo::SetBookIndent(int iBookIndent)
{
    SetConfigValueInt(CKI_BookIndent, iBookIndent);
    return TRUE;
}

int SystemSettingInfo::GetLineGap()
{
    int iLineGap = GetConfigValueInt(CKI_BookLineGap);
    return (iLineGap <= 0) ? 0 : iLineGap;
}

int SystemSettingInfo::GetTextDarkLevel()
{
    return GetConfigValueInt(CKI_TextDarkLevel);
}

int SystemSettingInfo::GetParaSpacing()
{
    int iParaSpacing = GetConfigValueInt(CKI_BookParaSpacing);
    return (iParaSpacing <= 0) ? 0 : iParaSpacing;
}

int SystemSettingInfo::GetTabStop()
{
    int iTabStop = GetConfigValueInt(CKI_BookTabStop);
    return iTabStop;
}

int SystemSettingInfo::GetBookIndent()
{
    int iIndent = GetConfigValueInt(CKI_BookIndent);
    return iIndent;
}

int SystemSettingInfo::GetFontSize()
{
    int iFontSize = GetConfigValueInt(CKI_BookFontSize);
    return iFontSize;
}

bool SystemSettingInfo::SetTypeSetting(int eTypesetting)
{
    SetConfigValueInt(CKI_TypeSetting, MakeInRange(eTypesetting, 0, 3));
    return TRUE;
}

bool SystemSettingInfo::SetImageRotate(Imagerotate eImgRotate)
{
    SetConfigValueInt(CKI_ImageRotate, (int)eImgRotate);
    return TRUE;
}

bool SystemSettingInfo::SetImageCartoon(Imagecartoon eImgCartoon)
{
    SetConfigValueInt(CKI_ImageCartoon, (INT)eImgCartoon);
    return TRUE;
}


Imagecartoon SystemSettingInfo::GetImgCartoon()
{
    return (Imagecartoon)GetConfigValueInt(CKI_ImageCartoon);
}

Imagerotate SystemSettingInfo::GetImgRotate()
{
    return (Imagerotate)GetConfigValueInt(CKI_ImageRotate);
}

LPCSTR SystemSettingInfo::GetMTDPathLP()
{
    return GetConfigValueString(CKI_MTDPath);
}

int SystemSettingInfo::GetDKSmartLayoutMode()
{
    return GetConfigValueInt(CKI_SmartLayout);
}

int SystemSettingInfo::GetScreenSavers()
{
    return GetConfigValueInt(CKI_ScreenSavers);
}
int     SystemSettingInfo::GetLanguageIndex()
{

    return GetConfigValueInt(CKI_Language);
}

bool SystemSettingInfo::GetIsAppFirstLoading()
{
    return GetConfigValueInt(CKI_AppFirstLoading);
}

bool SystemSettingInfo::GetIsFavorSysPath()
{
    return FALSE;
}
CString SystemSettingInfo::GetVersion()
{
    CString version = CString("xTouch");
#ifdef _X86_LINUX
    version = version + ".rtm";
#endif
    #ifdef RELEASE_TO_MARKET
        version = version + ".rtm";
    #endif
    return version;
}
DKDiskSpace SystemSettingInfo::GetDiskSpace()
{
    return m_DiskSpace;
}
    
int SystemSettingInfo::GetRepaintPage()
{
    return GetConfigValueInt(CKI_RepaintPage);
}
int SystemSettingInfo::GetTypeSetting()
{
    return MakeInRange(GetConfigValueInt(CKI_TypeSetting), 0, 3);
}

int SystemSettingInfo::GetSecsPerPage()
{
    return GetConfigValueInt(CKI_PageDown_Speed);  
}
int SystemSettingInfo::GetBlackLevel()
{
    return GetConfigValueInt(CKI_BoldLevel);
}

int SystemSettingInfo::GetPdfSharpen()
{
    return GetConfigValueInt(CKI_PdfSharpen);
}

bool SystemSettingInfo::GetBatteryRatioMode()
{
    return (bool)GetConfigValueInt(CKI_BatteryRatioMode);
}

bool SystemSettingInfo::SetBatteryRatioMode(bool mode)
{
    return SetConfigValueInt(CKI_BatteryRatioMode, mode);
}

int SystemSettingInfo::GetProgressBarMode() const
{
    int barMode = GetConfigValueInt(CKI_ProgressBarMode);
    (barMode >= 0) || (barMode = 0);
    (barMode <= 1) || (barMode = 1);
    return barMode;
}

int SystemSettingInfo::GetFontRender() const
{
    int render = GetConfigValueInt(CKI_FontRender);
    (render >= 0) || (render = 0);
    (render <= 1) || (render = 1);
    return render;
}

bool SystemSettingInfo::GetMetadataOpen()
{
	return (bool)GetConfigValueInt(CKI_EpubMobiMetaData);
}

int SystemSettingInfo::GetPageHorizontalMargin(int mode)
{
	if(READINGLAYOUT_FULLSCREEN == mode)
	{
		return GetConfigValueInt(CKI_PageHorizontalMargin);
	}
    return GetConfigValueInt(CKI_PageHorizontalMarginMode);
}

bool SystemSettingInfo::SetPageHorizontalMargin(int mode, int iMargin)
{
	if(READINGLAYOUT_FULLSCREEN == mode)
	{
		SetConfigValueInt(CKI_PageHorizontalMargin, iMargin);
	}
    else
	{
		SetConfigValueInt(CKI_PageHorizontalMarginMode, iMargin);
	}
    return true;
}

int SystemSettingInfo::GetPageTopMargin(int mode)
{
	if(READINGLAYOUT_FULLSCREEN == mode)
	{
		return GetConfigValueInt(CKI_PageTopMargin);
	}
    return GetConfigValueInt(CKI_PageTopMarginMode);
}

bool SystemSettingInfo::SetPageTopMargin(int mode, int iMargin)
{
	if(READINGLAYOUT_FULLSCREEN == mode)
	{
		SetConfigValueInt(CKI_PageTopMargin, iMargin);
	}
    else
	{
		SetConfigValueInt(CKI_PageTopMarginMode, iMargin);
	}
    return true;
}

string SystemSettingInfo::GetKindleMAC()
{
    return GetConfigValueString(CKI_KindleMAC);
}

bool SystemSettingInfo::SetKindleMAC(const string& mac)
{
    SetConfigValueString(CKI_KindleMAC, mac.c_str());

    return true;
}

int SystemSettingInfo::GetPageBottomMargin(int mode)
{
	if(READINGLAYOUT_FULLSCREEN == mode)
	{
		return GetConfigValueInt(CKI_PageBottomMargin);
	}
    return GetConfigValueInt(CKI_PageBottomMarginMode);
}

bool SystemSettingInfo::SetPageBottomMargin(int mode, int iMargin)
{
	if(READINGLAYOUT_FULLSCREEN == mode)
	{
		SetConfigValueInt(CKI_PageBottomMargin, iMargin);
	}
    else
	{
		SetConfigValueInt(CKI_PageBottomMarginMode, iMargin);
	}
    return true;
}


int SystemSettingInfo::GetAutoFlipTimeSecond()
{
    return GetConfigValueInt(CKI_AutoFlipTimeSecond);
}

bool SystemSettingInfo::SetAutoFlipTimeSecond(int iSec)
{
    SetConfigValueInt(CKI_AutoFlipTimeSecond, iSec);

    return true;
}


string  SystemSettingInfo::GetDuokanBookMD5(int majorVersion, int minorVersion)
{
    //TODO: if you need, add more to tag this for reader engine.
    string bookLineGap(GetConfigValueString(CKI_BookLineGap) );
    string bookParaSpace(GetConfigValueString(CKI_BookParaSpacing) );
    string bookTabStop(GetConfigValueString(CKI_BookTabStop) ); 
    string bookIndent(GetConfigValueString(CKI_BookIndent) );
    string bookFontSize(GetConfigValueString(CKI_BookFontSize) );

	string pageHorrizonMargin;
    string pageTopMargin;
    string pageBottomMargin;
	if(READINGLAYOUT_FULLSCREEN == GetReadingLayout())
	{
		pageHorrizonMargin = GetConfigValueString(CKI_PageHorizontalMargin);
		pageTopMargin = GetConfigValueString(CKI_PageTopMargin);
		pageBottomMargin = GetConfigValueString(CKI_PageBottomMargin);
	}
	else
	{
		pageHorrizonMargin = GetConfigValueString(CKI_PageHorizontalMarginMode);
		pageTopMargin = GetConfigValueString(CKI_PageTopMarginMode);
		pageBottomMargin = GetConfigValueString(CKI_PageBottomMarginMode);
	}
    
    string textTypeSetting(GetConfigValueString(CKI_TypeSetting));
    string ChineseFontSetting(GetConfigValueString(CKI_ChineseFont));
    string EnglishFontSetting(GetConfigValueString(CKI_EnglishFont));
    string DefaultFont(GetConfigValueString(CKI_SysDefaultFont));
    
    string fontTag;
    if (m_fontDirSize < 0)
    {
        m_fontDirSize = g_pFontManager->GetDirFileSize();
    }
    int fontSizeTag = m_fontDirSize;
    stringstream ss;
    ss << fontSizeTag;
    ss >> fontTag;
    
    string version;
    ss << majorVersion << minorVersion;
    ss >> version;

    string kernelVersion(DK_GetKernelVersion());

    string UniqeTag = bookLineGap + "-"
                    + bookParaSpace  + "-"
                    + bookTabStop + "-"
                    + bookIndent + "-"
                    + bookFontSize  + "-"
                    + pageHorrizonMargin  + "-"
                    + pageTopMargin  + "-"
                    + pageBottomMargin  + "-"
                    + textTypeSetting  + "-"
                    + fontTag  + "-"
                    + version  + "-"
                    + kernelVersion + "-"
                    + ChineseFontSetting  + "-"
                    + EnglishFontSetting  + "-"
                    + DefaultFont;


    char* pMd5String =MD5Checker::GetInstance()->DK_MDString(UniqeTag.c_str());

    if(NULL == pMd5String)
    {
        DebugPrintf(DLC_CHENM,"pMd5String is NULL err");
        return "";
    }
    DebugPrintf(DLC_CHENM,"pMd5String is %s",pMd5String);
    std::string strMd5(pMd5String);

    // 将生成的MD5值中的小写字母转换为大写字母
    transform(strMd5.begin(), strMd5.end(), strMd5.begin(), ::toupper);
    DebugPrintf(DLC_CHENM,"strMd5.c_str() : %s",strMd5.c_str());
    return strMd5;
}

bool  SystemSettingInfo::SetNTPTime(void)
{
	const char *timesrv[3] = {"210.72.145.44", "pool.ntp.org", "time.nist.gov" };  
	char buf[128] = {0};
	for(int i = 0; i < 3; i++)
	{
		memset(buf, 0, 128);
		sprintf(buf, "ntpdate %s", timesrv[i]);
		string str = SystemUtil::ReadStringFromShellOutput(buf);
		if(str.find("adjust time server", 0) != string::npos)  
		{
			SystemUtil::ExecuteCommand("hwclock -w -u");
			return true;
		}
	}

	return false;	
}

const char* SystemSettingInfo::GetDefaultFontByCharset(DK_CHARSET_TYPE charset)
{
    switch (charset)
    {
        case DK_CHARSET_ANSI:
            return GetConfigValueString(CKI_DefaultEnglishFont);
        default:
            return GetConfigValueString(CKI_DefaultChineseFont);
    }
}

int SystemSettingInfo::CharSetToConfigId(DK_CHARSET_TYPE charset)
{
    return DK_CHARSET_ANSI == charset ? CKI_EnglishFont : CKI_ChineseFont;
}

bool SystemSettingInfo::SetFontByCharset(DK_CHARSET_TYPE charset, const char* fontname)
{
    if (NULL == fontname)
    {
        return false;
    }
    int configId = CharSetToConfigId(charset);
    return SetConfigValueString(configId, fontname);
}

const char* SystemSettingInfo::GetFontByCharset(DK_CHARSET_TYPE charset) const
{
    return GetConfigValueString(CharSetToConfigId(charset));
    
}

bool SystemSettingInfo::UseDefaultFont(DK_CHARSET_TYPE charset)
{
    return SetFontByCharset(charset, GetDefaultFontByCharset(charset));
}

const char* SystemSettingInfo::GetSysDefaultFont() const
{
    return GetConfigValueString(CKI_SysDefaultFont);
}

int SystemSettingInfo::GetVolume() const
{
    return GetConfigValueInt(CKI_Volume);
}

int SystemSettingInfo::GetConfigValueInt(int index) const
{
    return atoi(GetConfigValueString(index));
}

bool SystemSettingInfo::SetConfigValueInt(int index, int value)
{
    return Config::SetConfigValueInt(index, value);
}

const char* SystemSettingInfo::GetConfigValueString(int index) const
{
    return Config::GetConfigValueString(index);
}

const char* SystemSettingInfo::GetConfigName(int index) const
{
    return Config::GetConfigName(index);
}

bool SystemSettingInfo::SetConfigValueString(int index, const char* s)
{
    return Config::SetConfigValueString(index, s);
}

bool SystemSettingInfo::SetFontNameChanged(bool changed)
{
    return SetConfigValueInt(CKI_FontnameChange, changed);
}


bool SystemSettingInfo::SetProgressBarMode(int mode)
{
    return SetConfigValueInt(CKI_ProgressBarMode, MakeInRange(mode, 0, 1));
}

bool SystemSettingInfo::SetFontRender(int render)
{
    return SetConfigValueInt(CKI_FontRender, MakeInRange(render, 0, 1));
}

int SystemSettingInfo::GetEpubMobiMetaDataMode() const
{
    //界面显示顺序为元数据，文件名，实际0为文件名，1为元数据，为统一界面代码，这里转化一下
    return 1 - MakeInRange(GetConfigValueInt(CKI_EpubMobiMetaData), 0, 1);
}

bool SystemSettingInfo::SetEpubMobiMetaDataMode(int mode)
{
    mode = MakeInRange(mode, 0, 1);
    return SetConfigValueInt(CKI_EpubMobiMetaData, 1 - mode);

}
bool SystemSettingInfo::IsInstantTurnPageModel() const
{
    return GetConfigValueInt(CKI_InstantTurnMode) != 0;
}

int SystemSettingInfo::GetInitReadingOrder() const
{
    return GetConfigValueInt(CKI_InitReadingOrder);
}

bool SystemSettingInfo::SetInitReadingOrder(int order)
{
    return SetConfigValueInt(CKI_InitReadingOrder, order);
}

bool SystemSettingInfo::IsFontNameChanged() const
{
    return GetConfigValueInt(CKI_FontnameChange) != 0;
}

int SystemSettingInfo::GetTurnPageFlag() const
{
    return GetConfigValueInt(CKI_TurnPageCustoms);
}

bool SystemSettingInfo::SetTurnPageFlag(int flag)
{
    return SetConfigValueInt(CKI_TurnPageCustoms, flag);
}

bool SystemSettingInfo::ShouldTreatUpDownGestureAsPageSwitch() const
{
    return GetConfigValueInt(CKI_ReadingUpDownGestureCustoms);
}

bool SystemSettingInfo::SetTreatUpDownGestureAsPageSwitch(bool v)
{
    return SetConfigValueInt(CKI_ReadingUpDownGestureCustoms, v);
}

DK_FileSorts SystemSettingInfo::GetBookSortType() const
{
    int type = GetConfigValueInt(CKI_BookSortType);
    switch (type)
    {
        case RecentlyAdd:
        case RecentlyReadTime:
        case Name:
        case DIRECTORY:
            break;
        default:
            type = DIRECTORY;
            break;
    }
    return (DK_FileSorts)type;
}

bool SystemSettingInfo::SetBookSortType(DK_FileSorts fileSortType)
{
    return SetConfigValueInt(CKI_BookSortType, fileSortType);
}

DK_FileSorts SystemSettingInfo::GetMediaSortType() const
{
    int type = GetConfigValueInt(CKI_MediaSortType);
    switch (type)
    {
        case RecentlyAdd:
        case RecentlyReadTime:
        case Name:
        case DIRECTORY:
            break;
        default:
            type = DIRECTORY;
            break;
    }
    return (DK_FileSorts)type;
}

bool SystemSettingInfo::SetMediaSortType(DK_FileSorts fileSortType)
{
    return SetConfigValueInt(CKI_MediaSortType, fileSortType);
}

bool SystemSettingInfo::IsScreenSaverUnlock() const
{
    return GetConfigValueInt(CKI_ScreenSaverUnlock);
}

const char* SystemSettingInfo::GetScreenSaverPassword() const
{
    return GetConfigValueString(CKI_ScreenSaverPassword);
}

bool SystemSettingInfo::SetScreenSaverPassword(const char* password)
{
    return SetConfigValueString(CKI_ScreenSaverPassword, password);
}

const char* SystemSettingInfo::GetSinaWeiboToken() const
{
    return GetConfigValueString(CKI_SWToken);
}

bool SystemSettingInfo::SetSinaWeiboToken(const char* token)
{
    return SetConfigValueString(CKI_SWToken, token);
}

int SystemSettingInfo::GetSinaWeiboTokenTime() const
{
    return GetConfigValueInt(CKI_SWTokenTime);
}

bool SystemSettingInfo::SetSinaWeiboTokenTime(int tokenTime)
{
    return SetConfigValueInt(CKI_SWTokenTime, tokenTime);
}

const char* SystemSettingInfo::GetSinaWeiboUid() const
{
    return GetConfigValueString(CKI_SWUid);
}
bool SystemSettingInfo::SetSinaWeiboUid(const char* uid)
{
    return SetConfigValueString(CKI_SWUid, uid);
}

const char* SystemSettingInfo::GetSinaWeiboUserName() const
{
    return GetConfigValueString(CKI_SWUserName);
}
bool SystemSettingInfo::SetSinaWeiboUserName(const char* userName)
{
    return SetConfigValueString(CKI_SWUserName, userName);
}

const char* SystemSettingInfo::GetSinaWeiboUserMail() const
{
    return GetConfigValueString(CKI_SWUserMail);
}

bool SystemSettingInfo::SetSinaWeiboUserMail(const char* weiboMail)
{
    return SetConfigValueString(CKI_SWUserMail, weiboMail);
}

std::string SystemSettingInfo::GetLanguage() const
{
    std::string lang = GetConfigValueString(CKI_Language);
    DK_AUTO(availableLangs, GetAvailableLanguages());
    if (std::find(availableLangs.begin(), availableLangs.end(), lang) == availableLangs.end())
    {
        lang = StringManager::GetPCSTRById(CHINESE);
    }
    return lang;
}

std::vector<std::string> SystemSettingInfo::GetAvailableLanguages() const
{
     
    std::vector<std::string> otherLangs = PathManager::GetFilesInPath(PathManager::GetLanguagePath().c_str());
    for (DK_AUTO(cur, otherLangs.begin());
            cur != otherLangs.end();
            ++cur)
    {
        *cur = PathManager::GetFileNameWithoutExt(cur->c_str());
    }
    std::sort(otherLangs.begin(), otherLangs.end());
    std::vector<std::string> results;
    results.push_back(StringManager::GetPCSTRById(CHINESE));
    results.insert(results.end(), otherLangs.begin(), otherLangs.end());
    return results; 
}

bool SystemSettingInfo::IsCurrentLanguageChinese() const
{
    const char* s1 = StringManager::GetPCSTRById(CHINESE);
    const char* s2 = "Chinese";
    string lang = GetLanguage();
    return (lang.compare(s1) == 0) || (lang.compare(s2) == 0);
}


int SystemSettingInfo::GetTTSLanguage() const
{
    return GetConfigValueInt(CKI_TTSLanguage);
}

bool SystemSettingInfo::SetTTSLanguage(int lang)
{
    return SetConfigValueInt(CKI_TTSLanguage, lang);
}

int SystemSettingInfo::GetTTSSpeaker() const
{
    return GetConfigValueInt(CKI_TTSSpeaker);
}

bool SystemSettingInfo::SetTTSSpeaker(int speaker)
{
    return SetConfigValueInt(CKI_TTSSpeaker, speaker);
}

int SystemSettingInfo::GetTTSSpeed() const
{
    return GetConfigValueInt(CKI_TTSSpeed);
}

bool SystemSettingInfo::SetTTSSpeed(int speed)
{
    return SetConfigValueInt(CKI_TTSSpeed, speed);
}

int SystemSettingInfo::GetTTSSpeakStyle() const
{
    return GetConfigValueInt(CKI_TTSSpeakStyle);
}

bool SystemSettingInfo::SetTTSSpeakStyle(int style)
{
    return SetConfigValueInt(CKI_TTSSpeakStyle, style);
}

int SystemSettingInfo::GetReadingLayout() const
{
	return GetConfigValueInt(CKI_ReadingLayout);
}

bool SystemSettingInfo::SetReadingLayout(int mode)
{
	return SetConfigValueInt(CKI_ReadingLayout, mode);
}

BookListMode SystemSettingInfo::GetBookListMode() const
{
    return (BookListMode)MakeInRange(GetConfigValueInt(CKI_BookListMode), BLM_MIN, BLM_MAX);
}

bool SystemSettingInfo::SetBookListMode(BookListMode mode)
{
    return SetConfigValueInt(CKI_BookListMode, MakeInRange(mode, BLM_MIN, BLM_MAX));
}

void SystemSettingInfo::ResetFontSize()
{
    m_fontDirSize = -1;
}
