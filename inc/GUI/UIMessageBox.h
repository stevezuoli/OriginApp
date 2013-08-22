////////////////////////////////////////////////////////////////////////
// UIMessageBox.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UIMESSAGEBOX_H__
#define __UIMESSAGEBOX_H__
#include "GUI/UIDialog.h"
#include "GUI/UIText.h"
#include "GUI/UIImage.h"
#include "DkSPtr.h"
#ifdef KINDLE_FOR_TOUCH
#include "GUI/UITouchComplexButton.h"
#else
#include "GUI/UIComplexButton.h"
#endif
#include "GUI/UIButtonGroup.h"
#include <string>

#define MB_TIPMSG                    0x0
#define MB_OK                        0x1
#define MB_CANCEL                    0x2
#define MB_YES                       0x4
#define MB_NO                        0x8
#define MB_CLOSE                     0x10
#define MB_WITHOUTBTN                0x20

class UIMessageBox : public UIDialog
{
public:
    virtual const char* GetClassName() const
    {
        return "UIMessageBox";
    }

    UIMessageBox();
    UIMessageBox(UIContainer* pParent, const char* szText, int iType=MB_OK);

    virtual ~UIMessageBox();
    virtual int DoModal();
	virtual void Popup();
    virtual void SetText(const char* szText);
	virtual UIText* GetTextDisplay();
	virtual UIText* GetTextTip();
    virtual const char* GetText() const;
	virtual void SetTipText(const char* tipText);
    virtual BOOL OnKeyPress(int iKeyCode);
    virtual void SetFocusButton(int id);
    void SetButtonText(int id, const char* szText);
    void OnCommand(DWORD _dwCmdId, UIWindow* _pSender, DWORD _dwParam);
	void SetEnableHookTouch(bool isEnable);

protected:
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
    void Init();
    void Dispose();
    void RegisterTouchEvent(bool _bBlockLowerItems, bool _bModal);

private:
    void OnDispose(BOOL bDisposed);
    void InitMember();
    void InitUI();
    void InitSizer();
	void MakeCenterAndLayout();
    void Finalize();
    void InitFocusedObj();
    bool AddButtonForType(int type);

private:
    int m_iType;
    bool m_bIsDisposed;
    UIText m_textDisplay;
	UIText m_textTip;
    UIButtonGroup m_btnItems;
    int m_iSelectItems;
};
#endif
