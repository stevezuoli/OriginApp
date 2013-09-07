#ifndef __XIAOMI_SERVICE_FACTORY_H__
#define __XIAOMI_SERVICE_FACTORY_H__

#include "XiaoMi/MiCloudService.h"

namespace xiaomi
{
class XiaoMiServiceFactory
{
public:
    static MiCloudService* GetMiCloudService()
    {
        static xiaomi::MiCloudService miCloudService;
        return &miCloudService;
    }
};
}//xiaomi

#endif//__XIAOMI_SERVICE_FACTORY_H__
