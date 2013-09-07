#include "PersonalUI/UIPersonalCloudUploadPage.h"
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

UIPersonalCloudUploadPage::UIPersonalCloudUploadPage(ModelTree* model_tree)
    : UISelectPage(BLU_CLOUD_UPLOAD, model_tree)
    , m_btnBack(this)
{
    m_modelView.setRootNodeDisplayMode(BY_FOLDER);
    m_modelView.setStatusFilter(NODE_LOCAL | NODE_NOT_ON_CLOUD | NODE_SELF_OWN);
    AppendChild(&m_titleBar);
}

UISizer* UIPersonalCloudUploadPage::CreateTopSizer()
{
    if (NULL == m_pTopSizer)
    {
        m_pTopSizer = new UIBoxSizer(dkHORIZONTAL);
        if (m_pTopSizer)
        {
            m_txtTitle.SetText(CLOUD_SCANNING);
            m_txtTitle.SetFontSize(GetWindowFontSize(FontSize22Index));
            UpdateModelView(false);

            AppendChild(&m_btnBack);
            AppendChild(&m_txtTitle);

            const int horzSpacing = GetWindowMetrics(UIPixelValue15Index);
            const int vertSpacing = GetWindowMetrics(UIPixelValue5Index);
            m_imageMode.SetText("image");
            m_imageMode.SetFontSize(GetWindowFontSize(FontSize20Index));
            m_imageMode.SetInternalHorzSpacing(horzSpacing);
            m_imageMode.SetInternalVertSpacing(vertSpacing);

            m_expandMode.SetText("expand");
            m_expandMode.SetFontSize(GetWindowFontSize(FontSize20Index));
            m_expandMode.SetInternalHorzSpacing(horzSpacing);
            m_expandMode.SetInternalVertSpacing(vertSpacing);

            m_selectAll.SetText(SELECT_ALL);
            m_selectAll.SetFontSize(GetWindowFontSize(FontSize20Index));
            m_selectAll.SetInternalHorzSpacing(horzSpacing);
            m_selectAll.SetInternalVertSpacing(vertSpacing);

            m_btnGroup.AddButton(&m_imageMode, UISizerFlags().Expand().Center());
            m_btnGroup.AddButton(&m_expandMode, UISizerFlags().Expand().Center());
            m_btnGroup.AddButton(&m_selectAll, UISizerFlags().Expand().Center());
            AppendChild(&m_btnGroup);
            m_btnGroup.SetTopLinePixel(0);
            m_btnGroup.SetBottomLinePixel(0);
            m_btnGroup.SetSplitLineHeight(GetWindowMetrics(UIPixelValue20Index));

            const int margin = GetWindowMetrics(UIPixelValue10Index);
            const int rightMargin = GetWindowMetrics(UIPixelValue30Index) - horzSpacing;
            m_pTopSizer->SetMinHeight(GetWindowMetrics(UIBackButtonHeightIndex));
            m_pTopSizer->Add(&m_btnBack, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
            m_pTopSizer->Add(&m_txtTitle, UISizerFlags(1).Expand().Border(dkLEFT | dkRIGHT, margin));
            m_pTopSizer->Add(&m_btnGroup, UISizerFlags().Expand().Align(dkALIGN_RIGHT | dkALIGN_CENTER_VERTICAL).Border(dkRIGHT, rightMargin));
        }
    }
    return m_pTopSizer;
}

UISizer* UIPersonalCloudUploadPage::CreateModelTreeSizer()
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
            m_txtTotalBook.SetForeColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
            m_txtTotalBook.SetFont(0, 0, 18);
            m_txtPageNo.SetFont(0, 0, 18);
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

UISizer* UIPersonalCloudUploadPage::CreateBottomSizer()
{
    if (NULL == m_pBottomSizer)
    {
        m_pBottomSizer = new UIBoxSizer(dkHORIZONTAL);
        if (m_pBottomSizer)
        {
            int bottomHeight = GetWindowMetrics(UIBottomBoxHeightIndex);
            m_btnUpload.SetText(CLOUD_UPLOAD);
            m_btnUpload.SetFontSize(GetWindowFontSize(FontSize20Index));
            m_btnUpload.SetLeftMargin(GetWindowMetrics(UIButtonLeftPaddingIndex));
            m_btnUpload.SetTopMargin(GetWindowMetrics(UIButtonTopPaddingIndex));
            m_btnUpload.SetMinSize(GetWindowMetrics(UIAddBookToCategoryPageSaveButtonWidthIndex), bottomHeight - 1);
            m_btnUpload.ShowBorder(false);
            AppendChild(&m_btnUpload);

            int vertSepHeight = GetWindowMetrics(UIAddBookToCategoryPageVertSepHeightIndex);
            m_sepVert1.SetMinSize(1, vertSepHeight);
            m_sepVert1.SetDirection(UISeperator::SD_VERT);
            AppendChild(&m_sepVert1);

            m_sepVert2.SetMinSize(1, vertSepHeight);
            m_sepVert2.SetDirection(UISeperator::SD_VERT);
            AppendChild(&m_sepVert2);

            m_pBottomSizer->AddStretchSpacer();
            m_pBottomSizer->Add(&m_sepVert1, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
            m_pBottomSizer->Add(&m_btnUpload, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
            m_pBottomSizer->Add(&m_sepVert2, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
            m_pBottomSizer->AddStretchSpacer();
        }
    }
    return m_pBottomSizer;
}

void UIPersonalCloudUploadPage::UpdateModelView(bool layout)
{
    int bookNum = m_modelView.GetItemNum();
    int curPage = m_modelView.GetCurPageIndex();
    int totalPage = m_modelView.GetTotalPageCount();
    if (bookNum)
    {
        char bufPageNum[32] = {0};
        char bufTitle[128] = {0};
        snprintf(bufPageNum, DK_DIM(bufPageNum), "%d/%d %s", curPage + 1, totalPage,StringManager::GetPCSTRById(BOOK_PAGE));
        snprintf(bufTitle, DK_DIM(bufTitle), StringManager::GetPCSTRById(CLOUD_SCAN_SUCCESSED), bookNum);
        m_txtPageNo.SetText(bufPageNum);
        m_txtTitle.SetText(bufTitle);
        m_btnUpload.SetEnable(TRUE);
    }
    else
    {
        m_txtPageNo.SetText(StringManager::GetPCSTRById(BOOK_00_PAGE));
        m_txtTitle.SetText(StringManager::GetPCSTRById(CLOUD_SCAN_NOBOOK));
        m_btnUpload.SetEnable(FALSE);
    }

    char buf[32] = {0};
    snprintf(buf, DK_DIM(buf), "%s %d %s", StringManager::GetPCSTRById(BOOK_TOTAL), bookNum, StringManager::GetPCSTRById(BOOK_BEN));
    m_txtTotalBook.SetText(buf);
    
    if (layout)
    {
        Layout();
        Repaint();
    }
}

bool UIPersonalCloudUploadPage::OnChildSelectChanged(const EventArgs& args)
{
    UpdateModelView(true);
    return true;
}

bool UIPersonalCloudUploadPage::OnNodesUpdated(const EventArgs& args)
{
    UpdateModelView(true);
    return true;
}

bool UIPersonalCloudUploadPage::OnListTurnPage(const EventArgs&)
{
    UpdateModelView(true);
    return true;
}

bool UIPersonalCloudUploadPage::OnChildClick(UIWindow* child)
{
    if(child == &m_btnBack)
    {
        return OnBackClick();
    }
    else if (child == &m_imageMode)
    {
        return OnImageModeClick();
    }
    else if (child == &m_expandMode)
    {
        return OnExpandModeClick();
    }
    else if (child == &m_selectAll)
    {
        return OnSelectedAllClick();
    }
    else if (child == &m_btnUpload)
    {
        return OnUploadClicked();
    }
    return false;
}

bool UIPersonalCloudUploadPage::OnBackClick()
{
    if (m_modelView.rootNodeDisplayMode() == BY_FOLDER && m_modelView.BackToUpperFolder())
    {
        UpdateModelView(true);
    }
    else
    {
        CPageNavigator::BackToPrePage();
    }
    return true;
}

bool UIPersonalCloudUploadPage::OnSelectedAllClick()
{
    if (m_model)
    {
        ContainerNode* node = m_model->currentNode();
        if (node)
        {
            node->setSelected(!node->selected());
            Repaint();
        }
    }
    return true;
}

bool UIPersonalCloudUploadPage::OnExpandModeClick()
{
    DKDisplayMode mode = (m_modelView.rootNodeDisplayMode() == BY_FOLDER) ? EXPAND_ALL : BY_FOLDER;
    if (mode == EXPAND_ALL)
    {
        m_modelView.cdRoot();
    }
    m_modelView.setRootNodeDisplayMode(mode);
    m_modelView.InitListItem();
    Repaint();
    return true;
}

bool UIPersonalCloudUploadPage::OnImageModeClick()
{
    ModelDisplayMode newMode = BLM_ICON;
    if (BLM_ICON == SystemSettingInfo::GetInstance()->GetModelDisplayMode())
    {
        newMode = BLM_LIST;
    }
    SystemSettingInfo::GetInstance()->SetModelDisplayMode(newMode);
    m_modelView.SetModelDisplayMode(newMode);
    m_modelView.InitListItem();
    Repaint();
    return true;
}

bool UIPersonalCloudUploadPage::OnUploadClicked()
{
    if (m_model)
    {
        ContainerNode* rootNode = m_model->root();
        if (rootNode)
        {
            rootNode->upload();
        }
    }
    return true;
}

