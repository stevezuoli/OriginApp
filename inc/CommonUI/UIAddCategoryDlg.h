#ifndef _TOUCHAPPUI_UIADDCATEGORYDLG_H__
#define _TOUCHAPPUI_UIADDCATEGORYDLG_H__

#include "GUI/UIDialog.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UITextBox.h"
#include "GUI/UIButtonGroup.h"
#ifndef KINDLE_FOR_TOUCH
#include "GUI/UIComplexButton.h"
#endif

class UIAddCategoryDlg: public UIDialog
{
public:
    enum CategoryOperation
    {
        CAT_ADD = 0,
        CAT_RENAME,
    };
public:
    UIAddCategoryDlg(UIContainer* parent, CategoryOperation opt);
    virtual const char* GetClassName() const
    {
        return "UIAddCategoryDlg";
    }
    virtual HRESULT DrawBackGround(DK_IMAGE _drawingImg);
    std::string GetInputText();
    virtual bool OnChildClick(UIWindow* child);
#ifndef KINDLE_FOR_TOUCH
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
#endif
private:
    bool OnInputChange(const EventArgs& args);
    void UpdateSaveButtonStatus();
    void InitUI();
    UITextSingleLine m_txtHint;
    UITextBox m_input;
#ifdef KINDLE_FOR_TOUCH
    UITouchComplexButton m_btnCancel;
    UITouchComplexButton m_btnSave;
#else
    UIComplexButton m_btnCancel;
    UIComplexButton m_btnSave;
#endif
    UIButtonGroup m_btnCancelSaveGroup;
    CategoryOperation m_catOpt;
};

#endif
