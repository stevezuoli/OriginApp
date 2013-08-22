////////////////////////////////////////////////////////////////////////
// $Source: //depot/grava/private/sclient/src/Taipan/porting/inc/Framework/INativeDisplay.h $
// $Revision: 3 $
// Contact: yuzhan
// Latest submission: $Date: 2007/10/30 14:45:09 $ by $Author: FAREAST\\yuzhan $
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __INATIVEDISPLAY_H__
#define __INATIVEDISPLAY_H__

#include "Utility/BasicType.h"
#include "drivers/CFbBitDc.h"

namespace INativeDisplay
{
    void SetScreenDC(CFbBitDc *pScreenDC);
    INT32 GetScreenWidth();
    INT32 GetScreenHeight();
    CFbBitDc* GetScreenGC();
};

#endif /*INATIVEDISPLAY_H_*/
