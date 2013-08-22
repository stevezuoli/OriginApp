//===========================================================================
// Summary:
//		一个漫画帧类对象，进行漫画帧的渲染，获取帧相关信息等
// Usage:
//		Null
// Remarks:
//		Null
// Date:
//		2012-12-24
// Author:
//		Zhang JiaFang(zhangjf@duokan.com)
//===========================================================================

#ifndef __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKECOMICSFRAME_H__
#define __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKECOMICSFRAME_H__


//===========================================================================

#include "DKEDef.h"

//===========================================================================

class IDKEComicsFrame
{
public:
    virtual ~IDKEComicsFrame() {}

public:
    //-------------------------------------------
    //  Summary:
    //      渲染漫画帧
    //  Parameters:
    //      [in] option             - 绘制选项。
    //      [out] pResult           - 绘制结果。
    //  Return Value:
    //      如果成功返回DKR_OK。
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.3.1开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode RenderFrame(const DK_FLOWRENDEROPTION& option, DK_FLOWRENDERRESULT *pResult) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取漫画帧的外接矩形，以帧所在图片的左上角为参照
    //  Parameters:
    //      Null
    //  Return Value:
    //      帧的外接矩形
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.3.1开始支持。
    //-------------------------------------------
    virtual DK_BOX GetFrameBoundaryOnImage() = 0;

    //-------------------------------------------
    //  Summary:
    //      获取当前帧所在的图片的矩形
    //  Parameters:
    //      Null
    //  Return Value:
    //      帧的外接矩形
    //  Remarks:
    //      Null
    //  Availability:

    //      从ePubLib 2.3.1开始支持。
    //-------------------------------------------
    virtual DK_BOX GetComicsImageBoundary() = 0;

    //-------------------------------------------
    //  Summary:
    //      获取当前漫画帧在整个漫画帧阅读序列中的位置（从零开始）
    //  Parameters:
    //      Null
    //  Return Value:
    //      帧索引
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.3.1开始支持。
    //-------------------------------------------
    virtual DK_LONG GetFrameIndexInThread() = 0;

    //-------------------------------------------
    //  Summary:
    //      获取当前漫画帧所指向的章节序号（从零开始）
    //  Parameters:
    //      Null
    //  Return Value:
    //      章节序号
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.3.1开始支持。
    //-------------------------------------------
    virtual DK_LONG GetChapterIndexOfFrame() = 0;

    //-------------------------------------------
    //  Summary:
    //      获取当前帧所在漫画图片的数据
    //  Parameters:
    //      [out] pImageData        - 图片文件数据
    //  Return Value:
    //      成功返回DKR_OK，否则返回失败
    //  Remarks:
    //      调用FreeComicsImageData释放数据
    //  Availability:
    //      从ePubLib 2.3.1开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode GetComicsImageData(DKFILEINFO* pImageData) = 0;

    //-------------------------------------------
    //  Summary:
    //      释放当前帧所在漫画图片的数据
    //  Parameters:
    //      [in] pImageData        - 图片文件数据
    //  Return Value:
    //      Null
    //  Remarks:
    //      调用GetComicsImageData后调用该函数
    //  Availability:
    //      从ePubLib 2.3.1开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeComicsImageData(DKFILEINFO* pImageData) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取当前帧所在漫画图片在ePub包内的全路径
    //  Parameters:
    //      Null
    //  Return Value:
    //      包内路径
    //  Remarks:
    //      调用FreeComicsImagePath释放返回值内存
    //  Availability:
    //      从ePubLib 2.3.1开始支持。
    //-------------------------------------------
    virtual DK_WCHAR* GetComicsImagePath() = 0;

    //-------------------------------------------
    //  Summary:
    //      释放当前帧所在漫画图片在ePub包内的全路径
    //  Parameters:
    //      [in]pPath       - 包内路径
    //  Return Value:
    //      Null
    //  Remarks:
    //      调用GetComicsImagePath后调用
    //  Availability:
    //      从ePubLib 2.3.1开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeComicsImagePath(DK_WCHAR* pPath) = 0;
};


//===========================================================================
#endif

