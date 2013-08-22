
#include <vector>
#include"TouchAppUI/UIDownloadPage.h"
#include "Model/DownloadItemModelImpl.h"
#include "GUI/UISizer.h"
#include "TouchAppUI/UIWifiDialog.h"
#include "CommonUI/UIUtility.h"
#include "Wifi/WifiManager.h"
#include "Framework/CDisplay.h"
#include "../Common/FileManager_DK.h"
#include "Utility/ThreadHelper.h"
#include "SQM.h"
#include "I18n/StringManager.h"
#include "BookStore/BookDownloadController.h"
#include "Common/WindowsMetrics.h"
#include "GUI/GUISystem.h"

using namespace std;
using namespace dk::bookstore;
using namespace WindowsMetrics;

UIDownloadPage::UIDownloadPage():
UIPage()
	, m_imgLogo()
	, m_btnSearch()
	, m_lstDownload()
	, m_txtTotalDownload()
	, m_iTotalPage(0)
	, m_iCurPage(0)
	, m_iDownloadNum(0)
	, m_iDownloadNumPerPage(0)
	, m_bIsDisposed(FALSE)
	, m_pupdateth()
	, m_ppCurDownloadList(NULL)
	, m_ShowTaskList()
{
	Init();
}
UIDownloadPage::~UIDownloadPage()
{
	Dispose();
}

void UIDownloadPage::Dispose()
{
	UIPage::Dispose();
	OnDispose(m_bIsDisposed);
}

void UIDownloadPage::OnDispose(BOOL bIsDisposed)
{
	if (bIsDisposed)
	{
		return;
	}
	m_ShowTaskList.clear();
	m_bIsDisposed = TRUE;
	Finalize();
}

void UIDownloadPage::Init()
{
	UIPage::Init();

	m_txtTotalDownload.SetFontSize(GetWindowFontSize(UIPageNumIndex));
	m_txtTotalDownload.SetText(StringManager::GetStringById(TOTAL_OF_12));

	m_pageNum.SetFontSize(GetWindowFontSize(UIPageNumIndex));
	m_pageNum.SetEnglishGothic();
	m_pageNum.SetText(StringManager::GetStringById(HALF_PAGE));
	m_pageNum.SetAlign(ALIGN_RIGHT);

	m_lstDownload.SetItemHeight(GetWindowMetrics(UIDownloadListItemHeightIndex));
    SetFocus(false);

	AppendChild(&m_lstDownload);
	AppendChild(&m_txtTotalDownload);
	AppendChild(&m_pageNum);
}

void UIDownloadPage::MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight)
{
	UIPage::MoveWindow(iLeft, iTop, iWidth, iHeight);
    if (!m_windowSizer)
    {
        UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
        if (mainSizer)
        {
            UISizerItem* listItem = mainSizer->Add(&m_lstDownload, UISizerFlags(1).Expand().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIHorizonMarginIndex)));
            listItem->SetFlag(listItem->GetFlag() | dkRESERVE_SPACE_EVEN_IF_HIDDEN);

            UISizer* pageInfoSizer = new UIBoxSizer(dkHORIZONTAL);
            if (pageInfoSizer)
            {
                pageInfoSizer->Add(&m_txtTotalDownload, 1);
                pageInfoSizer->Add(&m_pageNum, 1);
                mainSizer->Add(pageInfoSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIHorizonMarginIndex)));
            }
            mainSizer->AddSpacer(20);
            SetSizer(mainSizer);
        }
    }
    Layout();

	//int HorizontalMargin = 30;
	//int VerticalMargin = 0;
	//int Left = 0;
	//int Width = m_txtTotalDownload.GetTextWidth();
	//int Height = m_txtTotalDownload.GetTextHeight();
	//int Top  = iHeight - Height;

	//m_txtTotalDownload.MoveWindow(HorizontalMargin, Top, 100, Height);

	//Width = m_btnPageNo.GetTextWidth();
	//Height = m_btnPageNo.GetTextHeight();
	//m_btnPageNo.MoveWindow(iWidth - Width - HorizontalMargin, Top, Width, Height);

	//Top -= VerticalMargin;
	//m_lstDownload.MoveWindow(Left, 0, iWidth, Top);
	InitDownloadList(true);
}

void UIDownloadPage::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
	switch (dwCmdId)
	{
	case ID_MNU_START_DOWNLOAD:
		{
			SQM::GetInstance()->IncCounter(SQM_ACTION_DOWNLOADPAGE_TASKLIST_LEFTKEY_START);
			bool bWifiConnected = UIUtility::CheckAndReConnectWifi();
			if(bWifiConnected)
			{
				this->m_lstDownload.GetItemModelList()[m_lstDownload.GetSelectedItem()]->ResumeFromPauseUrl();
				InitDownloadList();
				Repaint();
			}
		}
		break;
	case ID_MNU_PAUSE_DOWNLOAD:
		{
			SQM::GetInstance()->IncCounter(SQM_ACTION_DOWNLOADPAGE_TASKLIST_LEFTKEY_PAUSE);
			this->m_lstDownload.GetItemModelList()[m_lstDownload.GetSelectedItem()]->PauseDownloadUrl();
			InitDownloadList();
			CDisplay::CacheDisabler cache;
			Repaint();
		}
		break;
	case ID_MNU_DELETE_DOWNLOAD:
		{
			DebugPrintf(DLC_UISYSTEMSETTING, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            UIContainer* pParent = GUISystem::GetInstance()->GetTopFullScreenContainer();
			UIMessageBox messagebox(pParent, StringManager::GetStringById(SURE_DELETE), MB_OK | MB_CANCEL);
			if(MB_OK == messagebox.DoModal())
			{
				SQM::GetInstance()->IncCounter(SQM_ACTION_DOWNLOADPAGE_TASKLIST_LEFTKEY_DELETE);
				this->m_lstDownload.GetItemModelList()[m_lstDownload.GetSelectedItem()]->DeleteDownloadUrl();
				InitDownloadList();
				CDisplay::CacheDisabler cache;
				Repaint();
			}
		}
		break;
	case ID_MNU_READ_BOOK:
		break;
	default:
		break;
	}
}

HRESULT UIDownloadPage::Draw(DK_IMAGE drawingImg)
{
	DebugPrintf(DLC_XU_KAI,"UIDownloadPage::Draw begin UpdateNavigationButton()");
	UpdateNavigationButton();
	DebugPrintf(DLC_XU_KAI,"UIDownloadPage::Draw beginaaaaa");
	//UpdateDownloadListbox();
	DebugPrintf(DLC_XU_KAI,"UIDownloadPage::Draw begin");
	return UIPage::Draw(drawingImg);
}

void UIDownloadPage::Show(BOOL bIsShow, BOOL bIsRepaint)
{
	DebugPrintf(DLC_XU_KAI,"UIDownloadPage::Show(bIsShow : %d ; m_bIsVisible : %d",bIsShow,m_bIsVisible);
	if (m_bIsVisible == bIsShow)
	{
		return;
	}

	DebugPrintf(DLC_XU_KAI,"UIWindow::Show(%d) begin",bIsShow);

	UIWindow::Show(bIsShow, bIsRepaint);
	DebugPrintf(DLC_XU_KAI,"UIWindow::Show(%d) end",bIsShow);
}

void UIDownloadPage::StartUpdateThread()
{
	//pthread_create(&m_pupdateth, NULL, CDownloadList::UpdateDownloadProgressPthread, &this->m_lstDownload);
	ThreadHelper::CreateThread(&m_pupdateth, CDownloadList::UpdateDownloadProgressPthread, &this->m_lstDownload, "UIDownloadPage @ UpdateDownloadProgressPthread");
}

void UIDownloadPage::StopUpdateThread()
{
	ThreadHelper::CancelThread(m_pupdateth);
	ThreadHelper::JoinThread(m_pupdateth,NULL);
	m_pupdateth = 0;
}

void UIDownloadPage::UpdateDeletedOrAddedItem()
{
	IDownloader *_pDownloadManager = DownloaderImpl::GetInstance();
	int iDownLoadNum = _pDownloadManager->GetDownloadTaskNum();
    std::vector<std::string> strOldTaskList = m_ShowTaskList;
	m_ShowTaskList.clear();
	for(int i = 0; i < iDownLoadNum; i++)
	{
		IDownloadTask * _pTask = _pDownloadManager->GetTaskInfoByIndex(i);
		if(_pTask && _pTask->GetState() != IDownloadTask::DONE  && _pTask->GetType() != IDownloadTask::SCREENSAVER)
		{
			m_ShowTaskList.push_back(_pTask->GetOrigUrlID());
		}
		else
		{
			if (_pTask &&  _pTask->GetType() != IDownloadTask::SCREENSAVER)
			{
				for(size_t j = 0; j < strOldTaskList.size(); j++)
				{
					string urlid = _pTask->GetOrigUrlID();
					if(urlid == strOldTaskList[j])
					{
						m_ShowTaskList.push_back(urlid);
					}
				}
			}
		}
	}
	m_iDownloadNum = m_ShowTaskList.size();
	m_iDownloadNumPerPage = m_lstDownload.GetMaxDisplayNum();
	if(m_iDownloadNumPerPage <= 0 || m_iDownloadNum <= 0)
	{
        m_lstDownload.SetVisible(false);
		return;
	}
	//    assert(m_iDownloadNumPerPage > 0);
	m_iTotalPage = (m_iDownloadNum + m_iDownloadNumPerPage - 1) / m_iDownloadNumPerPage;
	if(m_iTotalPage <= 0)
	{
		m_iTotalPage = 1;
	}
	if(m_iCurPage < 0)
	{
		m_iCurPage =0;
	}
	else if(m_iCurPage >= m_iTotalPage)
	{
		m_iCurPage = m_iTotalPage -1;
	}

	CHAR str[50] = {0};
	memset(str, 0, 50);
	sprintf(str, "%s %d %s",StringManager::GetPCSTRById(BOOK_TOTAL),m_iDownloadNum,StringManager::GetPCSTRById(BOOK_TIAO));
	m_txtTotalDownload.SetText(CString(str));

	UpdateNavigationButton();
	m_lstDownload.SetVisible(false);
	OnChildSetFocus(&m_lstDownload);
	m_lstDownload.SetFocus(true);
	m_lstDownload.SetVisible(true);
}

void UIDownloadPage::InitDownloadList(BOOL bIsUpdatePageNum)
{
	IDownloader *_pDownloadManager = IDownloader::GetInstance();
	int iDownLoadNum = _pDownloadManager->GetDownloadTaskNum();
	m_ShowTaskList.clear();
	for(int i = 0; i < iDownLoadNum; i++)
	{
		IDownloadTask * _pTask = _pDownloadManager->GetTaskInfoByIndex(i);
		if(_pTask && _pTask->GetState() != IDownloadTask::DONE && _pTask->GetType() != IDownloadTask::SCREENSAVER)
		{
			m_ShowTaskList.push_back(_pTask->GetOrigUrlID());
		}
	}
	m_iDownloadNum = m_ShowTaskList.size();
	m_iDownloadNumPerPage = m_lstDownload.GetMaxDisplayNum();
	if(m_iDownloadNumPerPage <= 0)
	{
		this->m_lstDownload.Show(false);
		return;
	}
	//    assert(m_iDownloadNumPerPage > 0);
	m_iTotalPage = (m_iDownloadNum + m_iDownloadNumPerPage - 1) / m_iDownloadNumPerPage;
	if(m_iTotalPage <= 0)
	{
		m_iTotalPage = 1;
	}
	if(bIsUpdatePageNum)
	{
		m_iCurPage = 0;
	}
	else if(m_iCurPage < 0)
	{
		m_iCurPage =0;
	}
	else if(m_iCurPage >= m_iTotalPage)
	{
		m_iCurPage = m_iTotalPage -1;
	}
	if( NULL == m_ppCurDownloadList)
	{
		m_ppCurDownloadList = new CDownloadItemModelImpl*[m_iDownloadNumPerPage ];
		if (NULL == m_ppCurDownloadList)
		{
			return;
		}
		memset(m_ppCurDownloadList, 0, m_iDownloadNumPerPage * sizeof(CDownloadItemModelImpl *));
	}

	CHAR str[50] = {0};
	memset(str, 0, 50);
	sprintf(str, "%s %d %s",StringManager::GetPCSTRById(BOOK_TOTAL),m_iDownloadNum,StringManager::GetPCSTRById(BOOK_TIAO));
	m_txtTotalDownload.SetText(CString(str));

	UpdateNavigationButton();
	m_lstDownload.SetVisible(false);
	OnChildSetFocus(&m_lstDownload);
	m_lstDownload.SetFocus(true);
	m_lstDownload.SetVisible(true);
    UpdateDownloadListbox();
}

void UIDownloadPage::UpdateNavigationButton()
{
	CHAR str[50] = {0};
	memset(str, 0, 50);
	sprintf(str, "%d/%d %s", m_iCurPage + 1, m_iTotalPage,StringManager::GetPCSTRById(BOOK_PAGE));
	m_pageNum.SetText(CString(str));
}

void UIDownloadPage::UpdateDownloadListbox()
{
	DebugPrintf(DLC_XU_KAI,"enter UIDownloadPage::UpdateDownloadListbox :m_iDownloadNum is %d",m_iDownloadNum);
	INT32 _iIndex = 0;
	if(m_iDownloadNum <= 0)
	{
		m_lstDownload.SetDownloadList(NULL,0);
		m_lstDownload.SetVisible(false);
		m_txtTotalDownload.SetVisible(false);
		m_pageNum.SetVisible(false);
		return;
	}
	else
	{
		m_txtTotalDownload.SetVisible(true);
		m_pageNum.SetVisible(true);
	}
	INT32 _iNextPageIndex = (m_iCurPage+1) >= m_iTotalPage?m_iDownloadNum : (m_iCurPage+1) * m_iDownloadNumPerPage;
	DebugPrintf(DLC_XU_KAI,"_iNextPageIndex : %d",_iNextPageIndex);
	for (INT32 i = m_iCurPage * m_iDownloadNumPerPage; i < _iNextPageIndex; i++)
	{
		DebugPrintf(DLC_XU_KAI,"i : %d, _iindex: %d, %x",i, _iIndex, m_ppCurDownloadList[_iIndex]);
		if (NULL == m_ppCurDownloadList[_iIndex])
		{
			CDownloadItemModelImpl *pBookItemModel = new CDownloadItemModelImpl(m_ShowTaskList[i]);
			if(pBookItemModel)
			{
				m_ppCurDownloadList[_iIndex] = pBookItemModel;
			}
			else
			{
				break;
			}
		}
		else
		{
			m_ppCurDownloadList[_iIndex]->SetTaskUrl(m_ShowTaskList[i]);
		}

		_iIndex++;
	}
	DebugPrintf(DLC_XU_KAI, "after setURL");
	m_lstDownload.SetDownloadList((IDownloadItemModel **)m_ppCurDownloadList, _iIndex);
	DebugPrintf(DLC_XU_KAI,"leave UIDownloadPage::UpdateDownloadListbox :m_iDownloadNum is %d",m_iDownloadNum);
}

void UIDownloadPage::Finalize()
{
	if ( m_ppCurDownloadList )
	{
		for (INT32 i = 0; i < m_iDownloadNumPerPage; i++)
		{
			if (NULL != m_ppCurDownloadList[i])
			{
				delete m_ppCurDownloadList[i];
				m_ppCurDownloadList[i] = NULL;
			}
		}

		delete []m_ppCurDownloadList;
		m_ppCurDownloadList = NULL;
	}
}

void UIDownloadPage::HandlePageUpDown(BOOL fPageDown)
{
	if (fPageDown)
	{
		m_iCurPage++;
		m_iCurPage = (m_iCurPage>=m_iTotalPage) ? 0 : m_iCurPage;
	}

	if (!fPageDown)
	{
		m_iCurPage--;
		m_iCurPage = (m_iCurPage<0) ? (m_iTotalPage-1) : m_iCurPage;
	}
	m_lstDownload.SetSelectedItem(0);
	UpdateDownloadListbox();
	Repaint();
}

void UIDownloadPage::OnNotify(UIEvent rEvent)
{
	UIPage::OnNotify(rEvent);
}

UIPage* UIDownloadPage::GetUIPage()
{
#ifdef KINDLE_FOR_TOUCH
    return m_pParent ? m_pParent->GetUIPage() : this;
#else
    return this;
#endif
}

void UIDownloadPage::UpdateSectionUI(bool bIsFull)
{
	if(!m_pDialog || (m_pDialog && !m_pDialog->IsVisible()))
	{
		if(bIsFull)
		{
			UpdateDeletedOrAddedItem();
			Repaint();
		}
		else
		{
			m_lstDownload.UpdateDownloadProgress(&this->m_lstDownload);
		}
	}
	return;
}

void UIDownloadPage::OnLoad()
{
	SetVisible(TRUE);
	UIPage::OnLoad();
    Layout();
}


void UIDownloadPage::OnUnLoad()
{
	UIPage::OnUnLoad();
	SetVisible(FALSE);
}

void UIDownloadPage::OnWifiMessage(SNativeMessage event)
{
	if(!m_bIsVisible)
	{
		return;
	}

	if(ALLRECEIVER != event.iParam1)
	{
		LPSTR pReceiver = (LPSTR)event.iParam1;

		if(!pReceiver || strcmp(this->GetClassName(),pReceiver) != 0)
		{
			return;
		}
	}

	switch(event.iParam2)
	{
	case UPDATE_WIFI_STATUS:

		{
			UpdateTitleBar();
		}
		break;

	case POWERONOFF_WIFI:
		{
			UpdateTitleBar();
			WifiManager* wifiManager = WifiManager::GetInstance();
			if(wifiManager && wifiManager->IsPowerOn())
			{
				if(!wifiManager->IsConnected())
				{
					DebugPrintf(DLC_LIUJT, "UIDownloadPage::OnWifiMessage  No wifi connected, begin to auto join!");
					wifiManager->StartAutoJoin(WifiStatusCallBack, ConnectWifiCallBack);
				}
				else
				{
					DebugPrintf(DLC_LIUJT, "UIDownloadPage::OnWifiMessage  Has one wifi connected!!!");
				}
			}
			else
			{
				DebugPrintf(DLC_LIUJT, "UIDownloadPage::OnWifiMessage  Has no power now!!!");
			}
		}
		break;

	case CONNECT_WIFI:
		{
			UpdateTitleBar();
			ConnectionMessage* cm = (ConnectionMessage*)event.iParam3;
			if(cm && !cm->GetWifiConnected())
			{
				delete cm;
				cm = NULL;

				UIWifiDialog m_WifiDialog(this);
				CDisplay* pDisplay = CDisplay::GetDisplay();
				if(pDisplay)
				{
					m_WifiDialog.MoveWindow(0,0,pDisplay->GetScreenWidth(), pDisplay->GetScreenHeight());
					m_WifiDialog.DoModal();
				}
			}
		}

		break;
	default:
		break;

	}
}

void UIDownloadPage::ConnectWifiCallBack(BOOL connected, string ssid, string password, string strIdentity)
{
	ConnectionMessage* cm = new ConnectionMessage(connected, ssid, password, strIdentity);
	if(!cm)
	{
		DebugPrintf(DLC_LIUJT, "Failed to create ConnectionMessage for (connected=%d, ssid=%s, password=%s)", connected, ssid.c_str(), password.c_str());
		return;
	}
	SNativeMessage msg;
	msg.iType = KMessageWifiChange;
	msg.iParam1 = (UINT32)GetClassNameStatic();
	msg.iParam2 = CONNECT_WIFI;
	msg.iParam3 = (UINT32)cm;
	INativeMessageQueue::GetMessageQueue()->Send(msg);

}

void UIDownloadPage::WifiStatusCallBack()
{
	SNativeMessage msg;
	msg.iType = KMessageWifiChange;
	msg.iParam1 = (UINT32)GetClassNameStatic();
	msg.iParam2 = (UINT32)UPDATE_WIFI_STATUS;
	msg.iParam3 = (UINT32)DEFAULT;
	INativeMessageQueue::GetMessageQueue()->Send(msg);
}

void UIDownloadPage::WifiPowerOnCallBack()
{

	SNativeMessage msg;
	msg.iType = KMessageWifiChange;
	msg.iParam1 = (UINT32)GetClassNameStatic();
	msg.iParam2 = POWERONOFF_WIFI;
	INativeMessageQueue::GetMessageQueue()->Send(msg);
}
void UIDownloadPage::UpdateTitleBar()
{
	UITitleBar *pTitleBar = (CDisplay::GetDisplay()->GetCurrentPage())->GetTitleBar();
	if(pTitleBar && pTitleBar->IsVisible())
	{
		pTitleBar->UpdateSignal();
	}
}

void UIDownloadPage::OnEnter()
{
	StartUpdateThread();
}

void UIDownloadPage::OnLeave()
{
	StopUpdateThread();
}
