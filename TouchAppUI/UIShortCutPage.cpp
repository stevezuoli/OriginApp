#include "GUI/CTpGraphics.h"
#include "GUI/UISizer.h"
#include "I18n/StringManager.h"
#include "TouchAppUI/UIShortCutPage.h"
#include "TouchAppUI/UIBottomBar.h"
#include "TouchAppUI/UIFrontLightManager.h"
#include "Common/WindowsMetrics.h"
#include "drivers/DeviceInfo.h"

using namespace WindowsMetrics;

UIShortCutPage::UIShortCutPage() 
    : m_pDownloadPage(NULL)
{
    m_flingGestureListener.SetPage(this);
    m_gestureDetector.SetListener(&m_flingGestureListener);
    SetClickable(true);
	Init();
}

void UIShortCutPage::Init()
{
	AppendChild(&m_titleBar);
    m_titleBar.SetEnable(false);
	AppendChild(UIShortcutBar::GetInstance());

	if (DeviceInfo::IsKPW())  
	{
		AppendChild(UIFrontLightManager::GetInstance());
	}

	if (NULL == m_pDownloadPage)
	{
		m_pDownloadPage = new UIDownloadPage();
		m_pDownloadPage->SetParent(this);
	}
	AppendChild(m_pDownloadPage);
}

void UIShortCutPage::MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight)
{
	int top = m_titleBar.GetHeight() + UIShortcutBar::GetInstance()->GetHeight();
	if (DeviceInfo::IsKPW())  
	{
		const int frontLightHeight = UIFrontLightManager::GetInstance()->GetHeight();
		UIFrontLightManager::GetInstance()->MoveWindow(0, top, DeviceInfo::GetDisplayScreenWidth(), frontLightHeight);
		top += frontLightHeight;
	}
	m_pDownloadPage->MoveWindow(0, top, iWidth, iHeight - top - GetWindowMetrics(UIShortCutPagePushUpHeightIndex));
    m_pDownloadPage->SetFocus(false);
}

UIShortCutPage::~UIShortCutPage()
{
	Dispose();
}

void UIShortCutPage::OnDispose(BOOL bIsDisposed)
{
	if (bIsDisposed)
	{
		return;
	}
	//m_ShowTaskList.Clear();
	//m_bIsDisposed = TRUE;
	Finalize();
}

void UIShortCutPage::Finalize()
{
	if (NULL != m_pDownloadPage)
	{
		delete m_pDownloadPage;
		m_pDownloadPage = NULL;
	}
}

void UIShortCutPage::Dispose()
{
}

//TODO:使用继承会好一些，不过m_iLeft, m_iTop等参数会变得很混乱，使得窗口位置很难控制
void UIShortCutPage::OnEnter()
{
	if (DeviceInfo::IsKPW())  
	{
		UIFrontLightManager::GetInstance()->SetEnableHookTouch(false);
		UIFrontLightManager::GetInstance()->SetVisible(true);
	}
	UIPage::OnEnter();
	UIShortcutBar::GetInstance()->UpdateButtonStatus();
	if (m_pDownloadPage)
	{
		m_pDownloadPage->OnEnter();
	}
}

void UIShortCutPage::OnLeave()
{
	if (DeviceInfo::IsKPW())  
	{
		UIFrontLightManager::GetInstance()->SetEnableHookTouch(true);
		UIFrontLightManager::GetInstance()->SetVisible(false);
	}
	UIPage::OnLeave();
	if (m_pDownloadPage)
	{
		m_pDownloadPage->OnLeave();
	}
}

void UIShortCutPage::OnLoad()
{
	UIPage::OnLoad();
	if (m_pDownloadPage)
	{
		m_pDownloadPage->OnLoad();
	}
}

void UIShortCutPage::OnUnLoad()
{
	UIPage::OnUnLoad();
	if (m_pDownloadPage)
	{
		m_pDownloadPage->OnUnLoad();
	}
}

void UIShortCutPage::UpdateSectionUI(bool bIsFull)
{
	if (m_pDownloadPage)
	{
		m_pDownloadPage->UpdateSectionUI(bIsFull);
	}
}

void UIShortCutPage::OnWifiMessage(SNativeMessage event)
{
	UIShortcutBar::GetInstance()->UpdateButtonStatus();
	if (m_pDownloadPage)
	{
		m_pDownloadPage->OnWifiMessage(event);
	}
}

HRESULT UIShortCutPage::DrawBackGround(DK_IMAGE drawingImg)
{
    HRESULT hr(S_OK);

    UIPage::DrawBackGround(drawingImg);

	CTpGraphics grf(drawingImg);
    DKFontAttributes fontattr;
    fontattr.SetFontAttributes(0,0,GetWindowFontSize(UIShortCutPagePushUpTipsIndex));
    ITpFont* pFont = FontManager::GetInstance()->GetFont(fontattr, ColorManager::GetColor(COLOR_SCROLLBAR_BACK));//字体颜色
	RTN_HR_IF_FAILED(grf.FillRect(m_iLeft, m_iHeight-GetWindowMetrics(UIShortCutPagePushUpHeightIndex), m_iWidth, m_iHeight, 0x141313));//背景颜色
    if (pFont)
    {
        SPtr<ITpImage> imgTips = ImageManager::GetImage(IMAGE_TOUCH_UP_ARROW);
        int stringWidth = pFont->StringWidth(StringManager::GetPCSTRById(TOUCH_PUSH_UP_CLOSE));
        int stringHeight = pFont->GetHeight();
        int stringTop = m_iHeight - ((GetWindowMetrics(UIShortCutPagePushUpHeightIndex) + stringHeight) >> 1);
        int imgTop = stringTop + ((stringHeight - imgTips->GetHeight()) >> 1);
        int space = GetWindowMetrics(UIElementSpacingIndex);
        int totalWidth = stringWidth + space + imgTips->GetWidth();
        int left = (m_iWidth - totalWidth) >> 1;

        RTN_HR_IF_FAILED(grf.DrawImageBlend(imgTips.Get(), left, imgTop, 0, 0, imgTips->bmWidth, imgTips->bmHeight));
        left += (imgTips->GetWidth() + space);
        grf.DrawStringUtf8(StringManager::GetPCSTRById(TOUCH_PUSH_UP_CLOSE), left, stringTop, pFont);
    }

	return hr;
}

bool UIShortCutPage::OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy)
{
    int x = moveEvent1->GetX();
    int y = moveEvent1->GetY();

    switch (direction)
    {
        case FD_UP:
            if (PtInBottomArea(x, y))
            {
                CPageNavigator::BackToPrePage();
                return true;
            }
            break;
		default:
            break; 
    }
    return false;
}

bool UIShortCutPage::OnTouchEvent(MoveEvent* moveEvent)
{
    m_gestureDetector.OnTouchEvent(moveEvent);
    return true;
}

bool UIShortCutPage::PtInBottomArea(int x, int y)
{
    if (y >= (m_iHeight - GetWindowMetrics(UIShortCutPagePushUpHeightIndex)))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool UIShortCutPage::OnSingleTapUp(MoveEvent* moveEvent)
{
    if (PtInBottomArea(moveEvent->GetX(), moveEvent->GetY()))
    {
       CPageNavigator::BackToPrePage();
       return true;
    }
    return false;
}
