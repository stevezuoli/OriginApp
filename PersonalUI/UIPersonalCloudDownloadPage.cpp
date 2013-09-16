#include "PersonalUI/UIPersonalCloudDownloadPage.h"
#include "Common/WindowsMetrics.h"
#include "GUI/UISizer.h"
#include "I18n/StringManager.h"
#include "CommonUI/CPageNavigator.h"
#include "drivers/DeviceInfo.h"
#include "Utility/PathManager.h"
#include "BookStore/LocalCategoryManager.h"
#include <tr1/functional>
#include "Model/cloud_filesystem_tree.h"
#include "CommonUI/UIUtility.h"
#include "GUI/GUISystem.h"

using namespace WindowsMetrics;
using namespace dk::bookstore;
using namespace dk::utility;

UIPersonalCloudDownloadPage::UIPersonalCloudDownloadPage(ModelTree* model_tree)
    : UISelectPage(BLU_SELECT_EXPAND, model_tree)
{
    m_txtTitle.SetText(PERSONAL_CLOUDBOOKSHELF);
}

UIPersonalCloudDownloadPage::~UIPersonalCloudDownloadPage()
{
    if (m_model)
    {
        ContainerNode* node = m_model->currentNode();
        if (node)
        {
            node->setSelected(false);
        }
    }
}

void UIPersonalCloudDownloadPage::InitModelView()
{
    m_modelView.setRootNodeDisplayMode(EXPAND_ALL);
    m_modelView.setStatusFilter(NODE_LOCAL | NODE_CLOUD);
    UISelectPage::InitModelView();

    const int listItemHeight = GetWindowMetrics(UIPixelValue100Index);
    m_modelView.SetItemHeight(listItemHeight);
    m_modelView.SetItemsPerPage(6);
}

UISizer* UIPersonalCloudDownloadPage::CreateTopSizer()
{
    if (NULL == m_pTopSizer)
    {
        m_pTopSizer = new UIBoxSizer(dkHORIZONTAL);
        if (m_pTopSizer)
        {
            m_txtTitle.SetFontSize(GetWindowFontSize(FontSize22Index));
            UpdateModelView(false);

            AppendChild(&m_btnBack);
            AppendChild(&m_txtTitle);

            const int horzSpacing = GetWindowMetrics(UIPixelValue15Index);
            const int vertSpacing = GetWindowMetrics(UIPixelValue5Index);
            
            m_selectAll.SetText(SELECT_ALL);
            m_selectAll.SetFontSize(GetWindowFontSize(FontSize20Index));
            m_selectAll.SetInternalHorzSpacing(horzSpacing);
            m_selectAll.SetInternalVertSpacing(vertSpacing);
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

UISizer* UIPersonalCloudDownloadPage::CreateModelTreeSizer()
{
    m_pModelTreeSizer = UISelectPage::CreateModelTreeSizer();
    if (m_pModelTreeSizer)
    {
        const int horizontalMargin = GetWindowMetrics(UIPixelValue30Index);
        UISizer* pageNumSizer = new UIBoxSizer(dkHORIZONTAL);
        if (pageNumSizer)
        {
            const int fontSize = GetWindowFontSize(FontSize18Index);
            m_txtSpace.SetFontSize(fontSize);
            m_txtSpace.SetEnglishGothic();
            m_txtPageNo.SetFontSize(fontSize);
            m_txtPageNo.SetEnglishGothic();
            AppendChild(&m_txtSpace);
            AppendChild(&m_txtPageNo);

            pageNumSizer->Add(&m_txtSpace, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
            pageNumSizer->AddStretchSpacer();
            pageNumSizer->Add(&m_txtPageNo, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
            m_pModelTreeSizer->Add(pageNumSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizontalMargin).Border(dkTOP | dkBOTTOM, GetWindowMetrics(UIPixelValue5Index)));
        }
    }
    return m_pModelTreeSizer;
}

UISizer* UIPersonalCloudDownloadPage::CreateBottomSizer()
{
    if (NULL == m_pBottomSizer)
    {
        m_pBottomSizer = new UIBoxSizer(dkHORIZONTAL);
        if (m_pBottomSizer)
        {
            const int btnFontSize = GetWindowFontSize(FontSize18Index);
            m_delete.Initialize(IDSTATIC,StringManager::GetStringById(BOOK_DELETE),btnFontSize);
            m_download.Initialize(IDSTATIC,StringManager::GetStringById(TOUCH_DOWNLOAD),btnFontSize);

            m_btnBottomGroup.AddButton(&m_delete);
            m_btnBottomGroup.AddButton(&m_download);
            m_btnBottomGroup.SetBottomLinePixel(0);
            m_btnBottomGroup.SetMinHeight(GetWindowMetrics(UIBottomBarHeightIndex));
            m_btnBottomGroup.SetSplitLineHeight(GetWindowMetrics(UIBottomBarSplitLineHeightIndex));
            AppendChild(&m_btnBottomGroup);

            m_pBottomSizer->Add(&m_btnBottomGroup, UISizerFlags(1).Expand());
        }
    }
    return m_pBottomSizer;
}

void UIPersonalCloudDownloadPage::UpdateModelView(bool layout)
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

    char space[32] = {0};
    double ns_used = 0, total = 0, available = 0;
    UIUtility::GetCloudSpaceInfo(total, available, ns_used);
    snprintf(space, DK_DIM(space), StringManager::GetPCSTRById(CLOUD_LEFTSPACE), available, total);
    m_txtSpace.SetText(space);

    if (layout)
    {
        Layout();
        UpdateWindow();
    }
}

bool UIPersonalCloudDownloadPage::OnChildClick(UIWindow* child)
{
    if (child == &m_delete)
    {
        return OnDeleteClicked();
    }
    else if (child == &m_download)
    {
        return OnDownloadClicked();
    }

    return UISelectPage::OnChildClick(child);
}

bool UIPersonalCloudDownloadPage::OnDeleteClicked()
{
    if (m_model && (m_modelView.GetSelectedBookCount() > 0))
    {
        UIContainer* pParent = GUISystem::GetInstance()->GetTopFullScreenContainer();
        UIMessageBox messagebox(pParent, StringManager::GetStringById(SURE_DELETE), MB_OK | MB_CANCEL);
        if(MB_OK == messagebox.DoModal())
        {
            ContainerNode* rootNode = m_model->root();
            if (rootNode)
            {
                rootNode->remove(false, false);
                CPageNavigator::BackToPrePage();
            }
        }
    }
    return true;
}

bool UIPersonalCloudDownloadPage::OnDownloadClicked()
{
    SystemSettingInfo* systemSettingInfo = SystemSettingInfo::GetInstance();
    if (m_model && (m_modelView.GetSelectedBookCount() > 0) && systemSettingInfo)
    {
        int64_t total_size = 0;
        int number = 0;
        bool exceed = false;

        NodePtrs selectedChildren = m_model->getSelectedNodesInfo(total_size, number, exceed);
        int64_t freeSize = (int64_t)systemSettingInfo->GetDiskSpace().GetFreeSize();
        double totalSizeM = UIUtility::ConvertByteTo(total_size, "MB");
        if (totalSizeM >= (double)freeSize)
        {
            char text[128] = {0};
            snprintf(text, DK_DIM(text), StringManager::GetStringById(CLOUD_DOWNLOAD_SPACE_TOO_SMALL), (float)freeSize);
            UIContainer* pParent = GUISystem::GetInstance()->GetTopFullScreenContainer();
            UIMessageBox messagebox(pParent, text, MB_OK | MB_CANCEL);
            messagebox.SetButtonText(MB_OK, StringManager::GetStringById(CLOUD_IGNORE));
            if(MB_OK != messagebox.DoModal())
            {
                return false;
            }
        }

        ContainerNode* rootNode = m_model->root();
        if (rootNode)
        {
            rootNode->download(false);
            CPageNavigator::BackToPrePage();
        }
    }
    return true;
}

bool UIPersonalCloudDownloadPage::OnChildSelectChanged(const EventArgs& args)
{
    UpdateModelView(true);
    return true;
}

bool UIPersonalCloudDownloadPage::OnNodesUpdated(const EventArgs& args)
{
    UpdateModelView(true);
    return true;
}

bool UIPersonalCloudDownloadPage::OnListTurnPage(const EventArgs&)
{
    UpdateModelView(true);
    return true;
}

