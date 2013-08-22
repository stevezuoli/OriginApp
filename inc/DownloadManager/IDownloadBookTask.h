#ifndef __BOOKDOWNLOADFILE_H_
#define __BOOKDOWNLOADFILE_H_
#include "IDownloadTask.h"

class IDownloadBookTask :public IDownloadTask{
public:
    IDownloadBookTask(){}
    virtual ~IDownloadBookTask(){}
    virtual void SetDescription(std::string _des) = 0;
    virtual std::string GetDescription() = 0;

    virtual void SetSource(std::string _source) = 0;
    virtual std::string GetSource() = 0;

    virtual void SetArtist(std::string _artist) = 0;
    virtual std::string GetArtist() = 0;

	virtual void SetMoveTo(const std::string& moveTo) = 0;
	virtual std::string GetMoveTo() = 0;

};
#endif//__BOOKDOWNLOADFILE_H_
