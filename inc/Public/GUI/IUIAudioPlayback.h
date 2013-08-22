////////////////////////////////////////////////////////////////////////
// IUIAudioPlayback.h
// Contact: xzhan
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef IUIAUDIOPLAYBACK_H_
#define IUIAUDIOPLAYBACK_H_

#include "Public/GUI/IUIContainer.h"
#include "Utility/IInputStream.h"

class IUIAudioPlayback : public virtual IUIContainer
{
public:
    virtual INT32 GetDesiredHeight() = 0;
    virtual void SetBackGround(INT32 iRGB) = 0;
    virtual void SetTransparent(BOOL bIsTrans) = 0;
    virtual INT32 GetState() = 0;

    virtual void SetAudio(IInputStream* pInputStream, const CString& rstrContentType);
    // SetLoading should be called before SetAudio or SetNotAcailable
    virtual void SetLoading(BOOL bAnimation, const CString& strLoading);

    virtual void SetNotAvailable() = 0;

    virtual void Pause() = 0;
    virtual void Stop() = 0;
    virtual void Play() = 0;

    virtual void IncreaseVolume() = 0;
    virtual void DecreaseVolume() = 0;
    virtual INT32 GetVolume() = 0;

    virtual INT32 GetCurrentPlayTime() = 0;
    virtual INT32 GetDuration() = 0;     
    virtual ~IUIAudioPlayback() {}
};
#endif /*IUIAUDIOPLAYBACK_H_*/
