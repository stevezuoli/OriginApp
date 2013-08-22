#include "GUI/UIRadioButtonGroup.h"
#include "GUI/UISizer.h"
UIRadioButtonGroup::UIRadioButtonGroup()
{
}

UIRadioButtonGroup::~UIRadioButtonGroup()
{
    for (size_t i = 0; i < m_dynamicCreatedWindows.size(); ++i)
    {
        SafeDeletePointer(m_dynamicCreatedWindows[i]);
    }
    m_dynamicCreatedWindows.clear();
}

void UIRadioButtonGroup::AddButton(UITouchComplexButton* button)
{
    if (m_buttons.empty())
    {
        button->SetCornerStyle(false, false, false, false);
    }
    else
    {
        UITouchComplexButton* prevButton = m_buttons.back();
        bool leftTopSquare, leftBottomSquare;
        prevButton->GetCornerStyle(&leftTopSquare, NULL, NULL, &leftBottomSquare);
        prevButton->SetCornerStyle(leftTopSquare, true, true, leftBottomSquare);
        button->SetCornerStyle(true, false, false, true);
    }
    AppendChild(button);
    m_buttons.push_back(button);
}

void UIRadioButtonGroup::AddButton(int cmdId, const char* pText, int fontsize, bool isSelected)
{
    UITouchComplexButton* button = new UITouchComplexButton();
    if (button)
    {
        button->Initialize(cmdId, pText, fontsize);
        if (isSelected)
        {
            button->SetFocus(true);
        }
        AddButton(button);
        m_dynamicCreatedWindows.push_back(button);
    }
}

void UIRadioButtonGroup::MoveWindow(int left, int top, int width, int height)
{
    UIWindow::MoveWindow(left, top, width, height);
    if (!m_windowSizer)
    {
        UIBoxSizer* mainSizer = new UIBoxSizer(dkHORIZONTAL); 
        for (size_t i = 0; i < m_buttons.size(); ++i)
        {
            mainSizer->Add(m_buttons[i], 1);
        }
        SetSizer(mainSizer);
    }
    Layout();
}

dkSize UIRadioButtonGroup::GetMinSize() const
{
    int minWidth = 0;
    int minHeight = 0;
    for (DK_AUTO(cur, m_buttons.begin()); cur != m_buttons.end(); ++cur)
    {
        dkSize size = (*cur)->GetMinSize();
        minWidth += size.GetX();
        minHeight = dk_max(minHeight, size.GetY());
    }
    return dkSize(minWidth, minHeight);
}

void UIRadioButtonGroup::SelectChild(size_t index)
{
	if(index >= 0 && index < GetChildrenCount())
	{
		ClearFocus();
		UIWindow* focusControl = GetChildByIndex(index);
		if(focusControl)
		{
			focusControl->SetFocus(true);
		}
	}
	UpdateWindow();
}

void UIRadioButtonGroup::SetFocus(BOOL bIsFocus)
{
    // 如果SetFocus为false，则选中的按钮状态将被更新为false
    return;
}
