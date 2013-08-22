#ifndef TEXTEDITCONTROLLER_H
#define TEXTEDITCONTROLLER_H

#include <vector>

#include "GUI/EventArgs.h"
#include "GUI/EventSet.h"
#include "GUI/ITpFont.h"
#include "dkImage.h"

using DkFormat::DK_IMAGE;

enum TextEditChangeTypes
{
    eTextEditCurPageChanged,
    eTextEditTotalPageChanged,
    eTextEditContentChanged
};

class TextEditChangeEventArgs : public EventArgs
{
public:
    TextEditChangeEventArgs()
        : textEditChangeType(eTextEditCurPageChanged)
        , pSender(NULL)
    {}

    TextEditChangeEventArgs(TextEditChangeTypes tece, void* sender = NULL)
        : textEditChangeType(tece)
        , pSender(sender)
    {}

    virtual EventArgs* Clone() const
    {
        return new TextEditChangeEventArgs(*this);
    }

    TextEditChangeTypes textEditChangeType;
    void* pSender;
};

class TextEditController : public EventSet 
{

public:
    struct TextLineInfo
    {
        TextLineInfo(int start, int end, const std::vector<unsigned int>& cursorPos)
            : startCharIndex(start)
            , endCharIndex(end)
            , cursorPosition(cursorPos)
        {}

        int startCharIndex;
        int endCharIndex;
        std::vector<unsigned int> cursorPosition;
    };//TextLineInfo

    enum CursorMoveDirection
    {
        MOVE_LEFT,
        MOVE_RIGHT,
        MOVE_UP,
        MOVE_DOWN
    };

    static const char* EventTextEditChange;

    TextEditController();
    ~TextEditController();

    /*获取当前的总页数   */
    int GetTotalPages();
    /*获取当前的总行数   */
    int GetTotalLines();
    /*获取最大字符数   */
    int GetMaxDataLen() const;
    /*获取最大容量   */
    int GetMaxCapicty() const;
    /*获取当前字符数   */
    int GetCurDataLen() const;
    /*获取光标所在页码   */
    int GetCursorPageIndex() const;
    /*是否显示光标   */
    bool IsCursorShowing() const;

    /*设置页面宽度   */
    void SetPageWidth(int w);
    /*设置页面高度   */
    void SetPageHeight(int h);
    /*设置页面行间距   */
    void SetLineSpace(int linespace);
    /*设置字体大小   */
    void SetFontSize(int fontsize);
    /*设置字体颜色   */
    void SetFontColor(int fontColor);
    /*设置背景颜色   */
    void SetBackGroundColor(int backGroundColor);
    /*设置是否显示光标   */
    void SetShowCursor(bool show);

    /*设置当前编辑位置   */
    void SetCurrentEditIndex(int);

    /**
     * @brief Render 根据当前文字渲染页面
     *
     * @param image 源图片
     * @param pageIndex 所要渲染的页码
     *
     * @return 渲染结果,如pageIndex不合法则返回false
     */
    bool Render(DK_IMAGE& image, int pageIndex);

    /*记录是否需要再次渲染文字   */
    void SetModifyFlag(bool);
    /*记录是否需要再次渲染tips   */
    void SetTipsNeedRenderFlag(bool);

    /**
     * @brief CalcRenderInfos 根据当前文字计算渲染的结果选项，存储在m_lineInfos
     *
     * @return 计算结果
     */
    bool CalcRenderInfos();

    /**
     * @brief HitCursorPosition 根据点击更新光标位置
     *
     * @param x 点击的x坐标
     * @param y 点击的y坐标
     * @param curPage 当前页码
     *
     * @return 返回更新后的光标index
     */
    int HitCursorPosition(int x, int y, int curPage);
    /**
     * @brief MoveCursor 移动光标
     *
     * @param direction 移动方向
     *
     * @return 光标是否移动
     */
    bool MoveCursor(CursorMoveDirection direction);

	//内容修改
    virtual int SetTip(const wchar_t *strText, int iStrLen = 0);   //设置提示字符
    virtual int SetTipUTF8(LPCSTR strText, int iStrLen = 0);

    virtual int SetText(const wchar_t *strText, int iStrLen = 0);   //设置字符
    virtual int SetTextUTF8(LPCSTR strText, int iStrLen = 0);

    virtual std::wstring GetText() const;  //取得字符
    virtual std::string GetTextUTF8() const;
    virtual int InsertText(const wchar_t* strText, int iStrLen = 0);   //插入字符
    virtual int InsertTextUTF8(const char* strText, int iStrLen = 0);

    virtual int DeleteChar();
    virtual int ClearText();                                                //清除字符
    virtual void SetMaxDataLen(int iMaxLen);

    bool MoveCursorToPageStart(int page);
    int GetMinHeight() const;

private:
    bool Init();
    void UnInit();

    /*渲染在startLine,endLine的范围内的文字,[)区间   */
    bool RenderLineRange(DK_IMAGE& image, int startLine, int endLine);
    /*渲染提示   */
    bool RenderTips(DK_IMAGE& image);

	//TODO
	int wstrinsert(wchar_t* szDest, int nIndex, int nDestArraySize, const wchar_t* strSrc, int nLen);
	int wstrremove(wchar_t* szDest, int nIndex, int nCount);

    bool CalCursorPosition();//计算光标所在行,光标在所在行的索引值,光标所在页
    bool CalLineRangeByPageIndex(int pageIndex, int&startLine, int& endLine);
    int CalLineCountPerPage();

    ITpFont* GetCurFont() const;

    int GetClosedCursorIndexInLine(int line, int x);
    bool CursorMoveLeft();
    bool CursorMoveRight();
    bool CursorMoveUp();
    bool CursorMoveDown();

private:
    std::vector<TextLineInfo> m_lineInfos;
    //std::vector<unsigned int> mPageInfos;
    int m_lineSpace;
    int m_fontSize;

    int m_fontColor;
    int m_backGroundColor;

    const unsigned int mMaxCapicty;//数组容量
    int m_maxDataLen;//字符最大长度
    int m_dataLen;//字符长度

    int m_pageWidth;
    int m_pageHeight;
    int m_LineCountsPerPage;
    int m_startLineOnImage;//TODO,obsoleting
    int m_endLineOnImage;//TODO,obsoleting

    bool m_needRender;
    bool m_needCursorPosCalculated;
    bool m_showCursor;

    int m_cursorPageIndex;//光标所在的页
    int m_cursorLineIndex;//光标所在的行
    int m_cursorIndexInLine;//光标在该行的索引
    int m_currentEditIndex;//光标的总索引，光标表数目比总的字符数多1

    bool m_needRenderTips;//记录是否需要计算Tips的位置
    std::vector<TextLineInfo> m_lineTipsInfos;//Tips在每行索引的范围[).

    wchar_t* m_wstrData;
    wchar_t* m_wstrTips;
    char* m_cstrData;
};//TextEditController
#endif//TEXTEDITCONTROLLER_H
