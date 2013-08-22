//===========================================================================
// Summary:
//	    XMLSaxParser.h
// Usage:
//      定义XML的SAX解析器使用的回调函数类型
//  		...
// Remarks:
//	    null
// Date:
//	    2011-12-1
// Author:
//	    Zhang Jiafang(zhangjf@duokan.com)
//===========================================================================

#ifndef __KERNEL_COMMONLIBS_XMLBASE_XMLSAXHANDLER_H__
#define __KERNEL_COMMONLIBS_XMLBASE_XMLSAXHANDLER_H__

#include "XMLBase.h"

typedef DK_VOID (*StartElementHandler)(DK_VOID* pUserData, const DK_CHAR* pName, const DK_CHAR** ppAttrs);
typedef DK_VOID (*EndElementHandler)(DK_VOID* pUserData, const DK_CHAR* pName);

typedef DK_VOID (*CharacterDataHandler)(DK_VOID* pUserData, const DK_CHAR* pData, DK_INT length);

typedef DK_VOID (*CommentHandler)(DK_VOID* pUserData, const DK_CHAR* pComment);

typedef DK_VOID (*StartCDATAHandler)(DK_VOID* pUserData);
typedef DK_VOID (*EndCDATAHandler)(DK_VOID* pUserData);

typedef DK_VOID (*SkippedEntityHandler)(DK_VOID* pUserData, const DK_CHAR* pEntityName, DK_INT isParamEntity);

typedef DK_VOID (*StartNamespaceDeclHandler)(DK_VOID* pUserData, const DK_CHAR* pPrefix, const DK_CHAR* pUri);
typedef DK_VOID (*EndNamespaceDeclHandler)(DK_VOID* pUserData, const DK_CHAR* pPrefix);

typedef DK_VOID (*XMLDeclarationHandler)(DK_VOID* pUserData, const DK_CHAR* pVersion, const DK_CHAR* pEncoding, DK_INT standalone);

#endif
