//===========================================================================
// Summary:
//		ActionTypeDef.h
//      Action类型定义
// Usage:
//	    Null
// Remarks:
//		Null
// Date:
//		2011-10-25
// Author:
//		Liu Hongjia(liuhj@duokan.com)
//===========================================================================

#ifndef __KERNEL_COMMONLIBS_KERNELBASE_ACTIONTYPEDEF_H__
#define __KERNEL_COMMONLIBS_KERNELBASE_ACTIONTYPEDEF_H__

//===========================================================================

#include "KernelDef.h"
#include "KernelRenderType.h"

//===========================================================================

enum DK_ACTION_TYPE
{
	DK_ACTION_UNKNOWN,	// 未定义的

	DK_ACTION_GOTO,		// 文档内的跳转
	DK_ACTION_GOTOR,	// 文档间的跳转
	DK_ACTION_LAUNCH,	// 执行一个应用程序，或者是打开一份文档
	DK_ACTION_URI,		// 指向一个URI位置
	DK_ACTION_NAMED,	// 名字
};

enum DK_ACTION_DEST_TYPE
{
	DK_ACTION_DEST_UNKNOWN,		// 未定义

	DK_ACTION_DEST_XYZ,			// 目标区域由左上角位置（Left，Top）以及页面缩放比例（Zoom）确定
	DK_ACTION_DEST_FIT,			// 适合页面
	DK_ACTION_DEST_FITH,		// 适合宽度，目标区域位置仅由Top坐标确定。
	DK_ACTION_DEST_FITV,		// 适合高度，目标区域位置仅由Left 坐标确定
	DK_ACTION_DEST_FITR			// 适合窗口，目标区域为（Left，Top，Right，Bottom）所确定的矩形区域
};

enum DK_ACTION_LAUNCH_OSTYPE
{
	DK_ACTION_LAUNCH_OSTYPE_WIN,		// Windows
	DK_ACTION_LAUNCH_OSTYPE_MAC,		// Mac
	DK_ACTION_LAUNCH_OSTYPE_UNIX		// Unix
};

//===========================================================================

struct DK_ANNOTSINFO
{
	DK_BOX			box;	// 对象的区域
	DK_ACTION_TYPE	type;	// 对象类型
	DK_VOID*		pInfo;	// 对象的信息
} ;

// 页面可点击区域类型
enum DK_CLICKABLE_TYPE
{
	DK_CLICKABLE_ANNOT,				// 注释对象
	DK_CLICKABLE_ACTION,			// 带有动作的图元
};

// 可点击区域支持的鼠标事件
enum DK_CLICKEVENT_TYPE
{
	DK_CLICKEVENT_ME = 1,			// 鼠标进入区域
	DK_CLICKEVENT_MX = 2,			// 鼠标离开区域
	DK_CLICKEVENT_LMD = 3,			// 鼠标左键按下
	DK_CLICKEVENT_LMU = 4,			// 鼠标左键松开
	DK_CLICKEVENT_RMD = 5,			// 鼠标右键按下
	DK_CLICKEVENT_RMU = 6,			// 鼠标右键松开
};

// 可点击区域信息
struct DK_CLICKINFO
{
	DK_BOX				rcBBox;			// 可点击区域外接矩形
	DK_CLICKABLE_TYPE	nObjType;		// 区域类型
	DK_CLICKEVENT_TYPE	nEventType;		// 支持的鼠标事件
	DK_ACTION_TYPE		nActionType;	// 动作类型

	DK_CLICKINFO()
	: nObjType(DK_CLICKABLE_ANNOT), nEventType(DK_CLICKEVENT_LMD), nActionType(DK_ACTION_GOTO)
	{}
};

enum DK_DESTREGION_TYPE
{
	DK_DESTREGION_XYZ,			// 由左上角和缩放比决定
	DK_DESTREGION_FITPAGE,		// 适合页面
	DK_DESTREGION_FITH,			// 适合宽度，位置由Top坐标决定
	DK_DESTREGION_FITV,			// 适合高度，位置由Left坐标决定
	DK_DESTREGION_FITR,			// 适合窗口，目标区域为（Left， Top， Right，Bottom）所确定的矩形区域。
	DK_DESTREGION_FITB,			// 适合Bounding,以矩形大小为准，如果矩形垂直和水平放缩不同，取较小的值，矩形居中。
	DK_DESTREGION_FITBH,		// 同上，位置由Top决定。
	DK_DESTREGION_FITBV			// 同上，位置由Left决定。
};

// Goto Action
struct DKACTIONGOTO 
{
	DK_DESTREGION_TYPE	drType;			// 目标区域类型
	DK_DWORD			dwPageNum;		// 目标页码
	DK_BOX				boxRegion;		// 目标区域。各属性的含义是：X0 - left, X1 - right, Y0 - bottom, Y1 - top。
	DK_BOOL				bLeftValid;		// boxRegion 中，left属性是否有效。如果无效，阅读软件应沿用当前的值或默认值。
	DK_BOOL				bTopValid;		// 类似 bLeftValid。
	DK_BOOL				bRightValid;	// 类似 bLeftValid。
	DK_BOOL				bBottomValid;	// 类似 bLeftValid。
	DK_FLOAT			fZoom;			// 页面区域放缩比例。0 值表示无效。如果无效，阅读软件应沿用当前的值。
	const DK_WSTRING*   szAnchorName;	// 锚点名
	DKACTIONGOTO()
	: drType(DK_DESTREGION_XYZ), dwPageNum(0), bLeftValid(DK_FALSE), bTopValid(DK_FALSE),
	bRightValid(DK_FALSE), bBottomValid(DK_FALSE),fZoom(0), szAnchorName(DK_NULL)
	{}
};

//===========================================================================

// GotoR Action
struct DKACTIONGOTOR
{
	DK_BOOL			bNewWindow;					// 是否在新窗口中打开
	DK_WCHAR		pszFileLoc[DK_MAX_PATH];	// 目标文档的位置
	DKACTIONGOTO	dagDest;					// 目标区域描述
	DKACTIONGOTOR()
	: bNewWindow(DK_FALSE)
	{ memset(pszFileLoc, 0, sizeof(pszFileLoc)); }
};

//===========================================================================

// Launch Action
struct DKACTIONLAUNCH
{
	DK_ACTION_LAUNCH_OSTYPE		osType;						// 操作系统类型
	DK_WCHAR					pszFileLoc[DK_MAX_PATH];	// 目标程序的位置
	DK_WCHAR					pszPara[DK_MAX_PATH];		// 可执行程序参数
	DKACTIONLAUNCH()
	: osType(DK_ACTION_LAUNCH_OSTYPE_WIN)
	{
		memset(pszFileLoc, 0, sizeof(pszFileLoc));
		memset(pszPara, 0, sizeof(pszPara));
	}
};

//===========================================================================

// URI Action
struct DKACTIONURI
{
	DK_WCHAR		pszURI[DK_MAX_URI];			// 目标URI的位置，采用7位ASCII码编码，参见“RFC 2396”
	DK_WCHAR		pszBase[DK_MAX_URI];		// Base URI，用于相对地址引用。
	DKACTIONURI()
	{
		memset(pszURI, 0, sizeof(pszURI));
		memset(pszBase, 0, sizeof(pszBase));
	}
};

//===========================================================================

#endif // #ifndef __KERNEL_COMMONLIBS_KERNELBASE_ACTIONTYPEDEF_H__
