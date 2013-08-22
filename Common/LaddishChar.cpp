//===========================================================================
// Summary:
//	    LaddishChar.cpp
// Usage:
//      判断字符的特征，如是否为数字、标点等
//  		...
// Remarks:
//	    Null
// Date:
//      2012-03-22
// Author:
//      Liu Hongjia(liuhj@duokan.com)
//===========================================================================

#include "Common/LaddishChar.h"

const wchar_t LaddishCharUtil::m_widePunctuation[WIDE_PUNCTUATION_SIZE] = 
{
    0xA0,
    0xB7,
    0x3002,
    0x3001,
    0xFF01,
    0xFF1F,
    0xFF1F,
    0xFF1A,
    0xFF1B,
    0xFE51,
    0xFF02,
    0x2026,
    0x2018,
    0x2019,
    0x201C,
    0x201D,
    0x301D,
    0x301E,
    0x2215,
    0x2016,
    0x2014,
    0x3000,
    0xFE5B,
    0xFE5C,
    0x3008,
    0x3009,
    0xFE5D,
    0xFE5E,
    0x300C,
    0x300D,
    0x3016,
    0x3017,
    0x300A,
    0x300B,
    0x3014,
    0x3015,
    0x300E,
    0x300F,
    0x3010,
    0x3011,
    0xFE50,
    0xFE55,
    0xFE54,
    0xFF01,
    0xFF1F,
    0xFE56,
    0xFE4F,
    0xFF07,
    0x2CA,
    0xFE6B,
    0xFE33,
    0xFF3F,
    0xFE30,
    0xFE4C,
    0x2032,
    0xFE4B,
    0x2CB,
    0x2015,
    0xFE34,
    0x2C9,
    0xFFE3,
    0xFE62,
    0xFE66,
    0xFE64,
    0xFE5F,
    0xFE69,
    0xFE60,
    0xFE6A,
    0xFE61,
    0xFE68,
    0xFE4D,
    0xFE4E,
    0x2010,
    0xFE49,
    0xFE4A,
    0x2C7,
    0xFE35,
    0xFE36,
    0xFE38,
    0xFE37,
    0xFE3F,
    0xFE40,
    0xFE3A,
    0xFE39,
    0xFE3D,
    0xFE3E,
    0x2C9,
    0xFE41,
    0xFE42,
    0xFE44,
    0xFE43,
    0xFE3B,
    0xFE3C,
    0xFF3B,
    0xFF3D,
    0xFF0C,
    0xFF08,
    0xFF09,
    0xFFE5,
    0xFF0E,
    0x2500
};

const wchar_t LaddishCharUtil::m_asciiPunctuation[ASCII_PUNCTUATION_SIZE] =
{
    '!',
    '`',
    '#',
    '$',
    '%',
    '^',
    '&',
    '*',
    '(',
    ')',
    '+',
    '=',
    '{',
    '}',
    '[',
    ']',
    ';',
    ':',
    '\'',
    '\"',
    '|',
    '\\',
    ',',
    '<',
    '>',
    '.',
    '?',
    '/',
    ' '
};

bool LaddishCharUtil::IsWidePunctuation(wchar_t ch)
{
    for (unsigned int i = 0; i < WIDE_PUNCTUATION_SIZE; i++)
    {
        if (m_widePunctuation[i] == ch)
        {
            return true;
        }
    }

    return false;
}

bool LaddishCharUtil::IsAsciiPunctuation(wchar_t ch)
{
    if (!IsAscii(ch))
    {
        return false;
    }
    
    for (unsigned int i = 0; i < ASCII_PUNCTUATION_SIZE; i++)
    {
        if (m_asciiPunctuation[i] == ch)
        {
            return true;
        }
    }

    return false;
}

// 现在判断很粗糙,只检测了>128,非空格,非标点
bool LaddishCharUtil::IsHanCharacter(wchar_t ch)
{
    return ch > 128 && !IsBlankChar(ch) && !IsPunctuation(ch);
}
