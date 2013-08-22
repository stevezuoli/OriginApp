#ifndef __GUI_UILISTITEMBASE_H__
#define __GUI_UILISTITEMBASE_H__

#include "GUI/UICompoundListBox.h"
#include "BookStore/BookStoreInfoManager.h"

class UIListItemBase: public UIContainer
{
public:
    UIListItemBase();
    UIListItemBase(UICompoundListBox* pParent, const DWORD dwId, BOOL bBubbleNotify);
    UIListItemBase(UICompoundListBox* pParent, const DWORD dwId);
    virtual LPCSTR GetClassName()
    {
        return "UIListItemBase";
    }
    ~UIListItemBase();

    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
    virtual HRESULT UpdateFocus();
    virtual void SetHighLight(bool highLight, bool updateNow = true);
    virtual void SetShowBottomLine(bool showBottomLine);
    virtual bool ResponseTouchTap();
    virtual bool ResponseOperation() { return true; }
    virtual void UpdateChildStatus() {}
    virtual void SetInfoSPtr(dk::bookstore::model::BasicObjectSPtr info, int index = -1) {}
	void DrawHighlight(DK_IMAGE drawingImg);
    virtual HRESULT Draw(DK_IMAGE drawingImg);
private:
	void Init();

private:
    bool m_bIsHighLight;
    bool m_showBottomLine;
};
#endif// #ifndef __GUI_UILISTITEMBASE_H__
