#include "BookStoreUI/UIBookStoreFetchInfo.h"
#include "CommonUI/UIUtility.h"
#include "drivers/DeviceInfo.h"
#include "I18n/StringManager.h"
#include "GUI/UISizer.h"
#include "GUI/CTpGraphics.h"
#include "Common/WindowsMetrics.h"

using namespace dk::bookstore;
using namespace WindowsMetrics;

UIBookStoreFetchInfo::UIBookStoreFetchInfo()
    : UIContainer()
    , m_fetchStatus(NOFETCH)
    , m_fetchErrorSizer(NULL)
    , m_loadingSizer(NULL)
    , m_noElemSizer(NULL)
    , m_noElemCustomizedSizer(NULL)
    , m_bCustomized(false)
{
    Init();
}

UIBookStoreFetchInfo::~UIBookStoreFetchInfo()
{
}

void UIBookStoreFetchInfo::Init()
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    const int btnHMargin = GetWindowMetrics(UIBtnDefaultHorizontalMarginIndex);
    const int btnVMargin = GetWindowMetrics(UIBtnDefaultVerticalMarginIndex);
    const int fontsize18 = GetWindowFontSize(FontSize18Index);
    const int fontsize22 = GetWindowFontSize(FontSize22Index);

    m_error.SetFontSize(fontsize22);
    m_error.SetText(StringManager::GetPCSTRById(BOOKSTORE_LOADINGDATAFAILED));
    m_error.SetLeftMargin(btnHMargin);
    m_error.SetTopMargin(btnVMargin);
    m_error.SetVisible(false);

    m_retry.Initialize(ID_BOOKSTORE_RETRY, StringManager::GetPCSTRById(BOOKSTORE_RETRY),
#ifdef KINDLE_FOR_TOUCH 
        fontsize18);
#else
        KEY_RESERVED, 
        fontsize18, 
        ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
#endif
    m_retry.SetAlign(ALIGN_CENTER);
    m_retry.SetLeftMargin(btnHMargin);
    m_retry.SetTopMargin(btnVMargin);
    m_retry.SetVisible(false);

    m_btnNavigator.Initialize(ID_BOOKSTORE_STROLL, "",
#ifdef KINDLE_FOR_TOUCH 
        fontsize22);
#else
        KEY_RESERVED, 
        fontsize22, 
        ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
#endif
    m_btnNavigator.SetAlign(ALIGN_CENTER);
    m_btnNavigator.SetLeftMargin(btnHMargin);
    m_btnNavigator.SetTopMargin(btnVMargin);
    m_btnNavigator.SetVisible(false);

    m_loading.SetFontSize(fontsize22);
    m_loading.SetText(StringManager::GetPCSTRById(BOOKSTORE_LOADING_INFO));
    m_loading.SetVisible(false);

    m_noElem.SetFontSize(fontsize22);
    m_noElem.SetText(StringManager::GetPCSTRById(BOOKSTORE_FETCH_NOELEM));
    m_noElem.SetVisible(false);

    m_noElemCustomized.SetMinWidth(DeviceInfo::GetDisplayScreenWidth() - (GetWindowMetrics(UIHorizonMarginIndex) << 1));
    m_noElemCustomized.SetAlign(ALIGN_CENTER);
    m_noElemCustomized.SetFontSize(fontsize22);
    m_noElemCustomized.SetVisible(false);

    AppendChild(&m_error);
    AppendChild(&m_retry);
    AppendChild(&m_btnNavigator);
    AppendChild(&m_loading);
    AppendChild(&m_noElem);
    AppendChild(&m_noElemCustomized);

    if (!m_windowSizer)
    {
        UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
        m_fetchErrorSizer = new UIBoxSizer(dkVERTICAL);
        m_loadingSizer = new UIBoxSizer(dkVERTICAL);
        m_noElemSizer = new UIBoxSizer(dkVERTICAL);
        m_noElemCustomizedSizer = new UIBoxSizer(dkVERTICAL);
        if (mainSizer && m_fetchErrorSizer && m_loadingSizer && m_noElemSizer)
        {
            m_fetchErrorSizer->AddStretchSpacer();
            m_fetchErrorSizer->Add(&m_error, UISizerFlags().Align(dkALIGN_CENTER_HORIZONTAL));
            m_fetchErrorSizer->AddSpacer(m_retry.GetMinHeight());
            m_fetchErrorSizer->Add(&m_retry, UISizerFlags().Align(dkALIGN_CENTER_HORIZONTAL));
            m_fetchErrorSizer->AddStretchSpacer();

            m_loadingSizer->AddStretchSpacer();
            m_loadingSizer->Add(&m_loading, UISizerFlags().Align(dkALIGN_CENTER_HORIZONTAL));
            m_loadingSizer->AddStretchSpacer();

            m_noElemSizer->AddStretchSpacer();
            m_noElemSizer->Add(&m_noElem, UISizerFlags().Align(dkALIGN_CENTER_HORIZONTAL));
            m_noElemSizer->AddStretchSpacer();

            //m_noElemCustomizedSizer->AddStretchSpacer();
            m_noElemCustomizedSizer->AddSpacer(GetWindowMetrics(UIPixelValue30Index));
            m_noElemCustomizedSizer->Add(&m_noElemCustomized, UISizerFlags().Align(dkALIGN_CENTER_HORIZONTAL));
            m_noElemCustomizedSizer->AddSpacer(m_noElemCustomized.GetMinHeight());
            m_noElemCustomizedSizer->Add(&m_btnNavigator, UISizerFlags().Align(dkALIGN_CENTER_HORIZONTAL));

            mainSizer->Add(m_fetchErrorSizer, UISizerFlags(1).Expand());
            mainSizer->Add(m_noElemCustomizedSizer, UISizerFlags(1).Expand());
            mainSizer->Add(m_loadingSizer, UISizerFlags(1).Expand());
            mainSizer->Add(m_noElemSizer, UISizerFlags(1).Expand());
            SetSizer(mainSizer);

            const int width = DeviceInfo::GetDisplayScreenWidth();//(DeviceInfo::GetDisplayScreenWidth() >> 1);
            const int top = GetWindowMetrics(UIPixelValue30Index) 
#ifndef KINDLE_FOR_TOUCH
                + GetWindowMetrics(UIBookStoreNavigationTitleBottomLineHeightIndex)
#endif
                + GetWindowMetrics(UIPixelValue61Index)
                + GetWindowMetrics(UIBookStoreNavigationTitleBottomLineHeightIndex)
                + GetWindowMetrics(UITableControllerHeightIndex)
                + GetWindowMetrics(UIBookStorePageTableControllerBottomLinePixelIndex);
            const int height = DeviceInfo::GetDisplayScreenHeight() - top;
            const int left = 0;//(width >> 1);
            MoveWindow(left, top, width, height);
            Layout();
        }
        else
        {
            SafeDeletePointer(mainSizer);
            SafeDeletePointer(m_fetchErrorSizer);
            SafeDeletePointer(m_loadingSizer);
            SafeDeletePointer(m_noElemSizer);
        }
    }
}

HRESULT UIBookStoreFetchInfo::DrawBackGround(DK_IMAGE drawingImg)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    CTpGraphics grf(drawingImg);
    grf.EraserBackGround();
    return S_OK;
}

void UIBookStoreFetchInfo::SetFetchStatus(FETCH_STATUS status)
{
    bool visible = (status != FETCH_SUCCESS);
    SetVisible(visible);
    if (visible && (m_fetchStatus != status))
    {
        m_loadingSizer->Show(status == FETCHING);
        m_fetchErrorSizer->Show(status == FETCH_FAIL);

        if (!m_bCustomized)
            m_noElemSizer->Show(status == FETCH_NOELEM);
        else
        {
            m_noElemCustomizedSizer->Show(status == FETCH_NOELEM);
            if (!strcmp(m_noElemCustomized.GetText(), ""))
            {
                m_noElemCustomized.SetVisible(false);
            }
            if (!strcmp(m_btnNavigator.GetText(), ""))
            {
                m_btnNavigator.SetVisible(false);
            }
        }

#ifndef KINDLE_FOR_TOUCH
        if (status == FETCH_FAIL)
        {
            m_retry.SetFocus(true);
        }
#endif
        Layout();
    }
    m_fetchStatus = status;
}

void UIBookStoreFetchInfo::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    if (m_pParent)
    {
        m_pParent->OnCommand(dwCmdId, pSender, dwParam);
    }
}

void UIBookStoreFetchInfo::CustomizeNoElementInfo(const char* noElementNotice, const char* navigatorNotice)
{
    m_bCustomized = true;
    m_noElemCustomized.SetText(noElementNotice);
    m_btnNavigator.SetText(navigatorNotice);
}

