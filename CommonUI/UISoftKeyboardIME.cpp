#include "CommonUI/UISoftKeyboardIME.h"
#include "CommonUI/UIIMEManager.h"
#include "I18n/StringManager.h"
#include "GUI/CTpGraphics.h"
#include "interface.h"
#include "IME/IMEManager.h"
#include "IME/IMEFcitx.h"
#include "Framework/CDisplay.h"
#include "Utility/EncodeUtil.h"
#include "GUI/GlobalEventListener.h"
#include "GUI/GUISystem.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "CommonUI/UIUtility.h"
#include "drivers/DeviceInfo.h"

using dk::utility::EncodeUtil;
using namespace WindowsMetrics;

// 切换逻辑
// 按Shift则改变当前大小写模式，切到English模式，
// 按下方Number键，则根据当前mode进行切换
// LOWER, UPPER->SYNBOL1

static const int INPUT_BTN_PER_PAGE = 29;

static const int SHIFT_BTN_INDEX = 29;
static const int NUMBER_BTN_INDEX = 31;
static const int LANGUAGE_INDEX = 32;
static const int SPACEINDEX = 27;
static const int BACKSPACEINDEX = 30;
static const int ENTERINDEX = 33;

static const char* s_symbol = "#=~";
static const char* s_number = "123";
static const char* s_abc = "ABC";
static const char* s_null = "";

static const char* s_keyTables[][INPUT_BTN_PER_PAGE] =
{
    // english lower
    {
        "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", 
        "a", "s", "d", "f", "g", "h", "j", "k", "l", 
        "z", "x", "c", "v", "b", "n", "m", 
        ".", " ", "@",
    }, 
    // english upper
    {
        "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", 
        "A", "S", "D", "F", "G", "H", "J", "K", "L", 
        "Z", "X", "C", "V", "B", "N", "M", 
        ".", " ", "@", 
    }, 
    // english number
    {
        "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", 
        "$", "&", "(", ")", "\"", "'", "-", "+", "/", 
        "!", "?", ":", ";", ",",  "_",  "…", 
        ".", " ", "@", 
    }, 
    // english symbol
    {
        "`",  "*",  "#",  "￥",  "%",  "|",  "\\",  "×",  "‘",  "’", 
        "=",  "~",  "[",  "]",  "{",  "}",  "<",  ">", "^", 
        "™",  "§",  "®",  "¢",  "£",  "€",  "©",
        ".", " ", "@", 
    },

    // chinese lower
    {
        "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", 
        "a", "s", "d", "f", "g", "h", "j", "k", "l", 
        "z", "x", "c", "v", "b", "n", "m", 
        "，", " ", "。",
    }, 
    // chinese upper
    {
        "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", 
        "A", "S", "D", "F", "G", "H", "J", "K", "L", 
        "Z", "X", "C", "V", "B", "N", "M", 
        "，", " ", "。", 
    },
    // chinese number
    {
        "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", 
        "$", "&", "（", "）", "“", "‘", "-", "+", "/", 
        "！", "？", "：", "；", "@",  "_",  "……", 
        "，", " ", "。",
    }, 
    // chinese symbol
    {
        "`",  "、",  "#",  "￥",  "%",  "|",  "\\",  "×",  "“",  "”", 
        "—",  "‘",  "’",  "=",  "~",  "·",  "{",  "}", "^", 
        "【",  "】",  "《",  "》",  "<",  ">",  "*",
        "，", " ", "。",
    },
};

SINGLETON_CPP(UISoftKeyboardIME);


UISoftKeyboardIME::UISoftKeyboardIME()
    : m_spBackground()
    , m_bUsingBackgroundPicture(false)
    , m_inputMode(IUIIME_NONE)
    , m_curPyIndex(0)
#ifdef KINDLE_FOR_TOUCH
    , m_backspaceButton(NULL)
#endif
{
    SetZPosition(ZPOSITION_IME);
    SetVisible(FALSE);
    SetupKeyboard();
    SwitchMode(m_inputMode, IUIIME_ENGLISH_LOWER);
#ifdef KINDLE_FOR_TOUCH
    SetClickable(true);
    HookTouch();
#endif
}

void UISoftKeyboardIME::SetInputMode(int mode)
{
    if (IsValidMode(mode))
    {
        SwitchMode(m_inputMode, (INPUTMODE)mode);
    }
}

bool UISoftKeyboardIME::IsValidMode(int mode)
{
    return (mode > IUIIME_NONE && mode < IUIIME_END);
}

int UISoftKeyboardIME::GetKeyTableIndex(INPUTMODE mode)
{
    int index = -1;
    if (mode & IUIIME_USED)
    {
        index = (mode & IUIIME_CHINESE) ? 4 : 0;
        if (mode & IUIIME_NUMBER)
        {
            index += 2;
        }
        else if (mode & IUIIME_SYMBOL)
        {
            index += 3;
        }
        else if (mode & IUIIME_UPPER)
        {
            index ++;
        }
    }
    return index;
}

#ifdef KINDLE_FOR_TOUCH
bool UISoftKeyboardIME::OnHookTouch(MoveEvent* moveEvent)
{
    if (moveEvent->GetActionMasked() != MoveEvent::ACTION_DOWN)
    {
        return false;
    }
    if (!IsShow())
    {
        return false;
    }
    DebugPrintf(DLC_GESTURE, "x, y (%d, %d), rect (%d, %d)-(%d, %d)",
            moveEvent->GetX(), moveEvent->GetY(),
            m_iLeft, m_iTop, m_iWidth, m_iHeight);
    if (!PointInWindowAbsolute(moveEvent->GetX(), moveEvent->GetY())
            && (NULL != m_pTextBox && !m_pTextBox->PointInWindowAbsolute(moveEvent->GetX(), moveEvent->GetY())))
    {
        HideIME();
    }
    return false;
}
#endif

void UISoftKeyboardIME::ClearPinYinData()
{
    m_pyCandidateIndexes.clear();
    ClearCandidates();
    if (IsChineseMode())
    {
        m_pyLabel.SetText(s_null);
        if (IsVisible())
        {
            m_pyLabel.UpdateWindow();
        }
    }
    m_curPyIndex = 0;
    IMEReset();
}

UISoftKeyboardIME::~UISoftKeyboardIME()
{
    DebugPrintf(DLC_UISOFTKEYBOARDIME, "UISoftKeyboardIME::~UISoftKeyboardIME() Start");
    m_pTextBox      = NULL;
    if (m_pParent)
    {
        m_pParent->RemoveChild(this, false);
    }
    DebugPrintf(DLC_UISOFTKEYBOARDIME, "UISoftKeyboardIME::~UISoftKeyboardIME() End");
}

BOOL UISoftKeyboardIME::OnKeyPress(INT32 iKeyCode)
{
    DebugPrintf(DLC_LIWEI, "UISoftKeyboardIME::OnKeyPress() Start : iKeyCode = %d", iKeyCode);
#ifdef KINDLE_FOR_TOUCH
    if (KEY_HOME == iKeyCode)
    {
        HideIME();
    }
    return UIWindow::OnKeyPress(iKeyCode);
#else
    BOOL ret = TRUE;

    switch (iKeyCode)
    {
    case KEY_LEFT: 
    case KEY_UP:
    case KEY_RIGHT:
    case KEY_DOWN:
        {
            MoveFocusWithStructTable(iKeyCode, 30);
            ret = FALSE;
        }
        break;
    case KEY_DEL:
        if (m_pTextBox)
        {
            m_pTextBox->DeleteChar();
            ret = FALSE;
        }
        break;
    default:
        ret = UIContainer::OnKeyPress(iKeyCode);
        break;
    }

    if (ret)
    {
        if (iKeyCode == KEY_LPAGEUP || iKeyCode == KEY_RPAGEUP)
        {
            PYWordsPageUpDown(false);
            return FALSE;
        }

        if (iKeyCode == KEY_LPAGEDOWN || iKeyCode == KEY_RPAGEDOWN)
        {
            PYWordsPageUpDown(true);
            return FALSE;
        }
    }
    DebugPrintf(DLC_UISOFTKEYBOARDIME, "UISoftKeyboardIME::OnKeyPress() End");
    return ret ? UIIME::OnKeyPress(iKeyCode) : FALSE;
#endif
}

void UISoftKeyboardIME::ClearCandidates()
{
    DebugPrintf(DLC_UISOFTKEYBOARDIME, "UISoftKeyboardIME::ClearCandidates() Start");
    if (!IsChineseMode())
    {
        return;
    }
    //clear words button
    const size_t iSize = m_pvWordButton.size();
    for(size_t i =0; i < iSize; i++)
    {
        UITouchButton* pBtn = m_pvWordButton[i];
        if (pBtn)
        {
            pBtn->SetText("");
            pBtn->SetVisible(false);
        }
    }

    if (!IsVisible())
    {
        return;
    }

    DK_IMAGE imgParent;
    DK_RECT rectParent;
    GetParentDC(imgParent, rectParent);        


    DK_IMAGE imgButtonBackground;

    DK_RECT rcSelf={m_iLeft+55, m_iTop+40+3, m_iLeft+55 + 490, m_iTop+40+3 + 34 };
    CropImage(
        imgParent,
        rcSelf,
        &imgButtonBackground
        );

    CTpGraphics grf(imgButtonBackground);

    grf.FillRect(
        0,
        0,
        490,
        34,
        ColorManager::knWhite);

    UpdateToScreen(rcSelf);

    DebugPrintf(DLC_UISOFTKEYBOARDIME, "UISoftKeyboardIME::ClearCandidates() End");
}

void UISoftKeyboardIME::ShowIME(UIContainer *pParent)
{
    DebugPrintf(DLC_UISOFTKEYBOARDIME, "UISoftKeyboardIME::ShowIME() Start");
    SetShowDelay(false);
    SetShow(true);
    if (! IsVisible())
    {
        SetVisible(TRUE);
        ClearCandidates();
    }
    m_pParent = pParent;
    AppendChildToTopFullScreenContainer();

    ClearPinYinData();
#ifdef KINDLE_FOR_TOUCH
    if (m_pTextBox)
    {
        m_pTextBox->SetFocus(true);
    }
#else
    SetChildWindowFocus(GetChildIndex(&m_KeyboardButton[14]), false);
#endif
    RedrawWindow();
}

void UISoftKeyboardIME::HideIME()
{
    SetVisible(FALSE);
    UIIME::HideIME();
}

HRESULT UISoftKeyboardIME::DrawBackGround(DK_IMAGE drawingImg)
{
    if (!m_windowSizer)
    {
        return S_FALSE;
    }

    HRESULT hr(S_OK);
    const int horizontalMargin = GetWindowMetrics(UISoftKeyboardIMEHorizonMarginIndex);
    // erasebackground

    if (IsChineseMode())
    {
        int height = 0;
        for (int i = 0; i < 3; ++i)
        {
            UISizerItem* pItem = m_windowSizer->GetItem((size_t)i);
            if (pItem)
            {
                height += pItem->GetSize().GetHeight();
            }
        }
        const int pyLabelVerticalMargin = GetWindowMetrics(UISoftKeyboardIMEPYLabelVerticalMarginIndex);
        const int keyboardSpacing = GetWindowMetrics(UISoftKeyboardIMEKeyboardSpacingIndex);
        const int spacing = pyLabelVerticalMargin + keyboardSpacing;

        DK_IMAGE imgBorder;
        DK_RECT rcBorder={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + height + spacing};
        RTN_HR_IF_FAILED(CropImage(drawingImg, rcBorder, &imgBorder));
        CTpGraphics grf(imgBorder);
        grf.EraserBackGround();
        const int borderLine = GetWindowMetrics(UIDialogBorderLineIndex);
        RTN_HR_IF_FAILED(grf.DrawBorder(horizontalMargin, 0, horizontalMargin, spacing));
        RTN_HR_IF_FAILED(grf.DrawLine(horizontalMargin, 
                                     ((height - borderLine) >> 1), 
                                     m_iWidth - (horizontalMargin << 1), 
                                     borderLine, SOLID_STROKE));

    }

    UISizerItem* pItem = m_windowSizer->GetItem((size_t)m_windowSizer->GetItemCount() - 1);
    if (pItem)
    {
        DK_IMAGE imgBorder;
        const int top = m_iTop + pItem->GetPosition().y;
        const int height = pItem->GetSize().GetHeight();
        const int spacing = m_iTop + m_iHeight - top - height;
        DK_RECT rcBorder={m_iLeft, top, m_iLeft + m_iWidth, top + height + spacing};
        RTN_HR_IF_FAILED(CropImage(drawingImg, rcBorder, &imgBorder));
        CTpGraphics grf(imgBorder);
        grf.EraserBackGround();
        RTN_HR_IF_FAILED(grf.DrawBorder(horizontalMargin, 0, horizontalMargin, spacing));
    }
    return hr;
}

HRESULT UISoftKeyboardIME::Draw(DK_IMAGE image)
{
    DebugPrintf(DLC_UISOFTKEYBOARDIME, "UISoftKeyboardIME::Draw() m_pParent = %s", m_pParent ? m_pParent->GetClassName() : "NULL");
    if (!m_bIsVisible)
    {
        return S_OK;
    }

    HRESULT hr(S_OK);
    DK_IMAGE imgSelf;
    DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
    RTN_HR_IF_FAILED(CropImage(
        image,
        rcSelf,
        &imgSelf
        ));
    RTN_HR_IF_FAILED(DrawBackGround(image));
    
    for (size_t i = 0; i < GetChildrenCount(); i++)
    {
        UIWindow* pWin = GetChildByIndex(i);
        if (pWin && pWin->IsVisible())
        {
            pWin->Draw(imgSelf);
        }
    }
    return S_OK;
}

void UISoftKeyboardIME::SetBackground(SPtr<ITpImage> pBackgroundImage)
{
    if(pBackgroundImage)
    {
        m_spBackground = pBackgroundImage;
        m_bUsingBackgroundPicture = true;
    }
}

void UISoftKeyboardIME::SetupKeyboard()
{
    //no need to be called move window by any control
    CDisplay* pDisplay = CDisplay::GetDisplay();
    if (NULL == pDisplay)
    {
        return;
    }

    const int fontSize = GetWindowFontSize(FontSize20Index);
    const int horizontalMargin = GetWindowMetrics(UISoftKeyboardIMEHorizonMarginIndex);
    const int keyboardSize = GetWindowMetrics(UISoftKeyboardIMEKeyboardSizeIndex);
    const int keyboardWidth = GetWindowMetrics(UISoftKeyboardIMEKeyboardWidthIndex);
    const int spaceKeyWidth = GetWindowMetrics(UISoftKeyboardIMESpaceKeyWidthIndex);
    const int IMEHeight = GetWindowMetrics(UISoftKeyboardIMEHeightIndex);
    const int keyboardSpacing = GetWindowMetrics(UISoftKeyboardIMEKeyboardSpacingIndex);
    const int screenWidth = pDisplay->GetScreenWidth();
    const int screenHeight = pDisplay->GetScreenHeight();
    UIWindow::MoveWindow(0, screenHeight - IMEHeight, screenWidth, IMEHeight);

    SPtr<ITpImage> imgBtnSquare = ImageManager::GetImage(IMAGE_IME_KEY_SQUARE);
    SPtr<ITpImage> imgBtnSquareP = ImageManager::GetImage(IMAGE_IME_KEY_SQUARE_PRESSED);
    SPtr<ITpImage> imgBtnOblong = ImageManager::GetImage(IMAGE_IME_KEY_OBLONG);
    SPtr<ITpImage> imgBtnOblongP = ImageManager::GetImage(IMAGE_IME_KEY_OBLONG_PRESSED);
    SPtr<ITpImage> imgSpace = ImageManager::GetImage(IMAGE_IME_KEY_SPACE);
    SPtr<ITpImage> imgSpaceP = ImageManager::GetImage(IMAGE_IME_KEY_SPACEP);
    SPtr<ITpImage> imgLowerShift = ImageManager::GetImage(IMAGE_IME_KEY_LOWER_SHIFT);
    SPtr<ITpImage> imgLowerShiftP = ImageManager::GetImage(IMAGE_IME_KEY_LOWER_SHIFTP);
    SPtr<ITpImage> imgDel = ImageManager::GetImage(IMAGE_IME_KEY_DEL);
    SPtr<ITpImage> imgDelP = ImageManager::GetImage(IMAGE_IME_KEY_DELP);
    SPtr<ITpImage> imgEnter = ImageManager::GetImage(IMAGE_IME_KEY_ENTER);
    SPtr<ITpImage> imgEnterP = ImageManager::GetImage(IMAGE_IME_KEY_ENTERP);

    m_KeyboardButton[0].Initialize(ID_KEYBOARD_Q, "q", fontSize, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[1].Initialize(ID_KEYBOARD_W, "w", fontSize, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[2].Initialize(ID_KEYBOARD_E, "e", fontSize, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[3].Initialize(ID_KEYBOARD_R, "r", fontSize, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[4].Initialize(ID_KEYBOARD_T, "t", fontSize, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[5].Initialize(ID_KEYBOARD_Y, "y", fontSize, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[6].Initialize(ID_KEYBOARD_U, "u", fontSize, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[7].Initialize(ID_KEYBOARD_I, "i", fontSize, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[8].Initialize(ID_KEYBOARD_O, "o", fontSize, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[9].Initialize(ID_KEYBOARD_P, "p", fontSize, imgBtnSquare, imgBtnSquareP);

    m_KeyboardButton[10].Initialize(ID_KEYBOARD_A, "a", fontSize, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[11].Initialize(ID_KEYBOARD_S, "s", fontSize, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[12].Initialize(ID_KEYBOARD_D, "d", fontSize, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[13].Initialize(ID_KEYBOARD_F, "f", fontSize, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[14].Initialize(ID_KEYBOARD_G, "g", fontSize, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[15].Initialize(ID_KEYBOARD_H, "h", fontSize, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[16].Initialize(ID_KEYBOARD_J, "j", fontSize, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[17].Initialize(ID_KEYBOARD_K, "k", fontSize, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[18].Initialize(ID_KEYBOARD_L, "l", fontSize, imgBtnSquare, imgBtnSquareP);

    m_KeyboardButton[19].Initialize(ID_KEYBOARD_Z, "z", fontSize, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[20].Initialize(ID_KEYBOARD_X, "x", fontSize, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[21].Initialize(ID_KEYBOARD_C, "c", fontSize, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[22].Initialize(ID_KEYBOARD_V, "v", fontSize, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[23].Initialize(ID_KEYBOARD_B, "b", fontSize, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[24].Initialize(ID_KEYBOARD_N, "n", fontSize, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[25].Initialize(ID_KEYBOARD_M, "m", fontSize, imgBtnSquare, imgBtnSquareP);

    m_KeyboardButton[26].Initialize(ID_KEYBOARD_COMMA, ",", fontSize, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[SPACEINDEX].Initialize(ID_KEYBOARD_SPACE, "",  fontSize, imgSpace, imgSpaceP);
    m_KeyboardButton[28].Initialize(ID_KEYBOARD_PERIOD, ".",fontSize, imgBtnSquare, imgBtnSquareP);

    m_KeyboardButton[SHIFT_BTN_INDEX].Initialize(ID_KEYBOARD_SHIFT, "",      fontSize, imgLowerShift, imgLowerShiftP);
    m_KeyboardButton[BACKSPACEINDEX].Initialize(ID_KEYBOARD_BACKSPACE, "",  fontSize, imgDel, imgDelP);
#ifdef KINDLE_FOR_TOUCH
    m_backspaceButton = &m_KeyboardButton[BACKSPACEINDEX];
    m_backspaceButton->SetLongClickable(true);
#endif
    m_KeyboardButton[NUMBER_BTN_INDEX].Initialize(ID_KEYBOARD_NUMLOCK, "",fontSize, imgBtnOblong, imgBtnOblongP);
    m_KeyboardButton[NUMBER_BTN_INDEX].SetEnable(!DeviceInfo::IsK3());
    m_KeyboardButton[LANGUAGE_INDEX].Initialize(ID_KEYBOARD_EN, "",       fontSize, imgBtnOblong, imgBtnOblongP);
    m_KeyboardButton[ENTERINDEX].Initialize(ID_KEYBOARD_ENTER, "",      fontSize, imgEnter, imgEnterP);

    //pageup, pagedown
    m_KeyboardButton[34].Initialize(ID_KEYBOARD_WORDS_PAGEUP, "",   fontSize, ImageManager::GetImage(IMAGE_IME_ICON_ARROW_LEFT), ImageManager::GetImage(IMAGE_IME_ICON_ARROW_LEFTP));
    m_KeyboardButton[35].Initialize(ID_KEYBOARD_WORDS_PAGEDOWN, "", fontSize, ImageManager::GetImage(IMAGE_IME_ICON_ARROW_RIGHT), ImageManager::GetImage(IMAGE_IME_ICON_ARROW_RIGHTP));

    m_pyLabel.SetForeColor(ColorManager::knBlack);
    m_pyLabel.SetAlign(ALIGN_LEFT);
    m_pyLabel.SetText(s_null);
    AppendChild(&m_pyLabel);

    for (int i = 0; i < MAX_SELECTORS; ++i)
    {
        UITouchButton* pButton = new UITouchButton();
        if (pButton)
        {
            pButton->SetUpdateWindowFlag(PAINT_FLAG_DU);
            pButton->SetVisible(false);
            AppendChild(pButton);
            m_pvWordButton.push_back(pButton);
        }
    }

    if (!m_windowSizer)
    {
        UISizer* pMainSizer = new UIBoxSizer(dkVERTICAL);
        UISizer* pWordLstSizer = new UIBoxSizer(dkHORIZONTAL);
        UISizer* pKeyBoardSizer = new UIBoxSizer(dkVERTICAL);
        UISizer* pLine0Sizer = new UIGridSizer(1, UISOFTKEYBOARDIME_KEYCOUNT_LINE0, keyboardSpacing, keyboardSpacing);
        UISizer* pLine1Sizer = new UIGridSizer(1, UISOFTKEYBOARDIME_KEYCOUNT_LINE1, keyboardSpacing, keyboardSpacing);
        UISizer* pLine2Sizer = new UIBoxSizer(dkHORIZONTAL);
        UISizer* pLine3Sizer = new UIBoxSizer(dkHORIZONTAL);
        if ((NULL != pMainSizer) && (NULL != pWordLstSizer) && (NULL != pKeyBoardSizer)
            && (NULL != pLine0Sizer) && (NULL != pLine1Sizer) && (NULL != pLine2Sizer) && (NULL != pLine3Sizer))
        {
            const int pyLabelVerticalMargin = GetWindowMetrics(UISoftKeyboardIMEPYLabelVerticalMarginIndex);
            const int pyLabelMinHeight = GetWindowMetrics(UISoftKeyboardIMEPYLabelMinHeightIndex);
            const int fontSize20 = GetWindowFontSize(FontSize20Index);
            m_pyLabel.SetFontSize(fontSize20);
            m_pyLabel.SetMinHeight(pyLabelMinHeight);

            if (DeviceInfo::IsKPW())
            {
                pMainSizer->AddSpacer(pyLabelVerticalMargin + 4);
                pMainSizer->Add(&m_pyLabel, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizontalMargin << 1).ReserveSpaceEvenIfHidden());
            }
            else
            {
                pMainSizer->AddSpacer(pyLabelVerticalMargin - 1);
                pMainSizer->Add(&m_pyLabel, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizontalMargin + keyboardSpacing).ReserveSpaceEvenIfHidden());
            }

            pMainSizer->Add(pWordLstSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizontalMargin).ReserveSpaceEvenIfHidden());
            pMainSizer->AddSpacer(pyLabelVerticalMargin + keyboardSpacing);
            pMainSizer->Add(pKeyBoardSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizontalMargin));

            // 中文待选字布局
            pWordLstSizer->Add(&m_KeyboardButton[34], UISizerFlags().ReserveSpaceEvenIfHidden());
            pWordLstSizer->AddStretchSpacer();
            pWordLstSizer->Add(&m_KeyboardButton[35], UISizerFlags().ReserveSpaceEvenIfHidden());

            // 键盘布局
            pKeyBoardSizer->Add(pLine0Sizer, UISizerFlags().Align(dkALIGN_CENTER_HORIZONTAL).Border(dkTOP | dkBOTTOM, keyboardSpacing));
            pKeyBoardSizer->Add(pLine1Sizer, UISizerFlags().Align(dkALIGN_CENTER_HORIZONTAL).Border(dkBOTTOM, keyboardSpacing));
            pKeyBoardSizer->Add(pLine2Sizer, UISizerFlags().Align(dkALIGN_CENTER_HORIZONTAL).Border(dkLEFT | dkRIGHT, keyboardSpacing));
            pKeyBoardSizer->Add(pLine3Sizer, UISizerFlags().Align(dkALIGN_CENTER_HORIZONTAL).Border(dkALL, keyboardSpacing));

            for (int i = 0; i < KEYBOARD_COUNT; ++i)
            {
                m_KeyboardButton[i].SetMinWidth(keyboardSize);
                m_KeyboardButton[i].SetMinHeight(keyboardSize);
                m_KeyboardButton[i].SetUpdateWindowFlag(PAINT_FLAG_DU);
                AppendChild(&m_KeyboardButton[i]);
            }
            m_KeyboardButton[SPACEINDEX].SetMinWidth(spaceKeyWidth);
            m_KeyboardButton[SHIFT_BTN_INDEX].SetMinWidth(keyboardWidth);
            m_KeyboardButton[BACKSPACEINDEX].SetMinWidth(keyboardWidth);
            m_KeyboardButton[NUMBER_BTN_INDEX].SetMinWidth(keyboardWidth);
            m_KeyboardButton[ENTERINDEX].SetMinWidth(keyboardWidth);
            m_KeyboardButton[LANGUAGE_INDEX].SetMinWidth(keyboardWidth);

            for ( int i = 0; i < UISOFTKEYBOARDIME_KEYCOUNT_LINE0; ++i)
            {
                pLine0Sizer->Add(&m_KeyboardButton[i], UISizerFlags(1).Expand());
            }

            const int line1EndIndex = UISOFTKEYBOARDIME_KEYCOUNT_LINE0 + UISOFTKEYBOARDIME_KEYCOUNT_LINE1 - 1;
            for (int i = UISOFTKEYBOARDIME_KEYCOUNT_LINE0; i <= line1EndIndex; ++i)
            {
                pLine1Sizer->Add(&m_KeyboardButton[i], UISizerFlags().Expand());
            }

            const int line2Index[UISOFTKEYBOARDIME_KEYCOUNT_LINE2] = {SHIFT_BTN_INDEX, 19, 20, 21, 22, 23, 24, 25, BACKSPACEINDEX};
            const int halfSpacing = keyboardSpacing >> 1;
            for (int i = 0; i < UISOFTKEYBOARDIME_KEYCOUNT_LINE2; ++i)
            {
                pLine2Sizer->Add(&m_KeyboardButton[line2Index[i]], UISizerFlags(1).Expand().Border(dkLEFT | dkRIGHT, halfSpacing));
            }

            const int line3Index[UISOFTKEYBOARDIME_KEYCOUNT_LINE3] = {NUMBER_BTN_INDEX, 26, SPACEINDEX, 28, LANGUAGE_INDEX, ENTERINDEX};
            for (int i = 0; i < UISOFTKEYBOARDIME_KEYCOUNT_LINE3; ++i)
            {
                pLine3Sizer->Add(&m_KeyboardButton[line3Index[i]], UISizerFlags(1).Expand().Border(dkLEFT | dkRIGHT, halfSpacing));
            }

            if (!IsChineseMode())
            {
                m_KeyboardButton[34].SetVisible(false);
                m_KeyboardButton[35].SetVisible(false);
                m_pyLabel.SetVisible(false);
            }

            SetSizer(pMainSizer);
            Layout();
        }
        else
        {
            SafeDeletePointer(pMainSizer);
            SafeDeletePointer(pWordLstSizer);
            SafeDeletePointer(pKeyBoardSizer);
            SafeDeletePointer(pLine0Sizer);
            SafeDeletePointer(pLine1Sizer);
            SafeDeletePointer(pLine2Sizer);
            SafeDeletePointer(pLine3Sizer);
        }
    }
}

void UISoftKeyboardIME::ModifyBoxText(const std::string& keyboardText)
{
	if (!m_pTextBox)
		return;

	if (!m_pTextBox->IsAllSelected())
	{
        m_pTextBox->InsertTextUTF8( keyboardText.c_str(), keyboardText.size());
    }
    else
    {
        m_pTextBox->SetTextUTF8( keyboardText.c_str(), keyboardText.size());
    }
}

bool UISoftKeyboardIME::HasCandiatesToShow() const
{
    if (m_pvWordButton.empty())
    {
        return false;
    }

    const UITouchButton* btn = m_pvWordButton[0];
    if (btn->IsVisible())
    {
        return true;
    }
    return false;
}

void UISoftKeyboardIME::ProcessSingleChar(int index)
{
    if (index >= 0 && index < INPUT_BTN_PER_PAGE && m_pTextBox)
    {
        const int curKeyTableIndex = GetKeyTableIndex(m_inputMode);
        ModifyBoxText(s_keyTables[curKeyTableIndex][index]);
        m_pTextBox->UpdateWindow();
    }
}

void UISoftKeyboardIME::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    DebugPrintf(DLC_UISOFTKEYBOARDIME, "UISoftKeyboardIME::OnCommand()  Start: dwCmdId = %d", dwCmdId);
#ifndef KINDLE_FOR_TOUCH
    bool moveFocus = false;
#endif
    string _keyString = "";
    switch (dwCmdId)
    {
    case ID_KEYBOARD_Q:
    case ID_KEYBOARD_W:
    case ID_KEYBOARD_E:
    case ID_KEYBOARD_R:
    case ID_KEYBOARD_T:
    case ID_KEYBOARD_Y:
    case ID_KEYBOARD_U:
    case ID_KEYBOARD_I:
    case ID_KEYBOARD_O:
    case ID_KEYBOARD_P:
    case ID_KEYBOARD_A:
    case ID_KEYBOARD_S:
    case ID_KEYBOARD_D:
    case ID_KEYBOARD_F:
    case ID_KEYBOARD_G:
    case ID_KEYBOARD_H:
    case ID_KEYBOARD_J:
    case ID_KEYBOARD_K:
    case ID_KEYBOARD_L:
    case ID_KEYBOARD_Z:
    case ID_KEYBOARD_X:
    case ID_KEYBOARD_C:
    case ID_KEYBOARD_V:
    case ID_KEYBOARD_B:
    case ID_KEYBOARD_N:
    case ID_KEYBOARD_M:
        {
            int index = dwCmdId - ID_KEYBOARD_Q;
            
            if (IsChineseMode())
            {
                const int curKeyTableIndex = GetKeyTableIndex(IUIIME_CHINESE_LOWER);
                eProcessResult res = IMEInputKey(s_keyTables[curKeyTableIndex][index][0]);
                switch (res)
                {
                    case CAN_NOT_PROCESS:
                        {
                            return;
                        }
                        break;
					case PROCESSED_WITH_RESULT:
						{
							ModifyBoxText(IMEGetResultString());
                			ClearPinYinData();
							UpdateWindow();
						}
						break;
                    default:
                        break;
                        
                }
                m_curPyIndex = 0;
                m_pyCandidateIndexes.clear();
                m_pyLabel.SetText(IMEGetInputString().c_str());
                HandlePYWordsButton();
                UpdateWindow();
            }
            else
            {
                ProcessSingleChar(index);
            }
        }
        break;
    case ID_KEYBOARD_COMMA:
    case ID_KEYBOARD_PERIOD:
        {
            ProcessSingleChar(dwCmdId - ID_KEYBOARD_Q);
        }
        break;
    case ID_KEYBOARD_SPACE:
        {
            //what ever, just update the value directly
            if (IsChineseMode() && m_pvWordButton[0]->IsVisible())
            {
                string _input = m_pvWordButton[0]->GetText();
                if (!_input.empty())
                {
                    int index = 0;
                    if (!m_pyCandidateIndexes.empty())
                    {
                        index = m_pyCandidateIndexes.back();
                    }
                    eProcessResult res = IMESelectIndex(index);
                    if (res == PROCESSED_WITH_RESULT)
                    {
                        ModifyBoxText(IMEGetResultString());
                        m_pTextBox->UpdateWindow();

                    }
                }
                ClearPinYinData();
                UpdateWindow();
            }
            else
            {
                ClearPinYinData();
                ProcessSingleChar(dwCmdId - ID_KEYBOARD_Q);
            }
        }
        break;
    case ID_KEYBOARD_SHIFT:
        {
            int newMode = IUIIME_NONE;
            if (m_inputMode & IUIIME_SYMBOL)
            {
                newMode = (m_inputMode & ~IUIIME_SYMBOL) | IUIIME_NUMBER;
            }
            else if (m_inputMode & IUIIME_NUMBER)
            {
                newMode = (m_inputMode & ~IUIIME_NUMBER) | IUIIME_SYMBOL;
            }
            else if (m_inputMode & IUIIME_UPPER)
            {
                newMode = (m_inputMode & ~IUIIME_UPPER);
            }
            else
            {
                newMode = (m_inputMode | IUIIME_UPPER);
            }

            if (IsValidMode(newMode))
            {
                SwitchMode(m_inputMode, (INPUTMODE)newMode);
                UpdateWindow();
            }
        }
        break;

    case ID_KEYBOARD_BACKSPACE:
        {
            // when in PY, remove the py char first, then the text box
            if (IsChineseMode() && !IMEGetInputString().empty())
            {
                IMEDeleteLastInput();
                m_pyLabel.SetText(IMEGetInputString().c_str());
                m_pyLabel.UpdateWindow();
                m_curPyIndex = 0;
                if (IMEGetInputString().empty())
                {
                    ClearPinYinData();
                }
                else
                {
                    HandlePYWordsButton();
                }
                UpdateWindow();
                
            }
            else //otherwise remove the char in textbox
            {
				if (!m_pTextBox->IsAllSelected())
				{
                    m_pTextBox->DeleteChar();
				}
				else
				{
					m_pTextBox->ClearText();
				}
                m_pTextBox->UpdateWindow();
            }
        }
        break;

    case ID_KEYBOARD_NUMLOCK:
        if (!DeviceInfo::IsK3())
        {
            int newMode = IUIIME_NONE;
            if (m_inputMode & IUIIME_SYMBOL)
            {
                newMode = (m_inputMode & ~IUIIME_SYMBOL & ~IUIIME_NUMBER);
            }
            else if (m_inputMode & IUIIME_NUMBER)
            {
                newMode = (m_inputMode & ~IUIIME_SYMBOL  & ~IUIIME_NUMBER);
            }
            else
            {
                newMode = (m_inputMode | IUIIME_NUMBER);
            }

            if (IsValidMode(newMode))
            {
                SwitchMode(m_inputMode, (INPUTMODE)newMode);
                UpdateWindow();
            }
        }
        break;
    case ID_KEYBOARD_EN:
        {
            int newMode = IUIIME_NONE;
            if (m_inputMode & IUIIME_CHINESE)
            {
                newMode = (m_inputMode & ~IUIIME_CHINESE);
            }
            else
            {
                newMode = (m_inputMode | IUIIME_CHINESE);
            }

            if (IsValidMode(newMode))
            {
                SwitchMode(m_inputMode, (INPUTMODE)newMode);
                UpdateWindow();
            }
        }
        break;

    case ID_KEYBOARD_ENTER:
        {
            if (IsChineseMode() && !IMEGetInputString().empty())
            {
                string textbox_value = IMEGetInputString();
                ClearPinYinData();
                m_pTextBox->InsertTextUTF8( textbox_value.c_str(), textbox_value.size());
            }
            else
            {
                m_pTextBox->OnEnterPress();
                HideIME();
            }
        }
        break;
    case ID_IME_WORD_SELECTOR_1:
    case ID_IME_WORD_SELECTOR_2:
    case ID_IME_WORD_SELECTOR_3:
    case ID_IME_WORD_SELECTOR_4:
    case ID_IME_WORD_SELECTOR_5:
    case ID_IME_WORD_SELECTOR_6:
    case ID_IME_WORD_SELECTOR_7:
    case ID_IME_WORD_SELECTOR_8:
    case ID_IME_WORD_SELECTOR_9:
    case ID_IME_WORD_SELECTOR_10:
    case ID_IME_WORD_SELECTOR_11:
    case ID_IME_WORD_SELECTOR_12:
    case ID_IME_WORD_SELECTOR_13:
    case ID_IME_WORD_SELECTOR_14:
    case ID_IME_WORD_SELECTOR_15:
    case ID_IME_WORD_SELECTOR_16:
    case ID_IME_WORD_SELECTOR_17:
        {
            int btnIndex = dwCmdId - ID_IME_WORD_SELECTOR_1;
            int selectIndex = m_pyCandidateIndexes.back() + btnIndex;
            eProcessResult res = IMESelectIndex(selectIndex);
            if (res == PROCESSED_WITH_RESULT)
            {
                ModifyBoxText(IMEGetResultString());
                ClearPinYinData();
#ifndef KINDLE_FOR_TOUCH
                moveFocus = true;
#endif
            }
            else
            {
                m_pyLabel.SetText(IMEGetInputString().c_str());
                m_curPyIndex = 0;
                m_pyCandidateIndexes.clear();
                PYWordsPageUpDown(true);
            }
            UpdateWindow();
            break;
        }
        break;
    case ID_KEYBOARD_WORDS_PAGEUP:
        {
            if (HasCandiatesToShow())
            {
                PYWordsPageUpDown(false);
            }
        }
        break;

    case ID_KEYBOARD_WORDS_PAGEDOWN:
        {
            if (HasCandiatesToShow())
            {
                PYWordsPageUpDown(true);
            }
        }
        break;
    default:
        break;
    }
    if (IsShow() && pSender)
    {
        CDisplay::CacheDisabler cache;
#ifdef KINDLE_FOR_TOUCH
        pSender->SetFocus(true);
        pSender->SetFocus(false);
#else
        if (moveFocus)
        {
            SetChildWindowFocus(GetChildIndex(&m_KeyboardButton[14]), false);
        }
        else
        {
            OnChildSetFocus(pSender);
        }
#endif
    }

    DebugPrintf(DLC_UISOFTKEYBOARDIME, "UISoftKeyboardIME::OnCommand()  End");
}

void UISoftKeyboardIME::OnChildSetFocus(UIWindow* pWindow)
{
#ifndef KINDLE_FOR_TOUCH
    INT32 iIndex = GetChildIndex(pWindow);

    if (m_iFocusedChild == iIndex)
    {
        return;
    }

    ClearFocus();

    m_iFocusedChild = iIndex;
    m_bIsFocus = TRUE;
#endif
}

void UISoftKeyboardIME::PYWordsPageUpDown(bool pageDown)
{
    // 上翻页且没有下翻过
    if (!pageDown && m_pyCandidateIndexes.size() <= 1)
    {
        return;
    }
    
    const int fontsize24 = GetWindowFontSize(FontSize24Index);
    const int horizontalMargin = GetWindowMetrics(UISoftKeyboardIMEHorizonMarginIndex);
    const int keyboardSize = GetWindowMetrics(UISoftKeyboardIMEKeyboardSizeIndex);
    const int wordButtonSpacing = GetWindowMetrics(UIPixelValue15Index);
    const int pyLabelVerticalMargin = GetWindowMetrics(UISoftKeyboardIMEPYLabelVerticalMarginIndex);
    const int pyLabelMinHeight = GetWindowMetrics(UISoftKeyboardIMEPYLabelMinHeightIndex);
    int button_position_left_start = horizontalMargin + keyboardSize;
    int word_list_top = pyLabelVerticalMargin + pyLabelMinHeight;
    if (DeviceInfo::IsKPW())
    {
        // TODO(pengf): hack, should use layout
        word_list_top += 5;
    }


    int curIndex = -1;
    if (pageDown)
    {
        curIndex = m_curPyIndex;
        m_pyCandidateIndexes.push_back(curIndex);
    }
    else
    {
        m_pyCandidateIndexes.pop_back();
        curIndex = m_pyCandidateIndexes.back();
    }
    std::string pyWord = IMEGetStringByIndex(curIndex);

    int _id_index =0;

    if (!pyWord.empty())
    {
        ClearCandidates();
    }
    else
    {
    	ClearCandidates();
        return;
    }

    size_t wordIndex = 0;
    while (!pyWord.empty())
    {
        if (wordIndex >= (size_t)MAX_SELECTORS)
        {
            m_curPyIndex = curIndex;
            // rollback
            break;
        }

        UITouchButton* _pButton = m_pvWordButton[wordIndex++];
        if (_pButton)
        {
            _pButton->SetText(pyWord.c_str());
            _pButton->SetAlign(ALIGN_LEFT);
#ifdef KINDLE_FOR_TOUCH
            _pButton->SetBackgroundColor(ColorManager::knWhite);
            _pButton->SetFocusedBackgroundColor(ColorManager::knBlack);
            _pButton->SetFontSize(fontsize24);
#else
            _pButton->SetBackgroundColor(ColorManager::knWhite);
            _pButton->SetFocusedBackgroundColor(ColorManager::knBlack);
            _pButton->SetFont(0,0,fontsize24);
#endif
            _pButton->SetFontColor(ColorManager::knBlack);

            _pButton->SetFocusedFontColor(ColorManager::knWhite);

            _pButton->SetId(ID_IME_WORD_SELECTOR_1 + _id_index);
            _id_index++;


            int _button_height = _pButton->GetTextHeight();
            int _button_width = _pButton->GetTextWidth();

            if (button_position_left_start + _button_width > (m_iWidth - horizontalMargin - keyboardSize - wordButtonSpacing))
            {
                // rollback
                m_curPyIndex = curIndex;
                _pButton->SetVisible(false);
                //delete _pButton;
                break;
            }
            else
            {
                _pButton->SetVisible(true);
                //AppendChild(_pButton);
                //m_pvWordButton.push_back(_pButton);

                _pButton->MoveWindow(button_position_left_start, word_list_top + ((keyboardSize - _button_height) >> 1), _button_width, _button_height);
                _pButton->SetVisible(TRUE);
                //_pButton->UpdateWindow();

                button_position_left_start += _button_width + wordButtonSpacing;

                pyWord = IMEGetStringByIndex(++curIndex);

            }
        }
    }
    UpdateWindow();
    DebugPrintf(DLC_UISOFTKEYBOARDIME, "UISoftKeyboardIME::PYWordsPageUpDown()  End");
}


void UISoftKeyboardIME::HandlePYWordsButton()
{
    PYWordsPageUpDown( true );
    return;
}

void UISoftKeyboardIME::UpdateShiftKey(INPUTMODE oldMode, INPUTMODE newMode)
{
    if (oldMode != newMode)
    {
        SPtr<ITpImage> newImg = ImageManager::GetImage(IMAGE_IME_KEY_OBLONG);
        SPtr<ITpImage> newImgP = ImageManager::GetImage(IMAGE_IME_KEY_OBLONG_PRESSED);
        const char* pText = s_null;
        if (newMode & IUIIME_NUMBER)
        {
            pText = s_symbol;
        }
        else if (newMode & IUIIME_SYMBOL)
        {
            pText = s_number;
        }
        else if (newMode & IUIIME_UPPER)
        {
            newImg = ImageManager::GetImage(IMAGE_IME_KEY_LOWER_SHIFT);
            newImgP = ImageManager::GetImage(IMAGE_IME_KEY_LOWER_SHIFTP);
        }
        else
        {
            newImg = ImageManager::GetImage(IMAGE_IME_KEY_UPPER_SHIFT);
            newImgP = ImageManager::GetImage(IMAGE_IME_KEY_UPPER_SHIFTP);
        }
        m_KeyboardButton[SHIFT_BTN_INDEX].SetBackground(newImg);
        m_KeyboardButton[SHIFT_BTN_INDEX].SetFocusedBackground(newImgP);
        m_KeyboardButton[SHIFT_BTN_INDEX].SetText(pText);
    }
}

void UISoftKeyboardIME::UpdateABCOrNumKey(INPUTMODE oldMode, INPUTMODE newMode)
{
    if (oldMode != newMode)
    {
        SPtr<ITpImage> newImg = ImageManager::GetImage(IMAGE_IME_KEY_OBLONG);
        SPtr<ITpImage> newImgP = ImageManager::GetImage(IMAGE_IME_KEY_OBLONG_PRESSED);
        if ((newMode & IUIIME_NUMBER) || (newMode & IUIIME_SYMBOL))
        {
            m_KeyboardButton[NUMBER_BTN_INDEX].SetText(s_abc);
        }
        else
        {
            m_KeyboardButton[NUMBER_BTN_INDEX].SetText(s_number);
        }
        m_KeyboardButton[NUMBER_BTN_INDEX].SetBackground(newImg);
        m_KeyboardButton[NUMBER_BTN_INDEX].SetFocusedBackground(newImgP);
    }
}

void UISoftKeyboardIME::UpdatePageNameKey(INPUTMODE oldMode, INPUTMODE newMode)
{
    if (oldMode != newMode)
    {
        SPtr<ITpImage> newImg = ImageManager::GetImage(IMAGE_IME_KEY_SQUARE);
        SPtr<ITpImage> newImgP = ImageManager::GetImage(IMAGE_IME_KEY_SQUARE_PRESSED);
        if (newMode & IUIIME_CHINESE)
        {
            newImg = ImageManager::GetImage(IMAGE_IME_KEY_TO_PIN);
            newImgP = ImageManager::GetImage(IMAGE_IME_KEY_TO_PINP);
        }
        else
        {
            newImg = ImageManager::GetImage(IMAGE_IME_KEY_TO_EN);
            newImgP = ImageManager::GetImage(IMAGE_IME_KEY_TO_ENP);
        }
        m_KeyboardButton[LANGUAGE_INDEX].SetBackground(newImg);
        m_KeyboardButton[LANGUAGE_INDEX].SetFocusedBackground(newImgP);
    }
}

void UISoftKeyboardIME::SwitchMode(INPUTMODE oldMode, INPUTMODE newMode)
{
    if (oldMode == newMode)
    {
        return;
    }

    ClearPinYinData();
    IMEReset();
    m_pyLabel.SetText("");
    const int curKeyTableIndex = GetKeyTableIndex(newMode);
    for(int i =0; i< INPUT_BTN_PER_PAGE; i++)
    {
        m_KeyboardButton[i].SetText(s_keyTables[curKeyTableIndex][i]);
    }

    UpdateShiftKey(oldMode, newMode);
    UpdateABCOrNumKey(oldMode, newMode);
    UpdatePageNameKey(oldMode, newMode);
    m_inputMode = newMode;
    if ((IUIIME_CHINESE_LOWER == oldMode) || (IUIIME_CHINESE_LOWER == newMode))
    {
        const bool isChineseMode = IsChineseMode();
        m_KeyboardButton[34].SetVisible(isChineseMode);
        m_KeyboardButton[35].SetVisible(isChineseMode);
        m_pyLabel.SetVisible(isChineseMode);
        Layout();
        RedrawWindow();
    }
}

eProcessResult UISoftKeyboardIME::IMEDeleteLastInput()
{
    return IMEInputKey(8); // 8 is ascii code for back space
}

std::string UISoftKeyboardIME::IMEGetInputString()
{	
    return IMEManager::GetActiveIME()->GetInputString();
}

std::string UISoftKeyboardIME::IMEGetResultString()
{	
    return IMEManager::GetActiveIME()->GetResultString();
}

std::string UISoftKeyboardIME::IMEGetStringByIndex(int index)
{	
    return IMEManager::GetActiveIME()->GetStringByIndex(index);
}

eProcessResult UISoftKeyboardIME::IMEInputKey(char code)
{
    return IMEManager::GetActiveIME()->ProcessKeys(code);
}

eProcessResult UISoftKeyboardIME::IMESelectIndex(int index)
{
    return IMEManager::GetActiveIME()->SelectIndex(index);
}

void  UISoftKeyboardIME::IMEReset()
{
    IMEManager::GetActiveIME()->Reset();
}
bool UISoftKeyboardIME::OnChildLongPress(UIWindow* child)
{
#ifdef KINDLE_FOR_TOUCH
    if (child == m_backspaceButton)
    {
        if (IsChineseMode() && !IMEGetInputString().empty())
        {
            ClearPinYinData();
            UpdateWindow();
        }
        else if (m_pTextBox)
        {
            m_pTextBox->ClearText();
        }
    }
#endif
    return true;
}
