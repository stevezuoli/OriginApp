////////////////////////////////////////////////////////////////////////
// $Source: //depot/grava/private/sclient/src/Taipan/core/inc/Utility/IRunnable.h $ 
// $Revision: 2 $
// Contact: yuzhan
// Latest submission: $Date: 2007/10/31 14:17:13 $ by $Author: FAREAST\\yuzhan $
// Copyright (c) Duokan Corporation. All rights reserved.
// 
//
////////////////////////////////////////////////////////////////////////

#ifndef IRUNNABLE_H_
#define IRUNNABLE_H_

class IRunnable {
public:
    virtual ~IRunnable()
    {}
    
    virtual void Run()=0;
};

#endif /*IRUNNABLE_H_*/
