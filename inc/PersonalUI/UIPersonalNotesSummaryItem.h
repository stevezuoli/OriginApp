#ifndef __PERSONALUI_UIPERSONALNOTESSUMMAYITEM_H
#define __PERSONALUI_UIPERSONALNOTESSUMMAYITEM_H 

#include "GUI/UITextSingleLine.h"
#include "GUI/UIListItemBase.h"
#include "BookStoreUI/UIBookCoverView.h"
#include "BookStore/BookNoteSummary.h"

class UIPersonalNotesSummaryItem : public UIListItemBase
{
public:

    virtual LPCSTR GetClassName() const
    {
        return "UIPersonalNotesSummaryItem";
    }
    UIPersonalNotesSummaryItem();
    ~UIPersonalNotesSummaryItem();
    void InitUI();
    void SetInfoSPtr(dk::bookstore::model::BasicObjectSPtr bookNoteSummary, int index = -1);

    virtual bool ResponseTouchTap();
    virtual bool ResponseOperation();

private:
    void Init();
#ifdef KINDLE_FOR_TOUCH
    bool OnMessageBookCoverTouch(const EventArgs& args);
#endif
    void ShowBookDetailedInfoPage();
    void ShowBookNotePage();
    bool OpenBookFromCover();

private:
    UIBookCoverView m_bookCover;
    UITextSingleLine m_labelBookTitle;
    UITextSingleLine m_labelBookAuthor;
    UITextSingleLine m_labelLastUpdateTime;
    UITextSingleLine m_labelNotesCount;
    dk::bookstore::model::BookNoteSummarySPtr m_bookNoteSummary;
};
#endif//__PERSONALUI_UIPERSONALNOTESSUMMAYITEM_H
