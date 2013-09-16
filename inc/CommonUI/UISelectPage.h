#ifndef __COMMONUI_UISELECTPAGE_H__
#define __COMMONUI_UISELECTPAGE_H__

#include "TouchAppUI/UIModelView.h"
#include "GUI/UIPage.h"
#include "GUI/UISeperator.h"
#include "Model/model_tree.h"
#include "GUI/UITouchComplexButton.h"

using namespace dk::document_model;

class UISelectPage: public UIPage
{
public:
    UISelectPage(BookListUsage usage, ModelTree* model_tree);
    virtual ~UISelectPage() {}
    virtual const char* GetClassName() const
    {
        return "UISelectPage";
    }
    virtual void MoveWindow(
            int left,
            int top,
            int width,
            int height);
    virtual UISizer* CreateTopSizer();
    virtual UISizer* CreateModelTreeSizer();
    virtual UISizer* CreateBottomSizer();
    virtual void InitModelView();

    virtual bool OnChildClick(UIWindow* child);
    virtual bool OnBackClick();
    virtual bool OnSelectedAllClick();
    virtual bool OnExpandModeClick();
    virtual bool OnImageModeClick();
    virtual void UpdateModelView(bool layout);

    virtual bool OnChildSelectChanged(const EventArgs& args);
    virtual bool OnNodesUpdated(const EventArgs& args);
    virtual bool OnListTurnPage(const EventArgs&);

    virtual void OnEnter();

protected:
    UISizer* m_pTopSizer;
    UISizer* m_pModelTreeSizer;
    UISizer* m_pBottomSizer;
    ModelTree* m_model;
    UIModelView m_modelView;

    UITouchBackButton m_btnBack;
    UITextSingleLine m_txtTitle;
    UITouchComplexButton m_imageMode;
    UITouchComplexButton m_expandMode;
    UITouchComplexButton m_selectAll;
    UIButtonGroup m_btnGroup;
};
#endif
