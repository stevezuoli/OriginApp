////////////////////////////////////////////////////////////////////////
// UIAudioPlayerDialog.h
// Contact: wulf
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UIAUDIOPLAYERDIALOG_H__
#define __UIAUDIOPLAYERDIALOG_H__

#include "GUI/UIText.h"
#include "DkSPtr.h"
#include "GUI/UIDialog.h"
#include "GUI/UIButton.h"
#include "Public/Base/TPDef.h"
#include "GUI/UIImage.h"
#include "Utility/ColorManager.h"
#include "Utility/CString.h"
#include <time.h>

class UIAudioPlayerDialog : public UIDialog
{
public:
    UIAudioPlayerDialog(UIContainer* pParent, const DWORD dwId);
    virtual    ~UIAudioPlayerDialog();

    virtual    LPCSTR GetClassName() const
    {
        return ("UIAudioPlayerDialog");
    }
    virtual    HRESULT Draw(DK_IMAGE image);
    virtual    HRESULT DrawBackground(DK_IMAGE drawingImg);

    BOOL    OnKeyPress(INT32 iKeyCode);
    
    INT32    GetPlayStatus();
    BOOL    SetPlayStatus(INT32 iPlayStatus);

private:    
    
    INT32    SetNameText(const CHAR *strText);
    INT32    SetAuthorText(const CHAR *strText);

    void    SetVolume(INT32 iVolumeValue);
    INT32    GetVolumeStatus();

    INT32    GetLoopStatus();

protected:
    DWORD        m_dwId;
    
private:
    CString        m_csMusicName;
    CString        m_csMusicArtist;
    
    UIImage        m_imgPlayStop;
    UIImage        m_imgVolume;
    UIImage        m_imgLoop;    
    UIImage        m_imgPreAudio;
    UIImage        m_imgNextAudio;    
    UIImage        m_imgVolumeBack;
    UIImage        m_imgVolumeIcon;    
    UIImage        m_imgVolumeMax;
    UIImage        m_imgVolumeMin;
    
    INT32        m_iLoopStatus;
public:
    enum
    {
        VOLUME_ON = 0,
        VOLUME_OFF = 1,
    };
};

#endif
