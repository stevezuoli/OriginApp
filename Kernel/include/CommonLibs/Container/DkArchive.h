//===========================================================================
// Summary:
//     Archive 接口
// Usage:
//     Null
// Remarks:
//     Null
// Date:
//     2011-09-05
// Author:
//     Zhai Guanghe (zhaigh@duokan.com)
//===========================================================================

#ifndef __KERNEL_COMMONLIBS_CONTAINER_DKARCHIVE_H__
#define __KERNEL_COMMONLIBS_CONTAINER_DKARCHIVE_H__

#include "KernelType.h"
#include "ContainerRetCode.h"

class IDkStream;

class IDkArchive
{
public:
    enum TYPE
    {
        DK_ZIP,
        DK_RAR,
        DK_FOLDER, 
        DK_PDB
    };

    enum ENCRYPT_MODE
    {
        NOT_ENCRYPTED,
        ENCRYPTED,
        NAME_ENCRYPTED
    };

    enum CASE
    {
        SENSITIVE, 
        INSENSITIVE
    };

    virtual ~IDkArchive(){}

    virtual DK_BOOL Refresh() = 0;

    virtual ENCRYPT_MODE  GetEncryptMode() const = 0;

    //-------------------------------------------
    // Summary:
    //     设置压缩包密码。
    // Remarks:
    //     若pPassword为空或长度为0，设置密码长度为0。
    //-------------------------------------------
    virtual DK_VOID       SetPassword(const DK_CHAR *pPassword) = 0;

    //-------------------------------------------
    // Summary:
    //     获取压缩包项目数目。
    // ReturnValue:
    //     >= 0  : 项目数目
    //     -1    : 获取失败（压缩包解析失败）
    // Remarks:
    //     项目数目0只在增删压缩包文件时会出现，
    //     例如创建新压缩包或者删除全部压缩包项目。
    //-------------------------------------------
    virtual DK_INT        GetEntryNum() const = 0;

    //-------------------------------------------
    // Summary:
    //      根据index获取对应项目名。
    // ReturnValue:
    //     != DK_NULL : 项目名
    //     DK_NULL    : 获取失败（压缩包解析失败或index非法）
    // Remarks:
    //     index从0开始，返回的字符串空间需在外部用DK_FREE释放。
    //-------------------------------------------
    virtual DK_CHAR *     GetEntryNameByIndex(DK_INT index) const = 0;

    //-------------------------------------------
    // Summary:
    //      根据index获取对应项目大小（解压后）。
    // ReturnValue:
    //     >= 0 : 项目大小
    //     -1   : 获取失败（DK_FALSE == IsFile(index)）
    //-------------------------------------------
    virtual DK_FILESIZE_T GetEntrySizeByIndex(DK_INT index) const = 0;

    //-------------------------------------------
    // Summary:
    //      根据index获取对应项目的数据流。
    // ReturnValue:
    //     != DK_NULL : 项目数据流
    //     DK_NULL    : 获取失败（DK_FALSE == IsFile(index)）
    // Remarks:
    //     如返回成功，需要在外部使用delete释放
    //-------------------------------------------
    virtual IDkStream *   GetEntryStreamByIndex(DK_INT index) const = 0;

    //-------------------------------------------
    // Summary:
    //      根据index获取对应项目的数据。
    // ReturnValue:
    //     != DK_NULL : 项目数据指针
    //     DK_NULL    : 获取失败（DK_FALSE == IsFile(index)）
    // Remarks:
    //     如返回成功，需要在外部使用DK_FREE释放，内存空间长度为GetEntrySizeByIndex(index) + 1（末尾补'\0'）
    //-------------------------------------------
    virtual DK_BYTE *     GetEntryContentByIndex(DK_INT index) const = 0;

    //-------------------------------------------
    // Summary:
    //      根据index确定当前项目是否是文件。
    // ReturnValue:
    //     DK_TRUE  : 是文件
    //     DK_FALSE : 不是文件或压缩包解析失败或index非法
    //-------------------------------------------
    virtual DK_BOOL       IsFile(DK_INT index) const = 0;

    //-------------------------------------------
    // Summary:
    //      根据项目名获取index。
    // ReturnValue:
    //     >= 0 : 项目index
    //     -1   : 找不到匹配项
    //-------------------------------------------
    virtual DK_INT        GetEntryIndexByName(const DK_CHAR *pFileName, CASE charCase = SENSITIVE) const = 0;

    //-------------------------------------------
    // Summary:
    //      替换数据到Archive，外部保证是在原数据流基础上做的加密等操作，
    //      不会改变CRC等原始文件的数据。
    //      新的Entry会标记为compMethod = 0，由外部管理如何解压/解密
    //      传入的数据会在SaveTo之后被释放
    //-------------------------------------------
    virtual DK_ReturnCode ReplaceEntryStreamByIndex(DK_BYTE *pReplaceData, DK_UINT32 repalceLen, DK_INT index) = 0;

    //-------------------------------------------
    // Summary:
    //      保存到文件或路径
    //-------------------------------------------
    virtual DK_ReturnCode SaveTo(const DK_CHAR *pFileName) = 0;
};

#endif // __KERNEL_COMMONLIBS_CONTAINER_DKARCHIVE_H__
