////////////////////////////////////////////////////////////////////////
// UINodeViewOfLocalDir.h
// Contact: liujt
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __COMMONUI_UINODEVIEWOFLOCALDIR_H__
#define __COMMONUI_UINODEVIEWOFLOCALDIR_H__

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


class UINodeViewOfLocalDir : public IUINodeView
{
public:
    UINodeViewOfLocalDir(UICompoundListBox* pParent,
        ModelTree* model_tree, 
        BookListUsage usage,
        int iconWidth,
        int iconHeight);

    virtual ~UINodeViewOfLocalDir();

    virtual LPCSTR GetClassName() const
    {
        return "UINodeViewOfLocalDir";
    }

    virtual UISizer* CreateListViewSizer();
    virtual void updateByNode();
    virtual void SetModelDisplayMode(ModelDisplayMode mode);
    
private:
    UIImage            m_imgItem;
    UIImage            m_imgDirArrow;
};

#endif

