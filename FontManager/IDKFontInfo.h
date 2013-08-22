///////////////////////////////////////////////////////////////////////
// IDKFontInfo.h
// Summary:
//      读取单个字体文件，获取字体信息的接口类
// Contact:liuhj@duokan.com
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#ifndef _LADDISH_FONTMANAGER_IDKFONTINFO_H_
#define _LADDISH_FONTMANAGER_IDKFONTINFO_H_

#include <vector>
#include <string>
#include "KernelRenderType.h"

enum TT_FONTFILE_TYPE
{
    TT_FONTFILE_UNKNOWN = 0,
    TT_FONTFILE_TTF     = 1,
    TT_FONTFILE_TTC     = 2
};

class IDKFontInfo
{
public:
    virtual ~IDKFontInfo(){};
    virtual bool OpenFontFile(std::string &wsTtfPath, TT_FONTFILE_TYPE iFileType)=0;
    virtual void CloseFontFile()=0;
    virtual bool GetSupportedCodePage(std::vector<unsigned short>& lstCodepage)=0;
    virtual std::vector<std::wstring>* GetFullFontName()=0;
    virtual std::vector<std::wstring>* GetFontFamilyName()=0;
    virtual std::vector<std::wstring>* GetFontSubFamilyName()=0;
    virtual DK_FONT_STYLE GetFontStyle()=0;
};

#endif // _LADDISH_FONTMANAGER_IDKFONTINFO_H_
