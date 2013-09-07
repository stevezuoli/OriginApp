////////////////////////////////////////////////////////////////////////
// UIFrontLightManager.cpp
// Contact:
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////
#include "Common/WindowsMetrics.h"
#include "GUI/CTpGraphics.h"
#include "GUI/FontManager.h"
#include "Framework/CDisplay.h"
#include "I18n/StringManager.h"
#include "CommonUI/UIUtility.h"
#include "TouchAppUI/UIFrontLightManager.h"
#include "interface.h"
#include "drivers/DeviceInfo.h"
#include <tr1/functional>
#include "GUI/UISizer.h"
#include "drivers/FrontLight.h"
#include "Utility/Misc.h"
#include "GUI/UIComplexButton.h"

#include <iostream>
#include <fstream>

using namespace std;
using namespace WindowsMetrics;

SINGLETON_CPP(UIFrontLightManager)
static const int topPadding = 60;
const int barWidth = 14;

#define FRONTLIGHT_HEIGHT	100	
#define FRONTLIGHT_LEFT_MARGIN	18
#define FRONTLIGHT_LEFT_PADDING	16
#define BUTTON_WIDTH 52
#define BUTTON_HEIGHT 52
#define INTERVAL_VERT_SPACING 10  

UIFrontLightManager::UIFrontLightManager()
    : UIContainer(NULL, IDSTATIC)
{
    HookTouch();
    SetClickable(true);
	int fontsize = 18;
    m_blackBarImage = ImageManager::GetImage(IMAGE_KPW_ICON_FRONTLIGHT_BAR_BLACK);
    m_whiteBarImage = ImageManager::GetImage(IMAGE_KPW_ICON_FRONTLIGHT_BAR_WHITE);

    const int totalHeight = 172;
    const int leftPadding = 16;
    const int internalHorzSpacing = 10;

    UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
    mainSizer->AddSpacer(topPadding);
    UIBoxSizer* barSizer = new UIBoxSizer(dkHORIZONTAL);

    m_btnSub.Initialize(ID_BTN_TOUCH_FRONTLIGHT_SUB, NULL, fontsize);
    m_btnSub.SetIcons(ImageManager::GetImage(IMAGE_KPW_ICON_FRONTLIGHT_SUB), ImageManager::GetImage(IMAGE_KPW_ICON_FRONTLIGHT_SUB), UIComplexButton::ICON_TOP);
    m_btnSub.SetCornerStyle(true, true, true, true);
    m_btnSub.ShowBorder(false);
	AppendChild(&m_btnSub);
    barSizer->Add(&m_btnSub, UISizerFlags(1).Expand());
    barSizer->AddSpacer(internalHorzSpacing);

    for (int i = 0; i < TOTAL_LEVELS; ++i)
    {
        m_bars[i].SetAutoSize(true);
        AppendChild(&m_bars[i]);
        barSizer->Add(&m_bars[i]);
        barSizer->AddSpacer(internalHorzSpacing);
    }
    UpdateLighBar();
	
    m_btnAdd.Initialize(ID_BTN_TOUCH_FRONTLIGHT_ADD, NULL, fontsize);
    m_btnAdd.SetIcons(ImageManager::GetImage(IMAGE_KPW_ICON_FRONTLIGHT_ADD), ImageManager::GetImage(IMAGE_KPW_ICON_FRONTLIGHT_ADD), UIComplexButton::ICON_TOP);
    m_btnAdd.SetCornerStyle(true, true, true, true);
    m_btnAdd.ShowBorder(false);
    AppendChild(&m_btnAdd);
    barSizer->Add(&m_btnAdd, UISizerFlags(1).Expand());
    mainSizer->Add(barSizer, UISizerFlags().Expand());

    SetSizer(mainSizer);
	MoveWindow(0, 200, CDisplay::GetDisplay()->GetScreenWidth(), totalHeight);
    SetVisible(false);
    Layout();
    AppendChild(&m_levelText);
    m_levelText.SetFontSize(18);
    UpdateLevelText();
}


UIFrontLightManager::~UIFrontLightManager()
{
}

void UIFrontLightManager::MoveWindow(int _iLeft, int _iTop, int _iWidth, int _iHeight)
{
    UIWindow::MoveWindow(_iLeft, _iTop, _iWidth, _iHeight);
	return;
}


HRESULT UIFrontLightManager::Draw(DK_IMAGE drawingImg)
{   
    DebugPrintf(DLC_FRONTLIGHT, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (!m_bIsVisible)
    {
        DebugPrintf(DLC_FRONTLIGHT," UIFrontLightManager::Draw(DK_IMAGE drawingImg) !m_bIsVisible");
        return S_FALSE;
    }

    if (drawingImg.pbData == NULL)
    {
        return E_FAIL;
    }
     
    HRESULT hr(S_OK);
    DK_IMAGE imgSelf;
    DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
    RTN_HR_IF_FAILED(CropImage(
        drawingImg,
        rcSelf,
        &imgSelf
        ));

	DrawBackgroundEx(imgSelf, ColorManager::knWhite, false, 1, true, true, true, true);
	int bottomY = GetAbsoluteY() + m_iHeight;
	CTpGraphics grf(drawingImg);
    grf.FillRect(0, bottomY - 2, drawingImg.iWidth, bottomY, ColorManager::knBlack);
#if 1
    int iSize = GetChildrenCount();
    for (int i = 0; i < iSize; i++)
    {
        UIWindow* pWin = GetChildByIndex(i);
        if (pWin)
        {
            RTN_HR_IF_FAILED(pWin->Draw(imgSelf));
        }
    }
    
    return hr;
#endif
}

void UIFrontLightManager::OnCommand(DWORD _dwCmdId, UIWindow* _pSender, DWORD _dwParam)
{
    switch(_dwCmdId)
    {
    case ID_BTN_TOUCH_FRONTLIGHT_ADD:
		{
			OnBtnAddClicked();
		}
        break;
    case ID_BTN_TOUCH_FRONTLIGHT_SUB:
        {
			OnBtnSubClicked();
        }
        break;
    case ID_UI_TOUCH_FRONTLIGHT_BAR:
        {
			// not ready
        }
        break;
    default :
        break;
    }
}

void UIFrontLightManager::OnBtnAddClicked(void)
{
	int level = FrontLight::GetLevel();
    if (level < TOTAL_LEVELS)
    {
        level++;
        FrontLight::SetLevel(level);
        FrontLight::WriteSystemLevel();
        UpdateLighBar();
        UpdateLevelText();
        UpdateWindow();
    }
}

void UIFrontLightManager::OnBtnSubClicked(void)
{	
	int level = FrontLight::GetLevel();
    if (level > 0)
    {
        level--;
        FrontLight::SetLevel(level);

        FrontLight::WriteSystemLevel();
        UpdateLighBar();
        UpdateLevelText();
        UpdateWindow();
    }
}

bool UIFrontLightManager::SetVisible(bool visible)
{
    if (m_bIsVisible == visible)
    {
        return true;
    }
    
    UIWindow::SetVisible(visible);
    
    if (m_bIsVisible)
    {
        UpdateWindow();
    }
    else
    {
        UIContainer* parent = m_pParent;
        if (parent)
        {
            parent->RemoveChild(this, false);
            parent->UpdateWindow();
        }
        FrontLight::WriteSystemLevel();
    }
    
    return true;
}

void UIFrontLightManager::SetEnableHookTouch(bool isEnable)
{
	isEnable ? HookTouch() : UnhookTouch();
}

bool UIFrontLightManager::OnHookTouch(MoveEvent* moveEvent)
{
    if (moveEvent->GetActionMasked() != MoveEvent::ACTION_DOWN)
    {
        return false;
    }

    if (IsVisible() && !PointInWindowAbsolute(moveEvent->GetX(), moveEvent->GetY()))
    {
        DebugPrintf(DLC_DIAGNOSTIC, "UIFrontLightManager::OnHookTouch(%d, %d), self: (%d, %d)~(%d, %d)",
                moveEvent->GetX(), moveEvent->GetY(), 
                GetAbsoluteX(), GetAbsoluteY(),
                GetWidth(), GetHeight());
        SetVisible(false);
        DebugPrintf(DLC_DIAGNOSTIC, "UIFrontLightManager::OnHookTouch() return true");
        return true;
    }
    return false;
}

void UIFrontLightManager::UpdateLighBar()
{
	int level = FrontLight::GetLevel();
    for (int i = 0; i < TOTAL_LEVELS; ++i)
    {
        if (i < level)
        {
            m_bars[i].SetImage(m_blackBarImage);
        }
        else
        {
            m_bars[i].SetImage(m_whiteBarImage);
        }
    }
}

void UIFrontLightManager::UpdateLevelByX(int x)
{
    if (x >= m_bars[0].GetX() && x <= m_bars[TOTAL_LEVELS-1].GetX() + m_bars[TOTAL_LEVELS-1].GetWidth())
    {
        int prevBar = -1;
        for (int i = 0; i < TOTAL_LEVELS; ++i)
        {
            if (m_bars[i].GetX() >= x)
            {
                break;
            }
            prevBar = i;
        }
        int prevLevel = FrontLight::GetLevel();
        int curLevel = prevBar + 1;
        DebugPrintf(DLC_DIAGNOSTIC, "UIFrontLightManager::OnTouch(), %d-->%d", prevLevel, curLevel);
        if (curLevel != prevLevel)
        {
            FrontLight::SetLevel(curLevel);
            UpdateLighBar();
            UpdateLevelText();
            UpdateWindow();
        }
    }
}

bool UIFrontLightManager::OnTouchEvent(MoveEvent* moveEvent)
{
    
    UIContainer::OnTouchEvent(moveEvent);
    if (MoveEvent::ACTION_DOWN == moveEvent->GetActionMasked())
    {
        m_downX = moveEvent->GetX();
        m_downY = moveEvent->GetY();
    }
    if (MoveEvent::ACTION_MOVE == moveEvent->GetActionMasked())
    {
        int x = moveEvent->GetX(), y = moveEvent->GetY();
        if (!IsPointNeighbour(m_downX, m_downY, x, y, 20))
        {
            UpdateLevelByX(moveEvent->GetX());
        }
    }
    return true;
}

void UIFrontLightManager::UpdateLevelText()
{
    int level = FrontLight::GetLevel();
    (level >= 0) || (level = 0);
    (level <= TOTAL_LEVELS) || (level = TOTAL_LEVELS);
    char buf[20] = "";
    if (0 == level)
    {
        m_levelText.SetVisible(false);
        return;
    }
    else
    {
        snprintf(buf, DK_DIM(buf), "%d", level);
        m_levelText.SetText(buf);
        m_levelText.SetVisible(true);
    }
    int x = m_bars[level-1].GetX();
    dkSize textSize = m_levelText.GetMinSize();
    x += barWidth / 2 - textSize.x / 2;
    int y = (topPadding - textSize.y) / 2;
    m_levelText.MoveWindow(x, y, textSize.x, textSize.y);
}

void UIFrontLightManager::PerformClick(MoveEvent* moveEvent)
{
    UpdateLevelByX(moveEvent->GetX());
}
