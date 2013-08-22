#ifndef _DKREADER_TOUCHAPPUI_UIBOOKSTOREDUOKANRECOMMENDPAGE_H_
#define _DKREADER_TOUCHAPPUI_UIBOOKSTOREDUOKANRECOMMENDPAGE_H_

#include "BookStoreUI/UIBookStoreListBoxPage.h"

class UIBookStoreDuokanRecommendPage : public UIBookStoreListBoxPage
{
public:
    UIBookStoreDuokanRecommendPage();
    ~UIBookStoreDuokanRecommendPage();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreDuokanRecommendPage";
    }

    virtual bool CreateListBoxPanel();
    virtual dk::bookstore::FetchDataResult FetchInfo();
};

#endif//_DKREADER_TOUCHAPPUI_UIBOOKSTOREDUOKANRECOMMENDPAGE_H_
