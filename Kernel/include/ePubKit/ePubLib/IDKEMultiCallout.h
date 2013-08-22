//===========================================================================
//  Summary:
//      ePub图书多点交互图对象
//  Usage:
//      通过页面拾取逻辑获取当前多点交互图对象，然后通过该多点交互图对象操作多点交互图
//  Remarks:
//      Null
//  Date:
//      2013-08-12
// Author:
//      Zhang Jiafang (zhangjf@duokan.com)
//===========================================================================

#ifndef __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEMULTICALLOUT_H__
#define __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEMULTICALLOUT_H__

#include "DKEDef.h"

class IDKEImageIterator;

class IDKEMultiCallout
{
public:
    virtual ~IDKEMultiCallout() {}

public:
    //-------------------------------------------
    //  Summary:
    //      获取多点交互图块的起始位置
    //  Parameters:
    //      Null
    //  Return Value:
    //      DK_FLOWPOSITION 起始位置
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.6.1开始支持。
    //-------------------------------------------
    virtual DK_FLOWPOSITION GetBeginPosition() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取多点交互图块的结束位置
    //  Parameters:
    //      Null
    //  Return Value:
    //      DK_FLOWPOSITION 结束位置
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.6.1开始支持。
    //-------------------------------------------
    virtual DK_FLOWPOSITION GetEndPosition() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取多点交互图基础图片信息
    //  Parameters:
    //      [out]pImgInfo       - 图片信息
    //  Return Value:
    //      如果成功返回DKR_OK，否则返回失败
    //  Remarks:
    //      调用FreeBaseImageInfo释放图片信息
    //  Availability:
    //      从ePubLib 2.6.1开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode GetBaseImageInfo(DKE_HITTEST_OBJECTINFO* pImgInfo) const = 0;

    //-------------------------------------------
    //  Summary:
    //      释放多点交互图基础图片信息
    //  Parameters:
    //      [in]pImgInfo       - 图片信息
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.6.1开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeBaseImageInfo(DKE_HITTEST_OBJECTINFO* pImgInfo) = 0; 

    //-------------------------------------------
    //  Summary:
    //      获取多点交互图基础图片的枚举器
    //  Parameters:
    //      Null
    //  Return Value:
    //      IDKEImageIterator 枚举器
    //  Remarks:
    //      调用FreeBaseImageIterator释放枚举器
    //  Availability:
    //      从ePubLib 2.6.1开始支持。
    //-------------------------------------------
    virtual IDKEImageIterator* GetBaseImageIterator() const = 0;

    //-------------------------------------------
    //  Summary:
    //      释放多点交互图基础图片的枚举器
    //  Parameters:
    //      [in]pImageIterator          - 基础图片枚举器
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.6.1开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeBaseImageIterator(IDKEImageIterator* pImageIterator) = 0;

    //-------------------------------------------
    //  Summary:
    //      获取多点交互图的可交互点数目
    //  Parameters:
    //      Null
    //  Return Value:
    //      可交互点数目
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.6.1开始支持。
    //-------------------------------------------
    virtual DK_LONG GetCalloutCount() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取放置多点交互图导航信息的区域
    //  Parameters:
    //      Null
    //  Return Value:
    //      导航信息的区域
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.6.1开始支持。
    //-------------------------------------------
    virtual DK_BOX GetNavigationBox() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取指定位置的可交互点的基本信息
    //  Parameters:
    //      [in]calloutIndex        - 可交互点的位置
    //      [out]pCalloutInfo       - 可交互点的基本信息
    //  Return Value:
    //      如果成功返回DKR_OK，否则返回失败
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.6.1开始支持。
    //-------------------------------------------
    virtual DK_ReturnCode GetCalloutBasicInfo(DK_LONG calloutIndex, DKE_CALLOUT_INFO* pCalloutInfo) const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取指定位置的可交互点的标题
    //  Parameters:
    //      [in]calloutIndex        - 可交互点的位置
    //      [out]ppTitle            - 可交互点的标题
    //  Return Value:
    //      Null
    //  Remarks:
    //      调用FreeCalloutFlexPage释放ppTitle
    //  Availability:
    //      从ePubLib 2.6.1开始支持。
    //-------------------------------------------
    virtual DK_VOID GetCalloutTitle(DK_LONG calloutIndex, IDKEFlexPage** ppTitle) const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取指定位置的可交互点的说明
    //  Parameters:
    //      [in]calloutIndex        - 可交互点的位置
    //      [out]ppCaption          - 可交互点的说明
    //  Return Value:
    //      Null
    //  Remarks:
    //      调用FreeCalloutFlexPage释放ppCaption
    //  Availability:
    //      从ePubLib 2.6.1开始支持。
    //-------------------------------------------
    virtual DK_VOID GetCalloutCaption(DK_LONG calloutIndex, IDKEFlexPage** ppCaption) const = 0;

    //-------------------------------------------
    //  Summary:
    //      释放标题和说明对应的可伸缩页
    //  Parameters:
    //      [in]pFlexPage           - 可交互点的位置
    //  Return Value:
    //      Null
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.6.1开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeCalloutFlexPage(IDKEFlexPage* pFlexPage) = 0;
};


#endif // __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEMULTICALLOUT_H__
