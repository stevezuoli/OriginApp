#include "CommonUI/UIIME.h"
#include "CommonUI/UIIMEManager.h"

UIIME::UIIME()
    : m_fIsShow(false)
    , m_isUsing(false)
    , m_showDelay(false)
    , m_pTextBox(NULL)
{

}

UIIME::~UIIME()
{
    m_pTextBox = NULL;
}

void UIIME::SetTextBox(UIAbstractTextEdit *pTextBox)
{
    if (m_pTextBox != pTextBox)
    {
        m_pTextBox = pTextBox;
    }
}

void UIIME::DetachTextBox(const UIAbstractTextEdit *pTextBox)
{
    if (m_pTextBox == pTextBox)
    {
        m_pTextBox = NULL;
    }
}

const UIAbstractTextEdit* UIIME::GetTextBox() const
{
    return m_pTextBox;
}

bool UIIME::IsShow()
{
    return m_fIsShow;
}

void UIIME::SetShow(bool show)
{
    m_fIsShow = show;
}

bool UIIME::IsShowDelay()
{
    return m_showDelay;
}

void UIIME::SetShowDelay(bool show)
{
    m_showDelay = show;
}

void UIIME::HideIME()
{
    if (IsShow())
    {
        SetShow(false);
        UIIMEManager::ResetCurrentIME();

        if (m_pParent)
        {
            m_pParent->UpdateWindow();
            m_pParent->RemoveChild(this, false);
            m_pParent = NULL;
        }
    }
}

BOOL UIIME::OnKeyPress(INT32 iKeyCode)
{
    if(!IsShow())
    {
        return TRUE;
    }

    switch(iKeyCode)
    {
    case KEY_BACK:
        HideIME();
        return FALSE;
    case KEY_HOME:
        HideIME();
        return TRUE;
    default:
        break;
    }
    return TRUE;
}

