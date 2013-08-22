////////////////////////////////////////////////////////////////////////
// UIButton.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UILINEEDIT_H__
#define __UILINEEDIT_H__

#include "GUI/UIDialog.h"
#include "GUI/UIComplexButton.h"
#include "GUI/UITextBox.h"

class UIComplexDialog : public UIDialog
{
public:
    UIComplexDialog();
    UIComplexDialog(UIContainer* pParent, const DWORD dwId);
    UIComplexDialog(UIContainer* pParent, const DWORD dwId, bool showIME);
    virtual ~UIComplexDialog();

    virtual LPCSTR GetClassName() const
    {
        return ("UIComplexDialog");
    }

    virtual void SetEnterBtnText(const CString& strText);
    virtual INT32 DoModal();
    void SetFont(INT32 iFace, INT32 iStyle, INT32 iSize);
    virtual HRESULT Draw(DK_IMAGE image);
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
    BOOL OnKeyPress(INT32 iKeyCode);
    std::string GetTextUTF8();
    INT32 SetTextUTF8(const CHAR *strText, INT32 iStrLen = 0);
    void SetTipUTF8(const CHAR *strText, INT32 iStrLen = 0);
    void SetImage(SPtr<ITpImage> _spImage);
    void OnIMEShow(void* pSender);
    void OnIMEHide(void* pSender);
    void OnIMEChange(BOOL fIsShow);
    void SetNumberOnlyMode();
    virtual bool OnHookTouch(MoveEvent* moveEvent);
    virtual bool OnChildClick(UIWindow* child);
    virtual void OnCommand(DWORD cmdId, UIWindow* sender, DWORD wParam);
protected:
    virtual void Init();
    UIWindow* m_pEventHandler;
    DWORD      m_dwId;
    UIImage             *m_pImage;
    UITouchComplexButton     *m_pBtnEnter;
    UITextBox           *m_pTextBox;
private:
    DKFontAttributes    m_ifontAttr;
    INT32               m_iFontColor;
	bool				m_showIME;
};
#endif
