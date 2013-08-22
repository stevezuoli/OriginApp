#include "interface.h"
#include "Utility.h"
#include "GUI/CTpGraphics.h"
#include "GUI/FontManager.h"
#include "Utility/ImageManager.h"
#include "Utility/ColorManager.h"
#include "GUI/UITextBox.h"                      
#include "GUI/EventArgs.h"                      
#include "GUI/MessageEventArgs.h"
#include "CommonUI/UISoftKeyboardIME.h"
#include "CommonUI/UIUtility.h"
#include "drivers/DeviceInfo.h"
#include "GUI/UIContainer.h"
#include "Framework/CDisplay.h"
#include "Utility/TimerThread.h"
#include "Utility/SystemUtil.h"
#include "Utility/EncodeUtil.h"
#include "Common/WindowsMetrics.h"
#include "KernelDef.h"
#include "CommonUI/UIIMEManager.h"

using namespace dk::utility;
using namespace DkFormat;
using namespace WindowsMetrics;

#define TEXTBOX_DELETE          1001
#define TEXTBOX_INSERT          1002
#define TEXTBOX_SET_TEXT        1003
#define TEXTBOX_MOVE_LEFT       2002
#define TEXTBOX_MOVE_RIGHT      2003
#define TEXTBOX_RESIZE          3001
#define TEXTBOX_FONT_RESIZE     3002

void wstrnins(wchar_t* szDest,int nIndex,int nDestArraySize,const wchar_t* strSrc,int nLen)
{
    if( 0 == nLen || szDest == NULL || strSrc == NULL || nIndex + nLen >= nDestArraySize)
    {
        return;
    }
    int strlencount = 0;
    wchar_t *tmp=szDest;
    while(*szDest!=0)
    {
        strlencount++;
        szDest++;
    }
    szDest = tmp;
    int i=0;
    for(i=strlencount ;i>=nIndex;i--)
    {
        szDest[i+nLen]=szDest[i];
    }
    for(i=0 ;i<nLen;i++)
    {
        szDest[i+nIndex]=strSrc[i];
    }
}

void wstrRemovChar(wchar_t * szDest, int iIndex,  int iCount)
{
    int iLen = wcslen(szDest);
    int i=0;
    if(iIndex < 0 || iCount == 0)
    {
        return ;
    }

    if(iLen <= iIndex)
    {
        return;
    }

    for(i = iIndex ; i <= iLen - iCount; i++)//已经拷贝 \0
    {
        szDest[i] = szDest[i+ iCount];
    }
}

#ifdef KINDLE_FOR_TOUCH
BOOL NormalCallBack(const wchar_t* strText, INT32 iStrLen)
{
    return true;
}

BOOL NumberCallBack(const wchar_t* strText,INT32 iStrLen)
{
    const wchar_t *pStr = strText;
    for ( int i=0; i<iStrLen; ++i, pStr++)
        if (*pStr<'0' || *pStr>'9')
            return false;

    return true;
}

BOOL PercentageCallBack(const wchar_t* strText,INT32 iStrLen)
{
    const wchar_t *pStr = strText;
    int decimalPoint = 0;
    for ( int i=0; i<iStrLen; ++i, pStr++)
    {
        if (*pStr<'0' || *pStr>'9')
        {
            if (*pStr == '.')
            {
                if (decimalPoint==0)
                {
                    decimalPoint++;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
    }

    return true;
}
#else
static BOOL ConvertNum(wchar_t* keychar)
{
    switch(*keychar)
    {
        case L'0':
        case L'p':
        case L'P':
            *keychar = L'0';
            break;
        case L'1':
        case L'q':
        case L'Q':
            *keychar = L'1';
            break;
        case L'2':
        case L'w':
        case L'W':
            *keychar = L'2';
            break;
        case L'3':
        case L'e':
        case L'E':
            *keychar = L'3';
            break;
        case L'4':
        case L'r':
        case L'R':
            *keychar = L'4';
            break;
        case L'5':
        case L't':
        case L'T':
            *keychar = L'5';
            break;
        case L'6':
        case L'y':
        case L'Y':
            *keychar = L'6';
            break;
        case L'7':
        case L'u':
        case L'U':
            *keychar = L'7';
            break;
        case L'8':
        case L'i':
        case L'I':
            *keychar = L'8';
            break;
        case L'9':
        case L'o':
        case L'O':
            *keychar = L'9';
            break;
        default:
            return FALSE;
        }
    return TRUE;
}


BOOL NormalCallBack(const wchar_t* strText, wchar_t* keychar)
{
    return TRUE;
}

BOOL NumberCallBack(const wchar_t* strText, wchar_t* keychar)
{
    return ConvertNum(keychar);
}

BOOL PercentageCallBack(const wchar_t* strText, wchar_t* keychar)
{
    if((*keychar) == '.' || ConvertNum(keychar))
    {
        int len = wcslen(strText);
        int spot = -1;
        for (int i=0; i < len; i++)
        {
            if(strText[i] == L'.')
            {
                spot = i;
            }
        }
        if((*keychar) == '.')
        {
            if(spot != -1)
            {
                return FALSE;
            }
        }
        return TRUE;
    }
    return FALSE;
}
#endif

void UITextBox::SetFont(INT32 iFace, INT32 iStyle, INT32 iSize)
{
    m_ifontAttr.SetFontAttributes(iFace, iStyle, iSize);
    ITpFont* pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, m_iFontColor);
    if (pFont)
    {
        m_minHeight = m_nBorderLinePixel * 2 + m_nMarginTop + m_nMarginBottom + pFont->GetHeight();
    }
    OnChange(TEXTBOX_FONT_RESIZE);
}

UITextBox::~UITextBox()
{
    TimerThread::GetInstance()->CancelTimer(UITextBox::EchoAsAsterisksInstead, (void*)(this));
}

HRESULT UITextBox::UpdateFocus()
{
    return UpdateWindow();
}

void UITextBox::SetTextBoxStyle(TextBoxStyle eTextBoxStyle)
{
    UIAbstractTextEdit::SetTextBoxStyle(eTextBoxStyle);
    if(eTextBoxStyle == PERCENTAGE)
    {
        KeyPressCallBack = PercentageCallBack;
    }
    else if(eTextBoxStyle == NUMBER_ONLY)
    {
        KeyPressCallBack = NumberCallBack;
    }
    else if (eTextBoxStyle == ENGLISH_NORMAL)
    {
        KeyPressCallBack = NormalCallBack;
    }
    else
    {
        KeyPressCallBack = NormalCallBack;
    }
}
UITextBox::EchoMode UITextBox::GetEchoMode() const
{
    return m_echoMode;
}

void UITextBox::SetEchoMode(EchoMode echoMode)
{
    if (echoMode == m_echoMode)
        return;
    if (echoMode == Normal || echoMode == Password)
    {
        if (m_echoMode == PasswordEchoOnEdit)
            TimerThread::GetInstance()->CancelTimer(UITextBox::EchoAsAsterisksInstead, (void*)(this));
    }
    else if (echoMode == PasswordEchoOnEdit)
    {
        //TimerThread::GetInstance()->AddTimer(UITextBox::EchoAsAsterisksInstead, (void*)(this), SystemUtil::GetUpdateTimeInMs() + 2*1000, false, false, 0);
    }
    m_echoMode = echoMode;
}

void UITextBox::CreateEchoDataFromOriginalData()
{
    unsigned int dataLen = wcslen(m_strData);
    if (dataLen > TEXTBOX_DATA_MAX_LEN)
        dataLen = TEXTBOX_DATA_MAX_LEN;
    for ( unsigned int i = 0; i < dataLen; ++i)
    {
        m_strEchoData[i] = L'*';
    }
    m_strEchoData[dataLen + 1] = L'\0';
}

void UITextBox::EchoAsAsterisksInstead(void* dummy)
{
    DebugPrintf(DLC_DIAGNOSTIC, "UITextBox::EchoAsAsterisksInstead");
    UITextBox* dummyBox = static_cast<UITextBox*>(dummy);
    if (dummyBox)
    {
        dummyBox->CreateEchoDataFromOriginalData();
        dummyBox->OnChange(TEXTBOX_INSERT);
        //dummyBox->UpdateWindow();
    }
}

void UITextBox::SetNumberOnlyMode()
{
    SetTextBoxStyle(NUMBER_ONLY);
}

FuncKeyPress UITextBox::GetFuncCallBack()
{
    return KeyPressCallBack;
}

UITextBox::UITextBox()
    : UIAbstractTextEdit()
{
    Init();
}

UITextBox::UITextBox(UIContainer* pParent, DWORD rstrId)
    : UIAbstractTextEdit(pParent, rstrId)
{
    if (pParent != NULL)
    {
        pParent->AppendChild(this);
    }
    Init();
}

void UITextBox::Init()
{
    m_fIsSelectAll      = FALSE;
    m_bIsDisposed       = FALSE;
    m_showBorder = true;
    m_fireEvent = true;

    m_iStartIndex       = 0;
    m_iStartPosition    = 0;
    m_iEndIndex         = 0;


    m_iDataIndex            = 0;
    m_iDataLen              = 0;
    m_iMaxDataLen           = TEXTBOX_DATA_MAX_LEN;
    for(UINT32 i=0;i<sizeof(m_rgiALLPosition) / sizeof(m_rgiALLPosition[0]);i++)
    {
        m_rgiALLPosition[i] = 0;
    }
    memset(m_strData, 0, sizeof(m_strData));
    memset(m_strTip, 0, sizeof(m_strTip));
    Change_UpdatePosArr();
    Change_UpdateIndexs();

    m_ifontAttr.m_iSize = GetWindowFontSize(UITextBoxIndex);;
    m_nBorderLinePixel      = GetWindowMetrics(UITextBoxBorderLinePixelIndex);

    ITpFont* pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, m_iFontColor);
    if (pFont)
    {
        m_minHeight = m_nBorderLinePixel * 2 + m_nMarginTop + m_nMarginBottom + pFont->GetHeight();
    }
    KeyPressCallBack = NormalCallBack;
    m_echoMode = Normal;
}

void UITextBox::SetImage(SPtr<ITpImage> _spImage)
{
    m_spImage = _spImage;
}

void UITextBox::SetImage(int imageID, int focusedImageID)
{
    m_spImage = ImageManager::GetImage(imageID);
    m_spFocusedImage = ImageManager::GetImage(focusedImageID);
}

HRESULT UITextBox::Draw(DK_IMAGE image)
{
    //初始化绘图工具
    if (!m_bIsVisible)
    {
        return S_OK;
    }
    if (image.pbData == NULL)
    {
        return E_FAIL;
    }
    HRESULT hr(S_OK);
    DK_IMAGE img1;
    DK_RECT rect={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
    RTN_HR_IF_FAILED(CropImage(image,rect,&img1));
    CTpGraphics g1(img1);//主要画布
    RTN_HR_IF_FAILED(g1.EraserBackGround());

    //背景图边框
    int iLeft = 0, iTop = 0;
    if (m_showBorder)
    {
#ifdef KINDLE_FOR_TOUCH
        RTN_HR_IF_FAILED(UIUtility::DrawBorder(img1, 0, 0, 0, 0, 0, true, true));
#else
        SPtr<ITpImage> spImg = ImageManager::GetImage(IMAGE_TEXTBOX_BACK);
        if (spImg)
        {
            g1.DrawImageStretchBlend(spImg.Get(), iLeft, iTop, m_iWidth, m_iHeight,1,10);
        }
#endif
    }

    ITpImage* pImage = m_spImage.Get();
    if (IsFocus() && m_spFocusedImage)
    {
        pImage = m_spFocusedImage.Get();
    }
    if (pImage)
    {
        int iLeftOffset = (m_iHeight > pImage->GetHeight()) ? (m_iHeight - pImage->GetHeight()) >> 1 : 0;
        int iTopOffset  = (m_iHeight > pImage->GetWidth()) ? (m_iHeight - pImage->GetWidth()) >> 1 : 0;
        iLeft += iLeftOffset;
        iTop +=  iTopOffset;
        g1.DrawImageBlend(pImage, iLeft, iTop, 0, 0, pImage->GetWidth(), pImage->GetHeight());
        iLeft += pImage->GetWidth() + iLeftOffset;
    }


    //画字
    if(m_iDataLen > 0)
    {
        DK_IMAGE imgDrawChar;
        DK_RECT rcDrawChar={m_iLeft + m_nMarginLeft + m_nBorderLinePixel + iLeft,
                          m_iTop + m_nMarginTop + m_nBorderLinePixel, 
                          m_iLeft + m_iWidth - m_nBorderLinePixel, 
                          m_iTop + m_iHeight - m_nMarginBottom - m_nBorderLinePixel };        

        HRESULT hr(S_OK);        
        RTN_HR_IF_FAILED(CropImage(
                        image,
                        rcDrawChar,
                        &imgDrawChar
                        ));

        if(!imgDrawChar.pbData)
        {
            return E_FAIL;
        }

        CTpGraphics g2(imgDrawChar);//绘字画布

        wchar_t wcharTmp[TEXTBOX_DATA_MAX_LEN];
        if (m_echoMode == Normal)
        {
            memcpy(wcharTmp , m_strData + m_iStartIndex , (m_iEndIndex - m_iStartIndex + 1) * sizeof(wcharTmp[0]));
        }
        else if (m_echoMode == Password || m_echoMode == PasswordEchoOnEdit)
        {
            memcpy(wcharTmp , m_strEchoData + m_iStartIndex , (m_iEndIndex - m_iStartIndex + 1) * sizeof(wcharTmp[0]));
        }
        wcharTmp[m_iEndIndex - m_iStartIndex + 1]=0;

        if(m_fIsSelectAll && m_bIsFocus)
        {
            m_iFontColor = ColorManager::knWhite;
        }
        else if (!IsEnable())
        {
            m_iFontColor = ColorManager::GetColor(COLOR_SCROLLBAR_BACK);
        }
        else
        {
            m_iFontColor = ColorManager::knBlack;
        }
        ITpFont* pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, m_iFontColor);
        if (pFont)
        {
            const int maxTxtHeight = (m_iHeight -(m_nBorderLinePixel * 2 + m_nMarginTop + m_nMarginBottom));
            const int iTxtTop = (maxTxtHeight > pFont->GetHeight()) ? ((maxTxtHeight - pFont->GetHeight()) >> 1) : 0;
            if(m_fIsSelectAll)
            {
                int width = pFont->StringWidth(wcharTmp,m_iEndIndex - m_iStartIndex + 1);
                if(m_bIsFocus)
                {
                    g2.FillRect(0,iTxtTop,width,imgDrawChar.iHeight,ColorManager::knBlack);
                }
                else
                {
                    g2.FillRect(0,iTxtTop,width,imgDrawChar.iHeight,ColorManager::GetColor(COLOR_SCROLLBAR_SLIDER));
                }
            }
            g2.DrawStringUnicode(wcharTmp,0,iTxtTop,pFont,FALSE);
        }
        
        if (ColorManager::knBlack == m_iFontColor && !m_fIsSelectAll)
        {
            g2.Do2Gray(0,iTop,imgDrawChar.iWidth,imgDrawChar.iHeight);
        }
    }
    else if (m_strTip[0])
    {
        m_iFontColor = ColorManager::GetColor(COLOR_SCROLLBAR_BACK);
        ITpFont* pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, m_iFontColor);
        if (pFont)
        {
            const int iTxtTop = (m_iHeight > pFont->GetHeight()) ? ((m_iHeight - pFont->GetHeight()) >> 1) : 0;
            g1.DrawStringUnicode(m_strTip,m_nBorderLinePixel + m_nMarginLeft + iLeft,  iTxtTop,pFont,FALSE);
        }
    }
    //绘制光标
    if(IsEnable() && IsFocus())
    {
        int x = m_rgiALLPosition[m_iDataIndex ] - m_iStartPosition  + m_nBorderLinePixel + m_nMarginLeft + iLeft;
        int y = m_nBorderLinePixel + m_nMarginTop;
        int w = 3;
        int h = m_iHeight - m_nBorderLinePixel * 2 - m_nMarginTop - m_nMarginBottom;
        g1.DrawLine(x,y,w,h,SOLID_STROKE);
    }
    return hr;
}

BOOL UITextBox::OnKeyPress(INT32 iKeyCode)
{
#ifdef KINDLE_FOR_TOUCH
    return TRUE;
#else
    UIIME* pIME = UIIMEManager::GetCurrentIME();
    if (pIME && pIME->IsShow() && !pIME->OnKeyPress(iKeyCode))
    {
        return FALSE;
    }

    if(IsActionKey(iKeyCode))
    {
        return OnActionPress(iKeyCode);
    }
    else
    {
        char key = KeyCode2Char(iKeyCode);
        wchar_t curChar = key;
        if(-1 == InsertText(&curChar,1))
        {
            return TRUE;
        }
    }

    return FALSE;
#endif
}


BOOL UITextBox::OnDeletePress() 
{
#ifdef KINDLE_FOR_TOUCH
    return TRUE;
#endif
    if(m_fIsSelectAll && m_iDataLen)
    {
        ClearText();
        return FALSE;
    }
    if(0 == m_iDataIndex)
    {
        return FALSE;
    }
    wstrRemovChar(
        m_strData,
        m_iDataIndex - 1,
        1);
    m_iDataLen -= 1;
    m_iDataIndex -= 1;
    OnChange(TEXTBOX_DELETE);
    return FALSE;
}

BOOL UITextBox::OnEnterPress()
{
    OnClick();
    OnEvent(UIEvent(COMMAND_EVENT, this, GetId()));
    return FALSE;
}

BOOL UITextBox::OnDirectionPress(INT32 iKeyCode)
{
    BOOL returnValue;
    switch(iKeyCode)
    {
    case KEY_LEFT:
        if(m_fIsSelectAll)
        {
            m_iDataIndex = 0;
            m_fIsSelectAll = FALSE;
            returnValue = FALSE;
            OnChange(TEXTBOX_MOVE_LEFT);
        }
        else if(0 == m_iDataIndex)
        {
            returnValue = TRUE;
        }
        else
        {
            m_iDataIndex --;
            returnValue = FALSE;
            OnChange(TEXTBOX_MOVE_LEFT);
        }
        break;
    case KEY_RIGHT:
        if(m_fIsSelectAll)
        {
            m_iDataIndex = m_iDataLen;
            m_fIsSelectAll = FALSE;
            returnValue = FALSE;
            OnChange(TEXTBOX_MOVE_RIGHT);
        }
        else 
            if(m_iDataIndex == m_iDataLen)
            {
                returnValue =  TRUE;
            }
            else
            {
                m_iDataIndex ++;
                returnValue = FALSE;
                OnChange(TEXTBOX_MOVE_RIGHT);
            }
            break;
    case KEY_UP:
    case KEY_DOWN:
    default:
        returnValue = TRUE;
        break;
    }
    return returnValue;
}


BOOL UITextBox::OnActionPress(INT32 iKeyCode)
{
    BOOL retValue = TRUE;
    switch(iKeyCode)
    {
        case KEY_DEL:
            retValue = OnDeletePress();
            break;
        case KEY_ENTER:
            retValue = OnEnterPress();
            break;
        case KEY_LEFT:
        case KEY_RIGHT:
        case KEY_UP:
        case KEY_DOWN:
            retValue = OnDirectionPress(iKeyCode);
            break;
        case KEY_FONT:
            retValue = OnFontPress();
            break;
        case KEY_SYM:
            retValue = OnSymPress();
            break;
    }
    return retValue;
}

INT32 UITextBox::SetTipUTF8(LPCSTR strText,INT32 iStrLen)
{
    if(0 == iStrLen)
    {
        iStrLen = strlen(strText);
    }
    SetTip(
        EncodeUtil::ToWString(std::string(strText, iStrLen).c_str()).c_str());
    return 0;
}


INT32 UITextBox::SetTip(const wchar_t *strText,INT32 iStrLen)
{
    if( 0 == iStrLen)
    {
        iStrLen = wcslen(strText);
    }
    memcpy(m_strTip,strText,iStrLen * sizeof(m_strTip[0]));
    m_strTip[iStrLen] = 0;
    return 1;
}

INT32 UITextBox::SetTextUTF8(LPCSTR strText,INT32 iStrLen)
{
    if(0 == iStrLen)
    {
        iStrLen = strlen(strText);
    }
    SetText(
        EncodeUtil::ToWString(std::string(strText, iStrLen)).c_str());
    return 0;
}

INT32 UITextBox::SetText(const wchar_t *strText,INT32 iStrLen)
{
    if(0 == iStrLen)
    {
        iStrLen = wcslen(strText);
    }

#ifdef KINDLE_FOR_TOUCH
    if (KeyPressCallBack != NULL)
    {
        if (!KeyPressCallBack(strText, iStrLen))
            return 0;
    }
#endif

    if(iStrLen >= m_iMaxDataLen)//需要等号  把末位结束符0留下来
    {
        return 0;
    }
    else
    {
        if(iStrLen > m_iMaxDataLen)
        {
            iStrLen = m_iMaxDataLen;
        }
        memcpy(m_strData , strText, iStrLen * sizeof(m_strData[0]));
        m_strData[iStrLen] = '\0';
        m_iDataIndex = iStrLen;
        m_iDataLen = iStrLen;
        m_iEndIndex = 0;
        m_iStartIndex = 0;
        OnChange(TEXTBOX_SET_TEXT);
        if (m_echoMode == PasswordEchoOnEdit)
        {
            UITextBox::EchoAsAsterisksInstead(this);
        }
        return 1;
    }
    return 0;
}

std::string UITextBox::GetTextUTF8() const
{
    return EncodeUtil::ToString(m_strData);
}

INT32 UITextBox::InsertTextUTF8(LPCSTR strText,INT32 iStrLen)
{
    if(0 == iStrLen)
    {
        iStrLen = strlen(strText);
    }
    std::wstring wText = EncodeUtil::ToWString(
            std::string(strText, iStrLen).c_str());
    InsertText(wText.c_str(), wText.size());
    return 0;
}

INT32 UITextBox::InsertText(const wchar_t *strText,INT32 iStrLen )
{
    if(0 == iStrLen)
    {
        iStrLen = wcslen(strText);
    }
#ifdef KINDLE_FOR_TOUCH
    if (KeyPressCallBack != NULL)
    {
        if (!KeyPressCallBack(strText, iStrLen))
            return 0;
    }
#else
    wchar_t wTmp[2] = {L'\0', L'\0'};
    if(iStrLen == 1)   //当插入单个字符时才进行数据校验
    {
        if(KeyPressCallBack != NULL)
        {
            wTmp[0] = strText[0];
            if(!KeyPressCallBack(m_strData, wTmp))
            {
                return -1; 
            }
            strText = wTmp;
        }
        else
        {
            return 0;
        }
    }
#endif

    if(m_fIsSelectAll)
    {
        return SetText(strText,iStrLen);
    }

    if(m_iDataLen + iStrLen > m_iMaxDataLen)
    {
        return 0;
    }
    else
    {
        if(m_iMaxDataLen  >= m_iDataLen + iStrLen)
        {
            //do nothing
        }
        else
        {
            iStrLen = m_iMaxDataLen - m_iDataLen;
        }
        if (m_echoMode == PasswordEchoOnEdit)
        {
            CreateEchoDataFromOriginalData();
            wstrnins(m_strEchoData ,
                    m_iDataIndex,
                    m_iMaxDataLen + 1,
                    strText,
                    iStrLen);
            TimerThread::GetInstance()->CancelTimer(UITextBox::EchoAsAsterisksInstead, (void*)(this));
            TimerThread::GetInstance()->AddTimer(UITextBox::EchoAsAsterisksInstead, (void*)(this), SystemUtil::GetUpdateTimeInMs() + 2*1000, false, true, 0);
        }
        wstrnins(m_strData ,
            m_iDataIndex,
            m_iMaxDataLen + 1,
            strText,
            iStrLen);
        if (m_echoMode == Password)
        {
            CreateEchoDataFromOriginalData();
        }
        m_iDataIndex += iStrLen;
        m_iDataLen += iStrLen;
        OnChange(TEXTBOX_INSERT);
        return 1;
    }
}

INT32 UITextBox::DeleteChar()
{
    //TODO:@zhangying merge the code to unit
#ifdef KINDLE_FOR_TOUCH
    if (m_iDataLen>0)
    {
      wstrRemovChar(m_strData,m_iDataLen-1,1);
      m_iDataLen--;
      m_iDataIndex = m_iDataLen;
      OnChange(TEXTBOX_DELETE);
    }
#else
    if (m_iDataLen > 0 && m_iDataIndex > 0)
    {
      wstrRemovChar(m_strData, m_iDataIndex-1,1);
      m_iDataLen--;
      m_iDataIndex --;
      OnChange(TEXTBOX_DELETE);
    }
#endif
    return 1;
}

void UITextBox::ShowBorder(bool showBorder) 
{
    m_showBorder = showBorder;
}

void UITextBox::DoFireEvent(bool fireEvent)
{
    m_fireEvent = fireEvent; 
}
void UITextBox::SetMaxDataLen(INT32 iMaxLen)
{
    if(iMaxLen > 0)
    {
        m_iMaxDataLen = iMaxLen;    
    }
}
INT32 UITextBox::ClearText()
{
    m_strData[0]=0;
    m_iDataIndex = 0 ;
    m_iDataLen = 0;
    OnChange(TEXTBOX_SET_TEXT);
    return 1;
}

void UITextBox::OnResize(INT32 iWidth, INT32 iHeight)
{
    UIWindow::OnResize(iWidth, iHeight);
    if(m_iDataLen != 0)
    {
        OnChange(TEXTBOX_RESIZE);
    }
}

void UITextBox::OnChange(INT32 iAction)
{
    if(
        iAction == TEXTBOX_DELETE 
        || iAction == TEXTBOX_INSERT
        || iAction == TEXTBOX_SET_TEXT
        || iAction == TEXTBOX_FONT_RESIZE
        )
    {
        Change_UpdatePosArr();
    }
    Change_UpdateStartPos(iAction);
    Change_UpdateIndexs();

    if(iAction != TEXTBOX_RESIZE && iAction != TEXTBOX_FONT_RESIZE)
    {
        m_fIsSelectAll = FALSE;
    }

    if (m_fireEvent)
    {
        TextEditChangeEventArgs args;
        FireEvent(EventTextEditChange, args);
    }
    UpdateWindow();
}

void UITextBox::Change_UpdatePosArr()
{
    INT32 nLen = m_iDataLen ;
    INT32 *rgnAllPosition = m_rgiALLPosition;
    INT32 i = 0;

    //确定所有光标可能的位置
    rgnAllPosition[0] = 0;
    for(i = 1 ;i <= nLen;i++)
    {
        ITpFont* pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, ColorManager::knBlack);
        if (m_echoMode == Password || m_echoMode == PasswordEchoOnEdit)
        {
            rgnAllPosition[i] = rgnAllPosition[i-1] + pFont->StringWidth((m_strEchoData + i- 1),1);
        }
        else if (m_echoMode == Normal)
        {
            rgnAllPosition[i] = rgnAllPosition[i-1] + pFont->StringWidth((m_strData + i- 1),1);
        }
    }
}
void UITextBox::Change_UpdateIndexs()
{
    INT32 i=0;
    INT32 iStrLen = m_iDataLen ;
    INT32 nStartPosition = m_iStartPosition;
    INT32 *rgnAllPosition = m_rgiALLPosition;
    INT32 nMaxLength = m_iWidth - m_nBorderLinePixel * 2 - m_nMarginLeft - m_nMarginRight ;
    if(nStartPosition <0)
    {
        nStartPosition = 0;
        m_iStartPosition = 0;
    }
    //求开始索引  这里要求使用<= 因为  坐标的位置个数会比字的个数多一个  这里查找的是光标表
    for(i=0 ;i<=iStrLen ;i++)
    {
        if(nStartPosition < rgnAllPosition[i])
        {
            m_iStartIndex = i -1;
            break;
        }
    }
    //找到结束的索引值
    for(i=m_iStartIndex;i<iStrLen;i++)
    {
        m_iEndIndex = i ;
        if(rgnAllPosition[i] > nStartPosition + nMaxLength )
        {
            m_iEndIndex = i ;
            break;
        }
    }
}

void UITextBox::Change_UpdateStartPos(INT32 iAction)
{
    INT32 iMaxWidth = m_iWidth - m_nBorderLinePixel * 2 - m_nMarginLeft - m_nMarginRight;
    if(iMaxWidth < 0)
    {
        iMaxWidth = 0;
    }
    INT32 iIndexPos = m_rgiALLPosition[m_iDataIndex ];
    INT32 iEndPos     = m_rgiALLPosition[m_iDataLen ];

    switch(iAction)
    {
    case TEXTBOX_MOVE_LEFT:
        if(iIndexPos - m_iStartPosition <= (iMaxWidth >> 2))
        {
            m_iStartPosition = iIndexPos - (iMaxWidth >> 2) *3;
        }
        break;
    case TEXTBOX_MOVE_RIGHT:
        if(
            iIndexPos - m_iStartPosition >= iMaxWidth - (iMaxWidth >> 2)  // 超过3/4
            )
        {
            m_iStartPosition = iIndexPos - (iMaxWidth >> 2);
            if ( iEndPos - m_iStartPosition <= iMaxWidth)//如果超过了末尾
            {
                m_iStartPosition = iEndPos - iMaxWidth;//将末尾定位在框尾
            }
        }
        break;
    case TEXTBOX_DELETE:
        if(iIndexPos - m_iStartPosition <= (iMaxWidth >> 2))//小于左侧1/4时候
        {
            //移动到3/4的位置
            m_iStartPosition = iIndexPos -(iMaxWidth >> 2) * 3;
        }
    case TEXTBOX_INSERT:
        if(iIndexPos - m_iStartPosition > iMaxWidth)
        {
            m_iStartPosition = iIndexPos - iMaxWidth ;
        }
        break;
    case TEXTBOX_SET_TEXT:
    case TEXTBOX_RESIZE:
    case TEXTBOX_FONT_RESIZE:
        m_iStartPosition = iIndexPos - iMaxWidth ;
        break;
    default:
        break;
    }
    if(m_iStartPosition < 0)
    {
        m_iStartPosition = 0;
    }

}

#ifdef KINDLE_FOR_TOUCH
bool UITextBox::OnTouchEvent(MoveEvent* moveEvent)
{
    if (moveEvent->GetActionMasked() == MoveEvent::ACTION_DOWN)
    {
		if (m_fIsSelectAll)
	    {
	        m_fIsSelectAll = false;
	        UpdateWindow();
	    }
        SetFocus(true);
        UIIME* pIME = UIIMEManager::GetIME(m_inputMode, this);
        if (pIME)
        {
            if (pIME->IsShow())
            {
                pIME->HideIME();
            }
            else
            {
                pIME->ShowIME();
            }
        }
        return true;
    }

    return false;
}
#endif

void UITextBox::SetFontSize(int fontSize)
{
    SetFont(m_ifontAttr.m_iFace, m_ifontAttr.m_iStyle, fontSize);
}

int UITextBox::GetCurDataLen() const
{
    return m_iDataLen;
}

