#ifndef _UIPERSONALSYSTEMMESSAGEITEM_H_
#define _UIPERSONALSYSTEMMESSAGEITEM_H_	

#include "GUI/UITextSingleLine.h"
#include "BookStore/BookStoreInfoManager.h"
#include "BookStore/PushedMessagesInfo.h"
#include "GUI/UIListItemBase.h"
#include "GUI/UIText.h"

class UIPersonalSystemMessageItem : public UIListItemBase
{
public:
    virtual LPCSTR GetClassName() const
    {
        return "UIPersonalSystemMessageItem";
    }
    UIPersonalSystemMessageItem();
    ~UIPersonalSystemMessageItem();
    void SetInfoSPtr(dk::bookstore::model::BasicObjectSPtr messageInfo, int index = -1);

    virtual bool ResponseTouchTap();
private:
    void Init();

private:
    UITextSingleLine m_title;
    UITextSingleLine m_time;
	UIText m_message;
    dk::bookstore::model::PushedMessageInfoSPtr m_messageInfo;
};
#endif//_UIPERSONALSYSTEMMESSAGEITEM_H_