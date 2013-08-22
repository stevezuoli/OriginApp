#include <tr1/functional>
#include "BookStoreUI/UIBookStoreListBoxWithPageNum.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "I18n/StringManager.h"

using namespace WindowsMetrics;

UIBookStoreListBoxWithPageNum::UIBookStoreListBoxWithPageNum(int itemCount, int itemHeight, int itemSpacing)
    : UIBookStoreListBoxPanel(itemCount, itemHeight, itemSpacing)
{
    Init();
}

UIBookStoreListBoxWithPageNum::~UIBookStoreListBoxWithPageNum()
{
}

void UIBookStoreListBoxWithPageNum::Init()
{
    const int fontsize14 = GetWindowFontSize(FontSize14Index);

    m_pageNum.SetFontSize(fontsize14);
	m_pageNum.SetEnglishGothic();
    m_pageNum.SetAlign(ALIGN_RIGHT);
    m_pageNum.SetVAlign(ALIGN_CENTER);
    AppendChild(&m_pageNum);

    m_itemNum.SetFontSize(fontsize14);
	m_itemNum.SetEnglishGothic();
    m_itemNum.SetVAlign(ALIGN_CENTER);
    AppendChild(&m_itemNum);
}

UISizer* UIBookStoreListBoxWithPageNum::CreateBottomSizer()
{
    if (NULL == m_pBottomSizer)
    {
        m_pBottomSizer = new UIGridSizer(1, 2, 0, 0);
        if (m_pBottomSizer)
        {
            m_pBottomSizer->Add(&m_itemNum, UISizerFlags().Expand());
            m_pBottomSizer->Add(&m_pageNum, UISizerFlags().Expand());
        }
    }
    return m_pBottomSizer;
}

void UIBookStoreListBoxWithPageNum::UpdateBottomItem(bool visible)
{
    if (m_pBottomSizer)
    {
        m_pBottomSizer->Show(visible);
        if (visible)
        {
            UpdateWindow();
        }
    }
}

bool UIBookStoreListBoxWithPageNum::IsLastPage(bool hasMoreData)
{
    const int totalPage = (m_totalItem + m_perPageNumber - 1) / m_perPageNumber;
    if (m_currentPage >= totalPage)
    {
        m_currentPage = totalPage - 1;
    }
    char str[32] = {0};
    snprintf(str, DK_DIM(str), "%d / %d %s", m_currentPage + 1, totalPage, StringManager::GetPCSTRById(BOOK_PAGE));
    m_pageNum.SetText(str);
    snprintf(str, DK_DIM(str), "%s %d %s", StringManager::GetPCSTRById(BOOK_TOTAL), m_totalItem, StringManager::GetPCSTRById(BOOK_TIAO));
    m_itemNum.SetText(str);
    return (m_currentPage < (totalPage - 1));
}

