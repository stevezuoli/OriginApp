#ifndef __JT_TEXTTOSPEECH_H__
#define __JT_TEXTTOSPEECH_H__

#include <stdio.h>
#include "singleton.h"
#include <pthread.h>
#include <vector>
#include <string>

using std::string;

// 回调用户数据定义
typedef struct tagUserData
{
    FILE *    pInputFile;        // 文本输入文件
    FILE *    pOutputFile;    // 合成语音数据输出文件
    unsigned long    hTTS;    // 合成引擎句柄
}jtUserData;

class JT_TTS
{
    SINGLETON_H(JT_TTS)

public:    
    JT_TTS();
    ~JT_TTS();
    
    int        TTS_Init();
    void       TTS_UnInit();
    
    int        TTS_Play(std::string& _strTxtBuffer, int _iBufferLen);
    int        TTS_Stop();
    
    bool       TTS_IsPlay();

private:
    static JT_TTS*                m_hInstance;
    static unsigned long        m_hTTS;
    static unsigned char*        m_pHeap;

    bool    m_bIsPlay;
};

#endif // __JT_TEXTTOSPEECH_H__
