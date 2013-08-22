//===========================================================================
// Summary:
//		DKTDef.h
// Usage:
//	    txt lib鍩烘湰绫诲瀷瀹氫箟
// Remarks:
//		Null
// Date:
//		2011-11-14
// Author:
//		Peng Feng(pengf@duokan.com)
//===========================================================================
#ifndef __KERNEL_TXTKIT_TXTLIB_DKTDEF_H__
#define __KERNEL_TXTKIT_TXTLIB_DKTDEF_H__

#include "KernelType.h"

#ifdef _LINUX
	#ifndef NOT_SUPPORT_FVISIBILITY
		#define TXT_API					__attribute__((visibility("default")))
		#define TXTSTDMETHODCALLTYPE		__attribute__((stdcall))
		#define TXTCALLBACK				__attribute__((stdcall))
	#else
		#define TXT_API					
		#define TXTSTDMETHODCALLTYPE	
		#define TXTCALLBACK				
	#endif
#else
    #define TXT_API					
	#define TXTSTDMETHODCALLTYPE		__stdcall
	#define TXTCALLBACK				__stdcall
#endif

#define TXTAPI				extern "C" TXT_API bool
#define TXTAPI_(type)		extern "C" TXT_API type

typedef DK_VOID* TXTDOCHANDLE;
typedef DK_VOID* TXTRENDERHANDLE;

struct DKT_CHAR_INFO
{
    DK_WCHAR ch;
    DK_BOX boundingBox;                 // 外接矩形
    DK_UINT offsetInBytes;                // 文字的索引位置
};

struct DKT_CHAR_INFO_FOR_RENDER
{
    DK_WCHAR ch;
    DK_CHARSET_TYPE charset; 
    DK_BOX boundingBox; // 外接矩形
    DK_DOUBLE x, y; // 基线坐标
    DK_BOOL hyphen; // 是否hyphen
    DK_BOOL visible; // 是否可见
    DK_UINT index; // 在原文中的index
};

struct DKT_LINE_INFO_FOR_RENDER
{
    DK_BOX lineBox;
    DK_UINT lineStartIndex; // line第一个字符对应的index
};

enum DKT_HITTEST_TEXT_MODE
{
    DKT_HITTEST_TEXT_UNKNOWN,
    DKT_HITTEST_TEXT_SENTENCE,                      // 以整句文本方式 HitTest 页面文字
    DKT_HITTEST_TEXT_WORD                           // 以文本分词方式 HitTest 页面文字
};

enum DKT_SELECTION_MODE
{
    DKT_SELECTION_UNKNOWN,
    DKT_SELECTION_MIDDLECROSS,                      // 当坐标点越过中线时即选中当前文字 
    DKT_SELECTION_INTERCROSS,                       // 当坐标与当前字有交叉时即选中当前文字
    DKT_SELECTION_INTERCROSS_WORD                   // 当坐标与当前字有交叉时，该字所在词也将被选中
};

#endif
