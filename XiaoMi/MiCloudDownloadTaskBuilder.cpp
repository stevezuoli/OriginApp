
#include "XiaoMi/MiCloudDownloadTaskBuilder.h"
#include "XiaoMi/XiaomiSecureRequest.h"
#include "XiaoMi/XiaoMiServiceFactory.h"
#include "XiaoMi/XiaoMiConstants.h"
#include "KernelDef.h"
#include "DownloadManager/MiCloudRequestTask.h"

namespace xiaomi
{
    string MiCloudDownloadTaskBuilder::BuildCreateFileUrl()
    {
        char url[256] = {0};
        snprintf(url, DK_DIM(url),
                (XiaoMiConstants::s_micloudServiceBaseUrl + XiaoMiConstants::s_micloudCreateFileUrl).c_str(), 
                XiaoMiServiceFactory::GetMiCloudService()->GetUserID().c_str(),
                XiaoMiConstants::s_micloudNameSpace.c_str()
                );

        return url;
    }

    string MiCloudDownloadTaskBuilder::BuildCreateDirectoryUrl()
    {
        char url[256] = {0};
        snprintf(url, DK_DIM(url),
                (XiaoMiConstants::s_micloudServiceBaseUrl + XiaoMiConstants::s_micloudCreateDirUrl).c_str(), 
                XiaoMiServiceFactory::GetMiCloudService()->GetUserID().c_str(),
                XiaoMiConstants::s_micloudNameSpace.c_str()
                );

        return url;
    }

    string MiCloudDownloadTaskBuilder::BuildCommitFileUrl()
    {
        char url[256] = {0};
        snprintf(url, DK_DIM(url),
                (XiaoMiConstants::s_micloudServiceBaseUrl + XiaoMiConstants::s_micloudCommitFileUrl).c_str(), 
                XiaoMiServiceFactory::GetMiCloudService()->GetUserID().c_str(),
                XiaoMiConstants::s_micloudNameSpace.c_str()
                );

        return url;
    }

    string MiCloudDownloadTaskBuilder::BuildRequestDownloadUrl(const string& fileId)
    {
        char url[256] = {0};
        snprintf(url, DK_DIM(url),
                (XiaoMiConstants::s_micloudServiceBaseUrl + XiaoMiConstants::s_micloudRequestDownloadUrl).c_str(), 
                XiaoMiServiceFactory::GetMiCloudService()->GetUserID().c_str(),
                XiaoMiConstants::s_micloudNameSpace.c_str(),
                fileId.c_str()
                );

        return url;
    }

    string MiCloudDownloadTaskBuilder::BuildDeleteFileUrl(const string& fileId)
    {
        char url[256] = {0};
        snprintf(url, DK_DIM(url),
                (XiaoMiConstants::s_micloudServiceBaseUrl + XiaoMiConstants::s_micloudDeleteFileUrl).c_str(),
                XiaoMiServiceFactory::GetMiCloudService()->GetUserID().c_str(),
                XiaoMiConstants::s_micloudNameSpace.c_str(),
                fileId.c_str()
                );
        return url;
    }

    string MiCloudDownloadTaskBuilder::BuildDeleteDirectoryUrl(const string& dirId)
    {
        char url[256] = {0};
        snprintf(url, DK_DIM(url),
                (XiaoMiConstants::s_micloudServiceBaseUrl + XiaoMiConstants::s_micloudDeleteDirUrl).c_str(),
                XiaoMiServiceFactory::GetMiCloudService()->GetUserID().c_str(),
                XiaoMiConstants::s_micloudNameSpace.c_str(),
                dirId.c_str()
                );
        return url;
    }

    string MiCloudDownloadTaskBuilder::BuildGetChildrenUrl(const string& dirId)
    {
        char url[256] = {0};
        snprintf(url, DK_DIM(url),
                (XiaoMiConstants::s_micloudServiceBaseUrl + XiaoMiConstants::s_micloudGetChildrenUrl).c_str(),
                XiaoMiServiceFactory::GetMiCloudService()->GetUserID().c_str(),
                XiaoMiConstants::s_micloudNameSpace.c_str(),
                dirId.c_str()
                );
        return url;
    }

    string MiCloudDownloadTaskBuilder::BuildGetInfoUrl()
    {
        char url[256] = {0};
        snprintf(url, DK_DIM(url),
                (XiaoMiConstants::s_micloudServiceBaseUrl + XiaoMiConstants::s_micloudGetInfoUrl).c_str(),
                XiaoMiServiceFactory::GetMiCloudService()->GetUserID().c_str(),
                XiaoMiConstants::s_micloudNameSpace.c_str()
                );
        return url;
    }

    string MiCloudDownloadTaskBuilder::BuildGetQuotaUrl()
    {
        char url[256] = {0};
        snprintf(url, DK_DIM(url), 
                (XiaoMiConstants::s_micloudServiceBaseUrl + XiaoMiConstants::s_micloudGetQuotaUrl).c_str(), 
                XiaoMiServiceFactory::GetMiCloudService()->GetUserID().c_str(),
                XiaoMiConstants::s_micloudNameSpace.c_str()
                );
        return url;
    }

    DownloadTaskSPtr MiCloudDownloadTaskBuilder::BuildCreateFileTask(const string& dirId, const string& name, const string& sha1
            , const string& data, const string& retry)
    {
        map<string, string> params = GenerateDefaultParams();
        params["parent_id"] = dirId;
        params["name"] = name;
        params["sha1"] = sha1;
        params["retry"] = retry;
        params["data"] = data;

        return SecureRequest::BuildDownloadTask(
                BuildCreateFileUrl(),
                params,
                XiaoMiServiceFactory::GetMiCloudService()->GetCookies(), 
                XiaoMiServiceFactory::GetMiCloudService()->GetSecurity(), 
                DownloadTask::POST);
    }

    DownloadTaskSPtr MiCloudDownloadTaskBuilder::BuildPhonyCreateFileTask(const string& dirId, 
        const string& filePath, const string& displayName)
    {
       DownloadTaskSPtr task(new MiCloudCreateFileRequestTask(dirId, filePath, displayName)); 
       task->SetMethod(DownloadTask::POST);
       task->SetUrl(BuildCreateFileUrl().c_str());

       return task;
    }

    DownloadTaskSPtr MiCloudDownloadTaskBuilder::BuildCreateDirectoryTask(const string& parentDirId, const string& name)
    {
        map<string, string> params = GenerateDefaultParams();
        params["parent_id"] = parentDirId;
        params["name"] = name;

        return SecureRequest::BuildDownloadTask(
                BuildCreateDirectoryUrl(),
                params,
                XiaoMiServiceFactory::GetMiCloudService()->GetCookies(), 
                XiaoMiServiceFactory::GetMiCloudService()->GetSecurity(), 
                DownloadTask::POST);
    }

    DownloadTaskSPtr MiCloudDownloadTaskBuilder::BuildCreateDirectoryTask(const string& dirPath)
    {
        map<string, string> params = GenerateDefaultParams();
        params["path"] = dirPath;
        params["recursive"] = "true";

        return SecureRequest::BuildDownloadTask(
                BuildCreateDirectoryUrl(),
                params,
                XiaoMiServiceFactory::GetMiCloudService()->GetCookies(), 
                XiaoMiServiceFactory::GetMiCloudService()->GetSecurity(), 
                DownloadTask::POST);
    }

    DownloadTaskSPtr MiCloudDownloadTaskBuilder::BuildCommitFileTask(const string& uploadId, const string& data, const string& retry)
    {
        map<string, string> params = GenerateDefaultParams();
        params["upload_id"] = uploadId;
        params["retry"] = retry;
        params["data"] = data;

        return SecureRequest::BuildDownloadTask(
                BuildCommitFileUrl(),
                params,
                XiaoMiServiceFactory::GetMiCloudService()->GetCookies(), 
                XiaoMiServiceFactory::GetMiCloudService()->GetSecurity(), 
                DownloadTask::POST
                );
    }

    DownloadTaskSPtr MiCloudDownloadTaskBuilder::BuildRequestDownloadTask(const string& fileId, const string& retry)
    {
        map<string, string> params = GenerateDefaultParams();
        params["fileId"] = fileId;
        params["retry"] = retry;

        return SecureRequest::BuildDownloadTask(
                BuildRequestDownloadUrl(fileId),
                params,
                XiaoMiServiceFactory::GetMiCloudService()->GetCookies(), 
                XiaoMiServiceFactory::GetMiCloudService()->GetSecurity(), 
                DownloadTask::GET
                );
    }

    DownloadTaskSPtr MiCloudDownloadTaskBuilder::BuildDeleteFileTask(const string& fileId)
    {
        map<string, string> params = GenerateDefaultParams();
        params["fileId"] = fileId;
        params["permanent"] = "true";

        return SecureRequest::BuildDownloadTask(
                BuildDeleteFileUrl(fileId),
                params,
                XiaoMiServiceFactory::GetMiCloudService()->GetCookies(), 
                XiaoMiServiceFactory::GetMiCloudService()->GetSecurity(), 
                DownloadTask::POST
                );
    }

    DownloadTaskSPtr MiCloudDownloadTaskBuilder::BuildDeleteDirectoryTask(const string& dirId)
    {
        map<string, string> params = GenerateDefaultParams();
        params["dirId"] = dirId;
        params["permanent"] = "true";

        return SecureRequest::BuildDownloadTask(
                BuildDeleteDirectoryUrl(dirId),
                params,
                XiaoMiServiceFactory::GetMiCloudService()->GetCookies(), 
                XiaoMiServiceFactory::GetMiCloudService()->GetSecurity(), 
                DownloadTask::POST
                );
    }

    DownloadTaskSPtr MiCloudDownloadTaskBuilder::BuildGetChildrenTask(const string& dirId, const string& offset, const string& limit)
    {
        map<string, string> params = GenerateDefaultParams();
        params["dirId"] = dirId;
        params["offset"] = offset;
        params["limit"] = limit;

        return SecureRequest::BuildDownloadTask(
                BuildGetChildrenUrl(dirId),
                params,
                XiaoMiServiceFactory::GetMiCloudService()->GetCookies(), 
                XiaoMiServiceFactory::GetMiCloudService()->GetSecurity(), 
                DownloadTask::GET);
    }

    DownloadTaskSPtr MiCloudDownloadTaskBuilder::BuildGetInfoTask(const string& filePath)
    {
        map<string, string> params = GenerateDefaultParams();
        params["path"] = filePath;

        return SecureRequest::BuildDownloadTask(
                BuildGetInfoUrl(),
                params,
                XiaoMiServiceFactory::GetMiCloudService()->GetCookies(), 
                XiaoMiServiceFactory::GetMiCloudService()->GetSecurity(), 
                DownloadTask::POST);
    }

    DownloadTaskSPtr MiCloudDownloadTaskBuilder::BuildGetQuotaTask()
    {
        map<string ,string> params = GenerateDefaultParams();

        return SecureRequest::BuildDownloadTask(
                BuildGetQuotaUrl(),
                params,
                XiaoMiServiceFactory::GetMiCloudService()->GetCookies(), 
                XiaoMiServiceFactory::GetMiCloudService()->GetSecurity(), 
                DownloadTask::GET);
    }


    map<string, string> MiCloudDownloadTaskBuilder::GenerateDefaultParams()
    {
        map<string ,string> params;
        params["userId"] = XiaoMiServiceFactory::GetMiCloudService()->GetUserID();
        params["ns"] = XiaoMiConstants::s_micloudNameSpace;

        return params;
    }
}
