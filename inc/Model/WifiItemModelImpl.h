////////////////////////////////////////////////////////////////////////
// CBookItemModelImpl.h
// Contact: zhangxb
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __WIFIITEMMODELIMPL_H__
#define __WIFIITEMMODELIMPL_H__

#include "Model/IWifiItemModel.h"

class CWifiItemModelImpl : public IWifiItemModel
{
public:
    CWifiItemModelImpl();
    
    void SetSSID(string& strSSID);
    string& GetSSID();
        
    void SetQualityLv(int value);
    int GetQualityLv();
    
    void SetIsProtected(BOOL fIsProtected);
    BOOL GetIsProtected();
    
    void SetIsConn(BOOL fIsConn);
    BOOL GetIsConn();
    
    void SetStateMess(string& strMess );
    string& GetStateMess();
    
private:
    string m_strSSID;
    INT32 m_iQualityLv;
    BOOL m_fIsProtected;
    BOOL m_fIsConn;
    string m_strStateMess;
};

#endif  // __WIFIITEMMODELIMPL_H__



