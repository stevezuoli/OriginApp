#ifndef __TEXTFONT_INCLUDED__
#define __TEXTFONT_INCLUDED__

#include "dkBaseType.h"
#include "dkImage.h"
#include "dkColor.h"
#include "dkObjBase.h"

namespace DkFormat
{
    class ITextFont
    {    
    public:
        virtual ~ITextFont(){};
        ITextFont(){};

        STDMETHOD(LoadFont)(const char* lpszFileName)=0;
        STDMETHOD(SetFontSize)(unsigned int nFontSize)=0; // size is in point unit

        STDMETHOD(GetCharHeight)(unsigned int charCode, int* piCharWidth)=0;
        STDMETHOD(GetCharWidth)(unsigned int charCode, int* piCharWidth)=0;
        STDMETHOD(GetMaxCharWidth)(int* piMaxCharWidth)=0;
        STDMETHOD(GetLineHeight)(int* piLineHeight)=0;
        STDMETHOD(GetDescender)(int* piDecender)=0;
        STDMETHOD(GetAscender)(int* piAscender)=0;
        STDMETHOD(GetFontSize)(int* piFontSize)=0;
        //
        // UTF8 字符串像素高度
        STDMETHOD(GetStringHeight)(const char *pStr, unsigned int nLength, int* height)=0;

        // Unicode  字符串像素高度
        STDMETHOD(GetStringHeight)(const wchar_t *pStr, unsigned int nLength, int* height)=0;
        
        // UTF8 字符串像素宽度
        STDMETHOD(GetStringWidth)(const char *pStr, unsigned int nLength, int* piWidth)=0;

        // Unicode  字符串像素宽度
        STDMETHOD(GetStringWidth)(const wchar_t *pStr, unsigned int nLength, int* piWidth)=0;

        // 将drawRect区域上的字符图片画到pbDest去
        STDMETHOD(DrawCharImage)(
            DK_IMAGE &rImage,        // 目标区域
            const DK_POINT_OLD &rPenPos,    // 画笔位置，与drawRect都是相对于同一个原点，由调用者确定
            const DK_RECT &rDrawRect,    // 显示区域，只有当当前字符处于drawRect之内（或部分处于），才会画到pbDest去
            unsigned int nCharCode,        // 要画的字符编码, UTF32
            int* piPenOff            // 画完之后需要把pen向前移动的距离
        )=0;

        // UTF8 字符串
        STDMETHOD(DrawStringUTF8)(
            DK_IMAGE &rImage, 
            const DK_RECT rDrawRect, 
            const char *pStr,
            bool fDrawEllipsis=true,
            unsigned int nCharSpacing=0
        )=0;

        // Unicode 字符串
        STDMETHOD(DrawStringW)(
            DK_IMAGE &rImage, 
            const DK_RECT &rDrawRect, 
            const wchar_t *pStr,
            bool fDrawEllipsis=true,
            unsigned int nCharSpacing=0
        )=0;


        STDMETHOD(SetFontColor(unsigned char r, unsigned char g, unsigned char b))=0;
    };

    HRESULT CreateTextFontInstance(
        ITextFont **ppTextFont
    );        
}

#endif    // ndef __TEXTFONT_INCLUDED__

