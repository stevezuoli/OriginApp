////////////////////////////////////////////////////////////////////////
// ColorManager.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __COLORMANAGER_H__
#define __COLORMANAGER_H__

#include "BasicType.h"
#include "Common/Defs.h"


enum DefinedColors
{
    COLOR_SOFTKEY_TXT = 1,
    COLOR_SOFTKEY_TXT_HIT,
    COLOR_SELECTION_TXT,
    COLOR_TXT,
    COLOR_MSG_ERR_TXT,
    COLOR_MSG_INFO_TXT,
    COLOR_SCROLLBAR_BACK,
    COLOR_SCROLLBAR_SLIDER,
    COLOR_SCROLLBAR_BORDER,
    COLOR_SCROLLBAR_SLIDER_BORDER,
    COLOR_MENUITEM_INACTIVE,
    COLOR_PROGRESSBAR_BACK,
    COLOR_PROGRESSBAR_FORE,
    COLOR_TITLE, 
    COLOR_MATCHED_BACKGROUND,
    COLOR_SEARCHPANE_BORDER,
    COLOR_SEARCHPANE_BACKCOLOR,
    COLOR_DISABLECOLOR,
    COLOR_KERNEL_DISABLECOLOR,
    COLOR_TITLE_BACKGROUND,
    COLOR_TAB_BACKGROUND
};

class ColorManager
{
    ////////////////////Constructor Section/////////////////////////
private:
    ColorManager();
    ////////////////////Constructor Section/////////////////////////

    ////////////////////Method Section/////////////////////////
public:
    static dkColor GetColor(int iId);
    static const dkColor knBlack = 0xFFFFFF;
    static const dkColor knWhite = 0x000000;
    ////////////////////Method Section/////////////////////////
};
#endif
