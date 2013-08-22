 /*************************************************************************
 * 北京多看科技有限公司
 * Copyright (C) 2011
 * File Name: ConfigInfo.h
 * Description: 包含Config文件的操作函数及变量声明
 * Version =
 * Create by: zzc
 * Create Date: 2011-05-10
 * _______________________________________________________________________
 * Modify by:       
 * Modify Date:     
 * Modify cause:
 *************************************************************************/

#ifndef _CONFIGINFO_H
#define _CONFIGINFO_H

#include <limits.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//计算数组大小
#define dim(x) (sizeof(x)/sizeof(x[0]))

//保存配置文件信息的全局数组
#define CONFIG_KEY_LIST(MACRO_NAME)\
            MACRO_NAME(PageDown_Speed,       "20", "20")\
            MACRO_NAME(BoldLevel,            "0", "0")\
            MACRO_NAME(SerializedBookUpdate, "0", "0")\
            MACRO_NAME(Language,             "0", "0")\
            MACRO_NAME(DefaultIME,           "0", "0")\
            MACRO_NAME(OptionalIME,          "3", "3")\
            MACRO_NAME(ScreenSavers,         "1", "1")\
            MACRO_NAME(LoadKdDir,            "0", "0")\
            MACRO_NAME(TimeSys,              "1", "1")\
            MACRO_NAME(MTDPath,              "/mnt/us", "/mnt/us")\
            MACRO_NAME(RepaintPage,          "5", "5")\
            MACRO_NAME(TypeSetting,          "1", "1")\
            MACRO_NAME(SmartLayout,          "1", "1")\
            MACRO_NAME(ImageRotate,          "0", "0")\
            MACRO_NAME(ImageCartoon,         "0", "0")\
            MACRO_NAME(SysDefaultFont,       "WenQuanYi Micro Hei", "WenQuanYi Micro Hei")\
            MACRO_NAME(ChineseFont,          "方正兰亭黑_GBK", "方正兰亭黑_GBK")\
            MACRO_NAME(DefaultChineseFont,   "方正兰亭黑_GBK", "方正兰亭黑_GBK")\
            MACRO_NAME(LocalFont,            "", "")\
            MACRO_NAME(EnglishFont,          "Gentium Book Basic", "Gentium Book Basic")\
            MACRO_NAME(DefaultEnglishFont,   "Gentium Book Basic", "Gentium Book Basic")\
            MACRO_NAME(ChineseFont_1,        "", "")\
            MACRO_NAME(ChineseFont_2,        "", "")\
            MACRO_NAME(EnglishFont_1,        "", "")\
            MACRO_NAME(EnglishFont_2,        "", "")\
            MACRO_NAME(LocalFont_1,          "", "")\
            MACRO_NAME(LocalFont_2,          "", "")\
            MACRO_NAME(Volume,               "10", "10")\
            MACRO_NAME(BookSortType,         "0", "0")\
            MACRO_NAME(InitReadingOrder,     "0", "0")\
            MACRO_NAME(MediaSortType,        "0", "0")\
            MACRO_NAME(PdfSharpen,           "0", "0")\
            MACRO_NAME(ScreenSaverUnlock,    "0", "0")\
            MACRO_NAME(ScreenSaverPassword,  "", "")\
            MACRO_NAME(InstantTurnMode,      "0", "0")\
            MACRO_NAME(TTSSpeed,             "2", "2")\
            MACRO_NAME(TTSSpeakStyle,        "1", "1")\
            MACRO_NAME(TTSLanguage,          "1", "1")\
            MACRO_NAME(TTSSpeaker,           "0", "0")\
            MACRO_NAME(ProgressBarMode,      "0", "0")\
            MACRO_NAME(FontRender,		     "0", "0")\
            MACRO_NAME(ReadingLayout,	     "0", "0")\
            MACRO_NAME(EpubMobiMetaData,	 "1", "1")\
            MACRO_NAME(BatteryRatioMode,     "1", "1")\
            MACRO_NAME(BookFontSize,         "24", "24")\
            MACRO_NAME(BookLineGap,          "120", "120")\
            MACRO_NAME(BookParaSpacing,      "90", "90")\
            MACRO_NAME(BookTabStop,          "2", "2")\
            MACRO_NAME(BookIndent,           "2", "2")\
			MACRO_NAME(PageHorizontalMarginMode, "42", "54")\
            MACRO_NAME(PageTopMarginMode,    "36", "46")\
            MACRO_NAME(PageBottomMarginMode, "36", "46")\
            MACRO_NAME(PageHorizontalMargin, "26", "33")\
            MACRO_NAME(PageTopMargin,        "15", "19")\
            MACRO_NAME(PageBottomMargin,     "19", "24")\
            MACRO_NAME(TextDarkLevel,        "0", "0")\
			MACRO_NAME(AutoFlipTimeSecond,   "20", "20")\
			MACRO_NAME(TurnPageCustoms,        "0", "0")\
			MACRO_NAME(ReadingUpDownGestureCustoms, "0", "0")\
			MACRO_NAME(FontnameChange,       "0", "0") \
			MACRO_NAME(AppFirstLoading,       "1", "1")\
            MACRO_NAME(FontStyle_FamiliarOrTraditional,     "0", "0")\
            MACRO_NAME(KindleMAC,     "", "")\
            MACRO_NAME(BookListMode,     "0", "0")\
            MACRO_NAME(SWToken,     "", "")\
            MACRO_NAME(SWTokenTime,     "0", "0")\
            MACRO_NAME(SWUid,     "", "")\
            MACRO_NAME(SWUserName,     "", "")\
            MACRO_NAME(SWUserMail,     "", "")\
            MACRO_NAME(TIMEZONE,     "", "")\

#define DEFINE_CONFIGINFO_KEYNAME_ENUM(Index, kt, kpw)\
        CKI_##Index,
enum
{
    CONFIG_KEY_LIST(DEFINE_CONFIGINFO_KEYNAME_ENUM)
    CKI_COUNT
};
#undef DEFINE_CONFIGINFO_KEYNAME_ENUM

typedef struct tagkeymap
{
    const char* keyname;
    char    keyvalue[256];
} keymap;

// 加载配置文件信息
void LoadConfigInfo();

class Config
{
public:
    static const char* GetConfigValueString(int index);
    static const char* GetConfigName(int index);
    static bool SetConfigValueInt(int index, int value);
    static bool SetConfigValueString(int index, const char* s);
};
void InitConfigMap();
#endif

