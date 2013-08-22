///////////////////////////////////////////////////////////////////////
// FCFontInfo.cpp
// Summary:
//      调用FontConfig读取单个字体文件，获取字体信息
// Contact:liuhj@duokan.com
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#include "FCFontInfo.h"
#include <iostream> 
#include "dkBaseType.h"
#include "dkWinError.h"
#include "KernelEncoding.h"
#include "Framework/BackgroundLoader.h"
#include "interface.h"

using std::wcout;
using std::vector;
using std::string;
using std::wstring;

//#define DEBUG_FCFONTINFO

const FcChar8* FC_PATTERN_FULL_NAME = (const FcChar8*)("%{fullname}");
const FcChar8* FC_PATTERN_FAMILY_NAME = (const FcChar8*)("%{family}");

FCFontInfo::FCFontInfo()
    : m_pFontSet(0),
      m_pPattern(0)
{
    DebugPrintf(DLC_LIUHJ, "FCFontInfo Constructed");
    m_lstFullFontName.clear();
    m_lstFontFamilyName.clear();
    m_lstSubFamilyName.clear();
}

FCFontInfo::~FCFontInfo()
{
    CloseFontFile();
}

bool FCFontInfo::OpenFontFile(std::string &wsTtfPath, TT_FONTFILE_TYPE iFileType)
{
    DebugPrintf(DLC_LIUHJ, "FCFontInfo::OpenFontFile Entrance filepath:%s", wsTtfPath.c_str());

    CBackgroundLoader::WaitForCheckingFontsCache();

    CloseFontFile();

    m_pFontSet = FcFontSetCreate ();
    if (!m_pFontSet)
    {
        return false;
    }

    // scan font file
    DebugPrintf(DLC_LIUHJ, "FCFontInfo::OpenFontFile scan font file start");
    FcFileScan (m_pFontSet, NULL, NULL, NULL, (FcChar8*)(wsTtfPath.c_str()), FcTrue);
    if (m_pFontSet->nfont <= 0 || !m_pFontSet->fonts[0])
    {
        DebugPrintf (DLC_LIUHJ, "FCFontInfo::OpenFontFile Failed ! filepath:%s", wsTtfPath.c_str ());
        return false;
    }

    // scan font file succeeded, create font pattern
    m_pPattern = m_pFontSet->fonts[0];

    DebugPrintf (DLC_LIUHJ, "FCFontInfo::OpenFontFile %s", wsTtfPath.c_str ());

#ifdef DEBUG_FCFONTINFO
    FcPatternPrint (m_pPattern);
#endif

    return ParseFontFile ();
}

void FCFontInfo::CloseFontFile()
{
    if (m_pFontSet)
    {
        FcFontSetDestroy (m_pFontSet);
    }
    if (m_pPattern)
    {
        FcPatternPrint (m_pPattern);
    }
    m_lstFullFontName.clear();
    m_lstFontFamilyName.clear();
    m_lstSubFamilyName.clear();
}

bool FCFontInfo::ParseFontFile ()
{
    if (!m_pPattern)
    {
        return false;
    }

    FcChar8* pFullName = FcPatternFormat (m_pPattern, FC_PATTERN_FULL_NAME);
    if (pFullName)
    {
        DebugPrintf (DLC_LIUHJ, "FCFontInfo::ParseFontFile pFullName %s", pFullName);
        PushFontNames ((DK_CHAR*)pFullName, &m_lstFullFontName); // 将多个字体名拆开压入容器
    }
    SafeFreePointer (pFullName);

    FcChar8* pFamily = FcPatternFormat (m_pPattern, FC_PATTERN_FAMILY_NAME);
    if (pFamily)
    {
        DebugPrintf (DLC_LIUHJ, "FCFontInfo::ParseFontFile pFamily %s", pFamily);
        PushFontNames ((DK_CHAR*)pFamily, &m_lstFontFamilyName);
    }
    SafeFreePointer (pFamily);

    return true;
}

void FCFontInfo::PushFontNames (const DK_CHAR* pName, std::vector<std::wstring>* pNameList)
{
    if (pName && pNameList)
    {
        const DK_CHAR* pEnd = pName + strlen (pName);
        const DK_CHAR* pCur = pName;
        const DK_CHAR* pFound(0);
        DK_CHAR* pInBuf(0);
        while (pCur < pEnd)
        {
            pFound = strchr (pCur, ',');

            DK_SIZE_T inLen (pEnd - pCur);
            if (pFound && pFound != pCur)
            {
                inLen = pFound - pCur;
            }

            pInBuf = DK_MALLOCZ_OBJ_N (DK_CHAR, inLen + 1);
            memcpy (pInBuf, pCur, inLen);
            pInBuf [inLen] = '\0';
            DebugPrintf(DLC_LIUHJ, "PushFontNames %s", pInBuf);

            DK_WCHAR* pWFullName = EncodingUtil::CharToWChar (pInBuf);
            if (pWFullName)
            {
                pNameList->push_back(wstring((DK_WCHAR*)pWFullName));
            }
            SafeFreePointer (pWFullName);
            SafeFreePointer (pInBuf);

            if (pFound)
            {
                pCur = ++pFound;
            }
            else
            {
                break;
            }
        }
    }
}

bool FCFontInfo::GetSupportedCodePage(std::vector<unsigned short>& lstCodepage)
{
    // not implement
    return false;
}

std::vector<wstring>* FCFontInfo::GetFullFontName()
{
    return &m_lstFullFontName;
}

std::vector<wstring>* FCFontInfo::GetFontFamilyName()
{
    return &m_lstFontFamilyName;
}

std::vector<wstring>* FCFontInfo::GetFontSubFamilyName()
{
    return &m_lstSubFamilyName;
}

DK_FONT_STYLE FCFontInfo::GetFontStyle()
{
    // not implement
    return DK_FontStyleRegular;
}
