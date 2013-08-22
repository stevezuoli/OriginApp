////////////////////////////////////////////////////////////////////////
// IUIButton.h
// Contact: xzhan
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef IUIBUTTON_H_
#define IUIBUTTON_H_

#include "Public/GUI/IUIWindow.h"
#include "Utility/CString.h"

class IUIButton : public virtual IUIWindow
{
public:
    virtual void SetAlign(INT32 iAlign) = 0;
    virtual void SetVAlign(INT32 iValign) = 0;
    virtual void SetFontColor(INT32 iRGB) = 0;
    virtual void SetFontSize(int size) = 0;
    
    virtual LPCSTR GetText() const =0;
    virtual void SetText(const CString& rstrText)=0;

    virtual ~IUIButton () {}
};

#endif /*IUIBUTTON_H_*/
