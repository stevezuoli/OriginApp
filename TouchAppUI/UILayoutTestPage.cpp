#include "TouchAppUI/UILayoutTestPage.h"

UILayoutTestPage::UILayoutTestPage()
    : UIPage()
{
    Init();
}

UILayoutTestPage::~UILayoutTestPage()
{
    UnInit();
}

bool UILayoutTestPage::Init()
{
    m_vecWins1.clear();
    m_vecWins2.clear();
    m_vecWins3.clear();

    UISizer* sizer = new UIBoxSizer(dkVERTICAL);
    if (!sizer)
    {
        return false;
    }


    m_txtTest1.SetText("abcdefghijklmnopqrstuvwxyz");
    m_txtTest2.SetText("1234567890");
    m_txtTest2.SetFontSize(40);
    m_txtTest3.SetText("Space Following");
    m_txtTest1.SetBackColor(0xabcdef);
    m_txtTest2.SetBackColor(0xabcdef);
    m_txtTest3.SetBackColor(0xabcdef);
    m_textTest.SetBackColor(0xabcdef);

    m_textTest.SetFontSize(40);
    m_textTest.SetText("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    m_textTest.SetMinSize(dkSize(100,300));
    m_textTest.SetAutoModifyHeight(false);

    m_plainTextEditTest1.SetTextUTF8("This is a Test");
    m_plainTextEditTest2.SetTextUTF8("This is a Test");

    m_boxTest1.SetTextUTF8("abcdefghijklmn");
    m_boxTest2.SetTipUTF8("abcdefghijklmn");
    m_boxTest3.SetTextUTF8("abcdefghijklmn");

	m_btnTest1.Initialize(0x0100, "TestButtonA", 22);
    m_btnTest1.SetMinSize(dkSize(100,40));
	m_btnTest2.Initialize(0x0200, "TestButtonA", 72);
    m_btnTest2.SetMinSize(dkSize(100,100));
    UISizer* topSizer = new UIBoxSizer(dkHORIZONTAL);
    topSizer->Add(&m_btnTest1, UISizerFlags(1).Expand());
    topSizer->Add(&m_btnTest2, UISizerFlags(2).Expand());

    UISizer* middleSizer = new UIBoxSizer(dkHORIZONTAL);
    middleSizer->Add(&m_boxTest1, UISizerFlags(1));
    middleSizer->Add(&m_boxTest2, UISizerFlags(1));
    middleSizer->Add(&m_boxTest3, UISizerFlags(2));

    m_txtTotal.SetText("Total: 100 items");
    m_txtPageNO.SetText("12345/99999");
    m_txtTotal.SetBackColor(0xabcdef);
    m_txtPageNO.SetBackColor(0xabcdef);

    UISizer* bottomSizer = new UIBoxSizer(dkHORIZONTAL);
    bottomSizer->Add(&m_txtTotal, UISizerFlags().Border(dkLEFT, 30));
    bottomSizer->AddStretchSpacer();
    bottomSizer->Add(&m_txtPageNO, UISizerFlags().Border(dkRIGHT, 30));

    sizer->Add(topSizer, UISizerFlags().Expand());
    sizer->Add(&m_plainTextEditTest1, UISizerFlags().Expand());
    sizer->Add(&m_plainTextEditTest2, UISizerFlags().Expand());
    sizer->Add(&m_txtTest1, UISizerFlags().Align(dkALIGN_CENTER_HORIZONTAL));
    sizer->Add(&m_txtTest2, UISizerFlags().Align(dkALIGN_LEFT));
    sizer->Add(&m_textTest, UISizerFlags().Expand());
    sizer->Add(&m_txtTest3, UISizerFlags().Align(dkALIGN_RIGHT));
    sizer->AddStretchSpacer();
    sizer->Add(middleSizer, UISizerFlags().Expand());
    sizer->AddSpacer(10);
    sizer->Add(bottomSizer, UISizerFlags().Expand());
    sizer->AddSpacer(100);

    AppendChild(&m_btnTest1);
    AppendChild(&m_btnTest2);
    AppendChild(&m_txtTest1);
    AppendChild(&m_txtTest2);
    AppendChild(&m_txtTest3);
    AppendChild(&m_textTest);
    AppendChild(&m_plainTextEditTest1);
    AppendChild(&m_plainTextEditTest2);
    AppendChild(&m_txtTotal);
    AppendChild(&m_txtPageNO);
    AppendChild(&m_boxTest1);
    AppendChild(&m_boxTest2);
    AppendChild(&m_boxTest3);

    SetSizer(sizer);
    sizer->SetSizeHints(this);
    return true;
}

void UILayoutTestPage::UnInit()
{
    if (m_windowSizer)
    {
        delete m_windowSizer;
        m_windowSizer = NULL;
    }
}

void UILayoutTestPage::MoveWindow(int iLeft, int iTop, int iWidth, int iHeight)
{
    UIPage::MoveWindow(iLeft, iTop, iWidth, iHeight);
    Layout();
}

void UILayoutTestPage::OnCommand(DWORD dwCmdId, UIWindow* pSender, DWORD dwParam)
{
    switch (dwCmdId)
    {
    case 0x0100:
    case 0x0101:
    default:
        break;
    }
}