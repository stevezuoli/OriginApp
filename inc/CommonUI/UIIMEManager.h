#ifndef __UIIMEMANAGER_H__
#define __UIIMEMANAGER_H__

#include "CommonUI/UIIME.h"
#include "GUI/UIAbstractTextEdit.h"

class UIIMEManager
{
public:
    static void ResetCurrentIME();
    static UIIME* GetCurrentIME();
    static UIIME* GetIME(int mode, UIAbstractTextEdit *pText);
private:
    static UIIME* m_pCurIME;
    static int m_curInputIME;
};

#endif //__UIIMEMANAGER_H__
