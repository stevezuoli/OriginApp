#ifndef _DKREADER_TOUCHAPPUI_UIBOOKSTOREDISCOUNTPAGE_H_
#define _DKREADER_TOUCHAPPUI_UIBOOKSTOREDISCOUNTPAGE_H_

#include "BookStoreUI/UIBookStoreTablePage.h"

class UIBookStoreListBoxPanel;

class UIBookStoreClassifyPage : public UIBookStoreTablePage
{
public:
    UIBookStoreClassifyPage(const std::string& id, const std::string& name);
    ~UIBookStoreClassifyPage();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreClassifyPage";
    }

    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);

    virtual dk::bookstore::FetchDataResult FetchInfo();
    virtual void SetTableController();
    
private:
    UIBookStoreListBoxPanel* m_pBestPanel;
    UIBookStoreListBoxPanel* m_pNewPanel;
    std::string m_id;
};
#endif//_DKREADER_TOUCHAPPUI_UIBOOKSTOREDISCOUNTPAGE_H_
