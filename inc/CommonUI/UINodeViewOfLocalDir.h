////////////////////////////////////////////////////////////////////////
// UINodeViewOfLocalDir.h
// Contact: liujt
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __COMMONUI_UINODEVIEWOFLOCALDIR_H__
#define __COMMONUI_UINODEVIEWOFLOCALDIR_H__

#include "GUI/UIImage.h"
#include "GUI/UIComplexButton.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UIProgressBar.h"
#include "CommonUI/IUINodeView.h"
#include "CommonUI/UICoverViewItem.h"
#include "CommonUI/UIBookListProgressBar.h"

#include "Model/node.h"
#include "Model/model_tree.h"

using namespace dk::document_model;

class UISizer;
class UISizerItem;
class CDKFile;


class UINodeViewOfLocalDir : public IUINodeView
{
public:
    UINodeViewOfLocalDir(UIModelView* pParent,
        ModelTree* model_tree, 
        BookListUsage usage,
        int iconWidth,
        int iconHeight);

    virtual ~UINodeViewOfLocalDir();

    virtual LPCSTR GetClassName() const
    {
        return "UINodeViewOfLocalDir";
    }

    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
    virtual UISizer* getListViewSizer();
    virtual void update();
    virtual void SetModelDisplayMode(ModelDisplayMode mode);
    
    virtual void execute(unsigned long command_id);
    virtual void handleClicked();
    virtual std::vector<int> getSupportedNodeTypes();
    virtual bool isSelectMode() const;
protected:
    virtual bool onStatusUpdatedAction(const EventArgs& args);
    virtual bool onLoadingFinishedAction(const EventArgs& args);

private:
    bool renameFolder();
    bool renameCategory();
    bool deleteFolder();
    bool deleteCategory();
    bool addFilesToCategory();
    bool openFolder();

private:
    UIImage            m_imgDirArrow;
};

#endif

