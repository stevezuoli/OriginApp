////////////////////////////////////////////////////////////////////////
// UINodeView.h
// Contact: liujt
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UINodeView_H__
#define __UINodeView_H__

#include "CommonUI/IUINodeView.h"
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

class UINodeView : public IUINodeView
{
public:
    UINodeView(UICompoundListBox* pParent,
               ModelTree* model_tree,
               BookListUsage usage,
               int iconWidth,
               int iconHeight);

    virtual ~UINodeView();

    virtual LPCSTR GetClassName() const
    {
        return "UINodeView";
    }

    //void SetItemModel(IDirectoryItemModel *pItemModel);
    void SetNode(NodePtr node);

    virtual BOOL OnKeyPress(INT32 iKeyCode);
    virtual void SetFocus(bool bIsFocus);
    virtual bool SetVisible(bool bVisible);
	virtual void SetHighLight(bool isHighLight);
    void SetModelDisplayMode(ModelDisplayMode mode);
    virtual HRESULT Draw(DK_IMAGE drawingImg);
    bool IsSelected() const;
    void SetSelected(bool selected);

    virtual void MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight);
    void SetIsLoading(bool isLoading);

    virtual bool onChildrenReady(const EventArgs& args);
    virtual bool onStatusUpdated(const EventArgs& args);
    virtual bool onLoadingFinished(const EventArgs& args);
private:
    bool Init();
    void updateByNode();
    void updateAsDirectory();
    void updateAsLocalFile();
    void updateAsCloudFile();
    void updateSelectedImage();

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
    
    ModelDisplayMode m_modelDisplayMode;
    UISizer* m_coverViewSizer;
    UISizer* m_listViewSizer;
    BookListUsage m_usage;
    UISizerItem* m_spaceBetweenAuthorAndArrow;
    UICoverViewItem m_cover;

    NodePtr m_node;
};

#endif

