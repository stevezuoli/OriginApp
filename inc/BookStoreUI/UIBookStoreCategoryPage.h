#ifndef _DKREADER_TOUCHAPPUI_UIBOOKSTORECATEGORYPAGE_H_
#define _DKREADER_TOUCHAPPUI_UIBOOKSTORECATEGORYPAGE_H_

#include "BookStoreUI/UIBookStoreListBoxPage.h"

class UIBookStoreCategoryPage : public UIBookStoreListBoxPage
{
public:
    UIBookStoreCategoryPage();
    ~UIBookStoreCategoryPage();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreCategoryPage";
    }

    bool OnCategoryTreeUpdate(const EventArgs& args);
    virtual bool CreateListBoxPanel();
    dk::bookstore::FetchDataResult FetchInfo();
    void SetCategoryId(std::string categoryId)
    {
        m_categoryId = categoryId;
    }

private:
    std::string m_categoryId;
};

#endif//_DKREADER_TOUCHAPPUI_UIBOOKSTORECATEGORYPAGE_H_
