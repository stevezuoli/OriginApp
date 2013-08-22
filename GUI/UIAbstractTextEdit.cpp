#include "Utility.h"
#include "GUI/UIAbstractTextEdit.h"
#include "CommonUI/UISoftKeyboardIME.h"
#include "CommonUI/UIIMEManager.h"
#include "Common/WindowsMetrics.h"
#include "drivers/DeviceInfo.h"

using namespace DkFormat;
using namespace WindowsMetrics;

const char* UIAbstractTextEdit::EventTextEditChange = "TextEditEvent";

UIAbstractTextEdit::UIAbstractTextEdit()
    : UIWindow()
    , m_bIsSquareCorner(true)
    , m_nBorderLinePixel(2)
    , m_fIsSelectAll(false)
    , m_inputMode(IUIIME_CHINESE_LOWER)
{
    Init();
}

UIAbstractTextEdit::UIAbstractTextEdit(UIContainer* pParent, DWORD rstrId)
    : UIWindow(pParent, rstrId)
    , m_bIsSquareCorner(true)
    , m_nBorderLinePixel(2)
    , m_fIsSelectAll(false)
    , m_inputMode(IUIIME_CHINESE_LOWER)
{
    Init();
}

UIAbstractTextEdit::~UIAbstractTextEdit()
{
    UIIME* pIME = UIIMEManager::GetCurrentIME();
    if (pIME)
    {
        pIME->DetachTextBox(this);
    }
}

bool UIAbstractTextEdit::OnTextEditChange(const EventArgs& args)
{
    return false;
}

void UIAbstractTextEdit::SelectAll(BOOL selected)
{
    m_fIsSelectAll = selected;
}

BOOL UIAbstractTextEdit::IsAllSelected()
{
    return m_fIsSelectAll;
}

bool UIAbstractTextEdit::IsActionKey(int iKeyCode)
{
    switch(iKeyCode)
    {
        case KEY_LEFTSHIFT_LP:
        case KEY_LEFTALT_LP:
        case KEY_SPACE_LP:
        case KEY_DOT_LP:
        case KEY_SLASH_LP:
        case KEY_VOLUMEUP_LP:
        case KEY_VOLUMEDOWN_LP:
        case KEY_LPAGEUP_LP:
        case KEY_LPAGEDOWN_LP:
        case KEY_SYM_LP:
        case KEY_FONT_LP:
        case KEY_RPAGEUP_LP:
        case KEY_RPAGEDOWN_LP:
        case KEY_OKAY_LP:
        case KEY_DEL_LP:
        case KEY_VOLUMEUP:
        case KEY_VOLUMEDOWN:
        case KEY_LEFTSHIFT:
        case KEY_LPAGEUP:
        case KEY_LPAGEDOWN:
        case KEY_RPAGEUP:
        case KEY_RPAGEDOWN:
        case KEY_DEL:
        case KEY_OKAY:
        case KEY_ENTER:
        case KEY_LEFT:
        case KEY_RIGHT:
        case KEY_UP:
        case KEY_DOWN:
        case KEY_FONT:
        case KEY_BACK:
        case KEY_SYM:
        case KEY_HOME:
        case KEY_MENU:
            return true;
        default:
            return false;
    }
}

char UIAbstractTextEdit::KeyCode2Char(int iKeyCode)
{
    char key = VirtualKeyToChar(iKeyCode);
    if( key != NOTVISIBLEKEY )
    {
        return key;
    }
    else
    {
        switch(iKeyCode)
        {
        case KEY_DOT:
            key = '.';
            break;
        
        case KEY_SPACE:
            key = ' ';
            break;
        default:
            break;
        }
    }
    return key;
}
void UIAbstractTextEdit::SetTextBoxStyle(TextBoxStyle eTextBoxStyle)
{
    m_eTextBoxStyle = eTextBoxStyle;
    if(eTextBoxStyle == PERCENTAGE)
    {
        m_inputMode = IUIIME_NUMBER_ONLY;
    }
    else if(eTextBoxStyle == NUMBER_ONLY)
    {
        m_inputMode = IUIIME_NUMBER_ONLY;
    }
    else if (eTextBoxStyle == ENGLISH_NORMAL)
    {
        m_inputMode = IUIIME_ENGLISH_LOWER;
    }
    else
    {
        m_inputMode = IUIIME_CHINESE_LOWER;
    }
}

TextBoxStyle UIAbstractTextEdit::GetTextBoxStyle ()
{
    return m_eTextBoxStyle;
}

int UIAbstractTextEdit::GetInputMode()
{
    return m_inputMode;
}

void UIAbstractTextEdit::SetMarginLeft(int marginLeft)
{
    m_nMarginLeft = marginLeft;
}

void UIAbstractTextEdit::SetMarginRight(int marginRight)
{
    m_nMarginRight = marginRight;
}

void UIAbstractTextEdit::SetMarginTop(int marginTop)
{
    m_nMarginTop = marginTop;
}

void UIAbstractTextEdit::SetMarginBottom(int marginBottom)
{
    m_nMarginBottom = marginBottom;
}

int UIAbstractTextEdit::GetMarginLeft() const
{
    return m_nMarginLeft;
}

int UIAbstractTextEdit::GetMarginRight() const
{
    return m_nMarginRight;
}

int UIAbstractTextEdit::GetMarginTop() const
{
    return m_nMarginTop;
}

int UIAbstractTextEdit::GetMarginBottom() const
{
    return m_nMarginBottom;
}

void UIAbstractTextEdit::Init()
{
    m_nMarginLeft           = GetWindowMetrics(UITextBoxLeftMarginIndex);
    m_nMarginRight          = GetWindowMetrics(UITextBoxLeftMarginIndex);
    m_nMarginTop            = GetWindowMetrics(UITextBoxTopMarginIndex);
    m_nMarginBottom     = GetWindowMetrics(UITextBoxTopMarginIndex);
    m_minWidth = WidthDefault(m_minWidth);
    m_minHeight = WidthDefault(m_minHeight);
    m_eTextBoxStyle = CHINESE_NORMAL;
    m_inputMode = IUIIME_CHINESE_LOWER;
}

void UIAbstractTextEdit::SetBorderLinePixel(int borderLinePixel)
{
    m_nBorderLinePixel = borderLinePixel;
}

int UIAbstractTextEdit::GetBorderLinePixel() const
{
    return m_nBorderLinePixel;
}

void UIAbstractTextEdit::SetBorderSquareCorner(bool squareCorner)
{
    m_bIsSquareCorner = squareCorner;
}

bool UIAbstractTextEdit::IsBorderSquareCorner() const
{
    return m_bIsSquareCorner;
}

BOOL UIAbstractTextEdit::OnSymPress()
{
#ifndef KINDLE_FOR_TOUCH
    int mode = m_inputMode;
    if (DeviceInfo::IsK3() && (mode != IUIIME_NUMBER_ONLY))
    {
        mode = IUIIME_ENGLISH_NUMBER;
    }
    UIIME* pIME = UIIMEManager::GetIME(mode, this);
    if (pIME)
    {
        if (pIME->IsShow())
        {
            pIME->HideIME();
        }
        else
        {
            pIME->ShowIME();
        }
        return FALSE;
    }
#endif
    return TRUE;
}

BOOL UIAbstractTextEdit::OnBackPress()
{
    return TRUE;
}

BOOL UIAbstractTextEdit::OnFontPress()
{
#ifndef KINDLE_FOR_TOUCH
    UIIME* pIME = UIIMEManager::GetIME(m_inputMode, this);
    if (pIME)
    {
        if (pIME->IsShow())
        {
            pIME->HideIME();
        }
        else
        {
            pIME->ShowIME();
        }
        return FALSE;
    }
#endif
    return TRUE;
}
