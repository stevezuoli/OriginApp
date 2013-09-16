#ifndef __COMMONUI_UIPERSONALCLOUDDOWNLOADPAGE_H__
#define __COMMONUI_UIPERSONALCLOUDDOWNLOADPAGE_H__

#include "CommonUI/UISelectPage.h"
#include "GUI/UIButtonGroup.h"

using namespace dk::document_model;

class UIPersonalCloudDownloadPage: public UISelectPage
{
public:
    UIPersonalCloudDownloadPage(ModelTree* model_tree);
    virtual ~UIPersonalCloudDownloadPage();
    virtual const char* GetClassName() const
    {
        return "UIPersonalCloudDownloadPage";
    }

    virtual UISizer* CreateTopSizer();
    virtual UISizer* CreateModelTreeSizer();
    virtual UISizer* CreateBottomSizer();
    virtual void InitModelView();

    virtual bool OnChildClick(UIWindow* child);
    virtual void UpdateModelView(bool layout);

    virtual bool OnChildSelectChanged(const EventArgs& args);
    virtual bool OnNodesUpdated(const EventArgs& args);
    virtual bool OnListTurnPage(const EventArgs&);

private:
    bool OnDeleteClicked();
    bool OnDownloadClicked();

private:
    UITouchComplexButton m_delete;
    UITouchComplexButton m_download;
    UIButtonGroup m_btnBottomGroup;
    UITextSingleLine m_txtSpace;
    UITextSingleLine m_txtPageNo;
};
#endif
