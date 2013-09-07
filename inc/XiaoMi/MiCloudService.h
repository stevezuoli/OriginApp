#ifndef __MICLOUD_SERVICE_H__
#define __MICLOUD_SERVICE_H__

#include "XiaoMi/IXiaoMiService.h"
#include "GUI/EventListener.h"
#include "GUI/EventSet.h"
#include "GUI/EventArgs.h"
#include "interface.h"

#include "XiaoMi/MiCloudServiceResult.h"
#include "KernelDef.h"
#include <string>
#include <vector>
using namespace std;

namespace xiaomi
{

enum FileOperation
{
    MICLOUD_UNKNONW = 0,
    MICLOUD_DOWNLOADING,
    MICLOUD_UPLOADING,
};

class FileCreatedArgs: public EventArgs
{
public:
    FileCreatedArgs()
        : succeeded(false)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new FileCreatedArgs(*this);
    }
    bool succeeded;
    string local_file_path;
    MiCloudServiceResultCreateFileSPtr result;
};

class FileDownloadedArgs: public EventArgs
{
public:
    FileDownloadedArgs()
        : succeeded(false)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new FileDownloadedArgs(*this);
    }
    bool succeeded;
    string file_id;
    string local_file_path;
    string reason;
};


class FileProgressArgs: public EventArgs
{
public:
    FileProgressArgs()
        : file_info()
        , current(-1.0)
        , total(-1.0)
        , speed(-1.0)
        , operation(MICLOUD_UNKNONW)
    {}
    virtual EventArgs* Clone() const
    {
        return new FileProgressArgs(*this);
    }
    MiCloudFileSPtr file_info;
    double current;
    double total;
    double speed;
    FileOperation operation;
    string local_path;
};

class QuotaRetrievedArgs: public EventArgs
{
public:
    QuotaRetrievedArgs()
        : succeeded(false)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new QuotaRetrievedArgs(*this);
    }
    bool succeeded;
    MiCloudServiceResultGetQuotaSPtr result;
};

class ChildrenRetrievedArgs: public EventArgs
{
public:
    ChildrenRetrievedArgs()
        : succeeded(false)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new ChildrenRetrievedArgs(*this);
    }
    bool succeeded;
    string parent_dir_id;
    MiCloudServiceResultGetChildrenSPtr result;
};

class DirectoryCreatedArgs: public EventArgs
{
public:
    DirectoryCreatedArgs()
        : succeeded(false)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new DirectoryCreatedArgs(*this);
    }
    bool succeeded;
    string dir_path;
    MiCloudServiceResultCreateDirSPtr result;
};

class RequestDownloadFinishedArgs: public EventArgs
{
public:
    RequestDownloadFinishedArgs()
        : succeeded(false)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new RequestDownloadFinishedArgs(*this);
    }
    bool succeeded;
    string file_id;
};

class FileDeletedArgs: public EventArgs
{
public:
    FileDeletedArgs()
        : succeeded(false)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new FileDeletedArgs(*this);
    }
    bool succeeded;
    string file_id;
};

class DirectoryDeletedArgs: public EventArgs
{
public:
    DirectoryDeletedArgs()
        : succeeded(false)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new DirectoryDeletedArgs(*this);
    }
    bool succeeded;
    string directory_id;
};

class GetInfoArgs: public EventArgs
{
public:
    GetInfoArgs()
        : succeeded(false)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new GetInfoArgs(*this);
    }
    bool succeeded;
    MiCloudServiceResultGetInfoSPtr result;
};

class FileOperationDelegate : public EventSet
{
public:
    static const char* EventFileOperationProgress;

public:
    FileOperationDelegate()
        : file_info_()
        , file_opt_(MICLOUD_UNKNONW)
    {}
    virtual ~FileOperationDelegate() {}

    void setFile(MiCloudFileSPtr file) { file_info_ = file; }
    void setOperation(FileOperation file_opt) { file_opt_ = file_opt; }
    void setFileLocalPath(const string& path) { local_path_ = path; }

public:
    inline void onProgress(double cur, double total, double speed)
    {
        FileProgressArgs progress_args;
        progress_args.operation = file_opt_;
        progress_args.file_info = file_info_;
        progress_args.current = cur;
        progress_args.total = total;
        progress_args.speed = speed;
        progress_args.local_path = local_path_;
        FireEvent(EventFileOperationProgress, progress_args);
    }

private:
    MiCloudFileSPtr file_info_;
    FileOperation   file_opt_;
    string          local_path_;
};

class XiaoMiServiceFactory;
class MiCloudService 
    : public IXiaoMiService
    , public EventListener
    , public EventSet
{
public:
    // Events
    static const char* EventFileCreated;
    static const char* EventQuotaRetrieved;
    static const char* EventChildrenRetrieved;
    static const char* EventDirectoryCreated;
    static const char* EventRequestDownloadFinished;
    static const char* EventFileDeleted;
    static const char* EventDirectoryDeleted;
    static const char* EventInfoGot;

public:
    bool CreateFile(const string& dirId, const string& filePath);
    bool CreateDirectory(const string& parentDirId, const string& dirName);
    bool CreateDirectory(const string& filePath);
    bool CommitFile(const string& uploadPath, const string& upload, const string& fileMeta, const vector<string>& commitMetas);
    bool RequestDownload(const string& fileId, const string& fileName = "");
    bool DeleteFile(const string& fileId);
    bool DeleteDirectory(const string& dirId);
    bool GetChildren(const string& dirId = "0", const string& offset = "0", const string& limit = "1000");
    bool GetInfo(const string& filePath);
    bool GetQuota();

    // file delegater
    FileOperationDelegate* fileOperationDelegate() { return &file_delegate_; }

protected:
    virtual bool Init();

private:
    bool OnGetQuotaFinished(const EventArgs& args);
    bool OnGetInfoFinished(const EventArgs& args);
    bool OnGetChildrenFinished(const EventArgs& args);
    bool OnCreateFileFinished(const EventArgs& args);
    bool OnDeleteFileFinished(const EventArgs& args);
    bool OnCreateDirectoryFinished(const EventArgs& args);
    bool OnCommitFileFinished(const EventArgs& args);
    bool OnDeleteDirectoryFinished(const EventArgs& args);
    bool OnRequestDownloadFinished(const EventArgs& args);

    string GetRequestDownloadUrl(const string& fileId) const;
    string GetRequestUploadUrl() const;
private:
    MiCloudService();
    DISALLOW_COPY_AND_ASSIGN(MiCloudService);
    string DataForCreateFile(const string& filePath);
    string DataForCommitFile(const string& fileMeta, const vector<string>& commitMetas);

private:
    FileOperationDelegate file_delegate_;

private:
    friend class XiaoMiServiceFactory;
};//MiCloudService

}///xiaomi
#endif//__MICLOUD_SERVICE_H__
