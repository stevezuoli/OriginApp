////////////////////////////////////////////////////////////////////////
// UITouchButton.h
// Contact: chenm
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////


#ifndef __UITouchButton_H__
#define __UITouchButton_H__

#include "GUI/UIButton.h"
#include "GUI/FontManager.h"
#include "Utility/ColorManager.h"

class UITouchButton : public UIButton
{
public:
    UITouchButton(UIContainer* pParent, const DWORD dwId);
    UITouchButton();
    ~UITouchButton();

    virtual LPCSTR GetClassName() const
    {
        return ("UITouchButton");
    }

    virtual void Initialize(DWORD dwId, LPCSTR szText, INT32 iFontSize, SPtr<ITpImage> pBackground, SPtr<ITpImage> pFocusedBackground);
    virtual void Initialize(DWORD dwId, LPCSTR szText, INT32 iFontSize, int iBackgroundColor = ColorManager::knWhite, int iFocuedBackgroundColor = ColorManager::knBlack);

    virtual dkSize GetMinSize() const;

    virtual int GetFocusedFontColor();
    virtual void SetFocusedFontColor(int _fontColor);

    virtual SPtr<ITpImage> GetBackground();
    virtual void SetBackground(SPtr<ITpImage> pImageBackground);

    virtual SPtr<ITpImage> GetFocusedBackground();
    virtual void SetFocusedBackground(SPtr<ITpImage> pImageFocusedBackground);

    virtual HRESULT Draw(DK_IMAGE drawingImg);

    virtual dkColor GetFocusedBackgroundColor() const
    {
        return m_focusedBackgroundColor;
    }
    virtual void SetFocusedBackgroundColor(dkColor color);
    virtual void SetBackTransparent(BOOL bIsTransparent);
#ifndef KINDLE_FOR_TOUCH
    virtual HRESULT UpdateFocus();
#endif

    virtual BOOL OnKeyPress(INT32 iKeyCode);

    virtual int GetTextWidth();
    virtual int GetTextHeight();
    virtual void SetDo2Gray(bool do2gray);

protected:
    virtual void Init();

protected:
    int           m_iFocusedFontColor;

    dkColor           m_focusedBackgroundColor;

    bool            m_bBackTransparent;

    SPtr<ITpImage>   m_spBackground;
    SPtr<ITpImage>   m_spFocusedBackground;
    
    bool        m_bUsingBackgroundPicture;
    bool m_do2gray;
};

#endif //__UITouchButton_H__

