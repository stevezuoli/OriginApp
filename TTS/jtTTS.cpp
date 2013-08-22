#include "eJTTS.h"
#include "jtTTS.h"
#include "cpcm.h"
#include "dkBaseType.h"
#include <stdlib.h>
#include <string.h>
#include "interface.h"
#include "Common/SystemSetting_DK.h"
#include <string>

#define    TTS_RESOURCE_PATH    "/mnt/us/DK_System/xKindle/res/tts/"

SINGLETON_CPP(JT_TTS)

unsigned long    JT_TTS::m_hTTS      = 0;
unsigned char*    JT_TTS::m_pHeap     = NULL;

jtErrCode TTS_SynthesizeText(unsigned long nhTTS, jtUserData * pUserData);
jtErrCode TTS_OutputVoiceProc(void* pParameter,long iOutputFormat, void* pData, long iSize);

JT_TTS::JT_TTS()
{
    m_bIsPlay =false;
}

JT_TTS::~JT_TTS()
{
}

int JT_TTS::TTS_Init()
{
    int         iFileNameLen = 128;
    char        csCNFileName[iFileNameLen];
    char        csENFileName[iFileNameLen];
    memset(csCNFileName, 0, iFileNameLen);
    memset(csENFileName, 0, iFileNameLen);
    strcpy(csCNFileName, TTS_RESOURCE_PATH);
    strcpy(csENFileName, TTS_RESOURCE_PATH);
    
    jtErrCode    dwError        = jtTTS_ERR_NONE;
    
    unsigned char     byMajor        = 0;            // 主版本号 
    unsigned char    byMinor        = 0;            // 次版本号 
    unsigned long    iRevision    = 0;            // 修订版本号
    dwError = jtTTS_GetVersion(&byMajor, &byMinor, &iRevision);  // 调用接口 
    if (jtTTS_ERR_NONE != dwError)
    {
        DebugPrintf(DLC_JUGH, "GetVersion error!!!!");
        return jtTTS_ERR_LICENCE;
    }
    
    int iLanguage = SystemSettingInfo::GetInstance()->GetTTSLanguage();
    int iSpeaker  = SystemSettingInfo::GetInstance()->GetTTSSpeaker();
    
    if (2 == iLanguage)
    {
        strcat(csCNFileName, "YCNPackage.dat");
        strcat(csENFileName, "YENPackage.dat");
    }
    else if (0 == iSpeaker)
    {
        strcat(csCNFileName, "BCNPackage.dat");
        strcat(csENFileName, "BENPackage.dat");
    }
    else
    {
        strcat(csCNFileName, "GCNPackage.dat");
        strcat(csENFileName, "GENPackage.dat");
    }

    // 获得HEAP大小 
    unsigned long  lSize = 0;                // 堆空间大小
    dwError = jtTTS_GetExtBufSize((const signed char*)csCNFileName, (const signed char*)csENFileName, NULL,(long *)&lSize);
    if(jtTTS_ERR_NONE != dwError)
    {
        DebugPrintf(DLC_JUGH, "GetExtBuffSize error ");
        return jtTTS_ERR_OPEN_DATA;
    }

    // 分配堆 
    m_pHeap = (unsigned char *)malloc(lSize);
    if(!m_pHeap)
    {
        DebugPrintf(DLC_JUGH,"m_pHeap malloc error ");
        return jtTTS_ERR_OPEN_DATA;
    }
    memset(m_pHeap, 0, lSize);

    // 初始化引擎 
    //myDebugPrintf(DLC_JUGH,("strCNFileName is %s,strlen(strCNFileName) is%d,strENFileName is %s,strlen(strENFileName) is %d",strCNFileName,strlen(strCNFileName),strENFileName,strlen(strENFileName));
    dwError = jtTTS_Init((const signed char*)csCNFileName, (const signed char*)csENFileName, NULL, &m_hTTS, m_pHeap);
    if (dwError != jtTTS_ERR_NONE)
    {
        free(m_pHeap);
        m_pHeap = NULL;
        if(dwError==jtTTS_ERR_OPEN_DATA)
        {
            DebugPrintf(DLC_JUGH,"dwError is %d",dwError);
        }
        DebugPrintf(DLC_JUGH,"Init error ");
        return jtTTS_ERR_NOT_INIT;
    }
    
    // 段式文本直接输入合成
    jtUserData UserData;            // 用户数据
    UserData.pInputFile = 0;
    UserData.pOutputFile = 0;
    UserData.hTTS = m_hTTS;
    dwError = TTS_SynthesizeText(m_hTTS, &UserData);
    return jtTTS_ERR_NONE;
}

void JT_TTS::TTS_UnInit()
{    
    DebugPrintf(DLC_JUGH, "JT_TTS::TTS_UnInit Entering m_hTTS=%d", m_hTTS);
    if (0 != m_hTTS)
    {
        jtTTS_End(m_hTTS);
        m_hTTS = 0;
    }

    DebugPrintf(DLC_JUGH, "JT_TTS::TTS_UnInit m_pHeap!=NULL: %d", (m_pHeap!=NULL));
    
    if (NULL != m_pHeap)
    {
        free(m_pHeap);
        m_pHeap = NULL;
    }
}

int JT_TTS::TTS_Play(string& _strTxtBuffer, int _iBufferLen)
{
    if (_strTxtBuffer.length() <= 0 || _iBufferLen <= 0 || m_hTTS <= 0)
        return jtTTS_ERR_INPUT_PARAM;
    
    m_bIsPlay = true;
    jtErrCode dwError = jtTTS_ERR_NONE;
    const char* pBuffer = _strTxtBuffer.c_str();
    while (_iBufferLen > 0 && m_bIsPlay)
    { 
        DebugPrintf(DLC_JUGH, "JT_TTS::TTS_Play m_bIsPlay=%d", m_bIsPlay);
        if (_iBufferLen < jtTTS_INPUT_TEXT_SIZE && m_bIsPlay)
        {
            DebugPrintf(DLC_JUGH, "JT_TTS::TTS_Play1 Len=%d, is reading", _iBufferLen);
            dwError = jtTTS_SynthesizeText(m_hTTS, pBuffer, _iBufferLen);
            DebugPrintf(DLC_JUGH, "JT_TTS::TTS_Play1_1, reading end and break out dwError=%d", dwError);
            break;
        }
        
        int iPlayLen = jtTTS_INPUT_TEXT_SIZE - 1;
        while (pBuffer[iPlayLen] & 0x80)
        {
            iPlayLen--;
            if (pBuffer[iPlayLen] & 0x40)
                break;
        }
        
        dwError = jtTTS_SynthesizeText(m_hTTS, pBuffer, iPlayLen);
        DebugPrintf(DLC_JUGH, "JT_TTS::TTS_Play2 PlayLen=%d, BufferLen=%d", iPlayLen, _iBufferLen);    
        if (dwError != jtTTS_ERR_NONE)
        {
            DebugPrintf(DLC_JUGH,"JT_TTS::TTS_Play2 jtTTS_SynthesizeText error code is %d",dwError);
            break;
        }
            
        pBuffer += iPlayLen;
        _iBufferLen  -= iPlayLen;
    }
    
    return dwError;
}

bool JT_TTS::TTS_IsPlay()
{
    cpcm* pPCM = cpcm::GetInstance();
    return (NULL == pPCM) ? false : pPCM->Pcm_IsPlay();
}

int JT_TTS::TTS_Stop()
{
    m_bIsPlay = false;
    int iRet = jtTTS_SynthStop(m_hTTS);
    DebugPrintf(DLC_JUGH, "JT_TTS::TTS_Stop iRet=%d", iRet);
    if (0 != iRet)
    {
        return iRet;
    }        
    
    cpcm* pPCM = cpcm::GetInstance();
    if (NULL != pPCM)
    {
        DebugPrintf(DLC_JUGH, "JT_TTS::TTS_Stop before Pcm_Stop");
        pPCM->Pcm_Stop();
        DebugPrintf(DLC_JUGH, "JT_TTS::TTS_Stop after  Pcm_Stop");
    }
    DebugPrintf(DLC_JUGH, "JT_TTS::TTS_Stop End");
    return jtTTS_ERR_NONE;
}

jtErrCode TTS_OutputVoiceProc(void* pParameter,long iOutputFormat, void* pData, long iSize)
{
    jtUserData *    pUserData;
    iOutputFormat = 0;
    if(pParameter == NULL)
    {
        return jtTTS_ERR_NO_INPUT;
    }
    pUserData = (jtUserData *)pParameter;
    if(iSize == -1)
    {
        jtTTS_SynthStop(pUserData->hTTS);
        return jtTTS_ERR_NO_INPUT;
    }

    cpcm* pPCM = cpcm::GetInstance();
    if (NULL != pPCM)
    {
    	pPCM->Pcm_SetRate(8000);
        pPCM->Pcm_Play((char*)pData,iSize);
        return jtTTS_ERR_NONE;
    }

    return jtTTS_ERR_ENGINE_BUSY;
}

// 段式文本直接输入合成
jtErrCode TTS_SynthesizeText(unsigned long nhTTS, jtUserData * pUserData)
{
    jtErrCode dwError = jtTTS_ERR_NONE;

    // 设置直接文本输入
    dwError = jtTTS_SetParam(nhTTS, jtTTS_PARAM_INPUTTXT_MODE,
        jtTTS_INPUT_TEXT_DIRECT);

    // 设置音频输出回调 
    dwError = jtTTS_SetParam(nhTTS, jtTTS_PARAM_OUTPUT_CALLBACK,
        (long)TTS_OutputVoiceProc);

    // 设置回调用户数据 
    dwError = jtTTS_SetParam(nhTTS, jtTTS_PARAM_CALLBACK_USERDATA,
        (long)pUserData);

    
    int iSpeed      = (SystemSettingInfo::GetInstance()->GetTTSSpeed() - 2) * 15000;
    int iSpeakStyle = SystemSettingInfo::GetInstance()->GetTTSSpeakStyle();
    
    jtTTS_SetParam(nhTTS, jtTTS_PARAM_SPEED,        iSpeed);
    jtTTS_SetParam(nhTTS, jtTTS_PARAM_SPEAK_STYLE,  iSpeakStyle);
    jtTTS_SetParam(nhTTS, jtTTS_PARAM_PITCH,        jtTTS_PITCH_NORMAL);
    jtTTS_SetParam(nhTTS, jtTTS_PARAM_PUNC_MODE,    jtTTS_PUNC_OFF);
    // 设置输入文本代码页 
    dwError = jtTTS_SetParam(nhTTS, jtTTS_PARAM_CODEPAGE, jtTTS_CODEPAGE_UTF8);
    return dwError;
}

