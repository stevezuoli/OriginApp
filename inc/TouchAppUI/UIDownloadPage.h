////////////////////////////////////////////////////////////////////////
// UIDownloadPage.h
// Contact: xukai
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////
#ifndef __UIDOWNLOADPAGE_H__
#define __UIDOWNLOADPAGE_H__

#include"TouchAppUI/DownloadItem.h"
#include "GUI/UIPage.h"
#include "GUI/UIImage.h"
#include "GUI/UIButton.h"
#include "GUI/UIMenu.h"
#include "GUI/UIComplexButton.h"
#include "GUI/UITextSingleLine.h"
#include "CommonUI/CPageNavigator.h"
#include "Model/DownloadItemModelImpl.h"
#include <string>


class UIDownloadPage : public UIPage
{
    ////////////////////Constructor Section/////////////////////////
public:
    UIDownloadPage();

    virtual LPCSTR GetClassName() const
    {
        return this->GetClassNameStatic();
    }

    static LPCSTR GetClassNameStatic()
    {
        return ("UIDownloadPage");
    }


    virtual ~UIDownloadPage();
    virtual UIPage* GetUIPage();
    virtual void OnNotify(UIEvent rEvent);
    virtual void Show(BOOL bIsShow, BOOL bIsRepaint = TRUE);
    virtual HRESULT Draw(DK_IMAGE drawingImg);
    void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    void StartUpdateThread();
    void StopUpdateThread();
    void UpdateSectionUI(bool bIsFull = false);
    void HandlePageUpDown(BOOL fPageDown);

    static void WifiStatusCallBack();
    static void WifiPowerOnCallBack();
    static void ConnectWifiCallBack(BOOL connected, std::string ssid, std::string password, string strIdentity);
    void UpdateTitleBar();
    void OnWifiMessage(SNativeMessage event);
    void OnEnter();
    void OnLeave();
    void OnLoad();
    void OnUnLoad();

    virtual void MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight);
protected:
    void Init();
    void Dispose();

private:
    void OnDispose(BOOL bIsDisposed);
    void InitDownloadList(BOOL bIsUpdatePageNum = false);
    void UpdateNavigationButton();
    void UpdateDownloadListbox();
    void Finalize();
    void UpdateDeletedOrAddedItem();

    ////////////////////Method Section/////////////////////////

    ////////////////////Field Section/////////////////////////

private:
    UIImage m_imgLogo;
    UIComplexButton m_btnSearch;
    CDownloadList m_lstDownload;
    UITextSingleLine m_txtTotalDownload;
    UITextSingleLine m_pageNum;

    INT32 m_iTotalPage;
    INT32 m_iCurPage;
    INT32 m_iDownloadNum;
    INT32 m_iDownloadNumPerPage;
    BOOL m_bIsDisposed;


    pthread_t m_pupdateth;
    CDownloadItemModelImpl **m_ppCurDownloadList;
    std::vector<std::string> m_ShowTaskList;
    ////////////////////Field Section/////////////////////////
};

#endif//__UIDOWNLOADPAGE_H__

