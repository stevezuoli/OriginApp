#ifndef _DKREADER_UBOXMESSAGESCOMMENTREPLYITEM_H_
#define _DKREADER_UBOXMESSAGESCOMMENTREPLYITEM_H_

#include "GUI/UITextSingleLine.h"
#include "GUI/UIImage.h"
#include "BookStore/BookStoreInfoManager.h"
#include "BookStore/PushedMessagesInfo.h"
#include "GUI/UIListItemBase.h"
#include "GUI/UIText.h"

class UIBoxMessagesCommentReplyItem : public UIListItemBase
{
public:
    virtual LPCSTR GetClassName() const
    {
        return "UIBoxMessagesCommentReplyItem";
    }
    UIBoxMessagesCommentReplyItem();
    ~UIBoxMessagesCommentReplyItem();
    void SetInfoSPtr(dk::bookstore::model::BasicObjectSPtr messageInfo, int index = -1);

    virtual bool ResponseTouchTap();
    virtual bool ResponseOperation();
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
private:
    void Init();
    bool OnMessageCommentDetailedInfoUpdate(const EventArgs& args);
    dk::bookstore::FetchDataResult FetchCommentReply(const std::string& bookId, const std::string& commentId, int start, int length);
private:
    UITextSingleLine m_time;
    UITextSingleLine m_replyAlias;
    UITextSingleLine m_replyAliasFollow;
    UIText m_replyContent;
    UITextSingleLine m_comment;
    UIImage m_imgComment;
    UIImage m_imgArrow;
    dk::bookstore::model::PushedMessageInfoSPtr m_messageInfo;
};
#endif//_DKREADER_UBOXMESSAGESCOMMENTREPLYITEM_H_
