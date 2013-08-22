
////////////////////////////////////////////////////////////////////////
// $Source: //depot/grava/private/sclient/src/Taipan/src/Framework/INativeDisplay.cpp $
// $Revision: 9 $
// Contact: yuzhan
// Latest submission: $Date: 2008/04/18 22:39:09 $ by $Author: fareast\\genchw $
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "Framework/INativeDisplay.h"
#include "Framework/CNativeThread.h"


static CFbBitDc *s_pScreenDC = NULL;

void INativeDisplay::SetScreenDC(CFbBitDc *pScreenDC)
{
      s_pScreenDC = pScreenDC;
}

INT32 INativeDisplay::GetScreenWidth()
{
    if (s_pScreenDC)
    {
        return s_pScreenDC->GetWidth();
    }
    else
    {
        return 0;
    }
}

INT32 INativeDisplay::GetScreenHeight()
{
    if (s_pScreenDC)
    {
        return s_pScreenDC->GetHeight();
    }
    else
    {
        return 0;
    }
}

CFbBitDc* INativeDisplay::GetScreenGC()
{
    return s_pScreenDC;
}

