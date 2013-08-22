#include "FontManager/DKFont.h"
#include "FontManager/DKTTFInfo.h"
#include "DKEAPI.h"
#include "interface.h"
#include <stdio.h>
#include <vector>
#include <dlfcn.h>

using std::vector;
using std::string;

DK_INT TestFontManager()
{
    DebugPrintf(DLC_JUGH, "TestFontManager START!!!"); 
    
    vector<string>                vFontDir;
    vector<string>::iterator      viFontDir;
    vector<DKFont*>               vFont;
    vector<DKFont*>::iterator     viFont;
    
    vector<DK_UINT16>::iterator   viCP;
    vector<DK_UINT16>             vCodepage;
    vCodepage.push_back(1361);
    vCodepage.push_back(1252);
    vCodepage.push_back(1250);
    vCodepage.push_back(1251);
    vCodepage.push_back(1253);
    vCodepage.push_back(1254);
    vCodepage.push_back(1257);
    vCodepage.push_back(1258);
    vCodepage.push_back(932);
    vCodepage.push_back(936);
    vCodepage.push_back(949);
    vCodepage.push_back(950);
    vCodepage.push_back(869);
    vCodepage.push_back(866);
    vCodepage.push_back(865);
    vCodepage.push_back(863);
    vCodepage.push_back(861);
    vCodepage.push_back(860);
    vCodepage.push_back(857);
    vCodepage.push_back(855);
    vCodepage.push_back(852);
    vCodepage.push_back(850);
    vCodepage.push_back(775);
    vCodepage.push_back(737);
    vCodepage.push_back(437);
    vCodepage.push_back(1);
    
    vector<DK_CHARSET_TYPE>::iterator   viCS;
    vector<DK_CHARSET_TYPE>             vCharset;
    vCharset.push_back(DK_CHARSET_ANSI);
    vCharset.push_back(DK_CHARSET_SHITJIS);
    vCharset.push_back(DK_CHARSET_JOHAB);
    vCharset.push_back(DK_CHARSET_GB);
    vCharset.push_back(DK_CHARSET_BIG5);
    
    DKFontManager FontManager;
    
    DebugPrintf(DLC_JUGH, "测试AddFontsDir功能");
    DebugPrintf(DLC_JUGH, "%s", USER_FONT_PATH);
    FontManager.AddFontsDir(USER_FONT_PATH);
    DebugPrintf(DLC_JUGH, "%s", SYSTEM_FONT_PATH);
    FontManager.AddFontsDir(SYSTEM_FONT_PATH);
    
    DebugPrintf(DLC_JUGH, "\n测试GetFontListByCodePage功能");    
    for (viCP = vCodepage.begin(); viCP != vCodepage.end(); viCP++)
    {
        DebugPrintf(DLC_JUGH, "Codepage = %d", *viCP);
        if (!FontManager.GetFontListByCodePage(*viCP, vFont))
        {
            DebugPrintf(DLC_JUGH, "\tGetFontListByCodePage ERROR");
        }
        else
        {            
            for (viFont = vFont.begin(); viFont != vFont.end(); viFont++)
            {
                DebugPrintf(DLC_JUGH, "\t%s", (*viFont)->GetFontPath());
            }
        }
    }

    DebugPrintf(DLC_JUGH, "\n测试GetFontListByCharSet功能");     
    for (viCS = vCharset.begin(); viCS != vCharset.end(); viCS++)
    {
        switch(*viCS)
        {
        case DK_CHARSET_ANSI:
            DebugPrintf(DLC_JUGH, "CharSet = DK_CHARSET_ANSI");
            break;
        case DK_CHARSET_SHITJIS:
            DebugPrintf(DLC_JUGH, "CharSet = DK_CHARSET_SHITJIS");
            break;
        case DK_CHARSET_JOHAB:
            DebugPrintf(DLC_JUGH, "CharSet = DK_CHARSET_JOHAB");
            break;
        case DK_CHARSET_GB:
            DebugPrintf(DLC_JUGH, "CharSet = DK_CHARSET_GB");
            break;
        case DK_CHARSET_BIG5:
            DebugPrintf(DLC_JUGH, "CharSet = DK_CHARSET_BIG5");
            break;
        default :
            DebugPrintf(DLC_JUGH, "CharSet is WRONG");
            break;
        }
        
        if (!FontManager.GetFontListByCharSet(*viCS, vFont))
        {
            DebugPrintf(DLC_JUGH, "\tGetFontListByCodePage ERROR");
        }
        else
        {            
            for (viFont = vFont.begin(); viFont != vFont.end(); viFont++)
            {
                DebugPrintf(DLC_JUGH, "\t%s", (*viFont)->GetFontPath());
            }
        }
    } 
    
    DebugPrintf(DLC_JUGH, "\n测试RefreshAllFontsInfo功能");
    FontManager.RefreshAllFontsInfo();
    DebugPrintf(DLC_JUGH, "GetFontsDir");
    FontManager.GetFontsDir(vFontDir);
    for (viFontDir = vFontDir.begin(); viFontDir != vFontDir.end(); viFontDir++)
    {
        DebugPrintf(DLC_JUGH, "\t%s", (*viFontDir).c_str());
    }
    
    DebugPrintf(DLC_JUGH, "\n测试GetAllFontsDir功能");
    FontManager.GetAllFontsDir(USER_FONT_PATH, vFontDir);
    DebugPrintf(DLC_JUGH, "GetAllFontsDir");
    for (viFontDir = vFontDir.begin(); viFontDir != vFontDir.end(); viFontDir++)
    {
        DebugPrintf(DLC_JUGH, "\t%s", (*viFontDir).c_str());
    }
    FontManager.GetAllFontsDir(SYSTEM_FONT_PATH, vFontDir);
    DebugPrintf(DLC_JUGH, "GetAllFontsDir");
    for (viFontDir = vFontDir.begin(); viFontDir != vFontDir.end(); viFontDir++)
    {
        DebugPrintf(DLC_JUGH, "\t%s", (*viFontDir).c_str());
    }
    
    FontManager.Release();
    DebugPrintf(DLC_JUGH, "TestFontManager FINISH!!!");
    return 1;
}
