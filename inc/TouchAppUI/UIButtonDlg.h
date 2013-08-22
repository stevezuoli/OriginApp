////////////////////////////////////////////////////////////////////////
// UIButtonDlg.h
// Contact:
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UIBUTTONDLG_H__
#define __UIBUTTONDLG_H__

#include "GUI/UIDialog.h"
#include "GUI/UITouchComplexButton.h"
#include "GUI/UITextSingleLine.h"

// class defined in this file
class UIButtonDlg;

class UIButtonDlg : public UIDialog
{
public:
    UIButtonDlg(UIContainer* _pParent);
    ~UIButtonDlg();
    
    virtual LPCSTR GetClassName() const
    {
        return "UIButtonDlg";
    }

    virtual void    MoveWindow(int _iLeft, int _iTop, int _iWidth, int _iHeight);
    virtual void    OnCommand(DWORD _dwCmdId, UIWindow* _pSender, DWORD _dwParam);
    virtual HRESULT Draw(DK_IMAGE drawingImg);


    virtual bool AppendButton(DWORD _dwId, LPCSTR _szText, INT32 _iFontSize, SPtr<ITpImage> Icon = SPtr<ITpImage>());

    virtual bool SetSelectedButtonByIndex(int _iSelectedButton);
    virtual bool SetSelectedButtonById(DWORD _dwCmdId);
    virtual int  GetSelectedButtonIndex() const { return m_iSelectedButton; }
    virtual int  GetSelectedButtonID()    const;
    
    virtual bool SetItemMargin(int _iItemMargin);

    virtual bool SetButtonHeight(int _iHeight);
    virtual int  GetButtonHeight() const { return m_iButtonHeight; }
    virtual int  GetTotalHeight() const;

    virtual bool SetNumPerPage(int _iNumPerPage);
    virtual bool SetTextAlign(int _iAlign);
    virtual bool OnHookTouch(MoveEvent* moveEvent);
    virtual bool OnChildClick(UIWindow* child);
    void SetData(const std::vector<std::string>& ss, int selectedIndex);
    void SetButtonFontSize(int btnFontSize);
    int CalculateTop(const UIButton* button);
    enum ButtonId
    {
        BI_Left = 10000,
        BI_Right,
        BI_Base
    };
    virtual HRESULT UpdateWindow();
    
private:
    void UpdatePageNum();
    void SetSelectedButtonIndex(int index);
    void UpdateButtons();
    void UpdatePageButtonIndexRange();

private:
    int m_iSelectedButton;
    int m_iItemMargin;
    int m_iButtonHeight;
    unsigned int m_iNumPerPage;
    int m_iCurPage;
    int m_iTotalPage;
    int m_iTextAlign;
    int m_iCurPageStartIndex;
    int m_iCurPageEndIndex;

    UISizer* m_pBottomSizer;
    UISizer* m_pButtonSizer;
    UITouchComplexButton m_btnLeft;
    UITextSingleLine m_txtPageNum;
    UITouchComplexButton m_btnRight;
    int m_btnFontSize;
    int m_vertSpacingFromButtonAttached;
};

#endif //__UIBUTTONDLG_H__

