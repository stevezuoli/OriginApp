#include "CommonUI/UIUtility.h"
#include "Wifi/WifiManager.h"
#include "Framework/CDisplay.h"
#include "I18n/StringManager.h"
#include "Common/CAccountManager.h"
#ifdef KINDLE_FOR_TOUCH
#include "TouchAppUI/UIWifiDialog.h"
#else
#include "AppUI/UIWifiDialog.h"
#endif
#include "CommonUI/UIForgetPasswordDlg.h"
#include "CommonUI/UIAccountLoginDialog.h"
#include "Common/WindowsMetrics.h"
#include "GUI/GUISystem.h"
#include "Utility.h"
#include "time.h"

using namespace dk::account;
using namespace WindowsMetrics;

void UIUtility::ShowMessageBox(string text)
{
    usleep(500000);
    UIMessageBox* msgbox = CreateMessageBox();

    if(NULL != msgbox)
    {
        msgbox->SetText(text.c_str());
        msgbox->DoModal();
        delete msgbox;
        msgbox = NULL;
    }
}

void UIUtility::ShowAccountLoginDlg()
{
    static bool displaying = false;
    if (displaying)
    {
        // the dialog is displaying, abandon this request
        return;
    }
    displaying = true;
    UIAccountLoginDialog loginDialog(
            GUISystem::GetInstance()->GetTopFullScreenContainer(),
            StringManager::GetPCSTRById(TOUCH_SYSTEMSETTING_LOGIN_ACCOUNT));
    loginDialog.DoModal();
    if (loginDialog.ForgettingPassword())
    {
        UIForgetPasswordDlg forgetPWDDlog(
                GUISystem::GetInstance()->GetTopFullScreenContainer()
                , loginDialog.GetUserAccount().c_str());
        forgetPWDDlog.DoModal();
    }
    displaying = false;
}

bool UIUtility::CheckAndReLogin(RELOGIN_TYPE loginType)
{
    static bool running = false;
    if (running)
    {
        return false;
    }

    running = true;
    if(!UIUtility::CheckAndReConnectWifi())
    {
        running = false;
        return false;
    }
    
    CAccountManager* accountManager = CAccountManager::GetInstance();
    if (accountManager->IsLoggedIn())
    {
        running = false;
        return true;
    }
    if (((RELOGIN_WITH_TOKEN_ASYNC == loginType) && accountManager->LoginWithTokenAsync())
        || ((RELOGIN_WITH_TOKEN_SYNC == loginType) && accountManager->LoginWithToken()))
    {
        running = false;
        return true;
    }
    running = false;
    return accountManager->IsLoggedIn();
}

bool UIUtility::CheckAndReLoginSync()
{
    if(!UIUtility::CheckAndReConnectWifi())
    {
        return false;
    }

    CAccountManager* accountManager = CAccountManager::GetInstance();
    if(NULL == accountManager)
    {
        return false;
    }

    if (accountManager->IsLoggedIn() || accountManager->LoginWithToken())
    {
        return true;
    }
    return accountManager->IsLoggedIn();
}


bool UIUtility::CheckAndReConnectWifi(UIContainer* parent)
{
    bool bWifiConnected = WifiManager::GetInstance()->IsConnected();
    if(!bWifiConnected)
    {
        UIMessageBox* messagebox = CreateMessageBox(MB_OK | MB_CANCEL, parent);
        if(!messagebox)
        {
            return false;
        }
        messagebox->SetText(StringManager::GetStringById(NET_FUNCTION_NEED_NET));
#ifndef KINDLE_FOR_TOUCH
        messagebox->SetFocusButton(MB_OK);
#endif
        int iRet = messagebox->DoModal();
        delete messagebox;
        messagebox = NULL;
        if(MB_OK == iRet)
        {
            WifiManager* wifiBase = WifiManager::GetInstance();
            if(FALSE == wifiBase->IsPowerOn())
            {
               wifiBase->PowerOn();
            }
            
            UIContainer *pDisplay = GetCurDisplay();
            if(pDisplay)
            {
#ifdef KINDLE_FOR_TOUCH
                UIWifiDialog wifiDlg(pDisplay, true);
                wifiDlg.MoveWindow(0, 0, pDisplay->GetScreenWidth(), pDisplay->GetScreenHeight());
#else
                UIWifiDialog wifiDlg(pDisplay);
#endif

                wifiDlg.DoModal();

                //Retry wifi status again;
                bWifiConnected = WifiManager::GetInstance()->IsConnected();
            }

        }
    }
    return bWifiConnected;
}

UIContainer* UIUtility::GetCurDisplay()
{
    CDisplay *display = CDisplay::GetDisplay();
    if(NULL == display)
    {
        DebugPrintf(DLC_LIUJT, "UIUtility::GetCurDisplay display==NULL");
        return NULL;
    }
    UIPage* current = display->GetCurrentPage();
    
    if(NULL == current)
    {
        DebugPrintf(DLC_LIUJT, "UIUtility::GetCurDisplay current==NULL");
        return NULL;
    }
    UIDialog* dialog = current->GetDialog();

    if(dialog)
    {
        DebugPrintf(DLC_LIUJT, "UIUtility::GetCurDisplay current==dialog: %s", (NULL == dialog->GetClassName()) ? "is null" : dialog->GetClassName());
        return dialog;
    }
    else
    {
        DebugPrintf(DLC_LIUJT, "UIUtility::GetCurDisplay current==UIPage: %s", (NULL == current->GetClassName()) ? "is null" : current->GetClassName());
        return current;
    }

}

UIMessageBox* UIUtility::CreateMessageBox(INT32 iType, UIContainer* parent)
{
    DebugPrintf(DLC_LIUJT, "UIUtility::CreateMessageBox Entering with type=%d", iType);
    
    UIContainer* display = parent;
    if(!display)
    {
        display = GUISystem::GetInstance()->GetTopFullScreenContainer();
    }

    if(!display)
    {
        return NULL;
    }
    
    return new UIMessageBox(display,StringManager::GetStringById(ACTION_OK), iType);
}

UITextSingleLine* UIUtility::CreateTip(CString tipText, UIContainer* parent)
{
    UIContainer* display = parent;
    if(!display)
    {
        display = GetCurDisplay();
    }
    if(!display)
    {
        return  NULL;
    }
    
    UITextSingleLine* textLine = new UITextSingleLine(display,IDSTATIC);

    if(NULL != textLine)
    {
        textLine->SetBackColor(ColorManager::knWhite);
        textLine->SetBackTransparent(FALSE);
        textLine->SetFontSize(31);
    
        textLine->SetText(tipText);

        int width = textLine->GetTextWidth();
        int height = textLine->GetTextHeight();
        if (width > 0)
        {
            //textLine->MoveWindow(display->GetScreenWidth()/2-60, display->GetScreenHeight()/2-50, stringWidth, 30);              
            textLine->MakeCenter(width, height);
        }
    }

    return textLine;
}

void UIUtility::SetScreenTips(const char* tips, unsigned int msec)
{
    CDisplay::GetDisplay()->SetScreenTips(tips, msec);
}
void UIUtility::ShowTip(UITextSingleLine* uiText, bool showTip)
{
    DebugPrintf(DLC_LIUJT, "UIUtility::ShowTip Entering with show = %d", showTip);

    if(NULL == uiText)
    {
        return;
    }

    uiText->Show(showTip);
    uiText->UpdateWindow();
    usleep(500000);
    DebugPrintf(DLC_LIUJT, "UIUtility::ShowTip End");    
}

HRESULT UIUtility::DrawBorder(DK_IMAGE image, int leftMargin, int topMargin, int rightMargin, int bottomMargin, int titleHeight, bool isBtn, bool isRightAngle)
{
    HRESULT hr(S_OK);
    CTpGraphics grf(image);

    const int width = image.iWidth;
    const int height = image.iHeight;
    const int left = leftMargin;
    const int top = topMargin;
    const int right = width - rightMargin;
    const int bottom = height - bottomMargin;

    const int borderLine = GetWindowMetrics(isBtn ? UIBtnBorderLineIndex : UIDialogBorderLineIndex);
    RTN_HR_IF_FAILED(grf.FillRect(left, top, right, bottom, ColorManager::knBlack));
    
    int titleBottom = top + borderLine;
    if (titleHeight > 0)
    {
        titleBottom += titleHeight;
        RTN_HR_IF_FAILED(grf.FillRect(left + borderLine, top + borderLine
            , right - borderLine, titleBottom, ColorManager::GetColor(COLOR_TITLE_BACKGROUND)));
    }
    RTN_HR_IF_FAILED(grf.FillRect(left + borderLine, titleBottom
        , right - borderLine, bottom - borderLine, ColorManager::knWhite));

    if (!isRightAngle)
    {
        SPtr<ITpImage> pUpLeftImage, pUpRightImage, pLeftDownImage, pRightDownImage;
        if (isBtn)
        {
#ifdef KINDLE_FOR_TOUCH
            pUpLeftImage = ImageManager::GetImage(IMAGE_TOUCH_ICON_BTN_UPLEFT);
            pUpRightImage = ImageManager::GetImage(IMAGE_TOUCH_ICON_BTN_UPRIGHT);
            pLeftDownImage = ImageManager::GetImage(IMAGE_TOUCH_ICON_BTN_LEFTDOWN);
            pRightDownImage = ImageManager::GetImage(IMAGE_TOUCH_ICON_BTN_RIGHTDOWN);
#else
            pUpLeftImage = ImageManager::GetImage(IMAGE_IME_ICON_R4);
            pUpRightImage = ImageManager::GetImage(IMAGE_IME_ICON_R1);
            pLeftDownImage = ImageManager::GetImage(IMAGE_IME_ICON_R3);
            pRightDownImage = ImageManager::GetImage(IMAGE_IME_ICON_R2);
#endif
        }
        else
        {
            if (titleHeight > 0)
            {
#ifdef KINDLE_FOR_TOUCH
                pUpLeftImage = ImageManager::GetImage(IMAGE_TOUCH_MENU_GRAYBG_01);
                pUpRightImage = ImageManager::GetImage(IMAGE_TOUCH_MENU_GRAYBG_02);
#else
                pUpLeftImage = ImageManager::GetImage(IMAGE_MENU_GRAYBG_01);
                pUpRightImage = ImageManager::GetImage(IMAGE_MENU_GRAYBG_02);
#endif
            }
            else
            {
                pUpLeftImage = ImageManager::GetImage(IMAGE_IME_ICON_R4);
                pUpRightImage = ImageManager::GetImage(IMAGE_IME_ICON_R1);
            }
            pLeftDownImage = ImageManager::GetImage(IMAGE_IME_ICON_R3);
            pRightDownImage = ImageManager::GetImage(IMAGE_IME_ICON_R2);
        }

        if ((NULL == pUpLeftImage.Get()) || (NULL == pUpRightImage.Get()) || (NULL == pLeftDownImage.Get()) || (NULL == pRightDownImage.Get()))
        {
            return S_FALSE;
        }

        RTN_HR_IF_FAILED(grf.DrawImage(pUpLeftImage.Get(), left, top));
        RTN_HR_IF_FAILED(grf.DrawImage(pUpRightImage.Get(), right - pUpRightImage->GetWidth(), top));
        RTN_HR_IF_FAILED(grf.DrawImage(pLeftDownImage.Get(), left, bottom - pLeftDownImage->GetHeight()));
        RTN_HR_IF_FAILED(grf.DrawImage(pRightDownImage.Get(), right - pRightDownImage->GetWidth(), bottom - pRightDownImage->GetHeight()));
    }
    return hr;
}

std::string UIUtility::TransferIntToString(int time)
{
    time_t tm = (time_t)time;
    return DkFormat::CUtility::TransferTimeToString(tm);
}

bool UIUtility::LeftContainsRight(const DK_RECT& rectLeft, const DK_RECT& rectRight)
{
    return !CUtility::IsEmptyRect(rectLeft)
        //&& !CUtility::IsEmptyRect(rectRight)
        && (rectLeft.left <= rectRight.left)
        && (rectLeft.top <= rectRight.top)
        && (rectLeft.right >= rectRight.right)
        && (rectLeft.bottom >= rectRight.bottom);
}

bool UIUtility::IsEqual(const DK_RECT& rectLeft, const DK_RECT& rectRight)
{
    return (rectLeft.left == rectRight.left)
        && (rectLeft.top == rectRight.top)
        && (rectLeft.right == rectRight.right)
        && (rectLeft.bottom == rectRight.bottom);
}

bool UIUtility::PosInRect(const DK_RECT& rect, const DK_POS& pos)
{
    return !CUtility::IsEmptyRect(rect)
        && (rect.left <= pos.X)
        && (rect.top <= pos.Y)
        && (rect.right >= pos.X)
        && (rect.bottom >= pos.Y);
}

bool UIUtility::IsEmptyBox(DK_BOX box)
{
    return (box.X0 == box.X1) || (box.Y0 == box.Y1);
}

