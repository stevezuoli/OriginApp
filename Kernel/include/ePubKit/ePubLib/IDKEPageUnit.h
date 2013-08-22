//===========================================================================
//  Summary:
//      ePub页面对象
//  Usage:
//      通过页面导出接口获得页面对象，用于页面对象深度处理
//  Remarks:
//      Null
//  Date:
//      2013-07-22
// Author:
//      Zhang Lei (zhanglei@duokan.com)
//===========================================================================

#ifndef __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEPAGEUNIT_H__
#define __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEPAGEUNIT_H__

//===========================================================================

#include "DKEDef.h"

//===========================================================================

class IDKEPageUnit
{
public:
    virtual ~IDKEPageUnit() {}

public:
    //-------------------------------------------
    //  Summary:
    //      获取对象类型。
    //  Parameters:
    //      Null
    //  Return Value:
    //      对象类型
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.6.0开始支持。
    //-------------------------------------------
    virtual DKE_PAGEUNIT_TYPE GetType() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取对象在当前页面的外接矩形。
    //  Parameters:
    //      Null
    //  Return Value:
    //      外接矩形
    //  Remarks:
    //      以页面左上角为原点。
    //  Availability:
    //      从ePubLib 2.6.0开始支持。
    //-------------------------------------------
    virtual DK_BOX GetBoundary() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取对象的流式位置。
    //  Parameters:
    //      Null
    //  Return Value:
    //      流式位置
    //  Remarks:
    //      对于原子元素为其三级索引，对于非原子元素等于其起始流式位置
    //  Availability:
    //      从ePubLib 2.6.0开始支持。
    //-------------------------------------------
    virtual DK_FLOWPOSITION GetFlowPosition() const = 0;
};

//===========================================================================

class IDKEPageBlock : public IDKEPageUnit
{
public:
    virtual ~IDKEPageBlock() {}

public:
    //-------------------------------------------
    //  Summary:
    //      获取对象的起始位置。
    //  Parameters:
    //      Null
    //  Return Value:
    //      起始位置
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.6.0开始支持。
    //-------------------------------------------
    virtual DK_FLOWPOSITION GetBeginPosition() const = 0;

    //-------------------------------------------
    //  Summary:
    //      获取对象的结束位置。
    //  Parameters:
    //      Null
    //  Return Value:
    //      结束位置
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.6.0开始支持。
    //-------------------------------------------
    virtual DK_FLOWPOSITION GetEndPosition() const = 0;

    //-------------------------------------------
    //  Summary:
    //      开始枚举。
    //  Parameters:
    //      Null
    //  Return Value:
    //      成功与否
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.6.0开始支持。
    //-------------------------------------------
    virtual DK_BOOL BeginEnum() = 0;

    //-------------------------------------------
    //  Summary:
    //      移动到下一个枚举位置。
    //  Parameters:
    //      Null
    //  Return Value:
    //      成功与否
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.6.0开始支持。
    //-------------------------------------------
    virtual DK_BOOL MoveNext() = 0;

    //-------------------------------------------
    //  Summary:
    //      获取当前枚举对象。
    //  Parameters:
    //      Null
    //  Return Value:
    //      当前枚举对象
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.6.0开始支持。
    //-------------------------------------------
    virtual IDKEPageUnit* GetCurUnit() = 0;

    //-------------------------------------------
    //  Summary:
    //      结束枚举。
    //  Parameters:
    //      Null
    //  Return Value:
    //      成功与否
    //  Remarks:
    //      Null
    //  Availability:
    //      从ePubLib 2.6.0开始支持。
    //-------------------------------------------
    virtual DK_BOOL EndEnum() = 0;
};

//===========================================================================

class IDKEPageText : public IDKEPageUnit
{
public:
    virtual ~IDKEPageText() {}

public:
    //-------------------------------------------
    //	Summary:
    //		获取当前位置的Text信息。
    //  Parameters:
    //		[out] pTextInfo            - 当前位置Text信息。
    //	Return Value:
    //	    Null
    //	Remarks:
    //		调用FreeTextInfo释放pTextInfo hold的资源
    //  Availability:
    //		从ePubLib 2.6.0开始支持。
    //-------------------------------------------
    virtual DK_VOID GetTextInfo(DKEPageTextInfo* pTextInfo) const = 0;

    //-------------------------------------------
    //	Summary:
    //		释放Text信息。
    //  Parameters:
    //		[in] pTextInfo             - 待释放的信息。
    //	Return Value:
    //	    Null
    //	Remarks:
    //		Null
    //  Availability:
    //		从ePubLib 2.6.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeTextInfo(DKEPageTextInfo* pTextInfo) = 0;
};

//===========================================================================

class IDKEPageImage : public IDKEPageUnit
{
public:
    virtual ~IDKEPageImage() {}

public:
    //-------------------------------------------
    //	Summary:
    //		获取当前位置的图片信息。
    //  Parameters:
    //		[out] pImageInfo            - 当前位置图片信息。
    //	Return Value:
    //	    Null
    //	Remarks:
    //		调用FreeImageInfo释放pImageInfo hold的资源
    //  Availability:
    //		从ePubLib 2.6.0开始支持。
    //-------------------------------------------
    virtual DK_VOID GetImageInfo(DKE_HITTEST_OBJECTINFO* pImageInfo) const = 0;

    //-------------------------------------------
    //	Summary:
    //		释放图片信息。
    //  Parameters:
    //		[in] pImageInfo             - 待释放的信息。
    //	Return Value:
    //	    Null
    //	Remarks:
    //		Null
    //  Availability:
    //		从ePubLib 2.6.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreeImageInfo(DKE_HITTEST_OBJECTINFO* pImageInfo) = 0;
};

//===========================================================================

class IDKEPagePath : public IDKEPageUnit
{
public:
    virtual ~IDKEPagePath() {}

public:
    //-------------------------------------------
    //	Summary:
    //		获取当前位置的Path信息。
    //  Parameters:
    //		[out] pPathInfo            - 当前位置Path信息。
    //	Return Value:
    //	    Null
    //	Remarks:
    //		调用FreePathInfo释放pPathInfo hold的资源
    //  Availability:
    //		从ePubLib 2.6.0开始支持。
    //-------------------------------------------
    virtual DK_VOID GetPathInfo(DKEPagePathInfo* pPathInfo) const = 0;

    //-------------------------------------------
    //	Summary:
    //		释放Path信息。
    //  Parameters:
    //		[in] pPathInfo             - 待释放的信息。
    //	Return Value:
    //	    Null
    //	Remarks:
    //		Null
    //  Availability:
    //		从ePubLib 2.6.0开始支持。
    //-------------------------------------------
    virtual DK_VOID FreePathInfo(DKEPagePathInfo* pPathInfo) = 0;
};

//===========================================================================

#endif // __KERNEL_EPUBKIT_EPUBLIB_EXPORT_IDKEPAGEUNIT_H__

