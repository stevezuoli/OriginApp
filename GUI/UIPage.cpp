////////////////////////////////////////////////////////////////////////
// UIPage.cpp
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "GUI/IPage.h"
#include "GUI/UIPage.h"
#include "Utility/ImageManager.h"
#include "GUI/FontManager.h"
#include "Framework/CDisplay.h"
#include "Utility/ColorManager.h"
#include "GUI/UIMenu.h"
#include "GUI/UIDialog.h"
#include "GUI/UIMessageBox.h"
#include "dkWinError.h"
#include "GUI/CTpGraphics.h"
#include "TouchAppUI/UIShortCutPage.h"
#include "CommonUI/UIIMEManager.h"

#define COLOR_BORDER_LINE 0x888888
#define COLOR_SOFTKEY_BACK_HIT 0xAAAAAA
#define COLOR_SOFTKEY_BACK 0xEAEAEA

#define SOFTKEY_RIGHT 2
#define SOFTKEY_LEFT 1
#define SOFTKEY_NONE 0

#define SOFTKEY_CODE_LEFT 1001
#define SOFTKEY_CODE_RIGHT 1002


UIPage::UIPage() 
    : UIContainer(NULL, IDSTATIC)
    , m_pDialog(NULL)
    , m_pCurrentMenu(NULL)
    , m_pMainMenu(NULL)
#ifdef KINDLE_FOR_TOUCH	 
    , m_pBottomBar(NULL) 	
#endif
    , m_iScreenOrientation(0)
    , m_bIsScrollable(FALSE)
    , m_bIsDisposed(FALSE)
{
    Init();
}

UIPage::UIPage(const DWORD dwId, BOOL bIsScrollable) 
    : UIContainer(NULL, dwId)
    , m_pDialog(NULL)
    , m_pCurrentMenu(NULL)
    , m_pMainMenu(NULL)
#ifdef KINDLE_FOR_TOUCH	 
    , m_pBottomBar(NULL) 	
#endif
    , m_iScreenOrientation(0)
    , m_bIsScrollable(FALSE)
    , m_bIsDisposed(FALSE)
{
    Init();
}

UIPage::UIPage(const DWORD dwId)
    :UIContainer(NULL, dwId)
    , m_pDialog(NULL)
    , m_pCurrentMenu(NULL)
    , m_pMainMenu(NULL)
#ifdef KINDLE_FOR_TOUCH	 
    , m_pBottomBar(NULL) 	
#endif
    , m_iScreenOrientation(0)
    , m_bIsScrollable(FALSE)
    , m_bIsDisposed(FALSE)
{
    Init();
}

void UIPage::Init()
{
    m_bIsDisposed = FALSE;
    m_pCurrentMenu = NULL;
    m_pMainMenu = NULL;
#ifdef KINDLE_FOR_TOUCH	 
    m_pBottomBar = UIBottomBar::GetInstance(); 	
#endif
    m_pDialog = NULL;
    m_iScreenOrientation = SCREEN_NORMAL;
    MoveWindow(0, 0, GetScreenWidth(), GetScreenHeight());
}

UIPage::~UIPage()
{
    Dispose();
}

void UIPage::Dispose()
{
    UIContainer::Dispose();
    OnDispose(m_bIsDisposed);
}

BOOL UIPage::OnKeyPress(INT32 iKeyCode)
{
    DebugPrintf(DLC_MESSAGE, "%s::UIPage::OnKeyPress(%d)", GetClassName(), iKeyCode);
#ifdef KINDLE_FOR_TOUCH
    return UIContainer::OnKeyPress(iKeyCode);
#else
    // Send key messages to CurrentMenu and Dialog first.
    if (m_pCurrentMenu && (m_pCurrentMenu->IsVisible()))
    {
        if(iKeyCode == KEY_MENU && m_pMainMenu != m_pCurrentMenu)
        {
            m_pCurrentMenu->Show(FALSE);
            m_pCurrentMenu=NULL;
        }
        else if (!m_pCurrentMenu->OnKeyPress(iKeyCode))
        {
            return FALSE;
        }
    }

    if (m_pDialog)
    {
        return m_pDialog->OnKeyPress(iKeyCode);
    }

    if (UIContainer::OnKeyPress(iKeyCode))
    {
        DebugPrintf(DLC_WANGYJ, "m_pMainMenu=0x%08X, m_pCurrentMenu=0x%08X", m_pMainMenu, m_pCurrentMenu);
        switch (iKeyCode)
        {
        case KEY_MENU:
            if (m_pCurrentMenu && (m_pCurrentMenu->IsVisible()))
            {
                m_pCurrentMenu->Show(FALSE);
                m_pCurrentMenu=NULL;
            }
            else
            {
                if (m_pMainMenu)
                {
                    m_pCurrentMenu = m_pMainMenu;
                    m_pCurrentMenu->SetUIPage(this);

                    m_pCurrentMenu->Show(TRUE);
                }
                else
                {
                    return TRUE;
                }
            }
            break;
        default:
            return TRUE;
            break;
        }
    }

    return FALSE;
#endif
}

BOOL UIPage::OnKeyRelease(INT32 iKeyCode)
{
    if (m_pCurrentMenu)
    {
        return m_pCurrentMenu->OnKeyRelease(iKeyCode);
    }
    else if (m_pDialog)
    {
        return m_pDialog->OnKeyRelease(iKeyCode);
    }

    return UIContainer::OnKeyRelease(iKeyCode);
}

BOOL UIPage::OnPointerPressed(INT32 iX, INT32 iY)
{
    UNUSED(iX);
    UNUSED(iY);
    // Note, it is not supported in this milestone
    return TRUE;
}

BOOL UIPage::OnPointerDragged(INT32 iX, INT32 iY)
{
    UNUSED(iX);
    UNUSED(iY);
    // Note, it is not supported in this milestone
    return TRUE;
}

BOOL UIPage::OnPointerReleased(INT32 iX, INT32 iY)
{
    UNUSED(iX);
    UNUSED(iY);
    // Note, it is not supported in this milestone
    return TRUE;
}

INT32 UIPage::GetClientWidth()
{
    return UIContainer::GetClientWidth();
}

INT32 UIPage::GetClientHeight()
{
    return UIContainer::GetClientHeight();
}

void UIPage::OnChildSetFocus(UIWindow* pWindow)
{
    UIContainer::OnChildSetFocus(pWindow);
}

void UIPage::PopupDialog(UIDialog* pDialog)
{
    TP_ASSERT(pDialog != NULL);

    m_pDialog = pDialog;

    // m_pDialog.setParent(this);

    m_pDialog->SetUIPage(this);

    m_pDialog->Show(TRUE);
}

UIDialog* UIPage::GetDialog()
{
    return m_pDialog;
}

void UIPage::SetScreenOrientation(INT32 iScreenOrientation)
{
    if (iScreenOrientation != m_iScreenOrientation)
    {
        m_iScreenOrientation = iScreenOrientation;
        Repaint();
    }
}

void UIPage::DestroyMenu()
{
    m_pCurrentMenu = NULL;
    Repaint();
}

BOOL UIPage::DestroyDialog(BOOL fRepaintIt)
{
    if (NULL == m_pDialog)
    {
        return FALSE;
    }
    if (!m_pDialog->DestroyDialog(fRepaintIt))
    {
        // Note, the dialog instance is not deleted as it is passed to this Page
        m_pDialog->SetVisible(FALSE);
        m_pDialog = NULL;
        if(fRepaintIt)
        {
            Repaint();
        }
    }
    return TRUE;
}

void UIPage::SetLayout(UILayout* pLayout)
{
    UIContainer::SetLayout(pLayout);
}

void UIPage::Repaint()
{
    if (IsVisible())
    {
        CDisplay::GetDisplay()->Repaint();
    }
}

UIPage* UIPage::GetUIPage()
{
    return this;
}


UITitleBar *UIPage::GetTitleBar()
{
    return &m_titleBar;
}

INT32 UIPage::GetScreenHeight()
{
    switch (m_iScreenOrientation)
    {
    case SCREEN_LEFT_ROTATE:
    case SCREEN_RIGHT_ROTATE:
        return CDisplay::GetDisplay()->GetScreenWidth();
    default:
        break;
    }

    return CDisplay::GetDisplay()->GetScreenHeight();
}

INT32 UIPage::GetScreenWidth()
{
    switch (m_iScreenOrientation)
    {
    case SCREEN_LEFT_ROTATE:
    case SCREEN_RIGHT_ROTATE:
        return CDisplay::GetDisplay()->GetScreenHeight();
    default:
        break;
    }

    return CDisplay::GetDisplay()->GetScreenWidth();
}

void UIPage::MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight)
{
    UIContainer::MoveWindow(iLeft, iTop, iWidth, iHeight);
    if (m_pDialog)
    {
        m_pDialog->OnPageSizeChanged();
    }
}

void UIPage::OnLoad()
{
}

void UIPage::OnUnLoad()
{
}
    


HRESULT UIPage::DrawBackGround(DK_IMAGE drawingImg)
{
    CTpGraphics grf(drawingImg);
    HRESULT hr(S_OK);
    RTN_HR_IF_FAILED(grf.EraserBackGround());

    return hr;
}


HRESULT UIPage::Draw(DK_IMAGE drawingImg)
{
    DebugPrintf(DLC_XU_KAI," UIPage::Draw(DK_IMAGE drawingImg) begin for page: %s", this->GetClassName());
    if (!m_bIsVisible)
    {
        DebugPrintf(DLC_XU_KAI," UIPage::Draw(DK_IMAGE drawingImg) !m_bIsVisible");
        return S_FALSE;
    }

    if (drawingImg.pbData == NULL)// || !m_IsChanged)
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

    if (m_pCurrentMenu == NULL || !m_pCurrentMenu->IsCached() || (m_pCurrentMenu && !(m_pCurrentMenu->IsVisible())))
    {
        RTN_HR_IF_FAILED(UIContainer::Draw(drawingImg));
    }

    if (m_pCurrentMenu  && m_pCurrentMenu->IsVisible())
    {
        RTN_HR_IF_FAILED(m_pCurrentMenu->Draw(imgSelf));
    }
    // TODO(JUGH):?¡è?¨¨¨¦¨¨??¨º?¨¨?¡¤¡§??¨º?¡Á¡ä¨¬?¦Ì??¨¦??¡ê?¨¦¨¨????¨º?¡Á¡ä¨¬?¨º¡À¦Ì?TopFullContainero¨ª¦Ì¡À?¡ã¦Ì?2?¨º?¨°??¨´¦Ì?
    UIIME* pIME = UIIMEManager::GetCurrentIME();
    if (pIME && pIME->IsShowDelay())
    {
        pIME->ShowIME();
    }

    DebugPrintf(DLC_XU_KAI," UIPage::Draw(DK_IMAGE drawingImg) end");
    return hr;
}

void UIPage::PopupMenu(UIMenu* pMenu, INT32 iX, INT32 iY)
{
    DebugPrintf(DLC_WANGYJ, "%sUIPage::PopupMenu(iX=%d, iY=%d)", GetClassName(), iX, iY);

    if (pMenu)
    {
        m_pCurrentMenu = pMenu;
        pMenu->SetUIPage(this);
        pMenu->PopupMenu(iX, iY, GetScreenWidth(), GetScreenHeight());

        Repaint();
    }
}

UIMenu* UIPage::GetMenu()
{
    return m_pCurrentMenu;
}


void UIPage::OnResize(INT32 iWidth, INT32 iHeight)
{
    UIContainer::MoveWindow(m_iLeft, m_iTop, iWidth, iHeight);
}

void UIPage::OnNotify(UIEvent rEvent)
{
    if (rEvent.GetEventId()== MENU_CLICK_EVENT)
    {
        if (m_pEventHook != NULL)
        {
            m_pEventHook->HandleEvent(rEvent);
        }
    }

    UIContainer::OnNotify(rEvent);
}

void UIPage::Broadcast(UIEvent rEvent)
{
    UIContainer::OnBroadcast(rEvent);
}

void UIPage::OnDispose(BOOL bDisposed)
{
    if (bDisposed)
    {
        return;
    }

    m_bIsDisposed = TRUE;

    if (m_pDialog != NULL)
    {
        // Note, this dialog is not deleted here as it is passed in
        m_pDialog = NULL;
    }

    if (m_pCurrentMenu != NULL)
    {
        m_pCurrentMenu = NULL;
    }

#ifdef KINDLE_FOR_TOUCH	
    if(m_pBottomBar != NULL)
    {
        m_pBottomBar = NULL;
    } 	
#endif
}

HRESULT UIPage::DrawFocus(DK_IMAGE drawingImg)
{
    //    return E_NOTIMPL;
    return S_OK;
}


