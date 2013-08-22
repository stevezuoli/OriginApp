#ifndef __UTILITY_SYSTEMUTIL_H__
#define __UTILITY_SYSTEMUTIL_H__

#include <string>
#include <stdint.h>
#include "Common/Defs.h"

namespace dk
{
namespace utility
{

class SystemUtil
{
public:
    static std::string ReadStringFromFile(const char* file, bool log = true);
    static bool ReadIntFromFile(const char* file, int* value, bool log = true);
    static std::string ReadStringFromShellOutput(const char* command, bool log = true);
    static bool ReadIntFromShellOutput(const char* command, int* value, bool log = true);
    static int ExecuteCommand(const char* command, bool log = true);
    static dkUpTime GetUpdateTimeInMs();
    static void DumpMallocInfo();

    static unsigned long long DiskSpace(const char* mount_ponit);
    static unsigned long long FreeSpace(const char* mount_ponit);
    static unsigned long SystemTotalMemory();
    static unsigned long SystemFreeMemory();
    static unsigned long SafeMemoryLimit();
    static bool NeedReleaseMemory();

private:
    SystemUtil();
};

} // namespace utiltiy
} // namespace dk
#endif
