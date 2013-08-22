#ifndef __UIBOOKMENU_H__
#define __UIBOOKMENU_H__

#include "GUI/UIDialog.h"
#include "GUI/UITouchComplexButton.h"

class UIBookMenuDlg : public UIDialog
{
public:
    enum BookMenuType
    {
        // Below values are used for array indexing
        // Don't change
        BMT_BOOK = 0,
        BMT_FOLDER,
        BMT_DUOKAN_BOOK,
        BMT_DUOKAN_CATEGORY,
		BMT_PERSONAL_HIDEBOOKLIST,
        BMT_DUOKAN_USER_CUSTOMIZE
    };
    UIBookMenuDlg(UIContainer* parent, BookMenuType type);
    UIBookMenuDlg(UIContainer* parent, const std::vector<int>& customizedBtnIDs,
        const std::vector<int>& customizedStrIDs); 
    UIBookMenuDlg(UIContainer* parent, const std::vector<int>& customizedBtnIDs,
        const std::vector<string>& customizedStrings); 
    virtual ~UIBookMenuDlg();
    
public:
    bool OnHookTouch(MoveEvent* moveEvent);
    virtual LPCSTR GetClassName() const 
    { 
        return "UIBookMenuDlg";
    }
    HRESULT DrawBackGround (DK_IMAGE _drawingImg);
    virtual void OnCommand(DWORD _dwCmdId, UIWindow* _pSender, DWORD _dwParam);

    void InitUI();
    int GetCommandId() const;

private:
    enum {
        MAX_BUTTONS = 4
    };
    UITouchComplexButton   m_btns[MAX_BUTTONS];
    BookMenuType m_bookMenuType;

    std::vector<int> m_customizedBtnIDs;
    std::vector<std::string> m_customizedStrings;
    int m_commandId;
};

#endif // __UIBOOKMENU_H__

