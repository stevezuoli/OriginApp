////////////////////////////////////////////////////////////////////////
// ColorManager.cpp
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "Utility/ColorManager.h"

#define DEFAULT_SYSTEM_COLOR 0xFFFFFF

ColorManager::ColorManager()
{
    // empty
}

INT32 ColorManager::GetColor(INT32 iId)
{
    switch (iId)
    {
        case COLOR_TITLE:
            return 0xEEEEEE;
        case COLOR_SOFTKEY_TXT:
            return 0xFFFFFF; // white
        case COLOR_SOFTKEY_TXT_HIT:
            return 0xFFFFFF; // white
        case COLOR_SELECTION_TXT:
            return 0x122300;
        case COLOR_TXT:
            return 0x161616;
        case COLOR_MSG_ERR_TXT:
            return 0x4D0000;
        case COLOR_MSG_INFO_TXT:
            return 0x4D0000;
        case COLOR_SCROLLBAR_BACK:
            return 0xA9A9A9;
        case COLOR_SCROLLBAR_BORDER:
            return 0x9D9D9D;
        case COLOR_SCROLLBAR_SLIDER:
            return 0x5E5E5E;
        case COLOR_SCROLLBAR_SLIDER_BORDER:
            return 0x444444;
        case COLOR_MENUITEM_INACTIVE:
            return 0xAAAAAA;
        case COLOR_PROGRESSBAR_BACK:
            return 0x898989;
        case COLOR_PROGRESSBAR_FORE:
            return 0x91CE00;
        case COLOR_MATCHED_BACKGROUND:
            return 0x9D9D9D;
        case COLOR_SEARCHPANE_BORDER:
            return 0x9D9D9D;
        case COLOR_SEARCHPANE_BACKCOLOR:
            return 0xDDFD71;
        case COLOR_DISABLECOLOR:
            return 0x666666;
        case COLOR_KERNEL_DISABLECOLOR:
            return 0x999999;
        case COLOR_TITLE_BACKGROUND:
            return 0x161616;
        case COLOR_TAB_BACKGROUND:
            return 0x161616;
    }

    return DEFAULT_SYSTEM_COLOR;
}

