///////////////////////////////////////////////////////////////////////
// UIImageSettings.h
// Contact: zhangjf
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#ifndef __UIIMAGESETTINGSDLG_H__
#define __UIIMAGEREADERCONTAINER_H__

#include "GUI/UIImage.h"
#include "GUI/UIDialog.h"
#include "GUI/UITextSingleLine.h"
#include "DkSPtr.h"

typedef enum
{
    IMAGE_SIZE_SECTION,
    IMAGE_ROTATE_SECTION,
    IMAGE_CARTOON_SECTION
}ImgDlgSection, *PImgDlgSection;

typedef enum
{
    IMAGE_DISPLAY_ORIGINAL,
    IMAGE_DISPLAY_ZOOMIN,
    IMAFE_DISPLAY_ZOOMOUT
}ImgDisplay, *PImgDisplay;

typedef enum
{
    IMAGE_ROTATE_NONE = 0,
    IMAGE_ROTATE_90 = 90,
    IMAGE_ROTATE_180 = 180,
    IMAGE_ROTATE_270 = 270
}ImgRotate, *PImgRotate;

typedef enum
{
    CARTOON_ALL,
    CARTOON_LEFT_RIGHT,
    CARTOON_RIGHT_LEFT
}CartoonMode, *PCartoonMode;

#define IMAGE_DISPLAY_OPTION_COUNT           (3)
#define IMAGE_ROTATE_OPTION_COUNT       (4)
#define IMAGE_CARTOON_MODE_COUNT       (3)

class UIImageSettingsDlg : public UIDialog
{
public:
    UIImageSettingsDlg(UIContainer *pParent);
    virtual ~UIImageSettingsDlg();

    virtual LPCSTR GetClassName() const
    {
        return "UIImageSettingsDlg";
    }

    HRESULT Initialize(INT iImgRotateAngle, CartoonMode eCartoonMode, ImgDisplay eImgSize = IMAGE_DISPLAY_ORIGINAL);

    virtual BOOL OnKeyPress(INT32 iKeyCode);

    INT GetImgRotateAngle();
    CartoonMode GetCartoonMode();
    ImgDisplay GetImgSizeSelected();

protected:
    void Dispose();
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);

private:
    HRESULT InitMember();
    HRESULT InitUI();
    void OnDispose(BOOL fDisposed);

private:
    SPtr<ITpImage> m_spImgChecked;
    SPtr<ITpImage> m_spImgUnchecked;
    ImgDlgSection m_eImgCurSection;
    BOOL m_bIsDisposed;

    //image display section
    UITextSingleLine m_imgDisplayTitle;
    UITextSingleLine m_imgDisplayOptionTable[IMAGE_DISPLAY_OPTION_COUNT];
    UIImage m_imgDisplayCheckIconTable[IMAGE_DISPLAY_OPTION_COUNT];
    DK_RECT m_displayUnderlineAreaTable[IMAGE_DISPLAY_OPTION_COUNT];
    ImgDisplay m_eImgDisplaySelected;
    ImgDisplay m_eImgDisplayChecked;

    //image rotate section
    UITextSingleLine m_imgRotateTitle;
    UIImage m_imgRotateOptionTable[IMAGE_ROTATE_OPTION_COUNT];
    UIImage m_imgRotateCheckIconTable[IMAGE_ROTATE_OPTION_COUNT];
    DK_RECT m_rotateUnderlineAreaTable[IMAGE_ROTATE_OPTION_COUNT];
    ImgRotate m_eImgRotateSelected;
    ImgRotate m_eImgRotateChecked;

    //image cartoon section
    UITextSingleLine m_imgCartoonTitle;
    UIImage m_imgCartoonOptionTable[IMAGE_CARTOON_MODE_COUNT];
    UIImage m_imgCartoonCheckIconTable[IMAGE_CARTOON_MODE_COUNT];
    DK_RECT m_cartoonUnderlineAreaTable[IMAGE_CARTOON_MODE_COUNT];
    CartoonMode m_eImgCartoonSelected;
    CartoonMode m_eImgCartoonChecked;
    
};

#endif
