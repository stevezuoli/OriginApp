#include <tr1/functional>
#include "BookStoreUI/UIBookStoreSearchPage.h"
#include "CommonUI/CPageNavigator.h"
#include "CommonUI/UIUtility.h"
#include "I18n/StringManager.h"
#include "GUI/UISizer.h"
#include "GUI/CTpGraphics.h"
#include "Common/WindowsMetrics.h"

using namespace dk::bookstore;
using namespace WindowsMetrics;

UIBookStoreSearchPage::UIBookStoreSearchPage(const std::string& searchKey)
    : UIPage()
    , m_searchKey(searchKey)
    , m_fetchStatus(NOFETCH)
    , m_listBoxPanel(GetWindowMetrics(UIBookStoreSearchPageItemCountIndex), 
                     GetWindowMetrics(UIBookStoreSearchPageItemHeightIndex))
#ifdef KINDLE_FOR_TOUCH
    , m_backButton(this, ID_BTN_TOUCH_BACK)
#endif
{
    SubscribeMessageEvent(BookStoreInfoManager::EventSearchBookUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreSearchPage::OnMessageListBoxUpdate),
        this,
        std::tr1::placeholders::_1)
        );
    SetLeftMargin(GetWindowMetrics(UIHorizonMarginIndex));
    Init();
}

UIBookStoreSearchPage::~UIBookStoreSearchPage()
{
}


void UIBookStoreSearchPage::Init()
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    const int fontsize18 = GetWindowFontSize(FontSize18Index);
    const int fontsize20 = GetWindowFontSize(FontSize20Index);

#ifdef KINDLE_FOR_TOUCH
    m_searchBox.SetImage(ImageManager::GetImage(IMAGE_TOUCH_ICON_SEARCH));
#endif
    m_searchBox.SetTextUTF8(m_searchKey.c_str());
    m_searchBox.SelectAll();
    m_searchBox.SetFontSize(fontsize20);
    m_searchBox.SetId(ID_EDIT_SEARCH_BOOK);
    m_searchBox.SetMinHeight(GetWindowMetrics(UIPixelValue40Index));
    AppendChild(&m_searchBox);

    m_nonSearchResult.SetFontSize(fontsize18);
    m_nonSearchResult.SetLineSpace(1);
    AppendChild(&m_nonSearchResult);

    m_listBoxPanel.SetTopMargin(0);
    AppendChild(&m_listBoxPanel);

#ifdef KINDLE_FOR_TOUCH
    if (m_pBottomBar)
    {
        AppendChild(m_pBottomBar);
    }
#else
    m_titleBar.MoveWindow(0, 0, 600, 30);
    m_pTitleBar = &m_titleBar;
#endif
    AppendChild(&m_titleBar);
    // m_fetchInfo应该最后绘制，否则会被别的窗口挡住
    AppendChild(&m_fetchInfo);
}

void UIBookStoreSearchPage::MoveWindow(INT32 left, INT32 top, INT32 width, INT32 height)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__, __LINE__, GetClassName(), __FUNCTION__);
    UIPage::MoveWindow(left, top, width, height);

    if (!m_windowSizer)
    {
        UISizer* pMainSizer = new UIBoxSizer(dkVERTICAL);
        UISizer* pTopSizer = new UIBoxSizer(dkHORIZONTAL);
        if (pMainSizer && pTopSizer)
        {
            m_nonSearchResult.SetMaxWidth(m_iWidth - (m_iLeftMargin << 1));
#ifdef KINDLE_FOR_TOUCH
            pTopSizer->Add(&m_backButton, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Border(dkRIGHT, GetWindowMetrics(UIPixelValue20Index)));
            pTopSizer->SetMinHeight(GetWindowMetrics(UIBackButtonHeightIndex));
#endif
            pTopSizer->Add(&m_searchBox, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));

            pMainSizer->Add(&m_titleBar, UISizerFlags().Expand());
            pMainSizer->Add(pTopSizer, UISizerFlags().Expand().Border(dkRIGHT, m_iLeftMargin));
            pMainSizer->Add(&m_nonSearchResult, UISizerFlags(1).Expand().Border(dkLEFT | dkRIGHT, m_iLeftMargin));
            pMainSizer->Add(&m_listBoxPanel, UISizerFlags(1).Expand().Border(dkLEFT | dkRIGHT, m_iLeftMargin).ReserveSpaceEvenIfHidden());
#ifdef KINDLE_FOR_TOUCH
            if (m_pBottomBar)
            {
                pMainSizer->Add(m_pBottomBar, UISizerFlags().Expand());
            }
#endif
            SetSizer(pMainSizer);
        }
        else
        {
            SafeDeletePointer(pMainSizer);
            SafeDeletePointer(pTopSizer);
        }
    }
    FetchInfo();
}

bool UIBookStoreSearchPage::OnMessageListBoxUpdate(const EventArgs& args)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    bool result = m_listBoxPanel.OnMessageListBoxUpdate(args);
    SetFetchStatus(result ? (m_listBoxPanel.GetItemCount() > 0 ? FETCH_SUCCESS : FETCH_NOELEM) : FETCH_FAIL);
    return result;
}

void UIBookStoreSearchPage::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    switch (dwCmdId)
    {
#ifdef KINDLE_FOR_TOUCH
    case ID_BTN_TOUCH_BACK:
        CPageNavigator::BackToPrePage();
        break;
#endif
    case ID_EDIT_SEARCH_BOOK:
    case ID_LISTBOX_TURNPAGE:
    case ID_BOOKSTORE_RETRY:
        if(UIUtility::CheckAndReConnectWifi())
        {
            m_searchKey = m_searchBox.GetTextUTF8();
            m_searchBox.SelectAll();
            FetchInfo();
        }
        break;
    default:
        break;
    }
}

BOOL UIBookStoreSearchPage::OnKeyPress(INT32 iKeyCode)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
#ifndef KINDLE_FOR_TOUCH
    if ((m_searchBox.IsFocus() && !m_searchBox.OnKeyPress(iKeyCode))
        || ((m_listBoxPanel.GetItemCount() > 0) && !m_listBoxPanel.OnKeyPress(iKeyCode)))
    {
        return FALSE;
    }
#endif
    return UIPage::OnKeyPress(iKeyCode);
}

HRESULT UIBookStoreSearchPage::DrawBackGround(DK_IMAGE drawingImg)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    CTpGraphics grf(drawingImg);
    grf.EraserBackGround();
#ifndef KINDLE_FOR_TOUCH
    int lineTop = m_titleBar.GetHeight();
    const int lineHeight = 2;
    grf.FillRect(0, lineTop, m_iWidth, lineTop + lineHeight, ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
#endif
    return S_OK;
}

FetchDataResult UIBookStoreSearchPage::FetchInfo()
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    SetFetchStatus(FETCHING);

    FetchDataResult fdr;
    BookStoreInfoManager* manager = BookStoreInfoManager::GetInstance();
    if (manager)
    {
        const int pageNumber = m_listBoxPanel.GetNumberPerPage();
        const int startIndex = pageNumber * (m_listBoxPanel.GetCurrentPage());
        fdr = manager->FetchSearchBookList(m_searchKey.c_str(), startIndex, pageNumber);
    }
    else
    {
        SetFetchStatus(FETCH_FAIL);
    }
    return fdr;
}

void UIBookStoreSearchPage::SetFetchStatus(FETCH_STATUS status)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s m_fetchStatus = %d, status = %d", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, m_fetchStatus, status);
    if (m_fetchStatus != status)
    {
        m_fetchStatus = status;
        if (status == FETCH_NOELEM)
        {
            char szNonResult[256] = {0};
            snprintf(szNonResult,DK_DIM(szNonResult) ,StringManager::GetPCSTRById(BOOKSTORE_NONSEARCHRESULT_INFO), m_searchKey.c_str());
            m_nonSearchResult.SetText(szNonResult);

            m_nonSearchResult.SetVisible(true);
#ifndef KINDLE_FOR_TOUCH
            m_searchBox.SetFocus(true);
#endif
            m_fetchInfo.SetFetchStatus(FETCH_SUCCESS);
        }
        else
        {
            m_nonSearchResult.SetVisible(false);
            m_fetchInfo.SetFetchStatus(status);
        }
        m_listBoxPanel.SetVisible(status == FETCH_SUCCESS);
        Layout();
        Repaint();
    }
}

void UIBookStoreSearchPage::OnClick()
{
    if (m_searchBox.IsFocus() && m_searchBox.IsVisible())
    {
        m_searchKey = m_searchBox.GetTextUTF8();
        m_listBoxPanel.ResetListBox();
        m_searchBox.SelectAll();
        FetchInfo();
    }
}
