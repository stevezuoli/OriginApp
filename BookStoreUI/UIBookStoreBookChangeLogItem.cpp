#include "BookStoreUI/UIBookStoreBookChangeLogItem.h"
#include "I18n/StringManager.h"
#include "Utility/ImageManager.h"
#include "Utility/ColorManager.h"
#include "GUI/CTpGraphics.h"
#include "GUI/UIMessageBox.h"
#include "interface.h"
#include "Framework/CDisplay.h"
#include "drivers/DeviceInfo.h"
#include "BookStore/BookStoreInfoManager.h"
#include "BookStore/BookChangeLog.h"
#include <tr1/functional>
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"

using namespace WindowsMetrics;
using namespace dk::bookstore;

UIBookStoreBookChangeLogItem::UIBookStoreBookChangeLogItem()
{
    Init();
}

UIBookStoreBookChangeLogItem::~UIBookStoreBookChangeLogItem()
{

}

void UIBookStoreBookChangeLogItem::Init()
{
    const int fontsize16 = GetWindowFontSize(FontSize16Index);
    const int fontsize20 = GetWindowFontSize(FontSize20Index);
    m_updatedTime.SetFontSize(fontsize16);
    m_updatedTime.SetForeColor(ColorManager::GetColor(COLOR_DISABLECOLOR));
    AppendChild(&m_updatedTime);

    m_revision.SetFontSize(fontsize16);
    m_revision.SetForeColor(ColorManager::GetColor(COLOR_DISABLECOLOR));
    AppendChild(&m_revision);
    
    m_log.SetFontSize(fontsize20);
    m_log.SetMode(UIAbstractText::TextMultiLine);
    m_log.SetVAlign(ALIGN_LEFT);
    AppendChild(&m_log);

    const int elemSpacing = GetWindowMetrics(UIElementSpacingIndex);
    const int elemMidSpacing = GetWindowMetrics(UIElementMidSpacingIndex);
    UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
    UISizer* bottomSizer = new UIBoxSizer(dkHORIZONTAL);
    if (mainSizer && bottomSizer)
    {
        bottomSizer->Add(&m_revision);
        bottomSizer->AddStretchSpacer();
        bottomSizer->Add(&m_updatedTime, UISizerFlags().Align(dkALIGN_RIGHT));

        mainSizer->Add(&m_log,UISizerFlags(1).Border(dkTOP, elemMidSpacing).Expand());
        mainSizer->Add(bottomSizer,UISizerFlags().Border(dkTOP | dkBOTTOM, elemSpacing).Expand());
        SetSizer(mainSizer);
    }
    else
    {
        SafeDeletePointer(mainSizer);
        SafeDeletePointer(bottomSizer);
    }
}

void UIBookStoreBookChangeLogItem::SetInfoSPtr(dk::bookstore::model::BasicObjectSPtr changeLogInfo, int index)
{
    model::BookChangeLog* bookChangeLog = (model::BookChangeLog*)(changeLogInfo.get());
    if(NULL != bookChangeLog)
    {
        m_updatedTime.SetText(bookChangeLog->GetUpdatedTime().c_str());
        m_revision.SetText(bookChangeLog->GetRevision().c_str());
        m_log.SetText(bookChangeLog->GetLog().c_str());
        Layout();
    }
}

bool UIBookStoreBookChangeLogItem::ResponseTouchTap()
{
    UIContainer* pParent = GetParent();
    CDisplay* pDisplay = CDisplay::GetDisplay();
    if (pDisplay)
    {
        pParent = pDisplay->GetCurrentPage();
    }
    UIChangeLogDetailDlg changeLogDlg(pParent, m_revision.GetText());
    changeLogDlg.SetText(m_updatedTime.GetText(), m_log.GetText());

    dkSize minSize = changeLogDlg.GetMinSize();
    const int width = minSize.GetWidth();
    const int height = minSize.GetHeight();
    const int left = (DeviceInfo::GetDisplayScreenWidth() - width) >> 1;
    const int top = (DeviceInfo::GetDisplayScreenHeight() - height) >> 1;
    changeLogDlg.MoveWindow(left, top, width, height);
    changeLogDlg.DoModal();
    return true;
}

UIChangeLogDetailDlg::UIChangeLogDetailDlg(UIContainer* pParent, LPCSTR title)
    : UIModalDlg(pParent, title)
{
    ShowCloseIcon(true);
}

UIChangeLogDetailDlg::~UIChangeLogDetailDlg()
{

}

void UIChangeLogDetailDlg::SetText(const char* pUpdateTime, const char* pLog)
{
    string strUpdateTime = string(StringManager::GetPCSTRById(BOOKSTORE_BOOK_UPDATETIME)) + string(pUpdateTime);
    const int fontsize20 = GetWindowFontSize(FontSize20Index);
    m_updateTime.SetFontSize(fontsize20);
    m_updateTime.SetText(strUpdateTime.c_str());
    AppendChild(&m_updateTime);

    m_Log.SetFontSize(fontsize20);
    m_Log.SetText(pLog);
    m_Log.SetMode(UIAbstractText::TextMultiLine);
    int minHeight = (DeviceInfo::GetDisplayScreenHeight() >> 1);
    const int maxWidth = DeviceInfo::GetDisplayScreenWidth() >> 1;
    m_Log.SetMaxWidth(maxWidth);
    const int maxHeight = m_Log.GetTextHeight();
    if (minHeight >= maxHeight)
    {
        minHeight = maxHeight;
    }
    m_Log.SetMinSize(dkSize(maxWidth, minHeight));
    AppendChild(&m_Log);
    m_btnGroup.SetVisible(false);
    if (m_windowSizer)
    {
        const int horizontalMargin = GetWindowMetrics(UIModalDialogHorizonMarginIndex);
        m_windowSizer->Add(&m_updateTime, UISizerFlags().Expand().Border(dkTOP | dkLEFT | dkRIGHT, horizontalMargin).Border(dkBOTTOM, GetWindowMetrics(UIElementMidSpacingIndex)));
        m_windowSizer->Add(&m_Log, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, horizontalMargin).Border(dkBOTTOM, (horizontalMargin << 1)));
    }
}

void UIChangeLogDetailDlg::MoveWindow(int left, int top, int width, int height)
{
#ifdef KINDLE_FOR_TOUCH
    UIModalDlg::MoveWindow(left, top, width, height);
#else
    UIDialog::MoveWindow(left, top, width, height);
#endif
    Layout();
}

bool UIChangeLogDetailDlg::OnCloseClick(const EventArgs&)
{
    EndDialog(IDCANCEL);
    return true;
}

