#ifndef __IDOWNLOADSERIALIZEDBOOKTASK_H_
#define __IDOWNLOADSERIALIZEDBOOKTASK_H_
#include "IDownloadTask.h"

class IDownloadSerializedBookTask :public IDownloadTask{
public:
    virtual void SetDescription(std::string _des) = 0;
    virtual std::string GetDescription() = 0;
};
#endif//__IDOWNLOADSERIALIZEDBOOKTASK_H_

