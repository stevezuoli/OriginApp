/* -------------------------------------------------------------------------
//	File Name	:	CrossPlatformConfig.h
//	Author		:	Zhang Fan
//	Create Time	:	2012-3-10 16:00:21
//	Description	:	
//
// -----------------------------------------------------------------------*/

#ifndef __CrossPlatformConfig_H__
#define __CrossPlatformConfig_H__

// -------------------------------------------------------------------------

// basic types
#if defined(_WIN32) || defined(_WIN64)
	#include <basetsd.h>
	typedef INT16  Int16;
	typedef UINT16 UInt16;
	typedef INT32  Int32;
	typedef UINT32 UInt32;
	typedef INT64  Int64;
	typedef UINT64 UInt64;
#elif defined(__APPLE__)
	#include <stdint.h>
	typedef int16_t			Int16;
	typedef uint16_t		UInt16;
	typedef int32_t			Int32;
	typedef int64_t			Int64;
	typedef uint64_t		UInt64;
	#define UNREFERENCED_PARAMETER(p) {static_cast<void>(p)}

	#include "TargetConditionals.h"
	#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
		typedef unsigned long	UInt32;
	#elif TARGET_OS_MAC
		typedef uint32_t	UInt32;
	#endif
#else
	#include <stdint.h>
	typedef int16_t		Int16;
	typedef uint16_t	UInt16;
	typedef int32_t		Int32;
	typedef uint32_t	UInt32;
	typedef int64_t		Int64;
	typedef uint64_t	UInt64;
	#define UNREFERENCED_PARAMETER(p) {static_cast<void>(p)}
#endif

typedef unsigned char Byte;

#if defined(_WIN32) || defined(_WIN64)
	#ifndef E_ACCESSDENIED
		#include <wtypes.h>
	#endif	// E_ACCESSDENIED
#endif		// _WIN32 || _WIN64


#if defined(_WIN32) || defined(_WIN64)
    #ifdef LIBKSS_EXPORTS
        #define KSS_API __declspec(dllexport)
    #else
        #define KSS_API __declspec(dllimport)
    #endif		// LIBKSS_EXPORTS

    // for gtest
    #ifdef GTEST_UNITS
	    #undef KSS_API 
	    #define KSS_API
    #endif
#else
	    #define KSS_API
#endif	//_WIN32 || _WIN64


#if defined(_WIN32) || defined(_WIN64)
	#pragma warning(disable: 4251)
#endif // _WIN32 || _WIN64

#define KSS_INTERFACE struct


#include <string>
#include <vector>
#include "../Neptune/NptFile.h"

//--------------------------------------------------------------------------
#endif /* __CrossPlatformConfig_H__ */
