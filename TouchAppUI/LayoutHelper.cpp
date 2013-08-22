#include "TouchAppUI/LayoutHelper.h"
#include <stddef.h>
#include "dkDim.h"
#include "Common/ConfigInfo.h"
#include "Common/SystemSetting_DK.h"
#include "Framework/CDisplay.h"
#include "TouchAppUI/LayoutHelper.h"
#include "drivers/DeviceInfo.h"

#define DEFAULT_FONT_SIZE_INDEX 2
bool LayoutHelper::LayoutParaFromStyle(LayoutStyle layoutStyle, uint32_t* lineSpacing, uint32_t* paraSpacing, uint32_t* indent, uint32_t* pTopMargin, uint32_t* pLeftMargin)
{
    if (NULL == lineSpacing || NULL == paraSpacing || NULL == indent)
    {
        return false;
    }
    switch(layoutStyle)
    {
        case LAYOUT_STYLE_ELEGANT:
            *lineSpacing = 140;
            *paraSpacing = 120;
            *indent = 0;
            if (DeviceInfo::IsKPW())
            {
                *pTopMargin = 38;
                *pLeftMargin = 38;
            }
            else
            {
                *pTopMargin = 30;
                *pLeftMargin = 30;
            }
            break;
        case LAYOUT_STYLE_STANDARD:
            *lineSpacing = 120;
            *paraSpacing = 90;
            *indent = 2;
            if (DeviceInfo::IsKPW())
            {
                *pTopMargin = 19;
                *pLeftMargin = 26;
            }
            else
            {
                *pTopMargin = 15;
                *pLeftMargin = 20;
            }
            break;
        case LAYOUT_STYLE_COMPACT:
            *lineSpacing = 110;
            *paraSpacing = 60;
            *indent = 2;
            if (DeviceInfo::IsKPW())
            {
                *pTopMargin = 19;
                *pLeftMargin = 19;
            }
            else
            {
                *pTopMargin = 15;
                *pLeftMargin = 15;
            }
            break;
		case LAYOUT_SYSLE_NORMAL:
			*lineSpacing = 120;
			*paraSpacing = 90;
			*indent = 2;
            if (DeviceInfo::IsKPW())
            {
                *pTopMargin = 19;
                *pLeftMargin = 26;
            }
            else
            {
                *pTopMargin = 15;
                *pLeftMargin = 20;
            }
			break;
        default:
            return false;
            break;
    }
    return true;
}

const uint32_t LayoutHelper::s_fontSizeTable[] = {18, 20, 22, 24, 26, 28, 30, 32, 36, 40, 52};
const uint32_t LayoutHelper::s_fontSizeTable_kp[] = {22, 24, 26, 28, 30, 32, 34, 36, 40, 52, 66};

uint32_t LayoutHelper::GetFontSizeForBookReader() 
{
    SystemSettingInfo* systemSettingInfo = SystemSettingInfo::GetInstance();
    if (NULL == systemSettingInfo)
    {
        return GetFontSizeTable()[DEFAULT_FONT_SIZE_INDEX];
    }

    return systemSettingInfo->GetFontSize();
}

uint32_t LayoutHelper::GetFontSizeTableSize()
{
    if (DeviceInfo::IsKPW())
    {
        return DK_DIM(s_fontSizeTable_kp);
    }
    else
    {
        return DK_DIM(s_fontSizeTable);
    }
}

const uint32_t* LayoutHelper::GetFontSizeTable()
{
    if (DeviceInfo::IsKPW())
    {
        return s_fontSizeTable_kp;
    }
    else
    {
        return s_fontSizeTable;
    }
}
int LayoutHelper::GetFontSizeIndex()
{
    unsigned int iFontSize = GetFontSizeForBookReader();
    int iTableSize = (int)GetFontSizeTableSize();
    for(int i = 0; i < iTableSize; i++)
    {
        if(iFontSize == GetFontSizeTable()[i])
        {
            return i;
        }
    }
    return DEFAULT_FONT_SIZE_INDEX;
}

LayoutStyle LayoutHelper::GetLayoutStyle()
{
    LayoutStyle style = LAYOUT_STYLE_COMPACT;
    SystemSettingInfo* systemSettingInfo = SystemSettingInfo::GetInstance();
    if (NULL == systemSettingInfo)
    {
        return style;
    }
    int typeSetting = systemSettingInfo->GetTypeSetting();
    if (0 <= typeSetting && typeSetting < LAYOUT_STYLE_COUNT)
    {
        style = (LayoutStyle)typeSetting;
    }
    return style;
}

