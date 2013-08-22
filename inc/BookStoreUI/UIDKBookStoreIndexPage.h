#ifndef _DKREADER_TOUCHAPPUI_UIDKBOOKSTOREINDEXPAGE_H_
#define _DKREADER_TOUCHAPPUI_UIDKBOOKSTOREINDEXPAGE_H_

#include <string>
#include <vector>
#ifdef KINDLE_FOR_TOUCH
#include "GUI/UITouchComplexButton.h"
#include "GUI/UIPage.h"
#else
#include "GUI/UIComplexButton.h"
#include "BookStoreUI/UIBookStorePage.h"
#endif
#include "GUI/UITextBox.h"
#include "BookStoreUI/UIBookCoverView.h"
#include "BookStoreUI/UIBookStoreIndexPageItem.h"
#include "BookStore/BookStoreInfoManager.h"
#include "BookStore/AdInfo.h"
#include "GUI/UIButtonGroup.h"
#ifdef KINDLE_FOR_TOUCH
class UIDKBookStoreIndexPage : public UIPage
#else
class UIDKBookStoreIndexPage : public UIBookStorePage
#endif
{
public:
	static const int TOPICCOUNT = 4;
	static const int TOPICLINECOUNT = 2;
	static const int TOPICROWCOUNT = 2;

    static const int CHANNEL_COUNT = 6;
    static const int CHANNEL_LINE_COUNT = 2;
	UIDKBookStoreIndexPage();
    ~UIDKBookStoreIndexPage();
    virtual LPCSTR GetClassName() const
    {
        return "UIDKBookStoreIndexPage";
    }

#ifndef KINDLE_FOR_TOUCH
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
    virtual void OnClick();
    bool OnMessageSendBookKeyUpdate(const EventArgs& args);
#endif
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    virtual void OnEnter();

private:
    void Init();

    dk::bookstore::FetchDataResult FetchSpecialEvents();
	dk::bookstore::FetchDataResult FetchAds();
    dk::bookstore::FetchDataResult FetchBooksInCart();
	bool OnMessageTopicUpdate(const EventArgs& args);
    bool OnMessageTopicTouch(const EventArgs& args);
    bool OnGetBooksInCartUpdate(const EventArgs& args);
	void UpdateTopicCoverList();

private:
	UITextBox m_searchBox;
	UIBookCoverView m_topicCoverList[TOPICCOUNT];
    UIBookStoreIndexPageItem            m_itemList[CHANNEL_COUNT];
	dk::bookstore::model::AdInfoList    m_topicInfoList;
#ifdef KINDLE_FOR_TOUCH
    UITouchComplexButton m_btnCart;
    UITouchComplexButton m_btnSendDuokanKey;
#else
    UIComplexButton m_btnSendDuokanKey;
#endif
    UIButtonGroup m_orderButtonsGroup;
};

#endif//_DKREADER_TOUCHAPPUI_UIDKBOOKSTOREINDEXPAGE_H_
