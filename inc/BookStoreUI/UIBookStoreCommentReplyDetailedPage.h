#ifndef _DKREADER_APPUI_UIBOOKSTORECOMMENTREPLYDETAILEDPAGE_H_
#define _DKREADER_APPUI_UIBOOKSTORECOMMENTREPLYDETAILEDPAGE_H_

#include "BookStoreUI/UIBookStoreNavigationWithFling.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UIText.h"

class UIBookStoreCommentReplyDetailedPage : public UIBookStoreNavigationWithFling
{
public:
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreBookCommentDetailedPage";
    }
    UIBookStoreCommentReplyDetailedPage(std::string bookName, const dk::bookstore::model::CommentReplySPtr replyInfo);
    virtual ~UIBookStoreCommentReplyDetailedPage();

    UISizer* CreateMidSizer();
    void SetFetchStatus(FETCH_STATUS status);

    void UpdatePageNumber();
    bool TurnPage(bool pageDown);
    
private:
    void Init();
private:
    UITextSingleLine m_userId;
    UITextSingleLine m_replyTime;
    UIText m_replyContent;
    UITextSingleLine m_pageNumber;
};

#endif//_DKREADER_APPUI_UIBOOKSTORECOMMENTREPLYDETAILEDPAGE_H_
