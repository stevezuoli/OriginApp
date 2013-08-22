#ifndef __AUDIOPLAYER_h__
#define __AUDIOPLAYER_h__

#include "singleton.h"

enum AUDIOPLAYER_STATUS
{
    PLAYER_STATUS_STOP,
    PLAYER_STATUS_PLAY,
    PLAYER_STATUS_PAUSE
};
    
enum AUDIOPLAYER_LOOPSTATUS
{
    PLAYER_LOOP_ALL         = 0,
    PLAYER_LOOP_ONE         = 1,
    PLAYER_LOOP_RANDOM      = 2,
    PLAYER_LOOPSTATUS_END   = 3
};

#define AUDIOPLAYER_VOLUME_MAX  (14)
#define AUDIOPLAYER_VOLUME_MIN  (5)  // AUDIOPLAYER_VOLUME_MIN = AUDIOPLAYER_VOLUME_MAX - 10 + 1

class AudioPlayer
{
    SINGLETON_H(AudioPlayer)

public:
    AudioPlayer();
    ~AudioPlayer();

    void    Play(bool shuffle);
    void    Pause();
    void    UnPause();
    void    Stop(); 
    void    Next(); 
    void    Preious();     
    void    SetVolume(int iVolumeValue);
    int        GetVolumeValue();
    void    Shuffle(); 
    void    CreatPlayList(bool shuffle);

    AUDIOPLAYER_STATUS GetPlayStatus();
    
private:
    AUDIOPLAYER_STATUS m_ePlayStatus;
    int                   m_iVolumeValue;

};

#endif //__AUDIOPLAYER_h__
