////////////////////////////////////////////////////////////////////////
// UIDirectoryListItem.h
// Contact: liujt
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UIDIRECTORYLISTITEM_H__
#define __UIDIRECTORYLISTITEM_H__

#include "GUI/UIContainer.h"
#include "GUI/UIImage.h"
#include "GUI/UIComplexButton.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UIProgressBar.h"
#include "GUI/UIListItemBase.h"
#include "CommonUI/UICoverViewItem.h"
#include "CommonUI/UIBookListProgressBar.h"

#include "Model/node.h"

using namespace dk::document_model;

class UISizer;
class UISizerItem;
class CDKFile;

class UIDirectoryListItem : public UIListItemBase
{
public:
    UIDirectoryListItem(UICompoundListBox* pParent,
                        BookListUsage usage,
                        int iconWidth,
                        int iconHeight);

    ~UIDirectoryListItem();

    virtual LPCSTR GetClassName() const
    {
        return "UIDirectoryListItem";
    }

    //void SetItemModel(IDirectoryItemModel *pItemModel);
    void SetNode(NodePtr node);

    virtual BOOL OnKeyPress(INT32 iKeyCode);
    virtual void SetFocus(bool bIsFocus);
    virtual bool SetVisible(bool bVisible);
	virtual void SetHighLight(bool isHighLight);
    void SetBookListMode(BookListMode mode);
    virtual HRESULT Draw(DK_IMAGE drawingImg);
    bool IsSelected() const;
    void SetSelected(bool selected);

    virtual void MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight);
    void SetIsLoading(bool isLoading);

private:
    bool Init();
    void updateByNode();
    void updateAsDirectory();
    void updateAsFile();
    bool IsDir() const;

    HRESULT DrawCoverView(DK_IMAGE drawingImg);
    HRESULT DrawListView(DK_IMAGE drawingImg);
    
    //SPtr<ITpImage> GetImageByFormat(PCDKFile file);

    static int PageCountToProgressBarDotCount(int pageCount);

private:
    bool               m_bIsDisposed;
    UIImage            m_imgNewBook;
    UIImage            m_imgItem;
    UIImage            m_imgDirArrow;
    UITextSingleLine   m_txtItemNameLabel;
    UITextSingleLine   m_txtAuthorLabel;
    UITextSingleLine   m_txtTrialLabel;
    UIBookListProgressBar m_readingProgressBar;
    
    BookListMode m_bookListMode;
    UISizer* m_coverViewSizer;
    UISizer* m_listViewSizer;
    BookListUsage m_usage;
    UISizerItem* m_spaceBetweenAuthorAndArrow;
    UICoverViewItem m_cover;

    //IDirectoryItemModel *m_pItemModel;
    NodePtr m_node;
};

#endif

