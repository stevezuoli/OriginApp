#ifndef _DKREADER_TOUCHAPPUI_UIBOOKSTOREADDCOMMENTPAGE_H_
#define _DKREADER_TOUCHAPPUI_UIBOOKSTOREADDCOMMENTPAGE_H_

#include "GUI/UITextBox.h"
#include "GUI/UIPlainTextEdit.h"
#include "GUI/UIPage.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UIStars.h"
#include "BookStoreUI/UIBookStoreNavigation.h"
#ifdef KINDLE_FOR_TOUCH
    #include "GUI/UITouchComplexButton.h"
#else
    #include "GUI/UIComplexButton.h"
#endif

class UIBookStoreAddCommentPage : public UIBookStoreNavigation
{
public:
    static const int MYSCORE_STARS_MARGIN = 10;
    static const int COMMENT_MINLENGTH = 20;
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreAddCommentPage";
    }
    UIBookStoreAddCommentPage(const std::string& bookId,const std::string& bookName);
    ~UIBookStoreAddCommentPage();

    UISizer* CreateMidSizer();
    void SetFetchStatus(FETCH_STATUS status);

    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    virtual BOOL OnKeyPress(INT32 iKeyCode);
#ifndef KINDLE_FOR_TOUCH
    HRESULT MoveFocus(MoveDirection dir, BOOL bLoop = false);
#endif

private:
    void Init();
    void RequestBookScoreByCurrentUser();
    bool OnMessageBookScoreByCurrentUserUpdate(const EventArgs& args);
    bool OnMessageAddBookScoreUpdate(const EventArgs& args);
    bool OnMessageAddBookCommentUpdate(const EventArgs& args);
    void CheckAddComment();
    bool UpdateSubmitStatus(const EventArgs& args);
    void AddBookScore();
    void BackToPrePage();
private:
    UITextSingleLine m_addCommentLabel1;
    UITextSingleLine m_addCommentLabel2;
    UITextSingleLine m_starLevelLabel;
    UIStars          m_starLevel;
    UITextSingleLine m_commentTitleLabel;
    UITextBox        m_commentTitleBox;
    UITextSingleLine m_commentLabel;
    UIPlainTextEdit  m_commentBox;
    std::string m_bookId;

#ifdef KINDLE_FOR_TOUCH
    UITouchComplexButton  m_cancel;
    UITouchComplexButton  m_ok;
#else
    UIComplexButton  m_cancel;
    UIComplexButton  m_ok;
#endif
};

#endif//_DKREADER_TOUCHAPPUI_UIBOOKSTOREADDCOMMENTPAGE_H_

