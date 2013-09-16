#ifndef __MICLOUD_DOWNLOAD_TASK_BUILDER_H__
#define __MICLOUD_DOWNLOAD_TASK_BUILDER_H__

#include "DownloadManager/DownloadTask.h"
#include <map>
#include <string>

using namespace dk;
using namespace std;
namespace xiaomi
{
class MiCloudDownloadTaskBuilder
{
public:
    static DownloadTaskSPtr BuildCreateFileTask(const string& dirId, const string& name, const string& sha1,
        const string& data, const string& retry = "0");
    static DownloadTaskSPtr BuildPhonyCreateFileTask(const string& dirId, const string& filePath, const string& displayName);
    static DownloadTaskSPtr BuildCreateDirectoryTask(const string& parentDirId, const string& name);
    static DownloadTaskSPtr BuildCreateDirectoryTask(const string& dirPath);
    static DownloadTaskSPtr BuildCommitFileTask(const string& uploadId, const string& data, const string& retry = "0");
    static DownloadTaskSPtr BuildRequestDownloadTask(const string& fileId, const string& retry = "0");
    static DownloadTaskSPtr BuildDeleteFileTask(const string& fileId);
    static DownloadTaskSPtr BuildDeleteDirectoryTask(const string& dirId);
    static DownloadTaskSPtr BuildGetChildrenTask(const string& dirId,
            const string& offset, const string& limit);
    static DownloadTaskSPtr BuildGetInfoTask(const string& filePath);
    static DownloadTaskSPtr BuildGetQuotaTask();

    static string BuildRequestDownloadUrl(const string& fileId);
    static string BuildCreateFileUrl();

    static map<string, string> GenerateDefaultParams();
private:
    static string BuildCreateDirectoryUrl();
    static string BuildCommitFileUrl();
    static string BuildDeleteFileUrl(const string& fileId);
    static string BuildDeleteDirectoryUrl(const string& dirId);
    static string BuildGetChildrenUrl(const string& dirId);
    static string BuildGetInfoUrl();
    static string BuildGetQuotaUrl();

};//MiCloudDownloadTaskBuilder
}//xiaomi

#endif//__MICLOUD_DOWNLOAD_TASK_BUILDER_H__
