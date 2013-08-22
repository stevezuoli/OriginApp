#ifndef __DOWNLOADFRUITGARDENER_H_
#define __DOWNLOADFRUITGARDENER_H_

#include "DownloadManager/DownloadBookTaskImpl.h"
#include "DownloadManager/IDownloadReleaseTask.h"
#include "DownloadManager/IDownloadSerializedBookTask.h"
#include "DownloadManager/IDownloadTask.h"

class DownloadTaskFactory
{
public:
    static IDownloadTask* CreateDownloadTask(
            IDownloadTask::DLType _type);
    static IDownloadBookTask* CreateBookDownloadTask(
            std::string _url,
            std::string _urlID,
            std::string _author,
            std::string _description,
            std::string _source,
            std::string _filename,
            unsigned int _filesize,
            std::string _pwd,
            unsigned int  _priority = 3,
            std::string _useragent = "",
            const std::string& moveTo = "");
    static IDownloadReleaseTask* CreateDownloadReleaseTask(
            std::string _url,
            std::string _urlID,
            std::string _description,
            std::string _filename,
            unsigned int _filesize,
            unsigned int _priority = 5,
            std::string _useragent = "");
    static IDownloadSerializedBookTask* CreateDownloadSerializedBookTask(
            std::string _url,
            std::string _urlID,
            std::string _description,
            std::string _filename,
            unsigned int _filesize,
            unsigned int _priority = 3,
            std::string _useragent = "");
	static IDownloadTask* CreateScreenSaverDownloadTask(
            std::string _url,
            std::string _urlID,
            std::string _filename,
            unsigned int _filesize,
            unsigned int  _priority = 3,
            std::string _useragent = "");
private:
    DownloadTaskFactory();
};

#endif// __DOWNLOADFRUITGARDENER_H_

