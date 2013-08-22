#include "AudioPlayer/AudioPlayer.h"
#include "PowerManager/PowerManager.h"
#include "Common/SystemSetting_DK.h"
//#include "Framework/CNativeThread.h"
#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include "Utility/FileSystem.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "Utility/Misc.h"
#include "dkWinError.h"
#include "../Inc/interface.h"
#include "Utility/SystemUtil.h"

using dk::utility::SystemUtil;

#define AUDIO_FIFO     "/tmp/mplayer_fifo"
#define MPLAYER        "/DuoKan/mplayer"
#define PLAYLIST       "/tmp/playlist"

#define PLAY_CMD_PAUSE     "pause"
#define PLAY_CMD_QUIT     "quit"
#define PLAY_CMD_PREVIOUS    "pt_step -1"
#define PLAY_CMD_NEXT        "pt_step 1"

using namespace std;

SINGLETON_CPP(AudioPlayer)

AudioPlayer::AudioPlayer()
{
    m_ePlayStatus  = PLAYER_STATUS_STOP;
    m_iVolumeValue = SystemSettingInfo::GetInstance()->GetVolume();
    string cmd = "killall -9 mplayer";
    SystemUtil::ExecuteCommand(cmd.c_str());

    //clean the FIFO
    unlink(AUDIO_FIFO);
    mkfifo(AUDIO_FIFO, 0777 );
}

AudioPlayer::~AudioPlayer()
{
    /* destructor code */
    unlink(AUDIO_FIFO);
}

void  AudioPlayer::Play(bool shuffle)
{
    //TODO, move the /tmp/mplayer_fifo to #define
    CreatPlayList(shuffle);
    if (m_ePlayStatus == PLAYER_STATUS_STOP )
    {
        PowerManagement::DKPowerManagerImpl::GetInstance()->AccquireLock(PM_LOCK_SCREEN, MODULE_PLAYERSERVICE);
        string cmd= "";
        cmd = cmd + "nice -n -10  " + MPLAYER +"  -ao alsa -slave -quiet -loop 0 -playlist  " + PLAYLIST + " -input file=" + AUDIO_FIFO + "  &";
        SystemUtil::ExecuteCommand(cmd.c_str());
    }
    m_ePlayStatus=PLAYER_STATUS_PLAY;
}

void  AudioPlayer::Pause()
{
    //write "pause" to the FIFO

    string cmd = "";
    cmd = cmd + "echo pause > "+AUDIO_FIFO;
    SystemUtil::ExecuteCommand(cmd.c_str());
    m_ePlayStatus=PLAYER_STATUS_PAUSE;
    PowerManagement::DKPowerManagerImpl::GetInstance()->ReleaseLock(PM_LOCK_SCREEN, MODULE_PLAYERSERVICE);
}

void  AudioPlayer::UnPause()
{
    //write "pause" to the FIFO
    PowerManagement::DKPowerManagerImpl::GetInstance()->AccquireLock(PM_LOCK_SCREEN, MODULE_PLAYERSERVICE);
    DebugPrintf(DLC_JUGH, "AudioPlayer::UnPause Entering");
    string cmd = "";
    cmd = cmd + "echo pause > "+AUDIO_FIFO;
    SystemUtil::ExecuteCommand(cmd.c_str());
    m_ePlayStatus=PLAYER_STATUS_PLAY;
    DebugPrintf(DLC_JUGH, "AudioPlayer::UnPause finish");
}

void  AudioPlayer::Stop()
{
    //kill mplayer
    //remove FIFO file
    //write "quit" to the FIFO
    DebugPrintf(DLC_JUGH, "AudioPlayer::Stop Entering");
    if (m_ePlayStatus  != PLAYER_STATUS_STOP )
    {
        string cmd = "";
        cmd = cmd + "echo quit > "+AUDIO_FIFO;
        SystemUtil::ExecuteCommand(cmd.c_str());
    }
    m_ePlayStatus = PLAYER_STATUS_STOP;
    string kill_cmd = "killall -9 mplayer";
    SystemUtil::ExecuteCommand(kill_cmd.c_str());
    PowerManagement::DKPowerManagerImpl::GetInstance()->ReleaseLock(PM_LOCK_SCREEN, MODULE_PLAYERSERVICE);
    DebugPrintf(DLC_JUGH, "AudioPlayer::Stop finish");
}

void  AudioPlayer::Next()
{
    //write "pt_step 1" to the FIFO
    if (m_ePlayStatus  == PLAYER_STATUS_PLAY )
    {
        string cmd = "";
        cmd = cmd + "echo pt_step 1 > "+AUDIO_FIFO;
        SystemUtil::ExecuteCommand(cmd.c_str());
    }
}

void  AudioPlayer::Preious()
{
    //write "pt_step -1" to the FIFO
    if (m_ePlayStatus  == PLAYER_STATUS_PLAY )
    {
        string cmd = "";
        cmd = cmd + "echo pt_step -1 > "+AUDIO_FIFO;
        SystemUtil::ExecuteCommand(cmd.c_str());
    }
}

void  AudioPlayer::SetVolume(int iVolumeValue)
{
    iVolumeValue = (iVolumeValue < AUDIOPLAYER_VOLUME_MIN) ? 0 : 
                   ((iVolumeValue > AUDIOPLAYER_VOLUME_MAX) ? AUDIOPLAYER_VOLUME_MAX : iVolumeValue);

    char cmd[256] = {0};
    sprintf(cmd,"lipc-set-prop com.lab126.audio Volume %d", iVolumeValue);
    SystemUtil::ExecuteCommand(cmd);
    
    m_iVolumeValue = (iVolumeValue < AUDIOPLAYER_VOLUME_MIN) ? AUDIOPLAYER_VOLUME_MIN - 1 : iVolumeValue;
}

int AudioPlayer::GetVolumeValue()
{
    return m_iVolumeValue;
}

void  AudioPlayer::Shuffle()
{
    Stop();

    //rebuild files
    Play(true);
}

void  AudioPlayer::CreatPlayList(bool shuffle)
{
    vector<string> fn_music;
    string musicDir("/mnt/us/music");
    int fnum = GetFilesInDir(musicDir,fn_music);
    
    if ( fnum > 0 )
    {
        ofstream pfile(PLAYLIST, ios::out);
        for(int i=0;i <fnum; i++)
            if (shuffle)
            {
                DebugPrintf(DLC_JUGH, "AudioPlayer::CreatPlayList shuffle");
                pfile << musicDir + "/" + fn_music[Rand(fnum)] << endl;
            }
            else
            {
                DebugPrintf(DLC_JUGH, "AudioPlayer::CreatPlayList not shuffle");
                pfile << musicDir + "/" + fn_music[i] << endl;
            }

        pfile.close();
    }
}

AUDIOPLAYER_STATUS AudioPlayer::GetPlayStatus()
{
    return  m_ePlayStatus;
}
/* End of implementation class template. */


