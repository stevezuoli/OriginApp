#ifndef __PERSONUI_UIPERSONALCLOUDUPLOADPAGE_H__
#define __PERSONUI_UIPERSONALCLOUDUPLOADPAGE_H__

#include "TouchAppUI/UIModelView.h"
#include "CommonUI/UISelectPage.h"
#include "Model/model_tree.h"

using namespace dk::document_model;

class UIPersonalCloudUploadPage: public UISelectPage
{
public:
    UIPersonalCloudUploadPage(ModelTree* model_tree);
    virtual const char* GetClassName() const
    {
        return "UIPersonalCloudUploadPage";
    }

    virtual UISizer* CreateModelTreeSizer();
    virtual UISizer* CreateBottomSizer();
    virtual void InitModelView();

    virtual bool OnChildClick(UIWindow* child);
    virtual void UpdateModelView(bool layout);

    virtual bool OnChildSelectChanged(const EventArgs& args);
    virtual bool OnNodesUpdated(const EventArgs& args);
    virtual bool OnListTurnPage(const EventArgs&);

    virtual void OnEnter();

private:
    bool OnUploadClicked();
    bool onNodeSelectStatusUpdated(const EventArgs&);
private:
    UITouchComplexButton m_btnUpload;
    UITextSingleLine m_txtSpace;
    UITextSingleLine m_txtPageNo;
    UISeperator m_sepHorz;
    UISizer* m_pageInfoSizer;
};
#endif
