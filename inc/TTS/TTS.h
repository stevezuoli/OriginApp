#ifndef __CTEXTTOSPEECH_H__
#define __CTEXTTOSPEECH_H__

#include "Utility/ThreadHelper.h"
#include "BookReader/IBookReader.h"
#include "singleton.h"

enum TTS_MESSAGE_TYPE
{
    TTS_MESSAGE_PLAY,
    TTS_MESSAGE_STOP,
    TTS_MESSAGE_NEXTPAGE_MANUAL,
    TTS_MESSAGE_NEXTPAGE_AUTO,
};


class CTextToSpeech
{
    SINGLETON_H(CTextToSpeech)

public:    
    CTextToSpeech();
    ~CTextToSpeech();

    bool     TTS_Init();
    void    TTS_UnInit();
    int     TTS_Play(IBookReader* _pBookReader);
    void    TTS_Stop();
    bool    TTS_IsPlay();
    
    void    TTS_SetVolume(int iVolumeValue);
    int     TTS_GetVolume();
    
private:
    static void* TTS_Run(void* pParam);
    IBookReader*    m_pBookReader;
    pthread_t       m_pTTSThread;
    bool            m_bIsPlay;
    int             m_iVolumeValue;
};

#endif // __CTEXTTOSPEECH_H__
