//===========================================================================
// Summary:
//		CharsetDetect.h
// Usage:
//		±àÂë¼ì²â¿â
//			...
// Remarks:
//		Null
// Date:
//		2011-11-23
// Author:
//		Peng Feng(pengf@duokan.com)
//===========================================================================
#ifndef __KERNEL_COMMONLIBS_CHARSETDETECT_CHARSETDETECT_H__
#define __KERNEL_COMMONLIBS_CHARSETDETECT_CHARSETDETECT_H__

#include "KernelEncoding.h"

class IDkStream;
class CharsetDetector
{
public:
    static EncodingUtil::Encoding DetectEncoding(const DK_CHAR*pData, DK_UINT dataLen);
    static EncodingUtil::Encoding DetectEncoding(IDkStream* pStream);
private:
    CharsetDetector();
};
#endif
