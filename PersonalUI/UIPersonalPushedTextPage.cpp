#include <tr1/functional>
#include "PersonalUI/UIPersonalPushedTextPage.h"
#include "BookStore/DataUpdateArgs.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "drivers/DeviceInfo.h"
#include "I18n/StringManager.h"

using namespace dk::bookstore;
using namespace WindowsMetrics;

UIPersonalPushedTextPage::UIPersonalPushedTextPage(const std::string& title, const std::string& content)
    : UIBookStoreNavigationWithFling(title, false)
{
    const int contentFontsize = GetWindowFontSize(FontSize20Index);
    const int pageNumFontsize = GetWindowFontSize(FontSize16Index);
    m_content.SetText(content.c_str());
    m_content.SetFontSize(contentFontsize);
    m_content.SetMode(UIAbstractText::TextMultiPage);
    AppendChild(&m_content);

    m_pageNumber.SetFontSize(pageNumFontsize);
	m_pageNumber.SetEnglishGothic();
    m_pageNumber.SetAlign(ALIGN_RIGHT);
    AppendChild(&m_pageNumber);
}

UIPersonalPushedTextPage::~UIPersonalPushedTextPage()
{
}

void UIPersonalPushedTextPage::SetFetchStatus(FETCH_STATUS status)
{
    if (m_fetchStatus != status)
    {
        UIBookStoreNavigationWithFling::SetFetchStatus(FETCH_SUCCESS);
        m_totalPage = m_content.GetPageCount();
        UpdatePageNumber();
    }
}

UISizer* UIPersonalPushedTextPage::CreateMidSizer()
{
    if (NULL == m_pMidSizer)
    {
        m_pMidSizer = new UIBoxSizer(dkVERTICAL);
        if (m_pMidSizer)
        {
            m_content.SetMaxWidth(DeviceInfo::GetDisplayScreenWidth() - (m_iLeftMargin << 1));

            m_pMidSizer->Add(&m_content, UISizerFlags(1).Expand().Border(dkTOP | dkLEFT | dkRIGHT, m_iLeftMargin));
            m_pMidSizer->Add(&m_pageNumber, UISizerFlags().Expand().Border(dkTOP, GetWindowMetrics(UIPixelValue10Index)).Border(dkBOTTOM | dkLEFT | dkRIGHT, m_iLeftMargin));
        }
    }
    return m_pMidSizer;
}

void UIPersonalPushedTextPage::UpdatePageNumber()
{
    char szPageNumber[256] = {0};
    sprintf(szPageNumber, "%d / %d %s", m_curPage + 1, m_totalPage, StringManager::GetPCSTRById(BOOK_PAGE));
    m_pageNumber.SetText(szPageNumber);
}

bool UIPersonalPushedTextPage::TurnPage(bool downPage)
{
    if (UIBookStoreNavigationWithFling::TurnPage(downPage))
    {
        m_content.SetDrawPageNo(m_curPage);
        UpdateWindow();
        return true;
    }
    return false;
}

