#ifndef __IDOWNLOADENGINE_H_
#define __IDOWNLOADENGINE_H_

#include <string>
class IDownloadEngine
{
public:
    IDownloadEngine(){}
    virtual ~IDownloadEngine(){};
    virtual void Start()=0;
    virtual void Stop()=0;
    virtual int GetFileInfoFromServer(std::string _url,void* _pinfo) = 0;
};

#endif//__IDOWNLOADENGINE_H_
