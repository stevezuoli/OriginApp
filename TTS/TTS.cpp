#include "TTS/TTS.h"
#include "jtTTS.h"
#include <stdlib.h>
#include "interface.h"
#include "Utility/ThreadHelper.h"
#include "Framework/INativeMessageQueue.h"
#include "PowerManager/PowerManager.h"
#include "Common/SystemSetting_DK.h"
#include "Utility/SystemUtil.h"

using dk::utility::SystemUtil;


#define TTS_VOLUME_MIN  (1)
#define TTS_VOLUME_MAX  (14)

SINGLETON_CPP(CTextToSpeech)

void* CTextToSpeech::TTS_Run(void* pParam)
{  
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL); 

#ifndef _X86_LINUX
    JT_TTS* pTTS = JT_TTS::GetInstance();
    if (NULL == pTTS)
        return NULL;
#endif
    
    CTextToSpeech* pTTSInstance = (CTextToSpeech*)pParam;    
    while (1)
    {
        pthread_testcancel();
        IBookReader* pBookReader = pTTSInstance->m_pBookReader;
        if (pBookReader && pTTSInstance->TTS_IsPlay())
        { 
            string strPageText = pBookReader->GetCurrentPageText();
            int    iBufferLen  = strPageText.length(); 
            DebugPrintf(DLC_JUGH, "TTS_Run len=%d", iBufferLen);
                
            pthread_testcancel();
            if (iBufferLen <= 0)
            {
#ifndef _X86_LINUX
                // 当上层发送STOP后，jtTTS还有部分已经合成的语音流未播放，此处循环会让该未播放部分播放完
                int iWaitTimes = 10;
                while(--iWaitTimes > 0)
                {
                    if (!pTTS->TTS_IsPlay())
                        break;
                    sleep(1);
                    DebugPrintf(DLC_JUGH, "Playing audio!!!");
                }
#endif
                DebugPrintf(DLC_JUGH, "KMessageTTSChange");
                    SNativeMessage msg;
                    msg.iType   = KMessageTTSChange;
                    msg.iParam2 = TTS_MESSAGE_NEXTPAGE_AUTO; 
                    INativeMessageQueue::GetMessageQueue()->Send(msg);
            }            
            else 
            {
#ifdef _X86_LINUX
                sleep(3);
                DebugPrintf(DLC_JUGH, "KMessageTTSChange");
                SNativeMessage msg;
                msg.iType   = KMessageTTSChange;
                msg.iParam2 = TTS_MESSAGE_NEXTPAGE_AUTO; 
                INativeMessageQueue::GetMessageQueue()->Send(msg);
#else
                if (0 != pTTS->TTS_Play(strPageText, iBufferLen))
                {
                    DebugPrintf(DLC_JUGH, "TTS_Run break out and thread exit ");
                    break;
                }
                else if (pTTSInstance->TTS_IsPlay())
                {
                    // 当上层发送STOP后，jtTTS还有部分已经合成的语音流未播放，此处循环会让该未播放部分播放完
                    int iWaitTimes = 10;
                    while(--iWaitTimes > 0)
                    {
                        if (!pTTS->TTS_IsPlay())
                            break;
                                sleep(1);
                                DebugPrintf(DLC_JUGH, "Playing audio!!!");
                    }
                    DebugPrintf(DLC_JUGH, "KMessageTTSChange");
                    SNativeMessage msg;
                    msg.iType   = KMessageTTSChange;
                    msg.iParam2 = TTS_MESSAGE_NEXTPAGE_AUTO; 
                    INativeMessageQueue::GetMessageQueue()->Send(msg);
                }
#endif
            }

        }
        DebugPrintf(DLC_JUGH, "TTS TTS_Run ing...");
        sleep(1);
    }
    pTTSInstance->TTS_Stop();
    pTTSInstance->m_pTTSThread  = 0;
    
    ThreadHelper::ThreadExit(0); 
    return NULL;
}

CTextToSpeech::CTextToSpeech()
    : m_pBookReader(NULL)
    , m_pTTSThread(0)
    , m_bIsPlay(false)
{
    m_iVolumeValue = SystemSettingInfo::GetInstance()->GetVolume();
}

CTextToSpeech::~CTextToSpeech()
{
}

bool CTextToSpeech::TTS_Init()
{
    DebugPrintf(DLC_JUGH,"CTextToSpeech::TTS_Init start!!!");
    if (TTS_IsPlay())
        return true;

#ifndef _X86_LINUX
    JT_TTS* pTTS = JT_TTS::GetInstance();
    if (NULL == pTTS)
        return false;
#endif
    
    PowerManagement::DKPowerManagerImpl::GetInstance()->AccquireLock(PM_LOCK_SCREEN, MODULE_TTSSERVICE);
    TTS_SetVolume(m_iVolumeValue);

#ifdef _X86_LINUX
    int iRet = 0;
#else
    int iRet = pTTS->TTS_Init();
#endif
    if (0 == iRet)
    {
        if (0 != m_pTTSThread)
        {
            ThreadHelper::CancelThread(m_pTTSThread);
            ThreadHelper::JoinThread(m_pTTSThread, NULL);
        } 
        
        string strThreadName("TTS");    
        ThreadHelper::CreateThread(&m_pTTSThread, TTS_Run, this, strThreadName, false, 1024 * 100, SERIALIZEDBOOK_THREAD_PRIORITY);
        return true;
    }
    DebugPrintf(DLC_JUGH,"CTextToSpeech::TTS_Init end!!!");
    return false;
}

void CTextToSpeech::TTS_UnInit()
{
    DebugPrintf(DLC_JUGH, "CTextToSpeech::TTS_UnInit Entering");
    
    if (0 != m_pTTSThread)
    {
        ThreadHelper::CancelThread(m_pTTSThread);
        ThreadHelper::JoinThread(m_pTTSThread, NULL);
        m_pTTSThread  = 0;  
    }  

#ifndef _X86_LINUX
    JT_TTS* pTTS = JT_TTS::GetInstance();    
    if (NULL != pTTS)
    {
        pTTS->TTS_UnInit();        
    }
#endif
    
    m_pBookReader = NULL;

    PowerManagement::DKPowerManagerImpl::GetInstance()->ReleaseLock(PM_LOCK_SCREEN, MODULE_TTSSERVICE);
    
    DebugPrintf(DLC_JUGH,"CTextToSpeech::TTS_UnInit end!!!");
}

void CTextToSpeech::TTS_SetVolume(int iVolumeValue)
{
    m_iVolumeValue = (iVolumeValue < TTS_VOLUME_MIN) ? TTS_VOLUME_MIN : 
                     (iVolumeValue > TTS_VOLUME_MAX ? TTS_VOLUME_MAX : iVolumeValue);

#ifndef _X86_LINUX
    char cmd[256] = {0};
    sprintf(cmd,"lipc-set-prop com.lab126.audio Volume %d", m_iVolumeValue);
    SystemUtil::ExecuteCommand(cmd);
#endif
}

int CTextToSpeech::TTS_GetVolume()
{
    return m_iVolumeValue;
}

void CTextToSpeech::TTS_Stop()
{
    DebugPrintf(DLC_JUGH, "CTextToSpeech::TTS_Stop Entering"); 
    if (m_bIsPlay)
    {
        m_bIsPlay = false;

#ifndef _X86_LINUX
        JT_TTS* pTTS = JT_TTS::GetInstance();
        pTTS->TTS_Stop();
#endif
    }
    DebugPrintf(DLC_JUGH, "CTextToSpeech::TTS_Stop End"); 
}

int CTextToSpeech::TTS_Play(IBookReader* _pBookReader)
{
    DebugPrintf(DLC_JUGH,"CTextToSpeech::TTS_Play start!!!");
    if (!m_bIsPlay)
    {
        m_bIsPlay     = true;
        m_pBookReader = _pBookReader;
    }
    DebugPrintf(DLC_JUGH,"CTextToSpeech::TTS_Play end!!!");
    return 0;
}

bool CTextToSpeech::TTS_IsPlay()
{
    return m_bIsPlay;
}

