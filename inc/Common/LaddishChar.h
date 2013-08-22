//===========================================================================
// Summary:
//	    LaddishChar.h
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

#ifndef __KINDLEAPP_INC_COMMON_LADDISHCHAR_H__
#define __KINDLEAPP_INC_COMMON_LADDISHCHAR_H__

class LaddishCharUtil
{
public:
    enum 
    {
        WIDE_PUNCTUATION_SIZE = 101,
        ASCII_PUNCTUATION_SIZE = 29,
        EXASCII_SPACE_IN_WIDECHAR = 0xA0,
        CHINESE_SPACE_IN_WIDECHAR = 0x3000
    };

    static bool IsBlankChar(char ch)
    {
        return (ch >= 0x09 && ch <= 0x0D) || (0x20 == ch);
    }
    
    static bool IsBlankChar(wchar_t ch)
    {
        if (ch < 128)
        {
            return IsBlankChar((char)ch);
        }

        return false;
    }

    static bool IsWideBlankChar(wchar_t ch)
    {
        return IsBlankChar(ch) || 
                EXASCII_SPACE_IN_WIDECHAR == ch ||
                CHINESE_SPACE_IN_WIDECHAR == ch;
    }

    static bool IsNewLine(wchar_t ch)
    {
        return 0x0D == ch || 0x0A == ch;
    }

    static bool IsNewLine(char ch)
    {
        return 0x0D == ch || 0x0A == ch;
    }

    static bool IsAlpha(char ch)
    {
        return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
    }

    static bool IsAlpha(wchar_t ch)
    {
        if (ch < 128)
        {
            return IsAlpha((char)ch);
        }

        return false;
    }

    static bool IsDigit(char ch)
    {
        return ch >= '0' && ch <= '9';
    }

    static bool IsDigit(wchar_t ch)
    {
        if (ch < 128)
        {
            return IsDigit((char)ch);
        }

        return false;
    }

    static bool IsHxDigit(char ch)
    {
        return IsDigit(ch) || ('A' <= ch && ch <= 'F') || ('a' <= ch && ch <= 'f');
    }

    static bool IsAlphaNum(char ch)
    {
        return  IsAlpha(ch) || IsDigit(ch);
    }

    static bool IsAlphaNum(wchar_t ch)
    {
        return IsAlpha(ch) || IsDigit(ch);
    }

    static bool IsNumberStart(char ch)
    {
        return IsDigit(ch) || ('+' == ch) || ('-' == ch) || ('.' == ch);
    }

    static bool IsIdentifierChar(wchar_t ch)
    {
        return IsAlphaNum(ch) || ('_' == ch) || ('-' == ch);
    }

    static bool IsIdentifierChar(char ch)
    {
        return IsAlphaNum(ch) || ('_' == ch) || ('-' == ch);
    }
    
    static bool IsLatinExLetter(wchar_t ch)
    {
        return (ch >= 0xC0 && ch <= 0xFF /*latin-1*/)
            || (ch >= 0x100 && ch <= 0x17F/*latin EX-A*/)
            || (ch >= 0x180 && ch <= 0x24F /*latin EX-B*/);
    }

    static bool IsLatinExLetter(char ch)
    {
        return (ch >= 0xC0 && ch <= 0xFF /*latin-1*/)
            || (ch >= 0x100 && ch <= 0x17F/*latin EX-A*/)
            || (ch >= 0x180 && ch <= 0x24F /*latin EX-B*/);
    }

    static bool IsCyrillicLetter(wchar_t ch)
    {
        return (ch >= 0x400 && ch <= 0x481) || (ch >= 0x48A && ch <= 0x527);
    }

    static bool IsCyrillicLetter(char ch)
    {
        return false;
    }

    static bool IsAscii(wchar_t ch)
    {
        return ch < 128;
    }

    static bool IsSingleWideQuotation(wchar_t ch)
    {
        return 0x2018 == ch || 0x2019 == ch;
    }

    static bool IsDoubleWideQuotation(wchar_t ch)
    {
        return 0x201C == ch || 0x201D == ch;
    }

    static bool IsPunctuation (wchar_t ch)
    {
        return IsAsciiPunctuation(ch) || IsWidePunctuation(ch);
    }

    // 判断是否汉字
    static bool IsHanCharacter(wchar_t ch);

public:
    static bool IsWidePunctuation(wchar_t ch);
    static bool IsAsciiPunctuation(wchar_t ch);

private:
    LaddishCharUtil(){}
    static const wchar_t m_widePunctuation[WIDE_PUNCTUATION_SIZE];
    static const wchar_t m_asciiPunctuation[ASCII_PUNCTUATION_SIZE];
};

#endif // #ifndef __KINDLEAPP_INC_COMMON_LADDISHCHAR_H__
