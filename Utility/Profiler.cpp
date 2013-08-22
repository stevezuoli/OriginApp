#include "Utility/Profiler.h"
#include "interface.h"
#include <stdio.h>
#include <sys/time.h>

namespace dk
{
namespace utility
{

void Profiler::Dump()
{
    const Profiler* start = s_start;
#ifndef _RELEASE_VERSION
    DebugPrintf(DLC_AUTOTEST, "==========Profiler Dump Begin==========");
    DebugPrintf(DLC_DIAGNOSTIC, "==========Profiler Dump Begin==========");
#endif
    while (start != NULL)
    {
#ifndef _RELEASE_VERSION
        DebugPrintf(DLC_AUTOTEST, "%s: %u times, %f ms", start->GetName(), (uint32_t)start->GetCount(), start->GetTimeMs());
        DebugPrintf(DLC_DIAGNOSTIC, "%s: %u times, %f ms", start->GetName(), (uint32_t)start->GetCount(), start->GetTimeMs());
#endif
        start = start->GetNext();
    }
#ifndef _RELEASE_VERSION
    DebugPrintf(DLC_AUTOTEST, "==========Profiler Dump End==========");
    DebugPrintf(DLC_DIAGNOSTIC, "==========Profiler Dump End==========");
#endif
}

void Profiler::Reset()
{
    Profiler* start = (Profiler*)s_start;
    while (start)
    {
        start->Clear();
        start = (Profiler*)start->GetNext();
    }
}

double Profiler::GetTimeMs() const
{

    return (double)m_time / 1000;
}

Profiler* Profiler::s_start = NULL;


ProfileTimer::ProfileTimer(Profiler* parent)
{
    m_parent = parent;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    m_begin =  tv.tv_sec * 1000000 + tv.tv_usec;
}

ProfileTimer::~ProfileTimer()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t end =  tv.tv_sec * 1000000 + tv.tv_usec;

    m_parent->AccumulateTime(end - m_begin);
    m_parent->IncCount();
};

}
}
