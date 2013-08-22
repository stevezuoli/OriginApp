///////////////////////////////////////////////////////////////////////
// dk_kdttf.h
// Contact:
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#ifndef _DK_KINDLETTF_H_
#define _DK_KINDLETTF_H_

#include "IDKFontInfo.h"
#include "KernelRenderType.h"
#include "singleton.h"
#include "FontManager/DKFont.h"

class IDkStream;

class DKTTFInfo : public IDKFontInfo
{
public:
    DKTTFInfo();
    virtual ~DKTTFInfo();
    virtual bool  OpenFontFile(std::string &wsTtfPath, TT_FONTFILE_TYPE iFileType);
    virtual void  CloseFontFile();
    virtual bool  GetSupportedCodePage(std::vector<unsigned short>& lstCodepage);
    virtual std::vector<std::wstring>* GetFullFontName();
    virtual std::vector<std::wstring>* GetFontFamilyName();
    virtual std::vector<std::wstring>* GetFontSubFamilyName();
    virtual DK_FONT_STYLE              GetFontStyle();
    
    bool operator<(const DKTTFInfo& right)
    {
        // DKTTFInfo之间比较时，按DKTTFInfo第一个字体名比较
        return m_lstFullFontName.at(0) < right.m_lstFullFontName.at(0);
    }

private:
    void             InitMember();
    
    bool             ParseTtcFile();
    bool             ParseTtfFile(int iFileOffset = 0);
    bool             ParseNameTable(int iNameTableOffset);
    bool             ParseOSTable(int iOSTableOffset);
    
    int              GetEncodingType(int iPlatformID, int iEncodingID, int iLanguageID);
    bool             GetName(std::wstring& wsName, int iEncodingType, int iOffset, int iLength);

private:
    IDkStream *      m_pFile;
    unsigned short   m_uSelection;
    unsigned long    m_ulCodePageRange1;
    unsigned long    m_ulCodePageRange2;
    
    std::vector<std::wstring>  m_lstFullFontName;
    std::vector<std::wstring>  m_lstFontFamilyName;
    std::vector<std::wstring>  m_lstSubFamilyName;
};

#endif // _DK_KINDLETTF_H_


