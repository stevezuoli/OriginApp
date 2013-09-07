#include "XiaoMi/MiCloudService.h"
#include "XiaoMi/MiCloudManager.h"
#include "XiaoMi/XiaoMiConstants.h"
#include "XiaoMi/XiaomiSecureRequest.h"
#include "XiaoMi/MiCloudDownloadTaskBuilder.h"

#include "Utility/XmlUtil.h"
#include "Utility/PathManager.h"
#include "Utility/EncodeUtil.h"
#include "Utility/JsonObject.h"
#include "Utility/StringUtil.h"
#include "XMLDomDocument.h"
#include "Common/Defs.h"
#include "DownloadManager/DownloadManager.h"
#include "DownloadManager/DownloaderImpl.h"
#include "DownloadManager/DownloadTaskFactory.h"
#include "DownloadManager/DownloadMiCloudFileTask.h"
#include "interface.h"
#include "Thirdparties/KSSManager/DKKSSManager.h"

using namespace dk::utility;

std::tr1::shared_ptr<KSSMaster::IKSSMaster> g_testKssMaster(new DKKSSMaster::CKSSMaster()); 

namespace xiaomi
{

const char* FileOperationDelegate::EventFileOperationProgress = "EventFileOperationProgress";
const char* MiCloudService::EventFileCreated = "EventFileCreated";
const char* MiCloudService::EventQuotaRetrieved = "EventQuotaRetrieved";
const char* MiCloudService::EventChildrenRetrieved = "EventChildrenRetrieved";
const char* MiCloudService::EventDirectoryCreated = "EventDirectoryCreated";
const char* MiCloudService::EventRequestDownloadFinished = "EventRequestDownloadFinished";
const char* MiCloudService::EventFileDeleted = "EventFileDeleted";
const char* MiCloudService::EventDirectoryDeleted = "EventDirectoryDeleted";
const char* MiCloudService::EventInfoGot = "EventInfoGot";

string MiCloudService::DataForCreateFile(const string& filePath)
{
    vector<string> shaVecs, md5Vecs;
    vector<int> sizeVecs;

    bool getVecsSucceeded = EncodeUtil::CalcFileBlockInfos(filePath.c_str(), XiaoMiConstants::s_micloudBlockSize, &shaVecs, &md5Vecs, &sizeVecs);
    if (!getVecsSucceeded 
            || shaVecs.size() != md5Vecs.size()
            || shaVecs.size() != sizeVecs.size()
            || md5Vecs.size() != sizeVecs.size())
    {
        return "";
    }

    int arrayLen =  shaVecs.size();
    JsonObjectSPtr dataJsonObj = JsonObject::CreateJsonObject();
    JsonObjectSPtr kssJsonObj = JsonObject::CreateJsonObject();
    JsonObjectSPtr blockInfosObj = JsonObject::CreateJsonArrayObject();
    for (int i = 0; i < arrayLen; ++i)
    {
        JsonObjectSPtr blockInfoObj = JsonObject::CreateJsonObject();
        blockInfoObj->AddStringValue("sha1", shaVecs[i]);
        blockInfoObj->AddStringValue("md5", md5Vecs[i]);
        blockInfoObj->AddIntValue("size", sizeVecs[i]);

        blockInfosObj->AddJsonObjectArray(blockInfoObj);
    }

    kssJsonObj->AddJsonObject("block_infos", blockInfosObj);
    dataJsonObj->AddJsonObject("kss", kssJsonObj);

    return dataJsonObj->GetJsonString();
}

bool MiCloudService::CreateFile(const string& dirId, const string& filePath)
{
    string name = PathManager::GetFileName(filePath.c_str());
    string sha1 = EncodeUtil::SHA_160(filePath.c_str());
    string retry = "1";
    string data = DataForCreateFile(filePath);

    DownloadTaskSPtr task = MiCloudDownloadTaskBuilder::BuildCreateFileTask(dirId, name, sha1, retry, data);
    if (!task)
    {
        return false;
    }
    CONNECT(*task.get(), DownloadTask::EventDownloadTaskFinished, this, MiCloudService::OnCreateFileFinished)
    MiCloudManager::GetInstance()->InsertTask(task.get(), filePath, MiCloudManager::TT_CreateFile);
    DownloadManager::GetInstance()->AddTask(task);
    return true;
}

bool MiCloudService::OnCreateFileFinished(const EventArgs& args)
{
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
    DownloadTask* task = downloadTaskFinishedArgs.downloadTask;
    string filePath = MiCloudManager::GetInstance()->GetFileInfoForTask(task, MiCloudManager::TT_CreateFile);
    if (filePath.empty())
    {
        DebugPrintf(DLC_DIAGNOSTIC, "no file found for task: %x", task);
    }
    MiCloudManager::GetInstance()->EraseTask(task, MiCloudManager::TT_CreateFile);
    DebugPrintf(DLC_DIAGNOSTIC, "FILEPATH: %s", filePath.c_str());

    FileCreatedArgs file_create_args;
    file_create_args.local_file_path = filePath;
    if (downloadTaskFinishedArgs.succeeded)
    {
        std::string result  = SecureRequest::ParseStringResponse(task->GetString(), m_security);
        DebugPrintf(DLC_DIAGNOSTIC, "%s", result.c_str());
        MiCloudServiceResultCreateFileSPtr  createFileResult(MiCloudServiceResultCreateFile::CreateResult(result.c_str()));
        if (createFileResult)
        {
            //upload successfully
            if (createFileResult->GetFileInfo())
            {
                file_create_args.succeeded = !createFileResult->IsError();
                file_create_args.result = createFileResult;
                // need upload fire to KSS. Fire event now
                FireEvent(EventFileCreated, file_create_args);
            }
            else if (createFileResult->GetKSSInfo())
            {
                //test for KSSMaster
                string kssData = createFileResult->GetKSSStrInfo();
                if (!kssData.empty())
                {
                    IDownloadTask *pTask = DownloadTaskFactory::CreateMiCloudFileUploadTask(
                        GetRequestUploadUrl(),
                        filePath,
                        kssData,
                        createFileResult->GetKSSInfo()->uploadId,
                        0
                        );
                    IDownloader * downloadManager = DownloaderImpl::GetInstance();
                    if (downloadManager)
                    {
                        downloadManager->AddTask(pTask);
                        file_create_args.succeeded = true;
                    }
                }
            }
        }
    }
    else
    {
        CheckIfHttpErrorExists(task);
        // need upload fire to KSS. Fire event now
        FireEvent(EventFileCreated, file_create_args);
    }
    return true;
}

bool MiCloudService::CreateDirectory(const string& parentDirId, const string& dirName)
{
    DownloadTaskSPtr task = MiCloudDownloadTaskBuilder::BuildCreateDirectoryTask(parentDirId, dirName);
    if (!task)
    {
        return false;
    }
    CONNECT(*task.get(), DownloadTask::EventDownloadTaskFinished, this, MiCloudService::OnCreateDirectoryFinished)
    DownloadManager::GetInstance()->AddTask(task);
    return true;
}

bool MiCloudService::CreateDirectory(const string& filePath)
{
    DebugPrintf(DLC_DIAGNOSTIC, "CreateDirectory: %s, %d", filePath.c_str(), StringUtil::CountsOfChar(filePath, string(1, '/')));
    DownloadTaskSPtr task = MiCloudDownloadTaskBuilder::BuildCreateDirectoryTask(filePath);
    if (!task)
    {
        return false;
    }
    CONNECT(*task.get(), DownloadTask::EventDownloadTaskFinished, this, MiCloudService::OnCreateDirectoryFinished)
    MiCloudManager::GetInstance()->InsertTask(task.get(), filePath, MiCloudManager::TT_CreateDir);
    DownloadManager::GetInstance()->AddTask(task);
    return true;
}

bool MiCloudService::OnCreateDirectoryFinished(const EventArgs& args)
{
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
    DownloadTask* task = downloadTaskFinishedArgs.downloadTask;
    string filePath = MiCloudManager::GetInstance()->GetFileInfoForTask(task, MiCloudManager::TT_CreateDir);
    if (filePath.empty())
    {
        DebugPrintf(DLC_DIAGNOSTIC, "no file found for task: %x", task);
    }
    MiCloudManager::GetInstance()->EraseTask(task, MiCloudManager::TT_CreateDir);

    DirectoryCreatedArgs dir_create_args;
    dir_create_args.dir_path = filePath;
    if (downloadTaskFinishedArgs.succeeded)
    {
        std::string result  = SecureRequest::ParseStringResponse(task->GetString(), m_security);
        DebugPrintf(DLC_DIAGNOSTIC, "\n%s", result.c_str());
        MiCloudServiceResultCreateDirSPtr createDirResult(MiCloudServiceResultCreateDir::CreateResult(result.c_str()));
        if (createDirResult)
        {
            MiCloudFile* dirInfo = createDirResult->GetDirInfo().get();
            if (dirInfo)
            {
                DebugPrintf(DLC_DIAGNOSTIC, "id:%s, name: %s, parentId: %s, type: %d, sha1: %s",
                        dirInfo->id.c_str(),
                        dirInfo->name.c_str(),
                        dirInfo->parentId.c_str(),
                        dirInfo->type,
                        dirInfo->sha1.c_str());
            }
            dir_create_args.succeeded = !createDirResult->IsError();
            dir_create_args.result = createDirResult;
        }
    }
    else
    {
        CheckIfHttpErrorExists(task);
    }

    FireEvent(EventDirectoryCreated, dir_create_args);
    return true;
}

string MiCloudService::DataForCommitFile(const string& fileMeta, const vector<string>& commitMetas)
{
    JsonObjectSPtr dataObj = JsonObject::CreateJsonObject();
    JsonObjectSPtr kssObj = JsonObject::CreateJsonObject();
    JsonObjectSPtr commitMetasObj = JsonObject::CreateJsonArrayObject();
    if (kssObj && dataObj && commitMetasObj)
    {
        for (unsigned int i = 0; i < commitMetas.size(); ++i)
        {
            JsonObjectSPtr commitMetaObj = JsonObject::CreateJsonObject();
            commitMetaObj->AddStringValue("commit_meta", commitMetas[i]);

            commitMetasObj->AddJsonObjectArray(commitMetaObj);
        }

        kssObj->AddStringValue("file_meta", fileMeta);
        kssObj->AddJsonObject("commit_metas", commitMetasObj);
        dataObj->AddJsonObject("kss", kssObj);
        return dataObj->GetJsonString();
    }

    return "";
}

bool MiCloudService::CommitFile(const string& uploadPath, const string& upload, const string& fileMeta, const vector<string>& commitMetas)
{
    DownloadTaskSPtr task = MiCloudDownloadTaskBuilder::BuildCommitFileTask(upload, DataForCommitFile(fileMeta, commitMetas));
    if (!task)
    {
        return false;
    }
    CONNECT(*task.get(), DownloadTask::EventDownloadTaskFinished, this, MiCloudService::OnCommitFileFinished)
    MiCloudManager::GetInstance()->InsertTask(task.get(), uploadPath, MiCloudManager::TT_CommitUpload);
    DownloadManager::GetInstance()->AddTask(task);
    return true;
}

bool MiCloudService::OnCommitFileFinished(const EventArgs& args)
{
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
    DownloadTask* task = downloadTaskFinishedArgs.downloadTask;
    string filePath = MiCloudManager::GetInstance()->GetFileInfoForTask(task, MiCloudManager::TT_CommitUpload);
    if (filePath.empty())
    {
        DebugPrintf(DLC_DIAGNOSTIC, "commit file cann't find path.");
        return false;
    }

    FileCreatedArgs file_create_args;
    file_create_args.local_file_path = filePath;
    if (downloadTaskFinishedArgs.succeeded)
    {
        std::string result  = SecureRequest::ParseStringResponse(task->GetString(), m_security);
        DebugPrintf(DLC_DIAGNOSTIC, "\n%s", result.c_str());
        MiCloudServiceResultCreateFileSPtr  createFileResult(MiCloudServiceResultCreateFile::CreateResult(result.c_str()));
        if (createFileResult && createFileResult->GetFileInfo())
        {
            file_create_args.succeeded = !createFileResult->IsError();
            file_create_args.result = createFileResult;
            FireEvent(EventFileCreated, file_create_args);

            MiCloudFileSPtr fileInfo = createFileResult->GetFileInfo();
            if (fileInfo)
            {
                DebugPrintf(DLC_DIAGNOSTIC, "=====================================================================");
                DebugPrintf(DLC_DIAGNOSTIC, "%s %s %s %llu, %llu, %llu", fileInfo->id.c_str(), fileInfo->name.c_str(), fileInfo->parentId.c_str(), 
                        fileInfo->createTime, fileInfo->modifyTime, fileInfo->size);
                DebugPrintf(DLC_DIAGNOSTIC, "=====================================================================");
            }
        }
    }
    else
    {
        CheckIfHttpErrorExists(task);
    }

    FireEvent(EventFileCreated, file_create_args);
    return true;
}

bool MiCloudService::RequestDownload(const string& fileId, const string& fileName)
{
    DownloadTaskSPtr task = MiCloudDownloadTaskBuilder::BuildRequestDownloadTask(fileId);
    if (!task || fileId.empty() || fileName.empty())
    {
        return false;
    }
    CONNECT(*task.get(), DownloadTask::EventDownloadTaskFinished, this, MiCloudService::OnRequestDownloadFinished)
    //hack:use space to join the fileId and fileName
    //TODO:store the fileId and fileName in struct
    MiCloudManager::GetInstance()->InsertTask(task.get(), fileId + " " + fileName, MiCloudManager::TT_RequestDownload);
    DownloadManager::GetInstance()->AddTask(task);
    return true;
}

bool MiCloudService::OnRequestDownloadFinished(const EventArgs& args)
{
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
    DownloadTask* task = downloadTaskFinishedArgs.downloadTask;
    string fileIdAndName = MiCloudManager::GetInstance()->GetFileInfoForTask(task, MiCloudManager::TT_RequestDownload);
    size_t pos = fileIdAndName.find(' ');
    if (pos == string::npos)
    {
        DebugPrintf(DLC_DIAGNOSTIC, "no fileId And fileName for task: %x", task);
        return false;
    }
    string fileId = fileIdAndName.substr(0, pos);
    string fileName = fileIdAndName.substr(pos + 1);
    if (fileId.empty())
    {
        DebugPrintf(DLC_DIAGNOSTIC, "no file found for task: %x", task);
        return false;
    }
    if (fileName.empty())
    {
        fileName = fileId;
    }
    MiCloudManager::GetInstance()->EraseTask(task, MiCloudManager::TT_RequestDownload);
    RequestDownloadFinishedArgs request_download_args;
    request_download_args.file_id = fileId;
    if (downloadTaskFinishedArgs.succeeded)
    {
        std::string result  = SecureRequest::ParseStringResponse(task->GetString(), m_security);
        DebugPrintf(DLC_DIAGNOSTIC, "\n%s", result.c_str());
        MiCloudServiceResultRequestDownloadSPtr  downloadResult(MiCloudServiceResultRequestDownload::CreateResult(result.c_str()));
        if (downloadResult)
        {
            std::string kssInfo = downloadResult->GetKSSStrInfo();
            if (!kssInfo.empty())
            {
                IDownloadTask *pTask = DownloadTaskFactory::CreateMiCloudFileDownloadTask(
                        GetRequestDownloadUrl(fileId),
                        fileId,
                        //TODO: use filename instead
                        fileName,
                        kssInfo,
                        MiCloudManager::GetInstance()->GetLocalDownloadDir() + fileName,
                        //TODO: user real fileSize
                        0
                        );

                IDownloader * downloadManager = DownloaderImpl::GetInstance();
                if (downloadManager)
                {
                    downloadManager->AddTask(pTask);
                    request_download_args.succeeded = true;
                }
                //DKKSSManager::GetInstance()->Download(kssInfo, "/DuoKan/dota.txt", fastdelegate::MakeDelegate(&prog, &progress::logProgress));
            }
        }
    }
    else
    {
        CheckIfHttpErrorExists(task);
    }
    FireEvent(EventRequestDownloadFinished, request_download_args);
    return true;
}

bool MiCloudService::DeleteFile(const string& fileId)
{
    DownloadTaskSPtr task = MiCloudDownloadTaskBuilder::BuildDeleteFileTask(fileId);
    if (!task)
    {
        return false;
    }
    CONNECT(*task.get(), DownloadTask::EventDownloadTaskFinished, this, MiCloudService::OnDeleteFileFinished)
    MiCloudManager::GetInstance()->InsertTask(task.get(), fileId, MiCloudManager::TT_DeleteFile);
    DownloadManager::GetInstance()->AddTask(task);
    return true;
}

bool MiCloudService::OnDeleteFileFinished(const EventArgs& args)
{
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
    DownloadTask* task = downloadTaskFinishedArgs.downloadTask;
    string fileId = MiCloudManager::GetInstance()->GetFileInfoForTask(task, MiCloudManager::TT_DeleteFile);
    FileDeletedArgs file_delete_args;
    file_delete_args.file_id = fileId;
    if (fileId.empty())
    {
        DebugPrintf(DLC_DIAGNOSTIC, "no file found for task: %x", task);
        return false;
    }
    MiCloudManager::GetInstance()->EraseTask(task, MiCloudManager::TT_DeleteFile);
    if (downloadTaskFinishedArgs.succeeded)
    {
        std::string result  = SecureRequest::ParseStringResponse(task->GetString(), m_security);
        DebugPrintf(DLC_DIAGNOSTIC, "\n%s", result.c_str());
        MiCloudServiceResultDeleteFileSPtr deleteFileResult(MiCloudServiceResultDeleteFile::CreateResult(result.c_str()));
        if (deleteFileResult)
        {
            file_delete_args.succeeded = !deleteFileResult->IsError();
        }
    }
    else
    {
        CheckIfHttpErrorExists(task);
    }
    FireEvent(EventFileDeleted, file_delete_args);
    return true;
}

bool MiCloudService::DeleteDirectory(const string& dirId)
{
    DownloadTaskSPtr task = MiCloudDownloadTaskBuilder::BuildDeleteDirectoryTask(dirId);
    if (!task)
    {
        return false;
    }
    CONNECT(*task.get(), DownloadTask::EventDownloadTaskFinished, this, MiCloudService::OnDeleteDirectoryFinished)
    MiCloudManager::GetInstance()->InsertTask(task.get(), dirId, MiCloudManager::TT_DeleteDir);
    DownloadManager::GetInstance()->AddTask(task);
    return true;
}

bool MiCloudService::OnDeleteDirectoryFinished(const EventArgs& args)
{
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
    DownloadTask* task = downloadTaskFinishedArgs.downloadTask;
    string dirId = MiCloudManager::GetInstance()->GetFileInfoForTask(task, MiCloudManager::TT_DeleteDir);
    DirectoryDeletedArgs directoryDeletedArgs;
    directoryDeletedArgs.directory_id = dirId;
    if (dirId.empty())
    {
        DebugPrintf(DLC_DIAGNOSTIC, "no file found for task: %x", task);
        return false;
    }
    MiCloudManager::GetInstance()->EraseTask(task, MiCloudManager::TT_DeleteDir);

    if (downloadTaskFinishedArgs.succeeded)
    {
        std::string result  = SecureRequest::ParseStringResponse(task->GetString(), m_security);
        DebugPrintf(DLC_DIAGNOSTIC, "\n%s", result.c_str());
        MiCloudServiceResultDeleteDirSPtr deleteDirResult(MiCloudServiceResultDeleteDir::CreateResult(result.c_str()));
        if (deleteDirResult)
        {
            directoryDeletedArgs.succeeded = !deleteDirResult->IsError();
        }
    }
    else
    {
        CheckIfHttpErrorExists(task);
    }
    FireEvent(EventDirectoryDeleted, directoryDeletedArgs);

    return true;
}

bool MiCloudService::GetChildren(const string& dirId, const string& offset, const string& limit)
{
    DownloadTaskSPtr task = MiCloudDownloadTaskBuilder::BuildGetChildrenTask(dirId, offset, limit);
    if (!task)
    {
        return false;
    }

    CONNECT(*task.get(), DownloadTask::EventDownloadTaskFinished, this, MiCloudService::OnGetChildrenFinished)
    MiCloudManager::GetInstance()->InsertTask(task.get(), dirId, MiCloudManager::TT_GetChildren);
    DownloadManager::GetInstance()->AddTask(task);
    return true;
}

bool MiCloudService::OnGetChildrenFinished(const EventArgs& args)
{
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
    DownloadTask* task = downloadTaskFinishedArgs.downloadTask;
    string dirId = MiCloudManager::GetInstance()->GetFileInfoForTask(task, MiCloudManager::TT_GetChildren);
    if (dirId.empty())
    {
        DebugPrintf(DLC_DIAGNOSTIC, "no file found for task: %x", task);
    }
    MiCloudManager::GetInstance()->EraseTask(task, MiCloudManager::TT_GetChildren);

    ChildrenRetrievedArgs children_retrieve_args;
    children_retrieve_args.parent_dir_id = dirId;
    if (downloadTaskFinishedArgs.succeeded)
    {
        std::string result  = SecureRequest::ParseStringResponse(task->GetString(), m_security);
        DebugPrintf(DLC_DIAGNOSTIC, "\n%s", result.c_str());
        MiCloudServiceResultGetChildrenSPtr getChildResult(MiCloudServiceResultGetChildren::CreateResult(result.c_str()));
        if (getChildResult)
        {
            MiCloudFileSPtrList fileList = getChildResult->GetFileList();
            DebugPrintf(DLC_DIAGNOSTIC, "%d", fileList.size());
            for (unsigned int i = 0; i < fileList.size(); ++i)
            {
                MiCloudFileSPtr file = fileList[i];
                DebugPrintf(DLC_DIAGNOSTIC, "%s, %d", file->name.c_str(), file->type);
            }
        }
        children_retrieve_args.succeeded = !getChildResult->IsError();
        children_retrieve_args.result = getChildResult;
    }
    else
    {
        CheckIfHttpErrorExists(task);
    }

    FireEvent(EventChildrenRetrieved, children_retrieve_args);
    return true;
}

bool MiCloudService::GetInfo(const string& filePath)
{
    DownloadTaskSPtr task = MiCloudDownloadTaskBuilder::BuildGetInfoTask(filePath);
    if (!task)
    {
        return false;
    }
    CONNECT(*task.get(), DownloadTask::EventDownloadTaskFinished, this, MiCloudService::OnGetInfoFinished)
    MiCloudManager::GetInstance()->InsertTask(task.get(), filePath, MiCloudManager::TT_GetInfo);
    DownloadManager::GetInstance()->AddTask(task);
    return true;
}

bool MiCloudService::OnGetInfoFinished(const EventArgs& args)
{
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
    DownloadTask* task = downloadTaskFinishedArgs.downloadTask;
    string filePath = MiCloudManager::GetInstance()->GetFileInfoForTask(task, MiCloudManager::TT_GetInfo);
    if (filePath.empty())
    {
        DebugPrintf(DLC_DIAGNOSTIC, "No FIlePath found: %x", task);
    }
    DebugPrintf(DLC_DIAGNOSTIC, "GetInfo: %x, %s", task, filePath.c_str());
    MiCloudManager::GetInstance()->EraseTask(task, MiCloudManager::TT_GetInfo);

    GetInfoArgs get_info_args;
    if (downloadTaskFinishedArgs.succeeded)
    {
        std::string result  = SecureRequest::ParseStringResponse(task->GetString(), m_security);
        DebugPrintf(DLC_DIAGNOSTIC, "\n%s", result.c_str());
        MiCloudServiceResultGetInfoSPtr getInfoResult(MiCloudServiceResultGetInfo::CreateResult(result.c_str()));
        if (getInfoResult)
        {
            MiCloudFileSPtr fileInfo = getInfoResult->GetFileInfo();
            if (fileInfo)
            {
                DebugPrintf(DLC_DIAGNOSTIC, "=====================================================================");
                DebugPrintf(DLC_DIAGNOSTIC, "%s %s %s %llu, %llu, %llu", fileInfo->id.c_str(), fileInfo->name.c_str(), fileInfo->parentId.c_str(), 
                        fileInfo->createTime, fileInfo->modifyTime, fileInfo->size);
                DebugPrintf(DLC_DIAGNOSTIC, "=====================================================================");
            }
        }
        get_info_args.succeeded = !getInfoResult->IsError();
        get_info_args.result = getInfoResult;
    }
    else
    {
        CheckIfHttpErrorExists(task);
    }
    FireEvent(EventInfoGot, get_info_args);
    return true;
}

bool MiCloudService::GetQuota()
{
    DownloadTaskSPtr task = MiCloudDownloadTaskBuilder::BuildGetQuotaTask();
    if (!task)
    {
        return false;
    }
    CONNECT(*task.get(), DownloadTask::EventDownloadTaskFinished, this, MiCloudService::OnGetQuotaFinished)
    DownloadManager::GetInstance()->AddTask(task);
    return true;
}

bool MiCloudService::OnGetQuotaFinished(const EventArgs& args)
{
    const DownloadTaskFinishArgs& downloadTaskFinishedArgs = (const DownloadTaskFinishArgs&)args;
    DownloadTask* task = downloadTaskFinishedArgs.downloadTask;

    QuotaRetrievedArgs quota_retrieved_args;
    if (downloadTaskFinishedArgs.succeeded)
    {
        std::string result  = SecureRequest::ParseStringResponse(task->GetString(), m_security);
        DebugPrintf(DLC_DIAGNOSTIC, "\n%s", result.c_str());
        MiCloudServiceResultGetQuotaSPtr quotaResult(MiCloudServiceResultGetQuota::CreateResult(result.c_str()));
        if (quotaResult)
        {
            DebugPrintf(DLC_DIAGNOSTIC, "%d, %d, %d",
                    quotaResult->GetTotal(),
                    quotaResult->GetAvailable(),
                    quotaResult->GetNsUsed());
        }
        quota_retrieved_args.succeeded = !quotaResult->IsError();
        quota_retrieved_args.result = quotaResult;
    }
    else
    {
        CheckIfHttpErrorExists(task);
    }

    FireEvent(EventQuotaRetrieved, quota_retrieved_args);
    return true;
}

bool MiCloudService::Init()
{
    XMLDomDocument* pDocument = XMLDomDocument::CreateDocument();
    if (!pDocument)
    {
        return false;
    }

    if (!pDocument->LoadXmlFromFile(PathManager::GetMiCloudServiceTokenPath().c_str(), false))
    {
        XMLDomDocument::ReleaseDocument(pDocument);
        return false;
    }
    
    XMLDomNode* pRootNode = pDocument->RootElement();
    if (!pRootNode)
    {
        XMLDomDocument::ReleaseDocument(pDocument);
        return false;
    }

    DK_INT count = 0;
    pRootNode->GetChildrenCount(&count);
    string userId,serviceToken;
    for (DK_INT i = 0; i < count; ++i)
    {
        XMLDomNode TmpNode;
        pRootNode->GetChildByIndex(i, &TmpNode);
        if (strcmp(TmpNode.GetNodeValue(), "userid") == 0)
        {
            XmlUtil::ReadAtomNodeValue(TmpNode, userId);
            m_cookies["userId"] = userId;
        }
        else if (strcmp(TmpNode.GetNodeValue(), "security") == 0)
        {
            XmlUtil::ReadAtomNodeValue(TmpNode, m_security);
        }
        else if (strcmp(TmpNode.GetNodeValue(), "servicetoken") == 0)
        {
            XmlUtil::ReadAtomNodeValue(TmpNode, serviceToken);
            m_cookies["serviceToken"] = serviceToken;
        }
    }

    return !userId.empty() && !m_security.empty() && !serviceToken.empty();
}

MiCloudService::MiCloudService()
{
    Init();
}

string MiCloudService::GetRequestDownloadUrl(const string& fileId) const
{
    return MiCloudDownloadTaskBuilder::BuildRequestDownloadUrl(fileId);
}

string MiCloudService::GetRequestUploadUrl() const
{
    return MiCloudDownloadTaskBuilder::BuildCreateFileUrl();
}
}///xiaomi
