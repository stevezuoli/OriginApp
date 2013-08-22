//===========================================================================
// Summary:
//        DKTAPI.h
// Usage:
//        txt lib基本api定义
// Remarks:
//        Null
// Date:
//        2011-11-14
// Author:
//        Peng Feng(pengf@duokan.com)
//===========================================================================
#ifndef __KERNEL_TXTKIT_TXTLIB_TXTAPI_H__
#define __KERNEL_TXTKIT_TXTLIB_TXTAPI_H__

#include "DKTDef.h"
#include "DKTError.h"
#include "KernelRetCode.h"
#include "KernelEncoding.h"
#include "IDKTBook.h"
#include "IDKTPage.h"
#include "IDKTRender.h"
#include "IDKTTextIterator.h"

class IDkStream;

//-------------------------------------------
//    Summary:
//        初始化TxtLib，使用TxtLib中的任何接口之前必须先调用本函数。
//    Parameters:
//        [in] szResourceDir - TxtLib资源所在路径，该路径下包含TxtLib所需的Resource目录。
//  Return Value:
//      如果成功则返回DK_TRUE，否则返回DK_FALSE。
//    Remarks:
//        szResourceDir设置为空或不合法，则尝试选择程序模块所在目录(当前实现:Windows取DLL所在目录，Linux等平台取可执行程序目录)。
//  Availability:
//        从TxtLib 1.0开始支持。
//-------------------------------------------
TXTAPI DKT_Initialize(const DK_WCHAR* szResourceDir);

//-------------------------------------------
//    Summary:
//        执行清理操作。
//    Parameters:
//      Null
//  Return Value:
//      如果成功则返回DK_TRUE，否则返回DK_FALSE。
//  Remarks:
//      Null
//  Availability:
//        从TxtLib 1.0开始支持。
//-------------------------------------------
TXTAPI DKT_Destroy();

//-------------------------------------------
//  Summary:
//        设置字体名字到字体文件的对应关系，调用该接口后将影响相应字体的显示。
//  Parameters:
//        [in] szFontFaceName     - 字体名字。
//        [in] szFontFile         - 字体文件的路径。
//  Return Value:
//        如果成功则返回DK_TRUE，否则返回DK_FALSE。
//  Remarks:
//      Null
//  Availability:
//        从TxtLib 1.0开始支持。
//-------------------------------------------
TXTAPI DKT_RegisterFont(const DK_WCHAR* szFontFaceName, const DK_WCHAR* szFontFilePath);

//-------------------------------------------
//	Summary:
//		反注册字体名
//	Parameters:
//		[in] szFontFaceName     - 字体名字。
//  Return Value:
//		如果成功则返回 DK_TRUE，否则返回 DK_FALSE。
//  Remarks:
//      如果该 face name 在接口调用之前是默认字体，则该 face name 对应的默认字体设置将失效，
//      需要设置新的默认字体
//  Availability:
//		从TxtLib 2.1.1 开始支持。
//-------------------------------------------
TXTAPI DKT_UnregisterFont(const DK_WCHAR* szFontFaceName);

//-------------------------------------------
// Summary:
//        设置显示使用的默认字体名字，对于不存在的字体，绘制时将使用该默认字体。
// Parameters:
//        [in] szDefaultFontFaceName      - 默认字体名字。
//        [in] charset                    - 字符集，参见 DK_CHARSET_TYPE 枚举值说明。
//  Return Value:
//        如果成功则返回DK_TRUE，否则返回DK_FALSE。
//  Remarks:
//      Null
//  Availability:
//        从TxtLib 1.0开始支持。
//-------------------------------------------
TXTAPI DKT_SetDefaultFont(const DK_WCHAR* szDefaultFontFaceName, DK_CHARSET_TYPE charset);

//-------------------------------------------
//  Summary:
//        打开一个 txt 文档。
//    Parameters:
//        [in] szBookPath     - 文件路径。
//        [in] szWorkingDir   - txt文件工作路径，一般设为系统的临时目录，该目录需要有写权限。
//  Return Value:
//      如果成功返回 txt 文档对象句柄，否则返回 DK_NULL。
//    Remarks:
//        szWorkingDir 设置为空或不合法，则尝试使用系统临时文件目录。
//  Availability:
//        从 TxtLib 1.0 开始支持。
//-------------------------------------------
TXTAPI_(TXTDOCHANDLE) DKT_OpenDocument(const DK_WCHAR* szBookPath,
                                         const DK_WCHAR* szWorkingDir);

//-------------------------------------------
//  Summary:
//        打开一个 txt 文档。
//    Parameters:
//        [in] bookStream     - txt文件流,内部会创建拷贝,外部要自己处理释放。
//        [in] szWorkingDir   - txt文件工作路径，一般设为系统的临时目录，该目录需要有写权限。
//  Return Value:
//      如果成功返回 txt 文档对象句柄，否则返回 DK_NULL。
//    Remarks:
//        szWorkingDir 设置为空或不合法，则尝试使用系统临时文件目录。
//  Availability:
//        从 TxtLib 1.0 开始支持。
//-------------------------------------------
TXTAPI_(TXTDOCHANDLE) DKT_OpenDocumentFromStream(IDkStream* bookStream,
                                         const DK_WCHAR* szWorkingDir);

//-------------------------------------------
//  Summary:
//      按照指定的编码打开一个 txt 文档。
//  Parameters:
//      [in] bookStream     - txt文件流,内部会创建拷贝,外部要自己处理释放。
//      [in] encoding       - 指定编码
//      [in] szWorkingDir   - txt文件工作路径，一般设为系统的临时目录，该目录需要有写权限。
//  Return Value:
//      如果成功返回 txt 文档对象句柄，否则返回 DK_NULL。
//  Remarks:
//      szWorkingDir 设置为空或不合法，则尝试使用系统临时文件目录。
//  Availability:
//      从 TxtLib 2.0 开始支持。
//-------------------------------------------
TXTAPI_(TXTDOCHANDLE) DKT_OpenDocumentWithEncoding(IDkStream* bookStream, 
                                                   EncodingUtil::Encoding encoding, 
                                                   const DK_WCHAR* szWorkingDir);

//-------------------------------------------
//  Summary:
//        关闭一个 txt 文档。
//    Parameters:
//        [in] hFile    - txt 文档对象句柄。
//  Return Value:
//      如果成功则返回 DK_TRUE，否则返回 DK_FALSE。
//  Remarks:
//      Null
//  Availability:
//      从TxtLib 1.0 开始支持。
//-------------------------------------------
TXTAPI DKT_CloseDocument(TXTDOCHANDLE hFile);

//-------------------------------------------
//  Summary:
//      创建文本绘制器句柄。
//  Parameters:
//      Null
//  Return Value:
//      如果成功返回文本绘制器句柄，否则返回 DK_NULL。
//  Remarks:
//  Availability:
//      从 TxtLib 2.0.0 开始支持。
//-------------------------------------------
TXTAPI_(TXTRENDERHANDLE) DKT_CreateRender();

//-------------------------------------------
//  Summary:
//      释放文本绘制器句柄。
//  Parameters:
//      Null
//  Return Value:
//      如果成功则返回 DK_TRUE，否则返回 DK_FALSE。
//  Remarks:
//      Null
//  Availability:
//      从TxtLib 1.0 开始支持。
//-------------------------------------------
TXTAPI DKT_ReleaseRender(TXTRENDERHANDLE hRender);

//-------------------------------------------
//  Summary:
//      在指定区域内render一段文本
//  Parameters:
//      [in] option           - 绘制选项
//      [in] dstBox           - 目标矩形
//      [in] text             - 待绘制的文本
//      [in] textColor        - 文本颜色
//      [in] gbFontFile       - 中文字体路径
//      [in] ansiFontFil      - 英文字体路径
//      [in] fontSize         - 文字大小
//      [in] lineGap          - 行间距
//      [out] pFollowAfter    - 是否没有排完,排完是false,没排完是true
//      [out] ppTextRects     - 每行的矩形框
//      [out] pRectCount      - 行数目
//      [out] ppTextIterator  - 光标表
//  Return Value:
//      如果成功则返回 DKR_OK, 失败返回其它
//  Remarks:
//      Null
//  Availability:
//        从TxtLib 1.0.6 开始支持。
//-------------------------------------------
TXTAPI_(DK_ReturnCode) DKT_RenderText(
        const DK_FLOWRENDEROPTION& option,
        const DK_BOX& dstBox,
        const DK_WCHAR* text, 
        const DK_ARGBCOLOR& textColor,
        const DK_WCHAR* gbFontFile,
        const DK_WCHAR* ansiFontFile,
        DK_DOUBLE fontSize,
        DK_DOUBLE lineGap,
        DK_BOOL* pFollowAfter,
        DK_BOX** ppTextRects,
        DK_UINT* pRectCount,
        IDKTTextIterator** ppTextIterator);

//-------------------------------------------
//  Summary:
//      释放RenderText中生成的矩形框
//  Parameters:
//      [in] textRects        - DKT_RenderText生成的矩形框
//  Return Value:
//      Null
//  Remarks:
//      Null
//  Availability:
//        从TxtLib 1.0.6 开始支持。
//-------------------------------------------
TXTAPI_(DK_VOID) DKT_FreeTextRects(DK_BOX* textRects);

//-------------------------------------------
//  Summary:
//      释放RenderText中生成的光标表
//  Parameters:
//      [in] textIterator     -  DKT_RenderText生成的光标表
//  Return Value:
//      Null
//  Remarks:
//      Null
//  Availability:
//        从TxtLib 1.0.6 开始支持。
//-------------------------------------------
TXTAPI_(DK_VOID) DKT_FreeTextIterator(IDKTTextIterator* textIterator);

//-------------------------------------------
// Summary:
//		设置是否使用图书原有风格，包括行间距、段间距、行首缩进、空格策略
// Parameters:
//		[in] bUseBookStyle      - 是否使用图书原有风格。
//  Return Value:
//		Null
//  Remarks:
//      Null
//  Availability:
//		从TxtLib 1.0.8 开始支持。
//-------------------------------------------
TXTAPI_(DK_VOID) DKT_SetUseBookStyle(DK_BOOL bUseBookStyle);

//-------------------------------------------
// Summary:
//      在指定宽高的box中根据设定参数预排一段文本，返回每个字对应的box及其它一些相关信息
//      可用于后续渲染
// Parameters:
//      [in]  width                  - 排版区宽度
//      [in]  height                 - 排版区高度
//      [in]  text                   - 待排文本,要求0结尾
//      [in]  gbFontFile             - 中文字体，要求全路径
//      [in]  ansiFontFile           - 英文字体，要求全路径
//      [in]  fontSize               - 字号
//      [in]  firstLineIndent        - 首行缩进
//      [in]  lineGap                - 行间距
//      [in]  paraSpacing            - 段间距
//      [out] pFollowAfter           - 是否还有后续没排完的文本
//      [out] ppCharInfoForRender    - 返回的字符信息，包括每个字符的外接矩形，基线等信息
//      [out] charInfoForRenderCount - 返回的字符数（因为hyphen原因，可能比原文本要长
//      [out] ppLineInfoForRender    - 返回的行信息
//      [out] lineInfoForRenderCount - 返回的行数目
//  Return Value:
//      Null
//  Remarks:
//      Null
//  Availability:
//      从TxtLib 1.1.2 开始支持。
//-------------------------------------------
TXTAPI_(DK_ReturnCode) DKT_GetTextRenderInfo(
        DK_DOUBLE width,
        DK_DOUBLE height,
        const DK_WCHAR* text, 
        const DK_WCHAR* gbFontFile,
        const DK_WCHAR* ansiFontFile,
        DK_DOUBLE fontSize,
        DK_DOUBLE firstLineIndent,
        DK_DOUBLE lineGap,
        DK_DOUBLE paraSpacing,
        DK_BOOL* pFollowAfter,
        DKT_CHAR_INFO_FOR_RENDER** ppCharInfoForRender,
        DK_UINT* charInfoForRenderCount,
        DKT_LINE_INFO_FOR_RENDER** ppLineInfoForRender,
        DK_UINT* lineInfoForRenderCount);

//-------------------------------------------
// Summary:
//      根据DKT_GetTextRenderInfo的结果来绘制文本
// Parameters:
//      [in] opton        - 渲染选项
//      [in] offsetX      - 绘制相对于device左上角的x偏移
//      [in] offsetY      - 绘制相对于device左上角的y偏移
//      [in] textColor    - 字体颜色
//      [in] gbFontFile   - 中文字体全路径
//      [in] ansiFontFile - 英文字体全路径
//      [in] fontSize     - 字号
//      [in] renderStart  - 绘制起点
//      [in] renderEnd    - 绘制终点（该点不会被绘制）
//  Return Value:
//      Null
//  Remarks:
//      Null
//  Availability:
//      从TxtLib 1.1.2 开始支持。
//-------------------------------------------
TXTAPI_(DK_ReturnCode) DKT_RenderTextFromRenderInfo(const DK_FLOWRENDEROPTION& option,
        DK_DOUBLE offsetX,
        DK_DOUBLE offsetY,
        const DK_ARGBCOLOR& textColor,
        const DK_WCHAR* gbFontFile,
        const DK_WCHAR* ansiFontFile,
        DK_DOUBLE fontSize,
        const DKT_CHAR_INFO_FOR_RENDER* renderStart, const DKT_CHAR_INFO_FOR_RENDER* renderEnd);

//-------------------------------------------
// Summary:
//      释放DKT_GetTextRenderInfo中申请的内存
// Parameters:
//      [in] charInfoForRender      - 待渲染字符信息
//      [in] lineInfoForRender      - 待渲染行信息
//  Return Value:
//      Null
//  Remarks:
//      Null
//  Availability:
//      从TxtLib 1.1.2 开始支持。
//-------------------------------------------
TXTAPI_(DK_VOID) DKT_FreeTxtRenderInfo(DKT_CHAR_INFO_FOR_RENDER* charInfoForRender, 
        DKT_LINE_INFO_FOR_RENDER* lineInfoForRender);

//-------------------------------------------
//	Summary:
//		设置显示文本时是否做简繁转换。
//	Parameters:
//		[in] bGBToBig5 - 是否做简繁转换。
//  Return Value:
//      Null
//  Remarks:
//      Null
//  Availability:
//		从TxtLib 2.1.0 开始支持。
//-------------------------------------------
TXTAPI_(DK_VOID) DKT_SetGBToBig5(DK_BOOL bGBToBig5);

//-------------------------------------------
//	Summary:
//		获取显示文本时是否做简繁转换。
//	Parameters:
//	    [out] bGBToBig5 - 是否做简繁转换。
//  Return Value:
//      Null
//  Remarks:
//      Null
//  Availability:
//		从TxtLib 2.1.0 开始支持。
//-------------------------------------------
TXTAPI_(DK_VOID) DKT_GetGBToBig5(DK_BOOL& bGBToBig5);

//-------------------------------------------
//  Summary:
//      验证指定字体中，是否存在传入文本所有字符的字形
//  Parameters:
//      [in] szFontFaceName  - 待验证字体的 face name。
//      [in] szText          - 用来验证的文本。
//  Return Value:
//      如果 szFontFaceName 对应字体文件中，包含 szText 中所有字符的字形，
//      则返回 DK_TRUE, 否则返回 DK_FALSE
//  Remarks:
//      Null
//  Availability:
//      从TxtLib 2.3.0 开始支持。
//-------------------------------------------
TXTAPI DKT_VerifyFontFaceByText(const DK_WCHAR* szFontFaceName, const DK_WCHAR* szText);

#endif // __KERNEL_TXTKIT_TXTLIB_TXTAPI_H__
