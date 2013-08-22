#include "PersonalUI/UIPersonalExperienceLabel.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "GUI/ITpGraphics.h"
#include "GUI/UIImage.h"
#include "Utility/ColorManager.h"

using namespace WindowsMetrics;

UIPersonalExperienceLabel::UIPersonalExperienceLabel()
	: UIContainer(NULL, IDSTATIC)
{
	Init();
}

UIPersonalExperienceLabel::UIPersonalExperienceLabel(UIContainer* pParent)
	: UIContainer(pParent, IDSTATIC)
{
	Init();
}

UIPersonalExperienceLabel::~UIPersonalExperienceLabel()
{

}

void UIPersonalExperienceLabel::Init()
{
	const int margin = 15;
    const int fontsize20 = GetWindowFontSize(FontSize20Index);
    const int fontsize42 = GetWindowFontSize(FontSize42Index);
    m_title.SetFontSize(fontsize20);
    m_title.SetForeColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
    m_title.SetAlign(ALIGN_LEFT);
    AppendChild(&m_title);
    
    m_num.SetFontSize(fontsize42);
    m_num.SetAlign(ALIGN_LEFT);
    AppendChild(&m_num);

	m_quantifier.SetFontSize(fontsize20);
    m_quantifier.SetForeColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
    m_quantifier.SetAlign(ALIGN_RIGHT);
    AppendChild(&m_quantifier);

    UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
    UISizer* botttomSizer = new UIBoxSizer(dkHORIZONTAL);
    if (mainSizer && botttomSizer)
    {
        botttomSizer->Add(&m_num, UISizerFlags().Border(dkLEFT, margin).Align(dkALIGN_LEFT | dkALIGN_BOTTOM));
		botttomSizer->AddStretchSpacer();
        botttomSizer->Add(&m_quantifier, UISizerFlags().Border(dkRIGHT, margin).Align(dkALIGN_RIGHT | dkALIGN_BOTTOM));

		mainSizer->AddSpacer(margin);
		mainSizer->Add(&m_title, UISizerFlags().Border(dkLEFT, margin));
		mainSizer->AddStretchSpacer();
		mainSizer->Add(botttomSizer, UISizerFlags().Expand());
		mainSizer->AddSpacer(margin);
        SetSizer(mainSizer);
    }
    else
    {
        SafeDeletePointer(mainSizer);
        SafeDeletePointer(botttomSizer);
    }
}

HRESULT UIPersonalExperienceLabel::DrawBackGround(DK_IMAGE drawingImg)
{
    HRESULT hr(S_OK);
    CTpGraphics grf(drawingImg);
	RTN_HR_IF_FAILED(grf.EraserBackGround());
	
	RTN_HR_IF_FAILED(grf.FillRect(0, 0, m_iWidth, m_iHeight, ColorManager::GetColor(COLOR_DISABLECOLOR)));
	RTN_HR_IF_FAILED(grf.FillRect(1, 1, m_iWidth - 1, m_iHeight - 1, ColorManager::knWhite));
	SPtr<ITpImage> pUpLeftImage = ImageManager::GetImage(IMAGE_ICON_BTN_UPLEFT_DISABLE);
	SPtr<ITpImage> pUpRightImage = ImageManager::GetImage(IMAGE_ICON_BTN_UPRIGHT_DISABLE);
	SPtr<ITpImage> pLeftDownImage = ImageManager::GetImage(IMAGE_ICON_BTN_LEFTDOWN_DISABLE);
	SPtr<ITpImage> pRightDownImage = ImageManager::GetImage(IMAGE_ICON_BTN_RIGHTDOWN_DISABLE);
	if(pUpLeftImage && pUpRightImage && pLeftDownImage && pRightDownImage)
	{
		RTN_HR_IF_FAILED(grf.DrawImageBlend(pUpLeftImage.Get(), 0, 0, 0, 0, pUpLeftImage->GetWidth(),  pUpLeftImage->GetHeight()));
		int iUpRightWidth = pUpRightImage->GetWidth();
		RTN_HR_IF_FAILED(grf.DrawImageBlend(pUpRightImage.Get(), m_iWidth - iUpRightWidth, 0, 0, 0, iUpRightWidth,  pUpRightImage->GetHeight()));
		int iLeftDownWidth  = pLeftDownImage->GetWidth();
		int iLeftDownHeight = pLeftDownImage->GetHeight();
		RTN_HR_IF_FAILED(grf.DrawImageBlend(pLeftDownImage.Get(), 0, m_iHeight - iLeftDownHeight, 0, 0, iLeftDownWidth,  iLeftDownHeight));
		int iRightDownWidth  = pRightDownImage->GetWidth();
		int iRightDownHeight = pRightDownImage->GetHeight();
		RTN_HR_IF_FAILED(grf.DrawImageBlend(pRightDownImage.Get(), m_iWidth - iRightDownWidth, m_iHeight - iRightDownHeight, 0, 0, iRightDownWidth,  iRightDownHeight));
	}
	return hr;
}

void UIPersonalExperienceLabel::InitItem(CString title, CString num, CString quantifier)
{
	m_title.SetText(title);
	m_num.SetText(num);
    m_quantifier.SetText(quantifier);
}

