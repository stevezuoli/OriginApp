#ifndef __COMMONUI_UINODEVIEW_CLOUD_FOLDER_H__
#define __COMMONUI_UINODEVIEW_CLOUD_FOLDER_H__

#include "CommonUI/IUINodeView.h"
#include "GUI/UIImage.h"
#include "GUI/UIComplexButton.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UIProgressBar.h"
#include "CommonUI/UICoverViewItem.h"
#include "CommonUI/UIBookListProgressBar.h"

class UINodeViewCloudFolder : public IUINodeView
{
public:
    UINodeViewCloudFolder(UIModelView* pParent,
                          ModelTree* model_tree, 
                          BookListUsage usage,
                          int iconWidth,
                          int iconHeight);

    virtual ~UINodeViewCloudFolder();
    virtual LPCSTR GetClassName() const
    {
        return "UINodeViewCloudFolder";
    }

    virtual UISizer* getListViewSizer();
    virtual void SetModelDisplayMode(ModelDisplayMode mode);
    
    virtual void update();
    virtual void execute(unsigned long command_id);
    virtual void handleClicked();
    virtual std::vector<int> getSupportedNodeTypes();
    virtual bool isSelectMode() const;
private:
    bool openFolder();

private:
    UIImage m_imgItem;
    UIImage m_imgDirArrow;
};

#endif

