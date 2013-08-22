#include <tr1/functional>
#include "BookStoreUI/UIBookStoreBookAbstractPage.h"
#include "I18n/StringManager.h"
#include "Utility/ImageManager.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"

using namespace WindowsMetrics;


UIBookStoreBookAbstractPage::UIBookStoreBookAbstractPage(const std::string& content)
    : UIBookStoreNavigationWithFling(StringManager::GetPCSTRById(BOOKSTORE_BOOKABSTRACT_AND_COPYRIGHT), false, true, false)
{
    m_abstractText.SetText(content.c_str());
    Init();
}

UIBookStoreBookAbstractPage::~UIBookStoreBookAbstractPage()
{

}

void UIBookStoreBookAbstractPage::Init()
{
    const int abstractFontsize = GetWindowFontSize(FontSize20Index);
    const int pageNumFontsize = GetWindowFontSize(FontSize16Index);
    
    m_abstractText.SetAlign(ALIGN_JUSTIFY);
    m_abstractText.SetFontSize(abstractFontsize);
    m_abstractText.SetLineSpace(1.2);
    m_abstractText.SetVAlign(VALIGN_TOP);
    AppendChild(&m_abstractText);

    m_pageNumber.SetFontSize(pageNumFontsize);
	m_pageNumber.SetEnglishGothic();
    m_pageNumber.SetAlign(ALIGN_RIGHT);
    m_pageNumber.SetVAlign(VALIGN_BOTTOM);
    AppendChild(&m_pageNumber);
}

UISizer* UIBookStoreBookAbstractPage::CreateMidSizer()
{
    if (NULL == m_pMidSizer)
    {
        m_pMidSizer = new UIBoxSizer(dkVERTICAL);
        if (m_pMidSizer)
        {
            m_abstractText.SetMaxWidth(m_iWidth - (m_iLeftMargin << 1));
            m_pMidSizer->Add(&m_abstractText, UISizerFlags(1).Expand().Border(dkTOP | dkLEFT | dkRIGHT, m_iLeftMargin));
            m_pageNumber.SetMinHeight(GetWindowMetrics(UIPixelValue30Index));
            m_pMidSizer->Add(&m_pageNumber, UISizerFlags().Expand().Border(dkBOTTOM | dkLEFT | dkRIGHT, m_iLeftMargin));
        }
    }
    return m_pMidSizer;
}

void UIBookStoreBookAbstractPage::SetFetchStatus(FETCH_STATUS status)
{
    if (m_fetchStatus != status)
    {
        UIBookStoreNavigationWithFling::SetFetchStatus(FETCH_SUCCESS);
        m_totalPage = m_abstractText.GetPageCount();
        UpdatePageNumber();
    }
}

void UIBookStoreBookAbstractPage::UpdatePageNumber()
{
    char szPageNumber[256] = {0};
    sprintf(szPageNumber, "%d / %d %s", m_curPage + 1, m_totalPage, StringManager::GetPCSTRById(BOOK_PAGE));
    m_pageNumber.SetText(szPageNumber);
}

bool UIBookStoreBookAbstractPage::TurnPage(bool downPage)
{
    if (UIBookStoreNavigationWithFling::TurnPage(downPage))
    {
        m_abstractText.SetDrawPageNo(m_curPage);
        UpdateWindow();
        return true;
    }
    return false;
}

