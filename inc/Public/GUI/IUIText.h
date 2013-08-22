////////////////////////////////////////////////////////////////////////
// IUIText.h
// Contact: xzhan
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef IUITEXT_H_
#define IUITEXT_H_

#include "Public/GUI/IUIWindow.h"
#include "Utility/CString.h"

class IUIText : public virtual IUIWindow
{
public:
    virtual void SetAlign(INT32 iAlign) = 0;
    virtual INT32 GetTextHeight() = 0;
    virtual void SetBackGround(INT32 iRGB) = 0;
    virtual void SetTransparent(BOOL bIsTrans) = 0;
    virtual void SetForeGround(INT32 iRGB) = 0;
    virtual void SetHighLight(INT32 iRGB) = 0;
    virtual void SetFont(INT32 iFace, INT32 iStyle, INT32 iSize) = 0;

    virtual LPCSTR GetText() const = 0;
    virtual void SetText(const CString& rstrTxt) = 0;
    virtual ~IUIText() {}
};
#endif /*IUITEXT_H_*/
