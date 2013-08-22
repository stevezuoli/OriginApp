//===========================================================================
// Summary:
//		XMLAPI.h
// Usage:
//		XML API
// Remarks:
//		Null
// Date:
//		2012-01-10
// Author:
//		pengf(pengf@duokan.com)
//===========================================================================

#ifndef __KERNEL_COMMONLIBS_XMLBASE_XMLAPI_H__
#define __KERNEL_COMMONLIBS_XMLBASE_XMLAPI_H__
#include "XMLDef.h"
#include "XMLBase.h"

//-------------------------------------------
//	Summary:
//		初始化xmlbase库
//	Parameters:
//	    Null
//  Return Value:
//      如果成功则返回DK_TRUE，否则返回DK_FALSE。
//	Remarks:
//		从XMLBase 1.0.5 开始支持。
//-------------------------------------------
XMLAPI DKX_Initialize();

//-------------------------------------------
//	Summary:
//		销毁xmlbase库
//	Parameters:
//	    Null
//  Return Value:
//      如果成功则返回DK_TRUE，否则返回DK_FALSE。
//	Remarks:
//		从XMLBase 1.0.5 开始支持。
//-------------------------------------------
XMLAPI DKX_Destroy();
#endif
