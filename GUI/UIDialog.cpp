////////////////////////////////////////////////////////////////////////
// UIDialog.cpp
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#include "GUI/UIDialog.h"
#include "Framework/CMessageHandler.h"
#include "Framework/CDisplay.h"
#include "Framework/CNativeThread.h"
#include "GUI/CTpGraphics.h"
#include "Utility/ColorManager.h"
#include "drivers/Usb.h"
#include "TTS/TTS.h"
#include "CommonUI/UIIMEManager.h"
#include "GUI/GlobalEventListener.h"
#include "GUI/UIAbstractTextEdit.h"
#include "CommonUI/CPageNavigator.h"

//UIScrollContainer(NULL, IDSTATIC)
UIDialog::UIDialog() :
    UIContainer(NULL, IDSTATIC)
    , m_fIsEndDialog(FALSE)
    , m_bIsDisposed(FALSE)
    , m_iEndCode(0)
    , m_pDialog(NULL)
    , m_pParentDialog(NULL)
    , m_pPage(NULL)
{
    SetPropogateTouch(false);
    Init();
}


//UIScrollContainer(NULL, IDSTATIC)
UIDialog::UIDialog(UIContainer* pParent) :
      UIContainer(NULL, IDSTATIC)
    , m_fIsEndDialog(FALSE)
    , m_bIsDisposed(FALSE)
    , m_iEndCode(0)
    , m_pDialog(NULL)
    , m_pParentDialog(NULL)
    , m_pPage(NULL)
{
    // Note, pParent is used in this way to prevent to add this dialog as a child of the parent window
    // the parent is a fake parent

    SetPropogateTouch(false);
    Init();
}

UIDialog::~UIDialog()
{
    Dispose();
}

void UIDialog::Dispose()
{
    //UIScrollContainer::Dispose();
    OnDispose(m_bIsDisposed);
}

void UIDialog::OnDispose(BOOL bIsDisposed)
{
    if (bIsDisposed)
    {
        return;
    }

    m_bIsDisposed = TRUE;
    if (NULL != m_pDialog)
    {
        m_pDialog = NULL;
    }
    if (NULL != m_pPage)
    {
        m_pPage = NULL;
    }
}

void UIDialog::Init()
{
    m_bIsDisposed = FALSE;
    m_iEndCode = 0;
    m_pDialog = NULL;
    m_pParentDialog = NULL;
    m_pPage = NULL;
    m_bIsVisible = FALSE;
    m_fIsEndDialog = FALSE;
    SetZPosition(ZPOSITION_DIALOG);
}


BOOL UIDialog::OnKeyPress(INT32 iKeyCode)
{
    if (NULL != m_pDialog)
    {
        return m_pDialog->OnKeyPress(iKeyCode);
    }
#ifdef KINDLE_FOR_TOUCH
    EndDialog(0);
    return UIWindow::OnKeyPress(iKeyCode);
#else
    if (!SendKeyToChildren(iKeyCode))
    {
        return FALSE;
    }

    switch (iKeyCode)
    {
    case KEY_LEFT:
    case KEY_RIGHT:
        if (UIContainer::OnKeyPress(iKeyCode))
        {
            SetAppropriateFocus(TRUE, iKeyCode == KEY_LEFT);
        }
        break;
    case KEY_HOME:
        {
            // 按home键时结束当前对话框，并再发一个home键给父窗口
            EndDialog(IDCANCEL);
            CNativeThread::GetInstance()->SendKeyMessage(102, 1);
        }
        break;
    default:
        return UIContainer::OnKeyPress(iKeyCode);
    }

    return FALSE;
#endif
}

BOOL UIDialog::OnPointerPressed(INT32 iX, INT32 iY)
{
    UNUSED(iX);
    UNUSED(iY);
    // this is not implemented in this milestone
    return TRUE;
}

BOOL UIDialog::OnPointerDragged(INT32 iX, INT32 iY)
{
    UNUSED(iX);
    UNUSED(iY);
    // this is not implemented in this milestone
    return TRUE;
}

BOOL UIDialog::OnPointerReleased(INT32 iX, INT32 iY)
{
    UNUSED(iX);
    UNUSED(iY);
    // this is not implemented in this milestone
    return TRUE;
}

#if 0
INT32 UIDialog::ClientToScreenY(INT32 iY)
{
    return m_iTop + iY;
}

INT32 UIDialog::GetAbsoluteX()
{
    return m_iLeft;
}

INT32 UIDialog::GetAbsoluteY()
{
    return m_iTop;
}
#endif

void UIDialog::PopupDialog(UIDialog* pDialog)
{
    TP_ASSERT(pDialog != NULL);

    m_pDialog = pDialog;
    m_pDialog->SetUIDialog(this);
    m_pDialog->Show(TRUE);
}
void UIDialog::SetUIDialog(UIDialog *pDialog)
{
    m_pParentDialog = pDialog;
}
BOOL UIDialog::DestroyDialog(BOOL fRepaintIt)
{
    if (NULL == m_pDialog)
    {
        return FALSE;
    }

    if (!m_pDialog->DestroyDialog(fRepaintIt))
    {
        // Note, the dialog instance is passed from other object so no delete operation here
        m_pDialog = NULL;
        if(fRepaintIt)
        {
            Repaint();
        }
    }

    return TRUE;
}

INT32 UIDialog::DoModal()
{
    // ///////////////////////////////////////////////////////////
    // for sure that the waiting state don't appear
    // so invoke endWaitingUI at the begining of doModal
    // invoke beginWaitingUI at the end of doModal
    Popup();

    //DebugPrintf(DLC_DIAGNOSTIC, "%s begin do modal(%s)", GetClassName(), GetText());
    while (!m_fIsEndDialog)
    {
        SNativeMessage msg;
        CMessageHandler::GetMessage(msg);

        switch (msg.iType)
        {
        case KMessageScreenSaverIn:
        case KMessageUsbSingle:
            {
                EndDialog(-1, FALSE);
                // 这里必须用Send,不能直接CMessageHandler::DispatchMessage
                // 否则可能析构该对话框导致后续崩溃
                CNativeThread::Send(msg);
            }            
            break;
        case KMessageWebBrowserStart:
        case KMessageWebBrowserStop:
            {
                EndDialog(-1, FALSE);
                // 这里必须用Send,不能直接CMessageHandler::DispatchMessage
                // 否则可能析构该对话框导致后续崩溃
                CNativeThread::Send(msg);
            }            
            break;
        case KMessageWifiChange:
            {
                DebugPrintf(DLC_ERROR, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
                OnWifiMessage(msg);
            }
            break;
        case KMessageKeyboard:
            DebugPrintf(DLC_INPUT, "UIDialog::DoModal KeyId(code=%d, type=%d, value=%d)\n", msg.iParam1, msg.iType, msg.iParam2);
            {
                CUsb *lpUsb = CUsb::GetInstance();
                if(lpUsb->IsUsbConnected() != 1)
                {
                    //TRACE(CString(TEXT("Begin Handle message : KMessageKeyboard")));
                    INT32 keycode = (INT32)msg.iParam1;
                    EKeyboardEventType type = (EKeyboardEventType)msg.iParam2;
                    switch (type)
                    {
                    case DK_KEYTYPE_REPEAT:
                    case DK_KEYTYPE_DOWN:
                        //在其他Dialog(非AudioDialog)中需要响应音量按键消息
                        if(keycode == KEY_VOLUMEUP || keycode == KEY_VOLUMEDOWN)
                        {
                        }

                        if(KEY_HOME == keycode)
                        {
                            if (!CNativeThread::GetKeyHomeDealedAlready())
                            {
                                this->EndDialog(IDCANCEL);
                                if(NULL != m_pParentDialog) //If it has parent dialog, end it too since "Home" key is big wit!!!
                                {
                                    m_pParentDialog->EndDialog(IDCANCEL);
                                }
                            }
                            else
                            {
                                CMessageHandler::DispatchMessage(msg);
                            }
                        }
                        else if(KEY_BACK == keycode)
                        {
                            EndDialog(IDCANCEL);
                        }

                        break;
                    case DK_KEYTYPE_UP:
//                        OnKeyRelease(keycode);
                        break;
                    //case DK_KEYTYPE_REPEAT:
                        // not used in current implementation
                        //break;
                    }
                }
            }
            break;
        case KMessageTTSChange:
            {
                if (TTS_MESSAGE_NEXTPAGE_AUTO == msg.iParam2)
                    this->EndDialog(0);
                CMessageHandler::DispatchMessage(msg);
            }
            break;
        case KMessageMoveEvent:
            {
                MoveEvent* moveEvent = (MoveEvent*)msg.iParam1;
                DebugPrintf(DLC_GESTURE, "MoveEvent %d, downTime: %d, eventTime: %d",
                    moveEvent->GetActionMasked(),
                    (int)moveEvent->GetDownTime(),
                    (int)moveEvent->GetEventTime());
                if (CNativeThread::GetKeyHomePressStatus())
                {
                    DebugPrintf(DLC_GESTURE, "Home key pressed");
                    if (moveEvent->GetActionMasked() == MoveEvent::ACTION_DOWN)
                    {
                        struct timeval sTouchStartTime;
                        struct timeval sKeyHomePressedTime = CNativeThread::GetKeyHomePressedTime();
                        gettimeofday(&sTouchStartTime, NULL);
                        long intervalTimes = (sTouchStartTime.tv_sec - sKeyHomePressedTime.tv_sec)*1e6 +
                            (sTouchStartTime.tv_usec - sKeyHomePressedTime.tv_usec);
                        if (intervalTimes >= 1e6)//1-2s
                        {
                            CDisplay::GetDisplay()->PrintSrceen();
                            CNativeThread::SetScreenShotted(true);
                        }
                    }
                    CNativeThread::SetKeyHomeDealedAlready(true);
                }
                else
                {
                    if (!GlobalEventListener::GetInstance()->HookTouch(moveEvent))
                    {
                        int x, y;
                        x = moveEvent->GetX();
                        y = moveEvent->GetY();
                        UIPage* currentPage = CPageNavigator::GetCurrentPage();
                        DebugPrintf(DLC_GESTURE,
                                "current page: %s",
                                currentPage ? currentPage->GetClassName() : "NULL");
                        UIIME* pIME = UIIMEManager::GetCurrentIME();
                        bool shouldForward = false;
                        if ((pIME && pIME->IsShow() && pIME->PointInWindowAbsolute(x, y)) 
                                // 如果不是当前page的子窗口,则不在最顶，不可见，应当转发消息
                                // 或者是当前page子窗口，且点击在对话框中，也应转发
                                || (!IsChildOf(currentPage) || PointInWindowAbsolute(x, y)))
                        {
                            shouldForward = true;
                        }
                        DebugPrintf(DLC_GESTURE, "Should forward: %d",shouldForward);
                        if (NULL != currentPage)
                        {
                            if (shouldForward)
                            {
                                currentPage->DispatchTouchEvent(moveEvent);
                            }
                            else
                            {
                                moveEvent->SetAction(MoveEvent::ACTION_CANCEL);
                                currentPage->DispatchTouchEvent(moveEvent);
                            }
                        }
                    }
                    else
                    {
                        //DebugPrintf(DLC_DIAGNOSTIC, "Gesture was hooked");
                        UIPage* currentPage = CPageNavigator::GetCurrentPage();
                        if (NULL != currentPage)
                        {
                            moveEvent->SetAction(MoveEvent::ACTION_CANCEL);
                            currentPage->DispatchTouchEvent(moveEvent);
                        }
                    }
                }
                delete moveEvent;
            }

            break;
        default:
            CMessageHandler::DispatchMessage(msg);
            break;
        }

        /*if (msg.iType)
        {
        }
        else
        {
        CMessageHandler::DispatchMessage(msg);
        }*/
        /*        CMessageHandler::EHandlerReturnType ret = CMessageHandler::HandleMessage();

        if (CMessageHandler::END_DIALOG == ret)
        {
        break;
        }*/
    }
    // ///////////////////////////////////////////////////////////

    //DebugPrintf(DLC_DIAGNOSTIC, "%s end do modal", GetClassName(), GetText());
    return m_iEndCode;
}

HRESULT UIDialog::DrawBackGround(DK_IMAGE drawingImg)
{
    CTpGraphics grp(drawingImg);
    HRESULT hr(S_OK);
    RTN_HR_IF_FAILED(grp.FillRect(0, 0, m_iWidth, m_iHeight, ColorManager::knWhite));
    return hr;
}

HRESULT UIDialog::Draw(DK_IMAGE drawingImg)
{
    DebugPrintf(DLC_LIUJT, "%s::UIDialog::Draw, entering, m_bIsVisible = %d", GetClassName(), m_bIsVisible);
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

    CTpGraphics grp(imgSelf);
    RTN_HR_IF_FAILED(UIContainer::Draw(drawingImg));
    // TODO(JUGH):
    UIIME* pIME = UIIMEManager::GetCurrentIME();
    if (pIME && pIME->IsShowDelay())
    {
        pIME->ShowIME();
    }

    return hr;
}

void UIDialog::Popup()
{
    m_fIsEndDialog = FALSE;
    AppendChildToTopFullScreenContainer();
    if (m_pParent)
    {
        m_pParent->PopupDialog(this);
    }
}

void UIDialog::EndDialog(INT32 iEndCode,BOOL fRepaintIt)
{
    UIIME* pIME = UIIMEManager::GetCurrentIME();
    if (pIME && pIME->IsShow())
    {
        pIME->HideIME();
    }
    if (m_pParent)
    {
        m_pParent->RemoveChild(this, false);
        m_pParent = NULL;
    }
    m_iEndCode = iEndCode;
    if(m_pPage)
    {
        m_pPage->DestroyDialog(fRepaintIt);
    }
    if(m_pParentDialog)
    {
        m_pParentDialog->DestroyDialog(fRepaintIt);
    }
    m_fIsEndDialog = TRUE;
    UIEvent showEvent(HIDE_EVENT);
    OnEvent(showEvent);
    SetVisible(false);
}

BOOL UIDialog::IsEndDialog()
{
    return m_fIsEndDialog;
}

void UIDialog::OnChildSetFocus(UIWindow* pWindow)
{
    INT32 iIndex = GetChildIndex(pWindow);

    if (iIndex == -1 || m_iFocusedChild == iIndex)
    {
        return;
    }

    ClearFocus();

    m_iFocusedChild = iIndex;
    m_bIsFocus = TRUE;
}

void UIDialog::SetUIPage(UIPage* pPage)
{
    m_pPage = pPage;
}

void UIDialog::OnPageSizeChanged()
{
    // empty
}

bool UIDialog::OnChildClick(UIWindow* child)
{
    return false;
}
