////////////////////////////////////////////////////////////////////////
// UINodeView.h
// Contact: liujt
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __COMMONUI_UINODEVIEW_H__
#define __COMMONUI_UINODEVIEW_H__

#include "GUI/UIContainer.h"
#include "GUI/UIImage.h"
#include "GUI/UIComplexButton.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UIProgressBar.h"
#include "GUI/UIListItemBase.h"
#include "CommonUI/UICoverViewItem.h"
#include "CommonUI/UIBookListProgressBar.h"

#include "Model/node.h"
#include "Model/model_tree.h"

using namespace dk::document_model;

class UISizer;
class UISizerItem;
class CDKFile;
class IUINodeView;

IUINodeView* CreateNodeView(UICompoundListBox* pParent, ModelTree* model_tree, NodePtr node, BookListUsage usage, int iconWidth, int iconHeight);

class IUINodeView : public UIListItemBase
{
public:
    IUINodeView(UICompoundListBox* pParent,
               ModelTree* model_tree,
               BookListUsage usage,
               int iconWidth,
               int iconHeight);

    virtual ~IUINodeView();

    virtual void MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight);
    virtual void SetFocus(bool bIsFocus);

    virtual void SetSelectMode(bool showSelect);
    virtual void SetModelDisplayMode(ModelDisplayMode mode);

    virtual void SetNode(NodePtr node);
    virtual void SetHighLight(bool isHighLight);

    virtual bool IsSelected() const;
    virtual void SetSelected(bool selected);
    virtual void SetIsLoading(bool isLoading);
    virtual void updateByNode();
    virtual bool onChildrenReady(const EventArgs& args){ return true; }
    virtual bool onStatusUpdated(const EventArgs& args);
    virtual UISizer* CreateListViewSizer();
    virtual UISizer* CreateCoverViewSizer(); 

protected:
    NodePtr m_node;
    bool m_selectMode;
    ModelDisplayMode m_modelDisplayMode;
    UISizer* m_coverViewSizer;
    UISizer* m_listViewSizer;
    UICoverViewItem m_cover;
    UITextSingleLine   m_txtItemNameLabel;
    UIBookListProgressBar m_readingProgressBar;
    UIImage m_imgSelect;
};

#endif

