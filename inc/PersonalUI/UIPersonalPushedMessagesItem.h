#ifndef _DKREADER_UPERSONALPUSHEDMESSAGESITEM_H_
#define _DKREADER_UPERSONALPUSHEDMESSAGESITEM_H_

#include "GUI/UITextSingleLine.h"
#include "BookStore/BookStoreInfoManager.h"
#include "BookStore/PushedMessagesInfo.h"
#include "GUI/UIListItemBase.h"
#include "GUI/UIText.h"

class UIPersonalPushedMessagesItem : public UIListItemBase
{
public:
    virtual LPCSTR GetClassName() const
    {
        return "UIPersonalPushedMessagesItem";
    }
    UIPersonalPushedMessagesItem();
    ~UIPersonalPushedMessagesItem();
    void SetInfoSPtr(dk::bookstore::model::BasicObjectSPtr messageInfo, int index = -1);

    virtual bool ResponseTouchTap();
    virtual bool ResponseOperation();
private:
    void Init();

private:
    std::string m_messageID;
    std::string m_link;
    UITextSingleLine m_title;
    UIText m_message;
    UITextSingleLine m_time;
    dk::bookstore::model::PushedMessageInfoSPtr m_messageInfo;
};
#endif//_DKREADER_UPERSONALPUSHEDMESSAGESITEM_H_
