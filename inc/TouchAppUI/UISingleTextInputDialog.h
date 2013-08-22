///////////////////////////////////////////////////////////////////////
// UISingleTextInputDialog.h
// Contact: liuhj
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#ifndef __UISingleTextInputDialog_H__
#define __UISingleTextInputDialog_H__

#include "GUI/UIButton.h"
#include "GUI/UITouchComplexButton.h"
#include "GUI/UIDialog.h"
#include "GUI/UIImage.h"
#include "Utility/ImageManager.h"
#include "GUI/UITextBox.h"
#include "GUI/UITextSingleLine.h"
#include "Utility/ColorManager.h"
#include "DkSPtr.h"

//using namespace DkFormat;

enum DialogType
{
    Type_Default,        //默认密码输入样式
    Type_AddLabel        //添加书籍标签样式
};

class UISingleTextInputDialog : public UIDialog
{
public:
    virtual LPCSTR GetClassName() const
    {
        return "UISingleTextInputDialog";
    }

    UISingleTextInputDialog(UIContainer* pParent, DialogType type = Type_Default, BOOL bSupportFont = FALSE);
    virtual ~UISingleTextInputDialog();
    virtual void MoveWindow(int _iLeft, int _iTop, int _iWidth, int _iHeight);
    BOOL OnKeyPress(INT32 iKeyCode);
    void SetDialogTitle (const char* dialogText);
    void InitPassword (CString text);
    void SetMaxInputLen (INT32 iMaxLen);
    void EnableIME(BOOL bEnableIME)    {    m_bSupportFont = bEnableIME;    }
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    std::string GetInputText() const
    {
        return m_password;
    }
    
protected:
    void Dispose ();
    virtual HRESULT DrawBackGround (DK_IMAGE drawingImg);

private:
    void InitUI ();
    void OnDispose (BOOL bDisposed);


    ////////////////////Field Section/////////////////////////

private:
    static const int m_DEFAULT_MARGIN;
    static const int m_DEFAULT_WIDTH;
    static const int m_DEFAULT_HEIGHT;

    static const int m_DEFAULT_ELEMHEIGHT;
    static const int m_DEFAULT_FONTSIZE;
    static const int m_DEFAULT_ELEMSPACING;

    UITouchComplexButton      m_btnOK;
    UITouchComplexButton      m_btnCancel;
    UITextSingleLine     m_txtPasswordHintLabel;
    UITextBox            m_tbPassword;
    UITextSingleLine     m_txtLabelHint;
    BOOL                m_bIsDisposed;

    std::string              m_password;

    BOOL                m_bSupportFont;

    DialogType          m_iDialogType;
};

#endif

