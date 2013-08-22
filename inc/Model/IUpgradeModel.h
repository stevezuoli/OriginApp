#ifndef __IUPGRADEMODEL_H_
#define __IUPGRADEMODEL_H_
#include "Public/Base/TPComBase.h"
class IUpgradeModel
{
    public:
        virtual ~IUpgradeModel() { }
        virtual BOOL HasNewRelease(int *) = 0;
        virtual BOOL DownloadRelease() = 0;
        virtual BOOL InstallRelease() = 0;
};
    
#endif

