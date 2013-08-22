#ifndef _DKREADER_UIBOOKSTORETAGLISTPAGE_H_
#define _DKREADER_UIBOOKSTORETAGLISTPAGE_H_

#include "BookStoreUI/UIBookStoreListBoxPage.h"

class UIBookStoreTagListPage : public UIBookStoreListBoxPage
{
public:
    UIBookStoreTagListPage(const char* tagName);
    ~UIBookStoreTagListPage();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreTagListPage";
    }

    virtual bool CreateListBoxPanel();
    virtual dk::bookstore::FetchDataResult FetchInfo();
private:
    std::string m_tagName;
};
#endif//_DKREADER_UIBOOKSTORETAGLISTPAGE_H_

