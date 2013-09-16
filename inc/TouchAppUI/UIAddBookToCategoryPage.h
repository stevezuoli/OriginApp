#ifndef __TOUCHAPPUI_UIADDBOOKTOCATEGORYPAGE_H__
#define __TOUCHAPPUI_UIADDBOOKTOCATEGORYPAGE_H__

#include "TouchAppUI/UIModelView.h"
#include "CommonUI/UISelectPage.h"
#include "GUI/UISeperator.h"
#include "Model/model_tree.h"

using namespace dk::document_model;

class UIAddBookToCategoryPage: public UISelectPage
{
public:
    UIAddBookToCategoryPage(const char* category, ModelTree* model_tree);
    virtual const char* GetClassName() const
    {
        return "UIAddBookToCategoryPage";
    }
    virtual UISizer* CreateTopSizer();
    virtual UISizer* CreateModelTreeSizer();
    virtual UISizer* CreateBottomSizer();

    virtual bool OnChildClick(UIWindow* child);
    virtual void UpdateModelView(bool layout);

    virtual bool OnChildSelectChanged(const EventArgs& args);
    virtual bool OnNodesUpdated(const EventArgs& args);
    virtual bool OnListTurnPage(const EventArgs&);

    virtual void OnEnter();

private:
    bool OnSaveClicked();
    void UpdateSelectedBooks(size_t bookCount, bool updateWindow = true);

private:
    UITextSingleLine m_txtSelected;
    UITouchComplexButton m_btnSave;
    UITextSingleLine m_txtTotalBook;
    UITextSingleLine m_txtPageNo;
    std::string m_category;
    UISeperator m_sepHorz;
    UISeperator m_sepVert1;
    UISeperator m_sepVert2;
};
#endif
