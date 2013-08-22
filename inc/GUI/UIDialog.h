////////////////////////////////////////////////////////////////////////
// UIDialog.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UIDIALOG_H__
#define __UIDIALOG_H__

#include "GUI/UIPage.h"

class UIDialog : public UIContainer
{
public:
    UIDialog();
    UIDialog(UIContainer* pParent);

    virtual LPCSTR GetClassName() const
    {
        return ("UIDialog");
    }

    virtual ~UIDialog();

//    static UIDialog* CreateInstance(UIContainer* pParent);
    virtual BOOL OnKeyPress(INT32 iKeyCode);
    virtual BOOL OnPointerPressed(INT32 iX, INT32 iY);
    virtual BOOL OnPointerDragged(INT32 iX, INT32 iY);
    virtual BOOL OnPointerReleased(INT32 iX, INT32 iY);
#if 0
    virtual INT32 ClientToScreenY(INT32 iY);
    virtual INT32 GetAbsoluteX();
    virtual INT32 GetAbsoluteY();
#endif
    virtual void PopupDialog(UIDialog* pDialog);
    virtual INT32 DoModal();
    virtual HRESULT Draw(DK_IMAGE drawingImg);
    virtual void Popup();
    virtual void SetUIPage(UIPage* pPage);
    virtual void SetUIDialog(UIDialog *pDialog);
    virtual void EndDialog(INT32 iEndCode,BOOL fRepaintIt = TRUE);
    BOOL DestroyDialog(BOOL fRepaintIt = TRUE);
    virtual void OnPageSizeChanged();
    BOOL IsEndDialog();
    virtual bool OnChildClick(UIWindow* child);
    
    
protected:
    void Init();
    void Dispose();
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
    void OnChildSetFocus(UIWindow* pWindow);


private:
    void OnDispose(BOOL bDisposed);
    ////////////////////Method Section/////////////////////////

    ////////////////////Field Section/////////////////////////
public:
    enum DialogEndCode
    {
        OK,
        CANCEL
    };

private:
    BOOL m_fIsEndDialog;
    BOOL m_bIsDisposed;
    INT32 m_iEndCode;
    UIDialog* m_pDialog;
protected:
    UIDialog* m_pParentDialog;
private:
    UIPage* m_pPage;
};
#endif
