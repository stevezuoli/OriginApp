#include "PersonalUI/UIPersonalCloudBookShelfPage.h"
#include "PersonalUI/UIPersonalCloudUploadPage.h"
#include "PersonalUI/UIPersonalCloudDownloadPage.h"
#include "Common/WindowsMetrics.h"
#include "Common/CAccountManager.h"
#include "BookStore/BookDownloadController.h"
#include "GUI/UISizer.h"
#include "I18n/StringManager.h"
#include "CommonUI/CPageNavigator.h"
#include "drivers/DeviceInfo.h"
#include "Utility/PathManager.h"
#include "BookStore/LocalCategoryManager.h"
#include <tr1/functional>
#include "CommonUI/UIUtility.h"
#include "TouchAppUI/UITouchMenu.h"
#include "Model/cloud_filesystem_tree.h"

using namespace WindowsMetrics;
using namespace dk::bookstore;
using namespace dk::utility;
using namespace dk::account;

UIPersonalCloudBookShelfPage::UIPersonalCloudBookShelfPage()
    : UIPage()
    , m_btnBack(this)
    , m_myPurcharsedList(GetWindowMetrics(UIBookStoreListBoxPageItemCountIndex), GetWindowMetrics(UIPixelValue120Index))
    , m_freeBookList(3, GetWindowMetrics(UIBookStoreTablePageListBoxItemHeightIndex))
    , m_model(ModelTree::getModelTree(MODEL_MICLOUD))
    , m_cloudModelView(BLU_BROWSE, m_model)
    , m_wndStatus(NONE)
    , m_pPurcharsedNULLSizer(NULL)
    , m_pUploadNULLSizer(NULL)
    , m_pUploadSizer(NULL)
    , m_pSpaceInfoSizer(NULL)
    , cloud_files_ready_(false)
    , cloud_quota_ready_(false)
{
    SubscribeMessageEvent(BookStoreInfoManager::EventPurchasedListUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIPersonalCloudBookShelfPage::OnMessageListBoxUpdate),
        this,
        std::tr1::placeholders::_1)
        );

    SubscribeMessageEvent(BookStoreInfoManager::EventFreeListUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIPersonalCloudBookShelfPage::OnMessageFreeListUpdate),
        this,
        std::tr1::placeholders::_1)
        );

    SubscribeMessageEvent(CAccountManager::EventAccount,
        *CAccountManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIPersonalCloudBookShelfPage::onAccountEvent),
        this,
        std::tr1::placeholders::_1)
        );

//     CONNECT(m_cloudModelView, UICompoundListBox::ListItemClick, this, UIPersonalCloudBookShelfPage::OnCloudListItemClick)
    CONNECT(m_cloudModelView, UICompoundListBox::ListTurnPage, this, UIPersonalCloudBookShelfPage::OnCloudListContentChanged)
    CONNECT(m_cloudModelView, UIModelView::EventNodesUpdated, this, UIPersonalCloudBookShelfPage::OnCloudNodesUpdated)
    SubscribeMessageEvent(
        ModelTree::EventFileSystemChanged,
        *m_model,
        std::tr1::bind(
            std::tr1::mem_fn(&UIPersonalCloudBookShelfPage::OnFileListChanged),
            this,
            std::tr1::placeholders::_1)
    );
    SubscribeMessageEvent(
        ModelTree::EventQuotaRetrieved,
        *m_model,
        std::tr1::bind(
            std::tr1::mem_fn(&UIPersonalCloudBookShelfPage::onCloudQuotaRetrieved),
            this,
            std::tr1::placeholders::_1)
    );

    Init();
    UpdateWindowStatus(NONE);
}

UIPersonalCloudBookShelfPage::~UIPersonalCloudBookShelfPage()
{

}

bool UIPersonalCloudBookShelfPage::OnCloudListContentChanged(const EventArgs& args)
{
    if (UPLOADSTATUS & m_wndStatus)
    {
        UpdatePageNum();
        UpdateSpace();
    }
    return true;
}

bool UIPersonalCloudBookShelfPage::OnCloudNodesUpdated(const EventArgs& args)
{
    DebugPrintf(DLC_TRACE, "UIPersonalCloudBookShelfPage::OnCloudNodesUpdated START");
    cloud_files_ready_ = true;
    if (UPLOADSTATUS & m_wndStatus)
    {
        UpdatePageNum();
        UpdateSpace();
        UpdateWindowStatus((m_cloudModelView.GetItemNum() > 0) ? UPLOAD : NOUPLOAD);
    }
    return true;
}

bool UIPersonalCloudBookShelfPage::OnFileListChanged(const EventArgs& args)
{
    DebugPrintf(DLC_TRACE, "UIPersonalCloudBookShelfPage::OnFileListChanged START");
//     if (UPLOADSTATUS & m_wndStatus)
//     {
//         UpdatePageNum();
//         UpdateSpace();
//         UpdateWindowStatus((m_cloudModelView.GetItemNum() > 0) ? UPLOAD : NOUPLOAD);
//     }
    return true;
}

bool UIPersonalCloudBookShelfPage::onCloudQuotaRetrieved(const EventArgs& args)
{
    if (!cloud_quota_ready_)
    {
        UpdateWindowStatus(m_wndStatus);
    }
    UpdateSpace();
}

FetchDataResult UIPersonalCloudBookShelfPage::FetchPurchasedInfo()
{
    BookStoreInfoManager* manager = BookStoreInfoManager::GetInstance();
    if (manager && UIUtility::CheckAndReLogin())
    {
        return manager->FetchPurchasedList();
    }
    return FDR_FAILED;
}

FetchDataResult UIPersonalCloudBookShelfPage::FetchFreeBookListInfo()
{
    BookStoreInfoManager* manager = BookStoreInfoManager::GetInstance();
    if (manager)
    {
        return manager->FetchFreeBookList(0, 3);
    }
    return FDR_FAILED;
}

bool UIPersonalCloudBookShelfPage::OnMessageListBoxUpdate(const EventArgs& args)
{
    const DataUpdateArgs& msgArgs = (const DataUpdateArgs&)args;
    if (msgArgs.succeeded)
    {
        int totalItem = msgArgs.dataList.size();
        if (totalItem > 0)
        {
            m_myPurcharsedList.OnMessageListBoxUpdate(args);
            if (PURCHARSEDSTATUS & m_wndStatus)
            {
                m_fetchInfo.SetFetchStatus(FETCH_SUCCESS);
                UpdateWindowStatus(PURCHARSED);
            }
        }
        else
        {
            FetchFreeBookListInfo();
        }
    }
    else if (PURCHARSEDSTATUS & m_wndStatus)
    {
        m_fetchInfo.SetFetchStatus(FETCH_FAIL);
        UpdateWindowStatus(NONE);
    }
    return true;
}

bool UIPersonalCloudBookShelfPage::OnMessageFreeListUpdate(const EventArgs& args)
{
    const DataUpdateArgs& msgArgs = (const DataUpdateArgs&)args;
    if (msgArgs.succeeded)
    {
        int totalItem = msgArgs.total;
        if (totalItem > 0)
        {
            m_freeBookList.OnMessageListBoxUpdate(args);
        }
        if (PURCHARSEDSTATUS & m_wndStatus)
        {
            m_fetchInfo.SetFetchStatus(FETCH_SUCCESS);
            UpdateWindowStatus(NOPURCHARSED);
        }
    }
    else if (PURCHARSEDSTATUS & m_wndStatus)
    {
        m_fetchInfo.SetFetchStatus(FETCH_FAIL);
        UpdateWindowStatus(NONE);
    }
    return true;

}

bool UIPersonalCloudBookShelfPage::OnChildClick(UIWindow* child)
{
    if(child == &m_btnBack)
    {
        CPageNavigator::BackToPrePage();
        return true;
    }
    else if (child == &m_myPurchased)
    {
        return OnPurchasedClick();
    }
    else if (child == &m_myUpload)
    {
        return OnUploadClick();
    }
    else if (child == &m_processAll || child == &m_gotoUpload)
    {
        return OnProcessAllClick();
    }
    else if (child == &m_editAll)
    {
        return OnEditAllClick();
    }
    else if (child == &m_sortAll)
    {
        return OnSortClick();
    }
    return false;
}

string UIPersonalCloudBookShelfPage::GetSortString()
{
    DebugPrintf(DLC_UIHOMEPAGE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    string strSortText;
    if (PURCHARSEDSTATUS & m_wndStatus)
    {

    }
    else
    {
        switch(m_cloudModelView.getUIField())
        {
        case CREATE_TIME:
            strSortText = StringManager::GetPCSTRById(BOOKSTORE_PERSONALALLBOOKS);
            break;
        case NOT_ON_LOCAL:
            strSortText = StringManager::GetPCSTRById(BOOKSTORE_NON_DOWNLOADED);
            break;
        case NAME:
            strSortText = StringManager::GetPCSTRById(BY_BOOK_TITLE);
            break;
        default:
            break;
        }
    }
    return strSortText;
}

bool UIPersonalCloudBookShelfPage::OnSortClick()
{
    UITouchMenu menu(this);
    std::vector<std::string> buttonTitles;
    static string purchasedTitles[] = 
    {
        StringManager::GetPCSTRById(CLOUD_PURCHARSED_SORTBYBUYDATE),
        StringManager::GetPCSTRById(BOOKSTORE_NON_DOWNLOADED),
        StringManager::GetPCSTRById(CLOUD_PURCHARSED_SORTBYUPDATE),
        StringManager::GetPCSTRById(CLOUD_PURCHARSED_SORTBYNOREAD),
        StringManager::GetPCSTRById(CLOUD_PURCHARSED_SORTBYREADCOMPLETE)
    };
    static string uploadTitles[] = 
    {
        StringManager::GetPCSTRById(BOOKSTORE_PERSONALALLBOOKS),
        StringManager::GetPCSTRById(BOOKSTORE_NON_DOWNLOADED),
        StringManager::GetPCSTRById(BY_BOOK_TITLE)
    };
    static Field purchasedSortTypes[] = {CREATE_TIME, RATING, DESCRIPTION, TITLE, NODE_TYPE};
    static Field uploadSortTypes[] = {CREATE_TIME, NOT_ON_LOCAL, NAME};

    const bool purcharsed = (PURCHARSEDSTATUS & m_wndStatus);
    string* titles = purcharsed ? purchasedTitles : uploadTitles;
    Field *sortTypes =  purcharsed ? purchasedSortTypes : uploadSortTypes;
    int btnCount = purcharsed ? 5 : 3;

    int fontSize = GetWindowFontSize(FontSize24Index);

    int selected = 0;
    for (int i = 0; i < btnCount; ++i)
    {
        if (m_cloudModelView.getUIField() == sortTypes[i])
        {
            selected = i;
            break;
        }
    }

    for (int i = 0; i < btnCount; ++i)
    {
        menu.AppendButton(sortTypes[i], titles[i].c_str(), fontSize);
    }
    menu.SetCheckedButton(selected);

    dkSize menuSize = menu.GetMinSize();
    const int left = m_sortAll.GetAbsoluteX() + m_sortAll.GetWidth() - menuSize.x;
    const int top = m_sortAll.GetAbsoluteY() + m_sortAll.GetHeight();
    menu.MoveWindow(left,
        top,
        menuSize.x,
        menuSize.y);
    menu.Layout();
    if (IDOK == menu.DoModal())
    {
        Field sort_field = (Field)menu.GetCommandId();
        if (sort_field != m_cloudModelView.getUIField())
        {
            int newSelected = -1;
            for (int j = 0; j < btnCount; ++j)
            {
                if (sortTypes[j] == sort_field)
                {
                    newSelected = j;
                    break;
                }
            }
            if (-1 == newSelected)
            {
                return false;
            }
            cloud_files_ready_ = false;

            m_fetchInfo.SetFetchStatus(FETCHING);
            UpdateWindowStatus(NONE);
            int status_filter = NODE_NONE;
            if (sort_field == NOT_ON_LOCAL)
            {
                status_filter = NODE_CLOUD | NODE_NOT_ON_LOCAL;
            }
            m_cloudModelView.sort(sort_field, ASCENDING, status_filter);
        }
    }
    return true;
}

bool UIPersonalCloudBookShelfPage::OnEditAllClick()
{
    UIPersonalCloudDownloadPage* downloadBookPage = new UIPersonalCloudDownloadPage(m_model);
    if (downloadBookPage)
    {
        downloadBookPage->MoveWindow(0, 0, DeviceInfo::GetDisplayScreenWidth(), DeviceInfo::GetDisplayScreenHeight());
        CPageNavigator::Goto(downloadBookPage);
    }
    return false;
}

bool UIPersonalCloudBookShelfPage::OnProcessAllClick()
{
    DebugPrintf(DLC_TRACE, "UIPersonalCloudBookShelfPage::OnPurchasedClick START");
    if(!UIUtility::CheckAndReConnectWifi())
    {
        return false;
    }
    if (PURCHARSEDSTATUS & m_wndStatus)
    {
        bool isAllBook = true;
        UIMessageBox messagebox(m_pParent,
            StringManager::GetStringById(isAllBook ? SURE_UPDATE_ALL : SURE_DOWNLOAD_ALL),
            MB_OK | MB_CANCEL);
        if(MB_OK == messagebox.DoModal())
        {
            UIUtility::SetScreenTips(StringManager::GetStringById(BATCH_UPDATE_PENDING));
            BookDownloadController* manager = BookDownloadController::GetInstance();
            if(manager)
            {
                FetchDataResult ret = manager->BatchUpdateBooks(isAllBook ? (BS_NEEDSUPDATE) : (BS_UNDOWNLOAD | BS_DOWNLOADING | BS_FREE));
                switch (ret)
                {
                case FDR_PENDING:
                    // Top download button
                    m_processAll.SetText(StringManager::GetPCSTRById(BATCH_STOP_DOWNLOAD));
                    UIUtility::SetScreenTips(StringManager::GetStringById(isAllBook ? BATCH_UPDATE_PENDING : BATCH_DOWNLOAD_PENDING));
                    break;
                case FDR_SUCCESSED:
                    UIUtility::SetScreenTips(StringManager::GetStringById(isAllBook ? BATCH_UPDATE_SUCCEED : BATCH_DOWNLOAD_SUCCEED));
                    break;
                case FDR_EMPTY:
                    UIUtility::SetScreenTips(StringManager::GetStringById(isAllBook ? BATCH_UPDATE_EMPTY : BATCH_DOWNLOAD_EMPTY));
                    break;
                case FDR_FAILED:
                    UIUtility::SetScreenTips(StringManager::GetStringById(isAllBook ? BATCH_UPDATE_FAILED : BATCH_DOWNLOAD_FAILED));
                    break;
                default:
                    break;
                }
            }
        }
    }
    else 
    {
        UIPersonalCloudUploadPage* uploadBookPage = new UIPersonalCloudUploadPage(ModelTree::getModelTree(MODEL_LOCAL_FILESYSTEM));
        if (uploadBookPage)
        {
            uploadBookPage->MoveWindow(0, 0, DeviceInfo::GetDisplayScreenWidth(), DeviceInfo::GetDisplayScreenHeight());
            CPageNavigator::Goto(uploadBookPage);
        }
    }
    return false;
}

bool UIPersonalCloudBookShelfPage::OnPurchasedClick()
{
    DebugPrintf(DLC_TRACE, "UIPersonalCloudBookShelfPage::OnPurchasedClick START");
    m_fetchInfo.SetFetchStatus(FETCHING);
    UpdateWindowStatus(NONE);
    FetchPurchasedInfo();
    return false;
}

bool UIPersonalCloudBookShelfPage::OnUploadClick()
{
    DebugPrintf(DLC_TRACE, "UIPersonalCloudBookShelfPage::OnUploadClick START");
    if (cloud_files_ready_)
    {
        UpdatePageNum();
        UpdateSpace();
        UpdateWindowStatus((m_cloudModelView.GetItemNum() > 0) ? UPLOAD : NOUPLOAD);
    }
    else
    {
        m_fetchInfo.SetFetchStatus(FETCHING);
        UpdateWindowStatus(NONE);
    }
    return false;
}

void UIPersonalCloudBookShelfPage::Init()
{
    const int horizontalMargin = GetWindowMetrics(UIPixelValue30Index);
    const int btnFontSize = GetWindowFontSize(FontSize18Index);
    m_myPurchased.Initialize(IDSTATIC,StringManager::GetStringById(CLOUD_ALREADY_PUSHASED),btnFontSize);
    m_myUpload.Initialize(IDSTATIC,StringManager::GetStringById(CLOUD_ALREADY_UPLOADED),btnFontSize);

    m_btnGroup.AddButton(&m_myPurchased);
    m_btnGroup.AddButton(&m_myUpload);
    m_btnGroup.SetFocusedButton(&m_myUpload);
    m_btnGroup.SetBottomLinePixel(0);
    m_btnGroup.SetMinHeight(GetWindowMetrics(UIBottomBarHeightIndex));
    m_btnGroup.SetSplitLineHeight(GetWindowMetrics(UIBottomBarSplitLineHeightIndex));

    m_imagePurcharsedNull.SetImage(IMAGE_ICON_CLOUD_BIG);
    m_noPurchasedInfo.SetText(StringManager::GetPCSTRById(CLOUD_NOPUCHASED));
    m_gotoBookStore.Initialize(IDSTATIC,StringManager::GetStringById(CLOUD_GOTOBOOKSTORE),btnFontSize);
    m_gotoBookStore.SetBackgroundImage(IMAGE_TOUCH_WEIBO_SHARE);

    m_processAll.Initialize(IDSTATIC, "", btnFontSize);
    m_processAll.SetLeftMargin(0);
    m_processAll.SetInternalHorzSpacing(0);

    m_editAll.Initialize(IDSTATIC, StringManager::GetPCSTRById(TOUCH_EDIT_COMMENT), btnFontSize);
    m_editAll.SetLeftMargin(0);
    m_editAll.SetInternalHorzSpacing(0);

    m_sortAll.Initialize(IDSTATIC, "", btnFontSize);
    m_sortAll.SetIcon(ImageManager::GetImage(IMAGE_ARROW_DOWN), UIButton::ICON_RIGHT);
    m_sortAll.SetLeftMargin(0);
    m_sortAll.SetInternalHorzSpacing(0);
    const int border = GetWindowMetrics(UIPixelValue15Index);
    m_btnTopGroup.AddButton(&m_processAll, UISizerFlags().Expand().Center().Border(dkLEFT | dkRIGHT, border));
    m_btnTopGroup.AddButton(&m_editAll, UISizerFlags().Expand().Center().Border(dkLEFT | dkRIGHT, border));
    m_btnTopGroup.AddButton(&m_sortAll, UISizerFlags().Expand().Center().Border(dkLEFT | dkRIGHT, border));
    m_btnTopGroup.SetTopLinePixel(0);
    m_btnTopGroup.SetBottomLinePixel(0);
    m_btnTopGroup.SetSplitLineHeight(GetWindowMetrics(UIPixelValue20Index));

    m_txtTitle.SetText(StringManager::GetPCSTRById(PERSONAL_CLOUDBOOKSHELF));
    m_txtTitle.SetFontSize(GetWindowFontSize(FontSize26Index));

    m_myPurcharsedList.SetOnceAllItems(true);
    m_myPurcharsedList.SetItemEnableLongClick(true);

    m_freeBookList.SetShowScore(false);
    m_freeBookList.SetOnceAllItems(true);
    m_freeBookList.SetItemEnableLongClick(true);

    m_noUploadInfoSepHorz1.SetDirection(UISeperator::SD_HORZ);
    m_noUploadInfoSepHorz1.SetMinHeight(1);
    AppendChild(&m_noUploadInfoSepHorz1);
    m_noUploadInfoSepHorz2.SetDirection(UISeperator::SD_HORZ);
    m_noUploadInfoSepHorz2.SetMinHeight(4);
    AppendChild(&m_noUploadInfoSepHorz2);
    m_gotoUpload.Initialize(IDSTATIC,StringManager::GetStringById(CLOUD_UPLOADBOOK),btnFontSize);
    m_gotoUpload.SetBackgroundImage(IMAGE_TOUCH_WEIBO_SHARE);
    AppendChild(&m_gotoUpload);
    m_noUploadInfo1.SetText(StringManager::GetPCSTRById(CLOUD_NOUPLOAD1));
    m_noUploadInfo2.SetText(StringManager::GetPCSTRById(CLOUD_NOUPLOAD2));
    AppendChild(&m_noUploadInfo1);
    AppendChild(&m_noUploadInfo2);
    char space[32] = {0};
    double ns_used = 0, total = 0, available = 0;
    bool availableTooSmall = false;
    if (getCloudQuota(total, available, ns_used))
    {
        snprintf(space, DK_DIM(space), StringManager::GetPCSTRById(CLOUD_LEFTSPACE), available, total);
        availableTooSmall = (available <= total*0.1);
    }
    else
    {
        snprintf(space, DK_DIM(space), StringManager::GetPCSTRById(CLOUD_FETCHING));
    }
    m_noUploadSpace.SetText(space);
    AppendChild(&m_noUploadSpace);
    const int width = DeviceInfo::GetDisplayScreenWidth();
    const int itemHeight = GetWindowMetrics(availableTooSmall ? UIPixelValue90Index : UIPixelValue100Index);
    m_cloudModelView.SetItemWidth(width);
    m_cloudModelView.setRootNodeDisplayMode(BY_SORT);
//     m_cloudModelView.SetLeftMargin(horizontalMargin);
    m_cloudModelView.SetItemHeight(itemHeight);
    m_cloudModelView.SetItemSpacing(0);
    m_cloudModelView.SetItemsPerPage(6);
    m_cloudModelView.SetMinSize(width, itemHeight*6);
    AppendChild(&m_cloudModelView);

    m_spaceInfoSepHorz1.SetDirection(UISeperator::SD_HORZ);
    m_spaceInfoSepHorz1.SetMinHeight(1);
    AppendChild(&m_spaceInfoSepHorz1);
    m_spaceInfoSepHorz2.SetDirection(UISeperator::SD_HORZ);
    m_spaceInfoSepHorz2.SetMinHeight(4);
    AppendChild(&m_spaceInfoSepHorz2);
    m_spaceInfo.SetFontSize(GetWindowFontSize(FontSize16Index));
    m_spaceInfo.SetAlign(ALIGN_CENTER);
    m_spaceInfo.SetMaxWidth(DeviceInfo::GetDisplayScreenWidth() - (horizontalMargin << 1));
    AppendChild(&m_spaceInfo);

    m_fetchInfo.SetFetchStatus(FETCHING);

    m_imageUploadNull.SetImage(IMAGE_ICON_CLOUD_BIG);
    AppendChild(&m_imageUploadNull);

    m_txtSpace.SetFontSize(GetWindowFontSize(UIHomePageSortTypeButtonIndex));
    m_txtSpace.SetEnglishGothic();
    m_txtSpace.SetAlign(ALIGN_LEFT);
    m_txtSpace.SetLeftMargin(0);
    AppendChild(&m_txtSpace);

    m_txtPageNo.SetFontSize(GetWindowFontSize(UIHomePageSortTypeButtonIndex));
    m_txtPageNo.SetEnglishGothic();
    m_txtPageNo.SetAlign(ALIGN_RIGHT);
    m_txtPageNo.SetLeftMargin(0);
    AppendChild(&m_txtPageNo);

    AppendChild(&m_titleBar);
    AppendChild(&m_btnBack);
    AppendChild(&m_btnTopGroup);
    AppendChild(&m_txtTitle);
    AppendChild(&m_btnGroup);
    AppendChild(&m_imagePurcharsedNull);
    AppendChild(&m_noPurchasedInfo);
    AppendChild(&m_gotoBookStore);
    AppendChild(&m_freeBookList);
    AppendChild(&m_myPurcharsedList);
    AppendChild(&m_fetchInfo);

    UISizer* pMainSizer = new UIBoxSizer(dkVERTICAL);
    if (pMainSizer)
    {
        const int elemSpacing = GetWindowMetrics(UIPixelValue10Index);
        const int lineSpacing = GetWindowMetrics(UIPixelValue20Index);
        pMainSizer->Add(&m_titleBar);

        UISizer* pTitleSizer = new UIBoxSizer(dkHORIZONTAL);
        if (pTitleSizer)
        {
            pTitleSizer->Add(&m_btnBack, UISizerFlags().Expand().Align(dkALIGN_CENTER_VERTICAL));
            pTitleSizer->Add(&m_txtTitle, UISizerFlags().Expand().Align(dkALIGN_CENTER_VERTICAL).Border(dkLEFT, elemSpacing));
            pTitleSizer->AddStretchSpacer();
            pTitleSizer->Add(&m_btnTopGroup, UISizerFlags().Expand().Align(dkALIGN_CENTER_VERTICAL).Border(dkRIGHT, horizontalMargin - border));
            pTitleSizer->SetMinHeight(GetWindowMetrics(UIPixelValue61Index));

            pMainSizer->Add(pTitleSizer, UISizerFlags().Expand());
        }

        m_pPurcharsedNULLSizer = new UIBoxSizer(dkVERTICAL);
        if (m_pPurcharsedNULLSizer)
        {
            m_gotoBookStore.SetMinHeight(GetWindowMetrics(UIPixelValue60Index));
            m_pPurcharsedNULLSizer->Add(&m_imagePurcharsedNull, UISizerFlags().Align(dkALIGN_CENTER).Border(dkTOP, GetWindowMetrics(UIPixelValue61Index)).Border(dkBOTTOM, GetWindowMetrics(UIPixelValue50Index)));
            m_pPurcharsedNULLSizer->Add(&m_noPurchasedInfo, UISizerFlags().Align(dkALIGN_CENTER_HORIZONTAL));
            m_pPurcharsedNULLSizer->Add(&m_gotoBookStore, UISizerFlags().Expand().Align(dkALIGN_CENTER_HORIZONTAL).Border(dkTOP | dkBOTTOM, lineSpacing));
            m_pPurcharsedNULLSizer->Add(&m_freeBookList, UISizerFlags(1).Expand().Align(dkALIGN_CENTER_HORIZONTAL));
            pMainSizer->Add(m_pPurcharsedNULLSizer, UISizerFlags(1).Expand().Border(dkLEFT | dkRIGHT, horizontalMargin));
        }

        pMainSizer->Add(&m_myPurcharsedList, UISizerFlags(1).Expand().Border(dkLEFT | dkRIGHT, horizontalMargin));

        m_pUploadNULLSizer = new UIBoxSizer(dkVERTICAL);
        if (m_pUploadNULLSizer)
        {
            m_gotoUpload.SetMinHeight(GetWindowMetrics(UIPixelValue60Index));
            m_pUploadNULLSizer->Add(&m_imageUploadNull, UISizerFlags().Align(dkALIGN_CENTER).Border(dkTOP, GetWindowMetrics(UIPixelValue61Index)).Border(dkBOTTOM, GetWindowMetrics(UIPixelValue50Index)));
            m_pUploadNULLSizer->Add(&m_noUploadInfo1, UISizerFlags().Align(dkALIGN_CENTER_HORIZONTAL).Border(dkBOTTOM, lineSpacing));
            m_pUploadNULLSizer->Add(&m_noUploadInfo2, UISizerFlags().Align(dkALIGN_CENTER_HORIZONTAL));
            m_pUploadNULLSizer->Add(&m_noUploadInfoSepHorz1, UISizerFlags().Expand().Border(dkTOP, GetWindowMetrics(UIPixelValue45Index)));
            m_pUploadNULLSizer->Add(&m_noUploadSpace, UISizerFlags().Align(dkALIGN_CENTER_HORIZONTAL).Border(dkTOP | dkBOTTOM, GetWindowMetrics(UIPixelValue32Index)));
            m_pUploadNULLSizer->Add(&m_noUploadInfoSepHorz2, UISizerFlags().Expand().Border(dkBOTTOM, GetWindowMetrics(UIPixelValue45Index)));
            m_pUploadNULLSizer->Add(&m_gotoUpload, UISizerFlags().Expand().Align(dkALIGN_CENTER_HORIZONTAL).Border(dkLEFT | dkRIGHT, horizontalMargin));
            m_pUploadNULLSizer->AddStretchSpacer();
            pMainSizer->Add(m_pUploadNULLSizer, UISizerFlags(1).Expand());
        }
        m_pUploadSizer = new UIBoxSizer(dkVERTICAL);
        if (m_pUploadSizer)
        {
            m_pSpaceInfoSizer = new UIBoxSizer(dkVERTICAL);
            if (m_pSpaceInfoSizer)
            {
                m_pSpaceInfoSizer->Add(&m_spaceInfoSepHorz1, UISizerFlags().Expand());
                m_pSpaceInfoSizer->Add(&m_spaceInfo, UISizerFlags().Expand().Border(dkTOP | dkBOTTOM, GetWindowMetrics(UIPixelValue20Index)));
                m_pSpaceInfoSizer->Add(&m_spaceInfoSepHorz2, UISizerFlags().Expand());
                m_pUploadSizer->Add(m_pSpaceInfoSizer, UISizerFlags().Expand());
            }
            
            m_pUploadSizer->Add(&m_cloudModelView, UISizerFlags(1).Expand());
            UISizer* pPageInfoSizer = new UIBoxSizer(dkHORIZONTAL);
            if (pPageInfoSizer)
            {
                pPageInfoSizer->Add(&m_txtSpace, UISizerFlags().Expand().Border(dkLEFT, horizontalMargin));
                pPageInfoSizer->AddStretchSpacer();
                pPageInfoSizer->Add(&m_txtPageNo, UISizerFlags().Expand().Border(dkRIGHT, horizontalMargin));
                m_pUploadSizer->Add(pPageInfoSizer, UISizerFlags().Expand().Border(dkTOP | dkBOTTOM, GetWindowMetrics(UIPixelValue5Index)));
            }
            pMainSizer->Add(m_pUploadSizer, UISizerFlags(1).Expand());
        }
        pMainSizer->Add(&m_fetchInfo, UISizerFlags(1).Expand());
        pMainSizer->Add(&m_btnGroup, UISizerFlags().Expand());
        SetSizer(pMainSizer);
    }
}

void UIPersonalCloudBookShelfPage::UpdateWindowStatus(WNDSTATUS wndStatus, bool needLayout)
{
    m_wndStatus = wndStatus;
    double ns_used = 0, total = 0, available = 0;
    bool quota_retrieved = getCloudQuota(total, available, ns_used);
    bool availableTooSmall = quota_retrieved && (available <= total*0.1);
    bool noSpace = quota_retrieved && (available <= 0.01);

    if (m_pUploadNULLSizer)
    {
        m_pUploadNULLSizer->Show(wndStatus == NOUPLOAD);
    }
    if (m_pUploadSizer)
    {
        m_pUploadSizer->Show(wndStatus == UPLOAD);
    }
    if (m_pSpaceInfoSizer)
    {
        m_pSpaceInfoSizer->Show(availableTooSmall && wndStatus == UPLOAD);
    }
    if (m_pPurcharsedNULLSizer)
    {
        m_pPurcharsedNULLSizer->Show(wndStatus == NOPURCHARSED);
    }
    m_myPurcharsedList.SetVisible(wndStatus == PURCHARSED);
    m_fetchInfo.SetVisible(wndStatus == NONE);
    m_editAll.SetVisible(wndStatus == UPLOAD);

    if (wndStatus == PURCHARSED)
    {
        m_processAll.SetText(StringManager::GetPCSTRById(DOWNLOAD_ALL));
    }
    else if (wndStatus == UPLOAD)
    {
        m_processAll.SetText(StringManager::GetPCSTRById(CLOUD_UPLOADBOOK));
    }
    m_processAll.SetEnable(!noSpace);

    if (availableTooSmall)
    {
        m_spaceInfo.SetText(StringManager::GetPCSTRById(noSpace ? CLOUD_SPACE_IS_FULL : CLOUD_SPACE_TOO_SMALL));
    }
    const int itemHeight = GetWindowMetrics(availableTooSmall ? UIPixelValue90Index : UIPixelValue100Index);
    if (m_cloudModelView.GetItemHeight() != itemHeight)
    {
        m_cloudModelView.SetItemHeight(itemHeight);
        m_cloudModelView.Layout();
    }

    char sortString[32] = {0};
    snprintf(sortString, DK_DIM(sortString), "%s(%s)  ", StringManager::GetPCSTRById(SORT), GetSortString().c_str());
    m_sortAll.SetText(sortString);

    m_btnTopGroup.SetVisible(wndStatus == UPLOAD);
    if (needLayout)
    {
        Layout();
        UpdateWindow();
    }
}

void UIPersonalCloudBookShelfPage::UpdatePageNum()
{
    DebugPrintf(DLC_TRACE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    CHAR str[32] = {0};

    int iCurPage   = m_cloudModelView.GetCurPageIndex();
    int iTotalPage = m_cloudModelView.GetTotalPageCount();
    int itemNum = m_cloudModelView.GetItemNum();

    if(itemNum)
        snprintf(str, DK_DIM(str), "%d/%d %s", iCurPage + 1, iTotalPage,StringManager::GetPCSTRById(BOOK_PAGE));
    else
        snprintf(str, DK_DIM(str), "%s", StringManager::GetPCSTRById(BOOK_00_PAGE));
    m_txtPageNo.SetText(str);
    DebugPrintf(DLC_TRACE, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

bool UIPersonalCloudBookShelfPage::getCloudQuota(double& total_size, double& available, double& ns_used)
{
    if (!UIUtility::GetCloudSpaceInfo(total_size, available, ns_used))
    {
        cloud_quota_ready_ = false;
        CloudFileSystemTree* cloud_model = dynamic_cast<CloudFileSystemTree*>(m_model);
        if (cloud_model != 0)
        {
            cloud_model->FetchQuota();
        }
        return false;
    }
    cloud_quota_ready_ = true;
    return true;
}

void UIPersonalCloudBookShelfPage::UpdateSpace()
{
    DebugPrintf(DLC_TRACE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    char space[32] = {0};
    double ns_used = 0, total = 0, available = 0;
    
    if (getCloudQuota(total, available, ns_used))
    {
        snprintf(space, DK_DIM(space), StringManager::GetPCSTRById(CLOUD_LEFTSPACE), available, total);
        m_txtSpace.SetText(space);
        DebugPrintf(DLC_TRACE, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    }
}

void UIPersonalCloudBookShelfPage::OnEnter()
{
    DebugPrintf(DLC_UIHOMEPAGE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    UIPage::OnEnter();

    if (cloud_files_ready_)
    {
        m_cloudModelView.updateModelByContext(m_model);
        m_cloudModelView.InitListItem();
        UpdateWindow();
    }
    else
    {
        m_cloudModelView.sort(CREATE_TIME, ASCENDING, NODE_NONE);
    }
}

bool UIPersonalCloudBookShelfPage::onAccountEvent(const EventArgs& args)
{
    const AccountEventArgs& account_args = dynamic_cast<const AccountEventArgs&>(args);
    CAccountManager* account_manager = CAccountManager::GetInstance();
	if(account_manager)
	{
		CAccountManager::LoginStatus login_status = account_manager->GetLoginStatus();
		switch(login_status)
		{
			case CAccountManager::LoggedIn_XiaoMi:
			case CAccountManager::LoggedIn_DuoKan:
				{
				}
				break;
            case CAccountManager::NotLoggedIn:
                {
                    // TODO. Display login page
                    CPageNavigator::BackToEntryPageOnTypeAsync(PAT_Personal);
                }
                break;
            default:
                break;
		}
	}
    return true;
}
