//===========================================================================
// Summary:
//		IDKPPage是负责页面内容，获取页面一些属性以及内容流等数据的接口类。 
// Usage:
//		NULL
// Remarks:
//		Null
// Date:
//		2011-9-15
// Author:
//		Zhang JingDan (zhangjingdan@duokan.com)
//===========================================================================

#ifndef __KERNEL_PDFKIT_PDFLIB_IDKPPAGE_H__
#define __KERNEL_PDFKIT_PDFLIB_IDKPPAGE_H__

//===========================================================================

#include "DKPTypeDef.h"

//===========================================================================

enum DKP_FIND_ERRORNO
{
	DKP_FIND_ERROR,								// 查找出错。
	DKP_FIND_SUCCESS,							// 查找成功。
	DKP_FIND_NOMATCHWORD,						// 已经搜索到文件末尾，没有找到...
	DKP_FIND_REACHFILEEND,						// 已经搜索到文件末尾。
	DKP_FIND_USERSTOP,							// 用户结束
};

class IDKPPageDistiller;
class IDKPImageEnumerator;
class IDKPAnnotList;

//===========================================================================

class IDKPPage
{
public:
	//-------------------------------------------
	// Summary:
	//		获得页面矩形。
	// Parameters:
	//		[in] type - 矩形框类型。
	//		[out] pRect - 矩形框。
	// Returns:
	//		成功则返回DK_TRUE，失败则返回DK_FALSE。
	// Availability:
	//		从PDF 1.0开始支持。
	//-------------------------------------------
	virtual DK_BOOL GetPageBBox(DKP_PAGEBOX_TYPE type, DKP_DRECT* pRect) = 0;

	//-------------------------------------------
	// Summary:
	//		主工程要求的旋转角度。
	// Returns:
	//		返回旋转角度。
	// Availability:
	//		从PDF 1.0开始支持。
	//-------------------------------------------
	virtual DK_INT GetPageAppendRotate() = 0;

	//-------------------------------------------
	// Summary:
	//		页的旋转角度。
	// Returns:
	//		返回旋转角度。
	// Availability:
	//		从PDF 1.0开始支持。
	//-------------------------------------------
	virtual DK_INT GetPageRotate() = 0;

	//-------------------------------------------
	// Summary:
	//		获取Page的UserUnit,代表每个文档单位代表UserUnit个英寸。
	// Returns:
	//		返回Page的UserUnit。
	// Availability:
	//		从PDF 1.0开始支持。
	//-------------------------------------------
	virtual DK_DOUBLE GetPageUserUnit() = 0;

	//-------------------------------------------
	// Summary:
	//		获取页对象的注释跳转等信息。
	// Parameters:
	//		[out] pAnnotsInfo - Annot信息。
	//		[out] nCount - Annot的个数
	// Availability:
	//		从PDF 1.0开始支持。
	//-------------------------------------------
	//TODO(zhangjingdan):先闭掉,以后添加
	//virtual DK_BOOL GetPageAnnots(DK_ANNOTSINFO*& pAnnotsInfo, DK_INT& nCount) = 0;
	//virtual DK_BOOL FreePageAnnots(DK_ANNOTSINFO* pAnnotsInfo, DK_INT nCount) = 0;

	//-------------------------------------------
	// Summary:
	//		将文档单位转换为英寸。
	// Parameters:
	//		[in/out] 以文档单位为单位。
	// Availability:
	//		从PDF 1.0开始支持。
	//-------------------------------------------
	virtual DK_VOID DocUnit2Inch(DK_DOUBLE& dValue) = 0;
	virtual DK_VOID DocUnit2Inch(DK_POS& pos) = 0;
	virtual DK_VOID DocUnit2Inch(DKP_DRECT& dRect) = 0;

	//-------------------------------------------
	// Summary:
	//		将文档单位转换为MM。
	// Parameters:
	//		[in/out] 以文档单位为单位。
	// Availability:
	//		从PDF 1.0开始支持。
	//-------------------------------------------
	virtual DK_VOID DocUnit2MM(DK_DOUBLE& dValue) = 0;
	virtual DK_VOID DocUnit2MM(DK_POS& pos) = 0;
	virtual DK_VOID DocUnit2MM(DKP_DRECT& dRect) = 0;

	//-------------------------------------------
	// Summary:
	//		英寸转换为文档单位。
	// Parameters:
	//		[in/out] 为英寸为单位。
	// Availability:
	//		从PDF 1.0开始支持。
	//-------------------------------------------
	virtual DK_VOID Inch2DocUnit(DK_DOUBLE& dValue) = 0;
	virtual DK_VOID Inch2DocUnit(DK_POS& pos) = 0;
	virtual DK_VOID Inch2DocUnit(DKP_DRECT& dRect) = 0;

	//-------------------------------------------
	// Summary:
	//		MM转换为文档单位。
	// Parameters:
	//		[in/out] 为英寸为单位。
	// Availability:
	//		从PDF 1.0开始支持。
	//-------------------------------------------
	virtual DK_VOID MM2DocUnit(DK_DOUBLE& dValue) = 0;
	virtual DK_VOID MM2DocUnit(DK_POS& pos) = 0;
	virtual DK_VOID MM2DocUnit(DKP_DRECT& dRect) = 0;
    
    //-------------------------------------------
    //  Summary:
    //      获得版式页面文字内容的流式起始位置。
    //  Parameters:
    //      [out] pPos - 对应版式页面文字内容的流式起始位置，闭区间。
    //  Return Value:
    //      获取成功则返回 DK_TRUE，否则返回 DK_FALSE。
    //  Remarks:
    //      Null
    //  Availability:
    //      从 PDFLib 2.6.0开始支持。
    //-------------------------------------------
    virtual DK_BOOL GetPageTextContentStartPos (DK_FLOWPOSITION* pPos) = 0;

    //-------------------------------------------
    //  Summary:
    //      获得版式页面文字内容的流式结束位置。
    //  Parameters:
    //      [out] pPos - 对应版式页面文字内容的流式结束位置，开区间。
    //  Return Value:
    //      获取成功则返回 DK_TRUE，否则返回 DK_FALSE。
    //  Remarks:
    //      Null
    //  Availability:
    //      从 PDFLib 2.6.0开始支持。
    //-------------------------------------------
    virtual DK_BOOL GetPageTextContentEndPos (DK_FLOWPOSITION* pPos) = 0;

	//-------------------------------------------
	// Summary:
	//		获取页的Distiller
	// Returns:
	//		返回该页的Distiller，如果失败返回DK_NULL。
	// Availability:
	//		从PDF 1.0开始支持。
	//-------------------------------------------
	virtual IDKPPageDistiller* GetPageDistiller() = 0;

	//-------------------------------------------
	// Summary:
	//		获得页面文字内容。
	// Parameters:
	//		[out] pText - 文字流内容。调用者无需释放。当Release()被调用且当前页引用为0时，pText指向内存将被释放。
	//		[out] nTextLen - 文字流内容长度。
	//		[in] bAutoBreakLine - 是否自动换行。
	//		[in] nType - 获取类型，DKP_TEXTCONTENT_RAW		： 原始文本。
	//								DKP_TEXTCONTENT_TYPESET	： 按排版顺序。
	// Returns:
	//		成功则返回DK_TRUE，失败则返回DK_FALSE。
	// Availability:
	//		从PDF 1.0开始支持。
	//-------------------------------------------
	virtual DK_BOOL GetPageTextContent(DK_WCHAR *&pText, DK_INT& nTextLen, DK_BOOL bAutoBreakLine = DK_TRUE, DKP_TEXTCONTENT_TYPE type = DKP_TEXTCONTENT_RAW) = 0;

	//-------------------------------------------
	// Summary:
	//		获取指定位置文本。
	// Parameters:
	//		[in] ptStart - 起始点坐标。
	//		[in] ptEnd - 终止点坐标。
	//		[in] nType - 选择方式，1为按原始排版顺序，2为按矩形区域。
	//		[in] bAutoBreakLine - 是否换行。
    //      [in] mode - 设置传入坐标与待选中文字的位置关系，参考DKP_SELECTION_MODE
	//		[out] pRects - 选中文字矩形区域数组。
	//		[out] nRectCount - 选中文字矩形区域数组长度。
	//		[out] pText - 选中文字内容。
    //		[out] nTextLen - 选中文字内容长度。
    //		[out] startPos - 选中文字的起始流式位置（闭区间）。
    //		[out] endPos - 选中文字的结束流式位置（开区间）。
	// Returns:
	//		成功则返回DK_TRUE，失败则返回DK_FALSE。
	// Availability:
	//		从PDF 1.0开始支持。
	//-------------------------------------------
	virtual DK_BOOL GetTextContentEx(DK_POINT ptStart, DK_POINT ptEnd, DK_INT nType, DK_BOOL bAutoBreakLine, DKP_SELECTION_MODE mode, DKP_RECT*& pRects,
									DK_INT& nRectCount, DK_WCHAR*& pText, DK_INT& nTextLen, DK_FLOWPOSITION& startPos, DK_FLOWPOSITION& endPos) = 0;

    //-------------------------------------------
    // Summary:
    //		根据流式位置获取版式页面的文本矩形
    // Parameters:
    //		[in] startPos - 起始流式位置。
    //		[in] endPos - 终止流式位置。
    //		[in] nType - 选择方式，1为按原始排版顺序，2为按矩形区域。
    //		[in] bAutoBreakLine - 是否换行。
    //		[out] pRects - 选中文字矩形区域数组。
    //		[out] nRectCount - 选中文字矩形区域数组长度。
    // Returns:
    //		成功则返回DK_TRUE，失败则返回DK_FALSE。
    // Availability:
    //		从PDF 2.2.0开始支持。
    //-------------------------------------------
    virtual DK_BOOL GetTextRects(const DK_FLOWPOSITION& startPos, const DK_FLOWPOSITION& endPos, DK_INT nType, DK_BOOL bAutoBreakLine,
                                    DKP_RECT*& pRects, DK_INT& nRectCount) = 0;

    //-------------------------------------------
	// Summary:
	//		获取指定坐标处的一段连续文本。
    //      当坐标位置为象形文字时（如中文、日文），得到文字矩形对应该字所在的一整“句”内容；
    //      当坐标位置为西欧字符时，得到文字矩形对应该字所在“词”的内容
	// Parameters:
	//		[in] ptHit - 取词点坐标。
	//		[in] bAutoBreakLine - 是否换行。
	//		[out] pRects - 选中文字矩形区域数组。
	//		[out] nRectCount - 选中文字矩形区域数组长度。
	//		[out] pText - 选中文字内容。
	//		[out] nTextLen - 选中文字内容长度。
	// Returns:
	//		成功则返回DK_TRUE，失败则返回DK_FALSE。
	// Availability:
	//		从PDF 1.0开始支持。
	//-------------------------------------------
    virtual DK_BOOL GetHitTestEx(DK_POINT ptHit, DK_BOOL bAutoBreakLine, DKP_RECT *&pRects, DK_INT &nRectCount,
                                DK_WCHAR *&pText, DK_INT& nTextLen) = 0;

    //-------------------------------------------
    // Summary:
    //		按模式获取指定坐标处的一段连续文本。
    //      当坐标位置为象形文字时（如中文、日文），得到文字矩形对应该字所在的一整“句”内容；
    //      当坐标位置为西欧字符时，得到文字矩形对应该字所在“词”的内容
    // Parameters:
    //		[in] ptHit - 取词点坐标。
    //		[in] bAutoBreakLine - 是否换行。
    //      [in] mode - 指定HitTest模式，参考 DKP_HITTEST_TEXT_MODE 定义
    //		[out] pRects - 选中文字矩形区域数组。
    //		[out] nRectCount - 选中文字矩形区域数组长度。
    //		[out] pText - 选中文字内容。
    //		[out] nTextLen - 选中文字内容长度。
    //		[out] startPos - 选中文字的起始流式位置（闭区间）。
    //		[out] endPos - 选中文字的结束流式位置（开区间）。
    // Returns:
    //		成功则返回DK_TRUE，失败则返回DK_FALSE。
    // Availability:
    //		从PDF 2.2.0开始支持。
    //-------------------------------------------
    virtual DK_BOOL GetHitTestByModeEx(DK_POINT ptHit, DK_BOOL bAutoBreakLine, DKP_HITTEST_TEXT_MODE mode, DKP_RECT *&pRects, DK_INT &nRectCount,
                                        DK_WCHAR *&pText, DK_INT& nTextLen, DK_FLOWPOSITION& startPos, DK_FLOWPOSITION& endPos) = 0;

	//-------------------------------------------
	// Summary:
	//		获得页面文字内容全信息。
	// Parameters:
	//		[out] pStreamHead - 文字内容节点流根节点。
	//		[in] ctTag - 拷贝类型。
	// Returns:
	//		成功则返回DK_TRUE，失败则返回DK_FALSE。
	// Availability:
	//		从PDF 1.0开始支持。
	//-------------------------------------------
	virtual DK_BOOL GetPageTextContentStream(PDKPTEXTINFONODE& pStreamHead, DKP_COPY_TYPE ctTag = DKP_COPY_NOTRANS) = 0;

	//-------------------------------------------
	// Summary:
	//		获得页面图像内容全信息。
	// Parameters:
	//		[out] pStreamHead - 图像内容节点流根节。
	//		[out] nImageCount - 内存节点个数。
	// Returns:
	//		成功则返回DK_TRUE，失败则返回DK_FALSE。
	// Availability:
	//		从PDF 1.0开始支持。
	//-------------------------------------------
	virtual DK_BOOL GetPageImageContentStream(PDKPIMAGECONTENT& pStreamHead, DK_INT& nImageCount) = 0;

	//-------------------------------------------
	// Summary:
	//		获取图像枚举器。
	// Availability:
	//		从PDF 1.0开始支持。
	//-------------------------------------------
	virtual IDKPImageEnumerator* GetImageEnumerator() = 0;
	virtual DK_VOID ReleaseImageEnumerator(IDKPImageEnumerator* pImageEnumerator) = 0;

	//-------------------------------------------
	// Summary:
	//		在文档中搜索内容,在页面中搜索内容，返回所有匹配结果的位置。对返回数据的使用方式一般是：
	//		循环nMatchCount次，从pRectCounts取得每个匹配项所包含的矩形个数，再用这个个数
	//		遍历pRects，取得每个匹配的位置显示出来。
	// Parameters:
	//		[in] option - 查找设置。
	//		[in/out] pResults - 返回的查找结果。
	//		[in/out] nMatchCount - 匹配结果个数。
	// Availability:
	//		从PDF 1.0开始支持。
	//-------------------------------------------
	virtual DKP_FIND_ERRORNO FindTextInPage(const DKP_SEARCHOPTION& option, DKP_SEARCHRESULT*& pResults, DK_UINT& nMatchCount) = 0;

	//-------------------------------------------
	// Summary:
	//		释放指定页面内容。
	// Returns:
	//		成功则返回DK_TRUE，失败则返回DK_FALSE。
	// Availability:
	//		从PDF 1.0开始支持。
	//-------------------------------------------
	virtual DK_BOOL FreePageContentStream() = 0;

	//-------------------------------------------
	// Summary:
	//		释放当前页，一般不需要调用，会在Doc释放时统一调用。
	//		如果要提前释放页，则可以调用该接口。
	// Availability:
	//		从PDF 1.0开始支持。
	//-------------------------------------------
	virtual DK_INT Release() = 0;

	//-------------------------------------------
	// Summary:
	//		返回此页对象的注释列表。
	// Returns:
	//		此页对象的注释列表对象指针。
	// Remarks:
	//		此函数可用于判断此页是否含有一个注释列表。
	// Availability:
	//		从PDF 1.0开始支持。
	//-------------------------------------------
	virtual IDKPAnnotList* GetAnnotList() = 0;

	//-------------------------------------------
	// Summary:
	//		创建一个空注释列表。
	// Returns:
	//		新创建的注释列表对象指针。
	// Remarks:
	//		如果已有一个注释列表对象将直接返回。
	// Availability:
	//		从PDF 1.0开始支持。
	//-------------------------------------------
	virtual IDKPAnnotList* CreateAnnotList() = 0;

	//-------------------------------------------
	// Summary:
	//		删除注释列表对象。
	// Remarks:
	//		同时会删除它所包含的注释。
	// Availability:
	//		从PDF 1.0开始支持。
	//-------------------------------------------
	virtual DK_VOID RemoveAnnotList() = 0;

public:
	virtual ~IDKPPage() {}
};

//===========================================================================

#endif
