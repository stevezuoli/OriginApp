#include "cpcm.h"
#include <stdio.h>
#include <sys/ioctl.h>
#include <pthread.h>

#define TTS_BITSPERSAMPLE   16
#define TTS_RATE            16000
#define TTS_CHANNELS        2

SINGLETON_CPP(cpcm)

snd_pcm_t* cpcm::m_iWaveDrvhandle = NULL;

cpcm::cpcm()
    : m_ePcmFormat(SND_PCM_FORMAT_S16_LE)
    , m_iChannel(TTS_CHANNELS)
    , m_iRate(TTS_RATE)
    , m_iBitsPerSample(TTS_BITSPERSAMPLE)
{
}

cpcm::~cpcm()
{
}

void cpcm::Pcm_SetChannel(unsigned int iChannel)
{
    m_iChannel = iChannel;
}

void cpcm::Pcm_SetRate(unsigned int iRate)
{
    m_iRate = iRate;
}

void cpcm::Pcm_SetBitsPerSample(unsigned int iBitsPerSample)
{
    m_iBitsPerSample = iBitsPerSample;
}

bool cpcm::Pcm_IsPlay()
{
    if (NULL == m_iWaveDrvhandle)
    return false;

    return SND_PCM_STATE_RUNNING == snd_pcm_state(m_iWaveDrvhandle);;
}

void cpcm::Pcm_Stop()
{
    if(m_iWaveDrvhandle)
    {
        snd_pcm_drop(m_iWaveDrvhandle);
        snd_pcm_close(m_iWaveDrvhandle);
        m_iWaveDrvhandle = NULL;
    }
}

int cpcm::Pcm_Play(char* pBuffer, int iBufferLen)
{
    char*   pDevice     = "default";
    int     iErrorcount = 0;
     
    while (NULL == m_iWaveDrvhandle && iErrorcount < 5)
    {        
        if (snd_pcm_open((snd_pcm_t **)&m_iWaveDrvhandle, pDevice, SND_PCM_STREAM_PLAYBACK, 0) < 0)
        {
            iErrorcount++;
            usleep(500*1000);
        }
        else if ((snd_pcm_set_params(m_iWaveDrvhandle,
                                     m_ePcmFormat, //SND_PCM_FORMAT_U8, 16 位
                                     SND_PCM_ACCESS_RW_INTERLEAVED, 
                                     m_iChannel, // 双声道
                                     m_iRate, //采样率
                                     1, 
                                     500000)) < 0)
        {
            return -1;
        }     
    }

    if (NULL == m_iWaveDrvhandle)
    {
        return -1;
    }
    
    iErrorcount = 0;
    while (iErrorcount < 5)
    {
        int iSndPcmWrite = snd_pcm_writei(m_iWaveDrvhandle, pBuffer, iBufferLen / (m_iChannel * m_iBitsPerSample / 8));
        if (iSndPcmWrite >= 0)
            return 0;
        else if (snd_pcm_prepare(m_iWaveDrvhandle) <= 0)
            return -1;
        iErrorcount++;
    }

    return -1;
}

