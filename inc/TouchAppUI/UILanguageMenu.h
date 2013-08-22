#ifndef __UILANGUAGEMENU_H__
#define __UILANGUAGEMENU_H__

#include "GUI/UIDialog.h"
#include "GUI/MenuItem.h"
#include "DkSPtr.h"
#include "GUI/UIMenu.h"

#include "Public/Base/TPComBase.h"

//using namespace DkFormat;

class UILanguageMenu
    : public UIMenu
{
public:
    UILanguageMenu(UIWindow* pEventHandler,  DWORD Id);
    virtual ~UILanguageMenu();
    virtual void MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight);
    virtual HRESULT Draw(DK_IMAGE drawingImg);
    virtual BOOL OnKeyPress(INT32 iKeyCode);
    virtual void PopupMenu(INT32 iX, INT32 iY, INT32 iScreenWidth, INT32 iScreenHeight);
    virtual void ClickMenuItem();
    
private:
    void Dispose();
    void OnDispose(BOOL bIsDisposed);
    void Finalize();
    INT32 GetMenuItemSelectLine(int &x,int &y,int &w,int &h,int indexItem);
    BOOL DrawSelectItem(INT32 iOldItem);
    void DrawBackground(DK_IMAGE drawingImg);
    INT32 DrawMenuItem(DK_IMAGE drawingImg, MenuItem* pItem, INT32 iItemIndex, INT32 iMenuItemX, INT32 iMenuItemY);

public:
    int m_recentIndex[3];
    int m_recentCount;

private:
    int m_iFirstItem;
    int m_iTempSelectItem;
    int m_iPage;
    int m_iCurPage;
    int m_isLArrow;
    int m_isRArrow;
    int m_menuId;
    BOOL m_bIsDisposed;

};

// TODO: 需要跟UILanguageMenu做合并
class UIPopMenu : public UIMenu
{
public:
    UIPopMenu(UIWindow* pEventHandler,  DWORD Id);
    virtual ~UIPopMenu();

    virtual LPCSTR GetClassName() const
    {
        return ("UIPopMenu");
    }

    virtual void MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight);
    virtual HRESULT Draw(DK_IMAGE drawingImg);
    virtual BOOL OnKeyPress(INT32 iKeyCode);
    virtual void PopupMenu(INT32 iX, INT32 iY, INT32 iScreenWidth, INT32 iScreenHeight);
    virtual void ClickMenuItem();
    
private:
    void Dispose();
    void OnDispose(BOOL bIsDisposed);
    void Finalize();
    INT32 GetMenuItemSelectLine(int &x,int &y,int &w,int &h,int indexItem);
    BOOL DrawSelectItem(INT32 iOldItem);
    void DrawBackground(DK_IMAGE drawingImg);
    INT32 DrawMenuItem(DK_IMAGE drawingImg, MenuItem* pItem, INT32 iItemIndex, INT32 iMenuItemX, INT32 iMenuItemY);

private:
    int m_iFirstItem;
    int m_iTempSelectItem;
    int m_iPageCount;
    int m_iCurPage;
    int m_isLArrow;
    int m_isRArrow;
    int m_menuId;
    BOOL m_bIsDisposed;
    
};

#endif

