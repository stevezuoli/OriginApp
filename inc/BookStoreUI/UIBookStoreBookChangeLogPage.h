#ifndef _DKREADER_TOUCHAPPUI_UIBOOKSTOREBOOKCHANGELOGPAGE_H_
#define _DKREADER_TOUCHAPPUI_UIBOOKSTOREBOOKCHANGELOGPAGE_H_

#include "BookStoreUI/UIBookStoreListBoxPage.h"

class UIBookStoreBookChangeLogPage : public UIBookStoreListBoxPage
{
public:
    UIBookStoreBookChangeLogPage(const std::string& bookId, const std::string& bookName);
    ~UIBookStoreBookChangeLogPage();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreBookChangeLogPage";
    }

    virtual bool CreateListBoxPanel();
    dk::bookstore::FetchDataResult FetchInfo();
private:
    std::string m_bookId;
};
#endif//_DKREADER_TOUCHAPPUI_UIBOOKSTOREBOOKCHANGELOGPAGE_H_
