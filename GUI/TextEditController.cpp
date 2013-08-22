#include "GUI/TextEditController.h"
#include "GUI/CTpFont.h"
#include "GUI/CTpGraphics.h"
#include "GUI/FontManager.h"
#include "GUI/EventSet.h"
#include "Utility/ColorManager.h" 
#include "Utility/RenderUtil.h"
#include "Utility.h"
#include "Utility/EncodeUtil.h"

#include "Public/Base/TPDef.h"

#include "KernelDef.h"

using namespace dk::utility;
using namespace DkFormat;

const char* TextEditController::EventTextEditChange = "TextEditControllerEvent";

TextEditController::TextEditController()
    : mMaxCapicty(1024)
    , m_wstrData(NULL)
    , m_wstrTips(NULL)
    , m_cstrData(NULL)
{
    Init();
}

TextEditController::~TextEditController()
{
    UnInit();
}

bool TextEditController::Init()
{
    if (m_lineInfos.size()>0 || m_lineTipsInfos.size()>0 || m_wstrData || m_wstrTips || m_cstrData)
    {
        UnInit();
    }

    m_lineSpace = 1;
    m_fontSize = 26;
    m_fontColor = ColorManager::knBlack;
    m_backGroundColor = ColorManager::knWhite;
    m_maxDataLen = mMaxCapicty;
    m_dataLen = 0;
    m_pageWidth = 0;
    m_pageHeight = 0;
    m_LineCountsPerPage = 0;
    m_startLineOnImage = 0;
    m_endLineOnImage = 0;
    m_needRender = false;
    m_needCursorPosCalculated = false;
    m_showCursor = true;
    m_cursorPageIndex = 0;
    m_cursorLineIndex = 0;
    m_cursorIndexInLine = 0;
    m_currentEditIndex = 0;

    m_wstrData = new wchar_t[mMaxCapicty + 1];
    m_wstrTips = new wchar_t[mMaxCapicty + 1];
    if (m_wstrData==NULL || m_wstrData==NULL) 
        return false;
    memset(m_wstrData, 0, sizeof(m_wstrData[0])*(mMaxCapicty + 1));
    memset(m_wstrTips, 0, sizeof(m_wstrTips[0])*(mMaxCapicty + 1));

    m_needRenderTips = false;

    return true;
}

void TextEditController::UnInit()
{
    m_lineInfos.clear();
    m_lineTipsInfos.clear();
    SafeDeleteArrayPointer(m_wstrData);
    SafeDeleteArrayPointer(m_wstrTips);
    SafeDeleteArrayPointer(m_cstrData);
}



int TextEditController::wstrinsert(wchar_t* szDest,int nIndex,int nDestArraySize,const wchar_t* strSrc,int nLen)
{
    if (szDest==NULL || strSrc==NULL || nIndex<0 || nIndex>=nDestArraySize || nLen==0)
    {
        return 0;
    }

    int i = 0;
    int strLen = wcslen(szDest);

    int endCharIndex = strLen + nLen - 1;
    if (endCharIndex >= nDestArraySize)
        endCharIndex = nDestArraySize - 1;

    for (i = endCharIndex; i >= nIndex+nLen; --i)
        szDest[i] = szDest[i-nLen];

    for (i = 0; i < nLen && nIndex+i < nDestArraySize; ++i)
        szDest[nIndex+i] = strSrc[i];
    return i;
}

int TextEditController::wstrremove(wchar_t* szDest, int nIndex, int nCount)
{
    if (szDest==NULL || nIndex<0 || nCount<0)
    {
        return 0;
    }
    int i=0;
    int iLen = wcslen(szDest);
    if (nIndex > iLen)
    {
        return 0;
    }

    for(i = nIndex ; i <= iLen-nCount; i++)//已经拷贝 \0
    {
        szDest[i] = szDest[i+nCount];
    }
    return nCount;
}

void TextEditController::SetModifyFlag(bool status)
{
    m_needRender = status;
}

void TextEditController::SetTipsNeedRenderFlag(bool flag)
{
    m_needRenderTips = flag;
}

void TextEditController::SetPageWidth(int w)
{
    if (m_pageWidth != w)
    {
        m_pageWidth = w;
        SetModifyFlag(true);
        SetTipsNeedRenderFlag(true);
    }
}

void TextEditController::SetPageHeight(int h)
{
    if (m_pageHeight != h)
    {
        m_pageHeight = h;
        CalLineCountPerPage();
        SetModifyFlag(true);
        SetTipsNeedRenderFlag(true);
    }
}

void TextEditController::SetLineSpace(int linespace)
{
    if (m_lineSpace != linespace)
    {
        m_lineSpace = linespace;
        CalLineCountPerPage();
        SetModifyFlag(true);
    }
}

void TextEditController::SetFontSize(int fontsize)
{
    if (m_fontSize != fontsize)
    {
        m_fontSize = fontsize;
        CalLineCountPerPage();
        SetModifyFlag(true);
    }
}

void TextEditController::SetFontColor(int fontColor)
{
    if (m_fontColor != fontColor)
    {
        m_fontColor = fontColor;
        CalLineCountPerPage();
        SetModifyFlag(true);
        SetTipsNeedRenderFlag(true);
    }
}

void TextEditController::SetBackGroundColor(int backGroundColor)
{
    if (m_backGroundColor != backGroundColor)
    {
        m_backGroundColor = backGroundColor;
    }
}

ITpFont* TextEditController::GetCurFont() const
{
    DKFontAttributes fontAttr;
    fontAttr.m_iSize = m_fontSize;
    return FontManager::GetInstance()->GetFont(fontAttr, m_fontColor);
}

bool TextEditController::CalcRenderInfos()
{
    if (!m_needRender)
    {
        return true;
    }

    if (m_pageWidth<=0 || m_pageHeight<=0)
    {
        return false;
    }
    SetModifyFlag(false);
    //assert(m_pageWidth>0&&m_pageHeight>0);

    int maxTextWidth = m_pageWidth;
    int curStartCharIndex = 0;
    int usedWidth = 0;
    std::vector<unsigned int> cursorPos;
    cursorPos.push_back(usedWidth);
    int dataLen = m_dataLen;
    //unsigned int dataLen = wstrlen(m_wstrData);
    m_lineInfos.clear();
    int i = 0;

    ITpFont* pFont = GetCurFont();
    if (NULL == pFont)
    {
        return false;
    }

    if (dataLen > 0)
    {
        //给定宽度过小，阈值为第一个字符的宽度*2。
        if (maxTextWidth < (pFont->StringWidth(m_wstrData, 1) << 1))
        {
            return false;
        }
    }
    for (i = 0; i < dataLen; ++i)
    {
        if ((m_wstrData[i] == L'\n' && ((i > 0 && m_wstrData[i-1] != L'\r') || i == 0))
        ||  (m_wstrData[i] == L'\r'))
        {
            //换行符，换行
            m_lineInfos.push_back(TextLineInfo(curStartCharIndex, i, cursorPos));
            curStartCharIndex = i+1;
            cursorPos.clear();
            usedWidth = 0;
            cursorPos.push_back(usedWidth);
        }
        else if (m_wstrData[i] == L'\n')
        {
            //\r\n时\r已经做过处理，不再处理
            curStartCharIndex = i+1;
        }
        else 
        {
            usedWidth += pFont->StringWidth((m_wstrData+i), 1);
            if (usedWidth < maxTextWidth)
            {
                cursorPos.push_back(usedWidth);
            }
            else
            {
                //到达最大宽度，换行
                cursorPos.pop_back();
                m_lineInfos.push_back(TextLineInfo(curStartCharIndex, i, cursorPos));
                curStartCharIndex = i;
                i--;
                usedWidth = 0;
                cursorPos.clear();
                cursorPos.push_back(usedWidth);
            }
        }
    }

    //最后一个字符,写入信息
    m_lineInfos.push_back(TextLineInfo(curStartCharIndex, dataLen, cursorPos));

    //如果计算光标位置失败，需要重新计算一遍
    if (m_needCursorPosCalculated)
    {
        CalCursorPosition();
    }

    return true;
}

bool TextEditController::RenderLineRange(DK_IMAGE& image, int startLine, int endLine)
{
    CTpGraphics grf(image);
    ITpFont* pFont = GetCurFont();
    if (NULL == pFont)
    {
        return false;
    }

    int fontHeight = pFont->GetHeight();
    
    wchar_t wtemp[256] = {0};
    int top = 0;
    //HRESULT hr;
    for (int i = startLine; i < endLine; ++i)
    {
        int lineStrLen = (m_lineInfos[i].endCharIndex - m_lineInfos[i].startCharIndex);
        if (lineStrLen > 0)
        {
            memset(wtemp, 0, 256*sizeof(wtemp[0]));
            memcpy(wtemp, m_wstrData + m_lineInfos[i].startCharIndex, lineStrLen*sizeof(wtemp[0]));
            grf.DrawStringUnicode(wtemp, 0, top, pFont, false);
        }

        top += (fontHeight + m_lineSpace);
    }

    if (m_showCursor && m_cursorLineIndex>=0 && m_cursorIndexInLine>=0)
    {
        //printf("m_cursorLineIndex: %d, startLine: %d, endLine: %d, m_cursorIndexInLine: %d, editIndex: %d\n", 
        //    m_cursorLineIndex, startLine, endLine, m_cursorIndexInLine, m_currentEditIndex);
        if (m_LineCountsPerPage > 0 && (m_cursorLineIndex>=startLine) &&(m_cursorLineIndex<endLine)
            && (unsigned int)m_cursorLineIndex < m_lineInfos.size() && (unsigned int)m_cursorIndexInLine < m_lineInfos[m_cursorLineIndex].cursorPosition.size())
        {
            top = (m_cursorLineIndex - startLine)*(fontHeight + m_lineSpace);
            //printf("Draw Cursor: %d,%d\n", m_lineInfos[m_cursorLineIndex].cursorPosition[m_cursorIndexInLine], top);
            grf.DrawLine(m_lineInfos[m_cursorLineIndex].cursorPosition[m_cursorIndexInLine], top, 2, fontHeight, SOLID_STROKE);
        }
    }

    return true;
}

bool TextEditController::CalLineRangeByPageIndex(int pageIndex, int& startLine, int& endLine)
{
    bool result = CalcRenderInfos();
    if (!result)
        return false;

    int totalPages = GetTotalPages();
    if (pageIndex >= totalPages || pageIndex < 0)
    {
        return false;
    }

    startLine = pageIndex*m_LineCountsPerPage;
    endLine = (pageIndex+1)*m_LineCountsPerPage;
    if (pageIndex == totalPages-1)
    {
        endLine = m_lineInfos.size();
        startLine = endLine - m_LineCountsPerPage;
        startLine = startLine<0 ? 0 : startLine;
    }
    return true;
}

bool TextEditController::Render(DK_IMAGE& image, int pageIndex)
{
    int startLine = 0, endLine = 0;
    if (CalLineRangeByPageIndex(pageIndex, startLine, endLine))
    {
         if (RenderLineRange(image, startLine, endLine))
         {
             if (pageIndex == 0)
             {
                 RenderTips(image);
             }
             return true;
         }
    }
    return false;
}

bool TextEditController::RenderTips(DK_IMAGE& image)
{
    if (m_dataLen == 0 && m_wstrTips)
    {
        CTpGraphics grf(image);
        DKFontAttributes fontAttr;
        fontAttr.m_iSize = m_fontSize;
        ITpFont* pFont = FontManager::GetInstance()->GetFont(fontAttr, ColorManager::GetColor(COLOR_SCROLLBAR_BACK));
        if (m_needRenderTips)
        {
            int fontHeight = pFont->GetHeight();
            int usedWidth = 0;
            int tipsLen = wcslen(m_wstrTips);
            int lineCountPerPage = CalLineCountPerPage();
            m_lineTipsInfos.clear();
            int i = 0;
            int curStartCharIndex = 0;
            std::vector<unsigned int> cursorPos;
            int maxTextWidth = m_pageWidth;

            for (i = 0; i < tipsLen; ++i)
            {
                if (m_wstrTips[i] == L'\n')
                {
                    //换行符，换行
                    m_lineTipsInfos.push_back(TextLineInfo(curStartCharIndex, i, cursorPos));
                    curStartCharIndex = i+1;
                    usedWidth = 0;
                    if ((int)m_lineTipsInfos.size() == lineCountPerPage - 1)
                        break;
                }
                else
                {
                    usedWidth += pFont->StringWidth(m_wstrTips+i, 1);
                    if (usedWidth > maxTextWidth)
                    {
                        //到达最大宽度，换行
                        m_lineTipsInfos.push_back(TextLineInfo(curStartCharIndex, i, cursorPos));
                        curStartCharIndex = i;
                        i--;
                        usedWidth = 0;
                        if ((int)m_lineTipsInfos.size() == lineCountPerPage - 1)
                            break;
                    }
                }
            }
            //最后一个字符,写入信息
            m_lineTipsInfos.push_back(TextLineInfo(curStartCharIndex, tipsLen, cursorPos));

            wchar_t wtemp[256] = {0};
            pFont = FontManager::GetInstance()->GetFont(fontAttr, ColorManager::GetColor(COLOR_SCROLLBAR_BACK));
            for (size_t i = 0; i<m_lineTipsInfos.size(); ++i)
            {
                int lineStrLen = (m_lineTipsInfos[i].endCharIndex - m_lineTipsInfos[i].startCharIndex);
                if (lineStrLen > 0)
                {
                    memset(wtemp, 0, 256*sizeof(wtemp[0]));
                    memcpy(wtemp, m_wstrTips + m_lineTipsInfos[i].startCharIndex, lineStrLen*sizeof(wtemp[0]));
                    grf.DrawStringUnicode(wtemp, 0, (fontHeight + m_lineSpace)*i, pFont, ((int)i==lineCountPerPage-1));
                }

            }

        }
    }
    return true;
}

int TextEditController::CalLineCountPerPage()
{
    ITpFont* pFont = GetCurFont();
    if (NULL == pFont)
    {
        return -1;
    }

    int fontHeight = pFont->GetHeight();

    int lineCount = 0;
    if (m_pageHeight>0 && m_fontSize>0 && m_lineSpace>0)
    {
        int usedHeight = fontHeight;
        while (usedHeight<=m_pageHeight)
        {
            lineCount++;
            usedHeight += (m_lineSpace + fontHeight);
        }
    }

    m_LineCountsPerPage = lineCount;
    return lineCount;
}

int TextEditController::GetTotalPages()
{
    CalcRenderInfos();

    int lineCounts = CalLineCountPerPage();
    if (lineCounts > 0)
    {
        int totalPages = (m_lineInfos.size() + lineCounts -1 )/lineCounts;
        return totalPages > 0 ? totalPages : 1;
    }
    return 1;
}

int TextEditController::GetTotalLines() 
{
    CalcRenderInfos();
    return m_lineInfos.size();
}

int TextEditController::GetMaxDataLen() const
{
    return m_maxDataLen;
}

int TextEditController::GetMaxCapicty() const
{
    return mMaxCapicty;
}

int TextEditController::GetCurDataLen() const
{
    return wcslen(m_wstrData);
}

int TextEditController::GetCursorPageIndex() const
{
    return m_cursorPageIndex;
}

bool TextEditController::MoveCursor(CursorMoveDirection direction)
{
    switch (direction)
    {
    case MOVE_LEFT:
        return CursorMoveLeft();
    case MOVE_RIGHT:
        return CursorMoveRight();
    case MOVE_UP:
        return CursorMoveUp();
    case MOVE_DOWN:
        return CursorMoveDown();
    }
    return false;
}

bool TextEditController::CursorMoveLeft()
{
    if (m_currentEditIndex>0)
    {
        SetCurrentEditIndex(--m_currentEditIndex);
        return true;
    }
    else
    {
        return false;
    }
}

bool TextEditController::CursorMoveRight()
{
    if (m_currentEditIndex<m_dataLen)
    {
        SetCurrentEditIndex(++m_currentEditIndex);
        return true;
    }
    else
    {
        return false;
    }
}

int TextEditController::GetClosedCursorIndexInLine(int line, int x)
{
    if (line<0 || (unsigned int)line>=m_lineInfos.size() || x<0)
    {
        return -1;
    }

    int closedCursorIndex;
    const std::vector<unsigned>& cursorPos = m_lineInfos[line].cursorPosition;
    unsigned int cursorLen = cursorPos.size();
    unsigned int i= 0;
    for (;i < cursorLen-1; ++i)
    {
        if (x>=(int)cursorPos[i] && x<=(int)cursorPos[i+1])
            break;
    }
    if (i == cursorLen-1)
    {
        closedCursorIndex = cursorPos.size() - 1;
    }
    else
    {
        closedCursorIndex = i;
        if ((x-cursorPos[i]) > (cursorPos[i+1] - x))
            closedCursorIndex++;
    }

    return closedCursorIndex;
}

bool TextEditController::CursorMoveUp()
{
    if (m_cursorLineIndex>0 && (unsigned int)m_cursorLineIndex<m_lineInfos.size())
    {
        int coorx = m_lineInfos[m_cursorLineIndex].cursorPosition[m_cursorIndexInLine];
        m_cursorLineIndex--;
        int closeUPIndexInLine = GetClosedCursorIndexInLine(m_cursorLineIndex, coorx);
        SetCurrentEditIndex(m_lineInfos[m_cursorLineIndex].startCharIndex + closeUPIndexInLine);
        return true;
    }
    else
    {
        return false;
    }
}

bool TextEditController::CursorMoveDown()
{
    if (m_cursorLineIndex >= 0 && (unsigned int)m_cursorLineIndex < m_lineInfos.size()-1)
    {
        int coorx = m_lineInfos[m_cursorLineIndex].cursorPosition[m_cursorIndexInLine];
        m_cursorLineIndex++;
        int closeDownIndexInLine = GetClosedCursorIndexInLine(m_cursorLineIndex, coorx);
        SetCurrentEditIndex(m_lineInfos[m_cursorLineIndex].startCharIndex + closeDownIndexInLine);
        return true;
    }
    else
    {
        return false;
    }
}


void TextEditController::SetCurrentEditIndex(int curEditIndex)
{
    int cursorPageStore = m_cursorPageIndex;

    m_currentEditIndex = curEditIndex;

    if (CalCursorPosition())
    {
        if (cursorPageStore!=m_cursorPageIndex)
        {
            TextEditChangeEventArgs args(eTextEditCurPageChanged);
            FireEvent(EventTextEditChange, args);
        }
    }
}

int TextEditController::SetTip(const wchar_t *strText,int iStrLen)
{
    if (iStrLen <= 0)
    {
        iStrLen = wcslen(strText);
    }

    memcpy(m_wstrTips, strText, iStrLen * sizeof(m_wstrTips[0]));
    m_wstrTips[iStrLen+1] = L'\0';
    return 1;
}

int TextEditController::SetTipUTF8(const char* strText,int iStrLen)
{
    if(0 == iStrLen)
    {
        iStrLen = strlen(strText);
    }
    std::string source(strText, iStrLen);
    std::wstring wText = EncodeUtil::ToWString(source.c_str());
    int toCopy = wText.size() < mMaxCapicty ? wText.size() : mMaxCapicty;
    memcpy(m_wstrTips, wText.c_str(), (toCopy + 1) * sizeof(m_wstrTips[0]));
    return 1;
}
int TextEditController::SetText(const wchar_t *strText,int iStrLen)
{
    if(0 == iStrLen)
    {
        iStrLen = wcslen(strText);
    }

    if(iStrLen >= m_maxDataLen)//需要等号  把末位结束符0留下来
    {
        return 0;
    }
    else
    {
        if(iStrLen > m_maxDataLen)
        {
            iStrLen = m_maxDataLen;
        }
        memcpy(m_wstrData , strText, iStrLen * sizeof(m_wstrData[0]));
        m_wstrData[iStrLen] = L'\0';
        m_dataLen = iStrLen;
        SetModifyFlag(true);
        SetCurrentEditIndex(iStrLen);
        if (CalcRenderInfos())
        {
            return 1;
        }
    }
    return 0;
}

int TextEditController::SetTextUTF8(LPCSTR strText,int iStrLen)
{
    if(0 == iStrLen)
    {
        iStrLen = strlen(strText);
    }
    std::wstring wText = EncodeUtil::ToWString(
            std::string(strText, iStrLen).c_str());
    return SetText(wText.c_str(), wText.size());
}

std::wstring TextEditController::GetText() const
{
    return m_wstrData;
}

std::string TextEditController::GetTextUTF8() const
{
    return EncodeUtil::ToString(m_wstrData);
}

int TextEditController::InsertText(const wchar_t *strText,int iStrLen)
{
    int insertLen = wstrinsert(m_wstrData, m_currentEditIndex, m_maxDataLen, strText, iStrLen);
    m_currentEditIndex += insertLen;
    m_dataLen += insertLen;
    m_wstrData[m_dataLen] = L'\0';
    SetModifyFlag(true);
    SetCurrentEditIndex(m_currentEditIndex);
    return insertLen;
}

int TextEditController::DeleteChar()
{
    int removeLen = 0;
    if (m_dataLen>0 && m_currentEditIndex>0)
    {
        removeLen = wstrremove(m_wstrData, m_currentEditIndex-1, 1);
        m_currentEditIndex -= removeLen;
        m_dataLen -= removeLen;
        m_wstrData[m_dataLen] = L'\0';
        SetModifyFlag(true);
        SetCurrentEditIndex(m_currentEditIndex);
    }
    return removeLen;
}

int TextEditController::InsertTextUTF8(LPCSTR strText,int iStrLen)
{
    if(0 == iStrLen)
    {
        iStrLen = strlen(strText);
    }
    std::wstring wText = EncodeUtil::ToWString(
            std::string(strText, iStrLen).c_str());
    return InsertText(wText.c_str(), wText.size());
}

int TextEditController::ClearText()
{
    int dataLenStore = m_dataLen;
    m_wstrData[0] = L'\0';
    m_currentEditIndex = 0;
    m_dataLen = 0;
    SetModifyFlag(true);
    SetCurrentEditIndex(m_currentEditIndex);
    return dataLenStore;
}

void TextEditController::SetMaxDataLen(int iMaxLen)
{
    if (iMaxLen > 0)
    {
        m_maxDataLen = iMaxLen;
    }
}

int TextEditController::HitCursorPosition(int x, int y, int curPage)
{
    int startLine = 0, endLine = 0;
    ITpFont* pFont = GetCurFont();
    if (NULL == pFont)
    {
        return -1;
    }

    if (CalLineRangeByPageIndex(curPage, startLine, endLine))
    {
        int lineIndex = startLine;
        lineIndex += y/(pFont->GetHeight() + m_lineSpace);
        if (lineIndex<0 || (unsigned int)lineIndex>=m_lineInfos.size())
        {
            return -1;
        }

        m_currentEditIndex = m_lineInfos[lineIndex].startCharIndex + GetClosedCursorIndexInLine(lineIndex, x);
        //TODO,这里应该是不需要判断了
        //if (m_currentEditIndex>m_dataLen)
        //  m_currentEditIndex = m_dataLen;

        CalCursorPosition();
        return m_currentEditIndex;
    }
    return -1;
}

bool TextEditController::CalCursorPosition()
{
    if (CalcRenderInfos())
    {
        m_cursorLineIndex = 0;
        m_cursorIndexInLine = 0;
        for (unsigned int i = 0; i < m_lineInfos.size(); ++i)
        {
            int startChar = m_lineInfos[i].startCharIndex;
            int endChar = m_lineInfos[i].startCharIndex + m_lineInfos[i].cursorPosition.size() - 1;
            //printf("cursorpositon: i:%d, startChar: %d, endChar: %d, editindex: %d\n", i, startChar, endChar, m_currentEditIndex);
            if (m_currentEditIndex>=startChar && m_currentEditIndex<=endChar)
            {
                m_cursorLineIndex = i;
                m_cursorIndexInLine = m_currentEditIndex - m_lineInfos[i].startCharIndex;
                break;
            }
        }
        if (m_LineCountsPerPage>0)
        {
            m_cursorPageIndex = m_cursorLineIndex/m_LineCountsPerPage;
        }
        //printf("page: %d, line: %d, index: %d\n", m_cursorPageIndex, m_cursorLineIndex, m_cursorIndexInLine);
        m_needCursorPosCalculated = false;
        return true;
    }

    m_needCursorPosCalculated = true;
    return false;

}

bool TextEditController::MoveCursorToPageStart(int page)
{
    if (CalcRenderInfos())
    {
        int startLine = 0, endLine = 0;
        CalLineRangeByPageIndex(page, startLine, endLine);
        m_currentEditIndex = m_lineInfos[startLine].startCharIndex;
        CalCursorPosition();
    }
    return false;
}

void TextEditController::SetShowCursor(bool show)
{
    m_showCursor = show;
}

bool TextEditController::IsCursorShowing() const
{
    return m_showCursor;
}

int TextEditController::GetMinHeight() const
{
    ITpFont* pFont = GetCurFont();
    if (pFont)
    {
        return pFont->GetHeight();
    }

    return -1;
}
