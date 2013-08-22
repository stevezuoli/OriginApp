////////////////////////////////////////////////////////////////////////
// UIImage.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UIIMAGE_H__
#define __UIIMAGE_H__

#include "GUI/UIWindow.h"
#include "GUI/UIContainer.h"
#include "DkSPtr.h"

#include "Utility/CImageLoader.h"

//using namespace DkFormat;

class UIImage 
    : public UIWindow
{
public:
    virtual LPCSTR GetClassName() const
    {
        return ("UIImage");
    }

    UIImage();
    UIImage(UIContainer* pParent, const DWORD dwId);

    virtual ~UIImage();

    virtual void SetAutoSize(bool autoSize);
    virtual HRESULT DrawPartial(DK_IMAGE drawingImg, INT32 xInImage = 0, INT32 yInImage = 0);
    virtual HRESULT Draw(DK_IMAGE drawingImg);
    virtual void SetImage(SPtr<ITpImage> spImg);
    virtual void SetImage(int imgId);

    virtual INT32 GetImageWidth();
    virtual INT32 GetImageHeight();

    virtual void SetAlign(INT32 iAlign);
    virtual void SetVAlign(INT32 iVAlign);
    virtual int GetDrawLeftPositon() const;
    virtual int GetDrawTopPositon() const;
    virtual void SetImageFile(const char* imageFilePath);
    virtual void MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight);

protected:    
    void SetImageInternal(SPtr<ITpImage> image);
    virtual void OnResize(INT32 iWidth, INT32 iHeight);
    HRESULT DrawFocus(DK_IMAGE drawingImg);
    void Init();
    void Dispose();

    SPtr<ITpImage> m_spImage;

private:
    HRESULT DrawImage(DK_IMAGE drawingImg, INT32 xInImage, INT32 yInImage);
    
    bool m_autoSize;
    bool m_bIsDisposed;
    int   m_iAlign;
    int   m_iVAlign;
    std::string m_imageFile;

private:
    void OnDispose(BOOL bIsDisposed);
};
#endif
