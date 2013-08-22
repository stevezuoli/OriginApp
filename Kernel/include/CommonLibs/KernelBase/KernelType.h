//===========================================================================
// Summary:
//		KernelType.h
// Usage:
//		KernelBase库基本头文件，基本数据类型定义
//			...
// Remarks:
//		Null
// Date:
//		2011-08-27
// Author:
//		Peng Feng(pengf@duokan.com)
//===========================================================================

#ifndef	__KERNEL_COMMONLIBS_KERNELBASE_KERNELTYPE_H__
#define	__KERNEL_COMMONLIBS_KERNELBASE_KERNELTYPE_H__

//===========================================================================

#include <cstdlib>
#include <cstring>

#include "KernelBaseType.h"
#include "KernelRenderType.h"

//===========================================================================

// 字符集，以后还可以扩充
enum DK_CHARSET_TYPE
{
	DK_CHARSET_ANSI		= 0,
	DK_CHARSET_SHITJIS	= 128,	// SHIFTJIS_CHARSET, 日文
	DK_CHARSET_JOHAB	= 130,	// JOHAB_CHARSET, 韩文
	DK_CHARSET_GB		= 134,
	DK_CHARSET_BIG5		= 136
};

const DK_SIZE_T DKFILEEXTLENGTH = 32;
struct DKFILEINFO
{
	DK_WCHAR strFileExt[DKFILEEXTLENGTH];	// 文件后缀，暂时最长支持32个字符
	DK_LONG lFileDataLength;
	DK_BYTE* pFileDataBuf;

	DKFILEINFO()
	{
		memset(&strFileExt, 0, DKFILEEXTLENGTH * sizeof(DK_WCHAR));
		lFileDataLength = 0;
		pFileDataBuf = DK_NULL;
	};

};
typedef DKFILEINFO *PDKFILEINFO;

class IDkStream;
struct DKSTREAMINFO
{
	DK_WCHAR strFileExt[DKFILEEXTLENGTH];	// 文件后缀，暂时最长支持32个字符
	IDkStream* pStream;

	DKSTREAMINFO()
	{
		memset(&strFileExt, 0, DKFILEEXTLENGTH * sizeof(DK_WCHAR));
		pStream = DK_NULL;
	};

};
typedef DKSTREAMINFO *PDKSTREAMINFO;

// MIME 音视频格式定义，只添加最常用格式
// 参考网址 http://www.w3schools.com/html5/att_source_type.asp
enum DK_MIME_TYPE
{
    DK_MIME_UNKNOWN = 0,
    DK_MIME_VIDEO_OGG,
    DK_MIME_VIDEO_MP4,
    DK_MIME_VIDEO_WEBM,
    DK_MIME_AUDIO_OGG,
    DK_MIME_AUDIO_MPEG
};

// 枚举 ISO 639-1 Language Code
enum DK_LANG_CODE
{
    DK_LANG_UNKNOWN = 0,
    DK_LANG_DE,         // German
    DK_LANG_EN,         // English
    DK_LANG_IT,         // Italian
    DK_LANG_JA,         // Japanese
    DK_LANG_KO,         // Korean
    DK_LANG_RU,         // Russian
    DK_LANG_ZH,         // Chinese
    DK_LANG_ZH_HANS,    // Simplified Chinese (language+script)
    DK_LANG_ZH_HANT     // Traditional Chinese (language+script)
};

// 标点符号竖排时候的样式
enum DK_PUNCTUATION_VERTICAL_STYLE
{
    DK_PUNCTUATION_VERTICAL_STYLE_CHS = 0,	// chs style
    DK_PUNCTUATION_VERTICAL_STYLE_CHT,		// cht style
    DK_PUNCTUATION_VERTICAL_STYLE_JP		// jp style
};

struct DKTYPESETTING
{
    DK_BOOL ShrinkLineEndPunctuation;           // shrink line end punc
    DK_DOUBLE PuncShrinkRate;                   // punc max shrink rate, valid value 0.5 - 1.0， 1.0 will prevent punc shrinking
    DK_DOUBLE LineMaxStretchRate;               // line max stretch rate, used when setting align justify, negative for no limit
    DK_PUNCTUATION_VERTICAL_STYLE VPuncStyle;   // vertical punc layout style
    DK_BOOL EnableHyphenation;                  // enable hyphenation

    DKTYPESETTING()
        : ShrinkLineEndPunctuation(DK_FALSE)
        , PuncShrinkRate(0.6)
        , LineMaxStretchRate(-1.0)
        , VPuncStyle(DK_PUNCTUATION_VERTICAL_STYLE_CHT)
        , EnableHyphenation(DK_TRUE)
    {}
};

#endif
