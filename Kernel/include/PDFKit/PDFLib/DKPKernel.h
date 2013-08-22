//===========================================================================
// Summary:
//		导出宏和一些回调函数的定义。 
// Usage:
//		Null
// Remarks:
//		Null
// Date:
//		2011-05-16
// Author:
//		ZhangJingDan
//===========================================================================

#ifndef __KERNEL_PDFKIT_PDFLIB_DKPKERNEL_H__
#define __KERNEL_PDFKIT_PDFLIB_DKPKERNEL_H__

//===========================================================================

#ifdef _LINUX
	#ifdef DKPKERNEL_EXPORTS
	#define DKPKERNEL_API __attribute__((visibility("default")))
	#else
	#define DKPKERNEL_API 
	#endif

	#define DFECALLBACK	
#else
	#ifdef DKPKERNEL_EXPORTS
	#define DKPKERNEL_API __declspec(dllexport)
	#else
	#define DKPKERNEL_API __declspec(dllimport)
	#endif

	#define DFECALLBACK		__stdcall
#endif

#define DKPKERNELAPI			extern "C" DKPKERNEL_API DK_ReturnCode
#define DKPKERNELAPI_(type)		extern "C" DKPKERNEL_API type

//===========================================================================

// ImageOperate
#define WIDTHBYTES(width)	(((width)+31)/32 * 4)
// 得到调色板条目数 (bmiHeader:BITMAPINFOHEADER*)
#define BMPINFO2TABLENUM(bmiHeader)		(((bmiHeader)->biBitCount >= 24)?0: \
										(((bmiHeader)->biClrUsed != 0)? \
										(bmiHeader)->biClrUsed:(1<<(bmiHeader)->biBitCount)))
// 得到调色板大小 (bmiHeader:BITMAPINFOHEADER*)
#define BMPINFO2TABLESIZE(bmiHeader)	(BMPINFO2TABLENUM(bmiHeader) * 4 /* 4: sizeof(RGBQUAD) */)
// 获得bmpinfo数据长度 (info:BITMAPINFO* )
#define BMPINFOSIZE(info)			(BMPINFO2TABLESIZE(&((info)->bmiHeader)) + sizeof((info)->bmiHeader))
// 得到数据长度	(bmiHeader:BITMAPINFOHEADER*)
#define BMPINFO2DATASIZE(bmiHeader)	(WIDTHBYTES((bmiHeader)->biBitCount * (bmiHeader)->biWidth) * abs((bmiHeader)->biHeight))

// raster code
#define DFE_TRANSPAINT 0x00b8074a

#define RemoveAndDeletePointersInVecOrList(vecType, vec) \
	{ \
		vecType::iterator theIterator; \
		for (theIterator = vec.begin(); theIterator != vec.end(); theIterator++) \
			SafeDeletePointer(*theIterator); \
		vec.clear(); \
	}

#define RemoveAndReleasePointersInVecOrList(vecType, vec) \
	{ \
		vecType::iterator theIterator; \
		for (theIterator = vec.begin(); theIterator != vec.end(); theIterator++) \
			{\
			if (*theIterator != DK_NULL)\
				(*theIterator)->Release();\
			}\
		vec.clear(); \
	}

#define RemoveAndDeletePointersInMap(mapType, map) \
	{ \
		mapType::iterator theIterator; \
		for (theIterator = map.begin(); theIterator != map.end(); theIterator++) \
			delete theIterator->second; \
		map.clear(); \
	}

#define RemoveAndReleasePointersInMap(mapType, map) \
	{ \
		mapType::iterator theIterator; \
		for (theIterator = map.begin(); theIterator != map.end(); theIterator++) \
			(theIterator->second)->Release(); \
		map.clear(); \
	}

//===========================================================================
#endif
