////////////////////////////////////////////////////////////////////////
// UITablePage.h
// Contact:
////////////////////////////////////////////////////////////////////////

#include "TouchAppUI/UITablePage.h"
#include "Framework/CDisplay.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
using namespace WindowsMetrics;

#ifdef KINDLE_FOR_TOUCH

#include <tr1/functional>

UITablePage::UITablePage()
            :UIPage()
            ,m_pShowPage(NULL)
{
    m_tabBar.SetCurrentIndex(0);
    AppendChild(&m_tabBar);

    SubscribeMessageEvent(
        UITabBar::TabIndexChangedEvent,
        m_tabBar,
        std::tr1::bind(
        std::tr1::mem_fn(&UITablePage::OnTabIndexChanged),
        this,
        std::tr1::placeholders::_1));
}

#else
UITablePage::UITablePage()
            :UIPage()
            ,m_iSelectedIndex(0)
            ,m_pShowPage(NULL)
            ,m_titleBtns()
{
}
#endif


UITablePage::~UITablePage()
{
    m_pShowPage = NULL;
#ifndef KINDLE_FOR_TOUCH
    for(size_t i = 0; i < m_titleBtns.size(); i++)
    {
        if(m_titleBtns[i])
        {
            delete m_titleBtns[i];
            m_titleBtns[i] = NULL;
			delete m_panels[i];
			m_panels[i] = NULL;
        }
    }
    m_titleBtns.clear();
#else
    for (size_t i = 0; i < m_panels.size(); ++i)
    {
        if (m_panels[i])
        {
            UIContainer* pParent = m_panels[i]->GetParent();
            if (pParent)
            {
                pParent->RemoveChild(m_panels[i], false);
            }
            delete m_panels[i];
            m_panels[i] = NULL;
        }
    }
#endif
	m_panels.clear();
}

#ifdef KINDLE_FOR_TOUCH
void UITablePage::AppendTableBox(LPCSTR Text,const DWORD dwId,UIContainer* Page, BOOL bIsSelected)
{
	const int allDirectionMargin = GetWindowMetrics(UITablePageTableMarginIndex);
	const int tableBorder = GetWindowMetrics(UITablePageTableBorderIndex);
    if(Text && Page)
    {
        m_tabBar.AddTab(Text, dwId);
        m_panels.push_back(Page);
        Page->SetMinSize(dkSize(m_iWidth - ((allDirectionMargin + tableBorder) << 1), m_iHeight));
        Page->SetVisible(bIsSelected);
        AppendChild(Page);
        if(bIsSelected)
        {
            m_pShowPage = Page;
        }
    }
}
#else
void UITablePage::AppendTableBox(LPCSTR Text,CHAR cHotKey,const DWORD dwId,UIContainer* Page, BOOL bIsSelected)
{
	const int btnTitleFontSize = GetWindowFontSize(UITablePageBtnTitleIndex);
	const int btnTitleHeight = GetWindowMetrics(UITablePageBtnTitleHeightIndex);
	const int allDirectionMargin = GetWindowMetrics(UITablePageTableMarginIndex);
	const int tableBorder = GetWindowMetrics(UITablePageTableBorderIndex);
    if(Text && Page)
    {
		UIComplexButton * _pBtnTitle = new UIComplexButton();
		if(!_pBtnTitle)
		{
			return;
		}
		_pBtnTitle->SetFontSize(btnTitleFontSize);
		_pBtnTitle->SetHotKey(cHotKey);
		_pBtnTitle->SetText(CString(Text));
		_pBtnTitle->SetAlign(ALIGN_CENTER);
		_pBtnTitle->SetId(dwId);
		_pBtnTitle->SetIcon(ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
		_pBtnTitle->SetMinHeight(btnTitleHeight);
		_pBtnTitle->SetCornerStyle(true, true, true, true);
		_pBtnTitle->SetBackgroundColor(ColorManager::GetColor(COLOR_SCROLLBAR_SLIDER_BORDER));

        m_titleBtns.push_back(_pBtnTitle);
		m_panels.push_back(Page);

		Page->SetMinSize(dkSize(m_iWidth - ((allDirectionMargin + tableBorder) << 1), m_iHeight));

        AppendChild(_pBtnTitle);
        AppendChild(Page);
        if(bIsSelected)
        {
        	_pBtnTitle->SetBackgroundColor(ColorManager::knWhite);
        	m_pShowPage = Page;
            m_iSelectedIndex = m_titleBtns.size() - 1;
        }
    }
}
#endif

#ifdef KINDLE_FOR_TOUCH
void UITablePage::LayOutTableBox()
{
	const int allDirectionMargin = GetWindowMetrics(UITablePageTableMarginIndex);
    if (!m_windowSizer)
    {
        UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
        const int tabBarHeight = GetWindowMetrics(UITabBarHeightIndex);
        m_tabBar.SetMinSize(dkSize(m_iWidth, tabBarHeight));
        m_tabBar.SetSplitLineHeight(tabBarHeight);
        m_tabBar.SetHighlightMode(UITabBar::HM_TOPLINE);
        mainSizer->Add(&m_tabBar);
        mainSizer->AddStretchSpacer();
        if(m_pShowPage)
        {
            mainSizer->Add(m_pShowPage, UISizerFlags(1).Expand().Border(dkALL, allDirectionMargin));
        }
        SetSizer(mainSizer);
    }
}
#else
void UITablePage::LayOutTableBox()
{
	const int tableBorder = GetWindowMetrics(UITablePageTableBorderIndex);
	const int allDirectionMargin = GetWindowMetrics(UITablePageTableMarginIndex);
    INT32 iBoxCount = m_titleBtns.size();
	if (!m_windowSizer)
	{
		UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);

		UISizer* titlesSizer = new UIGridSizer(iBoxCount);
		for(int i=0; i<iBoxCount; i++)
		{
			titlesSizer->Add(m_titleBtns[i], UISizerFlags().Expand().Align(dkALIGN_CENTRE));
		}

		mainSizer->Add(titlesSizer, UISizerFlags().Expand().Border(dkRIGHT | dkLEFT | dkTOP, tableBorder));
		if(m_pShowPage)
		{
			mainSizer->Add(m_pShowPage, UISizerFlags().Expand().Border(dkALL, allDirectionMargin));
		}
		SetSizer(mainSizer);
	}
}
#endif

HRESULT UITablePage::DrawBackGround(DK_IMAGE drawingImg)
{
	const int tableBorder = GetWindowMetrics(UITablePageTableBorderIndex);
    UIPage::DrawBackGround(drawingImg);
    CTpGraphics grf(drawingImg);
#ifdef KINDLE_FOR_TOUCH
    int color = ColorManager::knBlack;
#else
    int color = ColorManager::GetColor(COLOR_DISABLECOLOR);
#endif
    grf.FillRect(0, 0, m_iWidth, tableBorder, color); // top
    grf.FillRect(0, 0, tableBorder, m_iHeight, color); // left
   	grf.FillRect(m_iWidth - tableBorder, 0, m_iWidth, m_iHeight, color); // right
    grf.FillRect(0, m_iHeight - tableBorder, m_iWidth, m_iHeight, color); // bottom
    return S_OK;
}

#ifdef KINDLE_FOR_TOUCH
bool UITablePage::OnTabIndexChanged(const EventArgs& args)
{
    const TabIndexChangedEventArgs& tabIndexChangedArgs = 
        (const TabIndexChangedEventArgs&)args;
    const int selectedIndex = tabIndexChangedArgs.m_currentTabIndex;

    UISizer* mainSizer = GetSizer();
    if (mainSizer && (selectedIndex >= 0) && (selectedIndex < (int)m_panels.size()))
    {
        UIContainer* pNewPage = m_panels[selectedIndex];
        if (pNewPage && (pNewPage != m_pShowPage))
        {
            mainSizer->Replace(m_pShowPage, pNewPage);
            m_pShowPage->SetVisible(false);
            m_pShowPage = pNewPage;
            m_pShowPage->SetVisible(true);
            Layout();
            Repaint();
        }
    }
    return false;
}
#else
void UITablePage::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
	switch(dwCmdId)
	{
	case ID_TABLEBOX_BOOKINFO:
    case ID_TABLEBOX_BOOKREVIEW:
    case ID_TABLEBOX_BOOKCATALOGUE:
		for(size_t i = 0; i < m_titleBtns.size(); i++)
		{
			if(m_titleBtns[i]->GetId() == dwCmdId)
			{
				m_titleBtns[m_iSelectedIndex]->SetBackgroundColor(ColorManager::GetColor(COLOR_SCROLLBAR_SLIDER_BORDER));
				m_iSelectedIndex = i;
				UISizer* mainSizer = GetSizer();
				mainSizer->Replace(m_pShowPage, m_panels[m_iSelectedIndex]);
				m_pShowPage->SetVisible(false);
				m_pShowPage = m_panels[m_iSelectedIndex];
				m_pShowPage->SetVisible(true);
				m_titleBtns[m_iSelectedIndex]->SetBackgroundColor(ColorManager::knWhite);
			}
		}
		Layout();
		Repaint();
		break;
	default:
		break;

	}
}
#endif

