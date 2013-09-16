////////////////////////////////////////////////////////////////////////
// UINodeViewOfLocalFile.h
// Contact: liujt
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __COMMONUI_UINODEVIEWOFLOCALFILE_H__
#define __COMMONUI_UINODEVIEWOFLOCALFILE_H__

#include "TouchAppUI/UINodeView.h"
#include "GUI/UIImage.h"
#include "GUI/UIComplexButton.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UITouchComplexButton.h"
#include "GUI/UIProgressBar.h"
#include "CommonUI/UICoverViewItem.h"
#include "CommonUI/UIBookListProgressBar.h"

#include "Model/node.h"
#include "Model/model_tree.h"

using namespace dk::document_model;

class UISizer;
class UISizerItem;
class CDKFile;

class UINodeViewOfLocalFile : public IUINodeView
{
public:
    UINodeViewOfLocalFile(UIModelView* pParent,
               ModelTree* model_tree,
               BookListUsage usage,
               int iconWidth,
               int iconHeight);

    ~UINodeViewOfLocalFile();

    virtual LPCSTR GetClassName() const
    {
        return "UINodeViewOfLocalFile";
    }

    virtual UISizer* getListViewSizer();
    virtual void update();
    virtual void execute(unsigned long command_id);
    virtual void handleClicked();
    virtual void SetModelDisplayMode(ModelDisplayMode mode);
    virtual std::vector<int> getSupportedNodeTypes();

protected:
    virtual bool onLoadingFinishedAction(const EventArgs& args);

private:
    bool deleteFile();
    bool openFile();
    bool deleteFromCategory();
    bool shareToWeibo();
    bool uploadFile();

    void updateWndStauts();
    void updateWndVisible();
    void Init();
    void updateItemNameLayout();

    static int PageCountToProgressBarDotCount(int pageCount);

private:
    UIImage            m_imgNewBook;
    UITextSingleLine   m_txtAuthorLabel;
    UITextSingleLine   m_txtTrialLabel;
    UITextSingleLine   m_txtSizeLabel;
    UITouchComplexButton m_btnUploadOrRead;
};

#endif

