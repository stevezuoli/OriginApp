////////////////////////////////////////////////////////////////////////
// UIAudioPlayerDialog.cpp
// Contact: wulf
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pthread.h>
#include "TouchAppUI/UIAudioPlayerDialog.h"
#include "GUI/CTpGraphics.h"
#include "GUI/FontManager.h"
#include "GUI/GUIHelper.h"
#include "Utility/ImageManager.h"
#include "I18n/StringManager.h"
#include "Common/SystemSetting_DK.h"
#include "AudioPlayer/AudioPlayer.h"
#include "Utility.h"

#define CONTROL_BORDER (8)
#define MUSICBOX_WIDTH (400)
#define MUSICBOX_HEIGHT (116)
#define TEXT_LEFTBORDER (32)
#define TEXT_RIGHTBORDER (275)
#define TEXT_TOPBORDER (36)
#define TEXT_BOTTOMBORDER (76)
#define CIRCLE_LEFT (312)
#define CIRCLE_RIGHT (368)
#define CIRCLE_TOP (30)
#define CIRCLE_BOTTOM (86)
#define CIRCLE_MIDDLE (340)
#define CIRCLE_CENTER (58)

UIAudioPlayerDialog:: UIAudioPlayerDialog(UIContainer* pParent, const DWORD dwId)
	: UIDialog(pParent)
	, m_imgPlayStop(this, dwId)
	, m_imgVolume(this, dwId)
	, m_imgLoop(this, dwId)
	, m_imgPreAudio(this, dwId)
	, m_imgNextAudio(this, dwId)
	, m_imgVolumeBack(this, dwId)
	, m_imgVolumeIcon(this, dwId)
	, m_imgVolumeMax(this, dwId)
	, m_imgVolumeMin(this, dwId)
	, m_iLoopStatus(PLAYER_LOOP_ALL)
{
	m_dwId = dwId;
	this->SetNameText(CString(StringManager::GetStringById(SONG_NOT_FOUND)));
	this->SetAuthorText(CString(StringManager::GetStringById(AUTHOR_UNKOWN)));
	this->MoveWindow(100, 342, 400, 150);

	AudioPlayer* pAudioPlayer = AudioPlayer::GetInstance();
	if (NULL != pAudioPlayer)
	{
		int iVolume = pAudioPlayer->GetVolumeValue();
		pAudioPlayer->SetVolume(iVolume);
	} 
}

UIAudioPlayerDialog::~UIAudioPlayerDialog()
{

}

HRESULT UIAudioPlayerDialog::DrawBackground(DK_IMAGE drawingImg)
{
	CTpGraphics prg(drawingImg);
	SPtr<ITpImage> spImg = ImageManager::GetImage(IMAGE_AUDIO_PLAYER_BACK);
	if(spImg)
	{
		return prg.DrawImageBlend(spImg.Get(), 0, 0, 0, 0, spImg.Get()->GetWidth(), spImg.Get()->GetHeight());
	}
	return S_OK;
}

HRESULT UIAudioPlayerDialog::Draw(DK_IMAGE drawingImg)
{
	if (!m_bIsVisible)
	{
		return S_OK;
	}
	if (drawingImg.pbData == NULL)
	{
		return E_FAIL;
	}

	HRESULT hr(S_OK);
	DK_IMAGE imgSelf;
	DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
	RTN_HR_IF_FAILED(CropImage(
		drawingImg,
		rcSelf,
		&imgSelf
		));

	DrawBackground(imgSelf);

	CTpGraphics prg(imgSelf);
	DKFontAttributes iFontAttribute;
	iFontAttribute.SetFontAttributes(0, 0, 24);
	INT32 iColor = ColorManager::GetColor(COLOR_MENUITEM_INACTIVE);
	ITpFont *pFont = FontManager::GetInstance()->GetFont(iFontAttribute, iColor);
	if(pFont == NULL)
	{
		return 0;
	}

	//计算音乐名文本占用的宽度
	int nameUsedWidth = 0;

	int iLeft = TEXT_LEFTBORDER + CONTROL_BORDER;
	int iTop = TEXT_TOPBORDER + (TEXT_BOTTOMBORDER - TEXT_TOPBORDER - 24) / 2;
	int totalWidth = TEXT_RIGHTBORDER - TEXT_LEFTBORDER - 2 *  CONTROL_BORDER;
	nameUsedWidth = (totalWidth - CONTROL_BORDER) * 2 / 3;

	//画音乐名
	CString lable = m_csMusicName;

	if(pFont->StringWidth(lable.GetBuffer()) >=  nameUsedWidth)
	{
		lable += CString("...");
		while (pFont->StringWidth(lable.GetBuffer()) > nameUsedWidth)
		{
			lable = lable.SubString(0, lable.Length() - 4);
			lable += CString("...");
		}
	}
	prg.DrawStringUtf8(lable.GetBuffer(), iLeft, iTop, pFont);

	//画作者名
	iLeft = TEXT_LEFTBORDER + 2 * CONTROL_BORDER + nameUsedWidth;
	iTop = TEXT_TOPBORDER + (TEXT_BOTTOMBORDER - TEXT_TOPBORDER - 16) / 2 + 2;
	int iWidth = TEXT_RIGHTBORDER - iLeft - CONTROL_BORDER;

	iFontAttribute.SetFontAttributes(0, 0, 16);
	pFont = FontManager::GetInstance()->GetFont(iFontAttribute, iColor);
	if(pFont == NULL)
	{
		return 0;
	}

	lable = m_csMusicArtist;


	if(pFont->StringWidth(lable.GetBuffer()) >=  iWidth)
	{
		lable += CString("...");
		while (pFont->StringWidth(lable.GetBuffer()) > iWidth)
		{
			lable = lable.SubString(0, lable.Length() - 4);
			lable += CString("...");
		}
	}
	prg.DrawStringUtf8(lable.GetBuffer(), iLeft, iTop, pFont);

	//画图标
	iLeft = 0;
	iTop = 0;
	if(AudioPlayer::GetInstance()->GetPlayStatus()  == PLAYER_STATUS_PLAY)
	{
		m_imgPlayStop.SetImage(ImageManager::GetImage(IMAGE_AUDIO_PAUSE));
		iLeft    = CIRCLE_MIDDLE - 5;
		iTop    = CIRCLE_CENTER - 12;
	}
	else
	{
		m_imgPlayStop.SetImage(ImageManager::GetImage(IMAGE_AUDIO_PLAY_STOP));
		iLeft    = CIRCLE_MIDDLE - 10;
		iTop    = CIRCLE_CENTER - 12;
	}
	m_imgPlayStop.DrawPartial(imgSelf, 0, 0);
	m_imgPlayStop.MoveWindow(iLeft, iTop, 20, 23);

	iLeft = CIRCLE_MIDDLE - 10;
	iTop = CIRCLE_BOTTOM + (116 - CIRCLE_BOTTOM - 13) / 2;
	m_imgVolume.MoveWindow(iLeft, iTop, 18, 13);
	m_imgVolume.SetImage(ImageManager::GetImage(IMAGE_AUDIO_PLAY_STOP));
	m_imgVolume.DrawPartial(imgSelf, 0, 23);

	iLeft = CIRCLE_MIDDLE - 6;
	iTop = CIRCLE_TOP - (CIRCLE_TOP - 17) / 2 - 17;
	m_imgLoop.MoveWindow(iLeft, iTop, 12, 17);
	m_imgLoop.SetImage(ImageManager::GetImage(IMAGE_AUDIO_PLAY_LOOP));
	m_imgLoop.DrawPartial(imgSelf, 0, 0);

	iLeft = CIRCLE_RIGHT + (m_iWidth - CIRCLE_RIGHT - 15) / 2;
	iTop = CIRCLE_CENTER - 6;
	m_imgNextAudio.MoveWindow(iLeft, iTop, 15, 11);
	m_imgNextAudio.SetImage(ImageManager::GetImage(IMAGE_AUDIO_PLAY_NEXT));
	m_imgNextAudio.DrawPartial(imgSelf, 0, 0);

	iLeft = 2 * CIRCLE_MIDDLE - m_iWidth + (m_iWidth - CIRCLE_RIGHT - 15) / 2;
	iTop = CIRCLE_CENTER - 6;
	m_imgPreAudio.MoveWindow(iLeft, iTop, 15, 11);
	m_imgPreAudio.SetImage(ImageManager::GetImage(IMAGE_AUDIO_PLAY_PRE));
	m_imgPreAudio.DrawPartial(imgSelf, 0, 0);

	if(GetVolumeStatus() == VOLUME_ON)
	{
		m_imgVolumeBack.MoveWindow(86, 103, 164, 40);
		m_imgVolumeBack.SetImage(ImageManager::GetImage(IMAGE_VOLUME_BACK));
		m_imgVolumeBack.Draw(imgSelf);

		m_imgVolumeMax.MoveWindow(140, 112, 86, 21);
		m_imgVolumeMax.SetImage(ImageManager::GetImage(IMAGE_VOLUME_MAX));
		m_imgVolumeMax.DrawPartial(imgSelf, 0, 0);

		AudioPlayer* pAudioPlayer = AudioPlayer::GetInstance();
		int iVolumeValue = (NULL == pAudioPlayer) ? 0 : pAudioPlayer->GetVolumeValue();
		int volumeWidth  = 86 * (AUDIOPLAYER_VOLUME_MAX - iVolumeValue) / 10;

		m_imgVolumeMin.MoveWindow(140 + 86 - volumeWidth, 112, volumeWidth, 21);
		m_imgVolumeMin.SetImage(ImageManager::GetImage(IMAGE_VOLUME_MIN));
		m_imgVolumeMin.DrawPartial(imgSelf, 86 - volumeWidth, 0);

		m_imgVolumeIcon.MoveWindow(104, 117, 18, 13);

		m_imgVolumeIcon.SetImage(ImageManager::GetImage(IMAGE_AUDIO_VOLUME_ON));
		m_imgVolumeIcon.DrawPartial(imgSelf, 0, 0);

		m_imgVolume.SetImage(ImageManager::GetImage(IMAGE_AUDIO_PLAY_STOP));
		m_imgVolume.DrawPartial(imgSelf, 0, 23);
	}
	else
	{
		m_imgVolumeIcon.SetImage(ImageManager::GetImage(IMAGE_AUDIO_VOLUME_OFF));
		m_imgVolumeIcon.DrawPartial(imgSelf, 0, 0);

		m_imgVolume.SetImage(ImageManager::GetImage(IMAGE_AUDIO_PLAY_STOP));
		m_imgVolume.DrawPartial(imgSelf, 0, 23);
	}

	return S_OK;
}

BOOL UIAudioPlayerDialog::OnKeyPress(INT32 iKeyCode)
{
	AudioPlayer* pAudioPlayer = AudioPlayer::GetInstance();
	if (NULL == pAudioPlayer)
		return FALSE;

	switch(iKeyCode)
	{
	case KEY_OKAY:
		if(pAudioPlayer->GetPlayStatus()  == PLAYER_STATUS_PLAY)
		{
			pAudioPlayer->Pause();
		}
		else if (pAudioPlayer->GetPlayStatus()  == PLAYER_STATUS_PAUSE)
		{
			pAudioPlayer->UnPause();
		}
		else
		{
			// TODO:有时候会由于操作过快导致播放器打不开,待考虑规避方法 
			pAudioPlayer->Play(false);
		}

		Repaint();
		break;
	case KEY_UP:
		pAudioPlayer->Shuffle();
		break;
	case KEY_DOWN:
		pAudioPlayer->Stop();
		Repaint();
		break;
	case KEY_RIGHT:
		if(pAudioPlayer->GetPlayStatus() == PLAYER_STATUS_PLAY)
		{
			pAudioPlayer->Next();
		}
		break;
	case KEY_LEFT:
		if(pAudioPlayer->GetPlayStatus() == PLAYER_STATUS_PLAY)
		{
			pAudioPlayer->Preious();
		}
		break;            
	case KEY_VOLUMEUP:
	case KEY_VOLUMEDOWN:
		{
			int iVolume = pAudioPlayer->GetVolumeValue();
			iVolume += (KEY_VOLUMEUP == iKeyCode) ? 1 : -1;
			pAudioPlayer->SetVolume(iVolume);

			if(GUIHelper::GetScreenSaverStatus() == 0)
			{            
				Repaint();
			}
		}
		return TRUE;
	case KEY_Q:
		pAudioPlayer->Stop();
		EndDialog(0);
		break;
	case KEY_BACK:
		EndDialog(0);
		break;
	default:
		return UIDialog::OnKeyPress(iKeyCode);
	}
	return FALSE;
}

INT32 UIAudioPlayerDialog::SetNameText(const CHAR *strText)
{
	m_csMusicName = CString(strText);
	return 0;
}

INT32 UIAudioPlayerDialog::SetAuthorText(const CHAR *strText)
{
	m_csMusicArtist = CString(strText);
	return 0;
}

INT32 UIAudioPlayerDialog::GetVolumeStatus()
{
	DebugPrintf(DLC_JUGH, "UIAudioPlayerDialog::GetVolumeStatus Entering");    
	AudioPlayer* pAudioPlayer = AudioPlayer::GetInstance();
	if (NULL == pAudioPlayer)
		return VOLUME_OFF;
	DebugPrintf(DLC_JUGH, "UIAudioPlayerDialog::GetVolumeStatus finish :%d", pAudioPlayer->GetVolumeValue());    
	return (pAudioPlayer->GetVolumeValue() < AUDIOPLAYER_VOLUME_MIN) ? VOLUME_OFF : VOLUME_ON;
}

INT32 UIAudioPlayerDialog::GetPlayStatus()
{
	AudioPlayer* pAudioPlayer = AudioPlayer::GetInstance();    
	return (NULL == pAudioPlayer) ? PLAYER_STATUS_STOP : pAudioPlayer->GetPlayStatus();
}

BOOL UIAudioPlayerDialog::SetPlayStatus(INT32 iPlayStatus)
{
	DebugPrintf(DLC_JUGH, "UIAudioPlayerDialog::SetPlayStatus(%d) Entering", iPlayStatus);
	AudioPlayer* pAudioPlayer = AudioPlayer::GetInstance();

	if (NULL == pAudioPlayer)
	{
		DebugPrintf(DLC_ERROR, "UIAudioPlayerDialog::SetPlayStatus has not a instance");
		return FALSE;
	}

	if (iPlayStatus == GetPlayStatus())
		return TRUE;

	switch(iPlayStatus)
	{
	case PLAYER_STATUS_PLAY:
		{
			pAudioPlayer->Play(false);
			break;
		}
	case PLAYER_STATUS_STOP:
		{
			pAudioPlayer->Stop();
			break;
		}
	case PLAYER_STATUS_PAUSE:
		{
			pAudioPlayer->Pause();
			break;
		}
	default:
		return FALSE;
	}

	if(GUIHelper::GetScreenSaverStatus() == 0)
	{            
		Repaint();
	}

	DebugPrintf(DLC_JUGH, "UIAudioPlayerDialog::SetPlayStatus finish");    
	return TRUE;
}

