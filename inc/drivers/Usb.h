#ifndef     __USB_H__
#define     __USB_H__

#define USB_CONFIGURED          41
#define USB_UNCONFIGURED        42
#define USB_PLUGOUT             43
#define USB_PLUGIN              44

enum EUsbSingleType
{
    usbUnknow = 0,
    usbConfigured,
    usbUnconfigured,
    usbPlugOut,
    usbPlugIn,
};


class CUsb
{
public:
    static CUsb *GetInstance();
    void Release();
    void StartWaitUsbSingle();
    int IsUsbConnected();
#ifdef _X86_LINUX
    void SetUsbConnected(bool connected);
#endif
    int SwitchToUsb();
    int SwitchToFlash();
    void ShowConfigurePic();
    void ShowUnConfigurePic();
    void StartWait();
    
private:
    CUsb();
    virtual ~CUsb();

    void ShowUsbPic(const char *lpFilePath);
    int SwitchToOtg(const char *lpFlash, int nBufferLength);
    char* GetUsbStatPath();
    
    

private:

    static CUsb *m_spInstance;
    char m_szUsbStatPath[512];
#ifdef _X86_LINUX
    bool m_connected;
#endif
};

#endif
