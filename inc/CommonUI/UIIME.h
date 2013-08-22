#ifndef __IUIIME_H__
#define __IUIIME_H__

#include "GUI/UIContainer.h"
#include "GUI/UIAbstractTextEdit.h"
#include "IME/IIMEBase.h" 
#include <vector>

// INPUTMODE & 0x10 = {0, ENGLISH; 1, CHINESE}
// INPUTMODE & 0x2 = {0, LOWER; 1, UPPER}
// INPUTMODE & 0x4 = {1, NUMBER}
// INPUTMODE & 0x8 = {1, SYMBOL}
enum IUIIME_TYPE
{
    IUIIME_USED = 0x1,
    IUIIME_NUMBER = 0x4,
    IUIIME_SYMBOL = 0x2,
    IUIIME_ENGLISH = 0x0,
    IUIIME_CHINESE = 0x10,
    IUIIME_LOWER = 0x0,
    IUIIME_UPPER = 0x8,
    IUIIME_ONLY = 0x20,
};
enum INPUTMODE
{
    IUIIME_NONE = 0x0,
    IUIIME_ENGLISH_LOWER  = IUIIME_USED | IUIIME_ENGLISH | IUIIME_LOWER, // 0x1, 
    IUIIME_ENGLISH_UPPER  = IUIIME_USED | IUIIME_ENGLISH | IUIIME_UPPER, // 0x9,
    IUIIME_ENGLISH_NUMBER = IUIIME_USED | IUIIME_ENGLISH | IUIIME_NUMBER, // 0x5,
    IUIIME_ENGLISH_SYMBOL = IUIIME_USED | IUIIME_ENGLISH | IUIIME_SYMBOL, // 0x3,
    IUIIME_CHINESE_LOWER  = IUIIME_USED | IUIIME_CHINESE | IUIIME_LOWER, // 0x11, 
    IUIIME_CHINESE_UPPER  = IUIIME_USED | IUIIME_CHINESE | IUIIME_UPPER, // 0x19,
    IUIIME_CHINESE_NUMBER = IUIIME_USED | IUIIME_CHINESE | IUIIME_NUMBER, // 0x15,
    IUIIME_CHINESE_SYMBOL = IUIIME_USED | IUIIME_CHINESE | IUIIME_SYMBOL, // 0x13,
    IUIIME_NUMBER_ONLY    = IUIIME_USED | IUIIME_ONLY | IUIIME_NUMBER, // 0x25,
    IUIIME_END = 0xFF,
};

class UIIME : public UIContainer
{
public:
    UIIME();
    virtual ~UIIME();

    virtual void SetTextBox(UIAbstractTextEdit *pTextBox);
    virtual void DetachTextBox(const UIAbstractTextEdit *pTextBox);
    virtual const UIAbstractTextEdit* GetTextBox() const;
    virtual void SetInputMode(int mode){ }
    //virtual int GetInputMode() const = 0;
    virtual bool IsShow();
    virtual void SetShow(bool show);
    virtual bool IsShowDelay();
    virtual void SetShowDelay(bool show);
    virtual void ShowIME(UIContainer *pParent = NULL) = 0;
    virtual void HideIME();
    virtual BOOL OnKeyPress(INT32 iKeyCode);

    // TODO(JUGH:) 使用UIIMEManager后删除后面
    virtual void UseIMEManager(bool isUsing) { m_isUsing = isUsing; }
    virtual bool IsUsingIMEManager() const { return m_isUsing; }

protected:
    bool m_fIsShow;
    bool m_isUsing;
    bool m_showDelay;
    UIAbstractTextEdit* m_pTextBox;
};

#endif //__IUIIME_H__
