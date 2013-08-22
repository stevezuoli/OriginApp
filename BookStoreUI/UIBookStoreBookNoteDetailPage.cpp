#include <tr1/functional>
#include "BookStoreUI/UIBookStoreBookNoteDetailPage.h"
#include "I18n/StringManager.h"
#include "Utility/ImageManager.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"

using namespace WindowsMetrics;

UIBookStoreBookNoteDetailPage::UIBookStoreBookNoteDetailPage(const std::string& content)
    : UIBookStoreNavigationWithFling(StringManager::GetPCSTRById(BOOKSTORE_BOOKABSTRACT_AND_COPYRIGHT), false)
{
    m_content.SetText(content.c_str());
    Init();
}

UIBookStoreBookNoteDetailPage::~UIBookStoreBookNoteDetailPage()
{

}

void UIBookStoreBookNoteDetailPage::Init()
{
    const int abstractFontsize = GetWindowFontSize(FontSize20Index);
    const int pageNumFontsize = GetWindowFontSize(FontSize16Index);
    
    m_content.SetAlign(ALIGN_JUSTIFY);
    m_content.SetFontSize(abstractFontsize);
    m_content.SetLineSpace(1.2);
    m_content.SetVAlign(VALIGN_TOP);
    AppendChild(&m_content);

    m_pageNumber.SetFontSize(pageNumFontsize);
	m_pageNumber.SetEnglishGothic();
    m_pageNumber.SetAlign(ALIGN_RIGHT);
    m_pageNumber.SetVAlign(VALIGN_BOTTOM);
    AppendChild(&m_pageNumber);
}

UISizer* UIBookStoreBookNoteDetailPage::CreateMidSizer()
{
    if (NULL == m_pMidSizer)
    {
        m_pMidSizer = new UIBoxSizer(dkVERTICAL);
        if (m_pMidSizer)
        {
            m_content.SetMaxWidth(m_iWidth - (m_iLeftMargin << 1));
            m_pMidSizer->Add(&m_content, UISizerFlags(1).Expand().Border(dkTOP | dkLEFT | dkRIGHT, m_iLeftMargin));
            m_pageNumber.SetMinHeight(GetWindowMetrics(UIPixelValue30Index));
            m_pMidSizer->Add(&m_pageNumber, UISizerFlags().Expand().Border(dkBOTTOM | dkLEFT | dkRIGHT, m_iLeftMargin));
        }
    }
    return m_pMidSizer;
}

void UIBookStoreBookNoteDetailPage::SetFetchStatus(FETCH_STATUS status)
{
    if (m_fetchStatus != status)
    {
        UIBookStoreNavigationWithFling::SetFetchStatus(FETCH_SUCCESS);
        m_totalPage = m_content.GetPageCount();
        UpdatePageNumber();
    }
}

void UIBookStoreBookNoteDetailPage::UpdatePageNumber()
{
    char szPageNumber[256] = {0};
    sprintf(szPageNumber, "%d / %d %s", m_curPage + 1, m_totalPage, StringManager::GetPCSTRById(BOOK_PAGE));
    m_pageNumber.SetText(szPageNumber);
}

bool UIBookStoreBookNoteDetailPage::TurnPage(bool downPage)
{
    if (UIBookStoreNavigationWithFling::TurnPage(downPage))
    {
        m_content.SetDrawPageNo(m_curPage);
        UpdateWindow();
        return true;
    }
    return false;
}

