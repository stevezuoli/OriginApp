////////////////////////////////////////////////////////////////////////
// UINavigation.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UINAVIGATION_H__
#define __UINAVIGATION_H__

#include "GUI/UIImage.h"
#include "GUI/UIButton.h"
#include "GUI/UIPage.h"

class UINavigation : public UIPage
{
    ////////////////////Constructor Section/////////////////////////
public:
    virtual LPCSTR GetClassName() const
    {
        return ("UINaviagation");
    }

    UINavigation();
    virtual ~UINavigation();
    virtual void OnNotify(UIEvent rEvent);
    virtual void Show(BOOL bIsShow, BOOL bIsRepaint = TRUE);

protected:
    void Init();
    void Dispose();
private:
    BOOL m_bIsDisposed;
    UILayout* m_pDefaultLayout;
private:
    void OnDispose(BOOL bIsDisposed);
    void Finalize();
    ////////////////////Field Section/////////////////////////
};
#endif

