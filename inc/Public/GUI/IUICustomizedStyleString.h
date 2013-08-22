////////////////////////////////////////////////////////////////////////
// IUICustomizedStyleString.h
// Contact: xzhan
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef IUICUSTOMIZEDSTYLESTRING_H_
#define IUICUSTOMIZEDSTYLESTRING_H_

#include "Public/GUI/IUIWindow.h"
#include "Uitlity/CString.h"

class IUICustomizedStyleString : public virtual IUIWindow
{
public:   
    LPCSTR GetText() const = 0;
    void SetText(const CString& rstrText) = 0;
    
    virtual void SetSpace(INT32 iSpace) = 0;
    virtual ~IUICustomizedStyleString () {}
};
#endif /*IUICUSTOMIZEDSTYLESTRING_H_*/
