#ifndef    __SYSTEMINFO_H__
#define    __SYSTEMINFO_H__

#include <unistd.h>
#include "ConfigInfo.h"
#include "../Utility/CString.h"
#include "interface.h"
#include <string>
#include <sys/time.h>
#include <sys/statfs.h>
#include "KernelType.h"
#include "Common/FileSorts_DK.h"
#include "Common/Defs.h"



//系统语言
enum LANGUAGE
{
    LANG_CHS = 0                                    //简体中文
    ,LANG_CHT = 1                                   //繁体中文
    ,LANG_ENG = 2                                   //英文
};

enum IMETYPE
{
    IME_EN
    ,IME_PY
    ,IME_WB
};

//屏保类型
enum    ScreenType
{
    ST_CUSTOM = 0                               //用户自定义
    ,ST_SYSTEM = 1                              //系统
};

//图片旋转
enum Imagerotate
{
    IMG_ROTATE_NONE = 0,
    IMG_ROTATE_90 = 90,
    IMG_ROTATE_180 = 180,
    IMG_ROTATE_270 = 270
};

//图片漫画模式
enum Imagecartoon
{
    IMG_CARTOON_ALL = 0,
    IMG_CARTOON_LR = 1,
    IMG_CARTOON_RL = 2
};

//界面布局
enum ReadingLayoutMode
{
	READINGLAYOUT_FULLSCREEN = 0,
	READINGLAYOUT_SHOWMESSAGE = 1
};

//输入法
class IME
{
public:
    int m_cIme;
    IMETYPE     m_eIme;
    int         m_OptionIME;
public:
    IME();
    void Init();
    bool SetDefaultIME(IMETYPE ime);    
    bool SetOptionIME(int option, BOOL bIsAdd);
    IMETYPE GetIme();
    int GetOptionIME();
};

// 时间
// 内部时间始终采用24时制。 外部显示可选择12时制或者24时制
//是否是24小时制
class DKTime
{
private:
	int tzeast;
	int tzhour;
	int tzminute;
	time_t tm;
    UINT32  year;
    UINT32  month;
    UINT32  day;
    UINT32  hour;
    UINT32  minute;
    UINT32  second;
public:
	bool  bIS24Hour;

public:
    DKTime();
	void Init();
	bool isLeapYear(int nYear);
	int GetDaysOfMonth(int nYear, int nMonth); 
	bool SetSysTime();
	bool SetLocalTime(time_t *time);
	bool SetYear(UINT32 nYear);
	bool SetMonth(UINT32 nMonth);
	bool SetDay(UINT32 nDay);
	bool SetHour(UINT32 nHour);
	bool SetMinute(UINT32 nMinute);
	bool SetSecond(UINT32 nSecond);	
	UINT32 GetYear(void);
	UINT32 GetMonth(void);
	UINT32 GetDay(void);
	UINT32 GetHour(void);
	UINT32 GetMinute(void);
	UINT32 GetSecond(void);
	int GetTimeZoneEast();
	int GetTimeZoneFull();
	int GetTimeZoneHalf();
	int GetTimeZoneSeconds();
	void SetTimeZone();	
	void SetTimeZoneEast(int east);
	void SetTimeZoneFull(int hour);
	void SetTimeZoneHalf(int minute);
	void SetTimeZoneSeconds(int offset_in_seconds);
	void SetTimeSystem(bool bValue);
	int GetTimeSystem();
};

//磁盘空间
class DKDiskSpace
{
public:
    DKDiskSpace();
    UINT32 GetTotalSize();
    UINT32 GetFreeSize();
    UINT32 GetFreePercent();
private:
    UINT32 m_TotalMiB;                                   //磁盘空间总大小
    UINT32 m_FreeMiB;                                    //磁盘空间剩余
    struct Info                                         //占用空间
    {
        UINT32 m_nSize;                                 //占用空间大小
        UINT32 m_nPercent;                              //百分比
    }book, video, audio;                               //书籍、音频、视频
private:
    void GetDiskSpaceMiB();
	void GetFileSpaceInfo();
    int FootPrint_Book_Get();
    int FootPrint_Video_Get();
    int FootPrint_Audio_Get();
};


class SystemSettingInfo
{
private:

    DKDiskSpace        m_DiskSpace;
    static SystemSettingInfo* m_Instance;
    static const UINT s_fontSizeTable[];

public:
    IME        m_IME;
    DKTime    m_Time;
    static SystemSettingInfo* GetInstance();

    Imagerotate    m_eImgRotate;
    Imagecartoon  m_eImgCartoon;

public:
    SystemSettingInfo();

    std::string  GetDuokanBookMD5(int majorVersion, int minorVersion);
    std::string GetLanguage() const;
    bool SetLanguage(const char* language);

    bool    SetAutoPage(int csecs);
    bool    SetTypeSetting(int iTypesetting);
    bool    SetRepaintPage(int iCount);
    bool    SetBlackLevel(int nlevel);
    bool    SetPdfSharpen(int nlevel);
    bool    SetFont(int nIndex);
    bool    SetScreenStyle(ScreenType type);
    void    SetFavorSysPath(bool value);


    // 设置排版信息
    bool    SetLineGap(int iLineGap);
    bool    SetParaSpacing(int iParaSpacing);
    bool    SetTabStop(int iTabStop);
    bool    SetBookIndent(int iBookIndent); 
    bool    SetFontSize(int _size);
    
    // 获取排版信息
    int  GetLineGap();
    int  GetParaSpacing();
    int  GetTabStop();
    int  GetBookIndent();
    int  GetFontSize();
    
    //设置获取 Reader 页面的Margin
    int GetPageHorizontalMargin(int mode);
    bool SetPageHorizontalMargin(int mode, int iMargin);
    int GetPageTopMargin(int mode);
    bool SetPageTopMargin(int mode, int iMargin);
    int GetPageBottomMargin(int mode);
    bool SetPageBottomMargin(int mode, int iMargin);

    bool SetTextDarkLevel(int _iLevel);
    int GetTextDarkLevel();
    //设置相应的值,并写入配置文件
    bool    SetSecsPerPage(int sec);

    bool    SetIsFavorSysPath(BOOL isFavor);
//    bool    SetVersion(CString version);
//    bool    SetDiskSpace(DKDiskSpace space);
//    IME DKTime的设置在相应类里面实现
//    bool    SetIME(IME ime);
//    bool    SetTime(DKTime time);

    bool    SetImageRotate(Imagerotate eImgRotate);
    bool     SetImageCartoon(Imagecartoon eImgCartoon);
    
    //获取相应的值
    int            GetSecsPerPage();
    int            GetBlackLevel();
    int            GetPdfSharpen();
    bool        GetIsFavorSysPath();
    CString        GetVersion();

    bool           GetIsAppFirstLoading();
//    IME            GetIME();
//    DKTime        GetTime();
    DKDiskSpace    GetDiskSpace();
    int            GetRepaintPage();
    int            GetTypeSetting();
    int            GetLanguageIndex();
    Imagerotate    GetImgRotate();
       Imagecartoon GetImgCartoon();
    int             GetDKSmartLayoutMode();
    int             GetScreenSavers();    
    LPCSTR        GetMTDPathLP();
    bool         GetBatteryRatioMode();
    bool         SetBatteryRatioMode(bool mode);
    int            GetProgressBarMode() const;
    bool SetProgressBarMode(int mode);
	int 			GetFontRender() const;
    bool SetFontRender(int render);
	bool			GetMetadataOpen();
	bool			SetNTPTime();
    //设置和读取自动翻页的时间
    int GetAutoFlipTimeSecond();
    bool SetAutoFlipTimeSecond(int iSec);
    bool SetFontStyle_FamiliarOrTraditional(int _iLevel);//0为简体，1为繁体
    int GetFontStyle_FamiliarOrTraditional();

    std::string GetKindleMAC();
    bool SetKindleMAC(const std::string& mac);
    const char* GetDefaultFontByCharset(DK_CHARSET_TYPE charset);
    bool SetFontByCharset(DK_CHARSET_TYPE charset, const char* fontname);
    const char* GetFontByCharset(DK_CHARSET_TYPE charset) const;
    bool UseDefaultFont(DK_CHARSET_TYPE charset);
    const char* GetSysDefaultFont() const;
    int GetVolume() const;
    bool SetFontNameChanged(bool changed);
    bool IsFontNameChanged() const;
    int GetEpubMobiMetaDataMode() const;
    bool SetEpubMobiMetaDataMode(int mode);
    bool IsInstantTurnPageModel() const;
    int GetInitReadingOrder() const;
    bool SetInitReadingOrder(int order);
    
    // 左侧点击上翻页或下翻页,0上翻，1下翻
    int GetTurnPageFlag() const;
    bool SetTurnPageFlag(int flag);
    
    // 上下手势翻章或翻页, 0翻章，1翻页
    bool ShouldTreatUpDownGestureAsPageSwitch() const;
    bool SetTreatUpDownGestureAsPageSwitch(bool v);

    // Sort type of local book
    Field GetBookSortType() const;
    bool SetBookSortType(Field fileSortType);

    Field GetCloudBookSort() const;
    bool SetCloudBookSortType(Field sort_type);
    
    // Sort type of local media
    Field GetMediaSortType() const;
    bool SetMediaSortType(Field fileSortType);
    
    bool IsScreenSaverUnlock() const;
    const char* GetScreenSaverPassword() const;
    bool SetScreenSaverPassword(const char* password);
    std::vector<std::string> GetAvailableLanguages() const;

    bool IsCurrentLanguageChinese() const;

    int GetTTSLanguage() const;
    bool SetTTSLanguage(int lang);

    int GetTTSSpeaker() const;
    bool SetTTSSpeaker(int speaker);

    int GetTTSSpeed() const;
    bool SetTTSSpeed(int speed);

    int GetTTSSpeakStyle() const;
    bool SetTTSSpeakStyle(int style);

	int GetReadingLayout() const;
    bool SetReadingLayout(int mode);

    ModelDisplayMode GetModelDisplayMode() const;
    bool SetModelDisplayMode(ModelDisplayMode mode);

    const char* GetSinaWeiboToken() const;
    bool SetSinaWeiboToken(const char* token);

    int GetSinaWeiboTokenTime() const;
    bool SetSinaWeiboTokenTime(int tokenTime);

    const char* GetSinaWeiboUid() const;
    bool SetSinaWeiboUid(const char* uid);

    const char* GetSinaWeiboUserName() const;
    bool SetSinaWeiboUserName(const char* userName);

    const char* GetSinaWeiboUserMail() const;
    bool SetSinaWeiboUserMail(const char* weiboMail);
    void ResetFontSize();
private:
    const char* GetConfigName(int index) const;
    bool SetConfigValueInt(int index, int value);
    bool SetConfigValueString(int index, const char* s);
    static int CharSetToConfigId(DK_CHARSET_TYPE charset);
    const char* GetConfigValueString(int index) const;
    int GetConfigValueInt(int index) const;
    int m_fontDirSize;

};

#endif
