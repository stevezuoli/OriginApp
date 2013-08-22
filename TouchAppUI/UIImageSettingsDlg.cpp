///////////////////////////////////////////////////////////////////////
// UIFontAndStyleDlg.cpp
// Contact: liuhj
// Modified: zhangjf
// Copyright (c) Duokan Corporation. All rights reserved.
//
////////////////////////////////////////////////////////////////////////

#include "dkWinError.h"
#include "TouchAppUI/UIImageSettingsDlg.h"
#include "GUI/CTpGraphics.h"
#include "Framework/CDisplay.h"
#include "I18n/StringManager.h"
#include "Utility/ImageManager.h"
#include "Utility/ColorManager.h"

using namespace DkFormat;

#define DIALOG_WIDTH                                        (480)
#define DIALOG_HEIGHT                                       (330)
#define PADDING_HORIZON                                   (45)
#define PADDING_VERTICAL                                  (35)
#define SECTION_TITLE_FONT_SIZE                       (32)
#define SECTION_TITLE_HEIGHT                            (35)
#define SECTION_MARGIN_TOP                              (35)
#define ROTATE_CHECKICON_MARGIN_TOP             (35)
#define ROTATE_CHECKICON_MARGIN_BOTTOM       (15)
#define CARTOON_CHECKICON_MARGIN_TOP           (40)
#define CARTOON_CHECKICON_MARGIN_BOTTOM     (35)
#define OPTION_WIDTH                                         (110)
#define CHECK_ICON_WIDTH                                 (12)
#define CHECK_ICON_HEIGHT                                (12)
#define CHECK_ICON_OPTION_SPACE                     (15)
#define SELECTLINE_MARGIN_TOP                          (7)
#define SOLID_LINE_HEIGHT                                  (5)
#define DOTTED_LINE_HEIGHT                                 (1)

#define IMAGE_ROTATE_TITLE                         (CString(StringManager::GetStringById(IMAGE_ROTATE_OPTIONS)))
#define IMAGE_CARTOON_TITLE                      (CString(StringManager::GetStringById(IMAGE_CARTOON_MODE)))

UIImageSettingsDlg::UIImageSettingsDlg(UIContainer * pParent)
	: UIDialog(pParent)
	, m_eImgCurSection(IMAGE_ROTATE_SECTION)
	, m_eImgDisplaySelected(IMAGE_DISPLAY_ORIGINAL)
	, m_eImgDisplayChecked(IMAGE_DISPLAY_ORIGINAL)
	, m_eImgRotateSelected(IMAGE_ROTATE_NONE)
	, m_eImgRotateChecked(IMAGE_ROTATE_NONE)
	, m_eImgCartoonSelected(CARTOON_ALL)
	, m_eImgCartoonChecked(CARTOON_ALL)
{
	//do nothing
}

UIImageSettingsDlg::~UIImageSettingsDlg()
{
	this->Dispose();
}

HRESULT UIImageSettingsDlg::Initialize(INT iImgRotateAngle, CartoonMode eCartoonMode, ImgDisplay eImgSize)
{

	if(iImgRotateAngle != 0  && iImgRotateAngle != 90 &&
		iImgRotateAngle != 180 && iImgRotateAngle != 270)
	{
		return E_INVALIDARG;
	}

	this->m_eImgRotateChecked = (ImgRotate)(iImgRotateAngle);
	this->m_eImgRotateSelected = m_eImgRotateChecked;

	this->m_eImgCartoonChecked = eCartoonMode;
	this->m_eImgCartoonSelected = eCartoonMode;

	this->m_eImgDisplayChecked = eImgSize;
	this->m_eImgDisplaySelected = eImgSize;

	this->MoveWindow((this->GetScreenWidth () - DIALOG_WIDTH)>>1, 
		(this->GetScreenHeight() - DIALOG_HEIGHT)>>1,
		DIALOG_WIDTH, 
		DIALOG_HEIGHT);

	HRESULT hr = this->InitMember();
	if(FAILED(hr))
	{
		return hr;
	}

	hr = this->InitUI();
	if(FAILED(hr))
	{
		return hr;
	}

	return S_OK;
}

HRESULT UIImageSettingsDlg::InitMember()
{
	this->m_spImgChecked = ImageManager::GetImage(IMAGE_SYS_OPTION_CHECKED);
	if(!this->m_spImgChecked)
	{
		return E_FAIL;
	}

	this->m_spImgUnchecked = ImageManager::GetImage(IMAGE_SYS_OPTION_UNCHECKED);
	if(!this->m_spImgUnchecked)
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT UIImageSettingsDlg::InitUI()
{
	this->m_imgRotateTitle.SetText(IMAGE_ROTATE_TITLE);
	this->m_imgRotateTitle.SetForeColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
	this->m_imgRotateTitle.SetFontSize(SECTION_TITLE_FONT_SIZE);
	this->m_imgRotateTitle.MoveWindow(PADDING_HORIZON, 
		PADDING_VERTICAL, 
		DIALOG_WIDTH -PADDING_HORIZON,  
		SECTION_TITLE_HEIGHT);

	this->AppendChild(&(this->m_imgRotateTitle));

	INT iRotateCheckIconTop = PADDING_VERTICAL + SECTION_TITLE_HEIGHT + ROTATE_CHECKICON_MARGIN_TOP;
	INT iItemLeft = PADDING_HORIZON;
	INT iRotateSectionBottom = iRotateCheckIconTop + CHECK_ICON_HEIGHT + ROTATE_CHECKICON_MARGIN_BOTTOM;
	INT iRotateIconBottom = iRotateSectionBottom - SELECTLINE_MARGIN_TOP;

	INT iIconWidth = 0;
	INT iIconHeight = 0;
	UINT uIndex = 0;
	while(uIndex < IMAGE_ROTATE_OPTION_COUNT)
	{
		if((uIndex * 90) == (UINT)this->m_eImgRotateChecked)
		{
			if(this->m_spImgChecked)
			{
				this->m_imgRotateCheckIconTable[uIndex].SetImage(this->m_spImgChecked);
			}
			else
			{
				return E_FAIL;
			}
		}
		else
		{
			if(this->m_spImgUnchecked)
			{
				this->m_imgRotateCheckIconTable[uIndex].SetImage(this->m_spImgUnchecked);
			}
			else
			{
				return E_FAIL;
			}
		}

		this->m_imgRotateCheckIconTable[uIndex].MoveWindow(iItemLeft, iRotateCheckIconTop, CHECK_ICON_WIDTH,  CHECK_ICON_HEIGHT);
		this->AppendChild(&(this->m_imgRotateCheckIconTable[uIndex]));

		INT iRotateIconLeft = iItemLeft + CHECK_ICON_WIDTH + CHECK_ICON_OPTION_SPACE;

        // Remark(zhaigh): 图片名里的旋转90指的是kindle，内部处理时旋转角度指的阅读界面，刚好相反
		if(0 == uIndex)
		{
			this->m_imgRotateOptionTable[uIndex].SetImage(ImageManager::GetImage(IMAGE_IMG_ROTATE_0_ICON));
		}
		if(1 == uIndex)
		{
			this->m_imgRotateOptionTable[uIndex].SetImage(ImageManager::GetImage(IMAGE_IMG_ROTATE_270_ICON));
		}
		if(2 == uIndex)
		{
			this->m_imgRotateOptionTable[uIndex].SetImage(ImageManager::GetImage(IMAGE_IMG_ROTATE_180_ICON));
		}
		if(3 == uIndex)
		{
			this->m_imgRotateOptionTable[uIndex].SetImage(ImageManager::GetImage(IMAGE_IMG_ROTATE_90_ICON));
		}

		iIconWidth = this->m_imgRotateOptionTable[uIndex].GetImageWidth();
		if(iIconWidth <= 0)
		{
			return E_FAIL;
		}

		iIconHeight = this->m_imgRotateOptionTable[uIndex].GetImageHeight();
		if(iIconHeight <= 0)
		{
			return E_FAIL;
		}

		this->m_imgRotateOptionTable[uIndex].MoveWindow(iRotateIconLeft, iRotateIconBottom - iIconHeight, iIconWidth, iIconHeight);
		this->AppendChild(&(this->m_imgRotateOptionTable[uIndex]));

		this->m_rotateUnderlineAreaTable[uIndex].left = iRotateIconLeft - 3;
		this->m_rotateUnderlineAreaTable[uIndex].top = iRotateSectionBottom - 1;
		this->m_rotateUnderlineAreaTable[uIndex].right = iRotateIconLeft + iIconWidth + 3;
		this->m_rotateUnderlineAreaTable[uIndex].bottom = iRotateSectionBottom;

		iItemLeft += OPTION_WIDTH;
		uIndex++;
	}

	uIndex = 0;
	iItemLeft = PADDING_HORIZON;
	INT iCartoonSectionTop =  iRotateSectionBottom + SECTION_MARGIN_TOP;
	INT iCartoonCheckIconTop = iCartoonSectionTop + SECTION_TITLE_HEIGHT + CARTOON_CHECKICON_MARGIN_TOP;
	INT iCartoonSectionBottom = iCartoonCheckIconTop + CHECK_ICON_HEIGHT + CARTOON_CHECKICON_MARGIN_BOTTOM;
	INT iCartoonIconBottom = iCartoonSectionBottom - SELECTLINE_MARGIN_TOP;

	this->m_imgCartoonTitle.SetText(IMAGE_CARTOON_TITLE);
	this->m_imgCartoonTitle.SetForeColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
	this->m_imgCartoonTitle.SetFontSize(SECTION_TITLE_FONT_SIZE);
	this->m_imgCartoonTitle.MoveWindow(iItemLeft, iCartoonSectionTop, DIALOG_WIDTH -PADDING_HORIZON, SECTION_TITLE_HEIGHT);
	this->AppendChild(&(this->m_imgCartoonTitle));

	while(uIndex < IMAGE_CARTOON_MODE_COUNT)
	{
		if(uIndex == (UINT)(this->m_eImgCartoonChecked))
		{
			if(this->m_spImgChecked)
			{
				this->m_imgCartoonCheckIconTable[uIndex].SetImage(this->m_spImgChecked);
			}
			else
			{
				return E_FAIL;
			}
		}
		else
		{
			if(this->m_spImgUnchecked)
			{
				this->m_imgCartoonCheckIconTable[uIndex].SetImage(this->m_spImgUnchecked);
			}
			else
			{
				return E_FAIL;
			}
		}

		this->m_imgCartoonCheckIconTable[uIndex].MoveWindow(iItemLeft,iCartoonCheckIconTop, CHECK_ICON_WIDTH, CHECK_ICON_HEIGHT);
		this->AppendChild(&(this->m_imgCartoonCheckIconTable[uIndex]));

		INT iCartoonIconLeft =  iItemLeft + CHECK_ICON_WIDTH + CHECK_ICON_OPTION_SPACE; 

		if(0 == uIndex)
		{
			this->m_imgCartoonOptionTable[uIndex].SetImage(ImageManager::GetImage(IMAGE_IMG_CARTOON_ALL_ICON));
		}
		if(1 == uIndex)
		{
			this->m_imgCartoonOptionTable[uIndex].SetImage(ImageManager::GetImage(IMAGE_IMG_CARTOON_LR_ICON));
		}
		if(2 == uIndex)
		{
			this->m_imgCartoonOptionTable[uIndex].SetImage(ImageManager::GetImage(IMAGE_IMG_CARTOON_RL_ICON));
		}

		iIconWidth = this->m_imgCartoonOptionTable[uIndex].GetImageWidth();
		if(iIconWidth <= 0)
		{
			return E_FAIL;
		}

		iIconHeight = this->m_imgCartoonOptionTable[uIndex].GetImageHeight();
		if(iIconHeight <= 0)
		{
			return E_FAIL;
		}

		this->m_imgCartoonOptionTable[uIndex].MoveWindow(iCartoonIconLeft, iCartoonIconBottom - iIconHeight, iIconWidth, iIconHeight);
		this->AppendChild(&(this->m_imgCartoonOptionTable[uIndex]));

		this->m_cartoonUnderlineAreaTable[uIndex].left = iCartoonIconLeft - 3;
		this->m_cartoonUnderlineAreaTable[uIndex].top = iCartoonSectionBottom - 1;
		this->m_cartoonUnderlineAreaTable[uIndex].right = iCartoonIconLeft + iIconWidth + 3;
		this->m_cartoonUnderlineAreaTable[uIndex].bottom = iCartoonSectionBottom;

		iItemLeft += OPTION_WIDTH;
		uIndex++;                                                                         
	}

	return S_OK;
}

BOOL UIImageSettingsDlg::OnKeyPress(INT32 iKeyCode)
{
	if (this->m_spImgChecked && this->m_spImgUnchecked)
	{
		switch (iKeyCode)
		{
		case KEY_LEFT:
			if(this->m_eImgCurSection == IMAGE_ROTATE_SECTION)
			{
				if (this->m_eImgRotateSelected != IMAGE_ROTATE_NONE)
				{
					this->m_eImgRotateSelected = (ImgRotate)((INT)(this->m_eImgRotateSelected) - 90);
					this->UpdateWindow();
				}
			}
			else if(this->m_eImgCurSection == IMAGE_CARTOON_SECTION)
			{
				if(this->m_eImgCartoonSelected != CARTOON_ALL)
				{
					this->m_eImgCartoonSelected = (CartoonMode)((INT)this->m_eImgCartoonSelected - 1);
					this->UpdateWindow();
				}
			}
			break;

		case KEY_RIGHT:
			if(this->m_eImgCurSection == IMAGE_ROTATE_SECTION)
			{
				if (this->m_eImgRotateSelected != IMAGE_ROTATE_270)
				{
					this->m_eImgRotateSelected = (ImgRotate)((INT)(this->m_eImgRotateSelected) + 90);
					this->UpdateWindow();
				}
			}
			else if(this->m_eImgCurSection == IMAGE_CARTOON_SECTION)
			{
				if(this->m_eImgCartoonSelected != CARTOON_RIGHT_LEFT)
				{
					this->m_eImgCartoonSelected = (CartoonMode)((INT)this->m_eImgCartoonSelected + 1);
					this->UpdateWindow();
				}
			}
			break;

		case KEY_UP:
			if(this->m_eImgCurSection == IMAGE_CARTOON_SECTION)
			{
				this->m_eImgCurSection = IMAGE_ROTATE_SECTION;
				this->UpdateWindow();
			}
			break;

		case KEY_DOWN:
			if(this->m_eImgCurSection == IMAGE_ROTATE_SECTION)
			{
				this->m_eImgCurSection = IMAGE_CARTOON_SECTION;
				this->UpdateWindow();
			}
			break;

		case KEY_BACK:
		case KEY_FONT:
			EndDialog(0);
			return FALSE;

		case KEY_OKAY:
			if(this->m_eImgCurSection == IMAGE_ROTATE_SECTION)
			{
				this->m_eImgRotateChecked = this->m_eImgRotateSelected;
			}
			else if(this->m_eImgCurSection == IMAGE_CARTOON_SECTION)
			{
				this->m_eImgCartoonChecked = this->m_eImgCartoonSelected;
			}

			EndDialog(0);
			return FALSE;

		default:
			break;
		}
	}

	return UIDialog::OnKeyPress (iKeyCode);
}

HRESULT UIImageSettingsDlg::DrawBackGround(DK_IMAGE drawingImg)
{
	HRESULT hr = S_OK;
	DK_IMAGE imgSelf;
	DK_RECT rcSelf={0, 0, m_iWidth, m_iHeight};

	RTN_HR_IF_FAILED(CropImage(
		drawingImg,
		rcSelf,
		&imgSelf));

	CTpGraphics grp(imgSelf);

	SPtr<ITpImage> spImg = ImageManager::GetImage (IMAGE_MENU_BACK); 
	if (spImg)
	{
		RTN_HR_IF_FAILED(grp.DrawImageStretchBlend (spImg.Get (), 0, 0, m_iWidth, m_iHeight));
	}

	UINT uIndex = 0;
	while(uIndex < IMAGE_ROTATE_OPTION_COUNT)
	{
		if((uIndex * 90) == (UINT)(this->m_eImgRotateSelected) && 
			this->m_eImgCurSection == IMAGE_ROTATE_SECTION)
		{
			grp.DrawLine (
				this->m_rotateUnderlineAreaTable[uIndex].left,
				this->m_rotateUnderlineAreaTable[uIndex].top - 2,
				this->m_rotateUnderlineAreaTable[uIndex].right - this->m_rotateUnderlineAreaTable[uIndex].left,
				SOLID_LINE_HEIGHT,
				SOLID_STROKE);
		}
		else
		{
			grp.DrawLine (
				this->m_rotateUnderlineAreaTable[uIndex].left,
				this->m_rotateUnderlineAreaTable[uIndex].top,
				this->m_rotateUnderlineAreaTable[uIndex].right - this->m_rotateUnderlineAreaTable[uIndex].left,
				DOTTED_LINE_HEIGHT,
				DOTTED_STROKE);
		}
		uIndex++;
	}

	uIndex = 0;
	while(uIndex < IMAGE_CARTOON_MODE_COUNT)
	{
		if(uIndex == (UINT)(this->m_eImgCartoonSelected) &&
			this->m_eImgCurSection == IMAGE_CARTOON_SECTION)
		{
			grp.DrawLine (
				this->m_cartoonUnderlineAreaTable[uIndex].left,
				this->m_cartoonUnderlineAreaTable[uIndex].top - 2,
				this->m_cartoonUnderlineAreaTable[uIndex].right - this->m_cartoonUnderlineAreaTable[uIndex].left,
				SOLID_LINE_HEIGHT,
				SOLID_STROKE);
		}
		else
		{
			grp.DrawLine (
				this->m_cartoonUnderlineAreaTable[uIndex].left,
				this->m_cartoonUnderlineAreaTable[uIndex].top,
				this->m_cartoonUnderlineAreaTable[uIndex].right - this->m_cartoonUnderlineAreaTable[uIndex].left,
				DOTTED_LINE_HEIGHT,
				DOTTED_STROKE);
		}
		uIndex++;
	}
	return hr;
}

INT UIImageSettingsDlg::GetImgRotateAngle()
{
	return (INT)(this->m_eImgRotateChecked);
}

CartoonMode UIImageSettingsDlg::GetCartoonMode()
{
	return this->m_eImgCartoonChecked;
}

ImgDisplay UIImageSettingsDlg::GetImgSizeSelected()
{
	return this->m_eImgDisplayChecked;
}

void UIImageSettingsDlg::Dispose()
{
	UIDialog::Dispose();
	this->OnDispose(this->m_bIsDisposed);
}

void UIImageSettingsDlg::OnDispose(BOOL fDisposed)
{
	if(fDisposed)
	{
		return;
	}

	this->m_bIsDisposed = fDisposed;
}



