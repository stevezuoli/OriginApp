////////////////////////////////////////////////////////////////////////
// IUITextSingleLine.h
// Contact: xzhan
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef IUITEXTSINGLELINE_H_
#define IUITEXTSINGLELINE_H_

#include "Public/GUI/IUIWindow.h"
#include "Utility/CString.h"

class IUITextSingleLine : public virtual IUIWindow
{
public:
    virtual void SetAlign(INT32 iAlign) = 0;
    virtual void SetFocus(BOOL bIsFocus) = 0;
    virtual void SetForeColor(INT32 iColor) = 0;
    virtual void StopScroll() = 0;
    virtual void StartScroll() = 0;
    virtual void SetFontFace(INT32 iFace) = 0;
    virtual void SetFontStyle(INT32 iStyle) = 0;
    virtual void SetBackColor(INT32 iColor) = 0;
    virtual void SetBackTransparent(BOOL bIsTransparent) = 0;

    virtual LPCSTR GetText() const = 0;
    
    virtual void SetText(const CString& rstrTxt) = 0;
    virtual void SetFontSize(INT32 iSize) = 0;
    virtual ~IUITextSingleLine () {}
};

#endif /*IUITEXTSINGLELINE_H_*/
