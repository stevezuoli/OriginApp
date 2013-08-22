////////////////////////////////////////////////////////////////////////
// UIProgressBar.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UIPROGRESSBAR_H__
#define __UIPROGRESSBAR_H__

#include "GUI/UIWindow.h"
#include "GUI/UIContainer.h"

class UIProgressBar : public UIWindow
{
public:
    virtual LPCSTR  GetClassName() const
    {
        return ("UIProgressBar");
    }

    UIProgressBar();
    UIProgressBar(UIContainer* pParent, const DWORD dwId);

    virtual ~UIProgressBar();
    
    /// <summary>
    /// This element cannot get focus
    /// </summary>
    virtual BOOL CanSetFocus();

    /// <summary>
    /// Set current progress to progress bar.
    /// This method is to redraw progress bar.
    /// </summary>
    /// <param name="iProgress">
    /// new current value of progress bar, must be >= 0 and <= max progress
    /// </param>
    virtual void SetProgress(INT32 iProgress);
    
    virtual INT32 GetProgress();

    /// <summary>
    /// Set the maximum value of this progress bar, default is 100.
    /// This method is to redraw progress bar.
    /// </summary>
    /// <param name="iMax">
    /// new maximum value of the progress bar, must be greater than 0.
    /// </param>
    virtual void SetMaximum(INT32 iMax);
    
    virtual INT32 GetMaximum();

    virtual HRESULT Draw(DK_IMAGE drawingImg);

    void SetBarHeight(INT32 iHeight);
    INT32 GetBarHeight() const;
	int GetBarLength() const;
    BOOL GetDrawBold();
    void SetDrawBold(BOOL fDrawBold);
    
protected:
    virtual void OnResize(INT32 iWidth, INT32 iHeight);
    void Init();
    void Dispose();

private:
    void RecalcBarLength();
    void OnDispose(BOOL bIsDisposed);
    ////////////////////Method Section/////////////////////////

    ////////////////////Field Section/////////////////////////
private:
    BOOL m_bIsDisposed;

protected:
	/// max value possible for the progress bar
    INT32 m_iMax;

    /// current value of the progress bar
    INT32 m_iCurrent;

    /// store progress bar length in pixel
    INT32 m_iBarLengthInPixel;

    INT32 m_iBarHeightInPixel;
    BOOL m_fDrawBold;
    ////////////////////Field Section/////////////////////////
};
#endif
