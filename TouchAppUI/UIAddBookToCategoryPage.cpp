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

static const string DK_BOOKSTORE = "DK_BookStore";

UIAddBookToCategoryPage::UIAddBookToCategoryPage(const char* category, ModelTree* model_tree)
    : UISelectPage(BLU_SELECT, model_tree)
{
    m_category = category;

    // jump to book store node
    ContainerNode* bookstore_node = model_tree->cdContainerNodeWithinTopNode(DK_BOOKSTORE);
    if (bookstore_node != 0)
    {
        m_modelView.setStatusFilter(NODE_DUOKAN_BOOK_NOT_CLASSIFIED);
    }
}

UISizer* UIAddBookToCategoryPage::CreateTopSizer()
{
    if (NULL == m_pTopSizer)
    {
        m_pTopSizer = new UIBoxSizer(dkHORIZONTAL);
        if (m_pTopSizer)
        {
            m_txtTitle.SetText(CATEGORY_ADD_FILE_TO_CATEGORY);
            m_txtTitle.SetFontSize(GetWindowFontSize(UIHomePageUpperFolderIndex));
            UpdateSelectedBooks(0, false);

            AppendChild(&m_btnBack);
            AppendChild(&m_txtTitle);
            AppendChild(&m_txtSelected);

            m_pTopSizer->SetMinHeight(GetWindowMetrics(UIBackButtonHeightIndex));
            m_pTopSizer->Add(&m_btnBack, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
            m_pTopSizer->AddStretchSpacer();
            m_pTopSizer->Add(&m_txtTitle, UISizerFlags().Align(dkALIGN_CENTER));
            m_pTopSizer->AddStretchSpacer();
            m_pTopSizer->Add(&m_txtSelected, UISizerFlags().Align(dkALIGN_RIGHT | dkALIGN_CENTER_VERTICAL).Border(dkRIGHT, GetWindowMetrics(UIPixelValue30Index)));
        }
    }
    return m_pTopSizer;
}

UISizer* UIAddBookToCategoryPage::CreateModelTreeSizer()
{
    m_pModelTreeSizer = UISelectPage::CreateModelTreeSizer();
    if (m_pModelTreeSizer)
    {
        m_sepHorz.SetDirection(UISeperator::SD_HORZ);
        m_sepHorz.SetMinHeight(1);
        AppendChild(&m_sepHorz);

        UISizer* pageNumSizer = new UIBoxSizer(dkHORIZONTAL);
        if (pageNumSizer)
        {
            const int fontSize = GetWindowFontSize(FontSize18Index);
            m_txtTotalBook.SetFontSize(fontSize);
            m_txtTotalBook.SetEnglishGothic();
            m_txtPageNo.SetFontSize(fontSize);
            m_txtPageNo.SetEnglishGothic();
            AppendChild(&m_txtTotalBook);
            AppendChild(&m_txtPageNo);

            pageNumSizer->Add(&m_txtTotalBook, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
            pageNumSizer->AddStretchSpacer();
            pageNumSizer->Add(&m_txtPageNo, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
            m_pModelTreeSizer->Add(pageNumSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIPixelValue30Index)));
        }
        m_pModelTreeSizer->AddSpacer(GetWindowMetrics(UIHomePageSpaceOverBottomBarIndex));
        m_pModelTreeSizer->Add(&m_sepHorz, UISizerFlags().Expand());
    }
    return m_pModelTreeSizer;
}

UISizer* UIAddBookToCategoryPage::CreateBottomSizer()
{
    if (NULL == m_pBottomSizer)
    {
        m_pBottomSizer = new UIBoxSizer(dkHORIZONTAL);
        if (m_pBottomSizer)
        {
            int bottomHeight = GetWindowMetrics(UIBottomBoxHeightIndex);
            m_btnSave.SetText(TOUCH_ACTION_SAVE);
            m_btnSave.SetFontSize(GetWindowFontSize(FontSize20Index));
            m_btnSave.SetLeftMargin(GetWindowMetrics(UIButtonLeftPaddingIndex));
            m_btnSave.SetTopMargin(GetWindowMetrics(UIButtonTopPaddingIndex));
            m_btnSave.SetMinSize(GetWindowMetrics(UIAddBookToCategoryPageSaveButtonWidthIndex), bottomHeight - 1);
            m_btnSave.ShowBorder(false);
            AppendChild(&m_btnSave);

            int vertSepHeight = GetWindowMetrics(UIAddBookToCategoryPageVertSepHeightIndex);
            m_sepVert1.SetMinSize(1, vertSepHeight);
            m_sepVert1.SetDirection(UISeperator::SD_VERT);
            AppendChild(&m_sepVert1);

            m_sepVert2.SetMinSize(1, vertSepHeight);
            m_sepVert2.SetDirection(UISeperator::SD_VERT);
            AppendChild(&m_sepVert2);

            m_pBottomSizer->AddStretchSpacer();
            m_pBottomSizer->Add(&m_sepVert1, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
            m_pBottomSizer->Add(&m_btnSave, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
            m_pBottomSizer->Add(&m_sepVert2, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
            m_pBottomSizer->AddStretchSpacer();
        }
    }
    return m_pBottomSizer;
}

void UIAddBookToCategoryPage::UpdateModelView(bool layout)
{
    char buf[50];
    int bookNum = m_modelView.GetItemNum();
    int curPage = m_modelView.GetCurPageIndex();
    int totalPage = m_modelView.GetTotalPageCount();
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
    if (layout)
    {
        Layout();
        UpdateWindow();
    }
}

bool UIAddBookToCategoryPage::OnChildSelectChanged(const EventArgs& args)
{
    const ChildSelectChangedArgs& childSelectChangedArgs =
        (const ChildSelectChangedArgs&)args;
    UpdateSelectedBooks(childSelectChangedArgs.selected);
    return true;
}

bool UIAddBookToCategoryPage::OnNodesUpdated(const EventArgs& args)
{
    UpdateModelView(true);
    return true;
}

bool UIAddBookToCategoryPage::OnListTurnPage(const EventArgs&)
{
    UpdateModelView(true);
    return true;
}

void UIAddBookToCategoryPage::UpdateSelectedBooks(size_t bookCount, bool updateWindow)
{
    char buf[200];
    snprintf(buf, DK_DIM(buf),
            StringManager::GetPCSTRById(CATEGORY_ALREADY_SELECTED),
            bookCount);
    m_txtSelected.SetText(buf);
    if (updateWindow)
    {
        if (m_pTopSizer)
        {
            m_pTopSizer->Layout();
        }
        m_txtSelected.UpdateWindow();
    }
}

bool UIAddBookToCategoryPage::OnChildClick(UIWindow* child)
{
    if (child == &m_btnSave)
    {
        return OnSaveClicked();
    }
    return UISelectPage::OnChildClick(child);
}

bool UIAddBookToCategoryPage::OnSaveClicked()
{
    DK_AUTO(ids, m_modelView.GetSelectedBookIds());
    for (DK_AUTO(cur, ids.begin()); cur != ids.end(); ++cur)
    {
        LocalCategoryManager::AddBookToCategory(m_category.c_str(), cur->c_str());
        m_modelView.InitListItem();
        //DebugPrintf(DLC_DIAGNOSTIC, "%s\n", cur->c_str());
    }
    CPageNavigator::BackToPrePage();
    return true;
}

void UIAddBookToCategoryPage::OnEnter()
{
    DebugPrintf(DLC_UIHOMEPAGE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    UIPage::OnEnter();

    m_modelView.updateModelByContext(m_model);
    m_modelView.InitListItem();
    UpdateWindow();
}
