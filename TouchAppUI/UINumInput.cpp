#include "TouchAppUI/UINumInput.h"
#include "interface.h"
#include "Utility.h"
#include "GUI/CTpGraphics.h"
#include "I18n/StringManager.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "CommonUI/UIUtility.h"

using namespace WindowsMetrics;

UINumInput::UINumInput(UIContainer* _pParent)
    : UIDialog(_pParent)
    , m_iInputNum(0)
{
    Init();
}

UINumInput::~UINumInput()
{
}

void UINumInput::Init()
{
    DebugPrintf(DLC_UINUMINPUT, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    const int fontSize20 = GetWindowFontSize(FontSize20Index);
    SPtr<ITpImage> imgBtnSquare = ImageManager::GetImage(IMAGE_IME_KEY_SQUARE);
    SPtr<ITpImage> imgBtnSquareP = ImageManager::GetImage(IMAGE_IME_KEY_SQUARE_PRESSED);
    SPtr<ITpImage> imgDel = ImageManager::GetImage(IMAGE_IME_KEY_DEL);
    SPtr<ITpImage> imgDelP = ImageManager::GetImage(IMAGE_IME_KEY_DELP);
    SPtr<ITpImage> imgEnter = ImageManager::GetImage(IMAGE_IME_KEY_ENTER);
    SPtr<ITpImage> imgEnterP = ImageManager::GetImage(IMAGE_IME_KEY_ENTERP);
    m_KeyboardButton[0].Initialize(ID_BTN_TOUCH_KEYBAORD_NUMBER_0, "0", fontSize20, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[1].Initialize(ID_BTN_TOUCH_KEYBAORD_NUMBER_1, "1", fontSize20, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[2].Initialize(ID_BTN_TOUCH_KEYBAORD_NUMBER_2, "2", fontSize20, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[3].Initialize(ID_BTN_TOUCH_KEYBAORD_NUMBER_3, "3", fontSize20, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[4].Initialize(ID_BTN_TOUCH_KEYBAORD_NUMBER_4, "4", fontSize20, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[5].Initialize(ID_KEYBOARD_BACKSPACE, "", fontSize20, imgDel, imgDelP);
    m_KeyboardButton[6].Initialize(ID_BTN_TOUCH_KEYBAORD_NUMBER_5, "5", fontSize20, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[7].Initialize(ID_BTN_TOUCH_KEYBAORD_NUMBER_6, "6", fontSize20, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[8].Initialize(ID_BTN_TOUCH_KEYBAORD_NUMBER_7, "7", fontSize20, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[9].Initialize(ID_BTN_TOUCH_KEYBAORD_NUMBER_8, "8", fontSize20, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[10].Initialize(ID_BTN_TOUCH_KEYBAORD_NUMBER_9, "9", fontSize20, imgBtnSquare, imgBtnSquareP);
    m_KeyboardButton[11].Initialize(ID_KEYBOARD_ENTER, "",  fontSize20, imgEnter, imgEnterP);

    m_txtInput.SetForeColor(ColorManager::knBlack);
    m_txtInput.SetBackTransparent(FALSE);
    m_txtInput.SetFontSize(fontSize20);
    m_txtInput.SetAlign(ALIGN_LEFT);

    m_btnCancel.SetId(IDCANCEL);
    SPtr<ITpImage> pImgClose = ImageManager::GetImage(IMAGE_TOUCH_BTN_CLOSE);
    if (pImgClose)
    {
        m_btnCancel.SetIcon(pImgClose, UIButton::ICON_TOP);
        m_btnCancel.SetMinSize(dkSize(pImgClose->GetWidth(), pImgClose->GetHeight()));
    }
    m_btnCancel.ShowBorder(false);

    for (int i = 0; i < 12; i++)
    {
        m_KeyboardButton[i].SetUpdateWindowFlag(PAINT_FLAG_DU);
        AppendChild(&m_KeyboardButton[i]);
    }
    AppendChild(&m_txtInput);
    AppendChild(&m_btnCancel);
    DebugPrintf(DLC_UINUMINPUT, "%s, %d, %s, %s END", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}
    
void UINumInput::MoveWindow(int _iLeft, int _iTop, int _iWidth, int _iHeight)
{
    DebugPrintf(DLC_UINUMINPUT, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    UIWindow::MoveWindow(_iLeft, _iTop, _iWidth, _iHeight);

    if (!m_windowSizer)
    {
        const int lineBtnCount = 6;
        const int lineNum = 2;
        const int keyboardSpacing = GetWindowMetrics(UIElementLargeSpacingIndex);
        const int elementSpacing = GetWindowMetrics(UIElementSpacingIndex);
        
        UISizer* pMainSizer = new UIBoxSizer(dkVERTICAL);
        UISizer* pTitleSizer = new UIBoxSizer(dkHORIZONTAL);
        UISizer* pLine0Sizer = new UIBoxSizer(dkHORIZONTAL);
        UISizer* pLine1Sizer = new UIBoxSizer(dkHORIZONTAL);
        if (pMainSizer && pTitleSizer && pLine0Sizer && pLine1Sizer)
        {
            pTitleSizer->Add(&m_txtInput, UISizerFlags(1).Align(dkALIGN_CENTRE_VERTICAL));
            pTitleSizer->Add(&m_btnCancel, UISizerFlags().Align(dkALIGN_CENTRE_VERTICAL));

            const int keyboardSize = GetWindowMetrics(UISoftKeyboardIMEKeyboardSizeIndex);
            const int keyboardWidth = GetWindowMetrics(UISoftKeyboardIMEKeyboardWidthIndex);
            for (int i = 0; i < lineBtnCount; i++)
            {
                m_KeyboardButton[i].SetMinSize(dkSize(keyboardSize, keyboardSize));
                pLine0Sizer->Add(&m_KeyboardButton[i], UISizerFlags().Border(dkLEFT | dkRIGHT, elementSpacing));
            }
            for (int i = 0; i < lineBtnCount; i++)
            {
                m_KeyboardButton[lineBtnCount + i].SetMinSize(dkSize(keyboardSize, keyboardSize));
                pLine1Sizer->Add(&m_KeyboardButton[lineBtnCount + i], UISizerFlags().Border(dkLEFT | dkRIGHT, elementSpacing));
            }
            m_KeyboardButton[lineBtnCount - 1].SetMinWidth(keyboardWidth);
            m_KeyboardButton[(lineBtnCount * lineNum) - 1].SetMinWidth(keyboardWidth);

            pMainSizer->Add(pTitleSizer, UISizerFlags().Expand().Border(dkALL, keyboardSpacing));
            pMainSizer->AddSpacer(elementSpacing);
            pMainSizer->Add(pLine0Sizer, UISizerFlags().Align(dkALIGN_CENTRE).Border(dkLEFT | dkRIGHT, keyboardSpacing));
            pMainSizer->AddSpacer(keyboardSpacing);
            pMainSizer->Add(pLine1Sizer, UISizerFlags().Align(dkALIGN_CENTRE).Border(dkLEFT | dkRIGHT, keyboardSpacing));
            pMainSizer->AddSpacer(keyboardSpacing);

            SetSizer(pMainSizer);
            Layout();
        }
        else
        {
            SafeDeletePointer(pMainSizer);
            SafeDeletePointer(pTitleSizer);
            SafeDeletePointer(pLine0Sizer);
        }
    }
    DebugPrintf(DLC_UINUMINPUT, "%s, %d, %s, %s END", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

void UINumInput::SetInitNum(int _iInitNum) 
{
    DebugPrintf(DLC_UINUMINPUT, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    m_iInputNum = _iInitNum;
    char strRepaint[128] = {0};
    sprintf(strRepaint, "%02d", _iInitNum);
    m_txtInput.SetText(strRepaint);
    DebugPrintf(DLC_UINUMINPUT, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

int UINumInput::GetTotalHeight() const
{
    DebugPrintf(DLC_UINUMINPUT, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    const int keyboardSize = GetWindowMetrics(UISoftKeyboardIMEKeyboardSizeIndex);
    const int keyboardSpacing = GetWindowMetrics(UIElementLargeSpacingIndex);
    return (keyboardSize * 3) + (keyboardSpacing * 4);
}

int UINumInput::GetTotalWidth() const
{
    DebugPrintf(DLC_UINUMINPUT, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    const int keyboardSize = GetWindowMetrics(UISoftKeyboardIMEKeyboardSizeIndex);
    const int keyboardWidth = GetWindowMetrics(UISoftKeyboardIMEKeyboardWidthIndex);
    const int keyboardSpacing = GetWindowMetrics(UIElementLargeSpacingIndex);
    const int elementSpacing = GetWindowMetrics(UIElementSpacingIndex);
    return (keyboardSize * 5) + keyboardWidth + (keyboardSpacing * 2) + (elementSpacing * 12);
}

void UINumInput::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    DebugPrintf(DLC_UINUMINPUT, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    switch(dwCmdId)
    {
    case ID_BTN_TOUCH_KEYBAORD_NUMBER_0:
    case ID_BTN_TOUCH_KEYBAORD_NUMBER_1:
    case ID_BTN_TOUCH_KEYBAORD_NUMBER_2:
    case ID_BTN_TOUCH_KEYBAORD_NUMBER_3:
    case ID_BTN_TOUCH_KEYBAORD_NUMBER_4:
    case ID_BTN_TOUCH_KEYBAORD_NUMBER_5:
    case ID_BTN_TOUCH_KEYBAORD_NUMBER_6:
    case ID_BTN_TOUCH_KEYBAORD_NUMBER_7:
    case ID_BTN_TOUCH_KEYBAORD_NUMBER_8:
    case ID_BTN_TOUCH_KEYBAORD_NUMBER_9:
        {
            DebugPrintf(DLC_UINUMINPUT, "%s, %d, %s, %s ID_BTN_TOUCH_KEYBAORD_NUMBER_%d", __FILE__,  __LINE__, GetClassName(), __FUNCTION__, (dwCmdId - ID_BTN_TOUCH_KEYBAORD_NUMBER_0));
            if (m_iInputNum >= 10)
                m_iInputNum %= 10;
            
            m_iInputNum *= 10;
            m_iInputNum += (dwCmdId - ID_BTN_TOUCH_KEYBAORD_NUMBER_0);
            
            char strRepaint[128] = {0};
            sprintf(strRepaint, "%02d", m_iInputNum);
            
            m_txtInput.SetText(strRepaint);
            m_txtInput.UpdateWindow();
        }
        break;
    case ID_KEYBOARD_BACKSPACE:
        {
            DebugPrintf(DLC_UINUMINPUT, "%s, %d, %s, %s ID_KEYBOARD_BACKSPACE", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            
            m_iInputNum /= 10;
            char strRepaint[128] = {0};
            sprintf(strRepaint, "%02d", m_iInputNum);
            m_txtInput.SetText(strRepaint);
            m_txtInput.UpdateWindow();
        }
        break;
    case ID_KEYBOARD_ENTER:
        {
            DebugPrintf(DLC_UINUMINPUT, "%s, %d, %s, %s ID_KEYBOARD_ENTER", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            EndDialog(1);
        }
        break;
    case IDCANCEL:
        {
            DebugPrintf(DLC_UINUMINPUT, "%s, %d, %s, %s IDCANCEL", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            EndDialog(0);
        }
        break;
    default:
        break;
    }
    DebugPrintf(DLC_UINUMINPUT, "%s, %d, %s, %s END", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
}

HRESULT UINumInput::Draw(DK_IMAGE image)
{
    DebugPrintf(DLC_UINUMINPUT, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (!m_bIsVisible)
    {
        return S_OK;
    }

    if (image.pbData == NULL)
    {
        return E_FAIL;
    }
    HRESULT hr(S_OK);
    DK_IMAGE imgSelf;
    DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
    RTN_HR_IF_FAILED(CropImage(
        image,
        rcSelf,
        &imgSelf
        ));

    CTpGraphics grf(imgSelf);

    RTN_HR_IF_FAILED(UIUtility::DrawBorder(imgSelf, 0, 0, 0, 0));
    UISizerItem* pItem = m_windowSizer->GetItem((size_t)0);
    if (pItem)
    {
        const int borderLine = GetWindowMetrics(UIDialogBorderLineIndex);
        RTN_HR_IF_FAILED(grf.DrawLine(0, pItem->GetRect().GetBottom() + borderLine, m_iWidth, borderLine, SOLID_STROKE));
    }
    
    int iSize = GetChildrenCount();
    for (int i = 0; i < iSize; i++)
    {
        UIWindow* pWin = GetChildByIndex(i);
        if (pWin)
        {
            DebugPrintf(DLC_UINUMINPUT, "%s: Drawing Child %d / %d : %s ...", GetClassName(), i, iSize, pWin->GetClassName());
            RTN_HR_IF_FAILED(pWin->Draw(imgSelf));
        }
        else
        {
            DebugPrintf(DLC_ERROR, "%s: FAILED to get child %d/%d !!!", GetClassName(), i, iSize);
        }
    }
    
    DebugPrintf(DLC_UINUMINPUT, "%s, %d, %s, %s END", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    return hr;
}
