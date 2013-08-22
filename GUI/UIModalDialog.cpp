////////////////////////////////////////////////////////////////////////
// UIModalDialog.cpp
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "GUI/UIModalDialog.h"
#include "Utility/ImageManager.h"
#include "Utility/ColorManager.h"
#include "GUI/CTpGraphics.h"
#include "Framework/CDisplay.h"
#include "I18n/StringManager.h"
#include "Common/WindowsMetrics.h"
using namespace WindowsMetrics;

UIModalDlg::UIModalDlg(UIContainer* pParent, LPCSTR szTitle)
    : UIDialog(pParent)
    , m_txtTitle(this, IDSTATIC)
    , m_pTopSizer(NULL)
{
    m_txtTitle.SetText(szTitle);
    m_txtTitle.SetFontSize(GetWindowFontSize(UIModalDialogTitleIndex));
#ifdef KINDLE_FOR_TOUCH
    m_btnCancelIcon.Initialize(IDCANCEL, "", GetWindowFontSize(UIModalDialogButtonIndex));
    SPtr<ITpImage> spImg = ImageManager::GetImage(IMAGE_TOUCH_ICON_CLOSE);
    if (spImg)
    {
        m_btnCancelIcon.ShowBorder(false);
        m_btnCancelIcon.SetElemSpacing(0);
        m_btnCancelIcon.SetLeftMargin(0);
        m_btnCancelIcon.SetIcon(spImg, UIButton::ICON_RIGHT);
        m_btnCancelIcon.SetVisible(false);
    }
#endif
    m_btnGroup.SetMinHeight(GetWindowMetrics(UIModalDialogBtnsHeightIndex));
    m_btnGroup.SetSplitLineHeight(GetWindowMetrics(UIModalDialogBtnsHeightIndex) - (GetWindowMetrics(UIButtonGroupSplitLineTopPaddingIndex) << 1));
    m_btnGroup.SetBottomLinePixel(0);
    AppendChild(&m_btnGroup);
    if (!m_windowSizer)
    {
        UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);

        if (mainSizer)
        {
            m_pTopSizer = new UIBoxSizer(dkHORIZONTAL);
            if (m_pTopSizer)
            {
                m_pTopSizer->Add(&m_txtTitle, UISizerFlags().Center());
                m_pTopSizer->AddStretchSpacer();
#ifdef KINDLE_FOR_TOUCH
                m_pTopSizer->Add(&m_btnCancelIcon, UISizerFlags().Center());
#endif
            }
            mainSizer->Add(m_pTopSizer, 
                UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIModalDialogHorizonMarginIndex))
                .Border(dkTOP | dkBOTTOM, GetWindowMetrics(UIModalDialogTitleVertBorderIndex)));
            SetSizer(mainSizer);
        }
    }
}

UIModalDlg::~UIModalDlg()
{
}

void UIModalDlg::ShowCloseIcon(bool isVisible)
{
#ifdef KINDLE_FOR_TOUCH
    m_btnCancelIcon.SetVisible(isVisible);
    AppendChild(&m_btnCancelIcon);
#endif
}

HRESULT UIModalDlg::DrawBackGround(DK_IMAGE drawingImg)
{
    HRESULT hr(S_OK);
    DrawBackgroundWithRoundBorder(drawingImg);
    return hr;
}

void UIModalDlg::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    switch (dwCmdId)
    {
        case IDCANCEL:
            EndDialog(IDCANCEL); 
    }
}
