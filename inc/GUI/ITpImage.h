////////////////////////////////////////////////////////////////////////
// $Source: //depot/grava/private/sclient/src/Taipan/porting/inc/GUI/ITpImage.h $
// $Revision: 5 $
// Contact: genchw, yuzhan
// Latest submission: $Date: 2007/11/09 20:53:04 $ by $Author: fareast\\genchw $
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __ITPIMAGE_H__
#define __ITPIMAGE_H__

#include "Utility/BasicType.h"
#include <stdint.h>

class ITpGraphics;

class ITpImage
{
    ////////////////////Constructor Section/////////////////////////
public:
    unsigned long  bmWidth;
    unsigned long  bmHeight;
    unsigned long  bmPitch;
    uint8_t*  bmBits;
public:
     ITpImage()
        :bmWidth(0)
        ,bmHeight(0)
        ,bmPitch(0)
        ,bmBits(NULL)
    {
        // empty
    }
    virtual ~ITpImage()
    {
        // empty
    }
    ////////////////////Constructor Section/////////////////////////

    ////////////////////Method Section/////////////////////////
public:
    virtual int32_t GetWidth() const = 0;
    virtual int32_t GetHeight() const = 0;
    ////////////////////Method Section/////////////////////////
};

#endif
