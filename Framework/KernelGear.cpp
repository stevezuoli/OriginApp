//===========================================================================
// Summary:
//      KernelGear.cpp
//      负责调用 Kernel 公共接口的初始化、销毁、字体注册、字体设置等
// Version:
//      0.1 draft - creating.
//
// Usage:
//      Null
// Remarks:
//      Null
// Date:
//      2011-12-28
// Author:
//      liuhj@duokan.com
//===========================================================================

#include "Framework/KernelGear.h"
#include <string>
#include <vector>
#include <stdio.h>
#include "DKTAPI.h" 
#include "DKEAPI.h" 
#include "DKDParserKernelAPI.h"
#include "DKMAPI.h"
#include "FontManager/DKFont.h"
#include "interface.h"
#include "KernelMacro.h"
#include "KernelEncoding.h"
#include "Utility/EncodeUtil.h"
#include "Utility/PathManager.h"

using std::string;
using std::wstring;
using std::vector;
using namespace dk::utility;

//---------------------------------------------------------------------------

void SetEpubDefaultFont();
void SetMobiDefaultFont();
void SetTxtDefaultFont ();

// 注册相似字体
void RegisterSimilarFonts ();
void UnRegisterSimilarFonts ();

//---------------------------------------------------------------------------

void InitializeKernel ()
{
    std::wstring wordSegPath = EncodeUtil::ToWString(PathManager::GetResourcePath());
    // EPUB
    if (!DKE_Initialize(wordSegPath.c_str()))
    {
        DebugPrintf (DLC_KERNELGEAR, "ERR - DKE_Initialize Failed!!!!!!!!!!!!!!!");
    }
    DKE_SetUseInteractivePre(true);

    // TXT
    if (!DKT_Initialize(wordSegPath.c_str()))
    {
        DebugPrintf (DLC_KERNELGEAR, "ERR - DKT_Initialize Failed!!!!!!!!!!!!!!!");
    }

    // PDF
    if (!DK_SUCCEEDED(DKP_Initialize(wordSegPath.c_str())))
    {
        DebugPrintf (DLC_KERNELGEAR, "ERR - DKP_Initialize Failed!!!!!!!!!!!!!!!");
    }

    // MOBI
    if (!DKM_Initialize(wordSegPath.c_str()))
    {
        DebugPrintf (DLC_KERNELGEAR, "ERR - DKM_Initialize Failed!!!!!!!!!!!!!!!");
    }
}

void DestroyKernel ()
{
    // MOBI
    DKM_Destroy();
// 
//     // PDF
    DKP_Destroy();

    // TXT
    DKT_Destroy();

//     // EPUB
    DKE_Destroy();
}

void RegisterKernelFonts ()
{
    DebugPrintf (DLC_KERNELGEAR, "RegisterKernelFonts start!!!");
    vector<DKFont*> lstFont;
    if (!g_pFontManager->GetAllFont(lstFont))
    {
        DebugPrintf (DLC_KERNELGEAR, "RegisterKernelFonts g_pFontManager->GetAllFont failed!!!!!!!");
        return;
    }

    // 注册所有字体，一个字体路径可能对应多个字体名
    vector<DKFont*>::iterator viFont;
    for (viFont = lstFont.begin(); viFont < lstFont.end(); ++viFont)
    {
        DK_WCHAR* pFontPath = EncodingUtil::CharToWChar((*viFont)->GetFontPath());
        MAKE_SCOPE_EXIT(pFontPathScope, pFontPath, &DK_FREE);
        if (pFontPath)
        {
            vector<wstring>::iterator viFullName;
            vector<wstring>* lstFullName = (*viFont)->GetFullFontName();
            if (!lstFullName || lstFullName->size() <= 0)
            {
                continue;
            }
            
            // 依次注册该字体文件包含的所有字体名
            for (viFullName = lstFullName->begin(); viFullName < lstFullName->end(); ++viFullName)
            {
                if ((*viFullName).length() <= 0)
                {
                    continue;
                }

                //DebugPrintf(DLC_DIAGNOSTIC, "RegisterKernelFonts %s:%s",
                //        EncodeUtil::ToString(viFullName->c_str()).c_str(),
                //        (*viFont)->GetFontPath());

                 if (!DKE_RegisterFont(viFullName->c_str(), pFontPath))
                 {
                     DebugPrintf(DLC_DIAGNOSTIC, "RegisterKernelFonts DKE_RegisterFont error!!!");
                 }
                 if (!DKM_RegisterFont((*viFullName).c_str(), pFontPath))
                 {
                     DebugPrintf(DLC_DIAGNOSTIC, "RegisterKernelFonts DKM_RegisterFont error!!!");
                 }
                if (!DKT_RegisterFont((*viFullName).c_str(), pFontPath))
                {
                    DebugPrintf(DLC_DIAGNOSTIC, "RegisterKernelFonts DKT_RegisterFont error!!!");
                }
                 if (DK_FAILED(DKP_RegisterFontFaceName ((*viFullName).c_str(), pFontPath)))
                 {
                     DebugPrintf(DLC_DIAGNOSTIC, "RegisterKernelFonts DKP_RegisterFont error!!!");
                 }
            }
            
        }
        else
        {
            DebugPrintf(DLC_KERNELGEAR, "RegisterKernelFonts Convert pFontPath error!!!");
        }
    }

    // 注册相似字体
    RegisterSimilarFonts ();

    DebugPrintf (DLC_KERNELGEAR, "RegisterKernelFonts finish!!!");
}

void UnRegisterKernelFonts ()
{
    DebugPrintf (DLC_KERNELGEAR, "UnRegisterKernelFonts start!!!");
    vector<DKFont*> lstFont;
    if (!g_pFontManager->GetAllFont(lstFont))
    {
        DebugPrintf (DLC_KERNELGEAR, "UnRegisterKernelFonts g_pFontManager->GetAllFont failed!!!!!!!");
        return;
    }

    // 注册所有字体，一个字体路径可能对应多个字体名
    vector<DKFont*>::iterator viFont;
    for (viFont = lstFont.begin(); viFont < lstFont.end(); ++viFont)
    {
        DK_WCHAR* pFontPath = EncodingUtil::CharToWChar((*viFont)->GetFontPath());
        MAKE_SCOPE_EXIT(pFontPathScope, pFontPath, &DK_FREE);
        if (pFontPath)
        {
            vector<wstring>::iterator viFullName;
            vector<wstring>* lstFullName = (*viFont)->GetFullFontName();
            if (!lstFullName || lstFullName->size() <= 0)
            {
                continue;
            }

            // 依次注册该字体文件包含的所有字体名
            for (viFullName = lstFullName->begin(); viFullName < lstFullName->end(); ++viFullName)
            {
                if ((*viFullName).length() <= 0)
                {
                    continue;
                }

                 DKE_UnregisterFont(viFullName->c_str());
//              DKM_RegisterFont((*viFullName).c_str());
                DKT_UnregisterFont((*viFullName).c_str());
//              DKP_RegisterFontFaceName ((*viFullName).c_str());
            }

        }
        else
        {
            DebugPrintf(DLC_KERNELGEAR, "UnRegisterKernelFonts Convert pFontPath error!!!");
        }
    }

    // 注册相似字体
    UnRegisterSimilarFonts ();

    DebugPrintf (DLC_KERNELGEAR, "UnRegisterKernelFonts finish!!!");
}

void SetKernelDefaultFonts ()
{
     SetEpubDefaultFont();
     SetMobiDefaultFont();
    SetTxtDefaultFont();
}

//---------------------------------------------------------------------------
// ePub 字体相关

void SetEpubDefaultFont()
{
    DebugPrintf(DLC_KERNELGEAR, "SetEpubDefaultFont start!!!");
    vector<DKFont*> lstFont;
    if (g_pFontManager->GetAllFont(lstFont))
    {        
        DK_CHARSET_TYPE lstCharset[] = {DK_CHARSET_ANSI, DK_CHARSET_GB, DK_CHARSET_BIG5, DK_CHARSET_SHITJIS, DK_CHARSET_JOHAB};
        int iSize = sizeof(lstCharset)/sizeof(DK_CHARSET_TYPE);
        for (int i = 0; i < iSize; ++i)
        {
            DK_CHARSET_TYPE &eCT = lstCharset[i];
            wstring strFaceName (L"");
            if (!g_pFontManager->GetDefaultFontFaceNameForCharSet(eCT, &strFaceName))
            {
                DebugPrintf(DLC_KERNELGEAR, "SetEpubDefaultFont GetDefaultFontFaceNameForCharSet No special font setted for charset %d", eCT);
                g_pFontManager->GetSystemDefaultFontFaceName(&strFaceName);
            }
            
            if (strFaceName.length () != 0)
            {
                if (!DKE_SetDefaultFont(strFaceName.c_str(), eCT))
                {
                    DebugPrintf(DLC_KERNELGEAR, "SetEpubDefaultFont SetDefaultFont error %d", eCT);
                }
            }
            else
            {
                DebugPrintf(DLC_KERNELGEAR, "SetEpubDefaultFont GetSystemDefaultFont error %d", eCT);
            }
        }
    }
    DebugPrintf(DLC_KERNELGEAR, "SetEpubDefaultFont finish!!!");
}

//---------------------------------------------------------------------------
// Mobi 字体相关

void SetMobiDefaultFont()
{
    DebugPrintf(DLC_KERNELGEAR, "SetMobiDefaultFont start!!!");
    vector<DKFont*> lstFont;
    if (g_pFontManager->GetAllFont(lstFont))
    {        
        DK_CHARSET_TYPE lstCharset[] = {DK_CHARSET_ANSI, DK_CHARSET_GB, DK_CHARSET_BIG5, DK_CHARSET_SHITJIS, DK_CHARSET_JOHAB};
        int iSize = sizeof(lstCharset)/sizeof(DK_CHARSET_TYPE);
        for (int i = 0; i < iSize; ++i)
        {
            DK_CHARSET_TYPE &eCT = lstCharset[i];
            wstring strFaceName (L"");
            if (!g_pFontManager->GetDefaultFontFaceNameForCharSet(eCT, &strFaceName))
            {
                DebugPrintf(DLC_KERNELGEAR, "SetMobiDefaultFont GetDefaultFontFaceNameForCharSet No special font setted for charset %d", eCT);
                g_pFontManager->GetSystemDefaultFontFaceName(&strFaceName);
            }
            
            if (strFaceName.length () != 0)
            {
                if (!DKM_SetDefaultFont(strFaceName.c_str(), eCT))
                {
                    DebugPrintf(DLC_KERNELGEAR, "SetMobiDefaultFont SetDefaultFont error %d", eCT);
                }
            }
            else
            {
                DebugPrintf(DLC_KERNELGEAR, "SetMobiDefaultFont GetSystemDefaultFont error %d", eCT);
            }
        }
    }
    DebugPrintf(DLC_KERNELGEAR, "SetMobiDefaultFont finish!!!");
}

//---------------------------------------------------------------------------
// txt字体相关

void GetAvailableFontInfo (DK_CHARSET_TYPE charset, wstring* pStrFontName, wstring* pStrFontPath)
{
    if (!pStrFontName && !pStrFontPath)
    {
        return;
    }

    // 得到默认字体的 facename
    wstring strFontName (L"");
    if (!g_pFontManager->GetDefaultFontFaceNameForCharSet(charset, &strFontName) || strFontName.length () <= 0)
    {
        DebugPrintf(DLC_KERNELGEAR, "GetAvailableFontInfo GetDefaultFontFaceNameForCharSet No special font setted for charset %d", charset);
        g_pFontManager->GetSystemDefaultFontFaceName(&strFontName);
    }

    if (strFontName.length () <= 0)
    {
        DebugPrintf(DLC_KERNELGEAR, "GetAvailableFontInfo GetDefaultFontFaceNameForCharSet error!!!");
        return;
    }

    if (pStrFontName)
    {
        *pStrFontName = strFontName;
    }

    if (pStrFontPath)
    {
        // 得到对应facename的DKFont
        DKFont* pFont = g_pFontManager->GetFontForFaceName (strFontName);
        if (!pFont)
        {
            DebugPrintf(DLC_KERNELGEAR, "GetAvailableFontInfo GetFontForFaceName error!!!");
            return;
        }

        // 得到默认字体的文件路径
        DK_WCHAR* pFontPath = EncodingUtil::CharToWChar(pFont->GetFontPath());
        if (NULL == pFontPath)
        {
            DebugPrintf(DLC_KERNELGEAR, "GetAvailableFontInfo wsFontPath CharToWChar error!!!");
            return;
        }

        *pStrFontPath = pFontPath;

        DK_FREE(pFontPath);
    }
}

void SetTxtDefaultFont ()
{
    DebugPrintf(DLC_KERNELGEAR, "SetTxtDefaultFont start!!!");

    // 初始化各字符集字体
    wstring fontName (L"");
    DK_CHARSET_TYPE lstCharset[] = {DK_CHARSET_ANSI, DK_CHARSET_GB, DK_CHARSET_BIG5, DK_CHARSET_SHITJIS, DK_CHARSET_JOHAB};
    int iSize = sizeof(lstCharset)/sizeof(DK_CHARSET_TYPE);
    for (int i = 0; i < iSize; ++i)
    {
        GetAvailableFontInfo (lstCharset[i], &fontName, NULL);

        // 设置默认字体
        if (!DKT_SetDefaultFont(fontName.c_str(), lstCharset[i]))
        {
            DebugPrintf(DLC_KERNELGEAR, "SetTxtDefaultFontInternal SetDefaultFont error %d", lstCharset[i]);
        }
    }
    DebugPrintf(DLC_KERNELGEAR, "SetTxtDefaultFont finish!!!");
}

//---------------------------------------------------------------------------

void RegisterSimilarFonts ()
{
    DebugPrintf (DLC_KERNELGEAR, "RegisterSimilarFonts start!!!");

    const DK_CHAR* fontFiles[] = {"fzssjt.ttf",  "fzxbsjt.ttf", "hwkt.ttf", "fzlth_gb18030.ttf", "fzltkh_gbk.ttf", "hyfs.ttf",  "Inconsolata.ttf", "GenBkBas.ttc", "dk-symbol.ttf",  NULL};
    const DK_WCHAR* fontFaces[][10] = 
    {
        {L"DK-SONGTI", L"宋体", L"方正宋三简体", L"方正宋三", L"方正书宋", L"方正书宋简体", L"方正书宋_GBK", DK_NULL},
        {L"DK-XIAOBIAOSONG", L"方正小标宋", L"方正小标宋简体", DK_NULL},
        {L"DK-KAITI", L"楷体", L"华文楷体", L"方正楷体", L"方正楷体简体", DK_NULL},
        {L"DK-HEITI", L"黑体", L"方正兰亭黑", L"方正兰亭黑简体", L"方正兰亭黑_GBK", L"方正兰亭黑_GB18030", DK_NULL},
        {L"DK-XIHEITI", L"方正兰亭刊黑", L"方正兰亭刊黑简体", L"方正兰亭刊黑_GBK", DK_NULL},
        {L"DK-FANGSONG", L"仿宋", L"华文仿宋", L"汉仪仿宋简", L"方正仿宋", L"方正仿宋简体", DK_NULL},
        {L"DK-CODE", L"Inconsolata", DK_NULL},
        {L"DK-SERIF", L"Palatino", DK_NULL},
        {L"DK-SYMBOL", L"Symbol", DK_NULL},
    };
    vector<DKFont*> lstFont;
    if (!g_pFontManager->GetAllFont(lstFont))
    {
        return;
    }

    for (int i = 0; fontFiles[i]; ++i)
    {
        for (size_t j = 0; j < lstFont.size(); ++j)
        {
            DKFont* font = lstFont[j];
            std::string baseName = PathManager::GetFileName(font->GetFontPath());
            if (baseName == fontFiles[i])
            {
                std::wstring wFullFontPath =
                    EncodeUtil::ToWString(font->GetFontPath());
                for (int k = 0; fontFaces[i][k]; ++k)
                {
                    //DebugPrintf (DLC_DIAGNOSTIC, "RegisterSimilarFonts font %s:%s", 
                    //    EncodeUtil::ToString(fontFaces[i][k]).c_str(), 
                    //    font->GetFontPath());
                     DKE_RegisterFont(fontFaces[i][k], wFullFontPath.c_str());
                     DKM_RegisterFont(fontFaces[i][k], wFullFontPath.c_str());
                    DKT_RegisterFont(fontFaces[i][k], wFullFontPath.c_str());
                     DKP_RegisterFontFaceName(fontFaces[i][k], wFullFontPath.c_str());
                }

            }
        }
    }
    DebugPrintf (DLC_KERNELGEAR, "RegisterSimilarFonts finish!!!");
}

void UnRegisterSimilarFonts ()
{
    DebugPrintf (DLC_KERNELGEAR, "UnRegisterSimilarFonts start!!!");

    const DK_CHAR* fontFiles[] = {"fzssjt.ttf",  "fzxbsjt.ttf", "hwkt.ttf", "fzlth_gb18030.ttf", "fzltkh_gbk.ttf", "hyfs.ttf",  "Inconsolata.ttf", "GenBkBas.ttc", "dk-symbol.ttf",  NULL};
    const DK_WCHAR* fontFaces[][10] = 
    {
        {L"DK-SONGTI", L"宋体", L"方正宋三简体", L"方正宋三", L"方正书宋", L"方正书宋简体", L"方正书宋_GBK", DK_NULL},
        {L"DK-XIAOBIAOSONG", L"方正小标宋", L"方正小标宋简体", DK_NULL},
        {L"DK-KAITI", L"楷体", L"华文楷体", L"方正楷体", L"方正楷体简体", DK_NULL},
        {L"DK-HEITI", L"黑体", L"方正兰亭黑", L"方正兰亭黑简体", L"方正兰亭黑_GBK", L"方正兰亭黑_GB18030", DK_NULL},
        {L"DK-XIHEITI", L"方正兰亭刊黑", L"方正兰亭刊黑简体", L"方正兰亭刊黑_GBK", DK_NULL},
        {L"DK-FANGSONG", L"仿宋", L"华文仿宋", L"汉仪仿宋简", L"方正仿宋", L"方正仿宋简体", DK_NULL},
        {L"DK-CODE", L"Inconsolata", DK_NULL},
        {L"DK-SERIF", L"Palatino", DK_NULL},
        {L"DK-SYMBOL", L"Symbol", DK_NULL},
    };
    vector<DKFont*> lstFont;
    if (!g_pFontManager->GetAllFont(lstFont))
    {
        return;
    }

    for (int i = 0; fontFiles[i]; ++i)
    {
        for (size_t j = 0; j < lstFont.size(); ++j)
        {
            DKFont* font = lstFont[j];
            std::string baseName = PathManager::GetFileName(font->GetFontPath());
            if (baseName == fontFiles[i])
            {
                std::wstring wFullFontPath =
                    EncodeUtil::ToWString(font->GetFontPath());
                for (int k = 0; fontFaces[i][k]; ++k)
                {
                    //DebugPrintf (DLC_DIAGNOSTIC, "RegisterSimilarFonts font %s:%s", 
                    //    EncodeUtil::ToString(fontFaces[i][k]).c_str(), 
                    //    font->GetFontPath());
                    DKE_UnregisterFont(fontFaces[i][k]);
                    //DKM_RegisterFont(fontFaces[i][k]);
                    DKT_UnregisterFont(fontFaces[i][k]);
                    //DKP_RegisterFontFaceName(fontFaces[i][k]);
                }

            }
        }
    }
    DebugPrintf (DLC_KERNELGEAR, "UnRegisterSimilarFonts finish!!!");
}

