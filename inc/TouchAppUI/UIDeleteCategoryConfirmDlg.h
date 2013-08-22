#ifndef __TOUCHAPPUI_UIDELETECATEGORYCONFIRMDLG_H__
#define __TOUCHAPPUI_UIDELETECATEGORYCONFIRMDLG_H__

#include "GUI/UIDialog.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UICheckButton.h"
class UIDeleteCategoryConfirmDlg: public UIDialog
{
public:
    UIDeleteCategoryConfirmDlg(UIContainer* parent);
    virtual const char* GetClassName() const
    {
        return "UIDeleteCategoryConfirmDlg";
    }
    virtual bool OnChildClick(UIWindow* child);
    HRESULT DrawBackGround(DK_IMAGE drawingImg);
    bool ShouldDeleteBooks() const;
private:
    void InitUI();

    UITextSingleLine m_txtHint;
    UICheckButton m_btnDeleteBooks;
    UITouchComplexButton m_btnCancel;
    UITouchComplexButton m_btnOK;
};
#endif
