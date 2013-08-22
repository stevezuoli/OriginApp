#ifndef __FTPSERVICE_H__
#define __FTPSERVICE_H__

//#include "singleton.h"

enum FTP_Service_Status{
    FTP_STATUS_STOP,
    FTP_STATUS_PLAY,
};

class FtpService
{
    static void Start();
    static void Stop();
public:
    static void EnableWifiTransfer();
};

#endif //__FTPSERVICE_H_
