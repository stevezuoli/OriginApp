#ifndef _DKREADER_BOOKSTORE_CART_H_
#define _DKREADER_BOOKSTORE_CART_H_

#include "GUI/UIButtonGroup.h"
#include "BookStoreUI/UIBookStoreListBoxPage.h"
#include "BookStore/BookInfo.h"


class UICartPageBottomBar : public UIButtonGroup 
{
public:
    UICartPageBottomBar();
    ~UICartPageBottomBar();
    
    virtual LPCSTR GetClassName() const
    {
        return "UICartPageBottomBar";
    }
    
    //HRESULT Draw(DK_IMAGE drawingImg);
    
    //virtual void MoveWindow(INT32 _iLeft, INT32 _iTop, INT32 _iWidth, INT32 _iHeight);
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);

private:
    void InitUI();
    bool OnGetBooksInCartUpdate(const EventArgs& args);
    bool OnGetSpecialEventsUpdate(const EventArgs& args);

    void ClearAllText();
    UIContainer* GetDlgParent();

private:
    UITextSingleLine m_textCartSummary;
    UITextSingleLine m_textPurchaseNotice;
    UITextSingleLine m_textDiscountTitle;
    UITextSingleLine m_textDiscountsNotice;
    UITextSingleLine m_textSpecialEventContent;
    UITextSingleLine m_textSpecialEventNotice;
    
    UITouchComplexButton m_btnLogin;
    friend class UIBookStoreCartPage;
};


class UIBookStoreCartPage : public UIBookStoreListBoxPage
{
public:
    UIBookStoreCartPage();
    ~UIBookStoreCartPage();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreCartPage";
    }

    virtual bool CreateListBoxPanel();
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    virtual void OnEnter();
    virtual dk::bookstore::FetchDataResult FetchInfo();

    // Handle events
    virtual bool OnBookItemLongTapped(const EventArgs& args);
    virtual bool OnBookInCartDelete(const EventArgs& args);
    virtual UIButtonGroup* GetCustomizedBottomBar();

private:
    bool OnAddCartUpdate(const EventArgs& args);
    bool OnRemoveCartUpdate(const EventArgs& args);
    bool OnAddFavouriteUpdate(const EventArgs& args);
    bool OnRemoveFavouriteUpdate(const EventArgs& args);

    void FetchLocalCart();
    void ShowTips(const std::string& tips);

private:
    dk::bookstore::model::BookInfoSPtr m_currentBookInfo;
};

#endif//_DKREADER_BOOKSTORE_CART_H_
