//===========================================================================
// Summary:
//     Zip File Archive
// Usage:
//     Null
// Remarks:
//     Null
// Date:
//     2011-09-05
// Author:
//     Zhai Guanghe(zhaigh@duokan.com)
//===========================================================================
#ifndef __KERNEL_COMMONLIBS_CONTAINER_ZIPARCHIVE_H__
#define __KERNEL_COMMONLIBS_CONTAINER_ZIPARCHIVE_H__

#include "DkArchive.h"

class IDkStream;
struct ZipDirent;
struct ZipCentralDir;

class ZipArchive : public IDkArchive
{
public:
    // 从流打开压缩包
    ZipArchive(IDkStream *pStream);
    virtual ~ZipArchive();

    virtual ENCRYPT_MODE  GetEncryptMode() { return m_encryptMode; }
    virtual DK_VOID       SetPassword(const DK_CHAR *pPassword);
    virtual DK_INT        GetEntryNum() { return m_entryNum; }
    virtual DK_CHAR *     GetEntryNameByIndex(DK_INT index);
    virtual DK_FILESIZE_T GetEntrySizeByIndex(DK_INT index);
    virtual IDkStream *   GetEntryStreamByIndex(DK_INT index);
    virtual DK_BYTE *     GetEntryContentByIndex(DK_INT index);
    virtual DK_BOOL       IsFile(DK_INT index);
    virtual DK_INT        GetEntryIndexByName(const DK_CHAR *pFileName, CASE charCase = SENSITIVE);
    virtual DK_ReturnCode ReplaceEntryStreamByIndex(DK_BYTE *pReplaceData, DK_UINT32 repalceLen, DK_INT index);
    virtual DK_ReturnCode SaveTo(const DK_CHAR *pFileName);
private:
    enum DirentType
    {
        CENTRAL, // 全局目录
        LOCAL    // 局部目录
    };

    // 解析压缩包的基础数据
    DK_BOOL     OpenArchive();
    // 读取包含CentralInfo数据的Buffer，外部Free
    DK_BOOL     ReadCentralInfoBuffer(DK_CHAR **ppBuf, DK_INT *pBufLen);
    // 会释放传入的pBuf
    DK_BOOL     GetCentralInfo(DK_CHAR *pBuf, DK_INT bufLen);

    // 释放申请的空间
    DK_VOID     Destroy();

    // 从包直接获取文件数据流（不做解压等处理）
    IDkStream * GetEntryBaseStreamByIndex(DK_INT index);

    // 读取目录信息，pDirent外部申请
    DK_BOOL     ReadDirent(ZipDirent *pDirent, DirentType type);
    DK_BOOL     WriteDirentToStream(DK_INT index, DirentType type, IDkStream *pNewStream);
    // 检查目录信息
    DK_BOOL     CheckLocalDirent(const ZipDirent *pLocalDirent, const ZipDirent *pCentralDirent);
    // 释放ReadDirent中可能申请的内存，pDirent外部释放
    DK_VOID     FreeDirent(ZipDirent *pDirent);

    // 字符串匹配
    DK_CHAR *   Memmem(DK_CHAR *pBig, DK_INT bigLen, const DK_CHAR *pLittle, DK_INT littleLen);
private:
    IDkStream *     m_pStream;         // Zip包的流
    ENCRYPT_MODE    m_encryptMode;     // 加密模式
    DK_CHAR         m_password[0x100]; // 密码
    DK_INT          m_entryNum;        // 目录项个数
    ZipCentralDir * m_pCentralDir;     // 全局目录
    DK_BOOL         m_changed;         // 是否被修改
};

#endif // __KERNEL_COMMONLIBS_CONTAINER_ZIPARCHIVE_H__
