#ifndef __UPGRADEMODELIMPL_H_
#define __UPGRADEMODELIMPL_H_

#include "IUpgradeModel.h"
#include "GUI/UIMessageBox.h"
#include "GUI/UITextSingleLine.h"
#include <semaphore.h>
#include "singleton.h"
#include "dkObjBase.h"
#include <time.h>
#include <string>
#include <vector>



namespace DKAutoUpgrade{
#define UpgradeHistoryFile "/mnt/us/system/History.ini"

#define UpgradeSection "Upgrade"
#define OldVersionItem "OldVersion"
#define NewVersionItem "NewVersion"
#define UpgradeTypeItem "UpgradeType"
#define UpgradeResult "UpgradeResult"
#define UpgradeFailureCode "UpgradeFailureCode"


#define LastReleaseCheck   "/mnt/us/system/LastReleaseCheck.ini"
#define LastCheck_Section "LastCheck"
#define LastCheck_Version "Version"
#define LastCheck_Time "Time"

#define LastCandidateInfo   "/mnt/us/system/ReleaseCandidate.info"

#define AutoDetection 1
#define ManualDetection 0

    enum UpgradeType
    {
        COMPLETE_UPGRADE,
        INCREMENTAL_UPGRADE
        
    };

    enum UpgradeFailureReason
    {
        UNKNOWN = 1,
        WIFINOTAVAILABLE = 2,
        DISK_FULL = 3,
        DISK_WRITE_ERROR = 4,
        COPY_DELETE_ERROR = 5,
        TAR_ERROR = 6,
        CD_DOWNLOADFOLDER_ERROR = 7,
        CD_LITE_ERROR = 8
    };
    
    class ReleaseInfo
    {
        
    public:
        ReleaseInfo()
        {
            m_szDependency = "";
            m_Type = INCREMENTAL_UPGRADE;
            m_szMd5code = "";
            m_szUrl = "";
            m_version = "";
			m_szFeature = "";
            m_iSize = 0;
        }

        ReleaseInfo(ReleaseInfo* _release)
        {
            m_szDependency = _release->GetDependency();
            m_Type = _release->GetUpdateType();
            m_szMd5code = _release->GetMd5code();
            m_szUrl = _release->GetUrl();
            m_version = _release->GetVersion();
			m_szFeature = _release->GetFeature();
            m_iSize = _release->GetReleaseSize();
        }
        
        ~ReleaseInfo(){}
        void SetUpdateType(UpgradeType _type)
        {
            m_Type = _type;
        }
        UpgradeType GetUpdateType()
        {
            return m_Type;
        }

        void SetDependency(std::string _dependency)
        {
            m_szDependency = _dependency;
        }
        std::string GetDependency()
        {
            return m_szDependency;
        }

        void SetMd5code(std::string _code)
        {
            m_szMd5code = _code;
        }
        std::string GetMd5code()
        {
            return m_szMd5code;
        }

        void SetUrl(std::string _url)
        {
            m_szUrl = _url;
        }
        std::string GetUrl()
        {
            return m_szUrl;
        }

        void SetVersion(std::string _version)
        {
            m_version = _version;
        }
        std::string GetVersion()
        {
            return m_version;
        }

		void SetFeature(std::string _freture)
		{
			m_szFeature = _freture;
		}

		std::string GetFeature()
		{
			return m_szFeature;
		}
		
        void SetReleaseSize(UINT32 _size)
        {
            m_iSize = _size;
        }
        UINT32 GetReleaseSize()
        {
            return m_iSize;
        }
        void Print()
        {
            DebugPrintf(DLC_LIUJT, "ReleaseInfo::Print get releaseinfo: version=%s, url=%s, md5=%s, type=%d, size=%d, dependency=%s",
            m_version.c_str(),
            m_szUrl.c_str(),
            m_szMd5code.c_str(),
            m_Type,
            m_iSize,
            m_szDependency.c_str());
        }

    private:
        std::string        m_szDependency;
        UpgradeType        m_Type;
        std::string        m_szMd5code;
        std::string        m_szUrl;
        std::string        m_version;
		std::string		   m_szFeature;
        UINT32            m_iSize;

    };

    
    class UpgradeModelImpl : public IUpgradeModel
    {
        SINGLETON_H(UpgradeModelImpl);
    public:
        UpgradeModelImpl();
        virtual ~UpgradeModelImpl();
        void CheckNewRelease(bool ignoreTimeCheck = false);
        BOOL IsReleaseDownloaded(ReleaseInfo* release);
        BOOL IsReleaseDownloaded();
        BOOL HasNewRelease(int *errorCode);
        BOOL IsNewerVersion(std::string oldVersion, std::string newVersion);
        BOOL DownloadRelease();
        string GetPercentage(string prefix, int percentage);
        BOOL InstallRelease();
        ReleaseInfo* GetUpgradeCandidate(){ return m_pUpgradeReleaseCandidate; };
        void SetUpgradeCandidate(ReleaseInfo* release);
        void HandleDownloadedRelease(ReleaseInfo* release, BOOL fRecordIfUserCancel);
        void HandleUndownloadedRelease(ReleaseInfo* release, BOOL fRecordIfUserCancel);        
        BOOL GotoSystemDir();
        BOOL GotoDownloadDir();
        void RemoveUpgradeHistory();
        std::string GetLastUpgradeRelease();
        UpgradeType GetLastUpgradeType();
        
        void SetLastCheck(std::string newversion, time_t newtime);
        std::string GetCurVersion();        
        ReleaseInfo* ReadReleaseFromFile();
        void WriteReleaseToFile(ReleaseInfo* releaseinfo);
        BOOL IsAutoDetect()    {    return m_bAutoDetectRelease;    }
        void SetAutoDetect(BOOL bDetect)    {    m_bAutoDetectRelease = bDetect;    }

    private:

        std::string GetFilenameFromRelease(ReleaseInfo *);
        std::string URLParasToGetRelease();
        std::string URLParasToNotifyUpgrade(std::string oldversion, std::string newversion, UpgradeType type,BOOL downloadOrUpgrade, BOOL success, int failureCode = 0);
        void GetUpgradeCandiateFromServer(int *error);
        ReleaseInfo* PickupRelease(std::string _curVersion, std::vector <ReleaseInfo*> list);
        std::vector <ReleaseInfo*> ParseXMLReleases(std::string _szResultData);

    private:
        std::string     m_szLastCheckVersion;
        time_t            m_lLastCheckTime;
        ReleaseInfo*     m_pUpgradeReleaseCandidate;
        pthread_t         m_pThreadCheckRelease;
        BOOL             m_fIsChecking;
        sem_t             m_sem;
        BOOL            m_bAutoDetectRelease;
    };
}
#endif //__UPDATEMODELIMPL_H_

