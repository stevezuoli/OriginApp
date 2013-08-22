////////////////////////////////////////////////////////////////////////
// IUIImage.h
// Contact: xzhan
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef IUIIMAGE_H_
#define IUIIMAGE_H_

#include "Public/GUI/IUIWindow.h"
#include "Utility/IInputStream.h"
#include "GUI/ITpImage.h"

class IUIImage : public virtual IUIWindow
{
public:
    virtual void SetStretch(BOOL bStretch) = 0;
    virtual void SetImage(SPtr<ITpImage> spImg) = 0;

    virtual INT32 GetImageWidth() = 0;
    virtual INT32 GetImageHeight() = 0;
    virtual ~IUIImage() {}
};
#endif /*IUIIMAGE_H_*/
