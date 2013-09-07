/* -------------------------------------------------------------------------
//  File Name   :   NptTransferStatusHandler.h
//  Author      :   Zhang Fan
//  Create Time :   2013-1-28 15:27:29
//  Description :
//
// -----------------------------------------------------------------------*/

#ifndef __TRANSFERSTATUSHANDLER_H__
#define __TRANSFERSTATUSHANDLER_H__

//-------------------------------------------------------------------------

struct NPT_TransferStatusHandler
{
    virtual NPT_Interface OnProgress(NPT_Int64 total, NPT_Int64 now, double speed_kb) = 0;
    virtual NPT_Interface OnSuccess() = 0;
    virtual NPT_Interface OnFail() = 0;
    virtual NPT_Interface OnCancelLocal() = 0;
    virtual NPT_Interface OnCancelRemote() = 0;
    virtual NPT_Interface OnCancelExit() = 0;

    virtual bool IsCancelLocal() = 0;
    virtual bool IsCancelRemote() = 0;
    virtual bool IsCancelExit() = 0;
};

//--------------------------------------------------------------------------
#endif /* __TRANSFERSTATUSHANDLER_H__ */
