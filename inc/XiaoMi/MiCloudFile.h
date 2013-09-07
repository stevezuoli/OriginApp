/*
 * =====================================================================================
 *       Filename:  MiCloudFile.h
 *    Description:  File DataStructure for cloud file
 *
 *        Version:  1.0
 *        Created:  08/19/2013 03:41:27 PM
 * =====================================================================================
 */

#ifndef __MICLOUD_FILE_H__
#define __MICLOUD_FILE_H__

#include "Utility/JsonObject.h"
#include <stdint.h>
#include <string>
#include <tr1/memory>
#include <vector>

namespace xiaomi
{
struct MiCloudFile
{
    static MiCloudFile* CreateFromJsonObj(const JsonObject* obj);

    MiCloudFile()
        : createTime(0)
        , modifyTime(0)
        , type(FT_Invalid)
        , size(0)
    {
    }

    enum FileType
    {
        FT_Dir,
        FT_File,
        FT_Invalid
    };//FileType

    std::string id;
    std::string name;
    std::string parentId;
    int64_t createTime;
    int64_t modifyTime;
    FileType type;

    //useful only in "file"
    int64_t size;
    std::string sha1;
};
typedef std::tr1::shared_ptr<MiCloudFile> MiCloudFileSPtr;
typedef std::vector<MiCloudFileSPtr> MiCloudFileSPtrList;
}//xiami
#endif//__MICLOUD_FILE_H__

