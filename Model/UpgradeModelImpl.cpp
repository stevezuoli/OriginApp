#include "Model/UpgradeModelImpl.h"
#include "DownloadManager/DownloadTaskFactory.h"
#include "DownloadManager/IDownloadReleaseTask.h"
#include "DownloadManager/IDownloader.h"

#include "Common/SystemSetting_DK.h"
#include "../DK91Search/DK_XmlParse.h"
#include "../DK91Search/Curl_Post.h"
#include "libxml/parser.h"
#include "Utility/ThreadHelper.h"
#include "Utility/MD5Checker.h"
#include "drivers/DeviceInfo.h"
#include "Wifi/WifiManager.h"
#include "I18n/StringManager.h"
#include "SQM.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UIComplexButton.h" 
#include "CommonUI/UIUtility.h"
#include "Framework/CNativeThread.h"
#include "Framework/CDisplay.h"
#include "Common/SystemOperation.h"
#include "Common/WindowsMetrics.h"
#include <sys/stat.h>
#include "IniFile.h"
#include "unistd.h"
#include "Utility.h"
#include <stdlib.h>
#include "Utility/SystemUtil.h"
#include "TouchAppUI/UISystemUpdateDlg.h"
#include "Utility/StringUtil.h"

using dk::utility::SystemUtil;
using dk::utility::StringUtil;

using namespace std;
using namespace DkFormat;
using namespace DKAutoUpgrade;
using namespace DK91SearchAPI;

#define         GROUPCATEGORY                          "1"
    
#define         DK_AUTOUPGRADE_SERVEURL                "http://update.duokan.com:9999/DuoKanServer/servlet/"
#define         DK_GETRELEASE_URL                  "http://update.duokan.com:9999/DuoKanServer/servlet/GetLatestRelease"
#define         DK_GETRELEASE_SERVLET                  "GetLatestRelease"

#define            DK_UPDATE_RESULT_XML_CODE                "UTF-8"
#define            DK_UPDATE_RESULT_NODE_CANDIDATES        "candidates"
#define            DK_UPDATE_RESULT_NODE_CANDIDATE            "candidate"
#define            DK_UPDATE_RESULT_NODE_VERSION            "version"
#define            DK_UPDATE_RESULT_NODE_TYPE              "type"
#define            DK_UPDATE_RESULT_NODE_DEPENDENCY        "dependency"
#define            DK_UPDATE_RESULT_NODE_MD5CODE            "md5code"
#define            DK_UPDATE_RESULT_NODE_URL                "url"
#define            DK_UPDATE_RESULT_NODE_RELEASESIZE        "size"
#define            DK_UPDATE_RESULT_NODE_FEATURE        	"feature"

SINGLETON_CPP(UpgradeModelImpl);

UpgradeModelImpl::UpgradeModelImpl()
{
    m_szLastCheckVersion = "";
    m_lLastCheckTime = 0;
    m_bAutoDetectRelease = TRUE;
    m_fIsChecking = FALSE;

    CIniFile iniFile;
    iniFile.SetPath(LastReleaseCheck);

    IniKeyMap::iterator keymapiter;

    if(iniFile.ReadFile())
    {
        for(keymapiter=iniFile.keys.begin();keymapiter!=iniFile.keys.end();++keymapiter)
        {
                string upgrade = keymapiter->first;
                if(upgrade.empty())
                {
                    continue;
                }
                m_szLastCheckVersion = iniFile.GetStringValue(LastCheck_Section, LastCheck_Version, "");
                string lastCheckTime = iniFile.GetStringValue(LastCheck_Section, LastCheck_Time, "");
                m_lLastCheckTime = (time_t)(strtol(lastCheckTime.c_str(),NULL,10));

                DebugPrintf(DLC_DIAGNOSTIC, "UpgradeModelImpl::UpgradeModelImpl get last verion = %s, last check time=%ld", m_szLastCheckVersion.c_str(),m_lLastCheckTime);
                break;
         }
    }
    
    
    m_pUpgradeReleaseCandidate = ReadReleaseFromFile(); //try to restore this candidate from file
    m_pThreadCheckRelease = 0;
    sem_init(&m_sem,0,1);
}


void UpgradeModelImpl::WriteReleaseToFile(ReleaseInfo* releaseinfo)
{
    if(!releaseinfo)
    {
        return;
    }

    FILE* pFile = fopen(LastCandidateInfo, "wb");
    if(!pFile)
    {
        return;
    }
    string szVersion = releaseinfo->GetVersion();
    string szURL = releaseinfo->GetUrl();
    string szMD5Code = releaseinfo->GetMd5code();
    char tmp[10] = {0};
    snprintf(tmp, 10, "%d",(int)releaseinfo->GetUpdateType());
    string szUpgradeType(tmp);
    
    memset(tmp, 0, 10);
    snprintf(tmp, 10, "%d", releaseinfo->GetReleaseSize());
    string szReleaseSize(tmp);

    string szDependency = releaseinfo->GetDependency();

	string szFeature = releaseinfo->GetFeature();
    
    CUtility::WriteStringToFile(szVersion, pFile);
    CUtility::WriteStringToFile(szURL, pFile);
    CUtility::WriteStringToFile(szMD5Code, pFile);
    CUtility::WriteStringToFile(szUpgradeType, pFile);
    CUtility::WriteStringToFile(szReleaseSize, pFile);
    CUtility::WriteStringToFile(szDependency, pFile);
    CUtility::WriteStringToFile(szFeature, pFile);
    fclose(pFile);
}


ReleaseInfo* UpgradeModelImpl::ReadReleaseFromFile()
{
    DebugPrintf(DLC_DIAGNOSTIC, "UpgradeModelImpl::ReadReleaseFromFile entering");
    FILE* pFile = fopen(LastCandidateInfo, "rb");
    if(!pFile)
    {
        return NULL;
    }
    //读出version
    string szVersion("");
    if(!CUtility::ReadStringFromFile(szVersion, pFile))
    {
        fclose(pFile);
        return NULL;
    }
    string szURL("");
    if(!CUtility::ReadStringFromFile(szURL, pFile))
    {
        fclose(pFile);
        return NULL;
    }

    string szMD5Code("");
    if(!CUtility::ReadStringFromFile(szMD5Code, pFile))
    {
        fclose(pFile);
        return NULL;
    }

    string szUpgradeType("");
    if(!CUtility::ReadStringFromFile(szUpgradeType, pFile))
    {
        fclose(pFile);
        return NULL;
    }

    string szReleaseSize("");
    if(!CUtility::ReadStringFromFile(szReleaseSize, pFile))
    {
        fclose(pFile);
        return NULL;
    }
    string szDependency("");
    if(!CUtility::ReadStringFromFile(szDependency, pFile))
    {
        fclose(pFile);
        return NULL;
    }

	string szFeature("");
	if(!CUtility::ReadStringFromFile(szFeature, pFile))
	{
		fclose(pFile);
		return NULL;
	}

    fclose(pFile);

    if(szVersion.empty() || szURL.empty() || szMD5Code.empty() || szReleaseSize.empty())
    {
        return NULL;
    }
    
    ReleaseInfo* pReleaseInfo = new ReleaseInfo();

    if(pReleaseInfo)
    {
        pReleaseInfo->SetVersion(szVersion);
        pReleaseInfo->SetUrl(szURL);
        pReleaseInfo->SetMd5code(szMD5Code);
        pReleaseInfo->SetUpdateType((UpgradeType)atoi(szUpgradeType.c_str()));
        pReleaseInfo->SetReleaseSize(strtol(szReleaseSize.c_str(), NULL, 10));
        pReleaseInfo->SetDependency(szDependency);
		pReleaseInfo->SetFeature(szFeature);
        pReleaseInfo->Print();
    }
    DebugPrintf(DLC_DIAGNOSTIC, "UpgradeModelImpl::ReadReleaseFromFile end");
    return pReleaseInfo;
}

UpgradeModelImpl::~UpgradeModelImpl()
{
    if(NULL != m_pUpgradeReleaseCandidate)
    {
        delete m_pUpgradeReleaseCandidate;
        m_pUpgradeReleaseCandidate = NULL;
    }
    
    m_pThreadCheckRelease = 0;

    sem_destroy(&m_sem);
}
void UpgradeModelImpl::SetUpgradeCandidate(ReleaseInfo* release)
{
    DebugPrintf(DLC_DIAGNOSTIC, "UpgradeModelImpl::SetUpgradeCandidate Entering: release!=NULL (%d)",(NULL != release));
    if(m_pUpgradeReleaseCandidate != release)
    {
        if(NULL != m_pUpgradeReleaseCandidate)
        {
            SafeDeletePointerEx(m_pUpgradeReleaseCandidate);
        }
        m_pUpgradeReleaseCandidate = release;
    }
    DebugPrintf(DLC_DIAGNOSTIC, "UpgradeModelImpl::SetUpgradeCandidate End");
}

string UpgradeModelImpl::URLParasToGetRelease()
{
    DebugPrintf(DLC_DIAGNOSTIC,"UpgradeModelImpl::URLParasToGetRelease Entering");
    string paras;
    paras.append("deviceid=").append(DeviceInfo::GetDeviceID())
         .append("&version=").append(GetCurVersion())
         .append("&usergroup=").append(GROUPCATEGORY);
    DebugPrintf(DLC_DIAGNOSTIC,"UpgradeModelImpl::URLParasToGetRelease set params for getting release: %s", paras.c_str());
    return paras;
}

/*
*This method return the most matching release from the given list, and release all the other elements' memory.
*And the caller should be responsible for releasing the memory of the return one!
*/
ReleaseInfo* UpgradeModelImpl::PickupRelease(string curVersion, vector <ReleaseInfo*> list)
{
    ReleaseInfo* latestRelease(NULL);
    if(!list.empty())
    {
        for(UINT32 i = 0; i < list.size(); i++)
        {
            ReleaseInfo* temp = list[i];
            if(NULL == temp)
            {
                continue;
            }

            if((temp->GetUpdateType() == COMPLETE_UPGRADE) ||
			   (temp->GetUpdateType() == INCREMENTAL_UPGRADE) )
            {
                latestRelease = new ReleaseInfo(temp);
                if(NULL != latestRelease)
                {
                    break;
                }
            }

        }
        for(UINT32 i = 0; i < list.size(); i++)
        {
            ReleaseInfo* temp = list[i];
            if(NULL != temp)
            {
                delete temp;
                temp = NULL;
            }
        }
        list.clear();
    }
    
    return latestRelease;
}
BOOL UpgradeModelImpl::HasNewRelease(int *errorCode)
{
    if(NULL == errorCode)
    {
        DebugPrintf(DLC_DIAGNOSTIC,"UpgradeModelImpl::HasNewRelease NULL errorCode ");
        return FALSE;
    }
    
	GetUpgradeCandiateFromServer(errorCode);
	ReleaseInfo* LatestRelease = GetUpgradeCandidate();
    if(NULL == LatestRelease)
    {
        DebugPrintf(DLC_DIAGNOSTIC,"UpgradeModelImpl::HasNewRelease NULL LatestRelease");
        return FALSE;
    }
    string sCurVersion = GetCurVersion();
    string sLastestVersion = LatestRelease->GetVersion();

    DebugPrintf(DLC_DIAGNOSTIC,"UpgradeModelImpl::HasNewRelease current version is: %s, new version is: %s", sCurVersion.c_str(), sLastestVersion.c_str());

    return IsNewerVersion(sCurVersion, sLastestVersion);
}

BOOL UpgradeModelImpl::IsNewerVersion(string oldVersion, string newVersion)
{
    DebugPrintf(DLC_DIAGNOSTIC, "UpgradeModelImpl::IsNewerVersion Entering oldversion=%s, newversion=%s", oldVersion.c_str(), newVersion.c_str());
    if(oldVersion.empty() || newVersion.empty())
    {
        return FALSE;
    }
    
    size_t indexOfOldVersion = oldVersion.find(".rtm");
    size_t indexOfNewVersion = newVersion.find(".rtm");    
    if(string::npos != indexOfOldVersion)
    {
        oldVersion = oldVersion.substr(0,indexOfOldVersion);
    }


    if(string::npos != indexOfNewVersion)
    {
        newVersion = newVersion.substr(0,indexOfNewVersion);
    }

    int result = oldVersion.compare(newVersion);

    if(result < 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


void UpgradeModelImpl::CheckNewRelease(bool ignoreTimeCheck)
{
    DebugPrintf(DLC_LIUJT, "UpgradeModelImpl::CheckNewRelease begin to check new release!");
    if(m_fIsChecking || !WifiManager::GetInstance()->IsConnected())
    {
        DebugPrintf(DLC_LIUJT, "UpgradeModelImpl::CheckNewRelease End ischecking=%d, wifi connected=%d",m_fIsChecking, WifiManager::GetInstance()->IsConnected());
        return;
    }
    DebugPrintf(DLC_LIUJT, "UpgradeModelImpl::CheckNewRelease begin to set thread properties!");
    pthread_detach(pthread_self());
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);

    WAIT(m_sem);
    m_fIsChecking = TRUE;

    int errorCode = 0;
    if(HasNewRelease(&errorCode))
    {
        ReleaseInfo* LatestRelease = GetUpgradeCandidate();
        string szCurVersion = GetCurVersion();

        BOOL NotifyNewRelease = (ignoreTimeCheck ? true : (m_szLastCheckVersion != LatestRelease->GetVersion()));

        if(!NotifyNewRelease)// if last check version equals with latest version
        {
            time_t now = time(NULL);
            double temp = 7*24*3600;
            if(difftime(now, m_lLastCheckTime) > temp)//If last check was one week ago
            {
                DebugPrintf(DLC_LIUJT, "UpgradeModelImpl::CheckNewRelease last check is before 7 days ago!");
                NotifyNewRelease = TRUE;
                SetLastCheck(LatestRelease->GetVersion(), now);
            }
            else
            {
                DebugPrintf(DLC_LIUJT, "UpgradeModelImpl::CheckNewRelease last check is inside 7 days, and the target version is same! Don't update this time!");
            }
        }
        
        if(NotifyNewRelease)
        {
            SNativeMessage msg;
            msg.iType = KMessageNewReleaseDownloadNotify;
            msg.iParam1 = (UINT32)(LatestRelease);
            msg.iParam2 = AutoDetection;
            CNativeThread::Send(msg);

            DebugPrintf(DLC_LIUJT, "UpgradeModelImpl::CheckNewRelease current version is: %s, target version is: %s", szCurVersion.c_str(),LatestRelease->GetVersion().c_str());
        }
    }
    
    m_fIsChecking = FALSE;
    POST(m_sem);
}


void UpgradeModelImpl::SetLastCheck(string newversion, time_t newtime)
{
    DebugPrintf(DLC_LIUJT,"UpgradeModelImpl::SetLastCheck Entering with newversion %s", newversion.c_str());
    if(newversion.empty())
    {
        return;
    }
    
    m_szLastCheckVersion = newversion;
    m_lLastCheckTime = newtime;


    CIniFile iniFile;
    iniFile.SetPath(LastReleaseCheck);
    iniFile.Reset();

    iniFile.StoreStringValue(LastCheck_Section, LastCheck_Version, newversion, TRUE);
    CHAR str[50];
    memset(str, 0, 50);
    sprintf(str,"%ld",(long)newtime);
    iniFile.StoreStringValue(LastCheck_Section, LastCheck_Time, string(str), TRUE);

    iniFile.WriteFile();
    
    DebugPrintf(DLC_LIUJT,"UpgradeModelImpl::SetLastCheck End");
}
BOOL UpgradeModelImpl::DownloadRelease()
{
    DebugPrintf(DLC_DIAGNOSTIC, "UpgradeModelImpl::DownloadRelease Entering");
    if(NULL == m_pUpgradeReleaseCandidate)
    {

        DebugPrintf(DLC_DIAGNOSTIC, "UpgradeModelImpl::DownloadRelease m_pUpgradeReleaseCandidate=NULL");
        return FALSE;
    }
    UITitleBar *pTitleBar = (CDisplay::GetDisplay()->GetCurrentPage())->GetTitleBar();
    if(NULL != pTitleBar && (pTitleBar->GetBatteryLevel() <= 1))
    {
        UIMessageBox * messagebox = UIUtility::CreateMessageBox();
        if(NULL == messagebox)
        {
            return FALSE;
        }
        
        string text(StringManager::GetStringById(LOW_POWER_NO_DOWNLOAD));
        messagebox->SetText(text.c_str());
        messagebox->DoModal();
        
        delete messagebox;
        messagebox = NULL;
        
        return FALSE;
   }

    struct statfs diskinfo;
    int result = statfs("/mnt/us/", &diskinfo);

    if(0 != result)
    {
        DebugPrintf(DLC_DIAGNOSTIC, "UpgradeModelImpl::DownloadRelease failed to get disk info!");
        return FALSE;
    }
    
	// 检测磁盘空间， 至少为 (1 + x )倍 tar.gz的大小 , 单位为KB
	// 经统计， 压缩比在40% - 50% ， 解压后大约为2-2.5倍空间
	// 增量包设为3+1, 完整包设为2.5 + 1
    // 增加到 2.8倍
    long lBlockSize = diskinfo.f_bsize;
    unsigned long lLeftSize = lBlockSize * (diskinfo.f_bfree/1024);
	unsigned long lReleaseSize = m_pUpgradeReleaseCandidate->GetReleaseSize() / 1024;
	unsigned long lRequiredSize;
	SOURCESTRINGID ota_package_stringid;

	// 目前认为增量包小于30MB
    // 增加1024KB （1MB）余量

	lRequiredSize = lReleaseSize + 1024;
	if(lReleaseSize < 30000)
    {
        lRequiredSize += lReleaseSize * 3;
		ota_package_stringid = UPGRADE_OTA_DIFF_PACKAGE;
    }
    else
    {
        lRequiredSize += (lReleaseSize * 28 / 10);
		ota_package_stringid = UPGRADE_OTA_FULL_PACKAGE;
    }

    DebugPrintf(DLC_DIAGNOSTIC, "UpgradeModelImpl::DownloadRelease block size=%ld bytes, free block number=%ld, Left space= %ld Kbytes, need space: %ld Kbytes", lBlockSize, diskinfo.f_bfree, lLeftSize, lRequiredSize);
    if(lLeftSize < lRequiredSize)
    {
        UIMessageBox* messagebox = UIUtility::CreateMessageBox();
        string ota_diskfull(StringManager::GetStringById(UPGRADE_DISK_FULL));
		string ota_packge(StringManager::GetStringById(ota_package_stringid));
		string ota_needspace(StringManager::GetStringById(UPGRADE_OTA_NEED_SPACE));
		string ota_freespace(StringManager::GetStringById(UPGRADE_OTA_FREE_SPACE));
		char buf[1024] = {0};
		snprintf(buf, DK_DIM(buf), "%s\n%s %lu MB\n%s %lu MB\n%s %lu MB", ota_diskfull.c_str(), ota_packge.c_str(), lReleaseSize/1024, \
			ota_needspace.c_str(), lRequiredSize/1024, ota_freespace.c_str(), lLeftSize/1024);
        messagebox->SetText(buf);
        messagebox->DoModal();
        delete messagebox;
        messagebox = NULL;
        return FALSE;
    }

    IDownloader* downloadManager = IDownloader::GetInstance();

    string url = m_pUpgradeReleaseCandidate->GetUrl();
        
    if(downloadManager->IsDownloading(url))
    {
        DebugPrintf(DLC_DIAGNOSTIC, "UpgradeModelImpl::DownloadRelease the release %s is being downloaded in the queue!", url.c_str());
        return TRUE;
    }
    
    string filename = GetFilenameFromRelease(m_pUpgradeReleaseCandidate);    

    if(filename.empty())
    {
        DebugPrintf(DLC_DIAGNOSTIC, "UpgradeModelImpl::DownloadRelease failed to get release name");
        return FALSE;
    }
    
    if(IsReleaseDownloaded(m_pUpgradeReleaseCandidate))
    {
        HandleDownloadedRelease(m_pUpgradeReleaseCandidate,FALSE);
        return TRUE;
    }

    //If the file was downloaded with failure, delete it firstly!
    string file = DOWNLOADPATH+filename;

    if(0 == unlink(file.c_str()))
    {
        DebugPrintf(DLC_DIAGNOSTIC, "UpgradeModelImpl::DownloadRelease delete file %s successfully!", file.c_str());
    }
    else
    {
        DebugPrintf(DLC_DIAGNOSTIC, "UpgradeModelImpl::DownloadRelease fail to delete file %s!", file.c_str());
    }
        
    IDownloadReleaseTask *pUpdateTask = DownloadTaskFactory::CreateDownloadReleaseTask(url,url,url, filename, m_pUpgradeReleaseCandidate->GetReleaseSize());
    if(NULL == pUpdateTask)
    {
        return FALSE;
    }
    
    IDownloader *pDownloadManager = IDownloader::GetInstance();
    if(NULL == pDownloadManager)
    {
        delete pUpdateTask;
        pUpdateTask = NULL;
        return FALSE;
    }
      
    DebugPrintf(DLC_DIAGNOSTIC, "UpgradeModelImpl::DownloadRelease create download task for given url: %s", url.c_str());
    pDownloadManager->AddTask(pUpdateTask);
    WriteReleaseToFile(m_pUpgradeReleaseCandidate);
    return TRUE;
}

/*
*This method will not release the memory for given pointer, which should be released by the caller!!!
*/
string UpgradeModelImpl::GetFilenameFromRelease(ReleaseInfo* release)
{
    if(NULL == release)
    {
        return "";
    }
    
    string url = release->GetUrl();
    
    if(url.empty())
    {
        DebugPrintf(DLC_LIUJT,"UpgradeModelImpl::DownloadRelease url is empty!");
        return FALSE;
    }
    
    string::size_type index = url.find_last_of("/"); 
    string filename;
    if(index == string::npos)
    {
        DebugPrintf(DLC_LIUJT,"UpgradeModelImpl::DownloadRelease failed to find valid file name!");
        return FALSE;
    }
    filename = url.substr(index+1);    
    return filename;
}

/*
*This method will not release the memory for given pointer, which should be released by the caller!!!
*/
BOOL UpgradeModelImpl::IsReleaseDownloaded(ReleaseInfo* release)
{
    if(NULL == release)
    {
        return FALSE;
    }
    //release->Print();
    string releasezipfile = GetFilenameFromRelease(release);
    DebugPrintf(DLC_DIAGNOSTIC, "UpgradeModelImpl::IsReleaseDownloaded releasezipfile=%s",releasezipfile.c_str());    

    if(releasezipfile.empty())
    {
        DebugPrintf(DLC_DIAGNOSTIC, "UpgradeModelImpl::IsReleaseDownloaded  get empty filename for given url: %s", release->GetVersion().c_str());
        return FALSE;
    }

    string::size_type index = releasezipfile.find_last_of(".tar.gz");
    
    if(index == string::npos)
    {
        DebugPrintf(DLC_DIAGNOSTIC, "UpgradeModelImpl::IsReleaseDownloaded wrong format of release downloaded file: %s", releasezipfile.c_str());
        return FALSE;
    }

    string file = DOWNLOADPATH+releasezipfile;

    struct stat aStat;
    int result = stat( file.c_str(), &aStat );
    UINT32 filesize = 0;
    if(result == 0)//If file exists
    {
        filesize = aStat.st_size;
    }
    DebugPrintf(DLC_DIAGNOSTIC, "UpgradeModelImpl::IsReleaseDownloaded download file %s, file size=%d, release size=%d",file.c_str(), filesize,release->GetReleaseSize());
    
    if(filesize != release->GetReleaseSize())
    {
        return FALSE;
    }

    char* pMD5Code = MD5Checker::GetInstance()->DK_MDFile(file.c_str());
    if(NULL == pMD5Code)
    {
        return FALSE;
    }

    string md5FromServer = release->GetMd5code();
    string md5Local(pMD5Code);
    
    DebugPrintf(DLC_DIAGNOSTIC, "UpgradeModelImpl::IsReleaseDownloaded md5 from server is %s, local md5 is %s", md5FromServer.c_str(), md5Local.c_str());

    return !md5FromServer.empty() && (md5FromServer == md5Local);
  
}

BOOL UpgradeModelImpl::IsReleaseDownloaded()
{
    return IsReleaseDownloaded(m_pUpgradeReleaseCandidate);
}


void UpgradeModelImpl::HandleDownloadedRelease(ReleaseInfo* release, BOOL fRecordIfUserCancel)
{
    UIMessageBox* messagebox = UIUtility::CreateMessageBox(MB_OK | MB_CANCEL);

    if(NULL != messagebox)
    {
        string text(StringManager::GetStringById(FINDNEWRELEASE));
        text.append(": ").append(release->GetVersion())
            .append(", ").append(StringManager::GetStringById(HAVEDOWNLOADNEWVERSION))
            .append("\n").append(StringManager::GetStringById(UPGRADENOW));

        messagebox->SetText(text.c_str());

        if(MB_OK == messagebox->DoModal())
        {
			DebugPrintf(DLC_LIUJT, "###### message box do modal , OK");
            SNativeMessage msg;
            msg.iType = KMessageNewReleaseUpgradeNotify;
            CNativeThread::Send(msg);
        }
        else if(fRecordIfUserCancel)
        {
			DebugPrintf(DLC_LIUJT, "###### message box do modal , cancel");
            DebugPrintf(DLC_LIUJT, "User cancelled to upgrade!!!!");
            time_t now = time(NULL);
            SetLastCheck(release->GetVersion(), now);
        }
        
        delete messagebox;
        messagebox = NULL;
    }
}


void UpgradeModelImpl::HandleUndownloadedRelease(ReleaseInfo* release, BOOL fRecordIfUserCancel)
{

	DebugPrintf(DLC_ZHY, "enter UpgradeModelImpl::HandleUndownloadedRelease");
    IDownloader* downloadManager = IDownloader::GetInstance();
    if (NULL == m_pUpgradeReleaseCandidate)
    {
        return;
    }

    string url = m_pUpgradeReleaseCandidate->GetUrl();
    IDownloadTask::DLState state = downloadManager->GetTaskStateByUrlId(url);

    if(IDownloadTask::WORKING == state || IDownloadTask::CANCELED == state || IDownloadTask::PAUSED == state || IDownloadTask::WAITING == state || IDownloadTask::FAILED == state)
    {
        DownloadRelease();
        return;
    }

	UIContainer* pParent = UIUtility::GetCurDisplay();
	if(!pParent)
	{
		return;
	}

	string strSeparator = "-------------------DESCRIPTION-SEPARATOR-------------------";
	string strFeature = release->GetFeature();

	string strReplace = "\n\t";
	strFeature = StringUtil::ReplaceString(strFeature, '\n', strReplace.c_str());
	string strRepair = "";
	string strNewFunction = "";
	string::size_type posStart = strFeature.find(strSeparator);
	if(posStart != string::npos)
	{
		strRepair = strFeature.substr(0, posStart);
		strNewFunction = strFeature.substr(posStart + strSeparator.size());
	}

	string text(StringManager::GetPCSTRById(FINDNEWRELEASE));
	text.append("\n");
    text.append(StringManager::GetPCSTRById(CURRENTVERSION)).append(":  ").append(GetCurVersion())
        .append("\n").append(StringManager::GetPCSTRById(LATESTVERSION)).append(":  ").append(release->GetVersion());
	if(!strNewFunction.empty())
	{
		text.append("\n\n").append(StringManager::GetPCSTRById(UPDATE_NEW_FUNCTION)).append(":\n\t").append(strNewFunction);
	}
	if(!strRepair.empty())
	{
		text.append("\n\n").append(StringManager::GetPCSTRById(UPDATE_NEW_REPAIR)).append(":\n\t").append(strRepair);
	}
    text.append("\n\n").append(StringManager::GetPCSTRById(AUTOREBOOT)).append("\n").append(StringManager::GetPCSTRById(UPGRADENOW));
	
	UISystemUpdateDlg systemUpdateDlg(pParent, StringManager::GetStringById(TOUCH_SYSTEM_UPDATE));
    systemUpdateDlg.SetText(text.c_str());
    if(systemUpdateDlg.DoModal() > 0)
	{
		DownloadRelease();
	}
	else if(fRecordIfUserCancel)
	{
		time_t now = time(NULL);
        SetLastCheck(release->GetVersion(), now);
	}
}
BOOL UpgradeModelImpl::GotoSystemDir()
{
    DebugPrintf(DLC_LIUJT, "UpgradeModelImpl::GotoSystemDir Entering");
    int result = chdir("/DuoKan/");
    if(0 != result)
    {
        DebugPrintf(DLC_LIUJT, "UpgradeModelImpl::InstallRelease can't change back to /DuoKan/");
        return FALSE;
    }
    DebugPrintf(DLC_LIUJT, "UpgradeModelImpl::GotoSystemDir End");
    return TRUE;
}
BOOL UpgradeModelImpl::GotoDownloadDir()
{
    DebugPrintf(DLC_LIUJT, "UpgradeModelImpl::GotoDownloadDir Entering");
    int result = chdir(DOWNLOADPATH); //change to user area to unzip tar.gz file
    if(0 != result)
    {
        DebugPrintf(DLC_LIUJT, "UpgradeModelImpl::GotoDownloadDir can't change back to %s", DOWNLOADPATH);
        return FALSE;
    }
    DebugPrintf(DLC_LIUJT, "UpgradeModelImpl::GotoDownloadDir End");
    return TRUE;
}

string UpgradeModelImpl::GetPercentage(string prefix, int percentage)
{
    if(percentage < 0)
    {
        percentage = 0;
    }else if(percentage > 100)
    {
        percentage = 100;
    }
    
    CHAR str[50];
    memset(str, 0,50);
    sprintf(str, "                 %d%%...", percentage);
    return prefix.append("\n").append(str);
}

BOOL UpgradeModelImpl::InstallRelease()
{
	DebugPrintf(DLC_DIAGNOSTIC, "UpgradeModelImpl::InstallRelease begin");

    SQM::GetInstance()->IncCounter(SQM_ACTION_SETTINGPAGE_SYSTEM_REBOOT);

    if(NULL == m_pUpgradeReleaseCandidate)
    {
        return FALSE;
    }
    CDisplay::CacheDisabler cache;
    
    string oldVersion = GetCurVersion(), newVersion = m_pUpgradeReleaseCandidate->GetVersion();

    //unzip the upgrade package, and overwrite the existing DK_System folder
    string zipfile(DOWNLOADPATH);
    zipfile.append(GetFilenameFromRelease(m_pUpgradeReleaseCandidate));

    BOOL result = GotoDownloadDir(); //change to user area to unzip tar.gz file
    if(!result)
    {
        string text(StringManager::GetStringById(UPGRADEFAILED));
        text.append(StringManager::GetStringById(UPGRADEFAILED_CDDIR));
        UIUtility::ShowMessageBox(text);

        GotoSystemDir();
        return FALSE;
    }

    result = GotoSystemDir();
    if(!result)
    {
        string text(StringManager::GetStringById(UPGRADEFAILED));
        text.append(StringManager::GetStringById(UPGRADEFAILED_CDDIR));
        UIUtility::ShowMessageBox(text);
        return FALSE;
    }

    UIMessageBox* msgbox = UIUtility::CreateMessageBox(MB_WITHOUTBTN);
    if(NULL == msgbox)
    {
        GotoSystemDir();
        return FALSE;
    }
    DebugPrintf(DLC_LIUJT, "Here should popup dialog!!!!!!!!!");
    string text(StringManager::GetStringById(UPGRADING));
    msgbox->SetText(GetPercentage(text,0).c_str());
    msgbox->Popup();
    usleep(2000000);

    
    result = GotoDownloadDir(); //change to user area to unzip tar.gz file

    if(!result)
    {
        string text(StringManager::GetStringById(UPGRADEFAILED));
        text.append(StringManager::GetStringById(UPGRADEFAILED_CDDIR));
        UIUtility::ShowMessageBox(text);
        
        delete msgbox;
        msgbox = NULL;

        GotoSystemDir();
        
        return FALSE;
    }

    msgbox->SetText(GetPercentage(text,30).c_str());
    msgbox->UpdateWindow();
    usleep(1000000);
        
    string cmd("tar xzf "+zipfile);
	cmd.append(" -C /mnt/us");
    int systemResult = SystemUtil::ExecuteCommand(cmd.c_str());
	DebugPrintf(DLC_DIAGNOSTIC, "UpgradeModelImpl::InstallRelease sytem Result: %d, cmd:\n%s",  systemResult, cmd.c_str());
    if(0 != systemResult)
    {
        msgbox->Show(FALSE);
        delete msgbox;
        msgbox = NULL;
		
        DebugPrintf(DLC_DIAGNOSTIC,"UpgradeModelImpl::InstallRelease failed to run %s to unzip file: %s",cmd.c_str(), zipfile.c_str());
        cmd.assign("rm -f /mnt/us/DK_System/install/DuoKanInstall.sh");
        systemResult = SystemUtil::ExecuteCommand(cmd.c_str());
        if(0 != systemResult)
        {
            DebugPrintf(DLC_DIAGNOSTIC, "UpgradeModelImpl::InstallRelease failed to run %s", cmd.c_str());
        }

        string text(StringManager::GetStringById(UPGRADEFAILED));
        text.append(StringManager::GetStringById(UPGRADEFAILED_FILE_SYSTEM_ERROR));
        UIUtility::ShowMessageBox(text);

        GotoSystemDir();
        return FALSE;
    }
    msgbox->SetText(GetPercentage(text,80).c_str());
    msgbox->UpdateWindow();
    usleep(1000000);
	   
    result = GotoSystemDir();
    if(!result)
    {
        msgbox->Show(FALSE);
        delete msgbox;
        msgbox = NULL;
        return FALSE;
    }
    
    msgbox->SetText(GetPercentage(text,90).c_str());
    msgbox->UpdateWindow();
    usleep(1000000);    

    text.assign(StringManager::GetStringById(UPGRADE_REBOOTING));
    msgbox->SetText(GetPercentage(text,100).c_str());
    msgbox->UpdateWindow();
    usleep(1000000);

    //delete the downloaded tar file
    if(0 == unlink(zipfile.c_str()))
    {
        DebugPrintf(DLC_DIAGNOSTIC, "UpgradeModelImpl::InstallRelease delete release zip file %s successfully!", zipfile.c_str());
    }
    else
    {
        DebugPrintf(DLC_DIAGNOSTIC, "UpgradeModelImpl::InstallRelease failed to delete release zip file %s!", zipfile.c_str());
    }
    
    //relaunch DuoKan to complete upgrade.
    SystemOperation::Upgrade();

    delete msgbox;
    msgbox = NULL;

    return TRUE;
}

string UpgradeModelImpl::GetLastUpgradeRelease()
{
    DebugPrintf(DLC_LIUJT, "UpgradeModelImpl::GetLastUpgradeRelease Entering");
    CIniFile iniFile;
    iniFile.SetPath(UpgradeHistoryFile);
    string newversion;

    IniKeyMap::iterator keymapiter;

    if (iniFile.ReadFile())
    {
        for(keymapiter=iniFile.keys.begin();keymapiter!=iniFile.keys.end();++keymapiter)
        {
                string upgrade = keymapiter->first;
                if(upgrade.empty())
                {
                    continue;
                }
                newversion = iniFile.GetStringValue(UpgradeSection, NewVersionItem, "");
         }
    }
    
    DebugPrintf(DLC_LIUJT, "UpgradeModelImpl::GetReleaseFromFile new version=%s", newversion.c_str());
    DebugPrintf(DLC_LIUJT, "UpgradeModelImpl::GetLastUpgradeRelease End");
    return newversion;
}

UpgradeType UpgradeModelImpl::GetLastUpgradeType()
{
    DebugPrintf(DLC_LIUJT, "UpgradeModelImpl::GetLastUpgradeType Entering");
    CIniFile iniFile;
    iniFile.SetPath(UpgradeHistoryFile);
    int upgradetype = 0;

    IniKeyMap::iterator keymapiter;

    if (iniFile.ReadFile())
    {
        for(keymapiter=iniFile.keys.begin();keymapiter!=iniFile.keys.end();++keymapiter)
        {
                string upgrade = keymapiter->first;
                if(upgrade.empty())
                {
                    continue;
                }
                upgradetype = iniFile.GetIntValue(UpgradeSection, UpgradeTypeItem, 0);
         }
    }
    
    DebugPrintf(DLC_LIUJT, "UpgradeModelImpl::GetReleaseFromFile End UpgradeType=%d", upgradetype);
    
    return (UpgradeType)upgradetype;
}

string UpgradeModelImpl::GetCurVersion()
{
    DebugPrintf(DLC_LIUJT, "UpgradeModelImpl::GetCurVersion Entering");
    string szVersion(SystemSettingInfo::GetInstance()->GetVersion().GetBuffer());
    DebugPrintf(DLC_LIUJT,"UpgradeModelImpl::GetCurVersion End szVersion is %s",szVersion.c_str());
    return szVersion;
}

void UpgradeModelImpl::GetUpgradeCandiateFromServer(int *error)
{
    if(NULL == error)
    {
        return;
    }
    vector <ReleaseInfo*> vctUpdateList;
    int errcode = 0;
    //http://117.79.157.170:9999/DuoKanServer/servlet/GetLatestRelease?deviceid=d002zz&version=xxyyzz&usergroup=1
    DebugPrintf(DLC_LIUJT, "UpgradeModelImpl::GetUpgradeCandiateFromServer Entering, url=%s, parameters=%s", DK_GETRELEASE_URL,URLParasToGetRelease().c_str());
    string szResultData = Get(DK_GETRELEASE_URL,URLParasToGetRelease(),&errcode);
    DebugPrintf(DLC_LIUJT, "UpgradeModelImpl::GetUpgradeCandiateFromServer get result=%s",szResultData.c_str());
    if(0 == errcode && !szResultData.empty())
    {
        vctUpdateList = ParseXMLReleases(szResultData);
    }
    else
    {
        (*error) = errcode;
        DebugPrintf(DLC_LIUJT, "UpgradeModelImpl::GetUpgradeCandiateFromServer get failed result: %d, or get empty response!!!",errcode);
    }
    ReleaseInfo* candidate = PickupRelease(GetCurVersion(), vctUpdateList);

    SetUpgradeCandidate(candidate);

    DebugPrintf(DLC_LIUJT, "UpgradeModelImpl::GetUpgradeCandiateFromServer End");
}


vector <ReleaseInfo*> UpgradeModelImpl::ParseXMLReleases(std::string _szResultData)
{
    vector <ReleaseInfo*>  UpdateInfoList;
    xmlDocPtr doc = NULL;
    xmlChar *szContent = NULL;
    doc = xmlReadMemory(_szResultData.c_str(),_szResultData.length(),NULL,DK_UPDATE_RESULT_XML_CODE,XML_PARSE_RECOVER);
    if(NULL == doc)
    {
        return UpdateInfoList;
    }
    xmlXPathObjectPtr xpath = getNodeSet(doc,(const xmlChar *)"/"DK_UPDATE_RESULT_NODE_CANDIDATES"/"DK_UPDATE_RESULT_NODE_CANDIDATE);
    if(NULL == xpath)
    {
        xmlFreeDoc(doc);
        doc = NULL;
        return UpdateInfoList;
    }
    xmlNodeSetPtr _nodeset = xpath->nodesetval;
    if(NULL == _nodeset)
    {
        xmlXPathFreeObject(xpath);
        xpath = NULL;
        xmlFreeDoc(doc);
        doc = NULL;
        return UpdateInfoList;
    }

    for(int i = 0;i< _nodeset->nodeNr;i++)
    {
        xmlNodePtr ChildNodePtr = _nodeset->nodeTab[i]->children;
        ReleaseInfo *release= new ReleaseInfo();
        if(NULL == release)
        {
            continue;
        }
        
        while(ChildNodePtr)
        {
            if ((!xmlStrcmp(ChildNodePtr->name, (const xmlChar *)DK_UPDATE_RESULT_NODE_VERSION)))
            {
                szContent = xmlNodeGetContent(ChildNodePtr);
                if(szContent)
                {
                    release->SetVersion((const char*)szContent);
                    xmlFree(szContent);
                }
            }
            else if((!xmlStrcmp(ChildNodePtr->name, (const xmlChar *)DK_UPDATE_RESULT_NODE_TYPE)))
            {
               szContent = xmlNodeGetContent(ChildNodePtr);
                if(szContent)
                {
                    int type = atoi((const char*)szContent);
                    if(type == 0)
                    {
                        release->SetUpdateType(COMPLETE_UPGRADE);
                    }
                    else
                    {
                        release->SetUpdateType(INCREMENTAL_UPGRADE);
                    }
                    xmlFree(szContent);
                }
            }else if((!xmlStrcmp(ChildNodePtr->name, (const xmlChar *)DK_UPDATE_RESULT_NODE_DEPENDENCY)))
            {
               szContent = xmlNodeGetContent(ChildNodePtr);
                if(szContent)
                {
                    release->SetDependency((const char*)szContent);
                    xmlFree(szContent);
                }
            }else if((!xmlStrcmp(ChildNodePtr->name, (const xmlChar *)DK_UPDATE_RESULT_NODE_MD5CODE)))
            {
               szContent = xmlNodeGetContent(ChildNodePtr);
                if(szContent)
                {
                    release->SetMd5code((const char*)szContent);
                    xmlFree(szContent);
                }
            }else if((!xmlStrcmp(ChildNodePtr->name, (const xmlChar *)DK_UPDATE_RESULT_NODE_URL)))
            {
               szContent = xmlNodeGetContent(ChildNodePtr);
                if(szContent)
                {
                    release->SetUrl((const char*)szContent);
                    xmlFree(szContent);
                }
			}else if((!xmlStrcmp(ChildNodePtr->name, (const xmlChar *)DK_UPDATE_RESULT_NODE_FEATURE)))
            {
                szContent = xmlNodeGetContent(ChildNodePtr);
                if(szContent)
                {
                    release->SetFeature((const char*)szContent);
                    xmlFree(szContent);
                }
            }else if((!xmlStrcmp(ChildNodePtr->name, (const xmlChar *)DK_UPDATE_RESULT_NODE_RELEASESIZE)))
            {
                szContent = xmlNodeGetContent(ChildNodePtr);
                if(szContent)
                {
                    long size = strtol((const char*)szContent,NULL,10);
                    release->SetReleaseSize(size);
                    xmlFree(szContent);
                }
            }
			
            ChildNodePtr =  ChildNodePtr->next;
        }

        DebugPrintf(DLC_LIUJT, "UpgradeModelImpl::ParseXMLReleases i=%d, version=%s, type=%d, base=%s, md5=%s, url=%s, size=%ld",i,
            release->GetVersion().c_str(),
            release->GetUpdateType(),
            release->GetDependency().c_str(),
            release->GetMd5code().c_str(),
            release->GetUrl().c_str(),
            release->GetReleaseSize());
        UpdateInfoList.push_back(release);

    }

    xmlXPathFreeObject(xpath);
    xpath = NULL;
    xmlFreeDoc(doc);
    doc = NULL;
    szContent = NULL;
    _nodeset = NULL;

    return UpdateInfoList;
}

