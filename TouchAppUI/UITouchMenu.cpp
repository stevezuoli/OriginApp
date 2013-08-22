#include "TouchAppUI/UITouchMenu.h"
#include "GUI/UISizer.h"
#include "GUI/UISeperator.h"
#include "GUI/UITouchComplexButton.h"
#include "Common/WindowsMetrics.h"
#include "GUI/CTpGraphics.h"
#include "Framework/CDisplay.h"

using namespace WindowsMetrics;

static const int SHADDOW_TOP = 3;
static const int SHADDOW_LEFT = 3;
static const int SHADDOW_RIGHT_EXCEEDING = 2;
static const int SHADDOW_BOTTOM_EXCEEDING = 3;
static const int BORDER_WIDTH = 2;
static const int BUTTON_LEFT_BLANK = 5;

UITouchMenu::UITouchMenu(UIContainer* parent)
    : UIDialog(parent)
    , m_checkedIndex(-1)
    , m_commandId(ID_INVALID)
{
    InitUI();
    HookTouch();
}

UITouchMenu::~UITouchMenu()
{
    if (m_windowSizer)
    {
        // delete windowSizer before delete obj in m_btns
        // otherwise windowSizer will use wild pointer to obj in m_btns
        delete m_windowSizer;
        m_windowSizer = NULL;
    }
    ClearObjectInVector(m_btns);
    ClearObjectInVector(m_seps);
}

void UITouchMenu::InitUI()
{
    const int seperatorWidth = GetWindowMetrics(UITouchMenuItemWidthIndex);
    m_checkedImage = ImageManager::GetImage(IMAGE_TOUCH_ICON_MENU_CHECK);
    UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
    UISizer* buttonSizer = new UIBoxSizer(dkVERTICAL);
    int leftPadding = GetWindowMetrics(UITouchMenuItemLeftPaddingIndex);
    buttonSizer->SetMinWidth(seperatorWidth + (leftPadding - BUTTON_LEFT_BLANK) * 2);

    m_buttonSizer = buttonSizer;
    mainSizer->Add(buttonSizer, UISizerFlags(1).Expand()
            .Border(dkLEFT, BORDER_WIDTH + BUTTON_LEFT_BLANK)
            .Border(dkTOP, BORDER_WIDTH)
            .Border(dkRIGHT, BORDER_WIDTH + BUTTON_LEFT_BLANK + SHADDOW_RIGHT_EXCEEDING)
            .Border(dkBOTTOM, BORDER_WIDTH + SHADDOW_BOTTOM_EXCEEDING));
    SetSizer(mainSizer);
}

bool UITouchMenu::AppendButton(DWORD id, const char* text, int fontSize)
{
    int btnOutMargin = 4;
    if (!m_btns.empty())
    {
        const int seperatorWidth = GetWindowMetrics(UITouchMenuItemWidthIndex);
        UISeperator* sep = new UISeperator();
        m_seps.push_back(sep);
        AppendChild(sep);
        sep->SetMinWidth(seperatorWidth);
        sep->SetMinHeight(1);
        sep->SetDirection(UISeperator::SD_HORZ);
        m_buttonSizer->Add(sep, UISizerFlags().Align(dkALIGN_CENTER_HORIZONTAL));
    }
    UITouchComplexButton* btn = new UITouchComplexButton(this, id);
    AppendChild(btn);
    m_btns.push_back(btn);
    m_buttonSizer->Add(btn, UISizerFlags().Border(dkUP | dkDOWN, BUTTON_LEFT_BLANK).Expand());

    int leftPadding = GetWindowMetrics(UITouchMenuItemLeftPaddingIndex);
    btn->SetText(text);
    btn->SetFontSize(fontSize);
    btn->ShowBorder(false);
    btn->SetLeftMargin(leftPadding - BUTTON_LEFT_BLANK);
    btn->SetMinHeight(
            GetWindowMetrics(UITouchMenuItemHeightIndex) - btnOutMargin * 2);
    btn->SetAlign(ALIGN_LEFT);
    btn->SetUpdateOnUnPress(false);
    return true;
}

bool UITouchMenu::OnHookTouch(MoveEvent* moveEvent)
{
    if (moveEvent->GetActionMasked() != MoveEvent::ACTION_UP)
    {
        return false;
    }
    if (IsVisible() && !PointInWindowAbsolute(moveEvent->GetX(), moveEvent->GetY()))
    {
        EndDialog(IDCANCEL);
        return true;
    }
    return false;
}

size_t UITouchMenu::GetButtonCount() const
{
    return m_btns.size();
}

UITouchComplexButton* UITouchMenu::GetButton(int index)
{
    if (index < 0 || index > (int)GetButtonCount())
    {
        return NULL;
    }
    return m_btns[index];
}

void UITouchMenu::SetCheckedButton(int checked)
{
    if (checked == m_checkedIndex || checked < -1 || checked >= (int)GetButtonCount())
    {
        return;
    }
    SPtr<ITpImage> imgEmpty;
    DK_AUTO(oldCheckedBtn, GetButton(m_checkedIndex));
    if (NULL != oldCheckedBtn)
    {
        oldCheckedBtn->SetIcons(imgEmpty, imgEmpty);
    }
    DK_AUTO(newCheckedBtn, GetButton(checked));
    if (NULL != newCheckedBtn)
    {
        newCheckedBtn->SetIcons(m_checkedImage, m_checkedImage, UIButton::ICON_RIGHT);
    }
    m_checkedIndex = checked;
}

dkSize UITouchMenu::GetMenuSize()
{
    DK_AUTO(mainSizer, GetSizer());
    if (NULL == mainSizer)
    {
        return dkSize(-1, -1);
    }
    return mainSizer->GetMinSize();
}

int UITouchMenu::CalculateTop(const UIButton* button)
{
    
    int vertSpacingFromButtonAttached = GetWindowMetrics(UIButtonDlgVertSpacingFromAttachedButtonIndex);
    int top = button->GetY() + button->GetHeight() + vertSpacingFromButtonAttached;
    int height = GetMinHeight();
    int parentHeight = GetParent()->GetHeight();
    if (top + height > parentHeight)
    {
        top = button->GetY() - height - vertSpacingFromButtonAttached;
    }
    return top;
}

bool UITouchMenu::OnChildClick(UIWindow* child)
{
    return true;
}

HRESULT UITouchMenu::DrawBackGround(DK_IMAGE drawingImg)
{
    CTpGraphics grf(drawingImg);
    grf.FillRect(BORDER_WIDTH, BORDER_WIDTH,
            m_iWidth - SHADDOW_RIGHT_EXCEEDING - BORDER_WIDTH,
            m_iHeight - SHADDOW_BOTTOM_EXCEEDING - BORDER_WIDTH,
            ColorManager::knWhite);
    grf.DrawRectBorder(0, 0,
            m_iWidth - SHADDOW_RIGHT_EXCEEDING,
            m_iHeight - SHADDOW_BOTTOM_EXCEEDING,
            BORDER_WIDTH,
            ColorManager::knBlack);
    dkColor shaddowColor = ColorManager::knBlack;
    grf.FillRect(
            m_iWidth - SHADDOW_RIGHT_EXCEEDING, SHADDOW_TOP, 
            m_iWidth, m_iHeight, shaddowColor);
    grf.FillRect(SHADDOW_LEFT, m_iHeight - SHADDOW_BOTTOM_EXCEEDING,
            m_iWidth - SHADDOW_RIGHT_EXCEEDING, m_iHeight, 
            shaddowColor);
    return S_OK;
}

void UITouchMenu::OnCommand(DWORD cmdId, UIWindow* sender, DWORD param)
{
    m_commandId = cmdId;
    EndDialog(IDOK);
    CDisplay::GetDisplay()->SetFullRepaint(true);
}

int UITouchMenu::GetCommandId() const
{
    return m_commandId;
}
