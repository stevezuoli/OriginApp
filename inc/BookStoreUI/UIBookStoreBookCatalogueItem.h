#ifndef _DKREADER_UIBOOKSTOREBOOKCATALOGUEITEM_H_
#define _DKREADER_UIBOOKSTOREBOOKCATALOGUEITEM_H_

#include "GUI/UITextSingleLine.h"
#include "GUI/UIModalDialog.h"
#include "BookStore/BookStoreInfoManager.h"
#include "GUI/UIListItemBase.h"

class UIBookStoreBookCatalogueItem : public UIListItemBase
{
public:
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreBookCatalogueItem";
    }
    UIBookStoreBookCatalogueItem();
    ~UIBookStoreBookCatalogueItem();
    void SetInfoSPtr(dk::bookstore::model::BasicObjectSPtr catalogueInfo, int index);
    bool ResponseTouchTap();

private:
    void Init();

private:
    UITextSingleLine m_catalogue;
    std::string m_preview;
    //UITextSingleLine m_pageNum;
};

class UIBookCataloguePreviewDlg : public UIModalDlg
{
public:
    virtual LPCSTR GetClassName() const
    {
        return "UIBookCataloguePreviewDlg";
    }
    UIBookCataloguePreviewDlg(UIContainer* pParent, const std::string& title, const std::string& preview);
    ~UIBookCataloguePreviewDlg();
private:
    void Init();
    UIText m_preview;
};

#endif//_DKREADER_UIBOOKSTOREBOOKCATALOGUEITEM_H_
