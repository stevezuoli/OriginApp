//===========================================================================
// Summary:
//     Filter的工厂类
// Usage:
//     Null
// Remarks:
//     Null
// Date:
//     2011-09-20
// Author:
//     Zhai Guanghe (zhaigh@duokan.com)
//===========================================================================

#ifndef __KERNEL_COMMONLIBS_CONTAINER_DKFILTERFACTORY_H__
#define __KERNEL_COMMONLIBS_CONTAINER_DKFILTERFACTORY_H__

#include "DkFilter.h"

class IDkStream;

class DkFilterFactory
{
public:
    static IDkEncode * CreateEncoder(FilterAlgorithm type);
    static IDkDecode * CreateDecoder(FilterAlgorithm type);
    static IDkStream * GetEncodeStream(FilterAlgorithm type, IDkStream *pStream, const FilterParam &filterParam, DK_UINT enoughEncodeLength = 0);
    static IDkStream * GetDecodeStream(FilterAlgorithm type, IDkStream *pStream, const FilterParam &filterParam, DK_UINT enoughDecodeLength = 0);

public:
    static DK_ReturnCode GetEnoughEncodeLength(FilterAlgorithm type, DK_UINT inLen, DK_UINT *pOutLen);
    static DK_ReturnCode EncodeBuffer(FilterAlgorithm type, const FilterParam *pFilterParam, const DK_BYTE *pIn, DK_UINT inLen, DK_BYTE *pOut, DK_UINT *pOutLen);
    static DK_ReturnCode GetEnoughDecodeLength(FilterAlgorithm type, const FilterParam *pFilterParam, DK_UINT inLen, DK_UINT *pOutLen);
    static DK_ReturnCode DecodeBuffer(FilterAlgorithm type, const FilterParam *pFilterParam, const DK_BYTE *pIn, DK_UINT inLen, DK_BYTE *pOut, DK_UINT *pOutLen);

public: // 保证pID有33字节空间
    static DK_ReturnCode GetLocalBookID(IDkStream *pStream, DK_CHAR *pID);
    static DK_ReturnCode GetLocalBookID(const DK_CHAR *pFileName, DK_CHAR *pID);
    static DK_ReturnCode GetLocalBookID(const DK_WCHAR *pFileName, DK_CHAR *pID);

public: // for cloud，先Flate压缩再加密
    static DK_ReturnCode EncodeStreamToFile(IDkStream *pStream, FilterAlgorithm cryptType, const FilterParam &cryptParam, const DK_WCHAR *pDestFileName);

public: // 保存bitmap到文件或流
    static DK_ReturnCode SaveBitmapToJpeg(const DK_BITMAPBUFFER_DEV &bitmap, DK_UINT quality, const DK_CHAR *pFileName);
    static DK_ReturnCode SaveBitmapToJpeg(const DK_BITMAPBUFFER_DEV &bitmap, DK_UINT quality, IDkStream *pWStream);
    static DK_ReturnCode SaveBitmapToPng(const DK_BITMAPBUFFER_DEV &bitmap, const DK_CHAR *pFileName);
    static DK_ReturnCode SaveBitmapToPng(const DK_BITMAPBUFFER_DEV &bitmap, IDkStream *pWStream);
};

#endif // __KERNEL_COMMONLIBS_CONTAINER_DKFILTERFACTORY_H__
