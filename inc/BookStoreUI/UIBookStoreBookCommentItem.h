#ifndef _DKREADER_UIBOOKSTOREBOOKCOMMENTITEM_H_
#define _DKREADER_UIBOOKSTOREBOOKCOMMENTITEM_H_

#include "GUI/UIImage.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UIText.h"
#include "GUI/UIContainer.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UIStars.h"
#include "BookStore/BookStoreInfoManager.h"
#include "interface.h"
#include "GUI/UIListItemBase.h"

class UIBookStoreBookCommentItem : public UIListItemBase
{
public:
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreBookCommentItem";
    }
    UIBookStoreBookCommentItem();
    ~UIBookStoreBookCommentItem();
	void InitUI();
    void SetInfoSPtr(dk::bookstore::model::BasicObjectSPtr commentInfo, int index = -1);

    virtual bool ResponseTouchTap();
private:
    void Init();
    bool OnMessageVoteBookComment(const EventArgs& args);

private:
    UITextSingleLine m_title;
    UITextSingleLine m_commenterNick;
    UITextSingleLine m_publishTime;
    UITextSingleLine m_usefulCount;
    UITextSingleLine m_replayCount;
    UITextSingleLine m_viewAll;
    UIText  m_content;
    UIStars          m_star;
    dk::bookstore::model::BookCommentSPtr    m_bookComment;
};
#endif//_DKREADER_UIBOOKSTOREBOOKCOMMENTITEM_H_
