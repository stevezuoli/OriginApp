#include "BookStore/BookStoreInfoManager.h"
#include "BookStoreUI/UIBookStoreBookRightsInfo.h"
#include "BookStoreUI/UIBookStoreBookChangeLogPage.h"
#include "drivers/DeviceInfo.h"
#include "I18n/StringManager.h"
#include "Utility/ImageManager.h"
#include "CommonUI/CPageNavigator.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"

using namespace WindowsMetrics;
using namespace dk::bookstore;

UIBookStoreBookRightsInfo::UIBookStoreBookRightsInfo(const std::string& bookId)
    : UITablePanel()
    , m_bookId(bookId)
{
    Init();
}

UIBookStoreBookRightsInfo::~UIBookStoreBookRightsInfo()
{
}

void UIBookStoreBookRightsInfo::Init()
{
    UISizer* pMainSizer = new UIBoxSizer(dkVERTICAL);
    if (pMainSizer)
    {
        const int margin = GetWindowMetrics(UIPixelValue15Index);
        const int infoFontSize = GetWindowFontSize(FontSize18Index);
        
        m_authors.SetFontSize(infoFontSize);
        m_authors.SetEnglishGothic();
        AppendChild(&m_authors);

        m_editors.SetFontSize(infoFontSize);
        m_editors.SetEnglishGothic();
        AppendChild(&m_editors);

        m_translators.SetFontSize(infoFontSize);
        m_translators.SetEnglishGothic();
        AppendChild(&m_translators);

        m_rights.SetFontSize(infoFontSize);
        m_rights.SetEnglishGothic();
        AppendChild(&m_rights);

        m_identifier.SetFontSize(infoFontSize);
        m_identifier.SetEnglishGothic();
        AppendChild(&m_identifier);

        m_issued.SetFontSize(infoFontSize);
        m_issued.SetEnglishGothic();
        AppendChild(&m_issued);

        m_updateTime.SetFontSize(infoFontSize);
        m_updateTime.SetEnglishGothic();
        AppendChild(&m_updateTime);

#ifdef KINDLE_FOR_TOUCH
        m_moreUpdate.Initialize(ID_BOOKSTORE_VIEWUPDATE_ALL, StringManager::GetStringById(BOOKSTORE_VIEW_UPDATE_ALL), infoFontSize);
        m_moreUpdate.ShowBorder(false);
        m_moreUpdate.SetIcon(ImageManager::GetImage(IMAGE_ARROW_STRONG_RIGHT),UIButton::ICON_RIGHT);
        m_moreUpdate.SetDisabledIcon(ImageManager::GetImage(IMAGE_ARROW_STRONG_RIGHT_DISABLE));
        m_moreUpdate.SetLeftMargin(0);
#else
        m_moreUpdate.Initialize(ID_BOOKSTORE_VIEWUPDATE_ALL, StringManager::GetStringById(BOOKSTORE_VIEW_UPDATE_ALL), KEY_RESERVED, infoFontSize, ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
        m_moreUpdate.SetFont(0, 0, infoFontSize);
#endif
        AppendChild(&m_moreUpdate);

        m_size.SetFontSize(infoFontSize);
        m_size.SetEnglishGothic();
        AppendChild(&m_size);

        m_wordCount.SetFontSize(infoFontSize);
        m_wordCount.SetEnglishGothic();
        AppendChild(&m_wordCount);

        pMainSizer->Add(&m_authors, UISizerFlags().Expand().Border(dkTOP, GetWindowMetrics(UIPixelValue30Index)));
        pMainSizer->Add(&m_editors, UISizerFlags().Expand().Border(dkTOP, margin));
        pMainSizer->Add(&m_translators, UISizerFlags().Expand().Border(dkTOP, margin));
        pMainSizer->Add(&m_rights, UISizerFlags().Expand().Border(dkTOP, margin));
        pMainSizer->Add(&m_identifier, UISizerFlags().Expand().Border(dkTOP, margin));
        pMainSizer->Add(&m_issued, UISizerFlags().Expand().Border(dkTOP, margin));

        UISizer* updateSizer = new UIBoxSizer(dkHORIZONTAL);
        if (updateSizer)
        {
            updateSizer->Add(&m_updateTime, UISizerFlags().Expand());
            updateSizer->AddStretchSpacer();
            updateSizer->Add(&m_moreUpdate, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
            pMainSizer->Add(updateSizer, UISizerFlags().Expand().Border(dkTOP, margin));
        }
        pMainSizer->Add(&m_size, UISizerFlags().Expand().Border(dkTOP, margin));
        pMainSizer->Add(&m_wordCount, UISizerFlags().Expand().Border(dkTOP, margin));
        pMainSizer->AddStretchSpacer();
        SetSizer(pMainSizer);
    }
}

void UIBookStoreBookRightsInfo::SetBookInfo(dk::bookstore::model::BookInfoSPtr bookDetailedInfo)
{
    m_bookDetailedInfo.reset((model::BookInfo*)bookDetailedInfo->Clone());
    if (m_bookDetailedInfo)
    {
        string author = m_bookDetailedInfo->GetAuthors();
        m_authors.SetVisible(!author.empty());
        if (!author.empty())
        {
            string label = StringManager::GetPCSTRById(BOOK_AUTHOR);
            label += "\t" + author;
            m_authors.SetText(label.c_str());
        }

        string editor = m_bookDetailedInfo->GetEditors();
        m_editors.SetVisible(!editor.empty());
        if (!editor.empty())
        {
            string label = StringManager::GetPCSTRById(BOOK_EDITORS);
            label += "\t" + editor;
            m_editors.SetText(label.c_str());
        }

        string translator = m_bookDetailedInfo->GetTranslators();
        m_translators.SetVisible(!translator.empty());
        if (!translator.empty())
        {
            string label = StringManager::GetPCSTRById(BOOK_TRANSLATORS);
            label += ":\t" + translator;
            m_translators.SetText(label.c_str());
        }

        string rights = m_bookDetailedInfo->GetCopyRights();
        m_rights.SetVisible(!rights.empty());
        if (!rights.empty())
        {
            string label = StringManager::GetPCSTRById(BOOK_PUBLISH);
            label += ":\t" + rights;
            m_rights.SetText(label.c_str());
        }

        string identifier = m_bookDetailedInfo->GetIdentifier();
        m_identifier.SetVisible(!identifier.empty());
        if (!identifier.empty())
        {
            string label = StringManager::GetPCSTRById(BOOK_IDENTIFIER);
            label += ":\t" + identifier;
            m_identifier.SetText(label.c_str());
        }

        string issuedTime = m_bookDetailedInfo->GetIssuedTime();
        m_issued.SetVisible(!issuedTime.empty());
        if (!issuedTime.empty())
        {
            string label = StringManager::GetPCSTRById(BOOK_ISSUED);
            label += ":\t" + issuedTime;
            m_issued.SetText(label.c_str());
        }

        string updateTime = m_bookDetailedInfo->GetUpdateTime();
        m_updateTime.SetVisible(!updateTime.empty());
        m_moreUpdate.SetVisible(!updateTime.empty() && m_bookDetailedInfo->HasChangeLog());
        if (!updateTime.empty())
        {
            string label = StringManager::GetPCSTRById(BOOK_UPDATE);
            label += ":\t" + updateTime;
            m_updateTime.SetText(label.c_str());
        }

        int size = m_bookDetailedInfo->GetSize();
        m_size.SetVisible(size > 0);
        if (size > 0)
        {
            char szTmp[64] = {0};
            string label = StringManager::GetPCSTRById(BOOK_SIZE);
            if (m_bookDetailedInfo->GetSize() >= 1024768)
            {
                snprintf(szTmp, DK_DIM(szTmp), "%s\t%.2f MB",label.c_str(), (float)size / 1048576.0);
            }
            else
            {
                snprintf(szTmp, DK_DIM(szTmp), "%s\t%.2f KB", label.c_str(), (float)size / 1024.0);
            }
            m_size.SetText(szTmp);
        }

        int wordCount = m_bookDetailedInfo->GetWordCount();
        m_wordCount.SetVisible(wordCount > 0);
        if (wordCount > 0)
        {
            char szTmp[64] = {0};
            snprintf(szTmp, DK_DIM(szTmp), StringManager::GetPCSTRById(BOOK_WORDCOUNT), wordCount/10000);
            m_wordCount.SetText(szTmp);
        }
        Layout();
    }
}

bool UIBookStoreBookRightsInfo::OnMessageBookDetailedInfoUpdate(const EventArgs& args)
{
    const BookInfoUpdateArgs& msgArgs = (const BookInfoUpdateArgs&)args;
    if (m_bookId == msgArgs.bookId && msgArgs.succeeded)
    {
        SetBookInfo(msgArgs.bookInfo);
    }
    return true;
}

void UIBookStoreBookRightsInfo::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    if (!m_bookDetailedInfo)
    {
        return;
    }

    switch (dwCmdId)
    {
    case ID_BOOKSTORE_VIEWUPDATE_ALL:
        {
            DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s ID_BOOKSTORE_VIEWUPDATE_ALL", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            UIBookStoreBookChangeLogPage *page = new UIBookStoreBookChangeLogPage(m_bookDetailedInfo->GetBookId(), m_bookDetailedInfo->GetTitle());
            if (page)
            {
                page->MoveWindow(0, 0, DeviceInfo::GetDisplayScreenWidth(), DeviceInfo::GetDisplayScreenHeight());
                CPageNavigator::Goto(page);
            }
        }
        break;
    default:
        break;
    }
}

BOOL UIBookStoreBookRightsInfo::OnKeyPress(INT32 iKeyCode)
{
#ifndef KINDLE_FOR_TOUCH
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    switch (iKeyCode)
    {
    case KEY_UP:
    case KEY_DOWN:
        if (m_moreUpdate.IsVisible())
        {
            if (m_moreUpdate.IsFocus())
            {
                m_moreUpdate.SetFocus(FALSE);
            }
            else
            {
                m_moreUpdate.SetFocus(TRUE);
                return FALSE;
            }
        }
        SetFocus(FALSE);
        return TRUE;
    default:
        if(SendHotKeyToChildren(iKeyCode))
        {
            DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s default", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
            return UIContainer::OnKeyPress(iKeyCode);
        }
        break;
    }
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s end", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
#endif
    return FALSE;
}

