//===========================================================================
// Summary:
//		Null
// Usage:
//		Null
// Remarks:
//		Null
// Date:
//		2011-10-13
// Author:
//		Zhang JingDan (zhangjingdan@duokan.com)
//===========================================================================

#ifndef	__KERNEL_COMMONLIBS_KERNELBASE_KERNELITIME_H__
#define	__KERNEL_COMMONLIBS_KERNELBASE_KERNELITIME_H__

#include "KernelType.h"
#include "KernelRetCode.h"
#include <time.h>

//创建时开始计时,析构时停止计时.
class DKStopWatch
{
public:
    DKStopWatch();
    ~DKStopWatch();
private:
    clock_t m_start;
};

#define GET_RUN_TIME printf("File:%s, Line:%d, Function:%s TimeTest \n", __FILE__, __LINE__, __FUNCTION__ ); DKStopWatch simple_time_test

#endif
