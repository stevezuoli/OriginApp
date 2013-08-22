//===========================================================================
// Summary:
//		一个简单的性能剖析器。
// Usage:
//		将 DK_PROFILE("MyFunctionOrCodeBlockName") " 宏置于要剖析的函数或代码块的前部即可，结果将通过日志文件输出（LogPerformanceData.h/cpp）。
//		_PROFILE_ 基于C++ 对象超出作用域时的自动析构机制来计时，因此只能用于C++代码，并且一个作用域内只能有一个_PROFILE_宏。
//		需要剖析几个语句时，可以用{}包围：
//		{DK_PROFILE("CodeBlockName");
//			//codes to be profiled...
//		}
//
//		DK_PROFILE 定义了名为 "MyFunctionOrCodeBlockName" 的 Profiler 对象，它记录了该函数或代码块的执行次数 getCount()
//		和总执行时间 getTimeMillis()。
//		所有的 Profiler 对象自动连成链表，可通过 getList()/getNext()访问。不过被测代码一般无需访问Profiler的数据。
//      DK_PROFILE_FUNCTION直接以function名来定义profiler
//
// Remarks:
//      先定义ENABLE_PROFILE宏，再#incude "Profile.h"即进行profile
//		不定义ENABLE_PROFILE, DK_PROFILE 宏展开为空串，即关闭剖析。
// Date:
//		2011-11-17
// Author:
//		Peng Feng (pengf@duokan.com)
//===========================================================================

#ifndef	__UTILITY_PROFILER_H__
#define	__UTILITY_PROFILER_H__

#include <stdint.h>

namespace dk
{
namespace utility
{
//时间累加器。
class Profiler
{
public:
	Profiler(const char* name): m_name(name), m_count(0), m_time(0), m_next(0)
	{
		//register myself
		m_next = s_start;
		s_start = this;
	}

	const char* GetName() const
	{
		return m_name;
	}

	const Profiler* GetNext() const
	{
		return m_next;
	}

	int64_t GetCount() const
	{
		return m_count;
	}

	void Clear()
	{
		m_count = 0;
        m_time = 0;
	}

	/**
	* Get overall time of the Profiler in millisecond.
	*/
	double GetTimeMs() const;


	static Profiler* getList()
	{
		return s_start;
	}

	friend class ProfileTimer;
    void AccumulateTime(int64_t duration)
    {
        m_time += duration;
    }
    void IncCount()
    {
        ++m_count;
    }

    static void Dump();
    static void Reset();
    static void DumpAndReset()
    {
        Dump();
        Reset();
    }
private:
	static Profiler* s_start;
	const char* m_name;
	int64_t m_count;
	int64_t m_time;
	Profiler* m_next;
};

//高精度计时器。构造时开始计时，析构时停止计时，并把时间间隔累加到构造器参数Profiler* parent_。
class ProfileTimer
{
public:
	ProfileTimer(Profiler* parent);

	~ProfileTimer();

private:
	Profiler* m_parent;
	int64_t m_begin;
};

}
}


#ifdef ENABLE_PROFILE
#define DK_PROFILE( name )	static dk::utility::Profiler _acc_(name); dk::utility::ProfileTimer _hptimer_(&_acc_);
#else
#define DK_PROFILE( name )
#endif

#define DK_PROFILE_FUNCTION  DK_PROFILE(__PRETTY_FUNCTION__)

//===========================================================================
#endif
