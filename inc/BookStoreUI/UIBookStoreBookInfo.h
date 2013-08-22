#ifndef _DKREADER_TOUCHAPPUI_UIBOOKSTOREBOOKINFO_H_
#define _DKREADER_TOUCHAPPUI_UIBOOKSTOREBOOKINFO_H_

#include "CommonUI/UITablePanel.h"
#include "GUI/UIImage.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UIText.h"
#include "GUI/UIStars.h"
#include "BookStoreUI/UIBookCoverView.h"
#include "BookStore/BookStoreInfoManager.h"
#include "BookStoreUI/UIBookStorePriceButtonController.h"
#include "BookStoreUI/UIBookStoreBookInfoRelatedList.h"
#include "BookStoreUI/UIDiscountTextSingleLine.h"
#ifdef KINDLE_FOR_TOUCH
    #include "GUI/UITouchComplexButton.h"
#else
    #include "GUI/UIComplexButton.h"
#endif
#include "GUI/UIButtonGroup.h"

class UITouchButton;

class UIBookStoreBookInfo : public UITablePanel
{
public:
    enum TAG_BUTTON_ID
    {
        ID_BOOKINFO_TAG_0 = 0x1000,
        ID_BOOKINFO_TAG_1,
        ID_BOOKINFO_TAG_2,
        ID_BOOKINFO_TAG_3,
        ID_BOOKINFO_TAG_4,
        ID_BOOKINFO_TAG_5,
        ID_BOOKINFO_TAG_6,
        ID_BOOKINFO_TAG_7,
        ID_BOOKINFO_TAG_8,
        ID_BOOKINFO_TAG_9,
    };
	static const int RELATED_BOOK_COUNT = 4;
	virtual LPCSTR GetClassName() const
	{
		return "UIBookStoreBookInfo";
	}
	UIBookStoreBookInfo(UIContainer* pParent, const std::string& bookId, const std::string& bookName, const std::string& duokanKey);
    ~UIBookStoreBookInfo();

	void UpdateMyBookDetailedInfo();
	void InitUI();
	void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    virtual BOOL OnKeyPress(INT32 iKeyCode);
    void UpdateSectionUI(bool bIsFull = false);
    bool OnMessageBookDetailedInfoUpdate(const EventArgs& args);
    std::string GetBookTitle() const;

private:
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
    void Init();
    bool OnMessagePriceButtonUpdate(const EventArgs& args);
    void GotoBookAbstructPage();
private:
#ifdef KINDLE_FOR_TOUCH
    bool OnTouchEvent(MoveEvent* moveEvent);
    bool OnSingleTapUp(MoveEvent* moveEvent);
    class BookStoreBookInfoGestureListener: public SimpleGestureListener
    {
    public:
        void SetBookStoreBookInfo(UIBookStoreBookInfo* bookInfo)
        {
            m_pBookInfo = bookInfo;
        }
        virtual bool OnSingleTapUp(MoveEvent* moveEvent)
        {
            return m_pBookInfo->OnSingleTapUp(moveEvent);
        }
    private:
        UIBookStoreBookInfo* m_pBookInfo;
    };

    GestureDetector m_gestureDetector;
    BookStoreBookInfoGestureListener m_gestureListener;
#endif
    UIBookCoverView m_bookCover;
    UITextSingleLine m_authorLabel;
    UITextSingleLine m_sizeLabel;
    UITextSingleLine m_priceTitle;
    UIDiscountTextSingleLine m_price;
    UITextSingleLine m_paperPriceTitle;
    UIDiscountTextSingleLine m_paperPrice;
    UITextSingleLine m_tagLabel;
    UIButtonGroup m_tagBtnLst;
    std::vector<std::string> m_tagList;
    UIStars m_scoring;
    UITextSingleLine m_scoreCount;

    UIBookStoreBookInfoRelatedList m_relatedBook;
    dk::bookstore::model::BookInfoSPtr m_bookDetailedInfo;
    std::string m_bookId;
    std::string m_bookName;
    static const int tagCount = 10;
#ifdef KINDLE_FOR_TOUCH
    UITouchComplexButton m_addFavouriteButton;
    UITouchComplexButton m_addToCartButton;
    UITouchComplexButton m_readPartContentButton;
    UITouchComplexButton m_buyButton;
    UITouchComplexButton m_shareButton;
    UITouchButton  m_tagBtn[tagCount];
#else
    UIComplexButton m_addFavouriteButton;
    UIComplexButton m_addToCartButton;
    UIComplexButton m_readPartContentButton;
    UIComplexButton m_buyButton;
    UIComplexButton m_moreAbstruct;
    UIComplexButton m_shareButton;
    UIComplexButton  m_tagBtn[tagCount];
#endif
    UIText m_abstruct;
    UIBookStorePriceButtonController m_buttonController;
    UISizer* m_btnSizer;
    UISizer* m_midSizer;
    UIButtonGroup* m_middleBar;
};

#endif//_DKREADER_TOUCHAPPUI_UIBOOKSTOREBOOKINFO_H_

