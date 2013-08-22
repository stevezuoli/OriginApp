////////////////////////////////////////////////////////////////////////
// IUIProgressBar.h
// Contact: xzhan
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef IUIPROGRESSBAR_H_
#define IUIPROGRESSBAR_H_

#include "Public/GUI/IUIWindow.h"

class IUIProgressBar : public virtual IUIWindow
{
public:
    /// <summary>
    /// This element cannot get focus
    /// </summary>
    virtual BOOL CanSetFocus() = 0;

    /// <summary>
    /// Set current progress to progress bar.
    /// This method is to redraw progress bar.
    /// </summary>
    /// <param name="iProgress">
    /// new current value of progress bar, must be >= 0 and <= max progress
    /// </param>
    virtual void SetProgress(INT32 iProgress) = 0;
    
    virtual INT32 GetProgress() = 0;

    /// <summary>
    /// Set the maximum value of this progress bar, default is 100.
    /// This method is to redraw progress bar.
    /// </summary>
    /// <param name="iMax">
    /// new maximum value of the progress bar, must be greater than 0.
    /// </param>
    virtual void SetMaximum(INT32 iMax) = 0;
    
    virtual INT32 GetMaximum() = 0;
    virtual ~IUIProgressBar () {}
};
#endif /*IUIPROGRESSBAR_H_*/
