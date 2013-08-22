#include "TouchAppUI/UICustomLayoutSettingDlg.h"
#include "GUI/CTpGraphics.h"
#include "I18n/StringManager.h"
#include "Utility/ColorManager.h"
#include  <sstream>

using namespace std;

#define CUSTOM_LAYOUT_DIALOG_WIDTH    (200)
#define CUSTOM_LAYOUT_DIALOG_HEIGTH   (180)

#define MAX_LINESPACING    500
#define MAX_PARASPACING    900
#define MAX_INDENT         10

UICustomLayoutSettingDlg::UICustomLayoutSettingDlg()
    : UIDialog()
    , m_strLineSpacingValue("")
    , m_strParaSpacingValue("")
    , m_strIndentValue("")
{
    Init();
};

UICustomLayoutSettingDlg::UICustomLayoutSettingDlg(UIContainer* pParent)
    : UIDialog(pParent)
    , m_strLineSpacingValue("")
    , m_strParaSpacingValue("")
    , m_strIndentValue("")
{
    Init();
};

UICustomLayoutSettingDlg::~UICustomLayoutSettingDlg()
{
};

void UICustomLayoutSettingDlg::Init()
{
    MoveWindow(0,0,CUSTOM_LAYOUT_DIALOG_WIDTH,CUSTOM_LAYOUT_DIALOG_HEIGTH);
    m_txtLineSpacing.SetText(StringManager::GetStringById(BOOK_LINE_GAP));
    m_txtLineSpacing.SetFontSize(28);
    m_txtLineSpacing.SetAlign(ALIGN_LEFT);
    m_txtLineSpacing.SetForeColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
    m_txtLineSpacing.MoveWindow(15,10,100,30);

    m_txtParaSpacing.SetText(StringManager::GetStringById(BOOK_PARA_SPACING));
    m_txtParaSpacing.SetFontSize(28);
    m_txtParaSpacing.SetAlign(ALIGN_LEFT);
    m_txtParaSpacing.SetForeColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
    m_txtParaSpacing.MoveWindow(15,50,100,30);

    m_txtIndent.SetText(StringManager::GetStringById(BOOK_INDENT));
    m_txtIndent.SetFontSize(28);
    m_txtIndent.SetAlign(ALIGN_LEFT);
    m_txtIndent.SetForeColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
    m_txtIndent.MoveWindow(15,90,100,30);

    m_boxInputLineSpacing.SetTipUTF8(m_strLineSpacingValue.c_str());
    m_boxInputLineSpacing.SetNumberOnlyMode();
    m_boxInputLineSpacing.MoveWindow(125,10,60,30);
    m_boxInputLineSpacing.SetMaxDataLen(3);

    m_boxInputParaSpacing.SetTipUTF8(m_strParaSpacingValue.c_str());
    m_boxInputParaSpacing.SetNumberOnlyMode();
    m_boxInputParaSpacing.MoveWindow(125,50,60,30);
    m_boxInputParaSpacing.SetMaxDataLen(3);

    m_boxInputIndet.SetTipUTF8(m_strIndentValue.c_str());
    m_boxInputIndet.SetNumberOnlyMode();
    m_boxInputIndet.MoveWindow(125,90,60,30);
    m_boxInputIndet.SetMaxDataLen(2);

    m_btnOk.Initialize(IDOK,StringManager::GetStringById(ACTION_OK), 24, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_btnOk.ShowIcon(false);
    m_btnOk.MoveWindow(15,140,80,30);
    m_btnOk.SetAlign(ALIGN_CENTER);

    m_btCancel.Initialize(IDCANCEL,StringManager::GetStringById(ACTION_CANCEL), 24, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
    m_btCancel.ShowIcon(false);
    m_btCancel.MoveWindow(115,140,80,30);

    AppendChild(&m_txtLineSpacing);
    AppendChild(&m_txtParaSpacing);
    AppendChild(&m_txtIndent);
    AppendChild(&m_boxInputLineSpacing);
    AppendChild(&m_boxInputParaSpacing);
    AppendChild(&m_boxInputIndet);
    AppendChild(&m_btnOk);
    AppendChild(&m_btCancel);
    m_boxInputLineSpacing.SetVisible(FALSE);
    m_boxInputLineSpacing.SetFocus(true);
    m_boxInputLineSpacing.SetVisible(TRUE);
}

int UICustomLayoutSettingDlg::GetLineSpacing()const 
{
    if(m_strLineSpacingValue.empty())
    {
        return -1;
    }
    int iLineSpacing = atoi(m_strLineSpacingValue.c_str());
    if(iLineSpacing > MAX_LINESPACING)
    {
        iLineSpacing = MAX_LINESPACING;
    }
    return iLineSpacing;
}

int UICustomLayoutSettingDlg::GetParaSpacing()const
{
    if(m_strParaSpacingValue.empty())
    {
        return -1;
    }
    int iParaSpacing = atoi(m_strParaSpacingValue.c_str());
    if(iParaSpacing > MAX_PARASPACING)
    {
        iParaSpacing = MAX_PARASPACING;
    }
    return iParaSpacing;
}

int UICustomLayoutSettingDlg::GetIndent()const
{
    if(m_strIndentValue.empty())
    {
        return -1;
    }
    int iIndent = atoi(m_strIndentValue.c_str());
    if(iIndent > MAX_INDENT)
    {
        iIndent = MAX_INDENT;
    }
    return iIndent;
}

HRESULT UICustomLayoutSettingDlg::DrawBackGround(DK_IMAGE drawingImg)
{
    UIDialog::DrawBackGround(drawingImg);

    if (drawingImg.pbData == NULL)
    {
        return E_FAIL;
    }

    CTpGraphics prg(drawingImg);
    SPtr<ITpImage> spImg = ImageManager::GetImage(IMAGE_MENU_BACK);
    if (spImg)
    {
        prg.DrawImageStretchBlend(spImg.Get(), 0, 0,GetWidth(), GetHeight());
    }
    return S_OK;
}

void UICustomLayoutSettingDlg::InitCurrentSpacing(int _iLineSpacing,int _iParaSpacing,int _iIndent)
{
    if(_iLineSpacing < 0 || _iParaSpacing < 0)
    {
        return;
    }
    stringstream spd;
    spd.str("");

    spd << _iLineSpacing;
    m_strLineSpacingValue = spd.str();
    m_boxInputLineSpacing.SetTipUTF8(m_strLineSpacingValue.c_str());

    spd.clear();
    spd.str("");
    spd << _iParaSpacing;
    m_strParaSpacingValue = spd.str();
    m_boxInputParaSpacing.SetTipUTF8(m_strParaSpacingValue.c_str());

    spd.clear();
    spd.str("");
    spd << _iIndent;
    m_strIndentValue = spd.str();
    m_boxInputIndet.SetTipUTF8(m_strIndentValue.c_str());

    DebugPrintf(DLC_XU_KAI,"m_strLineSpacingValue : %d");
}

void UICustomLayoutSettingDlg::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    switch (dwCmdId)
    {
    case IDOK:
        {
            m_strParaSpacingValue = m_boxInputParaSpacing.GetTextUTF8();
            m_strLineSpacingValue = m_boxInputLineSpacing.GetTextUTF8();
            m_strIndentValue = m_boxInputIndet.GetTextUTF8();
            this->EndDialog(1);
            break;
        }
    case IDCANCEL:
        {
            this->EndDialog(0);
        }
        break;
    default:
        break;
    }
}
