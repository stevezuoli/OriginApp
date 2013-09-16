#include "PersonalUI/UIPersonalCloudUploadPage.h"
#include "Common/WindowsMetrics.h"
#include "GUI/UISizer.h"
#include "I18n/StringManager.h"
#include "CommonUI/CPageNavigator.h"
#include "drivers/DeviceInfo.h"
#include "Utility/PathManager.h"
#include "BookStore/LocalCategoryManager.h"
#include <tr1/functional>
#include "CommonUI/UIUtility.h"
#include "Model/cloud_filesystem_tree.h"

using namespace WindowsMetrics;
using namespace dk::bookstore;
using namespace dk::utility;

UIPersonalCloudUploadPage::UIPersonalCloudUploadPage(ModelTree* model_tree)
    : UISelectPage(BLU_SELECT_EXPAND, model_tree)
    , m_pageInfoSizer(NULL)
{
    m_txtTitle.SetText(CLOUD_SCANNING);
    ContainerNode* root_node = model_tree->root();
    SubscribeMessageEvent(Node::EventNodeSelectStatusUpdated,
        *root_node,
        std::tr1::bind(
        std::tr1::mem_fn(&UIPersonalCloudUploadPage::onNodeSelectStatusUpdated),
        this,
        std::tr1::placeholders::_1));
}

bool UIPersonalCloudUploadPage::onNodeSelectStatusUpdated(const EventArgs& args)
{
    int64_t total_size = 0;
    int number = 0;
    bool exceed = false;
    NodePtrs selectedChildren = m_model->getSelectedNodesInfo(total_size, number, exceed);
    char text[64] = {0};
    if (total_size <= 0)
    {
        snprintf(text, DK_DIM(text), StringManager::GetPCSTRById(CLOUD_UPLOAD));
    }
    else if (total_size < 1048576)
    {
        snprintf(text, DK_DIM(text), "%s | %d%s | %.2fK",
            StringManager::GetPCSTRById(CLOUD_UPLOAD),
            number, StringManager::GetPCSTRById(CLOUD_UPLOAD_FILE_NUM),
            total_size / 1024.0);
    }
    else
    {
        snprintf(text, DK_DIM(text), "%s | %d%s | %.2fM",
            StringManager::GetPCSTRById(CLOUD_UPLOAD),
            number, StringManager::GetPCSTRById(CLOUD_UPLOAD_FILE_NUM),
            total_size / 1048576.0);
    }
    m_btnUpload.SetText(text);
    UpdateModelView(true);
    return true;
}

void UIPersonalCloudUploadPage::InitModelView()
{
    UISelectPage::InitModelView();
    m_modelView.setRootNodeDisplayMode(EXPAND_ALL);
    m_modelView.setStatusFilter(NODE_LOCAL | NODE_NOT_ON_CLOUD | NODE_SELF_OWN);
}

UISizer* UIPersonalCloudUploadPage::CreateModelTreeSizer()
{
    m_pModelTreeSizer = UISelectPage::CreateModelTreeSizer();
    if (m_pModelTreeSizer)
    {
        m_sepHorz.SetDirection(UISeperator::SD_HORZ);
        m_sepHorz.SetMinHeight(1);
        AppendChild(&m_sepHorz);

        m_pageInfoSizer = new UIBoxSizer(dkHORIZONTAL);
        if (m_pageInfoSizer)
        {
            const int fontSize = GetWindowFontSize(FontSize18Index);
            m_txtSpace.SetFontSize(fontSize);
            m_txtSpace.SetEnglishGothic();
            m_txtPageNo.SetFontSize(fontSize);
            m_txtPageNo.SetEnglishGothic();
            AppendChild(&m_txtSpace);
            AppendChild(&m_txtPageNo);

            m_pageInfoSizer->Add(&m_txtSpace, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
            m_pageInfoSizer->AddStretchSpacer();
            m_pageInfoSizer->Add(&m_txtPageNo, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
            m_pModelTreeSizer->Add(m_pageInfoSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIPixelValue30Index)));
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
            m_btnUpload.ShowBorder(false);
            AppendChild(&m_btnUpload);

            m_pBottomSizer->SetMinHeight(bottomHeight - 1);
            m_pBottomSizer->Add(&m_btnUpload, UISizerFlags(1).Expand());
        }
    }
    return m_pBottomSizer;
}

void UIPersonalCloudUploadPage::UpdateModelView(bool layout)
{
    int bookNum = m_modelView.GetItemNum();
    int curPage = m_modelView.GetCurPageIndex();
    int totalPage = m_modelView.GetTotalPageCount();
    m_btnUpload.SetEnable(TRUE);
    if (bookNum)
    {
        char bufPageNum[32] = {0};
        char bufTitle[128] = {0};
        snprintf(bufPageNum, DK_DIM(bufPageNum), "%d/%d %s", curPage + 1, totalPage,StringManager::GetPCSTRById(BOOK_PAGE));
        snprintf(bufTitle, DK_DIM(bufTitle), StringManager::GetPCSTRById(CLOUD_SCAN_SUCCESSED), bookNum);
        m_txtPageNo.SetText(bufPageNum);
        m_txtTitle.SetText(bufTitle);
    }
    else
    {
        m_txtPageNo.SetText(StringManager::GetPCSTRById(BOOK_00_PAGE));
        m_txtTitle.SetText(StringManager::GetPCSTRById(CLOUD_SCAN_NOBOOK));
        m_btnUpload.SetEnable(FALSE);
    }

    SystemSettingInfo* systemSettingInfo = SystemSettingInfo::GetInstance();
    if (systemSettingInfo)
    {
        int64_t total_size = 0;
        int number = 0;
        bool exceed = false;
        NodePtrs selectedChildren = m_model->getSelectedNodesInfo(total_size, number, exceed);
        double totalSizeM = UIUtility::ConvertByteTo(total_size, "MB");
        char space[32] = {0};
        int64_t freeSize = (int64_t)systemSettingInfo->GetDiskSpace().GetFreeSize();
        if (totalSizeM >= (double)freeSize)
        {
            m_btnUpload.SetEnable(FALSE);
            snprintf(space, DK_DIM(space), StringManager::GetPCSTRById(CLOUD_UPLOAD_SPACE_TOO_SMALL), (float)freeSize);
        }
        else
        {
            double ns_used = 0, total = 0, available = 0;
            UIUtility::GetCloudSpaceInfo(total, available, ns_used);
            snprintf(space, DK_DIM(space), StringManager::GetPCSTRById(CLOUD_LEFTSPACE), available, total);
        }
        m_txtSpace.SetText(space);
    }
    
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
    if (child == &m_btnUpload)
    {
        return OnUploadClicked();
    }

    return UISelectPage::OnChildClick(child);
}

bool UIPersonalCloudUploadPage::OnUploadClicked()
{
    if (m_model)
    {
        ContainerNode* rootNode = m_model->root();
        if (rootNode)
        {
            rootNode->upload(false);
        }
    }
    return true;
}

void UIPersonalCloudUploadPage::OnEnter()
{
    DebugPrintf(DLC_UIHOMEPAGE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    UIPage::OnEnter();

    m_modelView.updateModelByContext(m_model);
    m_modelView.InitListItem();
    UpdateWindow();
}

