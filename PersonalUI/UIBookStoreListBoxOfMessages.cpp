#include <tr1/functional>
#include "PersonalUI/UIBookStoreListBoxOfMessages.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "I18n/StringManager.h"
#include "Utility/ColorManager.h"
#include "PersonalUI/PushedMessagesManager.h"

using namespace WindowsMetrics;
using namespace dk::bookstore;

UIBookStoreListBoxOfMessages::UIBookStoreListBoxOfMessages(int itemCount, int itemHeight, int itemSpacing)
    : UIBookStoreListBoxWithBtn(itemCount, itemHeight, itemSpacing)
{
}

UIBookStoreListBoxOfMessages::~UIBookStoreListBoxOfMessages()
{
}

UISizer* UIBookStoreListBoxOfMessages::CreateBottomSizer()
{
    if (NULL == m_pBottomSizer)
    {
        m_pBottomSizer = new UIGridSizer(1, 2, 0, 0);
        if (m_pBottomSizer)
        {
            const int margin = GetWindowMetrics(UIPixelValue30Index);
            m_pBottomSizer->Add(&m_prevWnd, UISizerFlags().Expand().Border(dkLEFT, margin));
            m_pBottomSizer->Add(&m_nextWnd, UISizerFlags().Expand().Border(dkRIGHT, margin));
        }
    }
    return m_pBottomSizer;
}

bool UIBookStoreListBoxOfMessages::UpdatePushedMessagesList()
{
    m_listBox.ClearItemList();
    m_objectList.clear();
    m_objectType = model::OT_MESSAGES;
    PushedMessagesManager* pPushedMessagesMgr = PushedMessagesManager::GetInstance();
    if (pPushedMessagesMgr)
    {
        model::PushedMessageInfoList list = pPushedMessagesMgr->GetPushedMessageList();
        for (std::vector<model::PushedMessageInfoSPtr>::iterator itr = list.begin(); itr != list.end(); itr++)
        {
            m_objectList.push_back(model::BasicObjectSPtr((*itr)->Clone()));
        }
    }
    m_totalItem = m_objectList.size();
    m_hasMoreData = IsLastPage(false);
    UpdateListBox();
    return true;
}

bool UIBookStoreListBoxOfMessages::TurnPageUpOrDown(bool pageDown)
{
    if (!m_enableTurnPage)
    {
        DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return false;
    }

    if (pageDown)
    {
        if (m_hasMoreData)
        {
            m_currentPage++;
        }
        else
        {
            return false;
        }
    }
    else
    {
        if (m_currentPage > 0)
        {
            m_currentPage--;
        }
        else
        {
            return false;
        }
    }
    m_enableTurnPage = false;
    if (m_onceAllItems)
    {
        m_hasMoreData = IsLastPage(m_hasMoreData);
        UpdatePushedMessagesList();
#ifndef KINDLE_FOR_TOUCH
        if (m_bIsVisible && m_listBox.GetVisibleItemNum() > 0)
        {
            DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            m_listBox.SetFocus(false);
            m_listBox.SetSelectedItem(0);
            m_listBox.SetFocus(true);
        }
#endif
    }
    else if (m_pParent)
    {
        m_listBox.ClearItemList();
        m_pParent->OnCommand(ID_LISTBOX_TURNPAGE, this, 0);
    }
    return true;
}

bool UIBookStoreListBoxOfMessages::OnMessageListBoxUpdate(const EventArgs& args)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    const DataUpdateArgs& msgArgs = (const DataUpdateArgs&)args;
    if (msgArgs.succeeded)
    {
        m_objectList.clear();
        const int startIndex = m_onceAllItems ? 0 : (m_perPageNumber * m_currentPage);
        if ((startIndex == msgArgs.startIndex) && (msgArgs.dataList.size() > 0))
        {
            m_totalItem = msgArgs.total;
            m_objectType = msgArgs.dataList[0]->GetObjectType();
            switch(m_objectType)
            {
            case model::OT_BOXMESSAGES:
                {
                    model::PushedMessageInfoList list = model::PushedMessageInfo::FromBasicObjectList(msgArgs.dataList);
                    for (std::vector<model::PushedMessageInfoSPtr>::iterator itr = list.begin(); itr != list.end(); itr++)
                    {
                        m_objectList.push_back(model::BasicObjectSPtr((*itr)->Clone()));
                    }
                }
                break;
            default:
                break;
            }
            m_hasMoreData = IsLastPage(msgArgs.moreData);
            UpdateListBox();
        }
    }
    else
    {
        ShowTips(StringManager::GetPCSTRById(BOOKSTORE_UPDATE_LIST_PANEL_FAILED));
    }
    return msgArgs.succeeded;
}


