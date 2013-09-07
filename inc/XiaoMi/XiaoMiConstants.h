#ifndef __XIAOMI_CONSTANTS_H__
#define __XIAOMI_CONSTANTS_H__

#include <string>

namespace xiaomi
{
class MiCloudService;
class MiCloudManager;
class MiCloudDownloadTaskBuilder;
class XiaoMiConstants
{
    //http://wiki.n.miliao.com/xmg/MiCLOUD_%E5%BA%94%E7%94%A8%E6%95%B0%E6%8D%AE%E5%A4%87%E4%BB%BD_API%E6%96%87%E6%A1%A3
    static std::string s_micloudServiceBaseUrl;
    static std::string s_micloudNameSpace;
    static std::string s_micloudCreateFileUrl;
    static std::string s_micloudCreateDirUrl;
    static std::string s_micloudCommitFileUrl;
    static std::string s_micloudRequestDownloadUrl;
    static std::string s_micloudDeleteFileUrl;
    static std::string s_micloudDeleteDirUrl;
    static std::string s_micloudGetChildrenUrl;
    static std::string s_micloudGetInfoUrl;
    static std::string s_micloudGetQuotaUrl;
    static std::string s_micloudBookRootDir;
    static std::string s_micloudFileLocalDir;
    static int s_micloudBlockSize;
    friend class MiCloudManager;
    friend class MiCloudService;
    friend class MiCloudDownloadTaskBuilder;
};
}//xiaomi

#endif//__XIAOMI_CONSTANTS_H__
