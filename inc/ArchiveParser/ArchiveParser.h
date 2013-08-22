#ifndef __DKREADER_ARCHIVEPARSER_IARCHIVEPARSER_H__
#define __DKREADER_ARCHIVEPARSER_IARCHIVEPARSER_H__

#include "dkObjBase.h"
#include "dkWinError.h"
#include "SequentialStream.h"

class IDkStream;

enum ArchiveEncryption
{
    ARCHIVE_NO_ENCRYPTION, 
    ARCHIVE_NAME_ENCRYPTION, 
    ARCHIVE_CONTENT_ENCRYPTION
};

class IArchiverParser
{
public:
    virtual ~IArchiverParser() {}

    STDMETHOD(IsEncryption)(ArchiveEncryption *pEncryption) = 0;

    //-------------------------------------------
    // Summary:
    //     设置密码（长度不超过255）
    //-------------------------------------------
    STDMETHOD(SetPassword)(const char *pPassword) = 0;

    //-------------------------------------------
    // Summary:
    //     获取压缩包中项目的数量（包括文件和文件夹）
    //-------------------------------------------
    STDMETHOD(GetFileNum)(int *piFileNum) = 0;

    //-------------------------------------------
    // Summary:
    //     从索引号获取项目名（相对路径），使用ReleaseFileName释放
    //-------------------------------------------
    STDMETHOD(GetFileNameByIndex)(int iIndex, char **ppFileName) = 0;

    //-------------------------------------------
    // Summary:
    //     从项目名（相对路径）获取索引号
    //-------------------------------------------
    STDMETHOD(GetIndexByName)(const char *pFilePath, int *piIndex) = 0;

    //-------------------------------------------
    // Summary:
    //     释放由GetFileNameByIndex返回的资源
    //-------------------------------------------
    STDMETHOD(ReleaseFileName)(char *pFileName) = 0;

    //-------------------------------------------
    // Summary:
    //     从索引号获取项目是否是文件
    //-------------------------------------------
    STDMETHOD(CheckIsFileByIndex)(int iIndex, bool *pbIsFile) = 0;

    //-------------------------------------------
    // Summary:
    //     从索引号获取项目大小（解压后）
    //-------------------------------------------
    STDMETHOD(GetFileSizeByIndex)(int iIndex, int *piFileSize) = 0;

    //-------------------------------------------
    // Summary:
    //     从索引号获取指定长度的项目数据（解压后）
    //     若iDataSize大于项目数据长度，后面补0
    //-------------------------------------------
    STDMETHOD(GetFileContentByIndex)(int iIndex, unsigned char *pFileData, int iDataSize) = 0;

    //-------------------------------------------
    // Summary:
    //     从索引号获取项目数据流（解压后）
    //-------------------------------------------
    STDMETHOD(GetFileStreamByIndex)(int iIndex, IDkStream **ppStream) = 0;

    //-------------------------------------------
    // Summary:
    //     从索引号获取项目数据流（解压后）
    //-------------------------------------------
    STDMETHOD(GetFileStreamByIndex)(int iIndex, DkFormat::IDKSequentialStream **ppStream) = 0;
};

#endif // __DKREADER_ARCHIVEPARSER_IARCHIVEPARSER_H__
