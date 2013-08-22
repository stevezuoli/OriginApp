#ifndef _BOOKSTOREUI_UIBOOKSTOREBOOKOTHERINFO_H_
#define _BOOKSTOREUI_UIBOOKSTOREBOOKOTHERINFO_H_

#include "CommonUI/UITablePanel.h"
#include "GUI/UITextSingleLine.h"
#ifdef KINDLE_FOR_TOUCH
#include "GUI/UITouchComplexButton.h"
#else
#include "GUI/UIComplexButton.h"
#endif

class UIBookStoreBookRightsInfo : public UITablePanel
{
public:
    UIBookStoreBookRightsInfo(const std::string& bookId);
    ~UIBookStoreBookRightsInfo();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreBookRightsInfo";
    }
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    virtual BOOL OnKeyPress(INT32 iKeyCode);
    bool OnMessageBookDetailedInfoUpdate(const EventArgs& args);
    void SetBookInfo(dk::bookstore::model::BookInfoSPtr bookDetailedInfo);
private:
    void Init();
    std::string m_bookId;
    dk::bookstore::model::BookInfoSPtr m_bookDetailedInfo;
    UITextSingleLine m_authors;
    UITextSingleLine m_editors;
    UITextSingleLine m_translators;
    UITextSingleLine m_rights;
    UITextSingleLine m_identifier;
    UITextSingleLine m_issued;
    UITextSingleLine m_updateTime;
    UITextSingleLine m_size;
    UITextSingleLine m_wordCount;
#ifdef KINDLE_FOR_TOUCH
    UITouchComplexButton m_moreUpdate;
#else
    UIComplexButton m_moreUpdate;
#endif
};

#endif//_BOOKSTOREUI_UIBOOKSTOREBOOKOTHERINFO_H_

