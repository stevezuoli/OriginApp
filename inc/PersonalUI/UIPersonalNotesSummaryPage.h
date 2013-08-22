#ifndef __PERSONALUI_UIPERSONALNOTESSUMMARYPAGE_H__
#define __PERSONALUI_UIPERSONALNOTESSUMMARYPAGE_H__

#include <pthread.h>
#include "BookStoreUI/UIBookStoreListBoxPage.h"

class UIPersonalNotesSummaryPage : public UIBookStoreListBoxPage
{
public:
    UIPersonalNotesSummaryPage();
    ~UIPersonalNotesSummaryPage();
    virtual LPCSTR GetClassName() const
    {
        return "UIPersonalNotesSummaryPage";
    }

    virtual bool CreateListBoxPanel();
    dk::bookstore::FetchDataResult FetchInfo();

private:
    bool OnNotesSummaryUpdate(const EventArgs& args);
    bool OnMessageFetchBookInfoUpdate(const EventArgs& args);
    bool OnLocalNotesSummaryUpdate(const EventArgs& args);
    bool OnDuoKanNotesSummaryUpdate(const EventArgs& args);

private:
    pthread_mutex_t m_lock;
    bool m_dkBookNotesReady;
    bool m_localBookNotesReady;
};//UIPersonalNotesSummaryPage
#endif//__PERSONALUI_UIPERSONALNotesSummaryPage_H__

