#include <tr1/functional>
#include "BookStoreUI/UIBookStoreListBoxWithBtn.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "I18n/StringManager.h"
#include "Utility/ColorManager.h"

using namespace WindowsMetrics;

UIBookStoreListBoxWithBtn::UIBookStoreListBoxWithBtn(int itemCount, int itemHeight, int itemSpacing)
    : UIBookStoreListBoxPanel(itemCount, itemHeight, itemSpacing)
{
    Init();
}

UIBookStoreListBoxWithBtn::~UIBookStoreListBoxWithBtn()
{
}

void UIBookStoreListBoxWithBtn::Init()
{
    const int fontsize14 = GetWindowFontSize(FontSize14Index);
	const dkColor disabledColor = ColorManager::GetColor(COLOR_DISABLECOLOR);
    m_nextWnd.SetFontSize(fontsize14);
    m_nextWnd.SetAlign(ALIGN_RIGHT);
	m_nextWnd.SetForeColor(disabledColor);
	m_nextWnd.SetText(StringManager::GetPCSTRById(TOUCH_BOOKSETTING_TURN_DOWN));

    m_prevWnd.SetFontSize(fontsize14);
    m_prevWnd.SetAlign(ALIGN_LEFT);
	m_prevWnd.SetForeColor(disabledColor);
	m_prevWnd.SetText(StringManager::GetPCSTRById(TOUCH_BOOKSETTING_TURN_UP));

    AppendChild(&m_nextWnd);
    AppendChild(&m_prevWnd);
}

UISizer* UIBookStoreListBoxWithBtn::CreateBottomSizer()
{
    if (NULL == m_pBottomSizer)
    {
        m_pBottomSizer = new UIGridSizer(1, 2, 0, 0);
        if (m_pBottomSizer)
        {
            m_pBottomSizer->Add(&m_prevWnd, UISizerFlags().Expand());
            m_pBottomSizer->Add(&m_nextWnd, UISizerFlags().Expand());
        }
    }
    return m_pBottomSizer;
}

void UIBookStoreListBoxWithBtn::UpdateBottomItem(bool visible)
{
    if (!m_onceAllItems)
    {
		m_nextWnd.SetVisible(visible && m_hasMoreData);
		m_prevWnd.SetVisible(visible && m_currentPage > 0);
    }
    else
    {
    	m_nextWnd.SetForeColor(ColorManager::knBlack);
    	m_prevWnd.SetForeColor(ColorManager::knBlack);
    	m_nextWnd.SetVisible(visible);
		m_prevWnd.SetVisible(visible);
        char buf[32] = {0};
        sprintf(buf, StringManager::GetPCSTRById(BOOKSTORE_BOOK_COUNT), m_totalItem);
        m_prevWnd.SetText(buf);

        const int totalPage = (m_totalItem + m_perPageNumber - 1) / m_perPageNumber;
        memset(buf, 0, 32);
        if(m_totalItem > 0)
            sprintf(buf, "%d/%d %s", m_currentPage + 1, totalPage, StringManager::GetPCSTRById(BOOK_PAGE));
        else
            sprintf(buf, "%s", StringManager::GetPCSTRById(BOOK_00_PAGE));
        m_nextWnd.SetText(buf);
    }
    if (visible)
    {
        Repaint();
    }
}
