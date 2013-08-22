#ifndef __SQMUPLOAD_H_
#define __SQMUPLOAD_H_

#include <pthread.h>

namespace SQMUploader
{
    void    StartUploadSQMDataThread();
    void*   SQMUploadThread(void* puseless);
    void    PostDataToServer();
    bool    CheckTimeShift();
	string GetDataNeedToPost();
};
#endif   //__SQMUPLOAD_H_

