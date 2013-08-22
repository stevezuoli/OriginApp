//==============================================================================
// Summary:
//        UICommentEditDialog: ±à¼­Åú×¢¶Ô»°¿ò
// Usage:
//
// Remarks:
//      
// Date:
//        2013-04-08
// Author:
//        juguanhui@duokan.com
//==============================================================================

#ifndef _UICOMMENTEDITDIALOG_H_
#define _UICOMMENTEDITDIALOG_H_

#include "GUI/UIDialog.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UIPlainTextEdit.h"
#include "GUI/UIButtonGroup.h"
#ifdef KINDLE_FOR_TOUCH
#include "GUI/UITouchComplexButton.h"
#else
#include "GUI/UIComplexButton.h"
#endif

enum EDIT_RESULT
{
    GOTO_COMMENTORDIGEST = 0x3000,
    DELETE_COMMENT,
    SAVE_COMMENT,
};
class UICommentEditDialog : public UIDialog
{
public:
    virtual LPCSTR GetClassName() const
    {
        return "UICommentEditDialog";
    }

    UICommentEditDialog(UIContainer* pParent, int commentIndex = -1, std::string comment = std::string());
    virtual ~UICommentEditDialog();

    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
    virtual void OnCommand(DWORD _dwCmdId, UIWindow* _pSender, DWORD _dwParam);

#ifdef KINDLE_FOR_TOUCH
    virtual bool OnHookTouch(MoveEvent* moveEvent);
#endif
    virtual std::string GetComment() { return m_boxComment.GetTextUTF8(); }
    DWORD GetCmdID() const { return  m_resultCmdId; }
private:
    void InitUI();
    bool OnInputChange(const EventArgs& args);
    bool OnButtonClicked(const EventArgs& args);

    int m_commentIndex;
    UITextSingleLine m_txtTitle;
    UIPlainTextEdit m_boxComment;
#ifdef KINDLE_FOR_TOUCH
    UITouchComplexButton m_btnViewComment;
    UITouchComplexButton m_btnDeleteComment;
    UITouchComplexButton m_btnExitOrCancel;
    UITouchComplexButton m_btnSave;
#else
    UIComplexButton m_btnViewComment;
    UIComplexButton m_btnDeleteComment;
    UIComplexButton m_btnExitOrCancel;
    UIComplexButton m_btnSave;
#endif
    UIButtonGroup m_btnGroup;
    std::string  m_strPreComment;
    DWORD m_resultCmdId;
};

#endif //_UICOMMENTEDITDIALOG_H_

