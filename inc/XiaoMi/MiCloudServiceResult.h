/*
 * =====================================================================================
 *       Filename:  MiCloudServiceResult.h
 *    Description:  Data Structure returned from MiCloud service
 *
 *        Version:  1.0
 *        Created:  08/19/2013 10:47:39 AM
 * =====================================================================================
 */

#ifndef __MICLOUD_SERVICE_RESULT_H__
#define __MICLOUD_SERVICE_RESULT_H__

#include "XiaoMi/IXiaoMiServiceResult.h"
#include "XiaoMi/MiCloudFile.h"
#include "XiaoMi/KSSCloudInfo.h"

#include <string>
#include <tr1/memory>
#include "Utility/JsonObject.h"
using namespace std;

namespace xiaomi
{
class MiCloudServiceResultBase;
class MiCloudServiceResultGetQuota;
class MiCloudServiceResultGetInfo;
class MiCloudServiceResultGetChildren;
class MiCloudServiceResultCreateFile;
class MiCloudServiceResultCreateDir;
class MiCloudServiceResultDeleteFile;
class MiCloudServiceResultDeleteDir;
class MiCloudServiceResultRequestDownload;

typedef tr1::shared_ptr<MiCloudServiceResultBase> MiCloudServiceResultBaseSPtr;
typedef tr1::shared_ptr<MiCloudServiceResultGetQuota> MiCloudServiceResultGetQuotaSPtr;
typedef tr1::shared_ptr<MiCloudServiceResultGetChildren> MiCloudServiceResultGetChildrenSPtr;
typedef tr1::shared_ptr<MiCloudServiceResultCreateFile> MiCloudServiceResultCreateFileSPtr;
typedef tr1::shared_ptr<MiCloudServiceResultCreateDir> MiCloudServiceResultCreateDirSPtr;
typedef tr1::shared_ptr<MiCloudServiceResultDeleteDir> MiCloudServiceResultDeleteDirSPtr;
typedef tr1::shared_ptr<MiCloudServiceResultDeleteFile> MiCloudServiceResultDeleteFileSPtr;
typedef tr1::shared_ptr<MiCloudServiceResultRequestDownload> MiCloudServiceResultRequestDownloadSPtr;
typedef tr1::shared_ptr<MiCloudServiceResultGetInfo> MiCloudServiceResultGetInfoSPtr;

class MiCloudServiceResultBase : public IXiaoMiServiceResult
{
public:
    MiCloudServiceResultBase()
        : m_code(-1)
        , m_retryIntervalTime(-1)
    {
    }

    bool IsError() const
    {
        return m_code != 0;
    }

    string GetErrorReason() const
    {
        return m_errorReason;
    }

    string GetDescription() const
    {
        return m_description;
    }

    int GetRetryIntervalTime() const
    {
        return m_retryIntervalTime;
    }
protected:
    /**
     * @brief Parse parse the top-level of the result, if succeeded, the "data" filed will be stored
     *          in m_dataJsonObj, used by derived class
     *
     * @param the parsed string
     *
     * @return parsed successed or not
     */
    bool Parse(const char *);

protected:
    string m_result;
    string m_description;
    int m_code;
    //error reason:
    string m_errorReason;
    //right data:
    JsonObjectSPtr m_dataJsonObj;
    int m_retryIntervalTime;
};//MiCloudServiceResult

class MiCloudServiceResultGetQuota : public MiCloudServiceResultBase
{
public:
    MiCloudServiceResultGetQuota()
        : m_total(-1)
        , m_available(-1)
        , m_nsUsed(-1)
    {
    }

    static MiCloudServiceResultGetQuota* CreateResult(const char* result);

    int64_t GetTotal() const
    {
        return m_total;
    }

    int64_t GetAvailable() const
    {
        return m_available;
    }

    int64_t GetNsUsed() const
    {
        return m_nsUsed;
    }
protected:
    virtual bool Init(const char* result);

private:
    int64_t m_total;
    int64_t m_available;
    int64_t m_nsUsed;
};///MiCloudServiceResultGetQuota

class MiCloudServiceResultGetChildren : public MiCloudServiceResultBase
{
public:
    static MiCloudServiceResultGetChildren* CreateResult(const char* result);
    MiCloudFileSPtrList GetFileList() const
    {
        return m_fileList;
    }

protected:
    virtual bool Init(const char* result);
private:
    MiCloudFileSPtrList m_fileList;
};

class MiCloudServiceResultCreateFile : public MiCloudServiceResultBase
{
public:
    static MiCloudServiceResultCreateFile* CreateResult(const char* result);

    MiCloudFileSPtr GetFileInfo() const
    {
        return m_fileInfo;
    }
    KSSUploadInfoSPtr GetKSSInfo() const
    {
        return m_kssInfo;
    }
    std::string GetKSSStrInfo() const
    {
        return m_kssStrInfo;
    }
protected:
    virtual bool Init(const char* result);

private:
    MiCloudFileSPtr m_fileInfo;
    KSSUploadInfoSPtr m_kssInfo;
    std::string m_kssStrInfo;
};//MiCloudServiceResultCreateFile

class MiCloudServiceResultCreateDir : public MiCloudServiceResultBase
{
public:
    static MiCloudServiceResultCreateDir* CreateResult(const char* result);
    MiCloudFileSPtr GetDirInfo() const
    {
        return m_dirInfo;
    }
protected:
    virtual bool Init(const char* result);

private:
    MiCloudFileSPtr m_dirInfo;
};//MiCloudServiceResultCreateDir

class MiCloudServiceResultDeleteDir : public MiCloudServiceResultBase
{
public:
    static MiCloudServiceResultDeleteDir* CreateResult(const char* result);

protected:
    virtual bool Init(const char* result);
};//MiCloudServiceResultDeleteDir

class MiCloudServiceResultDeleteFile : public MiCloudServiceResultBase
{
public:
    static MiCloudServiceResultDeleteFile* CreateResult(const char* result);

protected:
    virtual bool Init(const char* result);
};//MiCloudServiceResultDeleteDir

class MiCloudServiceResultRequestDownload : public MiCloudServiceResultBase
{
public:
    static MiCloudServiceResultRequestDownload* CreateResult(const char* result);
    std::string GetKSSStrInfo() const
    {
        return m_kssStrInfo;
    }
protected:
    virtual bool Init(const char* result);

private:
    std::string m_kssStrInfo;
};//MiCloudServiceResultDownloadFile

class MiCloudServiceResultGetInfo : public MiCloudServiceResultBase
{
public:
    static MiCloudServiceResultGetInfo* CreateResult(const char* result);
    MiCloudFileSPtr GetFileInfo() const
    {
        return m_fileInfo;
    }

protected:
    virtual bool Init(const char* result);
private:
    MiCloudFileSPtr m_fileInfo;
};//MiCloudServiceResultGetInfo
}//xiaomi
#endif//__MICLOUD_SERVICE_RESULT_H__
