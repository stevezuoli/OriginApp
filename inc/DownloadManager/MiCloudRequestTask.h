#ifndef __MICLOUD_REQUEST_DOWNLOAD_TASK_H__
#define __MICLOUD_REQUEST_DOWNLOAD_TASK_H__

#include "DownloadManager/DownloadTask.h"
namespace dk
{
class MiCloudCreateFileRequestTask : public DownloadTask
{
public:
    MiCloudCreateFileRequestTask(const std::string& dirId, const std::string& filePath, const std::string& displayName);
    virtual bool PrepareData();

private:
    std::string GenerateSignatureData();

    std::string m_dirId;
    std::string m_filePath;
    std::string m_displayName;
};
typedef std::tr1::shared_ptr<MiCloudCreateFileRequestTask> MiCloudCreateFileRequestTaskSPtr;

}
#endif//__MICLOUD_REQUEST_DOWNLOAD_TASK_H__
