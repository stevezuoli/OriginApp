#ifndef _DKREADER_UIBOOKSTOREPUBLISHPAGE_H_
#define _DKREADER_UIBOOKSTOREPUBLISHPAGE_H_

#include "BookStoreUI/UIBookStoreNavigation.h"
#include "BookStoreUI/UIBookStorePublishingItem.h"

class UIBookStorePublishPage : public UIBookStoreNavigation
{
public:
    UIBookStorePublishPage();
    ~UIBookStorePublishPage();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStorePublishPage";
    }

    virtual UISizer* CreateMidSizer();
    dk::bookstore::FetchDataResult FetchInfo();
    virtual void SetFetchStatus(FETCH_STATUS status);
    virtual bool OnMessageListBoxUpdate(const EventArgs& args);
    HRESULT DrawBackGround(DK_IMAGE drawingImg);
    BOOL SetVisible(BOOL bVisible);
    BOOL OnKeyPress(INT32 iKeyCode);
private:
    void UpdatePublishList();

private:
    static const int PUBLISHCOUNT = 10;
    static const int LINE_COUNT = 2;
    UIBookStorePublishingItem m_itemList[PUBLISHCOUNT];
};
#endif//_DKREADER_UIBOOKSTOREPUBLISHPAGE_H_

