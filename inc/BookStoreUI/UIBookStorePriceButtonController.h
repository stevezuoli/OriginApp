#ifndef __TOUCHAPPUI_UIBOOKSTOREPRICEBUTTONCONTROLLER_H__
#define __TOUCHAPPUI_UIBOOKSTOREPRICEBUTTONCONTROLLER_H__

#include "GUI/EventListener.h"
#include "GUI/UIText.h"
#ifdef KINDLE_FOR_TOUCH
#include "GUI/UIComplexButton.h"
#else
#include "GUI/UITextSingleLine.h"
#endif
#include "BookStoreUI/UIDiscountTextSingleLine.h"
#include "BookStore/BookInfo.h"
#include "BookStore/BookStoreTypes.h"
#include "GUI/UIWindow.h"
#include "GUI/EventArgs.h"

using dk::bookstore::model::BookInfo;
using dk::bookstore::model::BookInfoSPtr;

enum BUTTONUPDATETYPE
{
    BookStatusUpdate = 0x1,
    TrialBookStatusUpdate = 0x2,
    FavouriteBookStatusUpdate = 0x3,
    BookInCartUpdate = 0x4,
    AllStatusUpdate = 0x5
};
class PriceButtonEventArgs: public EventArgs
{
public:
    PriceButtonEventArgs()
        : resultCode(-1)
        , updateStatus(AllStatusUpdate)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new PriceButtonEventArgs(*this);
    }
    int resultCode;
    std::string bookId;
    BUTTONUPDATETYPE updateStatus;
};

class UIBookStorePriceButtonListener: public EventSet
{
public:
    static const char* EventPriceButtonUpdate;
    static UIBookStorePriceButtonListener* GetInstance();

    void FireBookStatusUpdateEvent(BUTTONUPDATETYPE status, const std::string& bookId, int errorCode = 0);
};

class UIBookStorePriceButtonController: public EventListener 
{
public:

    UIBookStorePriceButtonController(
            UIContainer* root,
            UIDiscountTextSingleLine* originPriceLine,
            UIWindow* actionWnd,
            UIWindow* trialWnd = 0,
            UIWindow* addFavouriteWnd = 0,
            UIWindow* addCartWnd = 0,
            const std::string& duokanKey = std::string());

    void SetBookInfo(BookInfoSPtr bookInfo);
    void UpdateBookStatus();
    void UpdateTrialBookStatus();
    void UpdateFavouriteBookStatus();
    void UpdateBookInCartStatus();

    virtual HRESULT Draw(DkFormat::DK_IMAGE)
    {
        return 0;
    }

private:
    enum Action
    {
        NONE,
        CREATE_ORDER,
        FETCH_DRM,
        FETCH_BOOKINFO,
        DOWNLOADING,
        MAIL_ORDER,
        VERIFY_PAY,
        WAITING_QUEUE,
    };

    enum FavouriteAction
    {
        FAVOURITE_NONE,
        ADDING_FAVOURITE,
        REMOVING_FAVOURITE
    };

    enum CartAction
    {
        CART_NONE,
        ADDING_CART,
        REMOVING_CART
    };

private:
    bool OnMailOrderUpdate(const EventArgs& eventArgs);

    bool DoCreateOrder();
    bool OnCreateOrderUpdate(const EventArgs& args);
    bool OnDrmUpdate(const EventArgs& args);
    int GetBookStatus();
    int GetTrialBookStatus();

    // Remove book from cart once it's purchased
    bool RemoveFromCartOncePurchased();

    // handle click events
    bool OnActionButtonClick(const EventArgs&);
    bool OnTrialButtonClick(const EventArgs&);
    bool OnAddFavouriteButtonClick(const EventArgs&);
    bool OnAddCartButtonClick(const EventArgs&);

    bool OnPurchasedListUpdate(const EventArgs& args);
    bool OnBookInfoUpdate(const EventArgs& args);
    bool OnDownloadUpdate(const EventArgs& args);

    void DoExchangeDuokanKey();
    bool OnExchangeDuokanKeyUpdate(const EventArgs& args);

    // For adding to favourite list
    bool OnAddFavouriteUpdate(const EventArgs& args);
    bool OnRemoveFavouriteUpdate(const EventArgs& args);

    // For adding to favourite list
    bool OnAddCartUpdate(const EventArgs& args);
    bool OnRemoveCartUpdate(const EventArgs& args);

    UIContainer* GetRepaintRoot();
    UIContainer* GetDlgParent();

	bool CheckBookKernelVersion(const std::string& kernelVersion);
    void FireBookStatusUpdateEvent(BUTTONUPDATETYPE status, int errorCode = 0);
private:
    UIContainer* m_root;
    UIDiscountTextSingleLine* m_originPriceLine;
    UIWindow* m_actionWnd;
    UIWindow* m_trialWnd;
    UIWindow* m_addFavouriteWindow;
    UIWindow* m_addCartWindow;

    BookInfoSPtr m_bookInfo;
    Action m_currentAction;
    FavouriteAction m_favouriteAction;
    CartAction m_cartAction;
    std::string m_duokanKey;

    // download info
    int m_downloadingProgress;
    int m_trialDownloadingProgress;
};
#endif
