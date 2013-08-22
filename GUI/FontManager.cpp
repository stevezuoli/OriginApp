////////////////////////////////////////////////////////////////////////
// FontManager.cpp
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "GUI/FontManager.h"
#include "Utility/BasicType.h"
#include "GUI/CTpFont.h"
#include "GUI/GUIHelper.h"
#include "interface.h"

// 暂时不用 fzlth_gb18030.ttf， 因为系统设置界面，方正兰亭黑显示效果不如文泉驿微米黑
// fzlth_gb18030 cannot support some special characters, such as "?",  "§",  "?",  "?",  "?",  "�",  "?"...
// #define FONT_DEFAULT_PATH  "res/sysfonts/fzlth_gb18030.ttf"
#define FONT_DEFAULT_PATH  "res/sysfonts/wqy-microhei.ttc"
#define FONT_ENGLISH_PATH  "res/sysfonts/GOTHIC.TTF"

extern char g_szAppPath[];

FontManager* FontManager::m_sFontManager=   NULL;

FontManager::FontManager()
    : m_pFont(NULL),m_pFontEnglish(NULL)
{
}

FontManager::~FontManager()
{
    if (NULL != m_pFont)
    {
        delete m_pFont;
        m_pFont = NULL;
    }
    if (NULL != m_pFontEnglish)
    {
        delete m_pFontEnglish;
        m_pFontEnglish = NULL;
    }
}

FontManager* FontManager::GetInstance()
{
    if (!m_sFontManager)
    {
        // note, this implementation is not thread safety
        m_sFontManager = new FontManager();
        m_sFontManager->LoadFont();
    }

    return m_sFontManager;
}

ITpFont* FontManager::GetFont(INT32 iFontId)
{
    switch(iFontId)
    {
        
        case FONT_ENGLISH:
            {
                if(!m_pFontEnglish)
                {
                    return NULL;
                }
                else
                {
                    m_pFontEnglish->SetFontSize(20);
                    m_pFontEnglish->SetFontColor(0xff, 0xff, 0xff);
                    return m_pFontEnglish;
                }
            }
            break;

		case FONT_DEFAULT:
        default:
            {
                if(!m_pFont)
                {
                    return NULL;
                }
                else
                {
                    m_pFont->SetFontSize(20);
                    m_pFont->SetFontColor(0xff, 0xff, 0xff);
                    return m_pFont;
                }
            }
            break;
    }

    return NULL;
    
}

ITpFont* FontManager::GetFont(INT32 /*iFace*/, INT32 /*iStyle*/, INT32 /*iSize*/)
{
    return m_pFont;
}

ITpFont* FontManager::GetFont(const FontAttributes& rFontAttrs, INT32 iRGB)
{
    ITpFont* pFont = GetFont(rFontAttrs.m_iFace);
    if(pFont)
    {
        pFont->SetFontSize(rFontAttrs.m_iSize);
        pFont->SetFontColor(
            GUIHelper::ExtractRedComponent(iRGB),
            GUIHelper::ExtractGreenComponent(iRGB),
            GUIHelper::ExtractBlueComponent(iRGB)
        );
        return pFont;
    }
    return NULL;
}

void FontManager::LoadFont()
{
    if (NULL == m_pFont)
    {
        m_pFont = new CTpFont();
    }
    
    if(m_pFont)
    {
        char *pFontPath = NULL;
        INT32 iPathLength = strlen(g_szAppPath);
        INT32 iFontNameLength = strlen(FONT_DEFAULT_PATH);
        pFontPath = new char[iPathLength + iFontNameLength + 1];
        if(pFontPath)
        {
            memset(pFontPath, 0, iPathLength + iFontNameLength + 1);
            memcpy(pFontPath, g_szAppPath, iPathLength);
            memcpy(pFontPath + iPathLength, FONT_DEFAULT_PATH, iFontNameLength);
            m_pFont->LoadFont(pFontPath);
            delete []pFontPath;
        }
    }
    if(NULL == m_pFontEnglish)
    {
        m_pFontEnglish = new CTpFont();
    }
    if(m_pFontEnglish)
    {
        char *pFontPath = NULL;
        INT32 iPathLength = strlen(g_szAppPath);
        INT32 iFontNameLength = strlen(FONT_ENGLISH_PATH);
        pFontPath = new char[iPathLength + iFontNameLength + 1];
        if(pFontPath)
        {
            memset(pFontPath, 0, iPathLength + iFontNameLength + 1);
            memcpy(pFontPath, g_szAppPath, iPathLength);
            memcpy(pFontPath + iPathLength, FONT_ENGLISH_PATH, iFontNameLength);
            m_pFontEnglish->LoadFont(pFontPath);
            delete []pFontPath;
        }
    }
    return;

}

void FontManager::UnLoadFont()
{
    if (NULL != m_pFont)
    {
        m_pFont->UnLoadFont();
    }
    if(NULL != m_pFontEnglish)
    {
        m_pFontEnglish->UnLoadFont();
    }
}



