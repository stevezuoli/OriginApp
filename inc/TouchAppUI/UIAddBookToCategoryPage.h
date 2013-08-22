#ifndef __TOUCHAPPUI_UIADDBOOKTOCATEGORYPAGE_H__
#define __TOUCHAPPUI_UIADDBOOKTOCATEGORYPAGE_H__

#include "TouchAppUI/UIDirectoryListBox.h"
#include "GUI/UIPage.h"
#include "GUI/UISeperator.h"

class UIAddBookToCategoryPage: public UIPage
{
public:
    UIAddBookToCategoryPage(const char* category);
    virtual const char* GetClassName() const
    {
        return "UIAddBookToCategoryPage";
    }
    virtual void MoveWindow(
            int left,
            int top,
            int width,
            int height);
protected:
    virtual bool OnChildClick(UIWindow* child);
private:
    void InitUI();
    bool OnChildSelectChanged(const EventArgs& args);
    void UpdateSelectedBooks(size_t bookCount);
    UITouchBackButton m_btnBack;
    UIDirectoryListBox m_fileListBox;
    UITextSingleLine m_txtTitle;
    UITextSingleLine m_txtSelected;
    UITouchComplexButton m_btnSave;
    UITextSingleLine m_txtTotalBook;
    UITextSingleLine m_txtPageNo;
    std::string m_category;
    bool OnSaveClicked();
    UISeperator m_sepHorz;
    UISeperator m_sepVert1;
    UISeperator m_sepVert2;
    void UpdatePageNumber();
    bool OnListTurnPage(const EventArgs&);
};
#endif
