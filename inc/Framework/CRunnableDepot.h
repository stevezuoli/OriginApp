////////////////////////////////////////////////////////////////////////
// $Source: //depot/grava/private/sclient/src/Taipan/core/inc/Framework/CRunnableDepot.h $ 
// $Revision: 3 $
// Contact: yuzhan
// Latest submission: $Date: 2007/11/30 11:00:51 $ by $Author: fareast\\genchw $
// Copyright (c) Duokan Corporation. All rights reserved.
// 
//
////////////////////////////////////////////////////////////////////////

#ifndef CRUNNABLEDEPOT_H_
#define CRUNNABLEDEPOT_H_

#include "Utility/IRunnable.h"
#include "Utility/BasicType.h"
#include "DkSPtr.h"

//using namespace DkFormat;

using DkFormat::SPtr;

class CRunnableDepot
{
public:
    static void Initialize();
    static void Finalize();
    static INT32 RegisterRunnable(SPtr<IRunnable> spRunnable);
    static SPtr<IRunnable> GetRunnableAndDelete(INT32 iID);
    
private:
    static const INT32 DEPOT_SIZE = (1 << 9);
    static SPtr<IRunnable> s_RunnableDepot[DEPOT_SIZE];
    static INT32 s_CurrentSlot;
};
#endif /*CRUNNABLEDEPOT_H_*/
