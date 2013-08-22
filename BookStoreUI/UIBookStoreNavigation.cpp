#include <tr1/functional>
#include "BookStoreUI/UIBookStoreNavigation.h"
#include "CommonUI/CPageNavigator.h"
#include "CommonUI/UIUtility.h"
#include "I18n/StringManager.h"
#include "GUI/CTpGraphics.h"
#include "Common/WindowsMetrics.h"
#include "Framework/CDisplay.h"
#ifdef KINDLE_FOR_TOUCH
#include "TouchAppUI/UIBookMenuDlg.h"
#endif

using namespace dk::bookstore;
using namespace WindowsMetrics;

static const int TITLEBARLINEHEIGHT = 2;
UIBookStoreNavigation::UIBookStoreNavigation(const std::string& title,
                                             bool hasBottomBar,
                                             bool showFetchInfo,
                                             bool showRefresh)
#ifdef KINDLE_FOR_TOUCH
    : UIPage()
#else
    : UIBookStorePage()
#endif
    , m_fetchStatus(NOFETCH)
    , m_pMidSizer(NULL)
    , m_pTopSizer(NULL)
    , m_pNavigationLeftSizer(NULL)
    , m_pNavigationRightSizer(NULL)
    , m_bShowFetchInfo(showFetchInfo)
    , m_bShowRefresh(showRefresh)
#ifdef KINDLE_FOR_TOUCH
    , m_backButton(NULL, ID_BTN_TOUCH_BACK)
    , m_backToEntryPageButton(NULL, ID_BTN_TOUCH_BACK_TO_ENTRY)
#endif
{
#ifdef KINDLE_FOR_TOUCH
    if (!hasBottomBar)
    {
        RemoveChild(m_pBottomBar,false);
        m_pBottomBar = NULL;
    }
    m_backButton.ShowLeftSpace(false);
#endif
    m_title.SetText(title.c_str());
    SetLeftMargin(GetWindowMetrics(UIHorizonMarginIndex));
    Init();
}

UIBookStoreNavigation::~UIBookStoreNavigation()
{
}

void UIBookStoreNavigation::Init()
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    const int fontsize26 = GetWindowFontSize(FontSize26Index);

    m_title.SetFontSize(fontsize26);
    m_title.SetAlign(ALIGN_CENTER);
    m_title.SetVAlign(ALIGN_CENTER);

#ifdef KINDLE_FOR_TOUCH
    if (m_pBottomBar)
    {
        AppendChild(m_pBottomBar);
    }
#else
    m_titleBar.MoveWindow(0, 0, CDisplay::GetDisplay()->GetScreenWidth(), 30);
    m_pTitleBar = &m_titleBar;
#endif
    AppendChild(&m_titleBar);
}

UISizer* UIBookStoreNavigation::CreateNavigationRightSizer()
{
    if (NULL == m_pNavigationRightSizer)
    {
        m_pNavigationRightSizer = new UIBoxSizer(dkHORIZONTAL);
        if (m_pNavigationRightSizer)
        {
#ifdef KINDLE_FOR_TOUCH
            m_btnRefresh.Initialize(ID_BTN_REFRESH, "", 20);
            m_btnRefresh.SetIcon(ImageManager::GetImage(IMAGE_ICON_REFRESH), UIButton::ICON_LEFT);
            m_btnRefresh.ShowBorder(false);
            m_btnRefresh.SetAlign(ALIGN_CENTER);
            m_btnRefresh.SetElemSpacing(0);
            m_btnRefresh.SetLeftMargin(0);
            m_pNavigationRightSizer->AddStretchSpacer();
            m_pNavigationRightSizer->Add(&m_btnRefresh
                , UISizerFlags().Expand().Center().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIHorizonMarginIndex)).ReserveSpaceEvenIfHidden());
#else
            m_btnRefresh.Initialize(ID_BTN_REFRESH, StringManager::GetPCSTRById(PERSONAL_EXPERIENCE_REFRESH)
                , 'S', GetWindowFontSize(FontSize22Index), ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
            m_pNavigationRightSizer->Add(&m_btnRefresh, UISizerFlags().Center().ReserveSpaceEvenIfHidden());
#endif
            m_btnRefresh.SetVisible(m_bShowRefresh);
            AppendChild(&m_btnRefresh);
            m_pNavigationRightSizer->SetMinHeight(GetWindowMetrics(UIPixelValue61Index));
        }
    }
    return m_pNavigationRightSizer;
}

UISizer* UIBookStoreNavigation::CreateNavigationLeftSizer()
{
    if (NULL == m_pNavigationLeftSizer)
    {
        m_pNavigationLeftSizer = new UIBoxSizer(dkHORIZONTAL);
        if (m_pNavigationLeftSizer)
        {
#ifdef KINDLE_FOR_TOUCH
            m_backButton.ShowSeperator(m_pBottomBar);
            if (!m_pBottomBar)
            {
                const SPtr<ITpImage> imgs[PAT_Count] = 
                {
                    ImageManager::GetImage(IMAGE_TOUCH_HOME), ImageManager::GetImage(IMAGE_TOUCH_STORE), 
                    ImageManager::GetImage(IMAGE_TOUCH_ACCOUNT), ImageManager::GetImage(IMAGE_TOUCH_SETTING)
                };
                PageAncestorType pageType = CPageNavigator::GetCurrentActiveType();
                if (pageType >= 0 && pageType < PAT_Count)
                {
                    m_backToEntryPageButton.SetIcon(imgs[pageType], UIButton::ICON_TOP);
                    m_backToEntryPageButton.ShowBorder(false);
                    m_backToEntryPageButton.SetAlign(ALIGN_CENTER);
                    m_backToEntryPageButton.SetElemSpacing(0);
                    m_backToEntryPageButton.SetTopMargin(0);
                    m_backToEntryPageButton.SetLeftMargin(0);
                }
                m_topLeftBtns.AddButton((UIButton*)&m_backToEntryPageButton);
            }
            m_topLeftBtns.AddButton((UIButton*)&m_backButton);
            m_topLeftBtns.SetSplitLineHeight(GetWindowMetrics(UIPixelValue23Index));
            m_topLeftBtns.SetTopLinePixel(0);
            m_topLeftBtns.SetBottomLinePixel(0);
            AppendChild(&m_topLeftBtns);

            const int leftMargin = m_pBottomBar ? 0 : GetWindowMetrics(UIPixelValue10Index);
            m_pNavigationLeftSizer->Add(&m_topLeftBtns, UISizerFlags().Center().Border(dkLEFT, leftMargin));
#else
            AppendChild(&m_backButton);

            m_pNavigationLeftSizer->Add(&m_backButton, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
#endif
        }
    }
    return m_pNavigationLeftSizer;
}

UISizer* UIBookStoreNavigation::CreateTopSizer()
{
    if (NULL == m_pTopSizer)
    {
        m_pTopSizer = new UIBoxSizer(dkHORIZONTAL);
        if (m_pTopSizer)
        {
            m_title.SetMinHeight(GetWindowMetrics(UIPixelValue61Index));
            AppendChild(&m_title);

            CreateNavigationLeftSizer();
            CreateNavigationRightSizer();
            AdjustTitlePos();
        }
    }

    return m_pTopSizer;
}

void UIBookStoreNavigation::ShowFetchInfo(bool showFetchInfo) 
{
    m_bShowFetchInfo = showFetchInfo;
}

void UIBookStoreNavigation::MoveWindow(INT32 left, INT32 top, INT32 width, INT32 height)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__, __LINE__, GetClassName(), __FUNCTION__);
    UIPage::MoveWindow(left, top, width, height);

    if (!m_windowSizer)
    {
        UISizer* pMainSizer = new UIBoxSizer(dkVERTICAL);
        UISizer* pTopSizer = CreateTopSizer();
        UISizer* pMidSizer = CreateMidSizer();
        if (pMainSizer && pTopSizer && pMidSizer && m_pNavigationRightSizer)
        {
            const int titleLineHeight = GetWindowMetrics(UIBookStoreNavigationTitleBottomLineHeightIndex);
            pMainSizer->Add(&m_titleBar, UISizerFlags().Expand());

#ifdef KINDLE_FOR_TOUCH
            pMainSizer->Add(pTopSizer, UISizerFlags().Expand().Border(dkBOTTOM, titleLineHeight));
#else
            pMainSizer->Add(pTopSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIPixelValue30Index)).Border(dkBOTTOM, titleLineHeight).Border(dkTOP, TITLEBARLINEHEIGHT));
#endif
            UISizerFlags flags = GetMiddleSizerFlags();
            pMainSizer->Add(pMidSizer, flags);
#ifdef KINDLE_FOR_TOUCH
            
            if (m_pBottomBar)
            {
                pMainSizer->Add(m_pBottomBar, UISizerFlags().Expand());
            }
#endif
            UIButtonGroup* pCustomizedBottomBar = 0;
            if ((pCustomizedBottomBar = GetCustomizedBottomBar()) != 0)
            {
                AppendChild(pCustomizedBottomBar);
                pMainSizer->Add(pCustomizedBottomBar, UISizerFlags().Expand());
            }
            SetSizer(pMainSizer);
        }
        else
        {
            SafeDeletePointer(pMainSizer);
            SafeDeletePointer(pTopSizer);
            SafeDeletePointer(pMidSizer);
            SafeDeletePointer(m_pNavigationRightSizer);
        }
    }
    // m_fetchInfo应该最后绘制，否则会被别的窗口挡住
    if (m_bShowFetchInfo)
    {
        AppendChild(&m_fetchInfo);
    }
    FetchInfo();
}

UISizer* UIBookStoreNavigation::CreateMidSizer()
{
    return m_pMidSizer;
}

void UIBookStoreNavigation::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    switch (dwCmdId)
    {
#ifdef KINDLE_FOR_TOUCH
    case ID_BTN_TOUCH_BACK:
        CPageNavigator::BackToPrePage();
        break;
    case ID_BTN_TOUCH_BACK_HOME:
    case ID_BTN_TOUCH_BACK_TO_ENTRY:
        BackToEntryPage();
        break;
#endif
    case ID_LISTBOX_TURNPAGE:
    case ID_BOOKSTORE_RETRY:
    case ID_BTN_REFRESH:
        if(UIUtility::CheckAndReConnectWifi())
        {
            DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s ID_BOOKSTORE_RETRY", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            FetchInfo();
        }
        break;
    default:
#ifndef KINDLE_FOR_TOUCH
        UIBookStorePage::OnCommand(dwCmdId, pSender, dwParam);
#endif
        break;
    }
}

BOOL UIBookStoreNavigation::OnKeyPress(INT32 iKeyCode)
{
#ifdef KINDLE_FOR_TOUCH
    return UIPage::OnKeyPress(iKeyCode);
#else
    return UIBookStorePage::OnKeyPress(iKeyCode);
#endif
}

HRESULT UIBookStoreNavigation::DrawBackGround(DK_IMAGE drawingImg)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    CTpGraphics grf(drawingImg);
    grf.EraserBackGround();
#ifndef KINDLE_FOR_TOUCH
    int lineTop = m_titleBar.GetHeight();
    grf.FillRect(0, lineTop, m_iWidth, lineTop + TITLEBARLINEHEIGHT, ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
#endif

    const int lineTitle = m_title.GetAbsoluteY() + m_title.GetHeight();
    const int titleLineHeight = GetWindowMetrics(UIBookStoreNavigationTitleBottomLineHeightIndex);
    grf.DrawLine(0, lineTitle, m_iWidth, titleLineHeight, SOLID_STROKE);

    return S_OK;
}

FetchDataResult UIBookStoreNavigation::FetchInfo()
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    SetFetchStatus(FETCHING);
    return FDR_FAILED;
}

void UIBookStoreNavigation::SetFetchStatus(FETCH_STATUS status)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s m_fetchStatus = %d, status = %d", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, m_fetchStatus, status);
    if (m_fetchStatus != status)
    {
        m_fetchStatus = status;
        if (m_bShowFetchInfo)
        {
            m_fetchInfo.SetFetchStatus(status);
        }
        Layout();
        Repaint();
    }
}

void UIBookStoreNavigation::OnEnter()
{
#ifdef KINDLE_FOR_TOUCH
    UIPage::OnEnter();
#else
    UIBookStorePage::OnEnter();
#endif
    UpdateSectionUI(true);
}

#ifdef KINDLE_FOR_TOUCH
void UIBookStoreNavigation::BackToEntryPage()
{
    CPageNavigator::BackToEntryPageOnCurrentType();
}

void UIBookStoreNavigation::SetTitle(const std::string& title)
{
    m_title.SetText(title);
    AdjustTitlePos();
}
#endif

void UIBookStoreNavigation::AdjustTitlePos()
{
    if (m_pTopSizer && m_pNavigationLeftSizer && m_pNavigationRightSizer)
    {
        m_pTopSizer->Clear();
        const int elemSpacing = GetWindowMetrics(UIElementSpacingIndex);
        const int titleWidth = m_title.GetTextWidth();
        const int topLeftWidth = m_pNavigationLeftSizer->GetMinSize().GetWidth();
        const int topRightWidth = m_pNavigationRightSizer->GetMinSize().GetWidth();
        const int maxUsedWidth = dk_max(topLeftWidth, topRightWidth);
        const int remainWidth = CDisplay::GetDisplay()->GetScreenWidth() - ((maxUsedWidth + elemSpacing) << 1) - GetWindowMetrics(UIPixelValue10Index);
        if (titleWidth > remainWidth)
        {
            m_title.SetAlign((topLeftWidth >= topRightWidth) ? ALIGN_LEFT : ALIGN_RIGHT);
            //m_pNavigationRightSizer->SetMinWidth(-1);
            m_pTopSizer->Add(m_pNavigationLeftSizer, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
            m_pTopSizer->Add(&m_title, UISizerFlags(1).Expand().Border(dkLEFT | dkRIGHT, elemSpacing));
            m_pTopSizer->Add(m_pNavigationRightSizer, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
        }
        else
        {
            m_title.SetAlign(ALIGN_CENTER);
            //m_pNavigationRightSizer->SetMinSize(dkSize(maxUsedWidth, GetWindowMetrics(UIPixelValue61Index)));
            m_pTopSizer->Add(m_pNavigationLeftSizer, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Border(dkRIGHT, maxUsedWidth - topLeftWidth));
            m_pTopSizer->Add(&m_title, UISizerFlags(1).Expand().Border(dkLEFT | dkRIGHT, elemSpacing));
            m_pTopSizer->Add(m_pNavigationRightSizer, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Border(dkLEFT, maxUsedWidth - topRightWidth));
        }
    }
}


