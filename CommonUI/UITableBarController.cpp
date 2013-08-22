////////////////////////////////////////////////////////////////////////
// UITableController.h
// Contact:
////////////////////////////////////////////////////////////////////////

#include <tr1/functional>
#include "CommonUI/UITableBarController.h"
#include "CommonUI/UITablePanel.h"
#include "Utility/ColorManager.h"
#include "Framework/CDisplay.h"
#include "GUI/CTpGraphics.h"
#ifdef KINDLE_FOR_TOUCH
#include "GUI/UITouchComplexButton.h"
#else
#include "GUI/UIComplexButton.h"
#endif
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"

using namespace WindowsMetrics;

const char* UITableController::EventPanelIsVisible = "EventRecommendListUpdate";

UITableBar::UITableBar(UIContainer* pParent)
    : UIButtonGroup(pParent)
    , m_imageID(IMAGE_BOOKSTORE_TABLEBAR_ARROW_TOP)
{
    InitUI();
}

UITableBar::~UITableBar()
{
    for (size_t i = 0; i < m_buttons.size(); ++i)
    {
        SafeDeletePointer(m_buttons[i]);
    }
    m_buttons.clear();
}

void UITableBar::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    int currentFocusedIndex = IndexOfWindow(pSender);
    if ((currentFocusedIndex < 0) || (currentFocusedIndex >= (int)GetChildrenCount()) || (m_currentFocusedIndex == currentFocusedIndex))
    {
        UpdateWindow();
        return;
    }

    m_currentFocusedIndex = currentFocusedIndex;
    if (m_pParent)
    {
        m_pParent->OnCommand(dwCmdId, this, m_currentFocusedIndex);
    }
}

void UITableBar::SetFocusedImageID(int imageID)
{
    m_imageID = imageID;
}

HRESULT UITableBar::DrawFocusedSymbol(DK_IMAGE& image)
{
    HRESULT hr(S_OK);
    UIButton* button = (UIButton*)GetChildByIndex(m_currentFocusedIndex);
    SPtr<ITpImage> spImg = ImageManager::GetImage(m_imageID);
    if (button && spImg)
    {
        const int btnWidth = button->GetWidth();
        CTpGraphics grfButton(image);
        grfButton.DrawImage(spImg.Get(), button->GetX() + ((btnWidth - spImg->GetWidth()) >> 1), (m_iHeight - spImg->GetHeight()));
    }

    return hr;
}

void UITableBar::InitUI()
{
    CDisplay *pDisplay = CDisplay::GetDisplay();
    if(pDisplay)
    {
        SetMinWidth(pDisplay->GetScreenWidth());
    }
    SetMinHeight(GetWindowMetrics(UITableControllerHeightIndex));
    SetTopLinePixel(0);
    SetSplitLineHeight(GetWindowMetrics(UITableControllerSplitLineHeightIndex));
}

bool UITableBar::AddTab(const char* title, const DWORD cmdId, bool bIsSelected, char cHotKey)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
#ifdef KINDLE_FOR_TOUCH
    UITouchComplexButton* newTabButton = new UITouchComplexButton;
    if (newTabButton == NULL)
    {
        return false;
    }
    newTabButton->Initialize(cmdId, title, GetWindowFontSize(FontSize22Index));
    newTabButton->ShowBorder(false);
#else
    UIComplexButton* newTabButton = new UIComplexButton;
    if (newTabButton == NULL)
    {
        return false;
    }
    newTabButton->Initialize(cmdId, title, cHotKey, GetWindowFontSize(FontSize22Index), ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    newTabButton->SetEnabledFontColor(ColorManager::knBlack);
#endif

    if(bIsSelected)
    {
        m_currentFocusedIndex = m_buttons.size();
#ifndef KINDLE_FOR_TOUCH
        newTabButton->SetEnable(false);
#endif
    }

    AddButton(newTabButton);
    m_buttons.push_back(newTabButton);
    return (m_buttons.size() > 0);
}


UITableController::UITableController()
            : UIContainer()
            , m_pShowPanel(NULL)
            , m_tableBar(this)
            , m_selectedIndex(-1)
            , m_initPanelVisibleStatus(true)
{
}

UITableController::~UITableController()
{
    m_pShowPanel = NULL;
    for (size_t i = 0; i < m_panels.size(); ++i)
    {
        SafeDeletePointer(m_panels[i]);
    }
    m_panels.clear();
}

void UITableController::SetInitPanelVisibleStatus(bool visible)
{
    m_initPanelVisibleStatus = visible;
}

bool UITableController::GetInitPanelVisibleStatus() const
{
    return m_initPanelVisibleStatus;
}

void UITableController::SetBottomLinePixel(int pixel)
{
    m_tableBar.SetBottomLinePixel(pixel);
}

void UITableController::SetFocusedImageID(int imageID)
{
    m_tableBar.SetFocusedImageID(imageID);
}

void UITableController::AppendTableBox(const DWORD dwId, LPCSTR pText, UITablePanel* pPanel, bool bIsSelected, char cHotKey)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if(pText && pPanel)
    {
        m_tableBar.AddTab(pText, dwId, bIsSelected, cHotKey);
        m_panels.push_back(pPanel);
        pPanel->SetVisible(bIsSelected);
        AppendChild(pPanel);
        if(bIsSelected)
        {
            m_pShowPanel = pPanel;
            m_selectedIndex = m_panels.size() - 1;
        }
    }
}

void UITableController::LayOutTableBox()
{
    if (!m_windowSizer)
    {
        DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        UISizer* pMainSizer = new UIBoxSizer(dkVERTICAL);
        const int tableBorder = GetWindowMetrics(UIDialogBorderLineIndex);
        if (pMainSizer)
        {
            const int elemSpacing = tableBorder >> 1;
            pMainSizer->Add(&m_tableBar);
            if(m_pShowPanel)
            {
                pMainSizer->Add(m_pShowPanel, UISizerFlags(1).Expand().Border(dkLEFT, m_iLeftMargin).Border(dkRIGHT, m_iLeftMargin + GetWindowMetrics(elemSpacing)));
            }
            SetSizer(pMainSizer);
        }
    }
}

void UITableController::ShowPanel(bool isShown)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (m_pShowPanel)
    {
        m_pShowPanel->SetVisible(isShown);
    }
}

void UITableController::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (m_pParent)
    {
        int currentIndex = (int)dwParam;
        if ((pSender == &m_tableBar) && (m_selectedIndex != currentIndex) && (currentIndex >= 0))
        {
            m_selectedIndex = currentIndex;
            UISizer* mainSizer = GetSizer();
            if (mainSizer)
            {
                UITablePanel* pNewPanel = m_panels[m_selectedIndex];
                if (pNewPanel && (pNewPanel != m_pShowPanel))
                {
                    mainSizer->Replace(m_pShowPanel, pNewPanel);
                    m_pShowPanel->SetVisible(false);
                    m_pShowPanel = pNewPanel;
                    m_pShowPanel->SetVisible(m_initPanelVisibleStatus);

                    // Notify parents
                    EventPanelIsVisibleArgs args;
                    args.m_pShowPanel = m_pShowPanel;
                    args.m_bIsVisible = true;
                    FireEvent(EventPanelIsVisible, args);

                    Layout();
                    UpdateWindow();
                }
            }
        }
        m_pParent->OnCommand(dwCmdId, pSender, dwParam);
    }
}

BOOL UITableController::OnKeyPress(INT32 iKeyCode)
{
#ifndef KINDLE_FOR_TOUCH
    if (m_pShowPanel && (iKeyCode == KEY_BACK))
    {
        return m_pShowPanel->OnKeyPress(iKeyCode);
    }
    else if (m_pShowPanel && !m_pShowPanel->IsFocus() && (iKeyCode == KEY_UP || iKeyCode == KEY_DOWN))
    {
        return m_pShowPanel->OnKeyPress(iKeyCode);
    }
#endif
    return UIContainer::OnKeyPress(iKeyCode);
}

HRESULT UITableController::DrawBackGround(DK_IMAGE drawingImg)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    HRESULT hr(S_OK);
    CTpGraphics grf(drawingImg);
    RTN_HR_IF_FAILED(grf.EraserBackGround());
    return hr;
}



