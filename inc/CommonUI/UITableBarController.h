////////////////////////////////////////////////////////////////////////
// UITableController.h
// Contact:
////////////////////////////////////////////////////////////////////////

#ifndef _UITABLECONTROLLER_H_
#define _UITABLECONTROLLER_H_

#include "GUI/UIContainer.h"
#include "GUI/UIButtonGroup.h"


class UISizer;
class UITablePanel;
class UITouchComplexButton;
class UIComplexButton;

class UITableBar : public UIButtonGroup
{
public:
    UITableBar(UIContainer* pParent);
    ~UITableBar();

    virtual LPCSTR GetClassName() const
    {
        return "UITableBar";
    }
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    bool AddTab(const char* title, const DWORD cmdId, bool bIsSelected, char cHotKey);
    void SetFocusedImageID(int imageID);

protected:
    virtual HRESULT DrawFocusedSymbol(DK_IMAGE& image);

private:
    void InitUI();

private:
#ifdef KINDLE_FOR_TOUCH
    std::vector<UITouchComplexButton*> m_buttons;
#else
    std::vector<UIComplexButton*> m_buttons;
#endif
    int m_imageID;
};

class EventPanelIsVisibleArgs : public EventArgs
{
public:
    EventPanelIsVisibleArgs()
        : m_pShowPanel(0)
        , m_bIsVisible(false)
    {
    }
    virtual EventArgs* Clone() const
    {
        return new EventPanelIsVisibleArgs(*this);
    }

    UITablePanel* m_pShowPanel;
    bool          m_bIsVisible;
};

class UITableController : public UIContainer
{
public:
    static const char* EventPanelIsVisible;

public:
    UITableController();
    ~UITableController();
    virtual LPCSTR GetClassName() const
    {
        return "UITableController";
    }

    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
    virtual BOOL OnKeyPress(INT32 iKeyCode);
    void SetBottomLinePixel(int pixel);
    void SetFocusedImageID(int imageID);
    
    void AppendTableBox(const DWORD dwId, LPCSTR pText, UITablePanel* pPanel, bool bIsSelected = false, char cHotKey = KEY_RESERVED);
    void LayOutTableBox();
    void ShowPanel(bool isShown);
    UITablePanel* GetShowPanel() const { return m_pShowPanel; }
    void SetInitPanelVisibleStatus(bool visible);
    bool GetInitPanelVisibleStatus() const; 
private:

    UITablePanel* m_pShowPanel;
    UITableBar m_tableBar;
    std::vector<UITablePanel *> m_panels;
    int m_selectedIndex;
    bool m_initPanelVisibleStatus;
};

#endif //_UITABLECONTROLLER_H_
