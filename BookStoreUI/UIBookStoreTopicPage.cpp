#include <tr1/functional>
#include "BookStoreUI/UIBookStoreTopicPage.h"
#include "BookStoreUI/UIBookStoreListBoxWithBtn.h"
#include "BookStore/DataUpdateArgs.h"
#include "I18n/StringManager.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"

using namespace dk::bookstore;
using namespace WindowsMetrics;

UIBookStoreTopicPage::UIBookStoreTopicPage(int topicId, const std::string& topicTitle)
    : UIBookStoreNavigation(topicTitle)
    , m_topicId(topicId)
    , m_showCover(true)
    , m_pListBoxPanel(NULL)
{
    SubscribeMessageEvent(BookStoreInfoManager::EventTopicBookListUpdate,
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreTopicPage::OnMessageTopicInfoUpdate),
        this,
        std::tr1::placeholders::_1)
        );
}

UIBookStoreTopicPage::~UIBookStoreTopicPage()
{
    SafeDeletePointer(m_pListBoxPanel);
}

void UIBookStoreTopicPage::SetFetchStatus(FETCH_STATUS status)
{
    if (m_pMidSizer && (m_fetchStatus != status))
    {
        if (status == FETCH_SUCCESS)
        {
            m_pMidSizer->Show(true);
            UpdateChildWindow(m_showCover);
        }
        else
        {
            m_pMidSizer->Show(false);
        }
    }
    UIBookStoreNavigation::SetFetchStatus(status);
}

UISizer* UIBookStoreTopicPage::CreateMidSizer()
{
    if (NULL == m_pListBoxPanel)
    {
        m_pListBoxPanel = new UIBookStoreListBoxWithBtn(GetWindowMetrics(UIBookStoreListBoxPageItemCountIndex), 
            GetWindowMetrics(UIBookStoreListBoxPageItemHeightIndex));
        if (NULL == m_pListBoxPanel)
        {
            return NULL;
        }
        AppendChild(m_pListBoxPanel);
    }
    
    if (NULL == m_pMidSizer)
    {
        m_pMidSizer = new UIBoxSizer(dkVERTICAL);
        UISizer* pCoverSizer = new UIBoxSizer(dkVERTICAL);
        if (m_pMidSizer && pCoverSizer)
        {
            const int fontsize18 = GetWindowFontSize(FontSize18Index);
#ifdef KINDLE_FOR_TOUCH
            m_gotoTopic.Initialize(ID_BOOKSTORE_VIEW_ALL, StringManager::GetPCSTRById(BOOKSTORE_GOTOTOPIC), fontsize18);
            m_gotoTopic.ShowBorder(false);
            m_gotoTopic.SetIcon(ImageManager::GetImage(IMAGE_RIGHT_ARROW), UIButton::ICON_RIGHT);
#else
            m_gotoTopic.Initialize(ID_BOOKSTORE_VIEW_ALL, StringManager::GetPCSTRById(BOOKSTORE_GOTOTOPIC), KEY_RESERVED, fontsize18, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
            m_gotoTopic.SetFontSize(fontsize18);
#endif
            m_gotoTopic.SetAlign(ALIGN_RIGHT);
            m_gotoTopic.SetLeftMargin(0);
            AppendChild(&m_gotoTopic);

            m_cover.Initialize(CUSTOMIZED, IMAGE_BOOKSTORE_DEFAULTADCOVER);
            m_cover.SetMinSize(dkSize(GetWindowMetrics(UIBookStoreTopicPageCoverWidthIndex), 
                GetWindowMetrics(UIBookStoreTopicPageCoverHeightIndex)));
            AppendChild(&m_cover);

            m_topicDescription.SetFontSize(GetWindowFontSize(FontSize20Index));
            AppendChild(&m_topicDescription);

            int elementSpacing = GetWindowMetrics(UIElementMidSpacingIndex);
            pCoverSizer->Add(&m_cover, UISizerFlags().Align(dkALIGN_CENTER_HORIZONTAL));
            pCoverSizer->Add(&m_topicDescription, UISizerFlags(1).Expand().Border(dkLEFT | dkRIGHT, m_iLeftMargin).Border(dkTOP, (elementSpacing << 1)));
            pCoverSizer->Add(&m_gotoTopic, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, m_iLeftMargin).Border(dkTOP | dkBOTTOM, elementSpacing));
            m_pMidSizer->Add(pCoverSizer, UISizerFlags(1).Expand());
            m_pMidSizer->Add(m_pListBoxPanel, UISizerFlags(1).Expand().Border(dkLEFT | dkRIGHT, m_iLeftMargin));
        }
        else
        {
            SafeDeletePointer(pCoverSizer);
            SafeDeletePointer(m_pMidSizer);
            return NULL;
        }
    }
    return m_pMidSizer;
}

FetchDataResult UIBookStoreTopicPage::FetchInfo()
{
    SetFetchStatus(FETCHING);
    FetchDataResult fdr = FDR_FAILED;
    BookStoreInfoManager* manager = BookStoreInfoManager::GetInstance();
    if (manager && m_pListBoxPanel)
    {
        const int pageNumber = m_pListBoxPanel->GetNumberPerPage();
        const int startIndex = pageNumber * (m_pListBoxPanel->GetCurrentPage());
        fdr = manager->FetchTopicBookList(m_topicId, startIndex, pageNumber);
    }
    else
    {
        SetFetchStatus(FETCH_FAIL);
    }
    return fdr;
}

bool UIBookStoreTopicPage::OnMessageTopicInfoUpdate(const EventArgs& args)
{
    const TopicBookListUpdateArgs& msgArgs = dynamic_cast<const TopicBookListUpdateArgs&>(args);
    if (m_pListBoxPanel && (m_topicId == msgArgs.topicId))
    {
        bool result = msgArgs.succeeded;
        if (result)
        {
            result = m_pListBoxPanel->OnMessageListBoxUpdate(args);
            if (m_showCover)
            {
                m_showCover = !msgArgs.m_bannerUrl.empty();
                //UpdateChildWindow(!msgArgs.m_bannerUrl.empty());
                m_topicDescription.SetText(msgArgs.m_description.c_str());
                m_cover.SetCoverUrl(msgArgs.m_bannerUrl);
            }
        }
        SetFetchStatus(result ? ((m_pListBoxPanel->GetItemCount() > 0) ? FETCH_SUCCESS : FETCH_NOELEM) : FETCH_FAIL);
    }
    return true;
}

void UIBookStoreTopicPage::UpdateChildWindow(bool showCover)
{
    if (m_pListBoxPanel && m_pMidSizer)
    {
        m_showCover = showCover;
        m_pListBoxPanel->SetVisible(!showCover);
        m_topicDescription.SetVisible(showCover);
        m_cover.SetVisible(showCover);
        m_gotoTopic.SetVisible(showCover);
        m_pMidSizer->Layout();
#ifndef KINDLE_FOR_TOUCH
        if (showCover)
        {
            m_gotoTopic.SetFocus(true);
        }
        else
        {
            m_pListBoxPanel->SetFocus(true);
        }
#endif
    }
}

void UIBookStoreTopicPage::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    switch (dwCmdId)
    {
    case ID_BOOKSTORE_VIEW_ALL:
        {
            UpdateChildWindow(false);
            Repaint();
        }
        break;
    default:
        UIBookStoreNavigation::OnCommand(dwCmdId, pSender, dwParam);
        break;
    }
}

BOOL UIBookStoreTopicPage::OnKeyPress(INT32 iKeyCode)
{
#ifndef KINDLE_FOR_TOUCH
    if (m_mnuMain.IsVisible())
    {
        return UIBookStoreNavigation::OnKeyPress(iKeyCode);
    }

    if (!SendKeyToChildren(iKeyCode))
    {
        DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s if (!SendKeyToChildren(iKeyCode))", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return FALSE;
    }
    switch (iKeyCode)
    {
    case KEY_UP:
    case KEY_DOWN:
        if (m_pListBoxPanel && m_pListBoxPanel->IsVisible())
        {
            m_pListBoxPanel->SetFocus(false);
            return m_pListBoxPanel->OnKeyPress(iKeyCode);
        }
        else
        {
            m_gotoTopic.SetFocus(true);
        }
        break;
    default:
        if(SendHotKeyToChildren(iKeyCode))
        {
            DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s default", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            return UIBookStoreNavigation::OnKeyPress(iKeyCode);
        }
        break;
    }
#endif
    return TRUE;
}
