#ifndef __IPowerManager_h__
#define __IKPowerManager_h__

#define PM_LOCK_LIST(MACRO_NAME)    \
    MACRO_NAME(SCREEN)              \
    MACRO_NAME(CPU)                 \
    MACRO_NAME(WIFI)                \
    MACRO_NAME(USB)                 \


#define DEFINE_PM_LIST_ENUM(a)      \
        PM_LOCK_##a,                \

enum PM_LOCK
{
        PM_LOCK_LIST(DEFINE_PM_LIST_ENUM)
        PM_LOCK_COUNT
};

#define PM_MODULE_LIST(MACRO_NAME)  \
    MACRO_NAME(MAIN)                \
    MACRO_NAME(91SEARCH)            \
    MACRO_NAME(DOWNLOAD)            \
    MACRO_NAME(DKREADER)            \
    MACRO_NAME(AUDIOPLAYER)         \
    MACRO_NAME(FTPSERVICE)          \
    MACRO_NAME(TTSSERVICE)          \
    MACRO_NAME(PLAYERSERVICE)       \
    MACRO_NAME(WEBBROWSER)          \

#define DEFINE_MODULE_LIST_ENUM(a)      \
        MODULE_##a,             \

enum PMSOURCE
{
        PM_MODULE_LIST(DEFINE_MODULE_LIST_ENUM)
        PM_MODULE_COUNT
};

class IPowerManager
{
public:
    virtual ~IPowerManager(){};
    
    virtual void AccquireLock(PM_LOCK lock, PMSOURCE src)=0;
    virtual void ReleaseLock(PM_LOCK lock, PMSOURCE src)=0;
    virtual int  GetPowerLevel()=0;     
    virtual bool IsCharging()=0;
};

#endif //__DKPowerManager_h__

