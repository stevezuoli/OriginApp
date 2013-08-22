#ifndef _DKREADER_APPUI_UIBOOKSTOREBOOKCOMMENTREPLYDLG_H_
#define _DKREADER_APPUI_UIBOOKSTOREBOOKCOMMENTREPLYDLG_H_

#include "GUI/UITextSingleLine.h"
#include "GUI/UIPlainTextEdit.h"
#include "BookStoreUI/UIBookStoreNavigation.h"
#ifdef KINDLE_FOR_TOUCH
    #include "GUI/UITouchComplexButton.h"
#else
    #include "GUI/UIComplexButton.h"
#endif

class UIBookStoreBookCommentReplyPage : public UIBookStoreNavigation
{
public:
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreBookCommentReplyPage";
    }
    UIBookStoreBookCommentReplyPage(std::string bookName, std::string bookId, std::string commentId);
    ~UIBookStoreBookCommentReplyPage();

    UISizer* CreateMidSizer();
    void SetFetchStatus(FETCH_STATUS status);

    void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
private:
    void Init();
    bool OnMessageAddBookCommentReplyUpdate(const EventArgs& args);
    void AddCommentReply();
    void UpdateWindowsStatus(bool submitted);
private:
    UITextSingleLine m_addReplyLabel;
    UITextSingleLine m_replyLabel;
    UIPlainTextEdit m_inputBox;
    UITextSingleLine m_submitting;
    std::string m_bookId;
    std::string m_commentId;
#ifdef KINDLE_FOR_TOUCH
    UITouchComplexButton m_cancelBtn;
    UITouchComplexButton m_okBtn;
    UITouchComplexButton m_viewAllReplyBtn;
#else
    UIComplexButton m_cancelBtn;
    UIComplexButton m_okBtn;
    UIComplexButton m_viewAllReplyBtn;
#endif
};

#endif//_DKREADER_APPUI_UIBOOKSTOREBOOKCOMMENTREPLYDLG_H_
