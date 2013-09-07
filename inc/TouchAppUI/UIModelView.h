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

    bool BackToUpperFolder();
    bool UpdateListItem();
    void InitListItem();
    bool cdPath(const string& path);
    bool cdRoot();
    DKDisplayMode rootNodeDisplayMode();
    void setRootNodeDisplayMode(DKDisplayMode mode);
    void setStatusFilter(int status_filter);

    int GetTotalPageCount() const { return page_count_; }
    int GetCurPageIndex() const { return current_page_; }
    void SetCurPageIndex(int page) { current_page_ = page; }
    int GetItemNum() const { return item_count_; }

    void SetModelDisplayMode(ModelDisplayMode ModelDisplayMode);
    std::vector<std::string> GetSelectedBookIds();
    size_t GetSelectedBookCount();

    NodeType currentNodeType();
    string   currentNodePath();

private:
    void DeleteFileOnUI(NodePtr node);
    void DeleteDirectoryOnUI(NodePtr node);
    bool RenameDirectoryOnUI(NodePtr node, const std::string& newName, std::string& errMessage);
    
    bool ShouldShowContextMenuForItem(NodePtr node) const;
    bool PageDown();
    bool PageUp();
    bool LongTap();
    void ClearListItem();

    IUINodeView* GetSelectedUIItem();

    // added functions
    NodePtr firstVisibleNode();
    NodePtr firstSelectedNode();
    NodePtr selectedNode(const int selected_index);

    NodePtrs& childrenNodes(bool scan, bool update, RetrieveChildrenResult& result);
    bool gotoUp();
    bool gotoNode(NodePtr node);

protected:
    virtual bool OnItemClick(INT32 iSelectedItem);

private:
    bool UpdateListItemForList();
    bool UpdateListItemForIcon();
    void OnDelete();
    void OnDeleteCategory();
    void OnRenameCategory();
    void OnRenameFolder();
    bool ShouldShowContextMenuForNode(NodePtr node) const;

    static int GetItemPerPageByModelDisplayMode(ModelDisplayMode mode);

private:
    void OnLocalFileClicked(NodePtr local_file_node);
    void OnLocalCategoryClicked(NodePtr local_category_node);
    void OnBookStoreBookClicked(NodePtr bookstore_book_node);
    void OnBookStoreCategoryClicked(NodePtr bookstore_category_node);
    void OnCloudBookClicked(NodePtr cloud_book_node);
    void OnCloudCategoryClicked(NodePtr cloud_category_node);

    void OnNodeClicked(NodePtr node, int select_item);
    bool OpenBookByNode(NodePtr node, int select_item);
    bool uploadBookByNode(NodePtr node, int select_item);
    bool onChildrenNodesReady(const EventArgs& args);

private:
    BOOL m_bIsDisposed;

    int rows_;
    int columns_;
    int items_per_page_;
    int first_visible_;     ///< The first visible child node position.
    int current_page_;      ///< Current page number.
    int page_count_;        ///< Total page number.
    int item_count_;        ///< Number of total items.

    ModelDisplayMode display_mode_;
    int status_filter_;

    BookListUsage m_usage;
    std::vector<UISizer*> m_rowSizers;

    UISizer* m_iconSizer;
    UISizer* m_listSizer;

    std::vector<IUINodeView*> node_views_;
    ModelTree * model_tree_;
    bool m_needClearItem;
};

#endif

