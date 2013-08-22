#ifndef _LADDISH_APPUI_LAYOUTHELPER_H_
#define _LADDISH_APPUI_LAYOUTHELPER_H_

#include <stdint.h>

typedef enum
{
    LAYOUT_STYLE_ELEGANT,
    LAYOUT_STYLE_STANDARD,
    LAYOUT_STYLE_COMPACT,
	LAYOUT_SYSLE_NORMAL,
    LAYOUT_STYLE_CUSTOM,
    LAYOUT_STYLE_COUNT
}LayoutStyle, *pLayoutStyle;

class LayoutHelper
{
    static const uint32_t s_fontSizeTable[];
    static const uint32_t s_fontSizeTable_kp[];
public:
    static bool LayoutParaFromStyle(LayoutStyle layoutStyle,
                             uint32_t* lineSpacing,
                             uint32_t* paraSpacing,
                             uint32_t* indent,uint32_t* pTopMargin, uint32_t* pLeftMargin);
    static uint32_t GetFontSizeForBookReader();
    static int GetFontSizeIndex();
    static const uint32_t* GetFontSizeTable();
    static uint32_t GetFontSizeTableSize();
    static LayoutStyle GetLayoutStyle();

private:
    LayoutHelper();
};
#endif
