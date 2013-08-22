#ifndef _DKREADER_APPUI_UIBOOKSTOREBOOKCOMMENTREPLYITEM_H_
#define _DKREADER_APPUI_UIBOOKSTOREBOOKCOMMENTREPLYITEM_H_

#include "GUI/UIImage.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UIContainer.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UIText.h"
#include "BookStoreUI/UIBookCoverView.h"
#include "BookStoreUI/UIDiscountTextSingleLine.h"
#include "BookStore/BookStoreInfoManager.h"
#include "interface.h"
#include "GUI/UIListItemBase.h"

class UIBookStoreBookCommentReplyItem : public UIListItemBase
{
public:
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreBookCommentReplyItem";
    }
    UIBookStoreBookCommentReplyItem();
    ~UIBookStoreBookCommentReplyItem();
    void SetInfoSPtr(dk::bookstore::model::BasicObjectSPtr info, int index = -1);
    virtual void MoveWindow(INT32 left, INT32 top, INT32 width, INT32 height);
private:
    void Init();
private:
    UITextSingleLine m_userId;
    UITextSingleLine m_replyTime;
    UIText m_replyContent;
    dk::bookstore::model::CommentReplySPtr m_commentReply;

};

#endif//_DKREADER_APPUI_UIBOOKSTOREBOOKCOMMENTREPLYITEM_H_