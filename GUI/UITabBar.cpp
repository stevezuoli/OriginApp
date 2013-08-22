#include "GUI/UITabBar.h"
#include "GUI/UITouchComplexButton.h"
#include "GUI/CTpGraphics.h"
#include "Utility/ColorManager.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"

using namespace WindowsMetrics;

const char* UITabBar::TabIndexChangedEvent = "TabCurrentIndexChanged";

UITabBar::UITabBar(UIContainer* pParent)
	: UIContainer(pParent, IDSTATIC)
	, m_currentIndex(-1)
	, m_fontSize(GetWindowFontSize(UITabBarFontSizeIndex))
	, m_normalLineWidth(1)
	, m_normalLineHeight(-1)
	, m_focusedLineWidth(2)
	, m_baseCommandId(0x100)
    , m_focusTabColor(ColorManager::knWhite)
    , m_focusTopLineColor(ColorManager::knBlack)
    , m_backgroundTabColor(ColorManager::knWhite)
    , m_hightlightMode(HM_TOPLINE)
{
}

UITabBar::~UITabBar()
{
	for ( unsigned int i=0; i<m_tabButtons.size(); ++i)
	{
		delete m_tabButtons[i];
		m_tabButtons[i] = NULL;
	}
	m_tabButtons.clear();
	m_currentIndex = -1;
}


HRESULT UITabBar::DrawBackGround(DK_IMAGE drawingImg)
{
    CTpGraphics grf(drawingImg);
    unsigned int tabCounts = GetTabCounts();
    HRESULT hr(S_OK);
    RTN_HR_IF_FAILED(grf.EraserBackGround());

    int splitLineTop = (m_iHeight - m_normalLineHeight)>> 1;
    splitLineTop = splitLineTop > 0 ? splitLineTop : 0;
    int splitLineHeight = ((int)m_normalLineHeight > m_iHeight) ? m_iHeight : m_normalLineHeight;
    if (tabCounts > 0)
    {
        const unsigned int spliteWidth = m_iWidth - m_focusedLineWidth*(tabCounts+1);
        unsigned int tabWidth = spliteWidth / tabCounts;
        const unsigned int moreWidth = spliteWidth % tabCounts;
        unsigned int left = m_focusedLineWidth;
        for (unsigned int i = 0; i < tabCounts; ++i)
        {
            if (i == (tabCounts - 1))
            {
                tabWidth += moreWidth;
            }

            if ((unsigned int)m_currentIndex == i)
            {
                RTN_HR_IF_FAILED(grf.DrawLine(left, 0, tabWidth + m_focusedLineWidth, m_focusedLineWidth, SOLID_STROKE));
                RTN_HR_IF_FAILED(grf.DrawLine(left - m_focusedLineWidth, splitLineTop, m_focusedLineWidth, splitLineHeight, SOLID_STROKE));
                RTN_HR_IF_FAILED(grf.DrawLine(left + tabWidth, splitLineTop, m_focusedLineWidth, splitLineHeight, SOLID_STROKE));
            }
            else
            {
                RTN_HR_IF_FAILED(grf.DrawLine(left - m_focusedLineWidth, GetHeight() - m_focusedLineWidth, tabWidth + (m_focusedLineWidth << 1), m_focusedLineWidth, SOLID_STROKE));
                if ((int)i < (int)(m_currentIndex - 1))
                {
                    RTN_HR_IF_FAILED(grf.DrawLine(left + tabWidth, splitLineTop+10, m_normalLineWidth, splitLineHeight-20, SOLID_STROKE));
                }
                else if ((int)i > (int)(m_currentIndex + 1))
                {
                    RTN_HR_IF_FAILED(grf.DrawLine(left - m_normalLineWidth, splitLineTop+10, m_normalLineWidth, splitLineHeight-20, SOLID_STROKE));
                }
            }
            left += (m_focusedLineWidth + tabWidth);
        }
    }
	return hr;
}

HRESULT UITabBar::Draw(DK_IMAGE drawingImg)
{
	if (!m_bIsVisible)
	{
		return S_OK;
	}

	if (drawingImg.pbData == NULL)
	{
		return E_FAIL;
	}

	HRESULT hr(S_OK);
	DK_IMAGE imgSelf;
	DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};

	RTN_HR_IF_FAILED(CropImage(
		drawingImg,
		rcSelf,
		&imgSelf
        ));

    RTN_HR_IF_FAILED(DrawBackGround(imgSelf));

	UINT32 iSize = GetChildrenCount();

	for (UINT32 i = 0; i < iSize; i++)
	{
		UIWindow* pWin = GetChildByIndex(i);
		if (pWin)
		{
			DebugPrintf(DLC_ZHY, "%s: Drawing Child %d / %d : %s ...", GetClassName(), i, iSize, pWin->GetClassName());
            pWin->Draw(imgSelf);
		}
		else
		{
			DebugPrintf(DLC_ERROR, "%s: FAILED to get child %d/%d !!!", GetClassName(), i, iSize);
		}
	}

	DebugPrintf(DLC_ZHY, "Leaving HRESULT %s::Draw()", GetClassName());
	return hr;
}

void UITabBar::MoveWindow(INT32 left, INT32 top, INT32 width, INT32 height)
{
	UIContainer::MoveWindow(left, top, width, height);
	if (m_normalLineHeight < 0)
	{
		SetSplitLineHeight(height);
	}

    const unsigned int tabCounts = GetTabCounts();
    UISizer* pMainSizer = new UIBoxSizer(dkVERTICAL);
    UISizer* pGridSizer = new UIGridSizer(1, tabCounts, m_focusedLineWidth, m_focusedLineWidth);
    if (pMainSizer && pGridSizer)
    {
        for (unsigned int i=0; i<tabCounts; ++i)
        {
            pGridSizer->Add(m_tabButtons[i], UISizerFlags().Expand());
        }
        pMainSizer->Add(pGridSizer, UISizerFlags(1).Expand().Border(dkALL, m_focusedLineWidth));
        SetSizer(pMainSizer);
    }
    else
    {
        SafeDeletePointer(pMainSizer);
        SafeDeletePointer(pGridSizer);
    }
}

void UITabBar::SetSplitLineHeight(unsigned int height)
{
	m_normalLineHeight = height;
}

void UITabBar::OnCommand(DWORD dwCmdId, UIWindow* pSender, DWORD dwParam)
{
	SetCurrentIndex(IndexOfCommandId(dwCmdId));
#if 0
	if (m_pParent)
	{
		m_pParent->OnCommand(dwCmdId, pSender, dwParam);
	}
#endif
	FireTabIndexChangedEvent(m_currentIndex);
	UpdateWindow();
}

int UITabBar::AddTab(const char* title)
{
	return AddTab(title, m_baseCommandId + GetTabCounts());
}

int UITabBar::AddTab(const char* title, const DWORD cmdId)
{
	UITouchComplexButton* newTabButton = new UITouchComplexButton;
	if (newTabButton == NULL)
	{
		return -1;
	}

	newTabButton->Initialize(cmdId, title, m_fontSize);
	newTabButton->ShowBorder(false);
    newTabButton->SetBackgroundColor(m_backgroundTabColor);

	AppendChild(newTabButton);
	m_tabButtons.push_back(newTabButton);
	AutoModifySize();

	return m_tabButtons.size() - 1;
}

int UITabBar::AddTab(const char* title, const DWORD cmdId, SPtr<ITpImage> focusedImage, SPtr<ITpImage> unFocusedImage, int iconPos)
{
	UITouchComplexButton* newTabButton = new UITouchComplexButton;
	if (newTabButton == NULL)
	{
		return -1;
	}

	newTabButton->Initialize(cmdId, title, m_fontSize);
	newTabButton->SetIcons(focusedImage, unFocusedImage, iconPos);
	newTabButton->ShowBorder(false);
    newTabButton->SetBackgroundColor(m_backgroundTabColor);

	AppendChild(newTabButton);
	m_tabButtons.push_back(newTabButton);
	AutoModifySize();

	return m_tabButtons.size()-1;
}

int UITabBar::InsertTab(int index, const char* title)
{
	return InsertTab(index, title, m_baseCommandId + GetTabCounts());
}

int UITabBar::InsertTab(int index, const char* title, const DWORD cmdId)
{
	UITouchComplexButton* newTabButton = new UITouchComplexButton;
	if (newTabButton == NULL)
	{
		return -1;
	}

	newTabButton->Initialize(cmdId, title, m_fontSize);
	newTabButton->ShowBorder(false);
    newTabButton->SetBackgroundColor(m_backgroundTabColor);

	AppendChild(newTabButton);
	std::vector<UITouchComplexButton*>::iterator it = (index < (int)m_tabButtons.size()) ? m_tabButtons.begin() + index : m_tabButtons.end();
	it = m_tabButtons.insert(it, newTabButton);
	AutoModifySize();

	return it - m_tabButtons.begin();
}

int UITabBar::InsertTab(int index, const char* title, const DWORD cmdId, SPtr<ITpImage> focusedImage, SPtr<ITpImage> unFocusedImage, int iconPos)
{
	UITouchComplexButton* newTabButton = new UITouchComplexButton;
	if (newTabButton == NULL)
	{
		return -1;
	}

	newTabButton->Initialize(cmdId, title, m_fontSize);
	newTabButton->SetIcons(focusedImage, unFocusedImage, iconPos);
	newTabButton->ShowBorder(false);
    newTabButton->SetBackgroundColor(m_backgroundTabColor);

	AppendChild(newTabButton);
	std::vector<UITouchComplexButton*>::iterator it = (index < (int)m_tabButtons.size()) ? m_tabButtons.begin() + index : m_tabButtons.end();
	it = m_tabButtons.insert(it, newTabButton);
	AutoModifySize();

	return it - m_tabButtons.begin();
}

bool UITabBar::RemoveTab(unsigned int index)
{
	if (index<m_tabButtons.size())
	{
		std::vector<UITouchComplexButton*>::iterator it = m_tabButtons.begin() + index;
		delete *it;
		m_tabButtons.erase(it);
		AutoModifySize();
		return true;
	}
	return false;
}

void UITabBar::ElideText(bool elideText)
{
}

bool UITabBar::IsElideText() const
{
	return false;
}

void UITabBar::SetEnable(int index, bool enable)
{
	assert(index >= 0 && index < (int)m_tabButtons.size());
	m_tabButtons[index]->SetEnable(enable);
}

bool UITabBar::IsEnable(int index) const
{
	assert(index >= 0 && index < (int)m_tabButtons.size());
	return m_tabButtons[index]->IsEnable();
}

int UITabBar::CurrentIndex() const
{
	return m_currentIndex;
}

void UITabBar::SetCurrentIndex(int currentIndex)
{
	m_currentIndex = currentIndex;
    ResetTabButtonColors();
}

DWORD UITabBar::CurrentCmdId() const
{
    return m_tabButtons[m_currentIndex]->GetId();
}

void UITabBar::SetCurrentCmdId(DWORD cmdId)
{
    m_currentIndex = IndexOfCommandId(cmdId);
    ResetTabButtonColors();
}

void UITabBar::ResetTabButtonColors()
{
    if (HM_TOPLINE == m_hightlightMode)
    {
        for (size_t i = 0; i < m_tabButtons.size(); ++i)
        {
            m_tabButtons[i]->SetBackgroundColor(m_backgroundTabColor);
        }
    }
    else if (HM_FULL == m_hightlightMode)
    {
        for (size_t i = 0; i < m_tabButtons.size(); ++i)
        {
            m_tabButtons[i]->SetBackgroundColor((int)i == m_currentIndex ? m_focusTabColor : m_backgroundTabColor);
        }
    }
}

unsigned int UITabBar::GetTabCounts() const
{
	return m_tabButtons.size();
}

void UITabBar::SetFontSize(unsigned int fontSize)
{
	if (m_fontSize != fontSize)
	{
		m_fontSize = fontSize;
		for ( unsigned int i=0; i<m_tabButtons.size(); ++i)
			m_tabButtons[i]->SetFontSize(fontSize);
	}
}

void UITabBar::AutoModifySize()
{
	if (m_iWidth > 0)
	{
		MoveWindow(m_iLeft, m_iTop, m_iWidth, m_iHeight);
	}
}

int UITabBar::IndexOfCommandId(DWORD cmdId)
{
	unsigned int tabCounts = m_tabButtons.size();

	for ( unsigned int i=0; i<tabCounts; ++i)
	{
		if (cmdId == m_tabButtons[i]->GetId())
			return i;
	}

	return -1;
}

bool UITabBar::FireTabIndexChangedEvent(int index)
{
	TabIndexChangedEventArgs args(index);
	FireEvent(TabIndexChangedEvent, (EventArgs&)args);
    return true;
}

void UITabBar::SetFocusTabColor(dkColor focusTabColor)
{
    m_focusTabColor = focusTabColor;
    if (m_currentIndex >= 0 && m_tabButtons.size() > 0)
    {
        m_tabButtons[m_currentIndex]->SetBackgroundColor(m_focusTabColor);
    }
}

void UITabBar::SetBackgroundTabColor(dkColor backgroundTabColor)
{
    m_backgroundTabColor = backgroundTabColor;
    for (size_t i = 0; i < m_tabButtons.size(); ++i)
    {
        if ((int)i != m_currentIndex)
        {
            m_tabButtons[i]->SetBackgroundColor(backgroundTabColor);
        }
    }
    ResetTabButtonColors();
}

void UITabBar::SetFocusTopLineColor(dkColor focusTopLineColor)
{
    m_focusTopLineColor = focusTopLineColor;
}

void UITabBar::SetHighlightMode(HighlightMode highlightMode)
{
    m_hightlightMode = highlightMode;;
    if (m_hightlightMode == HM_FULL)
    {
        SetBackgroundTabColor(ColorManager::GetColor(COLOR_TAB_BACKGROUND));
    }
    else
    {
        SetBackgroundTabColor(ColorManager::knWhite);
    }
}
