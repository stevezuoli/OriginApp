//===========================================================================
// Summary:
//		Font管理系统接口。
// Usage:
//		Null
// Remarks:
//		Null
// Date:
//		2011-09-19
// Author:
//		Peng Feng(pengf@duokan.com)
//===========================================================================

#ifndef	__KERNEL_COMMONLIBS_RENDER_IDKFONTMANAGER_H__
#define	__KERNEL_COMMONLIBS_RENDER_IDKFONTMANAGER_H__

//===========================================================================

#include "KernelType.h"
#include <vector>
#include <map>
#include "KernelRetCode.h"
#include "DkRenderDef.h"


typedef std::vector<DK_WSTRING> StringArray;

//===========================================================================

class IDKFontManager
{
public:
	virtual ~IDKFontManager(){};

	//-------------------------------------------
	// Summary:
	//		创建字体管理引擎
	// Parameters:
	//		[in] type	- 字体管理引擎类型
	//	Remarks:
	//		工程设置中必须包含默认绘制引擎(SUPPORT_XXX_FONTMANAGER) 
	//		或备选引擎(USE_XXX_FONTMANAGER_DEFAULT)的宏定义方可创建相应类型的对象
	//		通过delete释放创建的对象
	//-------------------------------------------
	// static IDKFontManager* CreateFontManager(DK_FONTMANAGER_TYPE type = DK_FONTMANAGER_DEFAULT);

public:
	// 根据PDF中的字体名字得到相应字体的FaceName
	virtual DK_BOOL FindFaceName(const DK_WCHAR* pszFontNameInPDF, DK_WSTRING* strFaceName) const = 0;
	// 查找相近的字体并替换系统中不存在的字体
	virtual DK_BOOL FindFaceNameAlike(const DK_WSTRING& strFontName, DK_WSTRING* strSubstituteFaceName) const = 0;

	// 获取字体编码标准
	virtual DK_CHARSET_TYPE GetFontCharset(const DK_WCHAR* pszFontName) const = 0;

	// 初始化所有的TrueType字库信息
	virtual DK_VOID InitTrueTypeFontInfo(const DK_WCHAR* szResourceDir = DK_NULL, DK_BOOL bInitPFBFont = DK_TRUE) = 0;
	// 从字体的FaceName得到相应的字体文件名字
	virtual DK_BOOL GetFontFileNameByFaceName(const DK_WCHAR* pszFaceName,
                                                DK_WSTRING* strFileName,
                                                const DK_WCHAR** ppszActualFaceName = DK_NULL) const = 0;
	// 根据FaceName从注册表中获取相应的FullName信息
	virtual DK_BOOL GetAllFullFontNameFromCache(const DK_WCHAR* pszFaceName, StringArray& arrAllFullFontsName) = 0;

public:
	// 添加FaceName到字体文件的对应关系到列表
	virtual DK_BOOL AddFaceName(const DK_WCHAR* pszFaceName, const DK_WCHAR* pszFontFile) = 0;
	// 指定charset的默认字体名
	virtual DK_BOOL SetDefaultFontFaceName(const DK_WCHAR* pszDefaultGBFontFaceName, DK_CHARSET_TYPE charset) = 0;
	// 得到charset的默认的字体名
	virtual const DK_WCHAR* GetDefaultFontFaceName(DK_CHARSET_TYPE charset) const = 0;
	// 得到charset的默认字体文件路径名
	virtual const DK_WCHAR* GetDefaultFontFilePath(DK_CHARSET_TYPE charset) const = 0;

    // 添加嵌入字体
    virtual const DKR_MEMORY_FONT_INFO* AddEmbeddedFont(const DK_WCHAR* pszFaceName, const DK_BYTE* fontData, DK_UINT fontDataSize) = 0;

    // 判断嵌入字体是否已添加
    virtual DK_BOOL IsEmbeddedFontAdded(const DK_WCHAR* pszFaceName) const = 0;

    // 获得嵌入字体信息
    virtual const DKR_MEMORY_FONT_INFO* GetEmbeddedFont(const DK_WCHAR* pszFaceName) const = 0;
};

//===========================================================================

extern IDKFontManager* g_pSystemFontManager;		// 全局FontManager

//===========================================================================
#endif
