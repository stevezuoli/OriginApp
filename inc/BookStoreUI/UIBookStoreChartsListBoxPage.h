/*
 * =====================================================================================
 *       Filename:  UIBookStoreChartsListBoxPage.h
 *    Description:  charts list page
 *
 *        Version:  1.0
 *        Created:  04/01/2013 03:39:09 PM
 * =====================================================================================
 */

#ifndef _UIBOOKSTORECHARTSLISTBOXPAGE_H_
#define _UIBOOKSTORECHARTSLISTBOXPAGE_H_

#include "BookStoreUI/UIBookStoreListBoxPage.h"
#include "BookStore/BookStoreTypes.h"
using namespace dk::bookstore;

class UIBookStoreChartsListBoxPage : public UIBookStoreListBoxPage
{
public:
    UIBookStoreChartsListBoxPage(const std::string& title, const dk::bookstore::ChartsRankType& type);
    ~UIBookStoreChartsListBoxPage();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreChartsListBoxPage";
    }

    virtual bool CreateListBoxPanel();
    dk::bookstore::FetchDataResult FetchInfo();

private:
    const dk::bookstore::ChartsRankType m_rankType;
};//UIBookStoreChartsListBoxPage
#endif//_UIBOOKSTORECHARTSLISTBOXPAGE_H_


