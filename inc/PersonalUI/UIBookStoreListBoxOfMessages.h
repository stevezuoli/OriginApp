#ifndef _DKREADER_UIBOOKSTORETABLEPAGELISTBOXOFMESSAGES_H_
#define _DKREADER_UIBOOKSTORETABLEPAGELISTBOXOFMESSAGES_H_

#include "BookStoreUI/UIBookStoreListBoxWithBtn.h"
#include "GUI/UITextSingleLine.h"

class UIBookStoreListBoxOfMessages : public UIBookStoreListBoxWithBtn
{
public:
    UIBookStoreListBoxOfMessages(int itemCount, int itemHeight, int itemSpacing = 0);
    ~UIBookStoreListBoxOfMessages();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreListBoxOfMessages";
    }

    virtual UISizer* CreateBottomSizer();
    virtual bool UpdatePushedMessagesList();
    virtual bool OnMessageListBoxUpdate(const EventArgs& args);
    virtual bool TurnPageUpOrDown(bool pageDown);
};


#endif//_DKREADER_UIBOOKSTORETABLEPAGELISTBOXOFMESSAGES_H_
