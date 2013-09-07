/*
 * =====================================================================================
 *       Filename:  MiCloudManager.h
 *    Description:  micloud files manager
 *
 *        Version:  1.0
 *        Created:  08/27/2013 10:24:33 AM
 * =====================================================================================
 */

#ifndef __MICLOUD_MANAGER_H__
#define __MICLOUD_MANAGER_H__

#include "singleton.h"
#include "KernelDef.h"
#include "XiaoMi/XiaoMiConstants.h"
#include "DownloadManager/DownloadTask.h"
#include "GUI/EventListener.h"
#include "GUI/EventSet.h"
#include "GUI/EventArgs.h"

#include "BookStore/BookStoreTypes.h"
#include <map>
#include <string>
using namespace std;
using dk::DownloadTask;
using namespace dk::bookstore;

namespace xiaomi
{
class BookRootDirCreatedArgs : public EventArgs
{
public:
    BookRootDirCreatedArgs()
        : succeeded(false)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new BookRootDirCreatedArgs(*this);
    }
    bool succeeded;
    string id;
    string errorMsg;
};//BookRootDirCreatedArgs

class CloudFileScanFinishedArgs : public EventArgs
{
public:
    CloudFileScanFinishedArgs()
        : succeeded(false)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new CloudFileScanFinishedArgs(*this);
    }
    bool succeeded;
};//CloudFileScanFinishedArgs

class MiCloudManager
    : public EventListener
    , public EventSet
{
    SINGLETON_H(MiCloudManager)
public:
    static const char* BookRootDirCreated;
    static const char* BookRootDirFilesScanFinised;
    enum TaskType
    {
        TT_CreateFile,
        TT_CreateDir,
        TT_DeleteFile,
        TT_DeleteDir,
        TT_GetChildren,
        TT_GetInfo,
        TT_RequestDownload,
        TT_CommitUpload,

        TT_Count
    };

    bool InsertTask(DownloadTask* task, const string& filePath, TaskType type);
    bool EraseTask(DownloadTask* task, TaskType type);
    string GetFileInfoForTask(DownloadTask* task, TaskType type) const;

    string GetLocalDownloadDir() const
    {
        return XiaoMiConstants::s_micloudFileLocalDir;
    }
    string GetBookRootPath() const
    {
        return XiaoMiConstants::s_micloudBookRootDir;
    }
    FetchDataResult FetchBookRootDirID();
    bool OnBookRootDirCreatedFinished(const EventArgs& args);

    FetchDataResult FetchFileListsInBookRootDir();
    bool OnCloudFileScanFinishedArgs(const EventArgs& args);

    bool IsFileExistsInCloud(const string& fileName, const int64_t fileSize, bool recursive = false) const;
private:
    bool IsTypeValid(TaskType type) const
    {
        return type >= 0 && type < TT_Count;
    }
    bool MakeSureBookDirExist() const;
private:
    MiCloudManager();
    DISALLOW_COPY_AND_ASSIGN(MiCloudManager);
    map<DownloadTask*, string> m_fileTaskMaps[TT_Count]; 
    string m_bookRootDirID;
};//MiCloudManager
}//xiaomi

#endif//__MICLOUD_MANAGER_H__
