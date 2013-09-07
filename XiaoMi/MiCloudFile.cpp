#include "XiaoMi/MiCloudFile.h"

namespace xiaomi
{
MiCloudFile* MiCloudFile::CreateFromJsonObj(const JsonObject* fileInfoObj)
{
    if (!fileInfoObj)
    {
        return NULL;
    }

    MiCloudFile *file = new MiCloudFile;
    std::string strType;
    if (file)
    {
        fileInfoObj->GetStringValue("id", &file->id);
        fileInfoObj->GetStringValue("name", &file->name);
        fileInfoObj->GetStringValue("parent_id", &file->parentId);
        fileInfoObj->GetStringValue("type", &strType);
        fileInfoObj->GetInt64Value("create_time", &file->createTime);
        fileInfoObj->GetInt64Value("modify_time", &file->modifyTime);
        if (strType.compare("dir") == 0)
        {
            file->type = FT_Dir;
        }
        else if (strType.compare("file") == 0)
        {
            file->type = FT_File;
            fileInfoObj->GetStringValue("sha1", &file->sha1);
            fileInfoObj->GetInt64Value("size", &file->size);
        }
    }

    return file;
}
}
