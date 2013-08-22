#include "CommonUI/UICommentEditDialog.h"
#include "CommonUI/UIIMEManager.h"
#include "Framework/CDisplay.h"
#include "GUI/CTpGraphics.h"
#include "Common/WindowsMetrics.h"
#include "GUI/UISizer.h"
#include "I18n/StringManager.h"
#include <tr1/functional>
#include "drivers/DeviceInfo.h"

using namespace WindowsMetrics;

UICommentEditDialog::UICommentEditDialog(UIContainer* pParent, int commentIndex, std::string comment)
    : UIDialog(pParent)
    , m_commentIndex(commentIndex)
    , m_resultCmdId(IDCANCEL)
{
    EventListener::SubscribeEvent(
        UIAbstractTextEdit::EventTextEditChange,
        m_boxComment, 
        std::tr1::bind(
        std::tr1::mem_fn(&UICommentEditDialog::OnInputChange),
        this,
        std::tr1::placeholders::_1));
    EventListener::SubscribeEvent(
        UIButtonGroup::ButtonClickedChangeEvent,
        m_btnGroup, 
        std::tr1::bind(
        std::tr1::mem_fn(&UICommentEditDialog::OnButtonClicked),
        this,
        std::tr1::placeholders::_1));
    m_strPreComment = comment;
#ifdef KINDLE_FOR_TOUCH
    SetPropogateTouch(false);
    HookTouch();
#endif
    InitUI();
}

UICommentEditDialog::~UICommentEditDialog()
{

}

void UICommentEditDialog::InitUI()
{
    const int elemSpacing = GetWindowMetrics(UIElementLargeSpacingIndex);
    UISizer* pMainSizer = new UIBoxSizer(dkVERTICAL);
    if (pMainSizer)
    {
        const int fontSize20 = GetWindowFontSize(FontSize20Index);
        const int fontSize24 = GetWindowFontSize(FontSize24Index);
        const int horizontalMargin = GetWindowMetrics(UICommentWndHorizonMarginIndex);
        const int btnHeight = GetWindowMetrics(UICommentWndSaveCommentHeightIndex);
        const bool newComment = m_commentIndex < 0;
        m_txtTitle.SetFontSize(fontSize24);
        if (newComment)
        {
            m_txtTitle.SetText(StringManager::GetPCSTRById(TOUCH_ADD_COMMENT));
        }
        else
        {
            char strText[128] = {0};
            snprintf(strText, DK_DIM(strText), "%s %d", StringManager::GetPCSTRById(TOUCH_COMMENT), m_commentIndex);
            m_txtTitle.SetText(strText);
        }

        m_boxComment.SetFontSize(fontSize20);
        if (!m_strPreComment.empty())
        {
            m_boxComment.SetTextUTF8(m_strPreComment.c_str(), m_strPreComment.length());
            //TODO(JUGH):目前UIPlainTextEdit的SelectAll支持不完善
            //m_boxComment.SelectAll();
        }
        m_boxComment.SetMinHeight(newComment ? GetWindowMetrics(UICommentWndEditHeight1Index) : GetWindowMetrics(UICommentWndEditHeight2Index));
        m_btnViewComment.SetVisible(!newComment);
        m_btnViewComment.Initialize(GOTO_COMMENTORDIGEST, StringManager::GetPCSTRById(TOUCH_GOTO_COMMENTORDIGEST),
#ifdef KINDLE_FOR_TOUCH
            fontSize20);
        m_btnViewComment.ShowBorder(false);
        m_btnViewComment.SetMinHeight(GetWindowMetrics(UICommentWndViewCommentHeightIndex));
#else
            'V',
            fontSize20,
            ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
#endif

        m_btnGroup.SetMinHeight(btnHeight);

        m_btnDeleteComment.Initialize(DELETE_COMMENT, StringManager::GetPCSTRById(TOUCH_DELETE),
#ifdef KINDLE_FOR_TOUCH
            fontSize20);
#else
            'A',
            fontSize20,
            ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
#endif
        m_btnDeleteComment.SetEnable(!newComment);
        m_btnExitOrCancel.Initialize(IDCANCEL, StringManager::GetPCSTRById(newComment ? TOUCH_CANCEL : TOUCH_EXIT), 
#ifdef KINDLE_FOR_TOUCH
            fontSize20);
#else
            'B',
            fontSize20,
            ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
#endif
        m_btnSave.Initialize(SAVE_COMMENT, StringManager::GetPCSTRById(TOUCH_SAVE), 
#ifdef KINDLE_FOR_TOUCH
            fontSize20);
#else
            'C',
            fontSize20,
            ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
#endif
        m_btnSave.SetEnable(false);

        AppendChild(&m_txtTitle);
        AppendChild(&m_boxComment);
        AppendChild(&m_btnViewComment);
        AppendChild(&m_btnGroup);

        m_btnGroup.AddButton(&m_btnDeleteComment);
        m_btnGroup.AddButton(&m_btnExitOrCancel);
        m_btnGroup.AddButton(&m_btnSave);
        m_btnGroup.SetTopLinePixel(newComment ? 0 : 1);
        m_btnGroup.SetBottomLinePixel(0);
        m_btnGroup.SetSplitLineHeight(btnHeight - (GetWindowMetrics(UICommentWndBtnTopPaddingIndex) << 1));

        //note:UITextSingleLine内核返回的值比文字本身的矩形的要大一点，导致UI上不居中
        pMainSizer->Add(&m_txtTitle, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizontalMargin).Border(dkTOP, elemSpacing + 1).Border(dkBOTTOM, elemSpacing - 1));
        pMainSizer->Add(&m_boxComment, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizontalMargin));
#ifdef KINDLE_FOR_TOUCH
        pMainSizer->Add(&m_btnViewComment, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizontalMargin));
#else
        pMainSizer->Add(&m_btnViewComment, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizontalMargin).Border(dkTOP | dkBOTTOM, elemSpacing));
#endif
        pMainSizer->Add(&m_btnGroup, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizontalMargin).Border(dkBOTTOM, 3));
        SetSizer(pMainSizer);
        dkSize minSize = GetMinSize();

        int width = DeviceInfo::GetDisplayScreenWidth() - (GetWindowMetrics(UIHorizonMarginIndex) << 1);
        MakeCenterAboveIME(width, minSize.y);
        Layout();
        SetChildWindowFocus(GetChildIndex(&m_boxComment), false);
        UIIME* pIME = UIIMEManager::GetIME(IUIIME_CHINESE_LOWER, &m_boxComment);
        if (pIME)
        {
            pIME->SetShowDelay(true);
        }
    }
    else
    {
        SafeDeletePointer(pMainSizer);
    }
}

HRESULT UICommentEditDialog::DrawBackGround(DK_IMAGE drawingImg)
{
    HRESULT hr(S_OK);
    CTpGraphics grf(drawingImg);
    RTN_HR_IF_FAILED(grf.DrawBorder(0, 0, 0, 0));
    return hr;
}

#ifdef KINDLE_FOR_TOUCH
bool UICommentEditDialog::OnHookTouch(MoveEvent* moveEvent)
{
    if (moveEvent && IsVisible())
    {
        if (moveEvent->GetActionMasked() != MoveEvent::ACTION_DOWN)
        {
            return false;
        }
        if (!PointInWindowAbsolute(moveEvent->GetX(), moveEvent->GetY()))
        {
            UIIME* ime = UIIMEManager::GetCurrentIME();
            if ((NULL == ime) || !ime->PointInWindowAbsolute(moveEvent->GetX(), moveEvent->GetY()))
            {
                EndDialog(IDCANCEL);
                return true;
            }
        }
    }
    return false;
}
#endif

bool UICommentEditDialog::OnInputChange(const EventArgs& args)
{
    string strCurComment(m_boxComment.GetTextUTF8());
    bool preEnable = m_btnSave.IsEnable();
    bool curEnable = !(strCurComment.empty() || 0 == strCurComment.compare(m_strPreComment));
    if (preEnable != curEnable)
    {
        m_btnSave.SetEnable(curEnable);
        m_btnSave.RedrawWindow();
    }
    return true;
}

bool UICommentEditDialog::OnButtonClicked(const EventArgs& args)
{
    const ButtonClickedEventArgs& buttonClickEventArgs = (const ButtonClickedEventArgs&)args;
    OnCommand(buttonClickEventArgs.m_clickedButtonId, buttonClickEventArgs.m_pSender, buttonClickEventArgs.m_param);
    return true;
}

void UICommentEditDialog::OnCommand(DWORD _dwCmdId, UIWindow* _pSender, DWORD _dwParam)
{
    m_resultCmdId = _dwCmdId;
    EndDialog((IDCANCEL == m_resultCmdId) ? IDCANCEL : IDOK);
}