#define DK_BS_PUSHBUTTON        0x00000000L //正常按钮，有按键消息
#define DK_BS_DEFPUSHBUTTON     0x00000001L //默认按钮
#define DK_BS_CHECKBOX          0x00000002L //多选框
#define DK_BS_RADIOBUTTON       0x00000004L //单选框
#define DK_BS_STATICBUTTON      0x00000008L //静态按钮，没有按键消息
#define DK_BS_DOTTEDLINE        0X00003000L //带虚线

#define DK_BS_UP                0x0
#define DK_BS_DOWN              0x1
#define DK_BS_SELECTED          0x2
#define DK_BS_VISIBLE           0X10
class UISysButton : public UIButton
{
public:
    INT64   m_dwStyle;
    INT32   m_nStatus;
    INT32   m_nPage;
    INT32   m_HotKey;
    INT32   m_InputStatus;

    //button类型与子button
    INT32    m_type;
    std::vector<UISysButton*> m_chdBtn;
public:
    UISysButton() 
        : m_dwStyle(0)
        , m_nStatus(0)
        , m_HotKey(-1)
        , m_InputStatus(-1){}
    UISysButton(UIContainer* pParent, INT32 rstrId, long dwstyle, INT32 type, LPCSTR szText="", INT32 iFontSize=0)
        : UIButton(pParent, rstrId, szText)
        , m_dwStyle(dwstyle)
        , m_nStatus(0)
        , m_nPage(0)
        , m_HotKey(-1)
        , m_InputStatus(-1)
        , m_type(type)
    {
        if (iFontSize!=0)
        {
            m_ifontAttr.SetFontAttributes(0, 0, iFontSize);
        }
    }

    ~UISysButton(){}

    HRESULT Initialize(INT32 rstrId, long dwstyle, INT32 type, LPCSTR szText="", INT32 iFontSize=0)
    {
        SetId(rstrId);
        m_dwStyle = dwstyle;
        m_type = type;
        SetText(szText);
        if (iFontSize!=0)
        {
            m_ifontAttr.SetFontAttributes(0, 0, iFontSize);
        }

        if (m_dwStyle & DK_BS_RADIOBUTTON)
        {
            SetImage(ImageManager::GetImagePath(IMAGE_SYS_OPTION));
        }

        return S_OK;
    }

public:
    void SetButtonStatus(INT32 nStaus)
    {
        if(m_nStatus != nStaus)
        {
            m_nStatus = nStaus;
            if(DK_BS_VISIBLE != (m_nStatus & 0xF0))
            {
                Repaint();
                //UpdateWindow();
            }
        }
    }

    HRESULT UpdateSeleted()
    {
        HRESULT hr(S_OK);
        DK_IMAGE imgParent;
        DK_RECT rectParent;
        RTN_HR_IF_FAILED(GetParentDC(imgParent, rectParent));


        CTpGraphics grf(imgParent);
        RTN_HR_IF_FAILED(Draw(imgParent));


        DK_RECT updatedRect = {
            rectParent.left + m_iLeft,
            rectParent.top + m_iTop + 16,
            rectParent.left + m_iLeft + m_iWidth,
            rectParent.top + m_iTop + 24
        };
        UpdateToScreen(updatedRect);
        return hr;
    }

    void SetFocus(BOOL bIsFocus)
    {
        if (m_bIsFocus == bIsFocus)
        {
            return;
        }

        if (bIsFocus && m_pParent != NULL)
        {
            m_pParent->OnChildSetFocus(this);
        }

        m_bIsFocus = bIsFocus;
        UpdateSeleted();
    }

    INT32 GetStatus()
    {
        return m_nStatus;
    }

    void SetShortKey(INT32 keyvalue)
    {
        m_HotKey = keyvalue;
    }

    int GetTextLenth()
    {
        ITpFont* pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, m_iFontColor);
        return pFont->StringWidth(m_strText);
    }

    HRESULT  Draw(DK_IMAGE image)
    {
        int left = 0;
        if (!m_bIsVisible || DK_BS_VISIBLE == (m_nStatus & 0xF0))
        {
            return S_OK;
        }

        if (image.pbData == NULL)// || !m_IsChanged)
        {
            return E_FAIL;
        }
        HRESULT hr(S_OK);

        DK_RECT rcToClean = {m_iLeft, m_iTop - 2, m_iLeft + m_iWidth, m_iTop + m_iHeight};
        DK_IMAGE imgToClean;
        RTN_HR_IF_FAILED(CropImage(image, rcToClean, &imgToClean));
        CTpGraphics grfClean(imgToClean);
        RTN_HR_IF_FAILED(grfClean.FillRect(0, 0,
            m_iWidth,
            42,
            ColorManager::knWhite));

        DK_IMAGE imgSelf;
        DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
        RTN_HR_IF_FAILED(CropImage(
            image,
            rcSelf,
            &imgSelf
            ));

        CTpGraphics grf(imgSelf);
#if 0
        RTN_HR_IF_FAILED(grf.FillRect(0, 16,
            m_iWidth,
            24,
            ColorManager::knWhite));
#endif

        CString imgPath = GetImage();
        if(imgPath.Length() > 0)
        {
            SPtr<ITpImage> pbgImage = ImageManager::GetImage(imgPath);
            if(pbgImage)
            {
                if(DK_BS_RADIOBUTTON ==  (m_dwStyle & 0xF))
                {
                    if(m_nStatus == DK_BS_SELECTED)
                    {
                        RTN_HR_IF_FAILED(grf.DrawImageBlend(pbgImage.Get(), 0, 3, 0, 0, m_iWidth, pbgImage->bmHeight));
                    }
                    left += pbgImage->bmWidth + 10;
                }
                else
                {
                    RTN_HR_IF_FAILED(grf.DrawImageBlend(pbgImage.Get(), 0, 0, 0, 0, m_iWidth,  m_iHeight));
                    if(m_HotKey != -1)
                    {
                        char shortkey[2];
                        sprintf(shortkey, "%c", m_HotKey);
                        DKFontAttributes fontattr;
                        fontattr.SetFontAttributes(FACE_SYSTEM, STYLE_PLAIN, 16);
                        ITpFont* pFont = FontManager::GetInstance()->GetFont(fontattr, m_iFontColor);
                        INT32 iCharWidth = pFont->CharWidth(m_HotKey);
                        grf.DrawStringUtf8(shortkey, (left + (pbgImage->bmWidth - iCharWidth)) >> 1, 4, pFont);
                    }
                    left += 30;
                }
            }
        }

        int strWidth = 0;
        if(m_strText.Length())
        {
            ITpFont* pFont = FontManager::GetInstance()->GetFont(m_ifontAttr, m_iFontColor);
            grf.DrawStringUtf8(m_strText, left, -2, pFont);
            strWidth = pFont->StringWidth(m_strText);
        }

        if(this->GetId() == 0x251 ||
        this->GetId() == 0x271 ||
        this->GetId() == 0x281)
        {
            SPtr<ITpImage> pbgImage = ImageManager::GetImage(IMAGE_ARROW_DOWN);
            grf.DrawImageBlend(pbgImage.Get(), left + strWidth + 10, 3, 0, 0, 11, 6);
        }

     if(m_InputStatus == -1)
     {
            if(m_dwStyle & 0xF000)
            {
                RTN_HR_IF_FAILED(grf.DrawLine(
                    left,
                    17,
                    m_iWidth - 10,
                    1,
                    DOTTED_STROKE));
            }

            if (IsFocus())
            {
                RTN_HR_IF_FAILED(grf.DrawLine(
                    left,
                    16,
                    m_iWidth - 10,
                    5,
                    SOLID_STROKE));
            }
     }
     else
     {
         RTN_HR_IF_FAILED(grf.DrawLine(
                    left,
                    17,
                    m_iWidth - 10,
                    1,
                    SOLID_STROKE));
        RTN_HR_IF_FAILED(grf.DrawLine(
                    left,
                    0,
                    m_iWidth - 10,
                    1,
                    SOLID_STROKE));
        RTN_HR_IF_FAILED(grf.DrawLine(
                    left,
                    0,
                    1,
                    16,
                    SOLID_STROKE));
        RTN_HR_IF_FAILED(grf.DrawLine(
                    left + m_iWidth - 10,
                    0,
                    1,
                    16,
                    SOLID_STROKE));
     }
        m_IsChanged = false;
        return hr;
    }
};


