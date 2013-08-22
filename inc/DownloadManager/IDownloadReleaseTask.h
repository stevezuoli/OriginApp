#ifndef __IDOWNLOADRELEASETASK_H_
#define __IDOWNLOADRELEASETASK_H_
#include "IDownloadTask.h"

class IDownloadReleaseTask :public IDownloadTask{
public:
    virtual void SetDescription(std::string _des) = 0;
    virtual std::string GetDescription() const = 0;

};
#endif//__IDOWNLOADRELEASETASK_H_
