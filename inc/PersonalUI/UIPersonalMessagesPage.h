#ifndef _DKREADER_UIPERSONALMESSAGESPAGE_H_
#define _DKREADER_UIPERSONALMESSAGESPAGE_H_

#include "BookStoreUI/UIBookStoreListBoxPage.h"

class UIPersonalMessagesPage : public UIBookStoreListBoxPage
{
public:
    UIPersonalMessagesPage();
    ~UIPersonalMessagesPage();
    virtual LPCSTR GetClassName() const
    {
        return "UIPersonalMessagesPage";
    }
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    virtual bool CreateListBoxPanel();
    virtual dk::bookstore::FetchDataResult FetchInfo();
    virtual bool OnMessageListBoxUpdate(const EventArgs& args);

};
#endif//_DKREADER_UIPERSONALMESSAGESPAGE_H_

