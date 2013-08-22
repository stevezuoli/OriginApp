#include "CommonUI/UIAccountMigrateDlg.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "I18n/StringManager.h"
#include "drivers/DeviceInfo.h"
#include "Utility/WebBrowserUtil.h"
#include "CommonUI/UIUtility.h"
#include "Framework/CDisplay.h"

using namespace WindowsMetrics;
using namespace dk::utility;

const char* UIAccountMigrateDlg::EventMigrate = "EventMigrate";
const char* UIAccountMigrateDlg::EventMigrateLater = "EventMigrateLater";
const char* UIAccountMigrateDlg::EventLoginXiaomiAccount = "EventLoginXiaomiAccount";

class MigrationOperationArgs: public EventArgs
{
public:
    virtual EventArgs* Clone() const
    {
        return new MigrationOperationArgs(*this);
    }
    int operation_id;
};

UIAccountMigrateDlg::UIAccountMigrateDlg(UIContainer* pParent, PopupMode mode)
	: UIDialog(pParent)
    , m_popupMode(mode)
	, m_migrationTip(this, IDSTATIC)
	, m_splitLine1(this)
	, m_splitLine2(this)
{
    HookTouch();
	InitUI();
}

UIAccountMigrateDlg::~UIAccountMigrateDlg()
{
}

bool UIAccountMigrateDlg::InitUI()
{
	const int horizonMargin = GetWindowMetrics(UIHorizonMarginIndex);
	const int elementSpace = 15;
	int dlgWidth = DeviceInfo::GetDisplayScreenWidth()*3/4;
	int dlgHeight = DeviceInfo::GetDisplayScreenHeight()*3/4;
	m_migrationTip.SetFontSize(GetWindowFontSize(FontSize20Index));
    m_migrationTip.SetMode(UIAbstractText::TextMultiLine);
	m_migrationTip.SetAlign(ALIGN_JUSTIFY);
    m_migrationTip.SetText(GetTipsByMode(m_popupMode));
	m_migrationTip.SetMinWidth(dlgWidth - (horizonMargin << 1));

	m_splitLine1.SetMinHeight(1);
	m_splitLine2.SetMinHeight(1);
	
	m_title.SetAlign(ALIGN_CENTER);
    m_title.SetFontSize(GetWindowFontSize(FontSize24Index));
	m_title.SetText(StringManager::GetStringById(ACCOUNT_XIAOMI_MIGRATE_TITLE));
#ifdef KINDLE_FOR_TOUCH
	m_btnMigration.Initialize(ID_BIN_ACCOUNT_XIAOMI_MIGRATE, StringManager::GetStringById(ACCOUNT_XIAOMI_MIGRATE), GetWindowFontSize(FontSize22Index));
	m_btnMigration.ShowBorder(false);
	m_btnOtherOption.Initialize(ID_BIN_ACCOUNT_XIAOMI_NOT_MIGRATE, GetOpitonTextByMode(m_popupMode), GetWindowFontSize(FontSize22Index));
	m_btnOtherOption.ShowBorder(false);
#else
	m_btnMigration.Initialize(ID_BIN_ACCOUNT_XIAOMI_MIGRATE,
		StringManager::GetStringById(ACCOUNT_XIAOMI_MIGRATE),
		'Y', GetWindowFontSize(FontSize22Index),
		ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
	m_btnOtherOption.Initialize(ID_BIN_ACCOUNT_XIAOMI_NOT_MIGRATE,
		GetOpitonTextByMode(m_popupMode),
		'N', GetWindowFontSize(FontSize22Index),
		ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
#endif
    if (!m_windowSizer)
    {
        UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
        if (mainSizer)
        {
			mainSizer->Add(&m_title, UISizerFlags().Expand().Border(dkALL, horizonMargin));
			mainSizer->Add(&m_migrationTip, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizonMargin));
			mainSizer->AddStretchSpacer();
			mainSizer->Add(&m_splitLine1, UISizerFlags().Expand().Align(dkALIGN_BOTTOM).Border(dkLEFT | dkRIGHT, 2));
            mainSizer->Add(&m_btnMigration, UISizerFlags().Expand().Align(dkALIGN_BOTTOM).Border(dkLEFT | dkRIGHT, horizonMargin).Border(dkUP | dkDOWN, elementSpace));
			mainSizer->Add(&m_splitLine2, UISizerFlags().Expand().Align(dkALIGN_BOTTOM).Border(dkLEFT | dkRIGHT, 2));
            mainSizer->Add(&m_btnOtherOption, UISizerFlags().Expand().Align(dkALIGN_BOTTOM).Border(dkLEFT | dkRIGHT, horizonMargin).Border(dkUP | dkDOWN, elementSpace));
            SetSizer(mainSizer);
        }
    }

    AppendChild(&m_migrationTip);
    AppendChild(&m_btnOtherOption);
    AppendChild(&m_btnMigration);
    AppendChild(&m_title);

    if (NOTICE_FORCE == m_popupMode)
    {
	    m_btnOtherOption.SetVisible(FALSE);
	    m_splitLine2.SetVisible(FALSE);
    }

    MoveWindow((DeviceInfo::GetDisplayScreenWidth() - dlgWidth)/2, (DeviceInfo::GetDisplayScreenHeight() - dlgHeight)/2, dlgWidth, dlgHeight);
    Layout();
#ifndef KINDLE_FOR_TOUCH
    m_btnMigration.SetFocus(TRUE);
#endif
    return true;
}

CString UIAccountMigrateDlg::GetOpitonTextByMode(PopupMode popupMode)
{
    CString ret;
    switch (popupMode)
    {
    case NOTICE_NORMAL:
        ret = StringManager::GetStringById(ACCOUNT_XIAOMI_NOT_MIGRATE);
        break;
    case NOTICE_DUOKAN_LOGIN_CLOSED:
        ret = StringManager::GetStringById(TOUCH_SYSTEMSETTING_LOGIN_XIAOMI_ACCOUNT);
        break;
    case NOTICE_FORCE:
    default:
        break;
    }
    return ret;
}

CString UIAccountMigrateDlg::GetTipsByMode(PopupMode popupMode)
{
    CString ret;
    switch (popupMode)
    {
    case NOTICE_NORMAL:
        ret = StringManager::GetStringById(ACCOUNT_XIAOMI_MIGRATE_TIP);
        break;
    case NOTICE_DUOKAN_LOGIN_CLOSED:
        ret = StringManager::GetStringById(ACCOUNT_DUOKAN_CLOSED_TIP);
        break;
    case NOTICE_FORCE:
    default:
        break;
    }
    return ret;
}

HRESULT UIAccountMigrateDlg::DrawBackGround(DK_IMAGE drawingImg)
{
    HRESULT hr(S_OK);
    DrawBackgroundWithRoundBorder(drawingImg);
    return hr;
}

bool UIAccountMigrateDlg::OnHookTouch(MoveEvent* moveEvent)
{
    if (moveEvent->GetActionMasked() != MoveEvent::ACTION_DOWN)
    {
        return false;
    }
    if (!PointInWindowAbsolute(moveEvent->GetX(), moveEvent->GetY()))
    {
        EndDialog(IDCANCEL);
        return true;
    }
    return false;
}

void UIAccountMigrateDlg::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    switch (dwCmdId)
    {
        case ID_BIN_ACCOUNT_XIAOMI_MIGRATE:
			OnMigrationBtnClick();
            break;
        case ID_BIN_ACCOUNT_XIAOMI_NOT_MIGRATE:
            if (m_popupMode == NOTICE_DUOKAN_LOGIN_CLOSED)
            {
                OnLoginXiaomiAccountClick();
            }
            else
            {
			    OnMigrateLaterBtnClick();
            }
			break;
		default:
	        UIDialog::OnCommand(dwCmdId, pSender, dwParam);
	        break;
    }
}

void UIAccountMigrateDlg::OnMigrationBtnClick()
{
    CDisplay::CacheDisabler cache;
    EndDialog(0);
    MigrationOperationArgs args;
    args.operation_id = 0;
    FireEvent(EventMigrate, args);
}

void UIAccountMigrateDlg::OnMigrateLaterBtnClick()
{
    CDisplay::CacheDisabler cache;
    EndDialog(0);
    MigrationOperationArgs args;
    args.operation_id = 1;
    FireEvent(EventMigrateLater, args);
}

void UIAccountMigrateDlg::OnLoginXiaomiAccountClick()
{
    CDisplay::CacheDisabler cache;
    EndDialog(0);
    MigrationOperationArgs args;
    args.operation_id = 2;
    FireEvent(EventLoginXiaomiAccount, args);
}

