#include "GUI/TextDrawer.h"
#include "DKTAPI.h"
#include "Utility/EncodeUtil.h"
#include "Utility/RenderUtil.h"
#include  "BMPProcessor/IBMPProcessor.h"
#include "GUI/CTpGraphics.h"

using namespace dk::utility;

IDKTRender* TextDrawer::s_render = (IDKTRender*)DKT_CreateRender();

TextDrawer::TextDrawer()
    : m_maxWidth(-1)
    , m_maxHeight(-1)
    , m_maxLines(-1)
    , m_endWithEllipsis(false)
    , m_useTraditionalChinese(false)
    , m_mode(TDM_FIXED_LINES)
    , m_pCharInfos(NULL)
    , m_charInfoCount(0)
    , m_pLineInfos(NULL)
    , m_lineInfoCount(0)
    , m_modified(false)
    , m_fontSize(20.0f)
    , m_align(ALIGN_LEFT)
    , m_valign(VALIGN_CENTER)
    , m_firstLineIndent(0)
    , m_tabStop(0)
    , m_renderSucceeded(false)
    , m_pageHeight(-1)
    , m_textColor(0xFF, 0, 0, 0)
    , m_backgroundColor(0xFF, 0xFF, 0xFF, 0xFF)
	, m_strikeThrough(false)
	, m_underLine(false)
	, m_highlightStartPos(-1)
	, m_highlightEndPos(-1)
	, m_lineGap(1.0)
    , m_cachedTextBoxWidth(-1)
    , m_cachedTextBoxHeight(-1)
{
}

TextDrawer::~TextDrawer()
{
    ClearRenderInfo();
    //if (NULL != s_render)
    //{
    //    DKT_ReleaseRender(s_render);
    //}
}

void TextDrawer::ClearRenderInfo()
{
    if (NULL != s_render)
    {
        if (NULL != m_pCharInfos || NULL != m_pLineInfos)
        {
            s_render->ReleaseRenderInfo(m_pCharInfos, m_pLineInfos);
            m_pCharInfos = NULL;
            m_charInfoCount = 0;
            m_pLineInfos = NULL;
            m_lineInfoCount = 0;
        }
    }
    m_pageStartLines.clear();
}

void TextDrawer::SetPageHeight(int pageHeight)
{
    assert(pageHeight > 0);
    if (pageHeight != m_pageHeight)
    {
        m_pageHeight = pageHeight;
        m_pageStartLines.clear();
    }
}
int TextDrawer::GetPageHeight() const
{
	return m_pageHeight;
}

bool TextDrawer::GetEndWithEllipsis() const
{
    return m_endWithEllipsis;
}

int TextDrawer::GetMaxWidth() const
{
    return m_maxWidth;
}

int TextDrawer::GetTextHeightOfPageIndex(int pageIndex)
{
    if (TDM_UNLIMITED_HEIGHT != m_mode)
    {
        return -1;
    }

	unsigned int totalPages = GetTotalPages();

	if (pageIndex < 0 || (unsigned int)pageIndex >= totalPages || totalPages<=0)
	{
		return -1;
	}

	if (pageIndex>=0 && (unsigned int)pageIndex<totalPages)
	{
		int startLineIndex = m_pageStartLines[pageIndex];
		int endLineIndex = ((unsigned int)pageIndex == (totalPages -1 )) ? m_lineInfoCount : (m_pageStartLines[pageIndex + 1]);
		DK_BOX box = m_pLineInfos[startLineIndex].boundingBox;
		for (int i = startLineIndex+1; i < endLineIndex; ++i)
		{
			box = box.MergeBox(m_pLineInfos[i].boundingBox);
		}
		return box.Height() + 1;
	}
    return -1;
}

void TextDrawer::SetLineGap(double lineGap)
{
	assert(lineGap > 0);
	if (m_lineGap != lineGap)
	{
		m_lineGap = lineGap;
		SetModifiedFlag();
	}
}

void TextDrawer::SetTextColor(const DK_ARGBCOLOR& color)
{
    m_textColor = color;
	SetModifiedFlag();
}

void TextDrawer::SetBackgroundColor(const DK_ARGBCOLOR& color)
{
    m_backgroundColor = color;
	SetModifiedFlag();
}

void TextDrawer::SetModifiedFlag()
{
    m_modified = true;
    m_renderSucceeded = false;
    m_cachedTextBoxWidth = -1;
    m_cachedTextBoxHeight = -1;
}

void TextDrawer::ClearModifiedFlag()
{
    m_modified = false;
}

bool TextDrawer::IsModified()
{
    return m_modified;
}

void TextDrawer::SetMaxWidth(int maxWidth)
{
    if (maxWidth != m_maxWidth)
    {
        m_maxWidth = maxWidth;
        SetModifiedFlag();
    }
}

void TextDrawer::SetMaxHeight(int maxHeight)
{
    if (maxHeight != m_maxHeight)
    {
        m_maxHeight = maxHeight;
        SetModifiedFlag();
    }
}

void TextDrawer::SetMaxLines(int maxLines)
{
    if (maxLines != m_maxLines)
    {
        m_maxLines = maxLines;
        SetModifiedFlag();
    }
}

void TextDrawer::SetStrikeThrough(bool strikeThrough)
{
	if (m_strikeThrough != strikeThrough)
	{
		m_strikeThrough = strikeThrough;
		SetModifiedFlag();
	}
}

void TextDrawer::SetUnderLine(bool underLine)
{
	if (m_underLine != underLine)
	{
		m_underLine = underLine;
		SetModifiedFlag();
	}
}

void TextDrawer::SetText(const char* text)
{
    if (text && m_text != text)
    {
        m_text = text;
        //remove the trailing white space if in single-line mode
        if (m_mode == TDM_FIXED_LINES && m_maxLines == 1)
        {
            size_t found = m_text.find_last_not_of(' ');
            if (found != std::string::npos)
                m_text.erase(found + 1);
        }
        SetModifiedFlag();
    }
}

const char* TextDrawer::GetText() const
{
	return m_text.c_str();
}

void TextDrawer::SetMode(Mode mode)
{
    if (m_mode != mode)
    {
        m_mode = mode;
		if (TDM_FIXED_HEIGHT)
		{
			SetMaxLines(10000);
			if (m_maxHeight < 0)
			{
				m_maxHeight = 100000*m_fontSize;
			}
		}
        SetModifiedFlag();
    }
}

TextDrawer::Mode TextDrawer::GetMode() const
{
	return m_mode;
}

void TextDrawer::SetEndWithEllipsis(bool endWithEllipsis)
{
    if (m_endWithEllipsis != endWithEllipsis)
    {
        m_endWithEllipsis = endWithEllipsis;
        SetModifiedFlag();
    }
}

void TextDrawer::SetAnsiFontFile(const char* ansiFontFile)
{
    std::wstring wAnsiFontFile = EncodeUtil::ToWString(ansiFontFile);
    if (m_ansiFontFile != wAnsiFontFile)
    {
        m_ansiFontFile = wAnsiFontFile;
        SetModifiedFlag();
    }
}

void TextDrawer::SetGbFontFile(const char* gbFontFile)
{
    std::wstring wGbFontFile = EncodeUtil::ToWString(gbFontFile);
    if (m_gbFontFile != wGbFontFile)
    {
        m_gbFontFile = wGbFontFile;
        SetModifiedFlag();
    }
}

void TextDrawer::SetFontSize(double fontSize)
{
    if (m_fontSize != fontSize)
    {
        m_fontSize = fontSize;
        SetModifiedFlag();
    }
}

void TextDrawer::SetAlign(int align)
{
    if (m_align != align)
    {
        m_align = align;
        SetModifiedFlag();
    }
}
void TextDrawer::SetVAlign(int valign)
{
    if (m_valign != valign)
    {
        m_valign = valign;
        SetModifiedFlag();
    }
}

IDKTRender::LayoutOption TextDrawer::BuildLayoutOption()
{
    IDKTRender::LayoutOption layoutOption;
    layoutOption.fontSize = m_fontSize;
    layoutOption.lineGap = m_lineGap;
    layoutOption.paraSpacing = 0.2;
    layoutOption.tabStop = m_tabStop;
    layoutOption.indent = m_firstLineIndent;
    switch (m_align)
    {
        case ALIGN_LEFT:
            layoutOption.alignType = IDKTRender::LAYOUT_ALIGN_LEFT;
            break;
        case ALIGN_RIGHT:
            layoutOption.alignType = IDKTRender::LAYOUT_ALIGN_RIGHT;
            break;
        case ALIGN_CENTER:
            layoutOption.alignType = IDKTRender::LAYOUT_ALIGN_CENTER;
            break;
		case ALIGN_JUSTIFY:
            layoutOption.alignType = IDKTRender::LAYOUT_ALIGN_JUSTIFY;
            break;
        default:
            break;
    }
    return layoutOption;
}

void TextDrawer::SetFirstLineIndent(double firstLineIndent)
{
    if (m_firstLineIndent != firstLineIndent)
    {
        m_firstLineIndent = firstLineIndent;
        SetModifiedFlag();
    }
}

void TextDrawer::SetTabStop(double tabStop)
{
    if (m_tabStop != tabStop)
    {
        m_tabStop = tabStop;
        SetModifiedFlag();
    }
}

bool TextDrawer::CalcRenderInfo()
{
    // 无需重新计算
    if (!m_modified)
    {
        return m_renderSucceeded;
    }
    if (s_render == NULL)
    {
        s_render = (IDKTRender*)DKT_CreateRender();
        if (NULL == s_render)
        {
            return false;
        }
    }
	if (m_text.empty())
	{
		return false;
	}

    ClearRenderInfo();
    ClearModifiedFlag();

    if (TDM_FIXED_LINES == m_mode)
    {
        return CalcRenderInfoForFixedLines();
    }
    else if (TDM_FIXED_HEIGHT == m_mode)
    {
        return CalcRenderInfoForFixedHeight();
    }
    else
    {
        assert(TDM_UNLIMITED_HEIGHT == m_mode);
        return CalcRenderInfoForUnlimitedHeight();
    }
}

bool TextDrawer::CalcRenderInfoForFixedLines()
{
    assert(m_maxWidth > 0);
    assert(m_maxLines > 0);
    assert(s_render != NULL);
    assert(m_pCharInfos == NULL);
    assert(m_pLineInfos == NULL);
    DK_AUTO(wText, EncodeUtil::ToWString(m_text));
    DK_AUTO(wAnsiFont, m_ansiFontFile);
    DK_AUTO(wGbFont, m_gbFontFile);
    m_followAfter = false;
    DK_ReturnCode rc = s_render->CreateRenderInfoByLineCount(
        wText.c_str(),
        BuildLayoutOption(),
        wGbFont.c_str(),
        wAnsiFont.c_str(),
        m_maxWidth,
        m_maxLines,
        m_endWithEllipsis,
        &m_followAfter,
        &m_pCharInfos,
        &m_charInfoCount,
        &m_pLineInfos,
        &m_lineInfoCount);
    m_renderSucceeded = (rc == DKR_OK) && m_charInfoCount > 0 && m_lineInfoCount > 0;
    return m_renderSucceeded;
}

bool TextDrawer::CalcRenderInfoForFixedHeight()
{
    assert(m_maxHeight > 0);
    return CalcRenderInfoForHeight(m_maxHeight);
}

bool TextDrawer::CalcRenderInfoForUnlimitedHeight()
{
    return CalcRenderInfoForHeight(m_text.size() * m_fontSize * 10);
}

bool TextDrawer::CalcRenderInfoForHeight(int height)
{
    assert(m_maxWidth > 0);
    assert(s_render != NULL);
    assert(m_pCharInfos == NULL);
    assert(m_pLineInfos == NULL);
    DK_AUTO(wText, EncodeUtil::ToWString(m_text));
    DK_AUTO(wAnsiFont, m_ansiFontFile);
    DK_AUTO(wGbFont, m_gbFontFile);
    m_followAfter = false;
    DK_ReturnCode rc = s_render->CreateRenderInfoByHeight(
        wText.c_str(),
        BuildLayoutOption(),
        wGbFont.c_str(),
        wAnsiFont.c_str(),
        m_maxWidth,
        height,
        m_endWithEllipsis,
        &m_followAfter,
        &m_pCharInfos,
        &m_charInfoCount,
        &m_pLineInfos,
        &m_lineInfoCount);
    m_renderSucceeded = (rc == DKR_OK) && m_charInfoCount > 0 && m_lineInfoCount > 0;
    return m_renderSucceeded;
}

bool TextDrawer::GetTextBoundingBox(int* width, int* height)
{
    if (m_cachedTextBoxWidth != -1 && m_cachedTextBoxHeight != -1)
    {
        *width = m_cachedTextBoxWidth;
        *height = m_cachedTextBoxHeight;
        return true;
    }
	//存储单行模式相关变量，因此目前只为单行模式工作
	//得到的结果即为所有文字恰好排下所需要的空间大小
	int maxWidthStore = m_maxWidth;
	int alignMentStore = m_align;
	if (m_mode == TDM_FIXED_LINES)
	{
        SetMaxWidth(10000);
        SetAlign(ALIGN_LEFT);
	}

	if (!CalcRenderInfo())
	{
		//回复相关变量
		if (m_mode == TDM_FIXED_LINES)
		{
			SetMaxWidth(maxWidthStore);
			SetAlign(alignMentStore);
		}
		return false;
	}
	assert(m_lineInfoCount > 0);
	DK_BOX box = m_pLineInfos[0].boundingBox;
	for (size_t i = 1; i < m_lineInfoCount; ++i)
	{
		box = box.MergeBox(m_pLineInfos[i].boundingBox);
	}
	*width = box.Width() + 1;
	*height = box.Height() + 1;

	//回复相关变量
	if (m_mode == TDM_FIXED_LINES)
	{
		SetMaxWidth(maxWidthStore);
		SetAlign(alignMentStore);
	}
    m_cachedTextBoxWidth = *width;
    m_cachedTextBoxHeight = *height;
	return true;
}

bool TextDrawer::GetBoundingBox(int* width, int* height)
{
    if (!CalcRenderInfo())
    {
        return false;
    }
    assert(m_lineInfoCount > 0);
    //DK_BOX box = m_pLineInfos[0].boundingBox;
    //for (size_t i = 1; i < m_lineInfoCount; ++i)
    //{
    //    box = box.MergeBox(m_pLineInfos[i].boundingBox);
    //}
    //*width = box.X1 + 1;
    //*height = box.Y1 + 1;

    *width = 0;
    *height = 0;
    for ( size_t i = 0; i < m_lineInfoCount - 1; ++i)
    {
        unsigned int endCharIndex = m_pLineInfos[i+1].startIndex;
        IDKTRender::RenderCharInfo* pEndCharInfo = GetRenderCharInfoByIndex(endCharIndex - 1);
        if (pEndCharInfo)
        {
            *width = *width > pEndCharInfo->boundingBox.X1 ? *width : pEndCharInfo->boundingBox.X1;
            *height = *height > pEndCharInfo->boundingBox.Y1 ? *height : pEndCharInfo->boundingBox.Y1;
        }
    }
    IDKTRender::RenderCharInfo pEndCharInfo = m_pCharInfos[m_charInfoCount-1];
    *width = *width > pEndCharInfo.boundingBox.X1 ? *width : pEndCharInfo.boundingBox.X1;
    *height = *height > pEndCharInfo.boundingBox.Y1 ? *height : pEndCharInfo.boundingBox.Y1;

    *width += 1;
    *height += 1;
    return true;
}

size_t TextDrawer::GetTotalPages()
{
    if (!CalcRenderInfo())
    {
        return 0;
    }
    assert(m_lineInfoCount > 0);
    assert(TDM_UNLIMITED_HEIGHT == m_mode);

    // 在不限高时才有page模式
    if (TDM_UNLIMITED_HEIGHT != m_mode)
    {
        return 0;
    }
    if (m_pageStartLines.size() > 0)
    {
        return m_pageStartLines.size();
    }
    DK_BOX currentPageBox; 
    // 上一个pageBox是否超过pageHeight,即currentPageBox是否有效,可以与当前box merge
    bool currentPageBoxValid  = false;
    size_t currentPageStartLine = 0;

    for (size_t i = 0; i < m_lineInfoCount; ++i)
    {
        if (currentPageBoxValid)
        {
            currentPageBox = currentPageBox.MergeBox(m_pLineInfos[i].boundingBox);
            if (currentPageBox.Height() > m_pageHeight)
            {
                m_pageStartLines.push_back(currentPageStartLine);
                currentPageBoxValid = false;
            }
        }
        if (!currentPageBoxValid)
        {
            currentPageStartLine = i;
            currentPageBox = m_pLineInfos[i].boundingBox;
            currentPageBoxValid = currentPageBox.Height() < m_pageHeight;
            if (!currentPageBoxValid)
            {
                m_pageStartLines.push_back(i);
            }
        }
    }
    if (currentPageBoxValid)
    {
        m_pageStartLines.push_back(currentPageStartLine);
    }
    return m_pageStartLines.size();
}

bool TextDrawer::Render(DK_IMAGE image, int dstX, int dstY, int dstWidth, int dstHeight)
{
    if (TDM_UNLIMITED_HEIGHT == m_mode)
    {
        return false;
    }
    int width = 0, height = 0;
    if (!GetBoundingBox(&width, &height))
    {
        return false;
    }
	DK_IMAGE imgDraw;
    DK_BITMAPBUFFER_DEV dev;
	//width = (width > dstWidth) ? width : dstWidth;
	//height= (height> dstHeight)? height : dstHeight;
	//width = dstWidth;
    if (!RenderUtil::CreateRenderImageAndDevice32(width, height, m_backgroundColor, &imgDraw, &dev))
    {
        return false;
    }
    int top = 0;
    switch (m_valign)
    {
    case VALIGN_TOP:
        top = 0;
        break;
    case VALIGN_BOTTOM:
        top = image.iHeight - height;
        break;
    case VALIGN_CENTER:
        top = (image.iHeight - height)>>1;
        break;
    }
    top = top > 0 ? top : 0;
    CopyImageEx(imgDraw, 0, 0, image, dstX, dstY + top, width, height);
    DK_FLOWRENDEROPTION renderOption;
    renderOption.nDeviceType = DK_OUTPUTDEV_BITMAPBUFFER;
    renderOption.pDevice = &dev;
    //renderOption.gamma.dTextGamma = 0;
    std::string ansi = EncodeUtil::ToString(m_ansiFontFile);
    std::string gb = EncodeUtil::ToString(m_gbFontFile);
    if (DKR_OK != s_render->RenderByInfo(
        renderOption,
        m_useTraditionalChinese,
        0,
        0,
        m_textColor,
        m_gbFontFile.c_str(),
        m_ansiFontFile.c_str(),
        m_fontSize,
        m_pCharInfos,
        m_pCharInfos + m_charInfoCount))
    {
        DK_FREE(imgDraw.pbData);
        return false;
    }
	IBMPProcessor* grayProcessor = BMPProcessFactory::CreateInstance(IBMPProcessor::DK_BMPPROCESSOR_GRAYSCALE);
	if(NULL == grayProcessor || !grayProcessor->Process(&imgDraw))
    {
        DK_FREE(imgDraw.pbData);
		return false;
	}

	RenderStrikeThrough(imgDraw, 0, m_lineInfoCount);
	RenderUnderLine(imgDraw, 0, m_lineInfoCount);
	RenderHighlight(imgDraw, m_highlightStartPos, m_highlightEndPos);


    bool result = CopyImageEx(image, dstX, dstY + top, imgDraw, 0, 0, width, height);
    DK_FREE(imgDraw.pbData);
    return result;
}

bool TextDrawer::RenderPage(int pageIndex, DK_IMAGE image, int dstX, int dstY, int dstWidth, int dstHeight)
{
    if (TDM_UNLIMITED_HEIGHT != m_mode)
    {
        return false;
    }
	if (pageIndex < 0 || (unsigned int)pageIndex >= GetTotalPages())
	{
		return false;
	}

	DK_IMAGE imgDraw;
    DK_BITMAPBUFFER_DEV dev;
    if (!RenderUtil::CreateRenderImageAndDevice32(m_maxWidth, m_pageHeight, m_backgroundColor, &imgDraw, &dev))
    {
        return false;
    }
    DK_FLOWRENDEROPTION renderOption;
    renderOption.nDeviceType = DK_OUTPUTDEV_BITMAPBUFFER;
    renderOption.pDevice = &dev;
	int startLineIndex = m_pageStartLines[pageIndex];
	int endLineIndex = (pageIndex == (m_pageStartLines.size() -1 )) ? m_lineInfoCount : (m_pageStartLines[pageIndex + 1]);

	unsigned int startCharIndex = m_pLineInfos[m_pageStartLines[pageIndex]].startIndex;
	unsigned int endCharIndex = (pageIndex == (m_pageStartLines.size() -1 )) ? m_text.size() : (m_pLineInfos[m_pageStartLines[pageIndex + 1]].startIndex);

	IDKTRender::RenderCharInfo* pStartCharInfo = GetRenderCharInfoByIndex(startCharIndex);
	IDKTRender::RenderCharInfo* pEndCharInfo = GetRenderCharInfoByIndex(endCharIndex);
    if (DKR_OK != s_render->RenderByInfo(
        renderOption,
        m_useTraditionalChinese,
		//-pStartCharInfo->boundingBox.X0,
        0,
        -pStartCharInfo->boundingBox.Y0,
        m_textColor,
        m_gbFontFile.c_str(),
        m_ansiFontFile.c_str(),
        m_fontSize,
		pStartCharInfo,
		pEndCharInfo))
    {
        DK_FREE(imgDraw.pbData);
        return false;
    }
	IBMPProcessor* grayProcessor = BMPProcessFactory::CreateInstance(IBMPProcessor::DK_BMPPROCESSOR_GRAYSCALE);
	if(NULL == grayProcessor || !grayProcessor->Process(&imgDraw))
    {
        DK_FREE(imgDraw.pbData);
		return false;
	}

	RenderStrikeThrough(imgDraw, startLineIndex, endLineIndex);
	RenderUnderLine(imgDraw, startLineIndex, endLineIndex);

	int highlightStartPos = m_highlightStartPos > (int)startCharIndex ? m_highlightStartPos : startCharIndex;
	int highlightEndPos = m_highlightEndPos < (int)endCharIndex ? m_highlightEndPos : endCharIndex;
	RenderHighlight(imgDraw, highlightStartPos, highlightEndPos, pStartCharInfo->boundingBox.X0, pStartCharInfo->boundingBox.Y0);

    int textHeight = (pEndCharInfo-1)->boundingBox.Y1 - pStartCharInfo->boundingBox.Y0;
    int top = 0;
    switch (m_valign)
    {
    case VALIGN_TOP:
        top = 0;
        break;
    case VALIGN_BOTTOM:
        top = image.iHeight - textHeight;
        break;
    case VALIGN_CENTER:
        top = (image.iHeight - textHeight)>>1;
        break;
    }
    top = top > 0 ? top : 0;
    bool result = CopyImageEx(image, dstX, dstY + top, imgDraw, 0, 0, m_maxWidth, textHeight);
    DK_FREE(imgDraw.pbData);
    return result;
}

void TextDrawer::SetHighlightIndexRange(int startPos, int endPos)
{
	if (m_highlightStartPos != startPos || m_highlightEndPos != endPos)
	{
		m_highlightStartPos = startPos >= 0 ? startPos : 0;
		m_highlightEndPos = endPos >= 0 ? endPos : 0;
		SetModifiedFlag();
	}
}

void TextDrawer::RenderHighlight(DK_IMAGE image, int from, int to, int baseX, int baseY)
{
    if (from < 0 || to < 0)
    {
        return;
    }

	if ((unsigned int)from >= m_charInfoCount || (unsigned int)to >= m_charInfoCount)
	{
		return;
	}

	for ( unsigned int i=(unsigned int)from; i<(unsigned int)to&&i<m_charInfoCount; ++i)
	{
		IDKTRender::RenderCharInfo* charInfo = m_pCharInfos + i;
		DK_RECT rcSelf={charInfo->boundingBox.X0 - baseX, charInfo->boundingBox.Y0 - baseY, charInfo->boundingBox.X1 -baseX, charInfo->boundingBox.Y1 - baseY};
		DK_IMAGE imgSelf;
		CropImage(image, rcSelf, &imgSelf);
		ReverseImage(imgSelf);
	}
}

void TextDrawer::RenderUnderLine(DK_IMAGE imgDraw, int fromline, int toline)
{
	if (m_underLine)
	{
		CTpGraphics grf(imgDraw);
		int baseY = m_pLineInfos[fromline].boundingBox.Y0;
		for (int i=fromline; i<toline; ++i)
		{
			double x = m_pLineInfos[i].boundingBox.X0;
			double y = m_pLineInfos[i].boundingBox.Y1 - baseY;
			grf.DrawLine(x, y, m_pLineInfos[i].boundingBox.Width(), 1, SOLID_STROKE);
		}
	}
}

void TextDrawer::RenderStrikeThrough(DK_IMAGE imgDraw, int fromline, int toline)
{
	if (m_strikeThrough)
	{
		CTpGraphics grf(imgDraw);
		int baseY = m_pLineInfos[fromline].boundingBox.Y0;
		for ( int i=fromline; i<toline; ++i)
		{
			double x = m_pLineInfos[i].boundingBox.X0;
			double y = (m_pLineInfos[i].boundingBox.Y0 + m_pLineInfos[i].boundingBox.Y1)/2 - baseY;
			grf.DrawLine(x, y, m_pLineInfos[i].boundingBox.Width(), 1, SOLID_STROKE);
		}
	}
}

IDKTRender::RenderCharInfo* TextDrawer::GetRenderCharInfoByIndex(unsigned int charIndex)
{
	if (charIndex<0 || charIndex>m_text.size())
	{
		return NULL;
	} else if (charIndex == m_text.size())
	{
		return m_pCharInfos + m_charInfoCount;
	}

	int left = 0, right = m_charInfoCount-1;
	int mid = (left + right)>>1;

	while (left<=right)
	{
		mid = (left + right)>>1;
		if (m_pCharInfos[mid].index > charIndex)
		{
			right = mid - 1;
		}
		else if (m_pCharInfos[mid].index < charIndex)
		{
			left = mid + 1;
		}
		else
		{
			return m_pCharInfos+mid;
		}
	}

	return NULL;
}

const DK_ARGBCOLOR& TextDrawer::GetBackgroundColor() const
{
	return m_backgroundColor; 
}

void TextDrawer::SetUseTraditionalChinese(bool useTraditionalChinese)
{
    m_useTraditionalChinese = useTraditionalChinese;
}

bool TextDrawer::GetTraditionalChineseUsed() const
{
    return m_useTraditionalChinese;
}
