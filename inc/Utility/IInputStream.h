////////////////////////////////////////////////////////////////////////
// $Source: //depot/grava/private/sclient/src/Taipan/core/inc/Common/Utility/IInputStream.h $ 
// $Revision: 4 $
// Contact: yuzhan
// Latest submission: $Date: 2008/04/17 16:34:21 $ by $Author: fareast\\deyuan $
// Copyright (c) Duokan Corporation. All rights reserved.
// 
//
////////////////////////////////////////////////////////////////////////

#ifndef IINPUTSTREAM_H_
#define IINPUTSTREAM_H_
#include "Public/Base/TPComBase.h"

class IInputStream 
{
public:
    IInputStream() {}
    virtual ~IInputStream() {}

    // Get a unsigned char from input stream.
    // @return -1 for reaching the end of stream
    virtual INT32 ReadUINT8()=0;

    // fill passed-in buffer with given number of char from input stream
    // @return -1 for reaching the end of stream, >0 for number of chars filled into the buffer
    virtual INT32 Read(UINT8* pBuf, UINT32 iLen)=0;

    // fill passed-in buffer from given point with given number of char from input stream
    // @return -1 for reaching the end of stream, >0 for number of chars filled into the buffer
    virtual INT32 Read(UINT8* pBuf, UINT32 iStart, UINT32 iLen)=0;

    // Close this stream, subsequent read will return -1
    virtual void Close()=0;
    
    // Skip iLen bytes from this input stream
    virtual void Skip(UINT32 iLen)=0;
    
    static const int LENGTH_UNKNOWN = -1;
    
    // get total length of this input stream, don't care how many bytes have been read.
    // @return LENGTH_UNKNOWN for not being able to get length from this input stream
    virtual INT32 TotalLength()=0;
};

#endif /*IINPUTSTREAM_H_*/
