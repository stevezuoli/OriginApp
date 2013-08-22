#ifndef __GUI_UITEXTBOXONLYDLG_H__
#define __GUI_UITEXTBOXONLYDLG_H__
#include "GUI/UIDialog.h"
#include "GUI/UITextBox.h"
#include "CommonUI/UIIME.h"

class UITextBoxOnlyDlg: public UIDialog
{
public:
    UITextBoxOnlyDlg(UIContainer* parent);
    virtual const char* GetClassName() const
    {
        return "UITextBoxOnlyDlg";
    }
    void SetFontSize(int size);
    void SetTipUTF8(const char* text);
    void SetTextBoxStyle(TextBoxStyle eTextBoxStyle);
    void SetInitInputMode(INPUTMODE inputmode);
    std::string GetTextUTF8() const;
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
#ifdef KINDLE_FOR_TOUCH
    virtual bool OnHookTouch(MoveEvent* moveEvent);
#endif
    void SetTextBoxImage(int imageId, int focusedImageId);
    BOOL OnKeyPress(int keycode);
private:
    void InitUI();
    UITextBox m_textBox;
};
#endif
