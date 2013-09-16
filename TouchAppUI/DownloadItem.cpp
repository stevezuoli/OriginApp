#include <unistd.h>
#include <pthread.h>
#include <string>
#include "TouchAppUI/DownloadItem.h"
#include "TouchAppUI/DkReaderPage.h"
#include "CommonUI/CPageNavigator.h"
#include "TouchAppUI/UIDownloadPage.h"
#include "TouchAppUI/UISingleTextInputDialog.h"
#include "TouchAppUI/BookOpenManager.h"
#include "Framework/CNativeThread.h"
#include "Framework/CDisplay.h"
#include "Common/FileManager_DK.h"
#include "GUI/UIMessageBox.h"
#include "GUI/CTpGraphics.h"
#include "Common/WindowsMetrics.h"
#include "I18n/StringManager.h"
#include "DkSPtr.h"

using namespace std;
using namespace DkFormat;
using namespace WindowsMetrics;

CDownloadItem::CDownloadItem()
    : UIContainer(NULL, IDSTATIC)
    ,m_textBookNameLabel(this, IDSTATIC)
    ,m_textDownLoadStatus(this, IDSTATIC)
    ,m_textDownLoadProgress(this, IDSTATIC)
    ,m_proDownloadSeparatorbar(this, IDSTATIC)
    ,m_pDownloadSizer(NULL)
    ,m_pDeleteSizer(NULL)
    ,m_pPauseSizer(NULL)
    ,m_pItemModel(NULL)
    ,m_bIsDisposed(FALSE)
    ,m_OldPrecent(0)
    ,m_OldState(IDownloadTask::NONE)
{
}

CDownloadItem::CDownloadItem(UIContainer* pParent, const DWORD dwId)
    : UIContainer(pParent,dwId)
    ,m_textBookNameLabel(this, IDSTATIC)
    ,m_textDownLoadStatus(this, IDSTATIC)
    ,m_textDownLoadProgress(this, IDSTATIC)
    ,m_proDownloadSeparatorbar(this, IDSTATIC)
    ,m_pItemModel(NULL)
    ,m_bIsDisposed(FALSE)
    ,m_OldPrecent(0)
    ,m_OldState(IDownloadTask::NONE)
{
    if (pParent != NULL)
    {
        pParent->AppendChild(this);
    }
}



CDownloadItem::~CDownloadItem()
{
}

void CDownloadItem::SetDownloadItemModel(IDownloadItemModel *pItemModel)
{
    m_pItemModel = pItemModel;
    InitItem();
}

HRESULT CDownloadItem::DrawFocus(DK_IMAGE drawingImg)
{
    return S_OK;
}

void CDownloadItem::SetFocus(BOOL bIsFocus)
{
    if (m_bIsFocus != bIsFocus)
    {
        m_bIsFocus = bIsFocus;
    }
}

void CDownloadItem::UpdateProgress()
{
    if(m_pItemModel)
    {
        DebugPrintf(DLC_XU_KAI,"CDownloadItem::UpdateProgress m_OldPrecent=%d;  : m_pItemModel is not NULL", m_OldPrecent);
        int _iPercent = m_pItemModel->GetDownloadPercentage();
        string sNewName = m_pItemModel->GetBookName();
        CDisplay::CacheDisabler cache;
        DebugPrintf(DLC_XU_KAI,"CDownloadItem::UpdateProgress _iPercent is %d; m_OldPrecent : %d",_iPercent,m_OldPrecent);
        if(_iPercent != m_OldPrecent && _iPercent >= 0)
        {
            char _sztmp[64] = {0};
            sprintf(_sztmp,"%d%%",_iPercent);
            m_OldPrecent = _iPercent;
            m_textDownLoadProgress.SetText(_sztmp);
            m_proDownloadSeparatorbar.SetProgress(_iPercent);
            UpdateWindow();
        }

        if(sNewName != m_szOldBookname && !sNewName.empty())
        {
            m_textBookNameLabel.SetText(CString(sNewName.c_str()));
            m_textBookNameLabel.UpdateWindow();
            m_szOldBookname = sNewName;
        }
        UpdateButtonStatus();
        //DebugPrintf(DLC_XU_KAI,"CDownloadItem::UpdateProgress() end");
    }
}

void CDownloadItem::UpdateButtonStatus()
{
    IDownloadTask::DLState curState = m_pItemModel->GetDownloadStatus();
    if(m_OldState !=  curState)
    {
        m_OldState = curState;
        m_textDownLoadStatus.SetText(GetDownloadStatus().c_str());
        if (m_pItemModel && m_pItemModel->CanResume())
        {
            if ((curState == IDownloadTask::WORKING) || (curState == IDownloadTask::WAITING))
            {
                m_pDownloadSizer->Show(false);
                m_pPauseSizer->Show(true);
            }
            else if ((curState == IDownloadTask::PAUSED) || (curState == IDownloadTask::CANCELED) || (curState == IDownloadTask::FAILED))
            {
                m_pDownloadSizer->Show(true);
                m_pPauseSizer->Show(false);
            }
            else if (curState == IDownloadTask::DONE)
            {
                m_pDownloadSizer->Show(false);
                m_pPauseSizer->Show(false);
            }
        }
        UpdateWindow();
    }
}

void CDownloadItem::InitItem()
{
    if (NULL == m_pItemModel)
    {
        return;
    }
    m_btnTaskDownload.Initialize(ID_MNU_START_DOWNLOAD, "", GetWindowFontSize(UIDownloadListItemIndex));
    m_btnTaskDownload.SetIcons(ImageManager::GetImage(IMAGE_TOUCH_ICON_DOWNLOAD), ImageManager::GetImage(IMAGE_TOUCH_ICON_DOWNLOAD), UIComplexButton::ICON_LEFT);
    m_btnTaskDownload.SetElemSpacing(0);
    m_textTaskDownload.SetText(m_pItemModel && m_pItemModel->IsUploadTask() ? StringManager::GetStringById(CLOUD_UPLOAD) : StringManager::GetStringById(TOUCH_DOWNLOAD));
    m_textTaskDownload.SetFontSize(GetWindowFontSize(UIDownloadListItemIndex));
    m_textTaskDownload.SetForeColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));

    m_btnTaskDelete.Initialize(ID_MNU_DELETE_DOWNLOAD, "", GetWindowFontSize(UIDownloadListItemIndex));
    m_btnTaskDelete.SetIcons(ImageManager::GetImage(IMAGE_ICON_DOWNLOADFAILD), ImageManager::GetImage(IMAGE_ICON_DOWNLOADFAILD), UIComplexButton::ICON_LEFT);
    m_btnTaskDelete.SetElemSpacing(0);
    m_textTaskDelete.SetText(StringManager::GetStringById(BOOK_DELETE));
    m_textTaskDelete.SetFontSize(GetWindowFontSize(UIDownloadListItemIndex));
    m_textTaskDelete.SetForeColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));

    m_btnTaskPause.Initialize(ID_MNU_PAUSE_DOWNLOAD, "", GetWindowFontSize(UIDownloadListItemIndex));
    m_btnTaskPause.SetIcons(ImageManager::GetImage(IMAGE_TOUCH_ICON_PAUSE), ImageManager::GetImage(IMAGE_TOUCH_ICON_PAUSE), UIComplexButton::ICON_LEFT);
    m_btnTaskPause.SetElemSpacing(0);
    m_textTaskPause.SetText(StringManager::GetStringById(PAUSE));
    m_textTaskPause.SetFontSize(GetWindowFontSize(UIDownloadListItemIndex));
    m_textTaskPause.SetForeColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));

    m_textBookNameLabel.SetFontSize(GetWindowFontSize(UIDownloadListItemNameLabelIndex));
    m_textBookNameLabel.SetBackColor(ColorManager::knWhite);
    m_textBookNameLabel.SetBackTransparent(FALSE);
    m_textBookNameLabel.SetText(m_pItemModel->GetBookName().c_str());

    m_proDownloadSeparatorbar.SetProgress(m_pItemModel->GetDownloadPercentage());
    m_proDownloadSeparatorbar.SetMaximum(100);

    string downloadStatus = GetDownloadStatus();

    m_textDownLoadStatus.SetFontSize(GetWindowFontSize(UIDownloadListItemIndex));
    m_textDownLoadStatus.SetForeColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
    m_textDownLoadStatus.SetBackColor(ColorManager::knWhite);
    m_textDownLoadStatus.SetBackTransparent(false);
    m_textDownLoadStatus.SetText(downloadStatus.c_str());

    m_textDownLoadProgress.SetFontSize(GetWindowFontSize(UIDownloadListItemIndex));
    m_textDownLoadProgress.SetAlign(ALIGN_RIGHT);
    m_textDownLoadProgress.SetBackColor(ColorManager::knWhite);
    char _sztmp[8] = {0};
    m_OldPrecent = m_pItemModel->GetDownloadPercentage();
    sprintf(_sztmp,"%d%%",m_OldPrecent);
    m_textDownLoadProgress.SetText(_sztmp);

    AppendChild(&m_textBookNameLabel);
    AppendChild(&m_textDownLoadStatus);
    AppendChild(&m_textDownLoadProgress);
    AppendChild(&m_proDownloadSeparatorbar);
    AppendChild(&m_btnTaskDownload);
    AppendChild(&m_btnTaskDelete);
    AppendChild(&m_btnTaskPause);
    AppendChild(&m_textTaskDownload);
    AppendChild(&m_textTaskDelete);
    AppendChild(&m_textTaskPause);
   
    if (!m_windowSizer)
    {
        const int btnWidth = GetWindowMetrics(UIDownloadListItemButtonWidthIndex);
        const int btnHeight = GetWindowMetrics(UIDownloadListItemButtonHeightIndex);
        m_btnTaskDelete.SetMinSize(dkSize(btnWidth, btnHeight));
        m_btnTaskPause.SetMinSize(dkSize(btnWidth, btnHeight));
        m_btnTaskDownload.SetMinSize(dkSize(btnWidth, btnHeight));
        m_proDownloadSeparatorbar.SetMinHeight(11);

        UISizer* mainSizer = new UIBoxSizer(dkHORIZONTAL);

        if (mainSizer)
        {
            UISizer* leftSizer = new UIBoxSizer(dkVERTICAL);
            if (leftSizer)
            {
                leftSizer->Add(&m_textBookNameLabel, UISizerFlags().Expand().Border(dkBOTTOM, GetWindowMetrics(UIDownloadListItemNameLabelBottomMarginIndex)));
                leftSizer->Add(&m_proDownloadSeparatorbar, UISizerFlags().Expand());

                UISizer* bottomSizer = new UIBoxSizer(dkHORIZONTAL);
                if (bottomSizer)
                {
                    bottomSizer->Add(&m_textDownLoadStatus, 1);
                    bottomSizer->Add(&m_textDownLoadProgress, 1);
                    leftSizer->Add(bottomSizer, UISizerFlags().Expand().Border(dkTOP, GetWindowMetrics(UIDownloadListItemStatusTopMarginIndex)));
                }
                mainSizer->Add(leftSizer, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL).Border(dkRIGHT, 10));
            }

            m_pDownloadSizer = new UIBoxSizer(dkVERTICAL);
            if (m_pDownloadSizer)
            {
                m_pDownloadSizer->AddSpacer(GetWindowMetrics(UIDownloadListItemButtonMarginIndex));
                m_pDownloadSizer->Add(&m_btnTaskDownload, UISizerFlags().Align(dkALIGN_CENTER_HORIZONTAL).Border(dkBOTTOM, 10));
                m_pDownloadSizer->Add(&m_textTaskDownload, UISizerFlags().Align(dkALIGN_CENTER_HORIZONTAL));
                mainSizer->Add(m_pDownloadSizer, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
            }

            m_pPauseSizer = new UIBoxSizer(dkVERTICAL);
            if (m_pPauseSizer)
            {
                m_pPauseSizer->AddSpacer(GetWindowMetrics(UIDownloadListItemButtonMarginIndex));
                m_pPauseSizer->Add(&m_btnTaskPause, UISizerFlags().Align(dkALIGN_CENTER_HORIZONTAL).Border(dkBOTTOM, 10));
                m_pPauseSizer->Add(&m_textTaskPause, UISizerFlags().Align(dkALIGN_CENTER_HORIZONTAL));
                mainSizer->Add(m_pPauseSizer, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
            }

            m_pDeleteSizer = new UIBoxSizer(dkVERTICAL);
            if (m_pDeleteSizer)
            {
                m_pDeleteSizer->AddSpacer(GetWindowMetrics(UIDownloadListItemButtonMarginIndex));
                m_pDeleteSizer->Add(&m_btnTaskDelete, UISizerFlags().Align(dkALIGN_CENTER_HORIZONTAL).Border(dkBOTTOM, 10));
                m_pDeleteSizer->Add(&m_textTaskDelete, UISizerFlags().Align(dkALIGN_CENTER_HORIZONTAL));
                mainSizer->Add(m_pDeleteSizer, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Border(dkLEFT, 14));
            }

            SetSizer(mainSizer);
        }
    }
    UpdateButtonStatus();
}

HRESULT CDownloadItem::DrawBackGround(DK_IMAGE drawingImg)
{
    HRESULT hr(S_OK);
    CTpGraphics grf(drawingImg);
    
    RTN_HR_IF_FAILED(grf.FillRect(0, 0, m_iWidth, m_iHeight, ColorManager::knWhite));

    return hr;
}

void CDownloadItem::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    if (m_pParent)
    {
        m_pParent->OnCommand(dwCmdId, this, dwParam);
    }
}

string CDownloadItem::GetDownloadStatus()
{
    string _szstatus = "";
    bool isUploadTask = m_pItemModel->IsUploadTask();
    switch(m_pItemModel->GetDownloadStatus())
    {
        case IDownloadTask::WORKING:
            _szstatus = isUploadTask ? StringManager::GetStringById(UPLOADING) : StringManager::GetStringById(DOWNLOADING);
            break;
        case IDownloadTask::WAITING:
            _szstatus = StringManager::GetStringById(WAITING);
            break;
        case IDownloadTask::PAUSED:
        case IDownloadTask::CANCELED:
            _szstatus = StringManager::GetStringById(PAUSE);
            break;
        case IDownloadTask::DONE:
            _szstatus = isUploadTask ? StringManager::GetStringById(UPLOAD_FINISH) : StringManager::GetStringById(DOWNLOAD_FINISH);
            break;
        case IDownloadTask::FAILED:
            _szstatus = isUploadTask ? StringManager::GetStringById(UPLOAD_FAILED) : StringManager::GetStringById(DOWNLOAD_FAILED);
            break;
        default:
            _szstatus = StringManager::GetStringById(WAITING);
            break;
    }

    return _szstatus;
}

CDownloadList::CDownloadList()
    : UICompoundListBox(NULL,IDSTATIC), m_bIsDisposed(FALSE)
{
    Init();
}


CDownloadList::CDownloadList(UIContainer* pParent, const DWORD dwId)
    : UICompoundListBox(pParent, dwId), m_bIsDisposed(FALSE)
{
    if (pParent != NULL)
    {
        pParent->AppendChild(this);
    }

    Init();
}

IDownloadItemModel* CDownloadList::GetListItem(INT32 iItemIndex)
{
    if (iItemIndex < 0 || iItemIndex >= m_iVisibleItemNum)
    {
        return NULL;
    }    

    return m_Itemlist[iItemIndex];
}

bool CDownloadList::OnItemClick(INT32 iSelectedItem)
{
    if (iSelectedItem < 0 || iSelectedItem >= m_iVisibleItemNum)
    {
        return TRUE;
    }
    UICompoundListBox::OnItemClick(iSelectedItem);
    if(NULL == m_Itemlist[iSelectedItem])
    {
        return FALSE;
    }

    return FALSE;
}

void* CDownloadList::UpdateDownloadProgressPthread(void *_pv)
{
    DebugPrintf(DLC_XU_KAI,"CDownloadList::UpdateDownloadProgress begin pthread");
    pthread_detach(pthread_self());
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    while(1)
    {
        sleep(1);
        SNativeMessage msg;
        msg.iType = KMessageDownloadStatusUpdate;
        CNativeThread::Send(msg);
    }
    return NULL;
}


void * CDownloadList::UpdateDownloadProgress(void *_pv)
{
    if(NULL == _pv)
    {
        return NULL;
    }
    CDownloadList *pw = (CDownloadList *)_pv;
    DebugPrintf(DLC_XU_KAI,"CDownloadList::UpdateDownloadProgress m_iVisibleItemNum is %d", pw->m_iVisibleItemNum);
    for(int i = 0;i < pw->m_iVisibleItemNum;i++)
    {
        DebugPrintf(DLC_XU_KAI,"CDownloadList::UpdateDownloadProgress(void *_pv) : %d",i);
        CDownloadItem *pItem = (CDownloadItem *)(pw->GetChildByIndex(i));
        if(pItem)
        {
            DebugPrintf(DLC_XU_KAI,"CDownloadList::UpdateDownloadProgress pItem uPDATEpROGRESS");
            pItem->UpdateProgress();
            DebugPrintf(DLC_XU_KAI,"CDownloadList::UpdateDownloadProgress pItem uPDATEpROGRESS END");
        }
    }
    return NULL;
}


CDownloadList::~CDownloadList()
{
}

void CDownloadList::SetFocus(BOOL bIsFocus)
{
    if (m_bIsFocus != bIsFocus)
    {
        m_bIsFocus = bIsFocus;
        if (m_iSelectedItem < (int)GetChildrenCount())
        {
            CDownloadItem *pItem = (CDownloadItem *)GetChildByIndex(m_iSelectedItem);
            if(pItem)
            {
                pItem->SetFocus(bIsFocus);
            }
        }
    }
}

void CDownloadList::SetDownloadList(IDownloadItemModel **ppList, INT32 iNum)
{
    if(NULL == ppList && iNum > 0 )
    {
        return;
    }
    m_Itemlist.clear();
    for(int i=0;i<iNum;i++)
    {
        m_Itemlist.push_back(ppList[i]);
    }
    m_iVisibleItemNum = iNum;

    if(m_iVisibleItemNum)
    {
        if(m_iSelectedItem >= iNum)
        {
            m_iSelectedItem = iNum-1;
        }
    }
    else
    {
        return;
    }
    CDownloadItem *pItem = NULL;
    size_t i = 0;
    DebugPrintf(DLC_GUI, "CDownloadList::SetDownloadList m_Itemlist.size() = %d", m_Itemlist.size());
    for (i = 0; i <  m_Itemlist.size(); i++)
    {
        // TODO: fix memory leak here.
        if(i == GetChildrenCount())
        {
            pItem = new CDownloadItem(this,IDSTATIC);
            if(pItem)
            {
                pItem->SetDownloadItemModel(m_Itemlist[i]);
                pItem->SetMinHeight(GetItemHeight());
                if (m_windowSizer)
                {
                    m_windowSizer->Add(pItem, UISizerFlags().Expand());
                }
                //pItem->MoveWindow(0, i * ItemHeight, 600, ItemHeight);
                pItem->SetFocus(m_bIsFocus && m_iSelectedItem == (int)i);
            }
        }
        else
        {
            pItem = (CDownloadItem *)GetChildByIndex(i);
            if(pItem)
            {
                pItem->SetDownloadItemModel(m_Itemlist[i]);
                pItem->SetVisible(TRUE);
                pItem->SetFocus(m_bIsFocus && m_iSelectedItem == (int)i);
            }
        }
        DebugPrintf(DLC_GUI, "CDownloadList::SetDownloadList m_bIsFocus && m_iSelectedItem == i == %d ,i = %d,m_bIsFocus is %d", m_bIsFocus && m_iSelectedItem == (int)i,i,m_bIsFocus);
    }

    for(;i<GetChildrenCount();i++)
    {
        pItem = (CDownloadItem *)GetChildByIndex(i);
        if(pItem)
        {
            pItem->SetVisible(FALSE);
            pItem->SetDownloadItemModel(NULL);
        }
    }
    Layout();
    DebugPrintf(DLC_XU_KAI,"CDownloadList::SetDownloadList END SetDownloadList");
}

BOOL CDownloadList::OnKeyPress(INT32 iKeyCode)
{
    return UICompoundListBox::OnKeyPress(iKeyCode);
}

void CDownloadList::InitListItem()
{

}

void CDownloadList::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    CDownloadItem* pItem = NULL;
    for (size_t i=0; i<GetChildrenCount(); ++i)
    {
        pItem = (CDownloadItem*)GetChildByIndex(i);
        if (pItem && pItem == pSender)
        {
            SetSelectedItem((int)i);
            break;
        }
    }

    if (m_pParent)
    {
        m_pParent->OnCommand(dwCmdId, pSender, dwParam);
    }
}

bool CDownloadList::DoHandleListTurnPage(bool fKeyUp)
{
#ifdef KINDLE_FOR_TOUCH
    UIDownloadPage* downloadPage = dynamic_cast<UIDownloadPage*>(m_pParent);
    if (downloadPage)
        downloadPage->HandlePageUpDown(!fKeyUp);
    return true;
#endif
}
