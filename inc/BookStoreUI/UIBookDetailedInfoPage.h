#ifndef _DKREADER_TOUCHAPPUI_UIBOOKDETAILEDINFOPAGE_H_
#define _DKREADER_TOUCHAPPUI_UIBOOKDETAILEDINFOPAGE_H_

#ifdef KINDLE_FOR_TOUCH
    #include "GUI/UITouchComplexButton.h"
#endif
#include "BookStoreUI/UIBookStoreTablePage.h"
#include "GUI/UITextSingleLine.h"

class UIBookStoreBookInfo;
class UIBookStoreListBoxWithPageNum;
class UIBookStoreBookComment;
class UIBookStoreBookRightsInfo;

class UIBookDetailedInfoPage : public UIBookStoreTablePage
{
public:
    UIBookDetailedInfoPage(const std::string& bookId,const std::string& bookName, const std::string& duokanKey = std::string());
    ~UIBookDetailedInfoPage();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookDetailedInfoPage";
    }
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);

    virtual dk::bookstore::FetchDataResult FetchInfo();
    virtual void SetTableController();

    bool OnMessageFetchInfoUpdate(const EventArgs& args);
private:
    std::string m_bookId;
    std::string m_bookName;
    std::string m_duokanKey;
    UIBookStoreBookInfo* m_pBookinfo;
    UIBookStoreBookRightsInfo* m_pBookRights;
    UIBookStoreListBoxWithPageNum* m_pBookCatalogue;
    UIBookStoreBookComment* m_pBookComment;
    bool m_resultInfo;
    bool m_resultCatalogue;
    bool m_resultRights;
};

#endif//_DKREADER_TOUCHAPPUI_UIBOOKDETAILEDINFOPAGE_H_
