#include "TouchAppUI/UIAddBookToCategoryPage.h"
#include "Common/WindowsMetrics.h"
#include "GUI/UISizer.h"
#include "I18n/StringManager.h"
#include "CommonUI/CPageNavigator.h"
#include "drivers/DeviceInfo.h"
#include "Utility/PathManager.h"
#include "BookStore/LocalCategoryManager.h"
#include <tr1/functional>

using namespace WindowsMetrics;
using namespace dk::bookstore;
using namespace dk::utility;

UIAddBookToCategoryPage::UIAddBookToCategoryPage(const char* category)
    : m_btnBack(this)
    , m_fileListBox(BLU_SELECT)
{
    m_category = category;
    InitUI();
    SubscribeEvent(UIDirectoryListBox::EventChildSelectChanged,
            m_fileListBox,
            std::tr1::bind(
                std::tr1::mem_fn(&UIAddBookToCategoryPage::OnChildSelectChanged),
                this, std::tr1::placeholders::_1));
    CONNECT(m_fileListBox, UICompoundListBox::ListTurnPage, this, UIAddBookToCategoryPage::OnListTurnPage)
}

void UIAddBookToCategoryPage::InitUI()
{
    const int horizonMargin = GetWindowMetrics(UIHorizonMarginIndex);

    m_txtTitle.SetText(CATEGORY_ADD_FILE_TO_CATEGORY);
    m_txtTitle.SetFontSize(GetWindowFontSize(UIHomePageUpperFolderIndex));

    const int listItemHeight = GetWindowMetrics(UIHomePageListItemHeightIndex);
    const int listItemSpacing = GetWindowMetrics(UIHomePageListItemSpacingIndex);
    m_fileListBox.SetLeftMargin(horizonMargin);
    m_fileListBox.SetItemHeight(listItemHeight);
    int screenWidth = DeviceInfo::GetDisplayScreenWidth();
    m_fileListBox.SetItemWidth(screenWidth);
    m_fileListBox.SetItemSpacing(listItemSpacing);

    m_fileListBox.SetBookListMode(
            SystemSettingInfo::GetInstance()->GetBookListMode());
    m_fileListBox.SetCurrentPath(PathManager::GetBookStorePath().c_str());
    m_btnSave.SetText(TOUCH_ACTION_SAVE);
    m_btnSave.SetFontSize(GetWindowFontSize(FontSize20Index));
    m_btnSave.SetLeftMargin(GetWindowMetrics(UIButtonLeftPaddingIndex));
    m_btnSave.SetTopMargin(GetWindowMetrics(UIButtonTopPaddingIndex));
    UpdateSelectedBooks(0);

    m_txtTotalBook.SetForeColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
    m_txtTotalBook.SetFont(0, 0, 18);
    m_txtPageNo.SetFont(0, 0, 18);
	m_txtPageNo.SetEnglishGothic();

    AppendChild(&m_titleBar);
    AppendChild(&m_btnBack);
    AppendChild(&m_txtTitle);
    AppendChild(&m_txtSelected);
    AppendChild(&m_fileListBox);
    AppendChild(&m_btnSave);
    AppendChild(&m_sepHorz);
    AppendChild(&m_sepVert1);
    AppendChild(&m_sepVert2);
    AppendChild(&m_txtTotalBook);
    AppendChild(&m_txtPageNo);
}

bool UIAddBookToCategoryPage::OnSaveClicked()
{
    DK_AUTO(ids, m_fileListBox.GetSelectedBookIds());
    for (DK_AUTO(cur, ids.begin()); cur != ids.end(); ++cur)
    {
        LocalCategoryManager::AddBookToCategory(
                m_category.c_str(),
                cur->c_str());
        DebugPrintf(DLC_DIAGNOSTIC, "%s\n", cur->c_str());
    }
    CPageNavigator::BackToPrePage();
    return true;
}

bool UIAddBookToCategoryPage::OnChildClick(UIWindow* child)
{
    if(child == &m_btnBack)
    {
        CPageNavigator::BackToPrePage();
        return true;
    }
    else if (child == &m_btnSave)
    {
        return OnSaveClicked();
    }
    return false;
}

void UIAddBookToCategoryPage::MoveWindow(
        int left,
        int top,
        int width,
        int height)
{
    const int horizonMargin = GetWindowMetrics(UIHorizonMarginIndex);
    UIWindow::MoveWindow(left, top, width, height);
    if (!m_windowSizer)
    {
        UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
        mainSizer->Add(&m_titleBar);

        UISizer* topSizer = new UIBoxSizer(dkHORIZONTAL);
        topSizer->SetMinHeight(GetWindowMetrics(UIBackButtonHeightIndex));
        topSizer->Add(&m_btnBack, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
        topSizer->AddStretchSpacer();
        topSizer->Add(&m_txtTitle, UISizerFlags().Align(dkALIGN_CENTER));
        topSizer->AddStretchSpacer();
        topSizer->Add(&m_txtSelected, UISizerFlags().Align(dkALIGN_RIGHT | dkALIGN_CENTER_VERTICAL));
        mainSizer->Add(topSizer,
                UISizerFlags().Expand().Border(dkRIGHT, horizonMargin));

        mainSizer->Add(&m_fileListBox, UISizerFlags(1).Expand());
        int bottomHeight = GetWindowMetrics(UIBottomBoxHeightIndex);
        m_sepHorz.SetDirection(UISeperator::SD_HORZ);
        m_sepHorz.SetMinHeight(1);
        UISizer* pageNumSizer = new UIBoxSizer(dkHORIZONTAL);
        pageNumSizer->Add(&m_txtTotalBook, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
        pageNumSizer->AddStretchSpacer();
        pageNumSizer->Add(&m_txtPageNo, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
        mainSizer->Add(pageNumSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizonMargin));
        mainSizer->AddSpacer(GetWindowMetrics(UIHomePageSpaceOverBottomBarIndex));
        mainSizer->Add(&m_sepHorz, UISizerFlags().Expand());

        UISizer* bottomSizer = new UIBoxSizer(dkHORIZONTAL);
        bottomSizer->AddStretchSpacer();

        int vertSepHeight = GetWindowMetrics(UIAddBookToCategoryPageVertSepHeightIndex);
        m_sepVert1.SetMinSize(1, vertSepHeight);
        m_sepVert1.SetDirection(UISeperator::SD_VERT);
        bottomSizer->Add(&m_sepVert1, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));

        m_btnSave.SetMinSize(GetWindowMetrics(UIAddBookToCategoryPageSaveButtonWidthIndex), bottomHeight - 1);
        m_btnSave.ShowBorder(false);
        bottomSizer->Add(&m_btnSave, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));

        m_sepVert2.SetMinSize(1, vertSepHeight);
        m_sepVert2.SetDirection(UISeperator::SD_VERT);
        bottomSizer->Add(&m_sepVert2, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
        bottomSizer->AddStretchSpacer();

        mainSizer->Add(bottomSizer, UISizerFlags().Expand());
        SetSizer(mainSizer);
        Layout();
    }
    m_fileListBox.InitListItem();
    m_fileListBox.UpdateListItem();
    UpdatePageNumber();
    Layout();
}

void UIAddBookToCategoryPage::UpdatePageNumber()
{
    char buf[50];
    int bookNum = m_fileListBox.GetItemNum();
    int curPage = m_fileListBox.GetCurPageIndex();
    int totalPage = m_fileListBox.GetTotalPageCount();
    if (bookNum)
    {
		snprintf(buf, DK_DIM(buf), "%d/%d %s", curPage + 1, totalPage,StringManager::GetPCSTRById(BOOK_PAGE));
    }
    else
    {
		snprintf(buf, DK_DIM(buf), "%s", StringManager::GetPCSTRById(BOOK_00_PAGE));
    }
    m_txtPageNo.SetText(buf);

    snprintf(buf, DK_DIM(buf), "%s %d %s", StringManager::GetPCSTRById(BOOK_TOTAL), bookNum, StringManager::GetPCSTRById(BOOK_BEN));
    m_txtTotalBook.SetText(buf);
}

void UIAddBookToCategoryPage::UpdateSelectedBooks(size_t bookCount)
{
    char buf[200];
    snprintf(buf, DK_DIM(buf),
            StringManager::GetPCSTRById(CATEGORY_ALREADY_SELECTED),
            bookCount);
    m_txtSelected.SetText(buf);
    Layout();
    UpdateWindow();
}

bool UIAddBookToCategoryPage::OnChildSelectChanged(const EventArgs& args)
{
    const ChildSelectChangedArgs& childSelectChangedArgs =
        (const ChildSelectChangedArgs&)args;
    UpdateSelectedBooks(childSelectChangedArgs.selected);
    return true;
}

bool UIAddBookToCategoryPage::OnListTurnPage(const EventArgs&)
{
    UpdatePageNumber();
    Layout();
    UpdateWindow();
    return true;
}
