//===========================================================================
// Summary:
//		Render公共接口。
// Usage:
//		Null
// Remarks:
//		Null
// Date:
//		2012-01-30
// Author:
//		Peng Feng (pengf@duokan.com)
//===========================================================================

#ifndef __KERNEL_COMMONLIBS_RENDER_EXPORT_DKRAPI_H__
#define __KERNEL_COMMONLIBS_RENDER_EXPORT_DKRAPI_H__

#include "DKRDef.h"
#include "KernelType.h"
#include "KernelRetCode.h"

class IDkStream;

//-------------------------------------------
//  Summary:
//      获取图像宽高信息
//  Parameters:
//      [in]  pStream                    - 图像数据流
//      [out] pWidth                     - 图像宽度
//      [out] pHeight                    - 图像高度
//  Return Value:
//      如果成功则返回DKR_OK，否则返回失败。
//  Remarks:
//  Availability:
//      从Render 1.0 开始支持。
//-------------------------------------------
RENDERAPI_(DK_ReturnCode) DKR_AnalyseImageStream(IDkStream *pStream, DK_UINT *pWidth, DK_UINT *pHeight);

//-------------------------------------------
//  Summary:
//      获取图像宽高信息
//  Parameters:
//      [in]  pImageFile                 - 图像数据文件
//      [out] pWidth                     - 图像宽度
//      [out] pHeight                    - 图像高度
//  Return Value:
//      如果成功则返回DKR_OK，否则返回失败。
//  Remarks:
//  Availability:
//      从Render 1.0 开始支持。
//-------------------------------------------
RENDERAPI_(DK_ReturnCode) DKR_AnalyseImageFile(const DK_WCHAR *pImageFile, DK_UINT *pWidth, DK_UINT *pHeight);

//-------------------------------------------
//  Summary:
//      渲染图像流
//  Parameters:
//      [in] option                      - Render option
//      [in] dstBox                      - 目标矩形
//      [in] srcBox                      - 源矩形
//      [in] alpha                       - 透明度,0.0-1.0, 0全透明,1不透明
//      [in] stream                      - 图像数据流
//  Return Value:
//      如果成功则返回DKR_OK，否则返回失败。
//  Remarks:
//  Availability:
//      从Render 1.0 开始支持。
//-------------------------------------------
RENDERAPI_(DK_ReturnCode) DKR_RenderImageStream(
        const DK_FLOWRENDEROPTION& option,
        const DK_BOX& dstBox,
        const DK_BOX& srcBox,
        DK_DOUBLE alpha,
        IDkStream* stream);

//-------------------------------------------
//  Summary:
//      渲染图像文件
//  Parameters:
//      [in] option                      - Render option
//      [in] dstBox                      - 目标矩形
//      [in] srcBox                      - 源矩形
//      [in] alpha                       - 透明度,0.0-1.0, 0全透明,1不透明
//      [in] imageFile                   - 图像数据文件
//  Return Value:
//      如果成功则返回DKR_OK，否则返回失败。
//  Remarks:
//  Availability:
//      从Render 1.0 开始支持。
//-------------------------------------------
RENDERAPI_(DK_ReturnCode) DKR_RenderImageFile(
        const DK_FLOWRENDEROPTION& option,
        const DK_BOX& dstBox,
        const DK_BOX& srcBox,
        DK_DOUBLE alpha,
        const DK_WCHAR* imageFile);

#endif
