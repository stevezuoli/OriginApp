//===========================================================================
// Summary:
//		Render的数据类型定义。
// Usage:
//		Null
// Remarks:
//		Null
// Date:
//		2012-01-30
// Author:
//		Peng Feng(pengf@duokan.com)
//===========================================================================

#ifndef __KERNEL_COMMONLIBS_RENDER_EXPORT_DKRDEF_H__
#define __KERNEL_COMMONLIBS_RENDER_EXPORT_DKRDEF_H__

//===========================================================================

#ifdef _LINUX
	#ifndef NOT_SUPPORT_FVISIBILITY
		#define RENDER_API					__attribute__((visibility("default")))
		#define RENDERSTDMETHODCALLTYPE		__attribute__((stdcall))
		#define RENDERCALLBACK				__attribute__((stdcall))
	#else
		#define RENDER_API					
		#define RENDERSTDMETHODCALLTYPE	
		#define RENDERCALLBACK				
	#endif
#else
    #define RENDER_API					
	#define RENDERSTDMETHODCALLTYPE		__stdcall
	#define RENDERCALLBACK				__stdcall
#endif

#define RENDERAPI				extern "C" RENDER_API bool
#define RENDERAPI_(type)		extern "C" RENDER_API type

//===========================================================================

#endif 
