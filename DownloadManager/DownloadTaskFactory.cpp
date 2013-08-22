#include "DownloadManager/DownloadTaskFactory.h"
#include "DownloadManager/DownloadBookTaskImpl.h"
#include "DownloadManager/DownloadReleaseTaskImpl.h"
#include "DownloadManager/DownloadScreenSaverTaskImpl.h"

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
