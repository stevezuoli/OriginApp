#ifndef _DKREADER_BOOKSTORE_POST_ADD_CART_PAGE_H_
#define _DKREADER_BOOKSTORE_POST_ADD_CART_PAGE_H_

#include "BookStoreUI/UIBookStoreNavigation.h"
#include "GUI/UIImage.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UIText.h"
#include "BookStoreUI/UIBookCoverView.h"
#include "BookStore/BookStoreInfoManager.h"
#include "BookStoreUI/UIBookStoreBookInfoRelatedList.h"
#ifdef KINDLE_FOR_TOUCH
    #include "GUI/UITouchComplexButton.h"
#else
    #include "GUI/UIComplexButton.h"
#endif
#include "GUI/UIButtonGroup.h"

class UIBookStorePostAddCartPage : public UIBookStoreNavigation
{
public:
	virtual LPCSTR GetClassName() const
	{
		return "UIBookStorePostAddCartPage";
	}
	UIBookStorePostAddCartPage(dk::bookstore::model::BookInfoSPtr bookInfo);
    ~UIBookStorePostAddCartPage();

	void UpdateMyBookDetailedInfo();
	void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);

    virtual UISizer* CreateMidSizer();
    virtual BOOL OnKeyPress(INT32 iKeyCode);
    virtual dk::bookstore::FetchDataResult FetchInfo();
    virtual UISizerFlags GetMiddleSizerFlags();
    virtual void OnEnter();

private:
    void Init();
    bool OnGetBooksInCartUpdate(const EventArgs& args);

private:
    UIBookCoverView  m_bookCover;
    UITextSingleLine m_notificationLabel;
    UITextSingleLine m_bookCountLabel;
    UITextSingleLine m_bookStoreLabel;

#ifdef KINDLE_FOR_TOUCH
    UITouchComplexButton m_strollButton;
    UITouchComplexButton m_gotoCartButton;
#else
    UIComplexButton m_strollButton;
    UIComplexButton m_gotoCartButton;
#endif

    UIBookStoreBookInfoRelatedList m_relatedBook;
    dk::bookstore::model::BookInfoSPtr m_bookDetailedInfo;
    std::string m_bookName;
};

#endif //_DKREADER_BOOKSTORE_POST_ADD_CART_PAGE_H_
