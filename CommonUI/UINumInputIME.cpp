#include "CommonUI/UINumInputIME.h"
#include "interface.h"
#include "Utility.h"
#include "GUI/CTpGraphics.h"
#include "I18n/StringManager.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "CommonUI/UIUtility.h"
#include "Framework/CDisplay.h"

static const char* numValues = "0123456789.";

using namespace WindowsMetrics;
SINGLETON_CPP(UINumInputIME)

UINumInputIME::UINumInputIME()
	: m_btnWidth(GetWindowMetrics(UIUINumInputIMEBtnWidthIndex))
	, m_btnHeight(GetWindowMetrics(UIUINumInputIMEBtnHeightIndex))
{
    Init();
}

UINumInputIME::~UINumInputIME()
{
}

void UINumInputIME::Init()
{
	SetClickable(true);
	HookTouch();
    DebugPrintf(DLC_UINUMINPUT, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    const int fontSize26 = GetWindowFontSize(FontSize26Index);
    SPtr<ITpImage> imgDel = ImageManager::GetImage(IMAGE_ICON_NUMBERINPUTIME_BTN_DELETE);
	SPtr<ITpImage> imgDelP = ImageManager::GetImage(IMAGE_ICON_NUMBERINPUTIME_BTN_DELETE_PRESS);

	SPtr<ITpImage> imgBtn = ImageManager::GetImage(IMAGE_ICON_NUMBERINPUTIME_BTN);
	SPtr<ITpImage> imgBtnP = ImageManager::GetImage(IMAGE_ICON_NUMBERINPUTIME_BTN_PRESS);
	
    m_KeyboardButton[0].Initialize(ID_BTN_TOUCH_KEYBAORD_NUMBER_1, "1", fontSize26, imgBtn, imgBtnP);
    m_KeyboardButton[1].Initialize(ID_BTN_TOUCH_KEYBAORD_NUMBER_2, "2", fontSize26, imgBtn, imgBtnP);
    m_KeyboardButton[2].Initialize(ID_BTN_TOUCH_KEYBAORD_NUMBER_3, "3", fontSize26, imgBtn, imgBtnP);
    m_KeyboardButton[3].Initialize(ID_BTN_TOUCH_KEYBAORD_NUMBER_4, "4", fontSize26, imgBtn, imgBtnP);
    m_KeyboardButton[4].Initialize(ID_BTN_TOUCH_KEYBAORD_NUMBER_5, "5", fontSize26, imgBtn, imgBtnP);
    m_KeyboardButton[5].Initialize(ID_BTN_TOUCH_KEYBAORD_NUMBER_6, "6", fontSize26, imgBtn, imgBtnP);
    m_KeyboardButton[6].Initialize(ID_BTN_TOUCH_KEYBAORD_NUMBER_7, "7", fontSize26, imgBtn, imgBtnP);
    m_KeyboardButton[7].Initialize(ID_BTN_TOUCH_KEYBAORD_NUMBER_8, "8", fontSize26, imgBtn, imgBtnP);
    m_KeyboardButton[8].Initialize(ID_BTN_TOUCH_KEYBAORD_NUMBER_9, "9", fontSize26, imgBtn, imgBtnP);
	m_KeyboardButton[9].Initialize(ID_BTN_TOUCH_KEYBAORD_POINT, ".", fontSize26, imgBtn, imgBtnP);
	m_KeyboardButton[10].Initialize(ID_BTN_TOUCH_KEYBAORD_NUMBER_0, "0", fontSize26, imgBtn, imgBtnP);
    m_KeyboardButton[11].Initialize(ID_KEYBOARD_BACKSPACE, "",  fontSize26);
	m_KeyboardButton[11].SetIcons(imgDel, imgDelP, UIButton::ICON_CENTER);
	m_btnEnter.Initialize(ID_KEYBOARD_ENTER, StringManager::GetStringById(ACTION_OK), GetWindowFontSize(FontSize22Index));
    for (int i = 0; i < 12; i++)
    {
        m_KeyboardButton[i].SetUpdateWindowFlag(PAINT_FLAG_DU);
    	m_KeyboardButton[i].ShowBorder(false);
        AppendChild(&m_KeyboardButton[i]);
    }
	m_btnEnter.SetCornerStyle(true, true, false, false);
    AppendChild(&m_btnEnter);
	InitUI();
    DebugPrintf(DLC_UINUMINPUT, "%s, %d, %s, %s END", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

void UINumInputIME::InitUI()
{
	if (!m_windowSizer)
    {
        UISizer* pMainSizer = new UIBoxSizer(dkVERTICAL);
		UISizer* itemSizer = new UIGridSizer(3, GetWindowMetrics(UIUINumInputIMEElementMarginIndex), GetWindowMetrics(UIUINumInputIMEElementMarginIndex));
		for ( int i = 0; i < 12; ++i)
		{
            m_KeyboardButton[i].SetMinSize(dkSize(m_btnWidth, m_btnHeight));
			itemSizer->Add(m_KeyboardButton + i);
		}
        if (pMainSizer)
        {
            pMainSizer->Add(itemSizer, UISizerFlags().Border(dkALL, GetWindowMetrics(UIUINumInputIMEBorderIndex)));
			m_btnEnter.SetMinHeight(m_btnHeight);
			pMainSizer->Add(&m_btnEnter, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT | dkBOTTOM, 1));
            SetSizer(pMainSizer);
        }
        else
			
        {
            SafeDeletePointer(pMainSizer);
            SafeDeletePointer(itemSizer);
        }
    }
}

void UINumInputIME::MoveWindow(int _iLeft, int _iTop, int _iWidth, int _iHeight)
{
    DebugPrintf(DLC_UINUMINPUT, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    UIContainer::MoveWindow(_iLeft, _iTop, _iWidth, _iHeight);
	Layout();
    DebugPrintf(DLC_UINUMINPUT, "%s, %d, %s, %s END", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

void UINumInputIME::SetEnterText(const char* text)
{
	if(text)
	{
		m_btnEnter.SetText(text);
	}
}

int UINumInputIME::GetTotalHeight() const
{
    DebugPrintf(DLC_UINUMINPUT, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
	return GetMinHeight();
}

int UINumInputIME::GetTotalWidth() const
{
    DebugPrintf(DLC_UINUMINPUT, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return GetMinWidth();
}

void UINumInputIME::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    DebugPrintf(DLC_UINUMINPUT, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    switch(dwCmdId)
    {
    case ID_BTN_TOUCH_KEYBAORD_NUMBER_0:
    case ID_BTN_TOUCH_KEYBAORD_NUMBER_1:
    case ID_BTN_TOUCH_KEYBAORD_NUMBER_2:
    case ID_BTN_TOUCH_KEYBAORD_NUMBER_3:
    case ID_BTN_TOUCH_KEYBAORD_NUMBER_4:
    case ID_BTN_TOUCH_KEYBAORD_NUMBER_5:
    case ID_BTN_TOUCH_KEYBAORD_NUMBER_6:
    case ID_BTN_TOUCH_KEYBAORD_NUMBER_7:
    case ID_BTN_TOUCH_KEYBAORD_NUMBER_8:
    case ID_BTN_TOUCH_KEYBAORD_NUMBER_9:
	case ID_BTN_TOUCH_KEYBAORD_POINT:
	    {
			m_pTextBox->InsertTextUTF8(numValues + dwCmdId - ID_BTN_TOUCH_KEYBAORD_NUMBER_0, 1);
	    }
        break;
    case ID_KEYBOARD_BACKSPACE:
        {
			m_pTextBox->IsAllSelected() ? m_pTextBox->ClearText() : m_pTextBox->DeleteChar();
        }
        break;
    case ID_KEYBOARD_ENTER:
        {
			m_pTextBox->OnEnterPress();
            HideIME();
        }
        break;
    default:
        break;
    }
    DebugPrintf(DLC_UINUMINPUT, "%s, %d, %s, %s END", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

HRESULT UINumInputIME::DrawBackGround(DK_IMAGE drawingImg)
{
    HRESULT hr(S_OK);
    CTpGraphics grf(drawingImg);
	RTN_HR_IF_FAILED(grf.EraserBackGround(ColorManager::knWhite));
    RTN_HR_IF_FAILED(grf.DrawBorder(0, 0, 0, 0, 0, BORDER_DIALOG, BORDER_CORNER_ROUNDED));
    return hr;
}

bool UINumInputIME::OnHookTouch(MoveEvent* moveEvent)
{
    if (moveEvent->GetActionMasked() != MoveEvent::ACTION_DOWN)
    {
        return false;
    }
    if (!IsShow())
    {
        return false;
    }
    if (!PointInWindowAbsolute(moveEvent->GetX(), moveEvent->GetY())
            && (NULL != m_pTextBox && !m_pTextBox->PointInWindowAbsolute(moveEvent->GetX(), moveEvent->GetY())))
    {
        HideIME();
		return true;
    }
    return false;
}

void UINumInputIME::ShowIME(UIContainer *pParent)
{
	if (m_pTextBox)
    {
        m_pTextBox->SelectAll();
		m_pTextBox->UpdateWindow();
    }
    SetShowDelay(false);
    SetShow(true);
	
	int topX = (CDisplay::GetDisplay()->GetScreenWidth() - GetTotalWidth())/2;
	int topY = (CDisplay::GetDisplay()->GetScreenHeight() - GetTotalHeight())/2;
	if(m_pTextBox)
	{
		int inputBoxY = m_pTextBox->GetAbsoluteY();
		if(inputBoxY > CDisplay::GetDisplay()->GetScreenHeight()/2)
		{
			topY = inputBoxY - GetTotalHeight() - 30;
		}
		else
		{
			topY = inputBoxY + m_pTextBox->GetHeight() + 30;
		}
	}
	
	MoveWindow(topX, topY, GetTotalWidth(), GetTotalHeight());
    if (! IsVisible())
    {
        SetVisible(TRUE);
    }
    m_pParent = pParent;
    AppendChildToTopFullScreenContainer();
	
    RedrawWindow();
}

void UINumInputIME::HideIME()
{
	if (m_pTextBox && m_pTextBox->IsAllSelected())
    {
    	m_pTextBox->SelectAll(false);
		m_pTextBox->UpdateWindow();
    }
    SetVisible(FALSE);
    UIIME::HideIME();
}

