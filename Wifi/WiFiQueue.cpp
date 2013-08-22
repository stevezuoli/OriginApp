#include "Wifi/WiFiQueue.h"
#include "Utility/ThreadHelper.h"
#include "SQM.h"
#include "Model/UpgradeModelImpl.h"
#include "Wifi/WifiManager.h"
#include "Common/SystemOperation.h"
#include "../SQMUploader/SQMUploader.h"
#include "Common/CAccountManager.h"
#include "Framework/CNativeThread.h"
#include "interface.h"
#include "DownloadManager/IDownloader.h"
#include "ScreenSaverUpdate/ScreenSaverUpdateInfo.h"
#include "BookStore/OfflineBookStoreManager.h"
#include "BookStore/BookStoreOrderManager.h"
#include "Thirdparties/MiMigrationManager.h"

using namespace dk::account;
using namespace dk::bookstore;
using namespace dk::thirdparties;

pthread_t WiFiQueue::m_pThread = 0;

SINGLETON_CPP(WiFiQueue);

WiFiQueue::WiFiQueue()
{
    m_pThread = 0;
}

WiFiQueue::~WiFiQueue()
{
    m_pThread = 0;
}

HRESULT WiFiQueue::StartHandleQueueTask()
{
    if (m_pThread != 0)
	{	
		DebugPrintf(DLC_DIAGNOSTIC, "WiFiQueue::StartHandleQueueTask is already running.");
		return S_OK;
	}
    
    int err  = ThreadHelper::CreateThread(&m_pThread, HandleQueue,(void*)this, "WiFiQueue @ HandleQueue", true, 51200, SQM_THREAD_PRIORITY);
    
    //DebugPrintf(DLC_DIAGNOSTIC, "WiFiQueue::StartHandleQueueTask %d#, err(%d)", m_pThread, err);
    if (err != 0)
    {
        m_pThread = 0;
        return err;
    }
    return S_OK;
}


void* WiFiQueue::HandleQueue(void *p)
{
    WiFiQueue * pWiFiQueue = (WiFiQueue*)p;
    //DebugPrintf(DLC_DIAGNOSTIC, "WiFiQueue::HandleQueue begin to check new release!");
    if((NULL == pWiFiQueue) || !WifiManager::GetInstance()->IsConnected())
    {
        DebugPrintf(DLC_DIAGNOSTIC, "WiFiQueue::HandleQueue End: wifi connected=%d", WifiManager::GetInstance()->IsConnected());
        m_pThread = 0;
        ThreadHelper::ThreadExit(0);
        return NULL;
    }
    //DebugPrintf(DLC_DIAGNOSTIC, "WiFiQueue::HandleQueue begin to set thread properties!");
    pthread_detach(pthread_self());
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);

	SystemSettingInfo::GetInstance()->SetNTPTime();//时间同步服务

#if 0
    if(!SystemOperation::SniffNetwork(string(SniffURL), 5))
    {
        DebugPrintf(DLC_DIAGNOSTIC, "WiFiQueue::HandleQueue End with no network");        
        m_pThread = 0;
        ThreadHelper::ThreadExit(0);
        return NULL;
    }
#endif
    DebugPrintf(DLC_DIAGNOSTIC, "WiFiQueue::HandleQueue begin to get migrate stage");
    MiMigrationManager* pMigrateManager = MiMigrationManager::GetInstance();
	if(pMigrateManager)
	{
        string empty;
        pMigrateManager->FetchMigrateStage(empty, empty);
	}

	DebugPrintf(DLC_DIAGNOSTIC, "WiFiQueue::HandleQueue begin to check whether user token valid");
    CAccountManager* pAccount = CAccountManager::GetInstance();
	if(pAccount)
	{
		pAccount->LoginWithTokenAsync();
	}
    usleep(200000);

    DebugPrintf(DLC_DIAGNOSTIC, "WiFiQueue::HandleQueue begin sending offline messages");
    OfflineBookStoreManager* offlineBSManager = OfflineBookStoreManager::GetInstance();
    if (offlineBSManager != 0)
    {
        offlineBSManager->Send();
    }
    usleep(200000);

    DebugPrintf(DLC_DIAGNOSTIC, "WiFiQueue::HandleQueue get special events");
    BookStoreOrderManager* orderBSManager = BookStoreOrderManager::GetInstance();
    if (orderBSManager != 0)
    {
        orderBSManager->FetchSpecialEvents();
    }
    usleep(200000);

    DebugPrintf(DLC_DIAGNOSTIC, "WiFiQueue::HandleQueue begin to post sqm log to server");
    SQMUploader::PostDataToServer();
    usleep(200000);

    DebugPrintf(DLC_DIAGNOSTIC, "WiFiQueue::HandleQueue begin to check version");
    DKAutoUpgrade::UpgradeModelImpl* pUpgradeModel = DKAutoUpgrade::UpgradeModelImpl::GetInstance();
    if(pUpgradeModel->IsAutoDetect())
    {
        pUpgradeModel->CheckNewRelease();
    }
    usleep(200000);
	
	//屏保下载
	ScreenSaverUpdateInfo *pUpdateInfo = ScreenSaverUpdateInfo::GetInstance();
	if(pUpdateInfo)
	{
		if(pUpdateInfo->CheckForUpdate())
		{
			pUpdateInfo->AddWaitListToDownloader();
		}
	}
    usleep(200000);

    // Resuming all waiting downloads
	DebugPrintf(DLC_DIAGNOSTIC, "WiFiQueue::HandleQueue begin IDownloader");
    IDownloader* downloadManager = IDownloader::GetInstance();
    if(downloadManager)
    {
        downloadManager->ResumeAllPauses();
    }

	DebugPrintf(DLC_DIAGNOSTIC, "WiFiQueue::HandleQueue end");
    m_pThread = 0;
    ThreadHelper::ThreadExit(0);
    return NULL;
}
