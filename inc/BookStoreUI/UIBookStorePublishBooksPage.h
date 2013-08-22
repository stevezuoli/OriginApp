#ifndef _DKREADER_UIBOOKSTOREPUBLISHBOOKSPAGE_H_
#define _DKREADER_UIBOOKSTOREPUBLISHBOOKSPAGE_H_

#include "BookStoreUI/UIBookStoreListBoxPage.h"

class UIBookStorePublishBooksPage : public UIBookStoreListBoxPage
{
public:
    UIBookStorePublishBooksPage(const string& publishID, const string& publishName);
    ~UIBookStorePublishBooksPage();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStorePublishBooksPage";
    }

    virtual bool CreateListBoxPanel();
    dk::bookstore::FetchDataResult FetchInfo();
private:
    std::string m_publishID;
};
#endif//_DKREADER_UIBOOKSTOREPUBLISHBOOKSPAGE_H_

