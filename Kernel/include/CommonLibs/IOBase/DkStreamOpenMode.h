//===========================================================================
// Summary:
//     DkStream中的一些公用函数
// Usage:
//     Null
// Remarks:
//     Null
// Date:
//     2011-09-27
// Author:
//     Zhai Guanghe(zhaigh@duokan.com)
//===========================================================================

#ifndef __KERNEL_COMMONLIBS_IOBASE_STREAMCOMMONFUNCTION_H__
#define __KERNEL_COMMONLIBS_IOBASE_STREAMCOMMONFUNCTION_H__

#include "DkStream.h"

inline DK_BOOL IsClose(IDkStream::OPEN_MODE openMode)
{
    return IDkStream::DK_CLOSED == openMode;
}

inline DK_BOOL ErrMode(IDkStream::OPEN_MODE openMode)
{
    return IDkStream::DK_ERROR_STREAM == openMode
           || IDkStream::DK_OPEN_FAIL == openMode;
}

inline DK_BOOL IsOpen(IDkStream::OPEN_MODE openMode)
{
    return !ErrMode(openMode) && !IsClose(openMode);
}

inline DK_BOOL CanRead(IDkStream::OPEN_MODE openMode)
{
    return IsOpen(openMode)
           && IDkStream::DK_WRITE_ONLY != openMode;
}

inline DK_BOOL CanWrite(IDkStream::OPEN_MODE openMode)
{
    return IsOpen(openMode)
           && IDkStream::DK_READ_ONLY != openMode;
}

#endif // __KERNEL_COMMONLIBS_IOBASE_STREAMCOMMONFUNCTION_H__
