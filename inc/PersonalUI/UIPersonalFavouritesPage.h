#ifndef _DKREADER_PERSONALUI_FAVOURITES_H_
#define _DKREADER_PERSONALUI_FAVOURITES_H_

#include "BookStoreUI/UIBookStoreListBoxPage.h"
#include "BookStore/BookInfo.h"

class UIBookStorePersonalFavouritesPage : public UIBookStoreListBoxPage
{
public:
    UIBookStorePersonalFavouritesPage();
    ~UIBookStorePersonalFavouritesPage();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStorePersonalFavouritesPage";
    }

    virtual bool CreateListBoxPanel();
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    virtual void OnEnter();
    virtual dk::bookstore::FetchDataResult FetchInfo();

    // Handle events
    virtual bool OnBookItemLongTapped(const EventArgs& args);

private:
    bool OnAddFavouriteUpdate(const EventArgs& args);
    bool OnRemoveFavouriteUpdate(const EventArgs& args);
    bool OnAddCartUpdate(const EventArgs& args);
    bool OnRemoveCartUpdate(const EventArgs& args);

    void FetchLocalFavourites();
    void ShowTips(const std::string& tips);

private:
    dk::bookstore::model::BookInfoSPtr m_currentBookInfo;
};

#endif//_DKREADER_PERSONALUI_FAVOURITES_H_
