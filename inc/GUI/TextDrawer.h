#ifndef __GUI_TEXTDRAWER_H__
#define __GUI_TEXTDRAWER_H__

#include "dkImage.h"
#include "IDKTRender.h"
#include "Public/Base/TPDef.h"
#include <vector>
#include <stdint.h>

using DkFormat::DK_IMAGE;

class TextDrawer
{
public:
    enum Mode
    {
        // 定行数，高度由排版结果确定
        TDM_FIXED_LINES, //只有宽度和行数有效
        // 定高
        TDM_FIXED_HEIGHT, // 只有高度有效
        // 无限高,用于大数据分页显示
        TDM_UNLIMITED_HEIGHT

    };
    TextDrawer();
    ~TextDrawer();
    void SetMaxWidth(int maxWidth);
    void SetMaxHeight(int maxHeight);
    void SetText(const char* text);
    void SetMode(Mode mode);
    void SetMaxLines(int maxLines);
    void SetEndWithEllipsis(bool endWithEllipsis);
    void SetAnsiFontFile(const char* ansiFontFile);
    void SetGbFontFile(const char* ansiFontFile);
    void SetFontSize(double fontSize);
    void SetAlign(int align);
    void SetVAlign(int valign);
    void SetFirstLineIndent(double firstLineIndent);
    void SetTabStop(double tabStop);
    void SetPageHeight(int pageHeight);
	void SetLineGap(double lineGap);
    void SetTextColor(const DK_ARGBCOLOR& color);
    void SetBackgroundColor(const DK_ARGBCOLOR& color);
	void SetStrikeThrough(bool strikeThrough);
	void SetUnderLine(bool underLine);
	void SetHighlightIndexRange(int startPos, int endPos);
    void SetUseTraditionalChinese(bool useTraditionalChinese);

	const char* GetText() const;
	Mode GetMode() const;
    int GetPageHeight() const;//每一页的设定高度
    bool GetEndWithEllipsis() const;
    int GetMaxWidth() const;
	int GetTextHeightOfPageIndex(int pageIndex);//每一页文字实际占用的高度
	const DK_ARGBCOLOR& GetBackgroundColor() const;
    bool GetTraditionalChineseUsed() const;
    unsigned int GetTextLength() const
    {
        return m_text.size();
    }

    bool GetBoundingBox(int* width, int* height);
	bool GetTextBoundingBox(int* width, int*height);//得到的是纯文字的的矩形，不包括可能的空白(例如居右时产生的空白)
    // 无限高计算页数
    size_t GetTotalPages();
    bool RenderPage(int pageIndex, DK_IMAGE image, int dstX, int dstY, int dstWidth, int dstHeight);
    bool Render(DK_IMAGE image, int dstX, int dstY, int dstWidth, int dstHeight);
    // 首行缩进，以字号为单位
private:
    void CopyImgage32ToImage8(DK_IMAGE dstImg8, DK_IMAGE srcImg32);
    void ClearRenderInfo();
    void SetModifiedFlag();
    void ClearModifiedFlag();
    bool IsModified();
    bool CalcRenderInfo();
    bool CalcRenderInfoForFixedLines();
    bool CalcRenderInfoForFixedHeight();
    bool CalcRenderInfoForUnlimitedHeight();
    bool CalcRenderInfoForHeight(int height);
	/**
	 * @brief GetRenderCharInfoByIndex 由字符index得到m_pcharinfos的index
	 *
	 * @param charIndex 字符index
	 *
	 * @return 返回在m_pcharinfos的index
	 */
	IDKTRender::RenderCharInfo* GetRenderCharInfoByIndex(unsigned int charIndex);
	/**
	 * @brief RenderHighlight 高亮关键字
	 *
	 * @param image 原图
	 * @param from 高亮关键字的开始索引
	 * @param to 高亮关键字的末尾索引(不包括该索引)
	 * @param baseX 高亮矩形计算所需的偏移量x
	 * @param baseY  高亮矩形计算所需的偏移量y
	 */
	void RenderHighlight(DK_IMAGE image, int from, int to, int baseX = 0, int baseY = 0);
	/**
	 * @brief RenderUnderLine 下划线绘制
	 *
	 * @param DK_IMAGE 原图
	 * @param fromline 开始行
	 * @param toline 结束行
	 */
	void RenderUnderLine(DK_IMAGE, int fromline, int toline);
	/**
	 * @brief RenderStrikeThrough 中划线绘制
	 *
	 * @param DK_IMAGE 原图
	 * @param fromline 开始行
	 * @param toline 结束行
	 */
	void RenderStrikeThrough(DK_IMAGE, int fromline, int toline);
    IDKTRender::LayoutOption BuildLayoutOption();

    TextDrawer(const TextDrawer&);
    TextDrawer& operator=(const TextDrawer&);
    int m_maxWidth;
    int m_maxHeight;
    int m_maxLines;
    bool m_endWithEllipsis;
    bool m_useTraditionalChinese;//traditional chinese or simple chinese
    Mode m_mode;
    static IDKTRender* s_render;
    IDKTRender::RenderCharInfo* m_pCharInfos;
    DK_UINT m_charInfoCount;
    IDKTRender::RenderLineInfo* m_pLineInfos;
    DK_UINT m_lineInfoCount;
    bool m_modified;
    std::string m_text;
    std::wstring m_ansiFontFile;
    std::wstring m_gbFontFile;
    double m_fontSize;
    int m_align;
    int m_valign;
    double m_firstLineIndent;
    double m_tabStop;
    DK_BOOL m_followAfter;
    bool m_renderSucceeded;
    int m_pageHeight;//只在分页时有效
    std::vector<int> m_pageStartLines;
    DK_ARGBCOLOR m_textColor;
    DK_ARGBCOLOR m_backgroundColor;
	bool m_strikeThrough;
	bool m_underLine;
	int m_highlightStartPos;
	int m_highlightEndPos;
	double m_lineGap;
    int m_cachedTextBoxWidth;
    int m_cachedTextBoxHeight;
};
#endif

