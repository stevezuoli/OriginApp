#ifndef _DKREADER_COMMON_SYSTEMOPERATION_H_
#define _DKREADER_COMMON_SYSTEMOPERATION_H_


#include "dkBaseType.h"
#include <string>

#define SniffURL "http://opds.duokan.com/DuoKanServer/books/opds.xml"

class SystemOperation
{
public:
    static void  ExitApp(int nExitParam);
    static void  CleanupEnv();
    static void Reboot();
    static void Upgrade();
    static void SwitchToKindle();
    static void SwitchToDuoKan();
    static int CheckUpdate();
    static void Uninstall();
    static bool ViewRightInfo();
    static bool CheckEbook();
    static BOOL SniffNetwork(std::string url = std::string(SniffURL), UINT32 iTimeout = 5);
private:
    SystemOperation();
};
#endif
