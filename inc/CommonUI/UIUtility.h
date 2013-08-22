#ifndef __UIUTILITY__
#define __UIUTILITY__

#include "GUI/UIMessageBox.h"
#include "GUI/UITextSingleLine.h"
#include <string>

enum RELOGIN_TYPE
{
    RELOGIN_WITH_DIALOG = 0,
    RELOGIN_WITH_TOKEN_SYNC = 1,
    RELOGIN_WITH_TOKEN_ASYNC = 2
};
class UIUtility
{
public:
    static UIMessageBox* CreateMessageBox(
            INT32 iType = MB_OK,
            UIContainer* parent = NULL);
    static void ShowMessageBox(std::string text);
    static UITextSingleLine* CreateTip(
            CString tipText,
            UIContainer* parent = NULL);
    static void ShowTip(
            UITextSingleLine* uiText,
            bool showTip);
	static void ShowAccountLoginDlg();
    static bool CheckAndReConnectWifi(UIContainer* parent = NULL);
    static bool CheckAndReLogin(RELOGIN_TYPE loginType = RELOGIN_WITH_TOKEN_ASYNC);
    static bool CheckAndReLoginSync();
    static UIContainer* GetCurDisplay();
    static HRESULT DrawBorder(
            DK_IMAGE image,
            int leftMargin,
            int topMargin,
            int rightMargin,
            int bottomMargin,
            int titleHeight = 0,
            bool isBtn = false,
            bool isRightAngle = false);
    static void SetScreenTips(const char* tip, unsigned int msec = 3000);
    static std::string TransferIntToString(int time);
    static bool LeftContainsRight(const DK_RECT& rectLeft, const DK_RECT& rectRight);
    static bool IsEqual(const DK_RECT& rectLeft, const DK_RECT& rectRight);
    static bool PosInRect(const DK_RECT& rect, const DK_POS& pos);
    static bool IsEmptyBox(DK_BOX box);
};

#endif

