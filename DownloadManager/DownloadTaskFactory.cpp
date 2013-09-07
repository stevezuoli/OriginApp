#include "DownloadManager/DownloadTaskFactory.h"
#include "DownloadManager/DownloadBookTaskImpl.h"
#include "DownloadManager/DownloadReleaseTaskImpl.h"
#include "DownloadManager/DownloadScreenSaverTaskImpl.h"
#include "DownloadManager/DownloadMiCloudFileTask.h"
#include "Utility/PathManager.h"

IDownloadTask *DownloadTaskFactory::CreateDownloadTask(IDownloadTask::DLType _type)
{
    switch(_type)
    {
    case IDownloadTask::BOOK:
        return new DownloadBookTaskImpl();
    case IDownloadTask::UPGRADEPACKAGE:
        return new DownloadReleaseTaskImpl();
        break;
	case IDownloadTask::SCREENSAVER:
		return new DownloadScreenSaverTaskImpl();
		break;
	case IDownloadTask::MICLOUDFILE:
		return new DownloadMiCloudFileTask();
		break;
    default:
        break;
    }
    return NULL;
}

IDownloadBookTask *DownloadTaskFactory::CreateBookDownloadTask(std::string _url, std::string _urlID, std::string _author, std::string _description, std::string _source, std::string _filename, unsigned int _filesize, std::string _pwd, unsigned int  _priority,std::string _useragent,const std::string& moveTo)
{
    IDownloadBookTask *pDBTask = new DownloadBookTaskImpl();
    
    if (pDBTask)
    {
        pDBTask->SetFileName(_filename);
        pDBTask->SetUrl("");
        pDBTask->SetOrigUrl(_url);
        pDBTask->SetOrigUrlID(_urlID);
        pDBTask->SetState(IDownloadTask::WAITING);
        pDBTask->SetFileSize(_filesize);
        pDBTask->SetCanResume(false);
        pDBTask->SetLastOffSet(0);
        pDBTask->SetType(IDownloadTask::BOOK);
        pDBTask->SetDescription(_description);
        pDBTask->SetSource(_source);
        pDBTask->SetArtist(_author);
        pDBTask->SetFilePassword(_pwd);
        pDBTask->SetPriority(_priority);
        pDBTask->SetUserAgent(_useragent);
		pDBTask->SetMoveTo(moveTo);
    }
    return pDBTask;
}

IDownloadReleaseTask* DownloadTaskFactory::CreateDownloadReleaseTask(std::string _url,std::string _urlID,std::string _description,std::string _filename,unsigned int _filesize,unsigned int _priority,std::string _useragent)
{
    IDownloadReleaseTask *pDUTask = new DownloadReleaseTaskImpl();
    if(pDUTask)
    {
        pDUTask->SetFileName(_filename);
        pDUTask->SetUrl("");
        pDUTask->SetOrigUrl(_url);
        pDUTask->SetOrigUrlID(_urlID);
        pDUTask->SetState(IDownloadTask::WAITING);
        pDUTask->SetFileSize(_filesize);
        pDUTask->SetCanResume(false);
        pDUTask->SetLastOffSet(0);
        pDUTask->SetType(IDownloadTask::UPGRADEPACKAGE);
        pDUTask->SetDescription(_description);
        pDUTask->SetPriority(_priority);
        pDUTask->SetUserAgent(_useragent);
    }
    return pDUTask;
}


IDownloadTask* DownloadTaskFactory::CreateScreenSaverDownloadTask(std::string _url, std::string _urlID,std::string _filename, unsigned int _filesize,unsigned int  _priority,std::string _useragent)
{
	IDownloadTask *pScreenTask = new DownloadScreenSaverTaskImpl();
	if (pScreenTask)
	{
		pScreenTask->SetFileName(_filename);
		pScreenTask->SetUrl("");
		pScreenTask->SetOrigUrl(_url);
		pScreenTask->SetOrigUrlID(_urlID);
		pScreenTask->SetState(IDownloadTask::WAITING);
		pScreenTask->SetFileSize(_filesize);
		pScreenTask->SetCanResume(false);
		pScreenTask->SetLastOffSet(0);
		pScreenTask->SetType(IDownloadTask::SCREENSAVER);
		pScreenTask->SetFilePassword("");
		pScreenTask->SetPriority(_priority);
		pScreenTask->SetUserAgent(_useragent);
	}
	return pScreenTask;
}

IDownloadTask* DownloadTaskFactory::CreateMiCloudFileDownloadTask(
            std::string _url,
            std::string _urlID,
            std::string _filename,
            std::string _kssInfo,
            std::string _filePath,
            unsigned int _filesize,
            unsigned int  _priority,
            std::string _useragent)
{
    DownloadMiCloudFileTask *pMiCloudFileTask = new DownloadMiCloudFileTask();
    if (pMiCloudFileTask)
    {
		pMiCloudFileTask->SetFileName(_filename);
		pMiCloudFileTask->SetUrl("");
		pMiCloudFileTask->SetOrigUrl(_url);
		pMiCloudFileTask->SetOrigUrlID(_urlID);
		pMiCloudFileTask->SetState(IDownloadTask::WAITING);
		pMiCloudFileTask->SetFileSize(_filesize);
		pMiCloudFileTask->SetCanResume(false);
		pMiCloudFileTask->SetLastOffSet(0);
		pMiCloudFileTask->SetType(IDownloadTask::MICLOUDFILE);
		pMiCloudFileTask->SetFilePassword("");
		pMiCloudFileTask->SetPriority(_priority);
		pMiCloudFileTask->SetUserAgent(_useragent);
        pMiCloudFileTask->SetKSSInfo(_kssInfo);
        pMiCloudFileTask->SetMoveTo(_filePath);
    }

    return pMiCloudFileTask;
}

IDownloadTask* DownloadTaskFactory::CreateMiCloudFileUploadTask(
            std::string _url,
            std::string _urlID,
            std::string _kssInfo,
            std::string _uploadId,
            unsigned int _filesize,
            unsigned int  _priority,
            std::string _useragent)
{
    DownloadMiCloudFileTask *pMiCloudFileTask = new DownloadMiCloudFileTask();
    if (pMiCloudFileTask)
    {
        std::string fileName = dk::utility::PathManager::GetFileName(_urlID);
		pMiCloudFileTask->SetFileName(dk::utility::PathManager::GetFileNameWithoutExt(fileName.c_str()));
		pMiCloudFileTask->SetUrl("");
		pMiCloudFileTask->SetOrigUrl(_url);
		pMiCloudFileTask->SetOrigUrlID(_urlID);
		pMiCloudFileTask->SetState(IDownloadTask::WAITING);
		pMiCloudFileTask->SetFileSize(_filesize);
		pMiCloudFileTask->SetCanResume(false);
		pMiCloudFileTask->SetLastOffSet(0);
		pMiCloudFileTask->SetType(IDownloadTask::MICLOUDFILE);
		pMiCloudFileTask->SetFilePassword("");
		pMiCloudFileTask->SetPriority(_priority);
		pMiCloudFileTask->SetUserAgent(_useragent);
        pMiCloudFileTask->SetKSSInfo(_kssInfo);
        pMiCloudFileTask->SetMoveTo(_urlID);
        pMiCloudFileTask->SetUploadId(_uploadId);
    }

    return pMiCloudFileTask;
}

