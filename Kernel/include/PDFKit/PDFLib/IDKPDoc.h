//===========================================================================
// Summary:
//		IDPKDoc是负责文档打开，获取文档一些属性的接口类。 
// Usage:
//		使用DPK_CreateDoc进行创建，使用DPK_DestoryDoc销毁。
// Remarks:
//		Null
// Date:
//		2011-9-15
// Author:
//		Zhang JingDan (zhangjingdan@duokan.com)
//===========================================================================

#ifndef __KERNEL_PDFKIT_PDFLIB_IDKPDOC_H__
#define __KERNEL_PDFKIT_PDFLIB_IDKPDOC_H__

//===========================================================================

#include "KernelRetCode.h"
#include "DKPTypeDef.h"

class IDKPOutline;
class IDKPPage;
class IDKPPageEx;
class IDkStream;

//===========================================================================

class IDKPDoc
{
public:
    enum REARRANGE_PAGE_POSITION_TYPE
    {
        PREV_PAGE,      // 重排上一页
        LOCATION_PAGE,  // 根据位置跳转的重排页
        NEXT_PAGE,      // 重排下一页
    };

	//-------------------------------------------
	// Summary:
	//		打开文档。
	// Parameters:
	//		[in] pFileOP - 目标文档文件句柄。
	// Returns：
	//		成功则返回DKR_OK，如果需要口令则返回 DKR_PDF_NEED_READ_PASSWORD，打开失败则返回DKR_FAILED。	
	// SDK Version:
	//		从DKP 1.0开始支持
	//-------------------------------------------
	virtual DK_ReturnCode OpenDoc(IDkStream* pFileOP) = 0;

    //-------------------------------------------
    // Summary:
    //		传入口令以打开文档。
    // Parameters:
    //		[in] pFileOP - 目标文档文件句柄。
    //		[in] pPwd - 密码。
    //      [in] length - 密码长度。
    // Returns：
    //		成功则返回DK_TRUE，如果需要口令但传入口令错误，则返回 DKR_PDF_READ_PASSWORD_INCORRECT，打开失败则返回DK_FALSE。	
    // SDK Version:
    //		从DKP 2.2.0开始支持
    //-------------------------------------------
    virtual DK_ReturnCode OpenDoc(IDkStream* pFileOP, const DK_BYTE* pPwd, DK_INT length) = 0;

	//-------------------------------------------
	// Summary:
	//		关闭文档。
	// SDK Version:
	//		从DKP 1.0开始支持
	//-------------------------------------------
	virtual DK_VOID CloseDoc() = 0;

	//-------------------------------------------
	// Summary:
	//		获得页数。
	// Returns:
	//		成功则返回页数，否则返回-1。
	// SDK Version:
	//		从DKP 1.0开始支持
	//-------------------------------------------
	virtual DK_INT GetPageCount() = 0;

	//-------------------------------------------
	// Summary:
	//		绘制指定页面内容。
	// Parameters:
	//		[in] parrRenderInfo	- 绘制参数。
	// Returns:
	//		绘制成功则返回1，失败则返回其他值。
	// SDK Version:
	//		从DKP 1.0开始支持
	//-------------------------------------------
	virtual DK_INT RenderPage(DK_RENDERINFO* parrRenderInfo) = 0;

	//-------------------------------------------
	// Summary:
	//		获得指定页码的页对象。
	// Parameters:
	//		[in] nPageNum - 页码，由1开始。
	// Returns:
	//		成功则返回指定页码的页对象，失败则返回空。
	// SDK Version:
	//		从DKP 1.0开始支持
	//-------------------------------------------
	virtual IDKPPage* GetPage(DK_INT nPageNum) = 0;

    //-------------------------------------------
    // Summary:
    //		释放页对象，调用后页对象指针不再可用，再次使用时必须调用GetPage获取对象。
    // Parameters:
    //		[in] pPage - 页对象指针。
    //		[in] bAll -	是否释放所有内容。
    // SDK Version:
    //		从DKP 1.0开始支持
    //-------------------------------------------
    virtual DK_VOID ReleasePage(IDKPPage* pPage, DK_BOOL bAll = DK_FALSE) = 0;

    //-------------------------------------------
    //  Summary:
    //      设置流式排版模式，只对流式页有效
    //  Parameters:
    //      [in] typeSetting            - 排版模式。
    //  Return Value:
    //      Null
    //  Availability:
    //      从DKP 2.5.0开始支持。
    //-------------------------------------------
    virtual DK_VOID SetTypeSetting(const DKTYPESETTING &typeSetting) = 0;

	//-------------------------------------------
	// Summary:
	//		获得指定流位置的重排页对象。
	// Parameters:
    //		[in] pos            -  指定起始位置
    //                          pos.nChapterIndex 表示页码（起始页为1）;
    //                          pos.nParaIndex 表示通过调用 IDKPPage 函数 GetPageTextContentStream 获得的 PDKPTEXTINFONODE 链表下标;
    //                          pos.nElemIndex 表示 PDKPTEXTINFONODE.DKPTEXTINFO.strContent 下标;
    //      [in] option         - 选项，包括页面矩形，dpi，字号缩放等
    //      [in] posType        - 页偏移类型
    //      [in/out] ppPageEx   - 输出页面对象
	// Returns:
	//		成功则返回 DKR_OK
	// SDK Version:
	//		从DKP 1.0开始支持
	//-------------------------------------------
	virtual DK_ReturnCode GetPageEx(const DK_FLOWPOSITION& pos, const DKP_REARRANGE_PARSER_OPTION& option, REARRANGE_PAGE_POSITION_TYPE posType, IDKPPageEx** ppPageEx) = 0;

    //-------------------------------------------
    // Summary:
    //		释放重排页对象，调用后重排页对象指针不再可用，再次使用时必须调用GetPageEx获取对象。
    // Parameters:
    //		[in] pPage - 重排页对象指针。
    //		[in] bAll -	是否释放所有内容。
    // SDK Version:
    //		从DKP 1.0开始支持
    //-------------------------------------------
    virtual DK_VOID ReleasePageEx(IDKPPageEx* pPage, DK_BOOL bAll = DK_FALSE) = 0;

	//-------------------------------------------
	// Summary:
	//		获得目录对象。
	// Returns:
	//		成功则返回目录对象，失败则返回空。
	// SDK Version:
	//		从DKP 1.0开始支持
	//-------------------------------------------
	virtual IDKPOutline* GetOutline() = 0;

	//-------------------------------------------
	// Summary:
	//		设置默认字体名字。
	// Parameters:
	//		[in] pszDefaultFontFaceName - 字体名字。
	//		[in] charset - 字体编码。
	// SDK Version:
	//		从DKP 1.0开始支持
	//-------------------------------------------
	virtual DK_VOID SetDefaultFontFaceName(const DK_WCHAR* pszDefaultFontFaceName, DK_CHARSET_TYPE charset) = 0;

	//-------------------------------------------
	// Summary:
	//		获取文档的元数据。
	// Parameters:
	//		[out] pMetaData - 元数据,传入空即可。
	// SDK Version:
	//		从DKP 1.0开始支持
	//-------------------------------------------
	virtual DK_BOOL GetDocMetaData(PDKPMETADATA& pMetaData) = 0;
	virtual DK_BOOL ReleaseMetaData() = 0;

    //-------------------------------------------
    // Summary:
    //		获取当前文档的重排模式。
    // Parameters:
    //      重排模式由内核判断，所有页面的重排均采用同一种重排模式。
    // SDK Version:
    //		从DKP 2.4.0开始支持
    //-------------------------------------------
    virtual DKP_REARRANGE_MODE GetRearrangeMode() = 0;

public:
	virtual ~IDKPDoc() {}
};

//===========================================================================

#endif
