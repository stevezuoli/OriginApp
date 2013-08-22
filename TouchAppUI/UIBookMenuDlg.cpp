#include "TouchAppUI/UIBookMenuDlg.h"
#include "I18n/StringManager.h"
#include "Framework/CDisplay.h"
#include "GUI/CTpGraphics.h"
#include "GUI/GlobalEventListener.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
using namespace WindowsMetrics;

UIBookMenuDlg::UIBookMenuDlg(UIContainer* parent, BookMenuType type)
    : UIDialog(parent)
    , m_bookMenuType(type)
    , m_commandId(ID_INVALID)
{
    HookTouch();
    InitUI();
}

UIBookMenuDlg::UIBookMenuDlg(UIContainer* parent, const std::vector<int>& customizedBtnIDs, const std::vector<int>& customizedStrIDs)
    : UIDialog(parent)
    , m_bookMenuType(BMT_DUOKAN_USER_CUSTOMIZE)
    , m_customizedBtnIDs(customizedBtnIDs)
    , m_commandId(ID_INVALID)
{
    
    for (size_t i = 0 ; i < customizedStrIDs.size(); i++)
    {
        if (customizedStrIDs.at(i) < 0)
        {
            break;
        }
        m_customizedStrings.push_back(StringManager::GetPCSTRById((SOURCESTRINGID)customizedStrIDs[i]));
    }
    HookTouch();
    InitUI();
}

UIBookMenuDlg::UIBookMenuDlg(UIContainer* parent, const std::vector<int>& customizedBtnIDs, const std::vector<std::string>& customizedStrings)
    : UIDialog(parent)
    , m_bookMenuType(BMT_DUOKAN_USER_CUSTOMIZE)
    , m_customizedBtnIDs(customizedBtnIDs)
    , m_customizedStrings(customizedStrings)
    , m_commandId(ID_INVALID)
{
    HookTouch();
    InitUI();
}

UIBookMenuDlg::~UIBookMenuDlg()
{
}

void UIBookMenuDlg::InitUI()
{
    for (int i = 0; i < MAX_BUTTONS; ++i)
    {
        m_btns[i].SetVisible(false);
    }
    const int dlgWidth = GetWindowMetrics(UIBookMenuWidthIndex);
    const int btnHeight = GetWindowMetrics(UIBookMenuBtnHeightIndex);
    const int dlgMargin = GetWindowMetrics(UIBookMenuMarginIndex);
    UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
    mainSizer->AddSpacer(dlgMargin);

    for (int i = 0; i < MAX_BUTTONS; ++i)
    {
        if (m_bookMenuType != BMT_DUOKAN_USER_CUSTOMIZE)
        {
            const int btnIds[][MAX_BUTTONS + 1] = 
            {
                {ID_BTN_READ_BOOK, ID_BTN_DELETE, ID_BTN_SINAWEIBO_SHARE, ID_INVALID},
                {ID_BTN_OPEN_FOLDER, ID_BTN_RENAME_FOLDER, ID_BTN_DELETE_FOLDER, ID_INVALID},
                {ID_BTN_READ_BOOK, ID_BTN_DELETE, ID_BTN_DELETE_FROM_CATEGORY, ID_BTN_SINAWEIBO_SHARE, ID_INVALID},
                {ID_BTN_ADD_FILES_TO_CATEGORY, ID_BTN_RENAME_CATEGORY, ID_BTN_DELETE_CATEGORY, ID_INVALID},
                {ID_BTN_HIDEBOOK, ID_INVALID},
                //{ID_BTN_ADD_BOOK_TO_CART, ID_BTN_REMOVE_FAVOURITE, ID_BTN_OPEN_BOOK, ID_INVALID},
                //{ID_BTN_ADD_BOOK_TO_FAVOURITE, ID_BTN_REMOVE_FROM_CART, ID_BTN_OPEN_BOOK, ID_INVALID},
            };

            const int strIds[][MAX_BUTTONS+1] = 
            {
                {READ_BOOK, DELETE_BOOK, SHARE_BOOK_TO_SINAWEIBO, -1},
                {TOUCH_OPEN_FOLDER, CATEGORY_RENAME_BUTTON, TOUCH_DELETE_FOLDER, -1},
                {READ_BOOK, DELETE_BOOK, CATEGORY_DELETE_FILE_FROM_CATEGORY, SHARE_BOOK_TO_SINAWEIBO, -1},
                {CATEGORY_ADD_FILE, CATEGORY_RENAME_BUTTON, BOOK_DELETE, -1},
                {BOOKSTORE_HIDEBOOK_FROM_LIST, -1},
                //{ADD_BOOK_TO_CART, REMOVE_FAVOURITE, OPEN_BOOK_DETAILS, -1},
                //{ADD_BOOK_TO_FAVOURITE, REMOVE_FROM_CART, OPEN_BOOK_DETAILS, -1},
            };

            if (ID_INVALID == btnIds[m_bookMenuType][i])
            {
                break;
            }
            m_btns[i].Initialize(btnIds[m_bookMenuType][i],
                    StringManager::GetPCSTRById((SOURCESTRINGID)strIds[m_bookMenuType][i]),
                    GetWindowFontSize(FontSize22Index));
        }
        else
        {
            if (ID_INVALID == m_customizedBtnIDs[i])
            {
                break;
            }
            string text = m_customizedStrings[i];
            m_btns[i].Initialize(m_customizedBtnIDs[i],
                    text.c_str(),
                    GetWindowFontSize(FontSize22Index));
        }
        m_btns[i].SetVisible(true);
        m_btns[i].SetMinHeight(btnHeight);
        AppendChild(&m_btns[i]);
        mainSizer->Add(&m_btns[i],UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, dlgMargin));
        mainSizer->AddSpacer(dlgMargin);
    }

    SetSizer(mainSizer);
    const dkSize minSize = m_windowSizer->GetMinSize();
    SetSize(dkSize(dlgWidth, minSize.GetHeight()));
    Layout();
}

HRESULT UIBookMenuDlg::DrawBackGround(DK_IMAGE _drawingImg)
{
    HRESULT hr(S_OK);
    DK_IMAGE imgSelf;

    DK_RECT rcSelf={0, 0, m_iWidth, m_iHeight};

    RTN_HR_IF_FAILED(CropImage(
        _drawingImg,
        rcSelf,
        &imgSelf));

    CTpGraphics grp(imgSelf);

    SPtr<ITpImage> spImg = ImageManager::GetImage(IMAGE_MENU_BACK);
    if (spImg)
    {
        RTN_HR_IF_FAILED(grp.DrawImageStretchBlend(spImg.Get(), 0, 0, m_iWidth, m_iHeight));
    }
    return S_OK;
}

bool UIBookMenuDlg::OnHookTouch(MoveEvent* moveEvent)
{
    if (moveEvent->GetActionMasked() != MoveEvent::ACTION_DOWN)
    {
        return false;
    }
    if (!PointInWindowAbsolute(moveEvent->GetX(), moveEvent->GetY()))
    {
        EndDialog(IDCANCEL);
        return true;
    }
    return false;
}

void UIBookMenuDlg::OnCommand(DWORD _dwCmdId, UIWindow* _pSender, DWORD _dwParam)
{
    if (m_pParent)
    {
        EndDialog(IDOK);
        UnhookTouch();
        m_commandId = _dwCmdId;
    }
}

int UIBookMenuDlg::GetCommandId() const
{
    return m_commandId;
}

