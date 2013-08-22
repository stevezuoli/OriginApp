#include "PersonalUI/UIPersonalBookNoteItem.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "CommonUI/CPageNavigator.h"
#include "Framework/CDisplay.h"
#include "I18n/StringManager.h"
#include "BookStoreUI/UIBookStoreBookNoteDetailPage.h"
using namespace WindowsMetrics;

UIPersonalBookNoteItem::UIPersonalBookNoteItem(const std::string& bookTitle)
    : m_bookTitle(bookTitle)
{
    Init();
}

UIPersonalBookNoteItem::~UIPersonalBookNoteItem()
{
}

void UIPersonalBookNoteItem::SetBookNoteInfoSPtr(ICT_ReadingDataItemSPtr readingDataItemImpl)
{
    m_readingDataItemImpl = CT_ReadingDataItemImplSPtr(new CT_ReadingDataItemImpl(*(CT_ReadingDataItemImpl*)(readingDataItemImpl.get())));
    if (m_readingDataItemImpl)
    {
        m_textContent.SetText(m_readingDataItemImpl->GetBookContent());
        string modifyTime = m_readingDataItemImpl->GetLastModifyTime();
        //error with the data got from server
        if (modifyTime.find("1970") != string::npos)
        {
            modifyTime = m_readingDataItemImpl->GetCreateTime();
        }
        size_t pos = modifyTime.find('T');
        if (pos != string::npos)
        {
            modifyTime.replace(pos, 1, "  ");
        }
        m_labelModifyTime.SetText(modifyTime);
        if (!m_readingDataItemImpl->GetUserContent().empty())
        {
            char buf[1024] = {0};
            snprintf(buf, DK_DIM(buf), "%s%s", StringManager::GetPCSTRById(TOUCH_COMMENTITEM), m_readingDataItemImpl->GetUserContent().c_str());
            m_labelComment.SetText(buf);
        }
        else
        {
            m_labelComment.SetText("");
        }
    }
}

void UIPersonalBookNoteItem::ShowNoteDetailPage()
{
    if (m_readingDataItemImpl)
    {
        string content = m_readingDataItemImpl->GetBookContent();
        if (!m_readingDataItemImpl->GetUserContent().empty())
        {
            char buf[64] = {0};
            snprintf(buf, DK_DIM(buf), "\n%s\n", StringManager::GetPCSTRById(TOUCH_COMMENTITEM));
            content.append(buf).append(m_readingDataItemImpl->GetUserContent());
        }
        UIBookStoreBookNoteDetailPage* pPage = new UIBookStoreBookNoteDetailPage(content);
        pPage->SetTitle(m_bookTitle);
        pPage->MoveWindow(0, 0, CDisplay::GetDisplay()->GetScreenWidth(), CDisplay::GetDisplay()->GetScreenHeight());
        CPageNavigator::Goto(pPage);
    }
}

bool UIPersonalBookNoteItem::ResponseTouchTap()
{
    ShowNoteDetailPage();
    return true;
}

void UIPersonalBookNoteItem::Init()
{
#ifdef KINDLE_FOR_TOUCH
    SetLongClickable(true);
#endif

    m_textContent.SetFontSize(GetWindowFontSize(UIPersonalBookNoteItemContentIndex));
    m_textContent.SetMaxWidth(CDisplay::GetDisplay()->GetScreenWidth() - (GetWindowMetrics(UIHorizonMarginIndex) << 1));
    m_textContent.SetMode(UIAbstractText::TextMultiLine);
    m_textContent.SetAlign(ALIGN_JUSTIFY);
    m_labelModifyTime.SetFontSize(GetWindowFontSize(UIPersonalBookNoteItemNoteIndex));
    m_labelModifyTime.SetAlign(ALIGN_RIGHT);
    m_labelComment.SetFontSize(GetWindowFontSize(UIPersonalBookNoteItemNoteIndex));

    AppendChild(&m_textContent);
    AppendChild(&m_labelModifyTime);
    AppendChild(&m_labelComment);

    UISizer* mainSizer = new UIBoxSizer(dkVERTICAL);
    if (mainSizer)
    {
        mainSizer->AddSpacer(GetWindowMetrics(UIPersonalBookNoteItemHorizonBorderIndex));
        mainSizer->Add(&m_textContent, UISizerFlags(1).Expand().Border(dkBOTTOM, GetWindowMetrics(UIPixelValue3Index)));
        mainSizer->Add(&m_labelModifyTime, UISizerFlags().Expand());
        mainSizer->Add(&m_labelComment, UISizerFlags().Expand());
        mainSizer->AddSpacer(GetWindowMetrics(UIPersonalBookNoteItemHorizonBorderIndex));

        SetSizer(mainSizer);
    }
}

