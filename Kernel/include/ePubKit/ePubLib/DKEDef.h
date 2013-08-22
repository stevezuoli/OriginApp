//===========================================================================
// Summary:
//		ePubLib的数据类型定义。
// Usage:
//		Null
// Remarks:
//		Null
// Date:
//		2011-09-15
// Author:
//		Wang Yi (wangyi@duokan.com)
//===========================================================================

#ifndef __KERNEL_EPUBKIT_EPUBLIB_EXPORT_DKEDEF_H__
#define __KERNEL_EPUBKIT_EPUBLIB_EXPORT_DKEDEF_H__

//===========================================================================

#include "KernelRetCode.h"
#include "DKEError.h"
#include "DKEEPUBDef.h"

//===========================================================================

#ifdef _LINUX
	#ifndef NOT_SUPPORT_FVISIBILITY
		#define EPUB_API					__attribute__((visibility("default")))
		#define EPUBSTDMETHODCALLTYPE		__attribute__((stdcall))
		#define EPUBCALLBACK				__attribute__((stdcall))
	#else
		#define EPUB_API					
		#define EPUBSTDMETHODCALLTYPE	
		#define EPUBCALLBACK				
	#endif
#else
	/*#ifdef EPUBLIB_EXPORTS
		#define EPUB_API __declspec(dllexport)
	#else
		#define EPUB_API __declspec(dllimport)
	#endif*/

    #define EPUB_API					
	#define EPUBSTDMETHODCALLTYPE		__stdcall
	#define EPUBCALLBACK				__stdcall
#endif

#if defined(_ANDROID_LINUX) || defined(_IOS_LINUX)
#define DKE_SUPPORT_MOBI 0
#else
#define DKE_SUPPORT_MOBI 1
#endif

#if defined(_ANDROID_LINUX) || defined(_IOS_LINUX) || defined(_KINDLE_LINUX)
#define DKE_SUPPORT_DRM_SAVE 0
#define DKE_SUPPORT_CSS_ERROR 0
#define DKE_SUPPORT_BOOK_CHECK 0
#define DKE_SUPPORT_BOOK_EXPORT 0
#else
#define DKE_SUPPORT_DRM_SAVE 1
#define DKE_SUPPORT_CSS_ERROR 1
#define DKE_SUPPORT_BOOK_CHECK 1
#define DKE_SUPPORT_BOOK_EXPORT 1
#endif

#if defined(_ANDROID_LINUX) || defined(_KINDLE_LINUX)
#define DKE_SUPPORT_PAGE_TABLE 0
#else
#define DKE_SUPPORT_PAGE_TABLE 1
#endif

#if defined(_X86_LINUX) || defined(_WIN32) || defined(_MAC_LINUX)
#define DKE_SUPPORT_HTML 1
#define DKE_SUPPORT_FONT_DOWNLOAD 1
#define DKE_SUPPORT_DUMP 1
#else
#define DKE_SUPPORT_HTML 0
#define DKE_SUPPORT_FONT_DOWNLOAD 0
#define DKE_SUPPORT_DUMP 0
#endif


#define EPUBAPI				extern "C" EPUB_API bool
#define EPUBAPI_(type)		extern "C" EPUB_API type

#define EPUBDOCHANDLE				DK_VOID*
#define MOBIDOCHANDLE               DK_VOID*
#define HTMLDOCHANDLE               DK_VOID*

class IDKECallBackManager
{
public:
	//-------------------------------------------
	// Summary:
	//		输出解析信息
	//-------------------------------------------
	//virtual DK_VOID SendProcMessage(DK_WCHAR* pMessage) = 0;

	//-------------------------------------------
	// Summary:
	//		输出要解析的页码
	//-------------------------------------------
	//virtual DK_VOID SendPageNumParsed(DK_INT nPageNum) = 0;

	//-------------------------------------------
	// Summary:
	//		通过字体名获取字体文件路径
	//-------------------------------------------
	//virtual DK_WCHAR* GetFontFileByName(DK_WCHAR* pFontName, DK_CHARSET_TYPE charset) = 0;

	//-------------------------------------------
	// Summary:
	//		通过多个字体名获取字体文件路径、字体名称等信息
	//-------------------------------------------
	//virtual DKEFONTMATCHRESULT GetFontFileInfo(const DKEFONTITEM** pCurFontItemArr, DK_UINT uCount, DK_CHARSET_TYPE charset) = 0;

	//-------------------------------------------
	// Summary:
	//		通过图像数据获取图像像素宽高
	//-------------------------------------------
	//virtual DK_VOID GetImagePixel(DKFILEINFO *pFileInfo, DK_LONG &lPixelX, DK_LONG &lPixelY) = 0;

public:
	virtual ~IDKECallBackManager() {};
};

//===========================================================================

#endif // #ifndef __DKEDEF_HEADERFILE__
