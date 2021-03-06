////////////////////////////////////////////////////////////////////////
// UIModelView.h
// Contact: liujt
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UI_MODEL_VIEW_H__
#define __UI_MODEL_VIEW_H__

#include <vector>

#include "GUI/UICompoundListBox.h"
#include "CommonUI/IUINodeView.h"
#include "Model/model_tree.h"

using namespace dk::document_model;

class ChildSelectChangedArgs: public EventArgs
{
public:
    ChildSelectChangedArgs(size_t _selected)
        : selected(_selected)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new ChildSelectChangedArgs(*this);
    }
    size_t selected;
};

class NodesUpdatedArgs: public EventArgs
{
public:
    NodesUpdatedArgs()
        : children_num(0)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new NodesUpdatedArgs(*this);
    }
    size_t children_num;
};

class ModelViewContext
{
public:
    ModelViewContext()
        : layout_mode_(BLM_LIST)
        , display_mode_(BY_FOLDER)
        , status_filter_(NODE_NONE)
        , sort_field_(RECENTLY_ADD)
        , sort_order_(DESCENDING)
        , book_usage_(BLU_BROWSE) {}
    ~ModelViewContext() {}

    bool isSelectMode() { return book_usage_ & (BLU_SELECT | BLU_SELECT_EXPAND); }
public:
    ModelDisplayMode layout_mode_;
    DKDisplayMode display_mode_;
    int status_filter_;
    Field sort_field_;
    SortOrder sort_order_;
    BookListUsage book_usage_;
    string keyword_;
};

class UIModelView : public UICompoundListBox
{
public:
    static const char* EventChildSelectChanged;
    static const char* EventNodesUpdated;

public:
    UIModelView(BookListUsage usage, ModelTree * model);
    virtual ~UIModelView();
    virtual LPCSTR GetClassName() const
    {
        return "UIModelView";
    }
    virtual BOOL OnKeyPress(INT32 iKeyCode);
    virtual void OnCommand(DWORD _dwCmdId, UIWindow* _pSender, DWORD _dwParam);

    ModelTree* model() { return model_tree_; }

    virtual bool DoHandleListTurnPage(bool fKeyUp);
    virtual BOOL HandleLongTap(INT32 selectedItem);

    void updateModelByContext(ModelTree* model_tree);
    bool UpdateListItem();
    virtual void InitListItem();
    void initNodeViews(bool rescan);
    
    bool BackToUpperFolder();
    bool cdPath(const string& path);
    bool cdRoot();

    DKDisplayMode rootNodeDisplayMode();
    void setRootNodeDisplayMode(DKDisplayMode mode);
    void setStatusFilter(int status_filter);

    Field sortField();
    void sort(Field by, SortOrder order, int status_filter);
    const string& searchKeyword() const { return view_ctx_.keyword_; }
    void setSearchKeyword(const string& keyword);

    void setBookUsage(BookListUsage usage);

    int GetTotalPageCount() const { return page_count_; }
    int GetCurPageIndex() const { return current_page_; }
    void SetCurPageIndex(int page) { current_page_ = page; }
    int GetItemNum() const { return item_count_; }

    void SetModelDisplayMode(ModelDisplayMode layout_mode);
    std::vector<std::string> GetSelectedBookIds();
    size_t GetSelectedBookCount();

    NodeType currentNodeType();
    string   currentNodePath();

    virtual BOOL  SetItemHeight(INT32 _iItemHeight);
    int GetItemsPerPage() const;
    void SetItemsPerPage(int itemPerPage) { items_per_page_ = itemPerPage; }

    // event handlers
    bool onNodeSelected(const EventArgs& args);
    bool onNodeOpenBook(const EventArgs& args);

private:
    bool ShouldShowContextMenuForItem(NodePtr node) const;
    bool PageDown();
    bool PageUp();
    void ClearListItem();

    IUINodeView* selectedNodeView();
    IUINodeView* nodeView(const string& node_path, int& index);

    // added functions
    NodePtr firstVisibleNode();
    NodePtr firstSelectedNode();
    NodePtr selectedNode(const int selected_index);

    NodePtrs childrenNodes(bool scan, bool update, RetrieveChildrenResult& result);
    bool gotoUp();
    bool gotoNode(NodePtr node);

protected:
    virtual bool OnItemClick(INT32 iSelectedItem);

private:
    bool UpdateListItemForList();
    bool UpdateListItemForIcon();
    bool ShouldShowContextMenuForNode(NodePtr node) const;

private:
    bool onChildrenNodesReady(const EventArgs& args);
    bool onCurrentNodeChanged(const EventArgs& args);

private:
    BOOL m_bIsDisposed;

    int rows_;
    int columns_;
    int items_per_page_;
    int first_visible_;     ///< The first visible child node position.
    int current_page_;      ///< Current page number.
    int page_count_;        ///< Total page number.
    int item_count_;        ///< Number of total items.

    //BookListUsage m_usage;
    ModelViewContext view_ctx_;
    std::vector<UISizer*> m_rowSizers;

    UISizer* m_iconSizer;
    UISizer* m_listSizer;

    std::vector<IUINodeView*> node_views_;
    ModelTree * model_tree_;
    bool m_needClearItem;
};

#endif

