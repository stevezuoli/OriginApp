#include "XiaoMi/MiCloudManager.h"
#include "XiaoMi/MiCloudService.h"
#include "XiaoMi/XiaoMiServiceFactory.h"
#include "interface.h"
#include <tr1/functional>

namespace xiaomi
{
const char* MiCloudManager::BookRootDirCreated = "BookRootDirCreated";
const char* MiCloudManager::BookRootDirFilesScanFinised = "BookRootDirFilesScanFinised";
SINGLETON_CPP(MiCloudManager)
MiCloudManager::MiCloudManager()
{
}

bool MiCloudManager::InsertTask(DownloadTask* task, const string& filePath, TaskType type)
{
    if (IsTypeValid(type) && m_fileTaskMaps[type].find(task) != m_fileTaskMaps[type].end())
    {
        DebugPrintf(DLC_DIAGNOSTIC, "AddFileTask %x, %s already exists", task, filePath.c_str());
        return false;
    }

    m_fileTaskMaps[type][task] = filePath;
    return true;
}

bool MiCloudManager::EraseTask(DownloadTask* task, TaskType type)
{
    return m_fileTaskMaps[type].erase(task);
}

string MiCloudManager::GetFileInfoForTask(DownloadTask* task, TaskType type) const
{
    map<DownloadTask*, string>::const_iterator iter = m_fileTaskMaps[type].find(task);
    if (iter != m_fileTaskMaps[type].end())
    {
        return iter->second;
    }
    return "";
}

FetchDataResult MiCloudManager::FetchBookRootDirID()
{
    if (m_bookRootDirID.empty())
    {
        XiaoMiServiceFactory::GetMiCloudService()->CreateDirectory(GetBookRootPath());
        CONNECT(*XiaoMiServiceFactory::GetMiCloudService(), MiCloudService::EventDirectoryCreated, this, MiCloudManager::OnBookRootDirCreatedFinished)
    }
    else
    {
        BookRootDirCreatedArgs bookRootDirCreatedArgs;
        bookRootDirCreatedArgs.succeeded = true;
        bookRootDirCreatedArgs.id = m_bookRootDirID;
        FireEvent(BookRootDirCreated, bookRootDirCreatedArgs);
        return FDR_SUCCESSED;
    }

    return FDR_PENDING; 
}

bool MiCloudManager::OnBookRootDirCreatedFinished(const EventArgs& args)
{
    BookRootDirCreatedArgs bookRootDirCreatedArgs;
	const DirectoryCreatedArgs& dirCreatedArgs = (const DirectoryCreatedArgs&)args;
    if (dirCreatedArgs.succeeded)
    {
        MiCloudServiceResultCreateDirSPtr createResult = dirCreatedArgs.result;
        MiCloudFileSPtr dirInfo = createResult->GetDirInfo();
        if (dirInfo && dirInfo->parentId.compare("0") == 0)
                //&& dirInfo->name.compare(GetBookRootPath()) == 0)
        {
            bookRootDirCreatedArgs.succeeded = true;
            bookRootDirCreatedArgs.id = dirInfo->id;
            m_bookRootDirID = dirInfo->id;
        }
        else
        {
            bookRootDirCreatedArgs.succeeded = false;
            bookRootDirCreatedArgs.errorMsg = createResult->GetDescription();
        }
    }

    FireEvent(BookRootDirCreated, bookRootDirCreatedArgs);
    return true;
}

FetchDataResult MiCloudManager::FetchFileListsInBookRootDir()
{
    //TODO
    //FireEvent if scanning finished already
    //return FDR_SUCCESSED;
    if (!m_bookRootDirID.empty())
    {
        XiaoMiServiceFactory::GetMiCloudService()->GetChildren(m_bookRootDirID);
        CONNECT(*XiaoMiServiceFactory::GetMiCloudService(), MiCloudService::EventChildrenRetrieved, this, MiCloudManager::OnCloudFileScanFinishedArgs)
    }

    return FDR_FAILED;
}

bool MiCloudManager::OnCloudFileScanFinishedArgs(const EventArgs& args)
{
    CloudFileScanFinishedArgs cloudFileScanFinishedArgs;
    const ChildrenRetrievedArgs& childrenRetrievedArgs = (const ChildrenRetrievedArgs&)args;
    cloudFileScanFinishedArgs.succeeded = childrenRetrievedArgs.succeeded;

    FireEvent(BookRootDirFilesScanFinised, cloudFileScanFinishedArgs);

    return true;
}

bool MiCloudManager::MakeSureBookDirExist() const
{
    return true;
}

bool MiCloudManager::IsFileExistsInCloud(const string& fileName, const int64_t fileSize, bool recursive) const
{
    return false;
}
}

