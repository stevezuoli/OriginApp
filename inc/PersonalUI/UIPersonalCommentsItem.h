#ifndef _DKREADER_UPERSONALCOMMENTSITEM_H_
#define _DKREADER_UPERSONALCOMMENTSITEM_H_

#include "GUI/UIImage.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UIText.h"
#include "GUI/UIContainer.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UIStars.h"
#include "BookStoreUI/UIBookCoverView.h"
#include "BookStore/BookStoreInfoManager.h"
#include "interface.h"
#include "GUI/UIListItemBase.h"

class UIPersonalCommentsItem : public UIListItemBase
{
public:

    virtual LPCSTR GetClassName() const
    {
        return "UIPersonalCommentsItem";
    }
    UIPersonalCommentsItem();
    ~UIPersonalCommentsItem();
    void InitUI();
    void SetInfoSPtr(dk::bookstore::model::BasicObjectSPtr commentInfo, int index = -1);

    virtual bool ResponseTouchTap();
#ifndef KINDLE_FOR_TOUCH
    virtual bool ResponseOperation();
#endif

private:
    void Init();
    void ShowBookDetailedInfoPage();
    bool OnMessageVoteBookComment(const EventArgs& args);
    bool OnMessageBookCoverTouch(const EventArgs& args);

private:
    //UITextSingleLine m_bookName;
    UITextSingleLine m_title;
    UITextSingleLine m_commenterNick;
    UITextSingleLine m_publishTime;
    UITextSingleLine m_usefulCount;
    UITextSingleLine m_replyCount;
    UITextSingleLine m_deviceType;
    //UITextSingleLine m_viewAll;
    UIText  m_content;
    UIStars          m_star;
    UIBookCoverView m_bookCover;
    dk::bookstore::model::BookCommentSPtr    m_bookComment;
};
#endif//_DKREADER_UPERSONALCOMMENTSITEM_H_
