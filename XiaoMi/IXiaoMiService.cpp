#include "XiaoMi/IXiaoMiService.h"
#include "interface.h"

namespace xiaomi
{
bool IXiaoMiService::CheckIfHttpErrorExists(dk::DownloadTask* task)
{
    if (task)
    {
        if (task->GetRetCode() == 401)
        {
            DebugPrintf(DLC_DIAGNOSTIC, "HTTP UnAuthorized!");
            return true;
        }
    }

    return false;
}

}
