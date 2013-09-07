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
    UINodeViewOfLocalFile(UICompoundListBox* pParent,
               ModelTree* model_tree,
               BookListUsage usage,
               int iconWidth,
               int iconHeight);

    ~UINodeViewOfLocalFile();

    virtual LPCSTR GetClassName() const
    {
        return "UINodeViewOfLocalFile";
    }

    virtual UISizer* CreateListViewSizer();
    virtual void updateByNode();

private:
    static int PageCountToProgressBarDotCount(int pageCount);

private:
    UIImage            m_imgNewBook;
    UIImage            m_imgItem;
    UITextSingleLine   m_txtAuthorLabel;
    UITextSingleLine   m_txtTrialLabel;
    
    UISizerItem* m_spaceBetweenAuthorAndArrow;
};

#endif

