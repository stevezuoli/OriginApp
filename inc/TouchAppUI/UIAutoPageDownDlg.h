////////////////////////////////////////////////////////////////////////
// UIAutoPageDownDlg.h
// Contact: liuhj
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __APPUI_UIAUTOPAGEDOWNDLG_H__
#define __APPUI_UIAUTOPAGEDOWNDLG_H__

#include "TouchAppUI/UIComplexDialog.h"
#include "GUI/UITextSingleLine.h"

class UIAutoPageDownDlg : public UIComplexDialog
{
public:
    UIAutoPageDownDlg();
    UIAutoPageDownDlg(UIContainer* pParent, const DWORD dwId);
    virtual ~UIAutoPageDownDlg();

    virtual LPCSTR GetClassName() const
    {
        return ("UIAutoPageDownDlg");
    }

    void Initialize ();
    virtual BOOL OnKeyPress(INT32 iKeyCode);

protected:
    virtual void Init();
    virtual HRESULT Draw(DK_IMAGE image);
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);

private:
    BOOL m_fInited;
    UITextSingleLine m_txtHeadTitle;
    UITextSingleLine m_txtTailTitle;
};
#endif
