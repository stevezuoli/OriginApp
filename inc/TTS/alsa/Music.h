#ifndef _MUSIC_H
#define _MUSIC_H

void ts_SetVolume(int valume);
int ts_GetVolume(int channel);
void ts_SetMute(int MuteFlag);
void InputThread (int data);
void* DecodeThread (void* data);
int tsa_PlayFile(void * Device, char * filename);
void *tsa_Init(char *DeviceName);
int tsa_Close(void *handle);
void play();
void SearchSong();
void* MusicOperate(void* data);
void RandPlay();
void nextsong();
void prefsong();
void MusicShowName();
void SeekToData(FILE * f1);
int  play_words (char *name);
//void LoadSong();
void* TimeCount(void* data);
void SearchMusic();
void Kplay(int);
void Kplaypref();
void Kplaynext();
void stopplaying();
void Kplayermute();
void * monitplaying();
void playorpause();
void quitplaying();
void playorstop();
#endif
