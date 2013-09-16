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
using namespace std;

class NodeSelectedArgs: public EventArgs
{
public:
    NodeSelectedArgs()
        : selected(false)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new NodeSelectedArgs(*this);
    }
    bool   selected;
    string node_path;
};

class NodeOpenBookArgs: public EventArgs
{
public:
    NodeOpenBookArgs()
    {
    }
    virtual EventArgs* Clone() const
    {
        return new NodeOpenBookArgs(*this);
    }
    string node_path;
};

class UISizer;
class UISizerItem;
class UIModelView;

class IUINodeView : public UIListItemBase
{
public:
    static const char* EventNodeSelected;
    static const char* EventNodeOpenBook;

public:
    IUINodeView(UIModelView* pParent,
                ModelTree* model_tree,
                BookListUsage usage,
                int iconWidth,
                int iconHeight);

    virtual ~IUINodeView();

    virtual void MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight);
    virtual void SetFocus(bool bIsFocus);
    virtual void SetModelDisplayMode(ModelDisplayMode mode);

    virtual void SetNode(NodePtr node);
    virtual void SetHighLight(bool isHighLight);

    virtual bool IsSelected() const;
    virtual void SetSelected(bool selected);
    virtual void SetIsLoading(bool isLoading);
    virtual void update();

    virtual UISizer* getListViewSizer();
    virtual UISizer* getCoverViewSizer(); 

    virtual void execute(unsigned long command_id);
    virtual void handleClicked();

    virtual std::vector<int> getSupportedNodeTypes();
    bool support(NodePtr node);

    NodePtr data() { return m_node; }
    void updateSelectedStatus();

    static IUINodeView* createNodeView(UIModelView* pParent,
                                       ModelTree* model_tree,
                                       NodePtr node,
                                       BookListUsage usage,
                                       int iconWidth,
                                       int iconHeight);
protected:
    bool onChildrenReady(const EventArgs& args);
    bool onStatusUpdated(const EventArgs& args);
    bool onLoadingFinished(const EventArgs& args);

    virtual bool onChildrenReadyAction(const EventArgs& args);
    virtual bool onStatusUpdatedAction(const EventArgs& args);
    virtual bool onLoadingFinishedAction(const EventArgs& args);

    virtual bool isSelectMode() const;

protected:
    BookListUsage m_usage;
    // data
    NodePtr m_node;

    // display mode
    ModelDisplayMode m_modelDisplayMode;

    // UI elements
    UISizer* m_coverViewSizer;
    UISizer* m_listViewSizer;
    UICoverViewItem m_cover;
    UITextSingleLine m_txtItemNameLabel;
    UIBookListProgressBar m_readingProgressBar;
    UIImage m_imgSelect;
    UIImage m_imgItem;
};

#endif

