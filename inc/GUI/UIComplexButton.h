////////////////////////////////////////////////////////////////////////
// UIComplexButton.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////


#ifndef __UICOMPLEXBUTTON_H__
#define __UICOMPLEXBUTTON_H__

#include "GUI/UIButton.h"
#include "GUI/FontManager.h"

class UIComplexButton : public UIButton
{
public:
    UIComplexButton(UIContainer* pParent, const DWORD dwId);
    UIComplexButton();
    ~UIComplexButton();

    virtual LPCSTR GetClassName() const
    {
        return "UIComplexButton";
    }

#ifdef KINDLE_FOR_TOUCH
    virtual void Initialize(DWORD dwId, LPCSTR szText, INT32 iFontSize, SPtr<ITpImage> Icon = SPtr<ITpImage>());
    virtual void ShowDottedLine(bool _bIsShowDotted);
#else
    void Initialize(DWORD dwId, LPCSTR szText, char chHotKey, INT32 iFontSize, SPtr<ITpImage> Icon);
#endif

    virtual void Init();

#ifndef KINDLE_FOR_TOUCH
    virtual char GetHotKey();
    virtual void SetHotKey(char key);
#endif
	virtual void SetBackGroundImage(INT32 bg);

    bool SetBorder(int _iBorder);
    int  GetBorder() const;
    
    virtual int GetUnderLineWidth();
    virtual void SetUnderLineWidth(int width);

    virtual int GetUnderLineHeight();
    virtual void SetUnderLineHeight(int  height);

    virtual int  GetSelectLineWidth();
    virtual void SetSelectLineWidth(int  width);

    virtual int GetProcess();
    virtual void SetProcess(int porcess);

    virtual SPtr<ITpImage> GetIcon();
    virtual void SetIcon(SPtr<ITpImage> Icon);

    virtual void SetRightIcon(SPtr<ITpImage> spIcon);

    virtual HRESULT Draw(DK_IMAGE drawingImg);

    virtual HRESULT UpdateFocus();

    virtual void SetBackColor(INT32 iColor);
    virtual void SetBackTransparent(BOOL bIsTransparent);
    virtual BOOL OnKeyPress(INT32 iKeyCode);

protected:
	virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);

protected:
    INT32           m_iBackColor;
    bool            m_bBackTransparent;
#ifndef KINDLE_FOR_TOUCH
    char            m_chHotKey;
#endif
    int             m_UnderLineWidth;
    int             m_UnderLineHeight;
    int             m_Progreess;
    int             m_SelectLineWidth;
    SPtr<ITpImage>  m_spRightIcon;
    int             m_iIconSize;
    bool            m_fIsDrawn;
    int             m_iBorder;
    bool            m_bIsShowDotted;

	int  			m_iBackGroundImage;
private:
    HRESULT UpdateUI(DK_IMAGE imgParent, BOOL fDrawFocusOnly, DK_RECT *pParentRect);

};
#endif

