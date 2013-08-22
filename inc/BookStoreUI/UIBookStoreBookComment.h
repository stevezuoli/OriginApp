#ifndef _DKREADER_TOUCHAPPUI_UIBOOKSTOREBOOKCOMMENT_H_
#define _DKREADER_TOUCHAPPUI_UIBOOKSTOREBOOKCOMMENT_H_

#include "CommonUI/UITablePanel.h"
#include "GUI/UIImage.h"
#include "GUI/UIImageTransparent.h"
#include "GUI/UIMenu.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UIStars.h"
#include "BookStoreUI/UIBookStoreListBoxWithPageNum.h"
#include "BookStoreUI/UIDiscountTextSingleLine.h"
#include "BookStoreUI/UIBookStoreFetchInfo.h"
#include "BookStore/BookStoreInfoManager.h"
#ifdef KINDLE_FOR_TOUCH
    #include "GUI/UITouchComplexButton.h"
    #include "TouchAppUI/UITouchMenu.h"
#else
    #include "GUI/UIComplexButton.h"
    #include "GUI/UIMenu.h"
#endif

/********************************************
* UIBookStoreBookNoComment
********************************************/
class UIBookStoreBookNoComment : public UIContainer
{
public:
    UIBookStoreBookNoComment();
    ~UIBookStoreBookNoComment();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreBookNoComment";
    }
    void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    virtual void SetFocus(BOOL bIsFocus);
    virtual BOOL OnKeyPress(INT32 iKeyCode);
    void UpdateAddCommentStatus();
private:
    void Init();
private:
    UIImageTransparent m_imgNoComment;
    UITextSingleLine m_noCommentLable;
    UITextSingleLine m_addCommentLine;
#ifdef KINDLE_FOR_TOUCH
    UITouchComplexButton  m_addComment;
#else
    UIComplexButton  m_addComment;
#endif
};


/********************************************
* UIBookStoreBookComment
********************************************/
class UIBookStoreBookComment : public UITablePanel
{
public:
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreBookComment";
    }
    UIBookStoreBookComment(const std::string& bookId, const std::string& bookName);
    ~UIBookStoreBookComment();

    virtual BOOL OnKeyPress(INT32 iKeyCode);
    void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
    dk::bookstore::FetchDataResult FetchInfo();

private:
    void Init();
    void RequestBookScoreByCurrentUser();
    void RequestBookComment();
    void RequestBookScores();
    bool OnMessageBookScoreUpdate(const EventArgs& args);
    bool OnMessageListBoxUpdate(const EventArgs& args);
    bool OnMessageBookScoreByCurrentUserUpdate(const EventArgs& args);
    bool OnMessageAddBookScoreUpdate(const EventArgs& args);
    bool OnMessageVoteBookComment(const EventArgs& args);
    bool LogDataChanged(const EventArgs& args);
    bool CheckAndReLogin();
    bool IsPurchasedBook();
    void AddBookScore();
    void UpdateMyScore();
    void UpdateTotalScore();
    void CheckFetchReturn();
    std::string GetSortString();
    void ResetSortType(DWORD newSortType);
#ifdef KINDLE_FOR_TOUCH
    bool OnMessageListBoxTurnPage(const EventArgs& args);
#endif

private:
    std::string        m_bookId;
    std::string        m_bookName;
    UISizer* m_commentSizer;
    UIBookStoreBookNoComment m_noCommentWnd;
    UIBookStoreListBoxWithPageNum m_listBoxPanel;

    FETCH_STATUS m_fetchMyScoreReturn;
    FETCH_STATUS m_fetchTotalScoreReturn;
    FETCH_STATUS m_fetchListBoxReturn;
    bool m_isPurchased;

    UITextSingleLine m_myScoreLable;
    UITextSingleLine m_myScoreInfoLable;
    UIStars          m_totalStar;
    UIStars          m_myScore;
    UITextSingleLine m_totalStarLabel;
    dk::bookstore::model::BookScoreSPtr   m_bookScoreInfo;
    UITextSingleLine   m_userCommentLabel;
    UISizer* m_pMyScoreSizer;
    UISizer* m_pTotalScoreSizer;
    DWORD m_prevSortId;
    DWORD m_curSortId;
#ifdef KINDLE_FOR_TOUCH
    UITouchComplexButton  m_addComment;
    UITouchComplexButton  m_btnSort;
#else
    UIComplexButton  m_addComment;
    UIComplexButton  m_btnSort;
    UIMenu m_mnuSort;
#endif
};

#endif//_DKREADER_TOUCHAPPUI_UIBOOKSTOREBOOKCOMMENT_H_

