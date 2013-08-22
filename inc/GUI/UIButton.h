////////////////////////////////////////////////////////////////////////
// UIButton.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UIBUTTON_H__
#define __UIBUTTON_H__

#include "GUI/UIContainer.h"
#include "GUI/UIText.h"
#include "GUI/EventArgs.h"

class UIButton 
    : public UIWindow
{
public:
    enum {
        ICON_TOP = 0,
        ICON_LEFT,
        ICON_RIGHT,
        ICON_BOTTOM,
        ICON_CENTER,
        ICON_CUSTOMIZE
    };
    
    UIButton();
    UIButton(UIContainer* pParent, const DWORD dwId, LPCSTR szText="");

    virtual ~UIButton();

    virtual LPCSTR GetClassName() const
    {
        return ("UIButton");
    }

    virtual void SetAlign(INT32 iAlign);
    virtual int  GetAlign() const { return m_iAlign; }
    
    virtual void SetIcon(SPtr<ITpImage> Icon, int _iIconPos = ICON_TOP);
    virtual void SetIconPos(int _iIconPos = ICON_TOP);
    virtual void ShowIcon(bool _bShowIcon);
    virtual bool IsShowIcon() const { return m_bIsShowIcon; }
    virtual int  GetIconPos() const { return m_iIconPos; }

    virtual INT32 GetTextHeight();

    virtual void SetVAlign(INT32 iValign);
    virtual void SetFontColor(dkColor color);

    virtual const CString& ToString();
    virtual void SetFontSize(int iSize);

    virtual LPCSTR GetText() const;
    virtual void SetText(const CString& rstrText);

    virtual void SetText(int strId);
    virtual void SetText(LPCSTR pszString);

    virtual HRESULT Draw(DK_IMAGE image);


    virtual bool ShowBorder(bool _bIsShowBorder);
    virtual bool IsShowBorder() const { return m_bIsShowBorder; }
    virtual void SetFlashOnClick(bool flashOnClick)
    {
        m_flashOnClick = flashOnClick;
    }
    
protected:
    virtual void OnResize(INT32 iWidth, INT32 iHeight);
    virtual void Init();
    void Dispose();
    ////////////////////Method Section/////////////////////////

private:
    void OnDispose(BOOL bDisposed);
    ////////////////////Method Section/////////////////////////

    ////////////////////Field Section/////////////////////////
protected:
    CString m_strText;
    CString m_strPresentation;

    DKFontAttributes m_ifontAttr;
    
    INT32 m_iValign;
    dkColor m_fontColor;
    bool  m_bIsShowBorder;
    bool  m_bIsShowIcon;
    int   m_iAlign;
    int   m_iIconPos;
    SPtr<ITpImage>  m_Icon;
    bool  m_bIsDisable;
    int   m_iDisabledBackgroundColor;
    int   m_iDisabledFontColor;
    virtual void PerformClick(MoveEvent* moveEvent);
    virtual dkColor GetFontColor() const;
    void DrawHighlight(DK_IMAGE imgSelf);

private:
    BOOL  m_bIsDisposed;
    int   m_preGesture;
    bool m_flashOnClick;

//    UIText m_TxtContent;
    ////////////////////Field Section/////////////////////////
};
#endif
