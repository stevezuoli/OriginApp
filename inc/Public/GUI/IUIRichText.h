////////////////////////////////////////////////////////////////////////
// IUIRichText.h
// Contact: xzhan
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef IUIRICHTEXT_H_
#define IUIRICHTEXT_H_

#include "Public/GUI/IUIWindow.h"
#include "Utility/IInputStream.h"
#include "Utility/CString.h"

class IUIRichText : public virtual IUIWindow
{
public:
    virtual INT32 GetContentHeight() = 0;
    virtual INT32 GetScrollPos() = 0;
    virtual void SetScrollPos(INT32 iPos) = 0;
    virtual void SetContent(IInputStream* pInputStream) = 0;
    virtual BOOL IsReachBottom() = 0;
    virtual void LineScroll(BOOL bUp) = 0;
    virtual void PageScroll(BOOL bLeft) = 0;
    virtual INT32 GetLineHeight() = 0;
    virtual void AppendText(const CString& strText) = 0;
    virtual void AppendImage(SPtr<ITpImage> spImg) = 0;
    virtual INT32 GetPageHeight() = 0;
    virtual void Clear() = 0;
    virtual ~IUIRichText () {}
};
#endif /*IUIRICHTEXT_H_*/
