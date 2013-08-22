#ifndef __PCM_H__
#define __PCM_H__

#include "singleton.h"
#include "TTS/alsa/alsa/asoundlib.h"

class cpcm
{ 
    SINGLETON_H(cpcm)

public:    
    cpcm();
    ~cpcm();
    
    void    Pcm_SetChannel(unsigned int iChannel);
    void    Pcm_SetRate(unsigned int iRate);
    void    Pcm_SetBitsPerSample(unsigned int iBitsPerSample);
    
    int        Pcm_Play(char* pBuffer, int iBufferLen);
    void    Pcm_Stop();
    bool    Pcm_IsPlay();
    
private:
    static snd_pcm_t*    m_iWaveDrvhandle;

    _snd_pcm_format        m_ePcmFormat;  
    unsigned int        m_iChannel;  
    unsigned int        m_iRate;
    unsigned int        m_iBitsPerSample;
};

#endif // __PCM_H__

