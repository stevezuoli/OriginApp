////////////////////////////////////////////////////////////////////////
// $Source: //depot/grava/private/sclient/src/Taipan/core/src/Framework/CRunnableDepot.cpp $ 
// $Revision: 3 $
// Contact: yuzhan
// Latest submission: $Date: 2008/04/25 13:22:12 $ by $Author: fareast\\genchw $
// Copyright (c) Duokan Corporation. All rights reserved.
// 
//
////////////////////////////////////////////////////////////////////////

//#include "Framework/Framework.h"
#include "Framework/CRunnableDepot.h"


SPtr<IRunnable> CRunnableDepot::s_RunnableDepot[CRunnableDepot::DEPOT_SIZE];
INT32 CRunnableDepot::s_CurrentSlot = 0;

void CRunnableDepot::Initialize()
{
}

void CRunnableDepot::Finalize()
{
    for (INT32 i = 0; i < DEPOT_SIZE; i++)
    {
        if (s_RunnableDepot[i])
            s_RunnableDepot[i].Clear();
    }
}

INT32 CRunnableDepot::RegisterRunnable(SPtr<IRunnable> spRunnable)
{
    TP_ASSERT(spRunnable);
    
    // find an empty slot to store IRunnable
    for (INT32 index = s_CurrentSlot + 1; index != s_CurrentSlot; index = (index + 1) & (DEPOT_SIZE - 1))
    {
        if (!s_RunnableDepot[index])
        {
            s_CurrentSlot = index;
            s_RunnableDepot[index] = spRunnable;
            return index;
        }
    }

    TRACE("The runnalbe depot is full.");
    //TP_ASSERT(FALSE);
    return -1;
}

SPtr<IRunnable> CRunnableDepot::GetRunnableAndDelete(INT32 iID)
{
    TP_ASSERT(iID < DEPOT_SIZE && iID > 0);
    TP_ASSERT(s_RunnableDepot[iID]);
    
    SPtr<IRunnable> spRunnable = s_RunnableDepot[iID];
    
    s_RunnableDepot[iID].Clear();

    return spRunnable;    
}
