#include "XiaoMi/KSSCloudInfo.h"
#include "Utility/JsonObject.h"

namespace xiaomi
{
KSSUploadInfo* KSSUploadInfo::CreateFromJsonObj(const JsonObject* jsonObj)
{
    if (!jsonObj)
    {
        return NULL; 
    }

    KSSUploadInfo *info = new KSSUploadInfo;
    if (info)
    {
        JsonObjectSPtr kssInfo = jsonObj->GetSubObject("kss");
        if (kssInfo)
        {
            JsonObjectSPtr blockMetasInfo = kssInfo->GetSubObject("block_metas");
            if (blockMetasInfo)
            {
                int metaSize = blockMetasInfo->GetArrayLength(); 
                for (int i = 0; i < metaSize; ++i)
                {
                    BlockMeta meta;
                    JsonObjectSPtr metaInfo = blockMetasInfo->GetElementByIndex(i);
                    if (metaInfo)
                    {
                        metaInfo->GetStringValue("block_meta", &meta.blockMeta);
                        metaInfo->GetBooleanValue("is_existed", &meta.existed);
                    }
                    info->blockMetas.push_back(meta);
                }
            }

            JsonObjectSPtr urlsInfo = kssInfo->GetSubObject("node_urls");
            if (urlsInfo)
            {
                int urlSize = urlsInfo->GetArrayLength();
                for (int i = 0; i < urlSize; ++i)
                {
                    JsonObjectSPtr urlInfo = urlsInfo->GetElementByIndex(i);
                    if (urlInfo)
                    {
                        info->urls.push_back(urlInfo->GetJsonString());
                    }
                }
            }

            kssInfo->GetStringValue("secure_key", &info->securityKey);
            kssInfo->GetStringValue("file_meta", &info->fileMeta);
            kssInfo->GetStringValue("stat", &info->stat);
        }

        jsonObj->GetStringValue("upload_id", &info->uploadId);

        return info;

    }

    SafeDeletePointer(info);
    return NULL;
}
}
