#include "XiaoMi/XiaomiConstants.h"

namespace xiaomi
{
std::string XiaoMiConstants::s_micloudServiceBaseUrl = "http://sfsapi.micloud.xiaomi.net";
std::string XiaoMiConstants::s_micloudNameSpace = "duokan";
std::string XiaoMiConstants::s_micloudCreateFileUrl = "/mic/sfs/v2/user/%s/ns/%s/file";
std::string XiaoMiConstants::s_micloudCreateDirUrl = "/mic/sfs/v2/user/%s/ns/%s/dir";
std::string XiaoMiConstants::s_micloudCommitFileUrl = "/mic/sfs/v2/user/%s/ns/%s/file/storage";
std::string XiaoMiConstants::s_micloudRequestDownloadUrl = "/mic/sfs/v2/user/%s/ns/%s/file/%s/storage";
std::string XiaoMiConstants::s_micloudDeleteFileUrl = "/mic/sfs/v2/user/%s/ns/%s/file/%s/delete";
std::string XiaoMiConstants::s_micloudDeleteDirUrl = "/mic/sfs/v2/user/%s/ns/%s/dir/%s/delete";
std::string XiaoMiConstants::s_micloudGetChildrenUrl = "/mic/sfs/v2/user/%s/ns/%s/dir/%s/list";
std::string XiaoMiConstants::s_micloudGetInfoUrl = "/mic/sfs/v2/user/%s/ns/%s/info";
std::string XiaoMiConstants::s_micloudGetQuotaUrl = "/mic/sfs/v2/user/%s/ns/%s/quota";
std::string XiaoMiConstants::s_micloudBookRootDir = "/BOOKS";
std::string XiaoMiConstants::s_micloudFileLocalDir = "/mnt/us/DK_Download/";
int XiaoMiConstants::s_micloudBlockSize = 4 * 1024 * 1024;
}//xiaomi
