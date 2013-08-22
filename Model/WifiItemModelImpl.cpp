////////////////////////////////////////////////////////////////////////
// CBookItemModelImpl.h
// Contact: chenm
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "Model/WifiItemModelImpl.h"
#include "I18n/StringManager.h"

CWifiItemModelImpl::CWifiItemModelImpl()
{
    this->m_fIsConn = FALSE;
    this->m_fIsProtected = FALSE;
    this->m_iQualityLv = 0;
    this->m_strSSID.clear();
    this->m_strStateMess.clear();
}

void CWifiItemModelImpl::SetSSID(string& strSSID)
{
    this->m_strSSID = strSSID;  
}
string& CWifiItemModelImpl::GetSSID()
{
    return this->m_strSSID;
}

void CWifiItemModelImpl::SetQualityLv(int value)
{
    this->m_iQualityLv = value;
}
int CWifiItemModelImpl::GetQualityLv()
{
    return this->m_iQualityLv;
}
void CWifiItemModelImpl::SetIsProtected(BOOL fIsProtected)
{
    this->m_fIsProtected = fIsProtected;
}
BOOL CWifiItemModelImpl::GetIsProtected()
{
    return this->m_fIsProtected;
}

void CWifiItemModelImpl::SetIsConn(BOOL fIsConn)
{
    this->m_fIsConn = fIsConn;
}
BOOL CWifiItemModelImpl::GetIsConn()
{
    return this->m_fIsConn;
}

void CWifiItemModelImpl::SetStateMess(string& strMess )
{
    this->m_strStateMess = strMess;
}
string& CWifiItemModelImpl::GetStateMess()
{
    static string str;
    if(m_strStateMess.size() > 0)
    {
        return this->m_strStateMess;
    }
    else if(this->m_fIsConn)
    {
        str = StringManager::GetStringById(CONNECT_ALREADY);
        return str;
    }
    else
    {
        str = "";
        return str;
    }

}



