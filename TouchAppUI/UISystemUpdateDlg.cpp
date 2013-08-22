#include "TouchAppUI/UISystemUpdateDlg.h"
#include "I18n/StringManager.h"
#include "Utility/EncodeUtil.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"

using dk::utility::EncodeUtil;
using namespace WindowsMetrics;

UISystemUpdateDlg::UISystemUpdateDlg(UIContainer* pParent, LPCSTR title)
    :UIModalDlg(pParent, title)
    , m_Text(this, IDSTATIC)
    , m_iCurPage(-1)
    , m_iTotalPage(-1)
    , m_wstrText(L"")
{
}

UISystemUpdateDlg::~UISystemUpdateDlg()
{
}

void UISystemUpdateDlg::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    switch (dwCmdId)
    {
    case ID_BTN_TOUCH_SYSTEMUPDATE_OK:
        EndDialog(1); 
        break;
    case IDCANCEL:
    case ID_BTN_TOUCH_SYSTEMUPDATE_CANCEL:
        EndDialog(0); 
        break;
    case ID_BTN_TOUCH_SYSTEMUPDATE_PAGE_UP:
    case ID_BTN_TOUCH_SYSTEMUPDATE_PAGE_DOWN:
        HandlePageUpDown(ID_BTN_TOUCH_SYSTEMUPDATE_PAGE_UP == dwCmdId);
        break;
    }
}

void UISystemUpdateDlg::Popup()
{
    InitUI();
    UIDialog::Popup();
}

void UISystemUpdateDlg::SetText(const char* pstrUpdate)
{
    m_wstrText = EncodeUtil::ToWString(pstrUpdate);
}

void UISystemUpdateDlg::HandlePageUpDown(bool bPageUp)
{
    if (m_iTotalPage <= 1)
    {
        return;
    }

    if (bPageUp)
    {
        m_iCurPage = (m_iCurPage == 0) ? 0 : (m_iCurPage - 1);
    }
    else
    {
        m_iCurPage = (m_iCurPage == (m_iTotalPage - 1)) ? (m_iTotalPage - 1) : (m_iCurPage + 1);
    }
    m_Text.SetDrawPageNo(m_iCurPage);
    
    char cText[10] = {0};
    snprintf(cText, sizeof(cText), "%d / %d", (m_iCurPage + 1), m_iTotalPage);
    m_pageNum.SetText(cText);
    UpdateWindow();
}

void UISystemUpdateDlg::InitUI()
{
    m_btnCancel.Initialize(ID_BTN_TOUCH_SYSTEMUPDATE_CANCEL, StringManager::GetStringById(TOUCH_SYSTEMUPDATE_CANCEL), GetWindowFontSize(UISystemUpdateDialogTextIndex));
    m_btnOK.Initialize(ID_BTN_TOUCH_SYSTEMUPDATE_OK, StringManager::GetStringById(TOUCH_SYSTEMUPDATE_OK), GetWindowFontSize(UISystemUpdateDialogTextIndex));

    m_Text.SetFontSize(GetWindowFontSize(UISystemUpdateDialogTextIndex)); 
    m_Text.SetText(m_wstrText);
    m_Text.SetAlign(ALIGN_JUSTIFY);

    m_leftPage.SetId(ID_BTN_TOUCH_SYSTEMUPDATE_PAGE_UP);
    m_leftPage.SetIcon(ImageManager::GetImage(IMAGE_TOUCH_ARROW_LEFT), UIButton::ICON_RIGHT);
    m_leftPage.SetAlign(ALIGN_RIGHT);
    m_leftPage.ShowBorder(false);

    m_pageNum.SetFontSize(GetWindowFontSize(UIPageNumIndex));
    m_pageNum.SetAlign(ALIGN_CENTER);

    m_rightPage.SetId(ID_BTN_TOUCH_SYSTEMUPDATE_PAGE_DOWN);
    m_rightPage.SetIcon(ImageManager::GetImage(IMAGE_TOUCH_ARROW_RIGHT), UIButton::ICON_LEFT);
    m_rightPage.SetAlign(ALIGN_LEFT);
    m_rightPage.ShowBorder(false);
    
    AppendChild(&m_Text);
    AppendChild(&m_leftPage);
    AppendChild(&m_rightPage);
    AppendChild(&m_pageNum);
    m_btnGroup.AddButton(&m_btnOK);
    m_btnGroup.AddButton(&m_btnCancel);

    if (m_windowSizer)
    {
        m_Text.SetMinSize(dkSize(GetWindowMetrics(UIModalDialogWidthIndex) - (GetWindowMetrics(UIModalDialogHorizonMarginIndex) << 1), GetWindowMetrics(UISystemUpdateDlgTextHeightIndex)));
        m_windowSizer->Add(&m_Text, UISizerFlags().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIModalDialogHorizonMarginIndex)).Expand());

        UISizer* pPageSizer = new UIBoxSizer(dkHORIZONTAL);
        if (pPageSizer)
        {
            pPageSizer->Add(&m_leftPage, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL).Border(dkLEFT, GetWindowMetrics(UIModalDialogHorizonMarginIndex)));
            pPageSizer->Add(&m_pageNum, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL));
            pPageSizer->Add(&m_rightPage, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL).Border(dkRIGHT, GetWindowMetrics(UIModalDialogHorizonMarginIndex)));
            m_windowSizer->Add(pPageSizer, UISizerFlags().Expand().Border(dkTOP | dkBOTTOM, GetWindowMetrics(UIModalDialogHorizonMarginIndex)));
        }

        m_windowSizer->Add(&m_btnGroup, UISizerFlags().Expand());
        
        const int width = GetWindowMetrics(UIModalDialogWidthIndex);
        const int height = m_windowSizer->GetMinSize().GetHeight();
        MakeCenter(width, height);
        Layout();
        
        m_iCurPage = m_Text.GetCurPage();
        m_iTotalPage = m_Text.GetPageCount();
        char cText[10] = {0};
        snprintf(cText, sizeof(cText), "%d / %d", (m_iCurPage + 1), m_iTotalPage);
        m_pageNum.SetText(cText);
    }
}

bool UISystemUpdateDlg::OnTouchMoveNext(GESTURE_EVENT _ge, int _x, int _y)
{
    assert(GESTURE_MOVE_UP == _ge || GESTURE_MOVE_LEFT == _ge);
    {
        HandlePageUpDown(false);
    }
    
    return true;
}

bool UISystemUpdateDlg::OnTouchMovePrev(GESTURE_EVENT _ge, int _x, int _y)
{
    assert(GESTURE_MOVE_DOWN == _ge || GESTURE_MOVE_RIGHT == _ge);
    {
        HandlePageUpDown(true);
    }
    return true;
}

bool UISystemUpdateDlg::OnTouchTap(GESTURE_EVENT _ge, int _x, int _y)
{
    assert(GESTURE_END == _ge);
    double widthPercent  = 0.5;
    if (_x>=0 && _x<=m_iWidth*widthPercent)
    {
        HandlePageUpDown(true);
    }
    else
    {
        HandlePageUpDown(false);
    }
    return true;
}

