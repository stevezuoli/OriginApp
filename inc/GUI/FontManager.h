////////////////////////////////////////////////////////////////////////
// FontManager.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __FONTMANAGER_H__
#define __FONTMANAGER_H__

#include "GUI/ITpFont.h"
#include "Utility/ColorManager.h"

enum
{
    FONT_DEFAULT,
    FONT_ENGLISH,
    FONT_TITLE_BAR,
    FONT_RICH_TEXT,
    FONT_MENU,
    FONT_SOFTKEY_MENU,
    FONT_TAB_BAR
};

class FontManager
{
public:
    virtual ~FontManager();


    static FontManager* GetInstance();
    
    // Note, this mehtod is removed. It should be added back if it is needed.
    // static ITpFont* GetFontNoLargerThan(INT32 iFace, INT32 iStyle, INT32 iSizeInPixel);

    ITpFont* GetFont(INT32 iFontId=FONT_DEFAULT);
    ITpFont* GetFont(INT32 iFace, INT32 iStyle, INT32 iSize);
    ITpFont* GetFont(const FontAttributes& rFontAttrs, INT32 iRGB = ColorManager::knBlack);

    void LoadFont();
    void UnLoadFont();

private:
    FontManager();

private:
    static FontManager* m_sFontManager;
    ITpFont* m_pFont;
    ITpFont* m_pFontEnglish;
};
#endif

