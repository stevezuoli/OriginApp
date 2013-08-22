#ifndef _DKREADER_UIBOOKSTORETOPICPAGE_H_
#define _DKREADER_UIBOOKSTORETOPICPAGE_H_

#include "BookStoreUI/UIBookStoreListBoxPage.h"
#include "BookStoreUI/UIBookCoverView.h"
#include "GUI/UITouchComplexButton.h"
#include "GUI/UIText.h"

class UIBookStoreTopicPage : public UIBookStoreNavigation
{
public:
    UIBookStoreTopicPage(int topicId, const std::string& topicTitle);
    ~UIBookStoreTopicPage();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreTopicPage";
    }
    virtual BOOL OnKeyPress(INT32 iKeyCode);
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    virtual void SetFetchStatus(FETCH_STATUS status);
    virtual UISizer* CreateMidSizer();
    dk::bookstore::FetchDataResult FetchInfo();
    bool OnMessageTopicInfoUpdate(const EventArgs& args);
private:
    void UpdateChildWindow(bool showCover);
private:
    int m_topicId;
    bool m_showCover;
    UIBookStoreListBoxPanel* m_pListBoxPanel;
    UIBookCoverView m_cover;
    UITouchComplexButton m_gotoTopic;
    UIText m_topicDescription;
};

#endif//_DKREADER_UIBOOKSTORETOPICPAGE_H_

