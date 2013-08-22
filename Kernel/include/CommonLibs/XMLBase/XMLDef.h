//===========================================================================
// Summary:
//		XMLDef.h
// Usage:
//		XML Definition
// Remarks:
//		Null
// Date:
//		2012-01-10
// Author:
//		pengf(pengf@duokan.com)
//===========================================================================

#ifndef __KERNEL_COMMONLIBS_XMLBASE_XMLDEF_H
#define __KERNEL_COMMONLIBS_XMLBASE_XMLDEF_H__
//===========================================================================

#ifdef _LINUX
	#ifndef NOT_SUPPORT_FVISIBILITY
		#define XML_API					__attribute__((visibility("default")))
		#define XMLSTDMETHODCALLTYPE		__attribute__((stdcall))
		#define XMLCALLBACK				__attribute__((stdcall))
	#else
		#define XML_API					
		#define XMLSTDMETHODCALLTYPE	
		#define XMLCALLBACK				
	#endif
#else
	/*#ifdef XMLLIB_EXPORTS
		#define XML_API __declspec(dllexport)
	#else
		#define XML_API __declspec(dllimport)
	#endif*/

    #define XML_API					
	#define XMLSTDMETHODCALLTYPE		__stdcall
	#define XMLCALLBACK				__stdcall
#endif

#define XMLAPI				extern "C" XML_API bool
#define XMLAPI_(type)		extern "C" XML_API type

#endif
