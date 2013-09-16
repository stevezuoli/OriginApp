#ifndef __PERSONUI_UIPERSONALCLOUDBOOKSHELFPAGE_H__
#define __PERSONUI_UIPERSONALCLOUDBOOKSHELFPAGE_H__

#include "TouchAppUI/UIModelView.h"
#include "GUI/UIPage.h"
#include "GUI/UISeperator.h"
#include "Model/model_tree.h"
#include "CommonUI/UIButtonGroupWithFocusedLine.h"
#include "BookStoreUI/UIBookStoreFetchInfo.h"
#include "BookStoreUI/UIBookStoreListBoxWithPageNum.h"
#include "GUI/UISeperator.h"

using namespace dk::document_model;

class UIPersonalCloudBookShelfPage: public UIPage
{
public:
    UIPersonalCloudBookShelfPage();
    virtual ~UIPersonalCloudBookShelfPage();
    virtual const char* GetClassName() const
    {
        return "UIPersonalCloudBookShelfPage";
    }

    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);

    virtual void OnEnter();
private:
    enum WNDSTATUS
    {
        NONESTATUS = 0x1,
        SOMESTATUS = 0x2,
        PURCHARSEDSTATUS = 0x4,
        UPLOADSTATUS = 0x8,
        NOPURCHARSED = NONESTATUS | PURCHARSEDSTATUS,
        PURCHARSED = SOMESTATUS | PURCHARSEDSTATUS,
        NOUPLOAD = NONESTATUS | UPLOADSTATUS,
        UPLOAD = SOMESTATUS | UPLOADSTATUS,
    };
    FetchDataResult FetchPurchasedInfo();
    FetchDataResult FetchFreeBookListInfo();
    bool OnMessageListBoxUpdate(const EventArgs& args);
    bool OnMessageFreeListUpdate(const EventArgs& args);
    bool OnCloudNodesUpdated(const EventArgs& args);
    bool OnCloudListContentChanged(const EventArgs& args);
    bool OnFileListChanged(const EventArgs& args);
    bool OnChildClick(UIWindow* child);
    bool onCloudQuotaRetrieved(const EventArgs& args);
    bool onAccountEvent(const EventArgs& args);

    bool OnPurchasedClick();
    bool OnUploadClick();
    bool OnProcessAllClick();
    bool OnEditAllClick();
    bool OnSortClick();
    bool OnGotoBookStoreClick();

    void Init();
    void UpdateWindowStatus(WNDSTATUS wndStatus, bool needLayout = true);
    void UpdatePageNum();
    void UpdateSpace();
//     void UpdateUploadWnds(WNDSTATUS wndStatus);
    std::string GetSortString();

    bool getCloudQuota(double& total_size, double& available, double& ns_used);

private:
    UITouchBackButton m_btnBack;
    UITextSingleLine m_txtTitle;
    UITouchComplexButton m_processAll;
    UITouchComplexButton m_editAll;
    UITouchComplexButton m_sortAll;
    UITouchComplexButton m_myPurchased;
    UITouchComplexButton m_myUpload;
    UIButtonGroupWithFocusedLine m_btnGroup;
    UIButtonGroup m_btnTopGroup;

    UIBookStoreListBoxWithPageNum m_myPurcharsedList;
    UIImage m_imagePurcharsedNull;
    UITextSingleLine m_noPurchasedInfo;
    UITouchComplexButton m_gotoBookStore;
    UIBookStoreListBoxPanel m_freeBookList;

    ModelTree*    m_model;
    UIModelView   m_cloudModelView;
    UIImage m_imageUploadNull;
    UITextSingleLine m_noUploadInfo1;
    UITextSingleLine m_noUploadInfo2;
    UISeperator m_noUploadInfoSepHorz1;
    UISeperator m_noUploadInfoSepHorz2;
    UITextSingleLine m_noUploadSpace;
    UITouchComplexButton m_gotoUpload;
    UITextSingleLine m_txtSpace;
    UITextSingleLine m_txtPageNo;
    UIText m_spaceInfo;
    UISeperator m_spaceInfoSepHorz1;
    UISeperator m_spaceInfoSepHorz2;

    UIBookStoreFetchInfo m_fetchInfo;
    WNDSTATUS m_wndStatus;
    UISizer* m_pPurcharsedNULLSizer;
    UISizer* m_pUploadNULLSizer;
    UISizer* m_pUploadSizer;
    UISizer* m_pSpaceInfoSizer;

    bool cloud_files_ready_;
    bool cloud_quota_ready_;
};

#endif
