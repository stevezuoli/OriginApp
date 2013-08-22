#include "GUI/UICheckButton.h"
#include "Utility/ImageManager.h"

UICheckButton::UICheckButton(UIContainer* parent, DWORD id)
    : UITouchComplexButton(parent, id)
    , m_checked(false)
{
    ShowBorder(false);
    m_checkIcon = ImageManager::GetImage(IMAGE_ICON_COVER_SELECTED);
    m_uncheckIcon = ImageManager::GetImage(IMAGE_ICON_COVER_UNSELECTED);
    UpdateIcon();
}


UICheckButton::UICheckButton()
    : UITouchComplexButton()
    , m_checked(false)
{
    ShowBorder(false);
    m_checkIcon = ImageManager::GetImage(IMAGE_ICON_COVER_SELECTED);
    m_uncheckIcon = ImageManager::GetImage(IMAGE_ICON_COVER_UNSELECTED);
    UpdateIcon();
}

UICheckButton::~UICheckButton()
{
}


void UICheckButton::UpdateIcon()
{
    if (m_checked)
    {
        SetIcons(m_checkIcon, m_checkIcon, UIButton::ICON_LEFT);
    }
    else
    {
        SetIcons(m_uncheckIcon, m_uncheckIcon, UIButton::ICON_LEFT);
    }
}

void UICheckButton::SetChecked(bool checked)
{
    if (checked != m_checked)
    {
        m_checked = checked;
        UpdateIcon();
    }
}

bool UICheckButton::IsChecked() const
{
    return m_checked;
}
