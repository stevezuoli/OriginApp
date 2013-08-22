////////////////////////////////////////////////////////////////////////
// UIDirectoryListBox.h
// Contact: liujt
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UIDIRECTORYLISTBOX_H__
#define __UIDIRECTORYLISTBOX_H__

#include "GUI/UICompoundListBox.h"
#include "Model/IDirectoryItemModel.h"
#include "TouchAppUI/UIDirectoryListItem.h"
#include <vector>

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


class UIDirectoryListBox : public UICompoundListBox
{
public:
    static const char* EventChildSelectChanged;
    UIDirectoryListBox(BookListUsage usage);
    virtual ~UIDirectoryListBox();
    virtual LPCSTR GetClassName() const
    {
        return "UIDirectoryListBox";
    }
    virtual BOOL OnKeyPress(INT32 iKeyCode);
    virtual void InitListItem();
    virtual void UpdateListUI();

    virtual void OnCommand(DWORD _dwCmdId, UIWindow* _pSender, DWORD _dwParam);

    //void SetDirectoryList(IDirectoryItemModel **ppList, INT32 iNum);

#ifdef KINDLE_FOR_TOUCH
    virtual bool DoHandleListTurnPage(bool fKeyUp);
    virtual BOOL HandleLongTap(INT32 selectedItem);
#endif
    
    bool SetCurrentPath(const char* path, const char* category = NULL);
    bool SetCurrentPath(const std::string& path, const char* category = NULL)
    {
        return SetCurrentPath(path.c_str(), category);
    }
    bool BackToUpperFolder();
    bool UpdateListItem();

    
    int GetTotalPageCount() const 
    { 
        return m_totalPage;
    }
    int GetCurPageIndex() const 
    { 
        return m_curPage;
    }
    void SetCurPageIndex(int page)
    {
        m_curPage = page;
    }
    int GetItemNum() const 
    {
        return m_itemNum;
    }



    const std::string& GetCurrentPath() const 
    { 
        return m_curPath;
    }
    void SetBookListMode(BookListMode bookListMode);
    bool HasCategory() const;
    const std::string& GetCategory() const
    {
        return m_category;
    }
    std::vector<std::string> GetSelectedBookIds() const;
    size_t GetSelectedBookCount() const;

private:
    void DeleteFileOnUI(const std::string& filepath);
    void DeleteDirectoryOnUI(const std::string& dirpath);
    bool RenameDirectoryOnUI(const std::string& dirpath, const std::string& newName, std::string& errMessage);
    bool PageDown();
    bool PageUp();
    bool LongTap();
    void ClearListItem();
    void InitListItemFromDirectory();
    void InitListItemFromBookStoreDirectory();
    void InitListItemFromBookStoreDirectoryDirect();
    void InitListItemFromBookStoreDirectoryWithCategory();
    void InitListItemFromSortedFiles();
    UIDirectoryListItem* GetSelectedUIItem();
    IDirectoryItemModel *GetSelectedItemModel();
    IDirectoryItemModel *GetSelectedItemModel(int selectedItem);


protected:
    virtual bool OnItemClick(INT32 iSelectedItem);
    void InitListItemFromRootDirectory();

private:
    bool OnItemClickEvent(const EventArgs& args);
    BOOL m_bIsDisposed;
    std::vector< IDirectoryItemModel *> m_itemList;
	std::vector<UIDirectoryListItem*> m_directoryList;
    static int GetItemPerPageByBookListMode(BookListMode mode);

    int m_totalPage;
    int m_curPage;
    size_t m_itemNum;
    size_t m_itemNumPerPage;
    std::string m_curPath;
    BookListMode m_bookListMode;
    bool UpdateListItemForList();
    bool UpdateListItemForIcon();
    std::vector<UISizer*> m_rowSizers;
    std::string m_category;
    void OnDelete();
    void OnDeleteCategory();
    void OnRenameCategory();
    void OnRenameFolder();
    UISizer* m_iconSizer;
    UISizer* m_listSizer;

    bool ShouldShowContextMenuForItem(IDirectoryItemModel* item) const;
    BookListUsage m_usage;
    std::vector<int> m_curPageStack;
};

#endif

