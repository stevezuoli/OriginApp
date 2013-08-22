////////////////////////////////////////////////////////////////////////
// $Source: //depot/grava/private/sclient/src/Taipan/core/inc/Utility/IOutputStream.h $ 
// $Revision: 4 $
// Contact: yuzhan
// Latest submission: $Date: 2008/08/14 17:49:36 $ by $Author: fareast\\xzhan $
// Copyright (c) Duokan Corporation. All rights reserved.
// 
//
////////////////////////////////////////////////////////////////////////
#ifndef IOUTPUTSTREAM_H_
#define IOUTPUTSTREAM_H_

#include "BasicType.h"
#include "Public/Base/TPComBase.h"

TAIPAN_SHARED_INTERFACE(IOutputStream, "2D02D47A-3D51-42d6-BEFA-A026B0C96804")
: public IUnknown
{
    virtual void Close()=0;

    virtual void Flush()=0;

    virtual void Write(UINT8* pBuf, INT32 iLen)=0;

    virtual void Write(UINT8 iChar)=0;
};

#endif /*IOUTPUTSTREAM_H_*/
