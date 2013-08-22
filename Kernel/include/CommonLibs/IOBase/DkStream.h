//===========================================================================
// Summary:
//     Stream 接口
// Usage:
//     Null
// Remarks:
//     Null
// Date:
//     2011-09-05
// Author:
//     Zhai Guanghe(zhaigh@duokan.com)
//===========================================================================

#ifndef __KERNEL_COMMONLIBS_IOBASE_STREAM_H__
#define __KERNEL_COMMONLIBS_IOBASE_STREAM_H__

#include "KernelBaseType.h"
#include "IOBaseRetCode.h"
#include "KernelRenderType.h"

struct StreamInfo
{
    // 图片属性，当前只有JPX返回
    DK_COLORSPACE_TYPE colorSpace;
    DK_UINT            width;
    DK_UINT            height;
};

class IDkStream
{
public:
    enum OPEN_MODE
    {
        DK_ERROR_STREAM, // 参数错误造成的失败
        DK_OPEN_FAIL,    // 打开失败
        DK_CLOSED,       // 已关闭
        DK_READ_ONLY,
        DK_WRITE_ONLY,
        DK_READ_WRITE,
        DK_WRITE_READ
    };

    enum TYPE
    {
        MEMORY_STREAM, 
        FILE_STREAM, 
        SUB_STREAM, 
        ZIP_STREAM, 
        FILTER_STREAM, 
        MOBI_STREAM
    };

    enum SEEK_ORIGIN
    {
        DK_SEEK_SET,
        DK_SEEK_CUR,
        DK_SEEK_END
    };

    enum ReleseMethod
    {
        RELESE_INSIDE, 
        RELESE_OUTSIDE
    };

    virtual ~IDkStream(){}

    virtual TYPE          GetType() const = 0;

    //-------------------------------------------
    // Summary:
    //     创建当前流的一个副本。
    // Remarks:
    //     需要在外部delete
    //-------------------------------------------
    virtual IDkStream *   CreateCopy() = 0;
    virtual DK_ReturnCode Destroy() = 0;

    //-------------------------------------------
    // 与打开、关闭有关的接口
    //
    // Open: Parameters:
    //     [in] openMode : 打开模式
    // Open: ReturnValue:
    //     DKR_OK         : 打开成功
    //     DKR_INVALIDARG : 非法的打开模式
    //     DKR_OPENED     : 已经打开
    //     DKR_FAIL       : 打开失败
    // Open: Remarks:
    //     默认模式为只读。
    //     只有DK_CLOSED模式的流可以打开。
    //
    // Close: ReturnValue:
    //     DKR_OK          : 关闭成功
    //     DKR_DID_NOTHING : 已经关闭
    //-------------------------------------------
    virtual DK_ReturnCode Open(OPEN_MODE openMode = DK_READ_ONLY) = 0;
    virtual DK_ReturnCode Close() = 0;
    virtual OPEN_MODE     OpenMode() const = 0;
    virtual DK_ReturnCode GetStreamInfo(StreamInfo *pStreamInfo) const = 0;

    //-------------------------------------------
    // 与Tell有关的接口
    //
    // Tell: ReturnValue:
    //     >= 0  : 流的当前位置
    //     -1    : 未打开
    //
    // GetLength: ReturnValue:
    //     >= 0  : 流的长度
    //     -1    : 未打开
    //-------------------------------------------
    virtual DK_FILESIZE_T Tell() const = 0;
    virtual DK_FILESIZE_T GetLength() const = 0;
    virtual DK_BOOL       IsEOF() const = 0;

    //-------------------------------------------
    // 与Seek有关的接口
    //
    // ReturnValue:
    //     DKR_OK   : 成功
    //     DKR_FAIL : 失败
    //-------------------------------------------
    virtual DK_ReturnCode Seek(DK_FILESIZE_T offset, SEEK_ORIGIN origin) = 0;
    virtual DK_BOOL       CanSeekBack() const = 0;
    virtual DK_ReturnCode Reset() = 0;
    virtual DK_ReturnCode SkipByte() = 0;

    //-------------------------------------------
    // 与Read有关的接口
    //
    // Read: Parameters:
    //     [in] pDest     : 要读取到的内存位置
    //     [in] destLen   : 希望读取的长度
    //     [out] pReadLen : 实际读取的长度
    // Read: ReturnValue:
    //     DKR_OK   : 成功
    //     DKR_INVALIDARG : 非法的参数
    //     DKR_FAIL : 失败
    // Read: Remarks:
    //     若pReadLen传NULL，只有当读取到的长度和destLen一致时才返回成功
    //
    // GetByte: ReturnValue:
    //     >= 0  : 读取的Byte
    //     -1    : 读取失败
    //
    // PeekByte: ReturnValue:
    //     >= 0  : 读取的Byte
    //     -1    : 读取失败
    // PeekByte: Remarks:
    //     指针不移动
    //-------------------------------------------
    virtual DK_ReturnCode Read(DK_VOID *pDest, DK_FILESIZE_T destLen, DK_FILESIZE_T *pReadLen = DK_NULL) = 0;
    virtual DK_INT        GetByte() = 0;
    virtual DK_INT        PeekByte() = 0;
    virtual DK_ReturnCode ReadAll(DK_BYTE **ppDest, DK_FILESIZE_T *pReadLen) = 0;

    //-------------------------------------------
    // 与Write有关的接口
    //
    //-------------------------------------------
    virtual DK_ReturnCode Write(const DK_BYTE *pSrc, DK_FILESIZE_T writeLen) = 0;
    virtual DK_ReturnCode PutChar(const DK_BYTE ch) = 0;
    virtual DK_ReturnCode Flush() = 0;
    virtual DK_ReturnCode Clear() = 0;
};

#endif // __KERNEL_COMMONLIBS_IOBASE_STREAM_H__
