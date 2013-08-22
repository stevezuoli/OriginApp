#include "GUI/UIPlainTextEdit.h"
#include "GUI/UIContainer.h"
#include "GUI/CTpGraphics.h"
#include "CommonUI/UIIMEManager.h"
#include "CommandID.h"
#include "drivers/DeviceInfo.h"
#include "Common/WindowsMetrics.h"
#include <tr1/functional>
using namespace WindowsMetrics;

UIPlainTextEdit::UIPlainTextEdit()
    : m_curPage(0)
    , m_buttonHeight(25)
{
    Init();
}

UIPlainTextEdit::UIPlainTextEdit(UIContainer* pParent, DWORD rstrId)
    : m_curPage(0)
    , m_buttonHeight(25)
{
    Init();
}

UIPlainTextEdit::~UIPlainTextEdit()
{
    UnInit();
}

void UIPlainTextEdit::Init()
{
    EventListener::SubscribeEvent(
        TextEditController::EventTextEditChange,
        m_textEditController, 
        std::tr1::bind(
        std::tr1::mem_fn(&UIPlainTextEdit::OnTextEditChange),
        this,
        std::tr1::placeholders::_1));
#ifdef KINDLE_FOR_TOUCH
    m_leftImage = ImageManager::GetImage(IMAGE_TOUCH_ARROW_LEFT);
    m_leftDisImage = ImageManager::GetImage(IMAGE_TOUCH_ARROW_LEFT_DISABLED);
    m_rightImage = ImageManager::GetImage(IMAGE_TOUCH_ARROW_RIGHT);
    m_rightDisImage = ImageManager::GetImage(IMAGE_TOUCH_ARROW_RIGHT_DISABLED);
    
#else
    m_leftImage = ImageManager::GetImage(IMAGE_ARROW_LEFT);
    m_leftDisImage = ImageManager::GetImage(IMAGE_ARROW_LEFT_DISABLED);
    m_rightImage = ImageManager::GetImage(IMAGE_ARROW_RIGHT);
    m_rightDisImage = ImageManager::GetImage(IMAGE_ARROW_RIGHT_DISABLED);
#endif

    m_pageInfos.SetFontSize(12);
    m_pageInfos.SetAlign(ALIGN_CENTER);
    m_pageInfos.SetText("0 / 0");
    m_nBorderLinePixel = GetWindowMetrics(UIPlainTextEditBorderLinePixelIndex);
    
    m_minHeight = m_textEditController.GetMinHeight() + m_nMarginTop + m_nMarginBottom + m_buttonHeight;
}

void UIPlainTextEdit::UnInit()
{
}

bool UIPlainTextEdit::OnTextEditChange(const EventArgs& args)
{
    TextEditChangeEventArgs textEditChangeEventArgs = (const TextEditChangeEventArgs&)(args);
    if (textEditChangeEventArgs.textEditChangeType == eTextEditCurPageChanged)
        m_curPage = m_textEditController.GetCursorPageIndex();

    return true;
}

HRESULT UIPlainTextEdit::DrawBackGround(DK_IMAGE drawingImg)
{
    HRESULT hr(S_OK);
    CTpGraphics grf(drawingImg);

    if (m_bIsSquareCorner)
    {
        grf.FillRect(0, 0, m_iWidth, m_iHeight, ColorManager::knBlack);
        hr = grf.FillRect(m_nBorderLinePixel, m_nBorderLinePixel, m_iWidth - m_nBorderLinePixel, m_iHeight - m_nBorderLinePixel, ColorManager::knWhite);
    }
    else
    {
        SPtr<ITpImage> spImg = ImageManager::GetImage(IMAGE_TEXTBOX_BACK);
        if (spImg)
        {
            RTN_HR_IF_FAILED(grf.DrawImageStretchBlend(spImg.Get(), 0, 0, m_iWidth, m_iHeight,1,10));
        }
    }
    return hr;
}

HRESULT UIPlainTextEdit::DrawPageBar(DK_IMAGE& image)
{
    HRESULT hr(S_OK);
    CTpGraphics grf(image);

    int left = m_iWidth/3;
    int top = m_iHeight - m_buttonHeight;
    int width = m_leftImage.Get()->GetWidth();
    int height = m_leftImage.Get()->GetWidth();
    left -= width;
    top += (m_pageInfos.GetTextHeight() - height)>>1;
    int totalPages = m_textEditController.GetTotalPages();
    if (m_curPage == 0)
    {
        grf.DrawImage(m_leftDisImage.Get(), left, top);
    }
    else
    {
        grf.DrawImage(m_leftDisImage.Get(), left, top);
    }
    int right = m_iWidth/3*2;
    if (m_curPage >= totalPages-1)
    {
        grf.DrawImage(m_rightDisImage.Get(), right, top);
    }
    else
    {
        grf.DrawImage(m_rightDisImage.Get(), right, top);
    }
    char cText[50] = {0};
    snprintf(cText, sizeof(cText)/sizeof(cText[0]), "%d / %d", (m_curPage + 1), totalPages);
    m_pageInfos.SetText(cText);
    RTN_HR_IF_FAILED(m_pageInfos.Draw(image));

    return hr;
}

HRESULT UIPlainTextEdit::Draw(DK_IMAGE drawingImg)
{
    HRESULT hr(S_OK);
    DK_IMAGE img;
    DK_RECT rect={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
    RTN_HR_IF_FAILED(CropImage(drawingImg,rect,&img));

    RTN_HR_IF_FAILED(DrawBackGround(img));
    if (m_textEditController.GetTotalPages() > 1)
    {
        DrawPageBar(img);
    }
#ifdef KINDLE_FOR_TOUCH
    m_textEditController.SetShowCursor(IsFocus());
#else
    m_textEditController.SetShowCursor(IsEnable() && IsFocus());
#endif
    DK_IMAGE imageForRender;
    DK_RECT rectForRender={m_iLeft + m_nMarginLeft, m_iTop + m_nMarginTop, 
        m_iLeft + m_iWidth + m_nMarginLeft - m_nMarginRight, m_iTop + m_iHeight + m_nMarginTop - m_nMarginBottom};
    RTN_HR_IF_FAILED(CropImage(drawingImg,rectForRender,&imageForRender));
    m_textEditController.Render(imageForRender, m_curPage);

    return hr;
}

void UIPlainTextEdit::MoveWindow(int iLeft, int iTop, int iWidth, int iHeight)
{
    m_textEditController.SetPageWidth(iWidth - m_nMarginLeft - m_nMarginRight);
    m_textEditController.SetPageHeight(iHeight - m_buttonHeight - m_nMarginTop - m_nMarginBottom);

    int height = m_pageInfos.GetTextHeight();
    int top = iHeight - m_buttonHeight;
    m_pageInfos.MoveWindow(iWidth/3, top, iWidth/3, height);

    UIWindow::MoveWindow(iLeft, iTop, iWidth, iHeight);
}
#ifdef KINDLE_FOR_TOUCH
bool UIPlainTextEdit::OnTouchEvent(MoveEvent* moveEvent)
{
    int relativeX = moveEvent->GetX();
    int relativeY = moveEvent->GetY();
    UIIME* pIME = UIIMEManager::GetIME(IUIIME_CHINESE_LOWER, this);
    if (moveEvent->GetActionMasked() == MoveEvent::ACTION_DOWN && pIME)
    {
        if (relativeY < m_iHeight-m_buttonHeight)
        {
            m_textEditController.HitCursorPosition(relativeX, relativeY, m_curPage);
        }
        else
        {
            if (relativeX < m_iWidth/3 + 30)
            {
                if (m_curPage > 0)
                {
                    m_curPage--;
                    m_textEditController.MoveCursorToPageStart(m_curPage);
                }
            }
            else if (relativeX > m_iWidth/3*2 - 30)
            {
                if (m_curPage < m_textEditController.GetTotalPages() - 1)
                {
                    m_curPage++;
                    m_textEditController.MoveCursorToPageStart(m_curPage);
                }
            }
        }
        if (!pIME->IsShow())
        {
            pIME->SetTextBox(this);
            pIME->ShowIME();
            SetFocus(true);
        }
        UpdateWindow();
    }
    return true;
}
#endif

BOOL UIPlainTextEdit::OnKeyPress(int iKeyCode)
{
#ifdef KINDLE_FOR_TOUCH
    return TRUE;
#else
    UIIME* pIME = UIIMEManager::GetCurrentIME();
    if (pIME && pIME->IsShow() && !pIME->OnKeyPress(iKeyCode))
    {
        return FALSE;
    }

    if (IsActionKey(iKeyCode))
    {
        return OnActionPress(iKeyCode);
    }
    else
    {
        char key = KeyCode2Char(iKeyCode);
        wchar_t curChar = key;
        if (1 != InsertText(&curChar, 1))
        {
            return TRUE;
        }
    }

    return FALSE;
#endif
}
BOOL UIPlainTextEdit::OnActionPress( int iKeyCode)
{
    BOOL retValue = TRUE;
    switch(iKeyCode)
    {
    case KEY_DEL:
        retValue = OnDeletePress();
        break;
    case KEY_ENTER:
        retValue = OnEnterPress();
        break;
    case KEY_LEFT:
    case KEY_RIGHT:
    case KEY_UP:
    case KEY_DOWN:
        retValue = OnDirectionPress(iKeyCode);
        break;
    case KEY_FONT:
        retValue = OnFontPress();
        break;
    case KEY_SYM:
        retValue = OnSymPress();
        break;
    case KEY_LPAGEUP:
    case KEY_LPAGEDOWN:
    case KEY_RPAGEUP:
    case KEY_RPAGEDOWN:
        retValue = OnPageUpDownPress(iKeyCode);
    }
    if (!retValue)
    {
        UpdateWindow();
    }
    return retValue;
}

BOOL UIPlainTextEdit::OnDeletePress()
{
    int result = 0;
    if (m_fIsSelectAll && m_textEditController.GetCurDataLen()>0)
    {
        result = m_textEditController.ClearText();
    }
    else
    {
        result = m_textEditController.DeleteChar();
    }
    if (result > 0)
    {
        TextEditChangeEventArgs textEditChangeEventArgs(eTextEditContentChanged, (void*)(this));
        FireEvent(EventTextEditChange, textEditChangeEventArgs);
    }

    return FALSE;
}

BOOL UIPlainTextEdit::OnEnterPress()
{
    wchar_t curChar = L'\n';
    if (1 != InsertText(&curChar, 1))
        return TRUE;

    return FALSE;
}

BOOL UIPlainTextEdit::OnDirectionPress(int iKeyCode)
{
    bool retValue = false;
    switch (iKeyCode)
    {
    case KEY_LEFT:
        retValue = m_textEditController.MoveCursor(TextEditController::MOVE_LEFT);
        break;
    case KEY_RIGHT:
        retValue = m_textEditController.MoveCursor(TextEditController::MOVE_RIGHT);
        break;
    case KEY_UP:
        retValue = m_textEditController.MoveCursor(TextEditController::MOVE_UP);
        break;
    case KEY_DOWN:
        retValue = m_textEditController.MoveCursor(TextEditController::MOVE_DOWN);
        break;
    }
    return retValue ? FALSE : TRUE;
}

BOOL UIPlainTextEdit::OnPageUpDownPress(int iKeyCode)
{
    switch (iKeyCode)
    {
        case KEY_LPAGEUP:
        case KEY_RPAGEUP:
            if (m_curPage>0)
            {
                m_curPage--;
                m_textEditController.MoveCursorToPageStart(m_curPage);
            }
            return FALSE;
        case KEY_LPAGEDOWN:
        case KEY_RPAGEDOWN:
            if (m_curPage < m_textEditController.GetTotalPages() - 1)
            {
                m_curPage++;
                m_textEditController.MoveCursorToPageStart(m_curPage);
            }
            return FALSE;
    }
    return TRUE;
}

int UIPlainTextEdit::SetTip(const wchar_t* strText,int iStrLen)
{
    m_textEditController.SetTip(strText, iStrLen);
    return 1;
}

int UIPlainTextEdit::SetTipUTF8(LPCSTR strText,int iStrLen)
{
    m_textEditController.SetTipUTF8(strText, iStrLen);
    return 1;
}

int UIPlainTextEdit::SetText(const wchar_t *strText,int iStrLen)
{
    int result = m_textEditController.SetText(strText, iStrLen);
    UpdateWindow();
    if (result > 0)
    {
        TextEditChangeEventArgs textEditChangeEventArgs(eTextEditContentChanged, (void*)(this));
        FireEvent(EventTextEditChange, textEditChangeEventArgs);
    }
    return result;
}

int UIPlainTextEdit::SetTextUTF8(LPCSTR strText,int iStrLen)
{
    int result = m_textEditController.SetTextUTF8(strText, iStrLen);
    UpdateWindow();
    if (result > 0)
    {
        TextEditChangeEventArgs textEditChangeEventArgs(eTextEditContentChanged, (void*)(this));
        FireEvent(EventTextEditChange, textEditChangeEventArgs);
    }
    return result;
}

std::string UIPlainTextEdit::GetTextUTF8() const
{
    return m_textEditController.GetTextUTF8();
}

int UIPlainTextEdit::InsertText(const wchar_t *strText,int iStrLen)
{
    int result = m_textEditController.InsertText(strText, iStrLen);
    UpdateWindow();
    if (result > 0)
    {
        TextEditChangeEventArgs textEditChangeEventArgs(eTextEditContentChanged, (void*)(this));
        FireEvent(EventTextEditChange, textEditChangeEventArgs);
    }
    return result;
}

int UIPlainTextEdit::InsertTextUTF8(LPCSTR strText,int iStrLen)
{
    int result = m_textEditController.InsertTextUTF8(strText, iStrLen);
    UpdateWindow();
    if (result > 0)
    {
        TextEditChangeEventArgs textEditChangeEventArgs(eTextEditContentChanged, (void*)(this));
        FireEvent(EventTextEditChange, textEditChangeEventArgs);
    }
    return result;
}

int UIPlainTextEdit::ClearText()
{
    m_textEditController.ClearText();
    TextEditChangeEventArgs textEditChangeEventArgs(eTextEditContentChanged, (void*)(this));
    FireEvent(EventTextEditChange, textEditChangeEventArgs);
    UpdateWindow();
    return 1;
}

void UIPlainTextEdit::SetMaxDataLen(int iMaxLen)
{
    m_textEditController.SetMaxDataLen(iMaxLen);
}

int UIPlainTextEdit::DeleteChar()
{
    int result = m_textEditController.DeleteChar();
    UpdateWindow();
    if (result > 0)
    {
        TextEditChangeEventArgs textEditChangeEventArgs(eTextEditContentChanged, (void*)(this));
        FireEvent(EventTextEditChange, textEditChangeEventArgs);
    }
    return result;
}

void UIPlainTextEdit::SetFontSize(int fontSize)
{
    m_textEditController.SetFontSize(fontSize);
}

int UIPlainTextEdit::GetCurDataLen() const
{
    return m_textEditController.GetCurDataLen();
}

HRESULT UIPlainTextEdit::UpdateFocus()
{
    return UpdateWindow();
}

