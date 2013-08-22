#include "CommonUI/UIIMEManager.h"
#include "drivers/DeviceInfo.h"
#include "CommonUI/UISoftKeyboardIME.h"
#ifndef KINDLE_FOR_TOUCH
#include "IME/IMEManager.h"
#include "AppUI/UINumInput.h"
#include "AppUI/UIIM.h"
#include "AppUI/UINumTip.h"
#else
#include "CommonUI/UINumInputIME.h"
#endif

UIIME* UIIMEManager::m_pCurIME = NULL;
int UIIMEManager::m_curInputIME = IUIIME_NONE;

void UIIMEManager::ResetCurrentIME()
{
    m_pCurIME = NULL;
}

UIIME* UIIMEManager::GetCurrentIME()
{
    return m_pCurIME;
}

UIIME* UIIMEManager::GetIME(int mode, UIAbstractTextEdit *pText)
{
    if (mode <= IUIIME_NONE || mode >= IUIIME_END)
    {
        return NULL;
    }

    if ((m_curInputIME == mode) && m_pCurIME)
    {
        return m_pCurIME;
    }

    m_curInputIME = mode;
    if (m_pCurIME)
    {
        m_pCurIME->HideIME();
    }
#ifdef KINDLE_FOR_TOUCH
	switch(mode)
	{
	case IUIIME_NUMBER_ONLY:
	    m_pCurIME = UINumInputIME::GetInstance();
	    break;
	default:
	    m_pCurIME = UISoftKeyboardIME::GetInstance();
	    break;
	}
#else
    if(DeviceInfo::IsK4Series()) 
    {
        m_pCurIME = NULL;
        switch(mode)
        {
        case IUIIME_NUMBER_ONLY:
            m_pCurIME = UINumInput::GetInstance();
            break;
        default:
            m_pCurIME = UISoftKeyboardIME::GetInstance();
            break;
        }
    }
    else
    {
        m_pCurIME = NULL;
        switch(mode)
        {
        case IUIIME_ENGLISH_LOWER:
        case IUIIME_ENGLISH_UPPER:
            return NULL;
        case IUIIME_CHINESE_LOWER:
            m_pCurIME = UIIM::GetInstance();
            break;
        case IUIIME_ENGLISH_NUMBER:
        case IUIIME_ENGLISH_SYMBOL:
            m_pCurIME = UISoftKeyboardIME::GetInstance();
            break;
        case IUIIME_NUMBER_ONLY:
            m_pCurIME = UINumTip::GetInstance();
            break;
        default:
            break;
        }
    }
#endif

    if (m_pCurIME)
    {
        m_pCurIME->SetInputMode(mode);
        m_pCurIME->SetTextBox(pText);
        m_pCurIME->UseIMEManager(true);
    }
    return m_pCurIME;
}

