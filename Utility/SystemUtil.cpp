#include "Utility/SystemUtil.h"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <sstream>

#include <sys/sysinfo.h>
#include <sys/vfs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/apm_bios.h>
#include <sys/ioctl.h>
#include <assert.h>

#include "KernelDef.h"
#include "interface.h"
#include <malloc.h>

namespace dk
{
namespace utility
{

std::string SystemUtil::ReadStringFromFile(const char* file, bool log)
{
    std::string result;
    if (NULL == file)
    {
        return result;
    }

	//if (log)
	//{
	//	DebugPrintf(DLC_DIAGNOSTIC, "Read string from file begin: %s", file);
	//}

    FILE* f = fopen(file, "r");
    size_t readBytes = 0;
    if (NULL != f)
    {
        char buf[1024 + 1];
        do
        {
            memset(buf, 0, sizeof(buf));
            readBytes = fread(buf, 1, DK_DIM(buf) - 1, f);
            result += buf;
        } while (readBytes == DK_DIM(buf) - 1);
        fclose(f);
    }
		
	
	//if (log)
	//{
	//	DebugPrintf(DLC_DIAGNOSTIC, "Read string from file end: %s", file);
	//}

    
    return result;
}

bool SystemUtil::ReadIntFromFile(const char* file, int* value, bool log)
{
    if (NULL == file || NULL == value)
    {
        return false;
    }
    std::string s = ReadStringFromFile(file, log);
    if (s.empty())
    {
        return false;
    }
    *value = atoi(s.c_str());
    return true;
}

std::string SystemUtil::ReadStringFromShellOutput(const char* command, bool log)
{
    std::string result;
    if (NULL == command)
    {
        return result;
    }
	//if (log)
	//{
	//	 DebugPrintf(DLC_DIAGNOSTIC, "Read string from shell begin: %s", command);
	//}
   
    FILE* f = popen(command, "r");
    size_t readBytes = 0;
    if (NULL != f)
    {
        char buf[1024 + 1];
        do
        {
            memset(buf, 0, sizeof(buf));
            readBytes = fread(buf, 1, DK_DIM(buf) - 1, f);
            result += buf;
        } while (readBytes == DK_DIM(buf) - 1);
        pclose(f);
    }
	//if (log)
	//{
	//	DebugPrintf(DLC_DIAGNOSTIC, "Read string from shell end: %s", command);
	//}
   
    return result;
}

bool SystemUtil::ReadIntFromShellOutput(const char* command, int* value, bool log)
{
    if (NULL == command || NULL == value)
    {
        return false;
    }
    std::string s = ReadStringFromShellOutput(command, log);
    if (s.empty())
    {
        return false;
    }
    *value = atoi(s.c_str());
    return true;
}

int SystemUtil::ExecuteCommand(const char* command, bool log)
{
    if (NULL == command)
    {
        return -1;
    }
	//if (log)
	//{
	//	DebugPrintf(DLC_DIAGNOSTIC, "Begin execute command: %s", command);
	//}

    int result = system(command);

	//if (log)
	//{
	//	DebugPrintf(DLC_DIAGNOSTIC, "End execute command: %s", command);
	//}
    return result;
}

int64_t SystemUtil::GetUpdateTimeInMs()
{
    std::string s = ReadStringFromFile("/proc/uptime", false);
    if (s.empty())
    {
        return -1;
    }
    return (int64_t)(atof(s.c_str()) * 1000);
}

void SystemUtil::DumpMallocInfo()
{
    struct mallinfo info = mallinfo();
    printf("\n===== Malloc Info begin=====\n"
            "arena: %d bytes\n"
            "ordblks: %d chunks\n"
            "smblks: %d\n"
            "hblks: %d chunks\n"
            "hblkhd: %d bytes\n"
            "usmblks: %d\n"
            "fsmblks: %d\n"
            "uordblks: %d chunks\n"
            "fordblks: %d chunks\n"
            "keepcost: %d\n"
            "===== Malloc Info End =====\n\n",
            info.arena,
            info.ordblks,
            info.smblks,
            info.hblks,
            info.hblkhd,
            info.usmblks,
            info.fsmblks,
            info.uordblks,
            info.ordblks,
            info.keepcost);
}

unsigned long long SystemUtil::DiskSpace(const char* mount_ponit)
{
    struct statfs stats;
    statfs (mount_ponit, &stats);
    return (static_cast<unsigned long long>(stats.f_blocks) *
            static_cast<unsigned long long>(stats.f_bsize));
}

unsigned long long SystemUtil::FreeSpace(const char* mount_ponit)
{
    struct statfs stats;
    statfs (mount_ponit, &stats);
    return (static_cast<unsigned long long>(stats.f_bavail) *
            static_cast<unsigned long long>(stats.f_bsize));
}

unsigned long SystemUtil::SystemTotalMemory()
{
    struct sysinfo info;
    sysinfo(&info);
    return info.totalram * info.mem_unit;
}

// On Linux, we can also use  cat /proc/<pid>/status to get process memory
// usage.
unsigned long SystemUtil::SystemFreeMemory()
{
    struct sysinfo info;
    sysinfo(&info);
    return info.freeram * info.mem_unit;
}

unsigned long SystemUtil::SafeMemoryLimit()
{
    return 15 * 1024 * 1024;
}

bool SystemUtil::NeedReleaseMemory()
{
    return SystemFreeMemory() <= SafeMemoryLimit();
}

} // namespace utiltiy
} // namespace dk
