#ifndef __COMMONUI_UISELECTPAGE_H__
#define __COMMONUI_UISELECTPAGE_H__

#include "TouchAppUI/UIModelView.h"
#include "GUI/UIPage.h"
#include "GUI/UISeperator.h"
#include "Model/model_tree.h"

using namespace dk::document_model;

class UISelectPage: public UIPage
{
public:
    UISelectPage(BookListUsage usage, ModelTree* model_tree);
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

    virtual bool OnChildSelectChanged(const EventArgs& args);
    virtual bool OnNodesUpdated(const EventArgs& args);
    virtual bool OnListTurnPage(const EventArgs&);

protected:
    UISizer* m_pTopSizer;
    UISizer* m_pModelTreeSizer;
    UISizer* m_pBottomSizer;
    ModelTree* m_model;
    UIModelView m_modelView;
};
#endif
