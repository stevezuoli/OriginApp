#include "GUI/TextFont.h"
#include "Utility.h"
#include "dkDebug.h"
#include "dkWinError.h"
#include "Utility/EncodeUtil.h"

using namespace dk::utility;

extern "C"{
    #include "ft2build.h"
    #include FT_FREETYPE_H
}

#define DOT_ASCII_VALUE    46

namespace DkFormat
{
    class CTextFont : public ITextFont
    {
    public:
        CTextFont();
        virtual ~CTextFont(void);

        STDMETHOD(LoadFont)(const char* lpszFileName);
        STDMETHOD(SetFontSize)(unsigned int nFontSize); // size is in point unit

        STDMETHOD(GetCharHeight)(unsigned int charCode, int* charHeight);
        STDMETHOD(GetCharWidth)(unsigned int charCode, int* piCharWidth);
        STDMETHOD(GetMaxCharWidth)(int* piMaxCharWidth);
        STDMETHOD(GetLineHeight)(int* piLineHeight);
        STDMETHOD(GetDescender)(int* piDecender);
        STDMETHOD(GetAscender)(int* piAscender);
        STDMETHOD(GetFontSize)(int* piFontSize);

        STDMETHOD(GetStringHeight)(const char *pStr, unsigned int nLength, int* height);
        STDMETHOD(GetStringHeight)(const wchar_t *pStr, unsigned int nLength, int* height);

        // UTF8 字符串像素宽度
        STDMETHOD(GetStringWidth)(const CHAR *pStr, unsigned int nLength, int* piWidth);

        // Unicode  字符串像素宽度
        STDMETHOD(GetStringWidth)(const wchar_t *pStr, unsigned int nLength, int* piWidth);

        // 将drawRect区域上的字符图片画到pbDest去
        STDMETHOD(DrawCharImage)(
            DK_IMAGE &rImage,        // 目标区域
            const DK_POINT_OLD &rPenPos,    // 画笔位置，与drawRect都是相对于同一个原点，由调用者确定
            const DK_RECT &rDrawRect,    // 显示区域，只有当当前字符处于drawRect之内（或部分处于），才会画到pbDest去
            unsigned int nCharCode,        // 要画的字符编码, UTF32
            int* piPenOff            // 画完之后需要把pen向前移动的距离
        );

        // UTF8 字符串
        STDMETHOD(DrawStringUTF8)(
            DK_IMAGE &rImage, 
            const DK_RECT rDrawRect, 
            const char *pStr,
            bool fDrawEllipsis=true, 
            unsigned int nCharSpacing=0
        );

        // Unicode 字符串
        STDMETHOD(DrawStringW)(
            DK_IMAGE &rImage, 
            const DK_RECT &rDrawRect, 
            const wchar_t *pStr,
            bool fDrawEllipsis=true, 
            unsigned int nCharSpacing=0
        );

        STDMETHOD(SetFontColor)(unsigned char r, unsigned char g, unsigned char b);

    private:
        int GetStringWidth(const wchar_t *pStr, unsigned int nLength);
        int GetStringHeight(const wchar_t *pStr, unsigned int nLength);

        bool m_fIsInitialized;
        FT_Library m_pLibrary;
        FT_Face m_pFace;
        int m_iFontSize;
        DKColor m_rgbColor;
    };

}

using namespace DkFormat;

HRESULT DkFormat::CreateTextFontInstance(
    ITextFont **ppTextFont
    )
{
    if (NULL == ppTextFont)
    {
        return E_POINTER;
    }

    *ppTextFont = NULL;
    HRESULT hr(S_OK);

    CTextFont * pImpl= new CTextFont();
    if (pImpl)
    {
        *ppTextFont = pImpl;
    }
    else
    {
        return E_OUTOFMEMORY;
    }

    return hr;
}


CTextFont::CTextFont() 
    : m_fIsInitialized(FALSE), m_pLibrary(NULL), m_pFace(NULL), m_iFontSize(16), m_rgbColor(DKColors::black)
{
    // do nothing
}


CTextFont::~CTextFont(void)
{
    if (this->m_pFace != NULL)
    {
        FT_Done_Face(this->m_pFace);
        this->m_pFace = NULL;
    }

    if (this->m_pLibrary != NULL)
    {
        FT_Done_FreeType(this->m_pLibrary);
        this->m_pLibrary = NULL;
    }

    m_fIsInitialized = FALSE;
}

HRESULT CTextFont::LoadFont(const char* lpszFileName)
{
    this->m_fIsInitialized = FALSE;
    if (NULL == lpszFileName)
    {
        return E_FAIL;
    }

    FT_Error error;
    if (NULL == this->m_pLibrary)
    {
        error = FT_Init_FreeType(&this->m_pLibrary);
        if (error)
        {
            this->m_pLibrary = NULL;
            return E_FAIL;
        }
    }

    if (this->m_pFace != NULL)
    {
        FT_Done_Face(this->m_pFace);
        this->m_pFace = NULL;
    }

    error = FT_New_Face(this->m_pLibrary,
        lpszFileName,
        0,
        &this->m_pFace);
    if (error)
    {
        return E_FAIL;
    }

    FT_Select_Charmap(this->m_pFace, FT_ENCODING_UNICODE);

    this->m_fIsInitialized = true;
    return S_OK;
}

HRESULT CTextFont::SetFontSize(unsigned int nFontSize)
{
    if (FALSE == this->m_fIsInitialized)
    {
        return E_FAIL;
    }

    FT_Error error = FT_Set_Char_Size(this->m_pFace,
        0,
        nFontSize << 6,
        0, 
        0);
    if (error)
    {
        return E_FAIL;
    }

    this->m_iFontSize = nFontSize;
    return S_OK;
}

HRESULT CTextFont::GetCharHeight(unsigned int charCode, int* charHeight)
{
    if (NULL == charHeight)
    {
        return E_POINTER;
    }

    *charHeight = 0;

    if (FALSE == this->m_fIsInitialized)
    {
        return E_OBJ_NO_INIT;
    }

    FT_Error error = FT_Load_Char(this->m_pFace, charCode, FT_LOAD_RENDER);
    if (error)
    {
        // error occured, return the max char width in this font.
        return this->GetLineHeight(charHeight);
    }

    *charHeight = this->m_pFace->glyph->advance.y >> 6;
    return S_OK;
}

HRESULT CTextFont::GetCharWidth(unsigned int charCode, int* piCharWidth)
{
    if (NULL == piCharWidth)
    {
        return E_POINTER;
    }

    *piCharWidth = 0;

    if (FALSE == this->m_fIsInitialized)
    {
        return E_OBJ_NO_INIT;
    }

    FT_Error error = FT_Load_Char(this->m_pFace, charCode, FT_LOAD_RENDER);
    if (error)
    {
        // error occured, return the max char width in this font.
        return this->GetMaxCharWidth(piCharWidth);
    }

    *piCharWidth = this->m_pFace->glyph->advance.x >> 6;
    return S_OK;
}

HRESULT CTextFont::GetStringHeight(const char *pStr, unsigned int nLength, int* height)
{
    if (NULL == pStr || NULL == height)
    {
        return E_INVALIDARG;
    }

	std::wstring wstString = EncodeUtil::ToWString(pStr);
    *height = GetStringHeight(wstString.c_str(), wstString.size());
    return S_OK;
}

// UTF8 字符串长度
HRESULT CTextFont::GetStringWidth(const char *pStr, unsigned int nLength, int* piWidth)
{
    if (NULL == pStr || NULL == piWidth)
    {
        return E_INVALIDARG;
    }

	std::wstring wstString = EncodeUtil::ToWString(pStr);
    *piWidth = GetStringWidth(wstString.c_str(), wstString.size());
    return S_OK;
}

// Unicode 字符串长度
HRESULT CTextFont::GetStringWidth(const wchar_t* pStr, unsigned int nLength, int* piWidth)
{
    if (NULL == pStr || NULL == piWidth)
    {
        return E_INVALIDARG;
    }

    INT iWidth = GetStringWidth(pStr, nLength);
    if (iWidth < 0)
    {
        return E_FAIL;
    }

    *piWidth = iWidth;

    return S_OK;
}

HRESULT CTextFont::GetStringHeight(const wchar_t* pStr, unsigned int nLength, int* height)
{
    if (NULL == pStr || NULL == height)
    {
        return E_INVALIDARG;
    }

    INT iHeight = GetStringHeight(pStr, nLength);
    if (iHeight < 0)
    {
        return E_FAIL;
    }

    *height = iHeight;

    return S_OK;
}

HRESULT CTextFont::GetMaxCharWidth(int* piMaxCharWidth)
{
    if (NULL == piMaxCharWidth)
    {
        return E_POINTER;
    }

    *piMaxCharWidth = 0;

    if (FALSE == this->m_fIsInitialized)
    {
        return E_OBJ_NO_INIT;
    }

    *piMaxCharWidth = this->m_pFace->size->metrics.max_advance >> 6;
    return S_OK;
}

HRESULT CTextFont::GetLineHeight(int* piLineHeight)
{
    if (NULL == piLineHeight)
    {
        return E_POINTER;
    }

    *piLineHeight = 0;
    if (FALSE == this->m_fIsInitialized)
    {
        return E_OBJ_NO_INIT;
    }

    *piLineHeight = this->m_pFace->size->metrics.height >> 6;
    return S_OK;
}


HRESULT CTextFont::GetDescender(int* piDescender)
{
    if (NULL == piDescender)
    {
        return E_POINTER;
    }

    *piDescender = 0;
    if (FALSE == this->m_fIsInitialized)
    {
        return E_OBJ_NO_INIT;
    }

    *piDescender = this->m_pFace->size->metrics.descender >> 6;
    return S_OK;
}


HRESULT CTextFont::GetAscender(int* piAscender)
{
    if (NULL == piAscender)
    {
        return E_POINTER;
    }

    *piAscender = 0;
    if (FALSE == this->m_fIsInitialized)
    {
        return E_OBJ_NO_INIT;
    }

    *piAscender = this->m_pFace->size->metrics.ascender >> 6;

    return S_OK;
}

HRESULT CTextFont::DrawCharImage(
    DK_IMAGE &rImage, 
    const DK_POINT_OLD &rPenPos, 
    const DK_RECT &rDrawRect, 
    unsigned int nCharCode,
    int* piPenOff
)
{
    if (NULL == piPenOff)
    {
        return E_POINTER;
    }

    *piPenOff = 0;

    if (NULL == rImage.pbData)
    {
        return E_FAIL;
    }

    FT_Error error = FT_Load_Char(this->m_pFace, nCharCode, FT_LOAD_RENDER);
    if  (error)
    {
        return E_FAIL;
    }

    FT_GlyphSlot slot = this->m_pFace->glyph;

    INT iAscender(0);
    // TODO: Handle Error
    this->GetAscender(&iAscender);

    DK_RECT charRect;
    charRect.left = slot->bitmap_left + rPenPos.x;
    charRect.right = charRect.left + slot->bitmap.width;
    charRect.top = iAscender - slot->bitmap_top - 1 + rPenPos.y;
    charRect.bottom = charRect.top + slot->bitmap.rows;
    DK_RECT intersectRect = CUtility::GetIntersectRect(charRect, rDrawRect);
    if (CUtility::IsEmptyRect(intersectRect))
    {
        // no intersect rect for source and dest, return directly.
        *piPenOff = slot->advance.x >> 6;
        return S_FALSE;
    }

    BYTE8 *pbImg = rImage.pbData + intersectRect.top * rImage.iStrideWidth + intersectRect.left * rImage.iColorChannels;
    BYTE8 *pbSrc = slot->bitmap.buffer;

    if (slot->bitmap.pitch < 0)
    {
        pbSrc = slot->bitmap.buffer - slot->bitmap.pitch * (slot->bitmap.rows - 1);
    }

    pbSrc += (intersectRect.top - charRect.top) * slot->bitmap.pitch + (intersectRect.left - charRect.left);

    INT iIntersectWidth = intersectRect.right - intersectRect.left;
    if (1 == rImage.iColorChannels)
    {
        unsigned char color = (this->m_rgbColor.blue + this->m_rgbColor.green + this->m_rgbColor.red) / 3;
        for (int i = intersectRect.top; i < intersectRect.bottom; i++)
        {
            for(int j = 0; j < iIntersectWidth; j++)
            {
                if (pbSrc[j] != 0)
                {
                    // TODO:这个不对,(kindle黑白是相反的)
                    pbImg[j] = (pbSrc[j] * color + pbImg[j] * (255 - pbSrc[j])) >> 8;
                }
            }

            pbImg += rImage.iStrideWidth;
            pbSrc += slot->bitmap.pitch;
        }
    }
    else if (3 == rImage.iColorChannels || 4 == rImage.iColorChannels)
    {
        for (int i = intersectRect.top; i < intersectRect.bottom; i++)
        {
            for(int j = 0; j < iIntersectWidth; j++)
            {
                if (pbSrc[j] != 0)
                {
                    *pbImg = (this->m_rgbColor.blue * pbSrc[j] + (*pbImg) * (255 - pbSrc[j])) >> 8;
                    *(pbImg + 1) = (this->m_rgbColor.green * pbSrc[j] + pbImg[1] * (255 - pbSrc[j])) >> 8;
                    *(pbImg + 2) = (this->m_rgbColor.red * pbSrc[j] + pbImg[2] * (255 - pbSrc[j])) >> 8;
                }

                pbImg += rImage.iColorChannels;
            }

            pbImg += rImage.iStrideWidth;
            pbSrc += slot->bitmap.pitch;
        }
    }
    else
    {
        //DK_ASSERT(rImage.iColorChannels != 1 && rImage.iColorChannels != 3 && rImage.iColorChannels != 4);
        DK_ASSERT(FALSE);
    }

    *piPenOff = slot->advance.x >> 6;
    return S_OK;

}


HRESULT CTextFont::DrawStringUTF8(DK_IMAGE &rImage, const DK_RECT rDrawRect,const char *pStr, bool fDrawEllipsis, unsigned int nCharSpacing)
{
    if (NULL == pStr)
    {
        return E_POINTER;
    }

    int iStrLen = strlen(pStr);
    if(iStrLen < 1)
    {
        return S_FALSE;
    }

	std::wstring wstString = EncodeUtil::ToWString(pStr);
    // TODO: Handle error.
    DrawStringW(rImage, rDrawRect, wstString.c_str(), fDrawEllipsis, nCharSpacing);
    return S_OK;
}

HRESULT CTextFont::DrawStringW(DK_IMAGE &rImage, const DK_RECT &rDrawRect, const wchar_t *pStr, bool fDrawEllipsis, unsigned int nCharSpacing)
{
    if (NULL == rImage.pbData || NULL == pStr)
    {
        return E_POINTER;
    }

    if (rDrawRect.left >= rDrawRect.right || rDrawRect.top >= rDrawRect.bottom)
    {
        return E_INVALIDARG;
    }

    DK_POINT_OLD penPos;
    penPos.x = rDrawRect.left;
    penPos.y = rDrawRect.top;
    int advance = 0;
    INT iStringWidth = GetStringWidth(pStr, -1);
    INT iRectWidth = rDrawRect.right - rDrawRect.left;
    if (iStringWidth <= iRectWidth)
    {
        while(*pStr)
        {
            unsigned int uTmp = *pStr;
            pStr++;

            // TODO: Handle error.
            this->DrawCharImage(rImage, penPos, rDrawRect, uTmp, &advance);
            penPos.x += advance + nCharSpacing;
        }
    }
    else
    {
        INT iDotWidth = 0;
        HRESULT hr = GetCharWidth(DOT_ASCII_VALUE, &iDotWidth);
        if (FAILED(hr))
        {
            return E_FAIL;
        }

        INT iRemainingWidth = iRectWidth - 3 * iDotWidth;
        while (*pStr && iRemainingWidth > 0)
        {
            // TODO: Handle error.
            unsigned int uTmp = *pStr;

            INT iCharWidth = 0;
            GetCharWidth(uTmp, &iCharWidth);
            if (iRemainingWidth < iCharWidth)
            {
                break;
            }

            this->DrawCharImage(rImage, penPos, rDrawRect, uTmp, &advance);
            penPos.x += advance + nCharSpacing;
            iRemainingWidth -= (advance + nCharSpacing);
            pStr++;
        }

        for (INT i = 0; i < 3; i++)
        {
            this->DrawCharImage(rImage, penPos, rDrawRect, DOT_ASCII_VALUE, &advance);
            penPos.x += advance + nCharSpacing;
        }
    }

    return S_OK;
}

HRESULT CTextFont::GetFontSize(int* piFontSize)
{
    if (NULL == piFontSize)
    {
        return E_POINTER;
    }

    *piFontSize = this->m_iFontSize;
    return S_OK;
}

HRESULT CTextFont::SetFontColor(unsigned char r, unsigned char g, unsigned char b)
{
    this->m_rgbColor.blue = b;
    this->m_rgbColor.green = g;
    this->m_rgbColor.red = r;

    return S_OK;
}

int CTextFont::GetStringWidth(const wchar_t *pStr, unsigned int nLength)
{
    int iWidth = 0;
    int iIndex = 0;
    while (*pStr && (-1 == (int)nLength || iIndex < (int)nLength))
    {
        iIndex++;
        int iCharWidth = 0;
        unsigned int iChar = *pStr;
        HRESULT hr = GetCharWidth(iChar, &iCharWidth);
        if (FAILED(hr))
        {
            return -1;
        }

        iWidth += iCharWidth;
        pStr++;
    }

    return iWidth;
}

int CTextFont::GetStringHeight(const wchar_t *pStr, unsigned int nLength)
{
    int iHeight = 0;
    int iIndex = 0;
    while (*pStr && ((unsigned int)-1 == nLength || (unsigned int)iIndex < nLength))
    {
        iIndex++;
        int charHeight = 0;
        unsigned int iChar = *pStr;
        HRESULT hr = GetCharHeight(iChar, &charHeight);
        if (FAILED(hr))
        {
            return -1;
        }

        if (charHeight > iHeight);
        {
            iHeight = charHeight;
        }
        pStr++;
    }

    return iHeight;
}

