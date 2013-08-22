///////////////////////////////////////////////////////////////////////
// FCFontInfo.h
// Summary:
//      调用FontConfig读取单个字体文件，获取字体信息
// Contact:liuhj@duokan.com
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#ifndef _LADDISH_FONTMANAGER_FCFONTINFO_H_
#define _LADDISH_FONTMANAGER_FCFONTINFO_H_

#include "IDKFontInfo.h"
#include "fontconfig/fontconfig.h"

class FCFontInfo : public IDKFontInfo
{
public:
    FCFontInfo();
    virtual ~FCFontInfo();
    virtual bool OpenFontFile(std::string &wsTtfPath, TT_FONTFILE_TYPE iFileType);
    virtual void CloseFontFile();
    virtual bool GetSupportedCodePage(std::vector<unsigned short>& lstCodepage);
    virtual std::vector<std::wstring>* GetFullFontName();
    virtual std::vector<std::wstring>* GetFontFamilyName();
    virtual std::vector<std::wstring>* GetFontSubFamilyName();
    virtual DK_FONT_STYLE GetFontStyle();

private:
    bool ParseFontFile ();
    void PushFontNames (const DK_CHAR* pName, std::vector<std::wstring>* pNameList);

private:
    FcFontSet* m_pFontSet;
    FcPattern* m_pPattern;
    
    std::vector<std::wstring>  m_lstFullFontName;
    std::vector<std::wstring>  m_lstFontFamilyName;
    std::vector<std::wstring>  m_lstSubFamilyName;
};

#endif // _LADDISH_FONTMANAGER_FCFONTINFO_H_
