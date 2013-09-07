#ifndef __KSS_CLOUD_INFO_H__
#define __KSS_CLOUD_INFO_H__

#include <string>
#include <tr1/memory>
#include <vector>
using namespace std;

class JsonObject;
namespace xiaomi
{
struct BlockMeta
{
    string blockMeta;
    bool existed;
};//BlockMeta
typedef vector<BlockMeta> BlockMetaList;

//upload info data structure returned by url:/mic/sfs/v2/user/{userId:[0-9]+}/ns/{ns:[0-9a-zA-Z_\-\.]+}/file
struct KSSUploadInfo
{
    static KSSUploadInfo* CreateFromJsonObj(const JsonObject* );
    BlockMetaList blockMetas;
    vector<string> urls;
    string securityKey;
    string fileMeta;
    string stat;
    string uploadId;
};//KSSUploadInfo

typedef tr1::shared_ptr<KSSUploadInfo> KSSUploadInfoSPtr;
}//xiaomi
#endif//__KSS_CLOUD_INFO_H__
