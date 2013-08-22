////////////////////////////////////////////////////////////////////////
// UITitleBar.h
// Contact:
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UITITLEBAR_H__
#define __UITITLEBAR_H__

#include <pthread.h>
#include "Common/SystemSetting_DK.h"
#include "GUI/UIWindow.h"
#include "GUI/UIImage.h"
#include "GUI/UITextSingleLine.h"
#include "DownloadManager/IDownloader.h"
#include "GUI/GestureDetector.h"

class UITitleBar : public UIWindow
{
public:
    enum tagElements
    {
        LogoImage,
        AccountInfo,
        MailImage,
        MailInfo,
        DownloadImage,
        DownloadInfo,
        WifiImage,
        TimeInfo,
        BatteryImage,
        BatteryInfo,
        ElementsCounts
    };

    UITitleBar();
    ~UITitleBar();
    
    virtual LPCSTR GetClassName() const
    {
        return "UITitleBar";
    }
    
    static void WifiStatusCallBack();
    static void WifiPowerOnCallBack();
    static void ConnectWifiCallBack(BOOL _bConnected, std::string _strSSID, std::string _strPWD, string strIdentity);

    void OnWifiMessage(SNativeMessage _clsMsg);

    HRESULT Draw(DK_IMAGE drawingImg);
    void UpdateSignal();
    bool OnSignalChanged(const EventArgs& args);
    bool OnDownloadUpdate(const EventArgs& args);
    bool OnLogStatusUpdate(const EventArgs& args);
    bool OnXiaomiUserCardReceived(const EventArgs& args);

    HRESULT ClearTitleBar();
    int GetBatteryLevel() const
    {
        return m_iBatteryLevel;
    }
    void SetBatteryRatioMode();
    
protected:
    void    UpdateTime();
    void    UpdateWifiSignalLevel();
    void    UpdateBatteryLevel();
    void    UpdateDownLoadStauts();
    void    UpdateMailRecvInfo();
    void    UpdateAccountAlias();
    /**
     * @brief PrepareSourcePosition 计算元素位置,如果元素过长排不下，压缩用户名长度同时缩小两侧边距值
     */
    void    PrepareSourcePosition();
    void    PrepareSource();

private:
    virtual bool OnTouchEvent(MoveEvent* moveEvent);
    bool OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy);
    int GetCurrentDownloadNum();

private:
    class FlingGestureListener: public SimpleGestureListener
    {
    public:
        virtual bool OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy)
        {
            return m_titleBar->OnFling(moveEvent1, moveEvent2, direction, vx, vy);
        }
        void SetTitleBar(UITitleBar* titleBar)
        {
            m_titleBar = titleBar;
        }
    private:
       UITitleBar* m_titleBar;
    };
    GestureDetector m_gestureDetector;
    FlingGestureListener m_flingGestureListener;

private:
    int             m_iWifiLevel;
    int             m_iBatteryLevel;
    int             m_iWifiMaxLevel;  // Status of WiFi icon
    int             m_iBatteryMaxLevel; // Status of Battery icon
    int             m_iDownLoadMaxStatus; // Status of Download icon
    int             m_iTopOffset;//TODO,在kp上y方向需要一定的偏差.
    /*该值为-1表示无限制，否则为昵称的限制长度*/
    int             m_iAliasUsableWidth;
    SPtr<ITpImage>  m_imgBatteryIcon ;
    SPtr<ITpImage>  m_imgWifiIcon;
    SPtr<ITpImage>  m_imgDownLoadIcon;
    SPtr<ITpImage>  m_imgMails;
    SPtr<ITpImage>  m_imgLogo;
    string          m_strTime;
    string          m_strBatteryRatio;
    string          m_strDownLoadingProgress;
    string          m_strMailInfo;
    string          m_strAlias;
    int             m_iElementSpace;
    int             m_elementsXCoordinate[ElementsCounts];
    
    // download info
    int             m_iDownLoadStatus;
    string          m_currentDownloadTaskID;

};

BOOL ReadCmd(LPCSTR Cmd,LPSTR lpResult,int iSize);
int DKPowerIsCharge();
int DKGetBatteryLevel();


class TitleBarSignalManager
{
public:
    void Init();
    void Start();
    void Stop();
    void Destory();

    static void CreateTitleBarSignalManager();
    static TitleBarSignalManager * GetTitleBarSignalManager();

    static void *UpdateTitleBarSingalProc(void *lparam);
    TitleBarSignalManager();
    ~TitleBarSignalManager();
private:
    pthread_t m_PthreadId;
};
#endif //__UITITLEBAR_H__
