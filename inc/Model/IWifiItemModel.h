////////////////////////////////////////////////////////////////////////
// IBookItemModel.h
// Contact: zhangxb
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __IWIFIITEMMODEL_H__
#define __IWIFIITEMMODEL_H__

#include "dkBaseType.h"
#include <string>
using std::string;
class IWifiItemModel
{
public:
    
    virtual ~IWifiItemModel() {}

    virtual string& GetSSID() = 0;
        
    virtual int GetQualityLv() = 0;
    
    virtual BOOL GetIsProtected() = 0;
    
    virtual BOOL GetIsConn() = 0;
    
    virtual string& GetStateMess() = 0;

};

#endif  // __IWIFIITEMMODEL_H__


