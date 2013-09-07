#ifndef __PERSONUI_UIPERSONALCLOUDUPLOADPAGE_H__
#define __PERSONUI_UIPERSONALCLOUDUPLOADPAGE_H__

#include "TouchAppUI/UIModelView.h"
#include "CommonUI/UISelectPage.h"
#include "GUI/UISeperator.h"
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
    virtual UISizer* CreateTopSizer();
    virtual UISizer* CreateModelTreeSizer();
    virtual UISizer* CreateBottomSizer();

    virtual bool OnChildSelectChanged(const EventArgs& args);
    virtual bool OnNodesUpdated(const EventArgs& args);
    virtual bool OnListTurnPage(const EventArgs&);

protected:
    virtual bool OnChildClick(UIWindow* child);
private:
    void UpdateModelView(bool layout);
    bool OnBackClick();
    bool OnSelectedAllClick();
    bool OnExpandModeClick();
    bool OnImageModeClick();
    bool OnUploadClicked();

private:
    UITouchBackButton m_btnBack;
    UITextSingleLine m_txtTitle;
    UITouchComplexButton m_imageMode;
    UITouchComplexButton m_expandMode;
    UITouchComplexButton m_selectAll;
    UITouchComplexButton m_btnUpload;
    UIButtonGroup m_btnGroup;
    UITextSingleLine m_txtTotalBook;
    UITextSingleLine m_txtPageNo;
    UISeperator m_sepHorz;
    UISeperator m_sepVert1;
    UISeperator m_sepVert2;
};
#endif
