#include "DownloadManager/IDownloadTask.h"
#include "DownloadManager/IDownloader.h"

void IDownloadTask::FireDownloadProgressUpdateEvent()
{
    DownloadUpdateEventArgs args;
    IDownloader::GetInstance()->FireDownloadProgressUpdateEvent(GetType(),
                                                                GetState(),
                                                                GetPercentage(),
                                                                GetOrigUrlID());
}

void IDownloadTask::OnProgressUpdate(unsigned int downloadTotal, unsigned downloadNow)
{
    if (downloadTotal > 0 && downloadNow > 0)
    {
        if (0 == GetLastOffSet())
        {
            SetFileSize(downloadTotal);
        }
        SetLastOffSet(GetFileSize() - downloadTotal + downloadNow);
        time_t tnow;
        time(&tnow);
        if (tnow != m_lastUpdateTime || downloadNow == downloadTotal)
        {
            m_lastUpdateTime = tnow;
            FireDownloadProgressUpdateEvent();
        }
    }
}
