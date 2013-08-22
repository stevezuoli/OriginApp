#ifndef _DKREADER_APPUI_UIBOOKSTOREBOOKCOMMENTDETAILEDPAGE_H_
#define _DKREADER_APPUI_UIBOOKSTOREBOOKCOMMENTDETAILEDPAGE_H_

#include "BookStoreUI/UIBookStoreNavigationWithFling.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UIText.h"
#include "GUI/UIStars.h"
#include "GUI/UITouchCommonListBox.h"
#include "BookStore/BookStoreInfoManager.h"
#ifdef KINDLE_FOR_TOUCH
    #include "GUI/GestureDetector.h"
    #include "GUI/UITouchComplexButton.h"
#else
    #include "GUI/UIComplexButton.h"
#endif

typedef struct ViewAttribute
{
    UIWindow *pView;
    bool isEnable;
    int left;
    int top;
    int width;
    int height;
    int listboxStartIndex;
    int listboxEndIndex;
} VIEW_ATTRIBUTE;

typedef struct LayoutIndexItem 
{
    std::vector<VIEW_ATTRIBUTE> viewList;
    void InsertView(UIWindow *pView, bool isEnable, int startIndex = 0, int endIndex = 0)
    {
        if (!pView)
        {
            return;
        }
        VIEW_ATTRIBUTE userid = {pView, isEnable, pView->GetX(), pView->GetY(), pView->GetWidth(), pView->GetHeight(), startIndex, endIndex};
        viewList.push_back(userid);
    }
    VIEW_ATTRIBUTE GetAppointViewAttribute(UIWindow *pView)
    {
        for (std::vector<VIEW_ATTRIBUTE>::iterator itr = viewList.begin(); itr != viewList.end(); itr++)
        {
            if (itr->pView == pView)
            {
                return (*itr);
            }
        }
        VIEW_ATTRIBUTE tmpAttribute;
        tmpAttribute.pView = NULL;
        return tmpAttribute;
    }
} LAYOUT_INDEX_ITEM;

class UIBookStoreBookCommentDetailedPage : public UIBookStoreNavigationWithFling
{
public:
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreBookCommentDetailedPage";
    }
    UIBookStoreBookCommentDetailedPage(const std::string& bookId,const std::string& bookName, bool showRefresh = false);
    ~UIBookStoreBookCommentDetailedPage();

    UISizer* CreateMidSizer();
    void SetFetchStatus(FETCH_STATUS status);

    virtual void MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight);
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
    void SetCommentInfo(const dk::bookstore::model::BookCommentSPtr bookComment);
    BOOL OnKeyPress(INT32 iKeyCode);
    void UpdatePageNumber();
    bool TurnPage(bool pageDown);
    void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    virtual UISizer* CreateNavigationRightSizer();
private:
    void Init();
    void ReLayout();
    void UpdatePageView();
    bool OnMessageVoteBookComment(const EventArgs& args);
    bool OnMessageListBoxSelectedItemClicked(const EventArgs& args);
    bool OnMessageListBoxTurnPage(const EventArgs& args);
    void VoteBookComment();
    void ViewBookInfo();

private:
    UITextSingleLine m_commentTitle;
    UITextSingleLine m_userId;
    UITextSingleLine m_commentTime;
    UIText m_commentContent;
    UITextSingleLine m_commentReplyLabel;
    UITouchCommonListBox m_replyList;
    UITextSingleLine m_pageNumberLabel;
    UIStars m_star;
    std::string m_bookId;
    std::string m_bookName;
    std::vector <LAYOUT_INDEX_ITEM> m_layoutIndex;
    dk::bookstore::model::BookCommentSPtr m_bookComment;
    bool m_showRefresh;
    bool moveWindowDown;
    bool setCommentInfoDown;

#ifdef KINDLE_FOR_TOUCH
    UITouchComplexButton m_usefulButton;
    UITouchComplexButton m_replyButton;
#else
    UITitleBar m_titleBar;
    UIComplexButton m_usefulButton;
    UIComplexButton m_replyButton;
#endif
};
#endif//_DKREADER_APPUI_UIBOOKSTOREBOOKCOMMENTDETAILEDPAGE_H_
