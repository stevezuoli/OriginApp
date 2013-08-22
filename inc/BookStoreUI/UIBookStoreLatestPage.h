#ifndef _DKREADER_UIBOOKSTORELATESTPAGE_H_
#define _DKREADER_UIBOOKSTORELATESTPAGE_H_

#include "BookStoreUI/UIBookStoreListBoxPage.h"

class UIBookStoreLatestPage : public UIBookStoreListBoxPage
{
public:
    UIBookStoreLatestPage();
    ~UIBookStoreLatestPage();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreLatestPage";
    }

    virtual bool CreateListBoxPanel();
    dk::bookstore::FetchDataResult FetchInfo();
};
#endif//_DKREADER_UIBOOKSTORELATESTPAGE_H_

