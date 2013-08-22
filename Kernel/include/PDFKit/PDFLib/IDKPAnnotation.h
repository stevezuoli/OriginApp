//===========================================================================
// Summary:
//		IDKPAnnotation是负责注释相关功能的接口，它负责获取注释的各项属性。每
//		一个文档页面可以包含一个IDKPAnnotList对象，而一个IDKPAnnotList对象又
//		可以包含多个IDKPAnnotation对象。
// Usage:
//		获取到的指针或数组类型不需要外部进行释放，它们所占用的内存会在注释
//		析构时被释放。
// Remarks:
//		Null
// Date:
//		2011-05-11
// Author:
//		Zhang Jingdan (zhangjingdan@duokan.com)
//===========================================================================

#ifndef __KERNEL_PDFKIT_PDFLIB_IDKPANNOTATION_H__
#define __KERNEL_PDFKIT_PDFLIB_IDKPANNOTATION_H__

//===========================================================================

#include "DKPTypeDef.h"
#include "IDKPUnknown.h"

//===========================================================================

class IDKPAnnotation : public IDKPUnknown
{
public:
	//-------------------------------------------
	// Summary:
	//		获取注释类型
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DKP_ANNOT_TYPE GetType() = 0;

	//-------------------------------------------
	// Summary:
	//		获取注释标志位
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_ULONG GetFlags() = 0;

	//-------------------------------------------
	// Summary:
	//		获取注释名称
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual const DK_WCHAR* GetName() = 0;

	//-------------------------------------------
	// Summary:
	//		获取内容字符串
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual const DK_WCHAR* GetContents() = 0;

	//-------------------------------------------
	// Summary:
	//		获取注释边框
	// Parameters:
	//		[out] rcBB - 用于接收边框矩形
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_VOID GetBoundingBox(DKP_DRECT& rcBB) = 0;

	//-------------------------------------------
	// Summary:
	//		获取线宽，单位point。
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_DOUBLE GetBorderWidth() = 0;

	//-------------------------------------------
	// Summary:
	//		获取虚线数组
	// Parameters:
	//		[out] pDashArray - 用于接收虚线数组的值
	// Return:
	//		返回虚线数组的元素个数。
	// Remarks:
	//		虚线相位不能修改，且默认为0。
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_UINT GetDashArray(DK_DOUBLE*& pDashArray) = 0;

	//-------------------------------------------
	// Summary:
	//		获取颜色数组
	// Parameters:
	//		[out] pColor - 用于接收颜色数组各通道的值
	// Return:
	//		返回颜色通道数，[0, 4]。
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_UINT GetColorArray(DK_DOUBLE*& pColor) = 0;

	//-------------------------------------------
	// Summary:
	//		获取修改日期
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual const DK_WCHAR* GetModificationDate() = 0;

public:
	virtual ~IDKPAnnotation() {}
};

class IDKPAnnotList : public IDKPUnknown
{
public:
	//-------------------------------------------
	// Summary:
	//		获取列表中的注释个数
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_UINT GetAnnotCount() = 0;

	//-------------------------------------------
	// Summary:
	//		获取指定注释的指针
	// Parameters:
	//		[in] nIndex - 要获取的注释在当前页注释列表中的序号，从0开始
	// Return:
	//		注释的指针，若无指定序号的注释则返回DK_NULL。
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual IDKPAnnotation* GetAnnotByIndex(DK_UINT nIndex) = 0;

	//-------------------------------------------
	// Summary:
	//		根据注释指针查询序号
	// Parameters:
	//		[in] pAnnot - 要查询序号的注释
	// Return:
	//		指定注释的序号，若查找失败则返回-1。
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_INT GetIndexByAnnot(const IDKPAnnotation* pAnnot) = 0;

public:
	virtual ~IDKPAnnotList() {}
};

class IDKPMarkupAnnot : public IDKPUnknown
{
public:
	//-------------------------------------------
	// Summary:
	//		获取此批注注释的弹出窗口注释
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual IDKPAnnotation* GetPopup() = 0;

	//-------------------------------------------
	// Summary:
	//		获取批注作者，以'0'结尾的字符串
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual const DK_WCHAR* GetAuthor() = 0;

	//-------------------------------------------
	// Summary:
	//		获取批注透明度
	// Return:
	//		返回值0表示完全透明，1表示完全不透明
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_DOUBLE GetOpacity() = 0;

	//-------------------------------------------
	// Summary:
	//		获取批注生成日期，以'0'结尾的字符串
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual const DK_WCHAR* GetCreationDate() = 0;

	//-------------------------------------------
	// Summary:
	//		获取此注释所回复或从属的注释
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual IDKPAnnotation* GetIRT() = 0;

	//-------------------------------------------
	// Summary:
	//		获取回复类型
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DKP_ANNOT_REPLY_TYPE GetRT() = 0;

	//-------------------------------------------
	// Summary:
	//		获取批注动机，以'0'结尾的字符串
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual const DK_WCHAR* GetIT() = 0;

public:
	virtual ~IDKPMarkupAnnot() {}
};

class IDKPPopupAnnot : public IDKPUnknown
{
public:
	//-------------------------------------------
	// Summary:
	//		获取父注释对象指针
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual IDKPAnnotation* GetParent() = 0;

	//-------------------------------------------
	// Summary:
	//		获取开启状态
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_BOOL GetOpen() = 0;

public:
	virtual ~IDKPPopupAnnot() {}
};

class IDKPTextAnnot : public IDKPUnknown
{
public:
	//-------------------------------------------
	// Summary:
	//		获取文本注释开启状态
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_BOOL GetOpen() = 0;

	//-------------------------------------------
	// Summary:
	//		获取文本注释图标类型，如果从注释字典中读取到的类型不是标准类型，则返回默认值DKP_TEXT_ANNOT_ICON_NOTE。
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DKP_TEXT_ANNOT_ICON_TYPE GetIcon() = 0;

	//-------------------------------------------
	// Summary:
	//		获取文本注释状态
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DKP_TEXT_ANNOT_STATE_TYPE GetState() = 0;

	//-------------------------------------------
	// Summary:
	//		获取文本注释状态模型
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DKP_TEXT_ANNOT_STATE_MODEL_TYPE GetStateModel() = 0;

public:
	virtual ~IDKPTextAnnot() {}
};

class IDKPLinkAnnot : public IDKPUnknown
{
public:
	//-------------------------------------------
	// Summary:
	//		获取链接注释加亮类型
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DKP_LINK_ANNOT_HIGHLIGHT_TYPE GetHighlightType() = 0;

	//-------------------------------------------
	// Summary:
	//		获取链接激活区域的坐标。
	// Parameters:
	//		[out] pQuadArray - 用于接收四边形数组
	// Return:
	//		返回获取到的四边形个数。
	// Remarks:
	//		此区域由一或多个四边形构成。
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_UINT GetQuadArray(DKP_DQUAD*& pQuadArray) = 0;

public:
	virtual ~IDKPLinkAnnot() {}
};

class IDKPFreeTextAnnot : public IDKPUnknown
{
public:
	//-------------------------------------------
	// Summary:
	//		获取默认外观字符串
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual const DK_WCHAR* GetDA() = 0;

	//-------------------------------------------
	// Summary:
	//		获取对其方式
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DKP_FREETEXT_ANNOT_QUADDING_TYPE GetQ() = 0;

	//-------------------------------------------
	// Summary:
	//		获取标注线顶点数组
	// Parameters:
	//		[out] dPoints - 用于接收标注线顶点坐标
	// Return:
	//		返回获取到的顶点个数，2或3。
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_UINT GetCL(DKP_DPOINT*& dPoints) = 0;
	
	//-------------------------------------------
	// Summary:
	//		获取内外矩形差异
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_VOID GetRD(DKP_DRECT& rcRD) = 0;

	//-------------------------------------------
	// Summary:
	//		是否采用云形边框
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_BOOL IsCloudy() = 0;

	//-------------------------------------------
	// Summary:
	//		云形边框强度，[0.0, 2.0]。
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_DOUBLE GetIntensity() = 0;

	//-------------------------------------------
	// Summary:
	//		获取端点类型
	// Parameters:
	//		[in] bStart - 值为DK_TRUE时获取起点，为DK_FALSE时获取终点
	// Return:
	//		返回对应的端点类型
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DKP_LINE_ENDING_TYPE GetLineEnding(DK_BOOL bStart) = 0;

public:
	virtual ~IDKPFreeTextAnnot() {}
};

class IDKPTextMarkupAnnot : public IDKPUnknown
{
public:
	//-------------------------------------------
	// Summary:
	//		获取文本区域的坐标。
	// Parameters:
	//		[out] pQuadArray - 用于接收四边形数组
	// Return:
	//		返回获取到的四边形个数。
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_UINT GetQuadArray(DKP_DQUAD*& pQuadArray) = 0;

public:
	virtual ~IDKPTextMarkupAnnot() {}
};

class IDKPCaretAnnot : public IDKPUnknown
{
public:
	//-------------------------------------------
	// Summary:
	//		获取脱字符号类型
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DKP_CARET_ANNOT_SYMBOL_TYPE GetSymbol() = 0;

	//-------------------------------------------
	// Summary:
	//		获取内外矩形差异
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_VOID GetRD(DKP_DRECT& rcRD) = 0;

public:
	virtual ~IDKPCaretAnnot() {}
};

class IDKPStampAnnot : public IDKPUnknown
{
public:
	//-------------------------------------------
	// Summary:
	//		获取印章图标类型
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DKP_STAMP_ANNOT_ICON_TYPE GetIcon() = 0;

public:
	virtual ~IDKPStampAnnot() {}
};

class IDKPInkAnnot : public IDKPUnknown
{
public:
	//-------------------------------------------
	// Summary:
	//		获取此涂墨注释包含的路径数
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_UINT GetInkCount() = 0;

	//-------------------------------------------
	// Summary:
	//		获取指定的路径顶点数组
	// Parameters:
	//		[in] nIndex - 要获取的路径序号
	//		[out] pPoints - 用于接收路径的顶点数组
	// Return:
	//		返回获取到的路径顶点个数。
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_UINT GetInkByIndex(DK_UINT nIndex, DKP_DPOINT*& pPoints) = 0;

public:
	virtual ~IDKPInkAnnot() {}
};

class IDKPFileAttAnnot : public IDKPUnknown
{
public:
	//-------------------------------------------
	// Summary:
	//		获取此附属文件注释的图标类型
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DKP_FILEATT_ANNOT_ICON_TYPE GetIcon() = 0;

	//-------------------------------------------
	// Summary:
	//		获取此附属文件注释的文件描述字符串
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual const DK_WCHAR* GetF() = 0;

	//-------------------------------------------
	// Summary:
	//		获取此附属文件注释的文件描述unicode字符串
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual const DK_WCHAR* GetUF() = 0;

	//-------------------------------------------
	// Summary:
	//		用于判断此附属文件注释是否使用内嵌文件，若否以下6个函数无效。
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_BOOL IsEmbedded() = 0;

	//-------------------------------------------
	// Summary:
	//		获取嵌入文件子类型
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual const DK_WCHAR* GetEFSubtype() = 0;

	//-------------------------------------------
	// Summary:
	//		获取嵌入文件所占字节数
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_INT GetEFSize() = 0;

	//-------------------------------------------
	// Summary:
	//		获取嵌入文件的创建时间
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual const DK_WCHAR* GetEFCreationDate() = 0;

	//-------------------------------------------
	// Summary:
	//		获取嵌入文件的修改时间
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual const DK_WCHAR* GetEFModDate() = 0;

	//-------------------------------------------
	// Summary:
	//		获取嵌入文件的16字节校验和，对未压缩文件数据进行MD5摘要算法得来。
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual const DK_CHAR* GetEFCheckSum() = 0;

	//-------------------------------------------
	// Summary:
	//		获取内嵌文件数据
	// Parameters:
	//		[out] fileInfo - 用于接收文件信息
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_VOID GetFile(DKPFILEINFO& fileInfo) = 0;

public:
	virtual ~IDKPFileAttAnnot() {}
};
 
class IDKPSoundAnnot : public IDKPUnknown
{
	//-------------------------------------------
	// Summary:
	//		获取此声音注释的图标类型
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DKP_SOUND_ANNOT_ICON_TYPE GetIcon() = 0;
	
	//-------------------------------------------
	// Summary:
	//		获取采样率，单位：采样数/秒。
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_DOUBLE GetSoundR() = 0;

	//-------------------------------------------
	// Summary:
	//		获取声道数
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_UINT GetSoundC() = 0;

	//-------------------------------------------
	// Summary:
	//		获取每个采样数据所占位数
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_UINT GetSoundB() = 0;

	//-------------------------------------------
	// Summary:
	//		获取采样数据的编码模式
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DKP_SOUND_ANNOT_ENCODING_TYPE GetSoundE() = 0;

public:
	virtual ~IDKPSoundAnnot() {}
};
 
class IDKPMovieAnnot : public IDKPUnknown
{
	//-------------------------------------------
	// Summary:
	//		获取此电影注释的标题
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual const DK_WCHAR* GetT() = 0;

	//-------------------------------------------
	// Summary:
	//		获取此电影注释的宽和高，单位像素。
	// Return:
	//		返回含有2个元素的数组，分别是宽和高
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_UINT* GetAspect() = 0;

	//-------------------------------------------
	// Summary:
	//		获取此电影注释的旋转角度
	// Return:
	//		获取逆时针方向旋转角度，必须是90度的倍数
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_UINT GetRotate() = 0;

	//-------------------------------------------
	// Summary:
	//		获取此电影注释的海报图片
	// Parameters:
	//		[out] imageInfo - 用于接收海报图片信息
	// Return:
	//		是否需要显示海报。
	// Remarks:
	//		当返回DK_FALSE时，imageInfo无意义；
	//		当返回DK_TRUE且imageInfo中数据长度不为零时，按照图片数据显示海报；
	//		当返回DK_TRUE且imageInfo中数据长度为零时，需要从电影文件中获取海报。
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_BOOL GetPosterImage(DKPIMAGECONTENT& imageInfo) = 0;

	//-------------------------------------------
	// Summary:
	//		获取此附属文件注释的文件描述字符串
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual const DK_WCHAR* GetF() = 0;

	//-------------------------------------------
	// Summary:
	//		获取此附属文件注释的文件描述unicode字符串
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual const DK_WCHAR* GetUF() = 0;

	//-------------------------------------------
	// Summary:
	//		用于判断此电影注释是否使用内嵌文件，若否以下6个函数无效。
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_BOOL IsEmbedded() = 0;

	//-------------------------------------------
	// Summary:
	//		获取嵌入文件子类型
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual const DK_WCHAR* GetEFSubtype() = 0;

	//-------------------------------------------
	// Summary:
	//		获取嵌入文件所占字节数
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_INT GetEFSize() = 0;

	//-------------------------------------------
	// Summary:
	//		获取嵌入文件的创建时间
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual const DK_WCHAR* GetEFCreationDate() = 0;

	//-------------------------------------------
	// Summary:
	//		获取嵌入文件的修改时间
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual const DK_WCHAR* GetEFModDate() = 0;

	//-------------------------------------------
	// Summary:
	//		获取嵌入文件的16字节校验和，对未压缩文件数据进行MD5摘要算法得来。
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual const DK_CHAR* GetEFCheckSum() = 0;

	//-------------------------------------------
	// Summary:
	//		获取内嵌文件数据
	// Parameters:
	//		[out] fileInfo - 用于接收文件信息
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_VOID GetFile(DKPFILEINFO& fileInfo) = 0;

public:
	virtual ~IDKPMovieAnnot() {}
};

class IDKPScreenAnnot : public IDKPUnknown
{
	//-------------------------------------------
	// Summary:
	//		用于判断此屏幕注释是否使用内嵌文件，若否以下6个函数无效。
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_BOOL IsEmbedded() = 0;

	//-------------------------------------------
	// Summary:
	//		获取嵌入文件子类型
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual const DK_WCHAR* GetEFSubtype() = 0;

	//-------------------------------------------
	// Summary:
	//		获取嵌入文件所占字节数
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_INT GetEFSize() = 0;

	//-------------------------------------------
	// Summary:
	//		获取嵌入文件的创建时间
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual const DK_WCHAR* GetEFCreationDate() = 0;

	//-------------------------------------------
	// Summary:
	//		获取嵌入文件的修改时间
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual const DK_WCHAR* GetEFModDate() = 0;

	//-------------------------------------------
	// Summary:
	//		获取嵌入文件的16字节校验和，对未压缩文件数据进行MD5摘要算法得来。
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual const DK_CHAR* GetEFCheckSum() = 0;

	//-------------------------------------------
	// Summary:
	//		获取内嵌文件数据
	// Parameters:
	//		[out] fileInfo - 用于接收文件信息
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_VOID GetFile(DKPFILEINFO& fileInfo) = 0;

public:
	virtual ~IDKPScreenAnnot() {}
};

class IDKPWatermarkAnnot : public IDKPUnknown
{
public:
	//-------------------------------------------
	// Summary:
	//		获取矩阵
	// Return:
	//		返回含有6个元素的矩阵数据
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_DOUBLE* GetMatrix() = 0;

	//-------------------------------------------
	// Summary:
	//		获取水平方向缩放百分比，[0.0, 1.0]。
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_DOUBLE GetH() = 0;

	//-------------------------------------------
	// Summary:
	//		获取竖直方向缩放百分比，[0.0, 1.0]。
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_DOUBLE GetV() = 0;

public:
	virtual ~IDKPWatermarkAnnot() {}
};

class IDKPPolyAnnot : public IDKPUnknown
{
public:
	//-------------------------------------------
	// Summary:
	//		获取多边形/多段线顶点数组
	// Parameters:
	//		[out] pVertices - 用于接收顶点坐标
	// Return:
	//		返回获取到的顶点个数
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_UINT GetVertices(DKP_DPOINT*& pVertices) = 0;

	//-------------------------------------------
	// Summary:
	//		获取内部填充颜色数组
	// Parameters:
	//		[out] pIC - 用于接收颜色数组各通道的值
	// Return:
	//		返回颜色通道数，[0, 4]。
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_UINT GetIC(DK_DOUBLE*& pIC) = 0;

	//-------------------------------------------
	// Summary:
	//		是否采用云形边框
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_BOOL IsCloudy() = 0;

	//-------------------------------------------
	// Summary:
	//		云形边框强度，[0.0, 2.0]。
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_DOUBLE GetIntensity() = 0;

	//-------------------------------------------
	// Summary:
	//		获取端点类型
	// Parameters:
	//		[in] bStart - 值为DK_TRUE时获取起点，为DK_FALSE时获取终点
	// Return:
	//		返回对应的端点类型
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DKP_LINE_ENDING_TYPE GetLineEnding(DK_BOOL bStart) = 0;

public:
	virtual ~IDKPPolyAnnot() {}
};

class IDKPRectAnnot : public IDKPUnknown
{
public:
	//-------------------------------------------
	// Summary:
	//		获取内部填充颜色数组
	// Parameters:
	//		[out] pIC - 用于接收颜色数组各通道的值
	// Return:
	//		返回颜色通道数，[0, 4]。
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_UINT GetIC(DK_DOUBLE*& pIC) = 0;

	//-------------------------------------------
	// Summary:
	//		是否采用云形边框
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_BOOL IsCloudy() = 0;

	//-------------------------------------------
	// Summary:
	//		云形边框强度，[0.0, 2.0]。
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_DOUBLE GetIntensity() = 0;

	//-------------------------------------------
	// Summary:
	//		获取内外矩形差异
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_VOID GetRD(DKP_DRECT& rcRD) = 0;

public:
	virtual ~IDKPRectAnnot() {}
};

class IDKPLineAnnot : public IDKPUnknown
{
public:
	//-------------------------------------------
	// Summary:
	//		获取线段顶点数组
	// Return:
	//		返回含有两个元素的顶点数组
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DKP_DPOINT* GetL() = 0;

	//-------------------------------------------
	// Summary:
	//		获取内部填充颜色数组
	// Parameters:
	//		[out] pIC - 用于接收颜色数组各通道的值
	// Return:
	//		返回颜色通道数，[0, 4]。
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DK_UINT GetIC(DK_DOUBLE*& pIC) = 0;

	//-------------------------------------------
	// Summary:
	//		获取端点类型
	// Parameters:
	//		[in] bStart - 值为DK_TRUE时获取起点，为DK_FALSE时获取终点
	// Return:
	//		返回对应的端点类型
	// Availability:
	//		从PDFLib 1.0开始支持。
	//-------------------------------------------
	virtual DKP_LINE_ENDING_TYPE GetLineEnding(DK_BOOL bStart) = 0;

public:
	virtual ~IDKPLineAnnot() {}
};

//===========================================================================

#endif
