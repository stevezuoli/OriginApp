#include "PersonalUI/UIPersonalBookNotesPage.h"
#include "PersonalUI/UIPersonalBookNoteItem.h"
#include "I18n/StringManager.h"
#include "GUI/UISizer.h"
#include "GUI/UIMessageBox.h"
#include "GUI/CTpGraphics.h"
#include "CommonUI/CPageNavigator.h"
#include "CommonUI/UIWeiboSharePage.h"
#include "drivers/DeviceInfo.h"
#include "BookStore/BookStoreInfoManager.h"
#include "BookStoreUI/UIBookDetailedInfoPage.h"
#include "../Common/FileManager_DK.h"
#include "Framework/CNativeThread.h"
#include "Framework/CDisplay.h"
#ifdef KINDLE_FOR_TOUCH
#include "TouchAppUI/BookOpenManager.h"
#include "TouchAppUI/UIBookMenuDlg.h"
#include "TouchAppUI/UIBasicListItem.h"
#include "GUI/GUISystem.h"
#else
#include "DKXManager/DKX/DKXManager.h"
#include "AppUI/BookOpenManager.h"
#include "AppUI/DkxManagerHelper.h"
#include "AppUI/UIBookMenuDlg.h"
#include "AppUI/UIBasicListItem.h"
#endif
#include <tr1/functional>
#include "Thirdparties/EvernoteManager.h"
#include "CommonUI/UIUtility.h"

using namespace dk::bookstore;
using namespace WindowsMetrics;
using namespace dk::thirdparties;

UIPersonalBookNotesPage::UIPersonalBookNotesPage(const std::string& bookId,
                                                 const std::string& bookTitle, 
                                                 const std::string& bookAuthor,
                                                 const std::string& bookCoverUrl,
                                                 const std::string& updateTime,
                                                 const int bookNoteCount)
    : UIBookStoreNavigation(StringManager::GetPCSTRById(PERSONAL_NOTES), false)
    , m_bookId(bookId)
    , m_bookTitle(bookTitle)
    , m_bookAuthor(bookAuthor)
    , m_bookCoverUrl(bookCoverUrl)
    , m_updateTime(updateTime)
    , m_iSelectedItemIndex(-1)
    , m_bookNoteCount(bookNoteCount)
    , m_currentPage(0)
    , m_itemsPerPage(GetWindowMetrics(UIPersonalBookNotesPageItemCountIndex))
    , m_itemHeight(GetWindowMetrics(UIPersonalBookNotesPageItemHeightIndex))
    , m_fileInfo(NULL)
    , m_dkxBlock(NULL)
    , isWaitingToExport(false)
{
#ifdef KINDLE_FOR_TOUCH
    SubscribeEvent(UITouchCommonListBox::EventListBoxTurnPage, 
        m_lstBookNotes,
        std::tr1::bind(
        std::tr1::mem_fn(&UIPersonalBookNotesPage::OnMessageListBoxTurnPage),
        this,
        std::tr1::placeholders::_1)
        );
    SubscribeEvent(UIWindow::EventClick,
            m_bookCover,
            std::tr1::bind(
                std::tr1::mem_fn(&UIPersonalBookNotesPage::OnMessageBookCoverTouch),
                this,
                std::tr1::placeholders::_1)
            );
#else 
    SubscribeEvent(UIBookCoverView::EventBookCoverViewTouched,
            m_bookCover,
            std::tr1::bind(
                std::tr1::mem_fn(&UIPersonalBookNotesPage::OnMessageBookCoverTouch),
                this,
                std::tr1::placeholders::_1)
            );
#endif
    SubscribeEvent(UITouchCommonListBox::EventListBoxSelectedItemChange, 
        m_lstBookNotes,
        std::tr1::bind(
        std::tr1::mem_fn(&UIPersonalBookNotesPage::OnMessageListBoxSelectedItemChanged),
        this,
        std::tr1::placeholders::_1)
        );
    SubscribeEvent(UITouchCommonListBox::EventListBoxSelectedItemOperation, 
        m_lstBookNotes,
        std::tr1::bind(
        std::tr1::mem_fn(&UIPersonalBookNotesPage::OnMessageListBoxSelectedItemOperation),
        this,
        std::tr1::placeholders::_1)
        );
    SubscribeMessageEvent(
            BookStoreInfoManager::EventGetReadingDataUpdate,
            *BookStoreInfoManager::GetInstance(),
            std::tr1::bind(
                std::tr1::mem_fn(
                    &UIPersonalBookNotesPage::OnGetReadingDataUpdate),
                this,
                std::tr1::placeholders::_1));

    SubscribeMessageEvent(
        EvernoteManager::EventEvernoteAuthFinished,
        *EvernoteManager::GetInstance(),
        std::tr1::bind(
            std::tr1::mem_fn(&UIPersonalBookNotesPage::OnEvernoteAuthFinished),
            this,
            std::tr1::placeholders::_1));

    SubscribeMessageEvent(
        EvernoteManager::EventEvernoteGetUserFinished,
        *EvernoteManager::GetInstance(),
        std::tr1::bind(
            std::tr1::mem_fn(&UIPersonalBookNotesPage::OnEvernoteGetUserFinished),
            this,
            std::tr1::placeholders::_1));    

    SubscribeMessageEvent(
        EvernoteManager::EventEvernoteExportFinished,
        *EvernoteManager::GetInstance(),
        std::tr1::bind(
            std::tr1::mem_fn(&UIPersonalBookNotesPage::OnEvernoteExportFinished),
            this,
            std::tr1::placeholders::_1));

    m_fileInfo = CDKFileManager::GetFileManager()->GetFileByBookId(m_bookId.c_str());
    InitUI();
}

UIPersonalBookNotesPage::~UIPersonalBookNotesPage()
{
    m_lstBookNotes.ClearItemList();
#ifdef KINDLE_FOR_TOUCH
    if (m_dkxBlock)
    {
        DKXManager::DestoryDKXBlock(m_dkxBlock);
    }
#endif
}

void UIPersonalBookNotesPage::InitUI()
{
    m_bookCover.Initialize(CUSTOMIZED, IMAGE_ICON_COVER_BACKGROUND, m_bookCoverUrl);
    UIText* textTitle = m_bookCover.GetTextWidget();
    if (textTitle)
    {
        textTitle->SetFontSize(GetWindowFontSize(UIPersonalBookNotesPageCoverTitleIndex));
    }
    m_bookCover.SetBookTitle(m_bookTitle.c_str());
    m_labelBookTitle.SetText(m_bookTitle);
    char buf[64] = {0};
    snprintf(buf, DK_DIM(buf), "%s %s", StringManager::GetPCSTRById(BOOK_AUTHOR), m_bookAuthor.c_str());
    m_labelBookAuthor.SetVisible(!m_bookAuthor.empty());
    m_labelBookAuthor.SetText(buf);
    snprintf(buf, DK_DIM(buf), "%s: %d%s", StringManager::GetPCSTRById(NOTE), m_bookNoteCount, StringManager::GetPCSTRById(BOOK_TIAO));
    m_labelBookNoteCount.SetText(buf);
    size_t pos = m_updateTime.find_first_of(' ');
    if (pos != string::npos)
    {
        snprintf(buf, DK_DIM(buf), "%s: %s", StringManager::GetPCSTRById(ADD_DATE), m_updateTime.substr(0, pos+1).c_str());
        m_labelUpdateTime.SetText(buf);
    }
    m_lstBookNotes.SetItemHeight(m_itemHeight);
#ifdef KINDLE_FOR_TOUCH
    m_btnEmptyAllNotes.Initialize(ID_BTN_EMPTY_ALL, StringManager::GetPCSTRById(EMPTY_ALL_NOTES), GetWindowFontSize(FontSize18Index));
    m_btnEmptyAllNotes.SetBackgroundImage(IMAGE_TOUCH_WEIBO_SHARE);

    m_btnExportAllNotes.Initialize(ID_BTN_EXPORT_NOTE, StringManager::GetPCSTRById(EXPORT_TO_EVERNOTE), GetWindowFontSize(FontSize18Index));
    m_btnExportAllNotes.SetBackgroundImage(IMAGE_TOUCH_WEIBO_SHARE);

#else
    m_btnEmptyAllNotes.Initialize(ID_BTN_EMPTY_ALL, StringManager::GetPCSTRById(EMPTY_ALL_NOTES)
        ,KEY_RESERVED, GetWindowFontSize(FontSize18Index), ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));

    m_btnExportAllNotes.Initialize(ID_BTN_EXPORT_NOTE, StringManager::GetPCSTRById(EXPORT_TO_EVERNOTE)
        ,KEY_RESERVED, GetWindowFontSize(FontSize18Index), ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
#endif
    int top = GetWindowMetrics(UIPersonalBookNotesPageFetchInfoTopIndex);
    m_fetchInfo.MoveWindow(0, top, m_iWidth, m_iHeight - top);
    m_fetchInfo.Layout();

    AppendChild(&m_bookCover);
    AppendChild(&m_labelBookTitle);
    AppendChild(&m_labelBookAuthor);
    AppendChild(&m_labelBookNoteCount);
    AppendChild(&m_labelUpdateTime);
    AppendChild(&m_lstBookNotes);
    AppendChild(&m_labelItemCount);
    AppendChild(&m_labelPageNum);
    AppendChild(&m_btnEmptyAllNotes);
    AppendChild(&m_btnExportAllNotes);
}

void UIPersonalBookNotesPage::ShowEmptyNotesMsgBox()
{
    //if (BookStoreInfoManager::GetInstance()->SaveBookNoteToEvernoteFile(m_bookTitle, m_bookAuthor))
    //{
    //    return;
    //}
    UIMessageBox msgBox(this, StringManager::GetPCSTRById(EMPTY_ALL_NOTES_TIPS), MB_OK | MB_CANCEL);
    if (msgBox.DoModal() == MB_OK)
    {
        EmptyAllBookNote();
        UpdateListBox();
        UpdatePageNum();
    }
}

void UIPersonalBookNotesPage::GotoNoteInBook()
{
    if (!m_fileInfo)
    {
        return;
    }
#ifdef KINDLE_FOR_TOUCH
    if (!m_dkxBlock)
    {
        m_dkxBlock = DKXManager::CreateDKXBlock(m_fileInfo->GetFilePath());
        if (NULL == m_dkxBlock)
        {
            return;
        }
    }

    int itemIndex = m_currentPage*m_itemsPerPage + m_iSelectedItemIndex;
    if (itemIndex >= 0 && (unsigned int)itemIndex < m_objectList.size() && m_objectList.size() <= m_readingBookInfo.GetReadingDataItemCount())
    {
        CT_ReadingDataItemImpl* pReadingDataItem = (CT_ReadingDataItemImpl*)(m_objectList[itemIndex].get());
        if (pReadingDataItem)
        {
            ICT_ReadingDataItem *pclsProgress = DKXReadingDataItemFactory::CreateReadingDataItem(ICT_ReadingDataItem::PROGRESS);
            if(pclsProgress && m_dkxBlock->GetProgress(pclsProgress))
            {
                pclsProgress->SetCreateTime(pReadingDataItem->GetCreateTime());
                pclsProgress->SetLastModifyTime(pReadingDataItem->GetLastModifyTime());
                pclsProgress->SetBeginPos(pReadingDataItem->GetBeginPos());
                pclsProgress->SetEndPos(pReadingDataItem->GetEndPos());
                m_dkxBlock->SetProgress(pclsProgress);
            }
            m_dkxBlock->Serialize();
            SafeDeletePointerEx(pclsProgress);
        }
    }
#else
    DKXManager* dkxManager = DKXManager::GetInstance();
    if(!dkxManager)
    {
        return;
    }

    int itemIndex = m_currentPage*m_itemsPerPage + m_iSelectedItemIndex;
    if (itemIndex >= 0 && (unsigned int)itemIndex < m_objectList.size() && m_objectList.size() <= m_readingBookInfo.GetReadingDataItemCount())
    {
        CT_ReadingDataItemImpl* pReadingDataItem = (CT_ReadingDataItemImpl*)(m_objectList[itemIndex].get());
        if (pReadingDataItem)
        {
            CT_ReadingDataItemImpl readingBookmark;
            if(DkxManagerHelper::LoadReadingBookmark(m_fileInfo->GetFilePath(), &readingBookmark))
            {
                readingBookmark.SetCreateTime(pReadingDataItem->GetCreateTime());
                readingBookmark.SetLastModifyTime(pReadingDataItem->GetLastModifyTime());
                readingBookmark.SetBeginPos(pReadingDataItem->GetBeginPos());
                readingBookmark.SetEndPos(pReadingDataItem->GetEndPos());
                dkxManager->SetProgress(m_fileInfo->GetFilePath(), &readingBookmark);
                dkxManager->SaveCurDkx();
            }
        }
    }

#endif
    OpenBook();
}

void UIPersonalBookNotesPage::OnCommand(DWORD dwCmdId, UIWindow* pSender, DWORD dwParam)
{
    switch (dwCmdId)
    {
        case ID_BTN_EMPTY_ALL:
            ShowEmptyNotesMsgBox();
            break;
        case ID_BTN_CHECK_NOTE_IN_BOOK:
            GotoNoteInBook();
            break;
        case ID_BTN_SHARE_NOTE:
            ShareToWeibo(); 
            break;
        case ID_BTN_DELETE_NOTE:
            ShowNoteDeleteMsgBox();
            break;
        case ID_BTN_EXPORT_NOTE:
            ExportToEvernote();
            break;
        default:
            break;
    }

    return UIBookStoreNavigation::OnCommand(dwCmdId, pSender, dwParam);
}

void UIPersonalBookNotesPage::ExportToEvernote()
{
    if(!UIUtility::CheckAndReConnectWifi())
    {
        return;
    }

    if (EvernoteManager::GetInstance()->IsBusy())
    {
        UIUtility::SetScreenTips(StringManager::GetPCSTRById(EXPORT_WAIT_FOR_LAST_TASK));
        return;
    }

    // Get token and user to see whether we can export directly
    m_btnExportAllNotes.SetEnable(FALSE);
    isWaitingToExport = true;
    EvernoteTokenPtr token = EvernoteManager::GetInstance()->GetCurrentToken();
    if (token == 0 || token->token().empty())
    {
        if (EvernoteManager::GetInstance()->Auth())
        {
            // Display "Authorizing"
            UIUtility::SetScreenTips(StringManager::GetPCSTRById(EXPORT_AUTHORIZING));
        }
        else
        {
            isWaitingToExport = false;
            UIUtility::SetScreenTips(StringManager::GetPCSTRById(EXPORT_AUTHORIZING_FAILED));
            m_btnExportAllNotes.SetEnable(TRUE);
        }
        return;
    }

    ImplementExportToEvernote();
}

bool UIPersonalBookNotesPage::OnEvernoteAuthFinished(const EventArgs& args)
{
    const EvernoteTokenArgs& tokenArgs = (const EvernoteTokenArgs&)args;
    if (isWaitingToExport && tokenArgs.succeeded)
    {
        UIUtility::SetScreenTips(StringManager::GetPCSTRById(EXPORT_FETCHING_USER_INFO));
        return EvernoteManager::GetInstance()->FetchUser();
    }

    // Display Authorization failed
    if (isWaitingToExport)
    {
        UIUtility::SetScreenTips(StringManager::GetPCSTRById(EXPORT_AUTHORIZING_FAILED));
        isWaitingToExport = false;
        m_btnExportAllNotes.SetEnable(TRUE);
    }
    return true;
}

bool UIPersonalBookNotesPage::OnEvernoteGetUserFinished(const EventArgs& args)
{
    const EvernoteGetUserArgs& userArgs = (const EvernoteGetUserArgs&)args;
    if (isWaitingToExport && userArgs.succeeded)
    {
        return ImplementExportToEvernote();
    }

    // Display Authorization failed
    if (isWaitingToExport)
    {
        UIUtility::SetScreenTips(StringManager::GetPCSTRById(EXPORT_FETCHING_USER_INFO_FAILED));
        isWaitingToExport = false;
        m_btnExportAllNotes.SetEnable(TRUE);
    }
    return true;
}

bool UIPersonalBookNotesPage::ImplementExportToEvernote()
{
    std::string content;
    BookStoreInfoManager* pBookStoreInfoManager = BookStoreInfoManager::GetInstance();
    if (pBookStoreInfoManager)
    {
        content = pBookStoreInfoManager->GetBookNote();
        if (!content.empty() && EvernoteManager::GetInstance()->ExportCloudNote(content, m_bookTitle, m_bookAuthor))
        {
            UIUtility::SetScreenTips(StringManager::GetPCSTRById(EXPORT_IS_EXPORTING), 100000);
            return true;
        }
    }

    if (isWaitingToExport)
    {
        UIUtility::SetScreenTips(StringManager::GetPCSTRById(EXPORT_EXPORTING_FAILED));
        isWaitingToExport = false;
        m_btnExportAllNotes.SetEnable(TRUE);
    }
    return false;
}

bool UIPersonalBookNotesPage::OnEvernoteExportFinished(const EventArgs& args)
{
    const EvernoteExportArgs& exportArgs = (const EvernoteExportArgs&)args;
    if (exportArgs.book_name == m_bookTitle && exportArgs.author == m_bookAuthor)
    {
        if (exportArgs.succeeded)
            UIUtility::SetScreenTips(StringManager::GetPCSTRById(EXPORT_SUCCEEDED));
        else
            UIUtility::SetScreenTips(StringManager::GetPCSTRById(EXPORT_EXPORTING_FAILED));
    }

    if (isWaitingToExport)
    {
        m_btnExportAllNotes.SetEnable(TRUE);
        isWaitingToExport = false;
    }
    return true;
}

void UIPersonalBookNotesPage::ShareToWeibo()
{
    int itemIndex = m_currentPage*m_itemsPerPage + m_iSelectedItemIndex;
    if (itemIndex >= 0 && (unsigned int)itemIndex < m_objectList.size() && m_objectList.size() <= m_readingBookInfo.GetReadingDataItemCount())
    {
        std::string sharedContent = UIWeiboSharePage::PruneStringForDKComment(m_bookTitle, ((CT_ReadingDataItemImpl*)(m_objectList[itemIndex].get()))->GetBookContent());
        UIWeiboSharePage* pPage = new UIWeiboSharePage(sharedContent.c_str());
        if (pPage)
        {
            CPageNavigator::Goto(pPage);
        }
    }
}

void UIPersonalBookNotesPage::EmptyAllBookNote()
{
    m_objectList.clear();
    m_readingBookInfo.ClearReadingCommentDataItem();
    BookStoreInfoManager* pBookStoreInfoManager = BookStoreInfoManager::GetInstance();
    if (pBookStoreInfoManager)
    {
        pBookStoreInfoManager->PutReadingData(m_bookId.c_str(), m_bookTitle.c_str(), m_readingBookInfo.GetSyncResult().GetLatestVersion(),
                m_readingBookInfo, !m_bookCoverUrl.empty(), false);
    }
}

UISizer* UIPersonalBookNotesPage::CreateMidSizer()
{
    const int titleFontSize = GetWindowFontSize(UIPersonalBookNotesPageBookTitleIndex);
    const int infoFontSize = GetWindowFontSize(UIPersonalBookNotesPageBookInfoIndex);
	const int elementBorder = GetWindowMetrics(UIPersonalBookNotesPageBookTitleBottomBorderIndex);
    m_labelBookTitle.SetFontSize(titleFontSize);
    m_labelBookAuthor.SetFontSize(infoFontSize);
    m_labelBookNoteCount.SetFontSize(infoFontSize);
    m_labelUpdateTime.SetFontSize(infoFontSize);
    m_labelItemCount.SetFontSize(infoFontSize);
    m_labelPageNum.SetFontSize(infoFontSize);
    m_lstBookNotes.SetMinHeight(m_itemsPerPage * m_itemHeight);
    m_bookCover.SetMinSize(
            GetWindowMetrics(UIPersonalBookNotesPageBookCoverWidthIndex), GetWindowMetrics(UIPersonalBookNotesPageBookCoverHeightIndex));
    m_labelPageNum.SetAlign(ALIGN_RIGHT);
#ifdef KINDLE_FOR_TOUCH
    m_btnEmptyAllNotes.SetMinSize(
            GetWindowMetrics(UIPersonalBookNotesPageBtnWidthIndex), GetWindowMetrics(UIPersonalBookNotesPageBtnHeightIndex));
    m_btnExportAllNotes.SetMinSize(
            GetWindowMetrics(UIPersonalBookNotesPageBtnWidthIndex), GetWindowMetrics(UIPersonalBookNotesPageBtnHeightIndex));
#endif

    UISizer* middleSizer = new UIBoxSizer(dkVERTICAL);
    if (middleSizer)
    {
        middleSizer->AddSpacer(GetWindowMetrics(UIPersonalBookNotesPageBookCoverTopSpaceIndex));
        UISizer* bookInfoSizer = new UIBoxSizer(dkHORIZONTAL);
        if (bookInfoSizer)
        {
            bookInfoSizer->Add(&m_bookCover);
            bookInfoSizer->AddSpacer(GetWindowMetrics(UIPersonalBookNotesPageBookCoverRightSpaceIndex));

            UISizer* middleRightSizer = new UIBoxSizer(dkVERTICAL);
            if (middleRightSizer != 0)
            {
                middleRightSizer->AddSpacer(GetWindowMetrics(UIPersonalBookNotesPageBookTitleTopBorderIndex));
                middleRightSizer->Add(&m_labelBookTitle, UISizerFlags().Border(dkBOTTOM, elementBorder));
                middleRightSizer->Add(&m_labelBookAuthor, UISizerFlags().Border(dkBOTTOM, elementBorder/2));
                middleRightSizer->Add(&m_labelBookNoteCount, UISizerFlags().Border(dkBOTTOM, elementBorder/2));
                middleRightSizer->Add(&m_labelUpdateTime, UISizerFlags().Border(dkBOTTOM, elementBorder/2));

                UISizer* bottomSizer = new UIBoxSizer(dkHORIZONTAL);
                bottomSizer->Add(&m_btnEmptyAllNotes, UISizerFlags().Border(dkRIGHT, elementBorder));
                bottomSizer->Add(&m_btnExportAllNotes, UISizerFlags());
                middleRightSizer->Add(bottomSizer);

                bookInfoSizer->Add(middleRightSizer, UISizerFlags(1).Expand());
            }
            middleSizer->Add(bookInfoSizer, UISizerFlags().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIHorizonMarginIndex)));
        }
        middleSizer->AddSpacer(GetWindowMetrics(UIPersonalBookNotesPageBookCoverBottomSpaceIndex));
        middleSizer->Add(&m_lstBookNotes, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIHorizonMarginIndex)));
        middleSizer->AddSpacer(GetWindowMetrics(UIPersonalBookNotesPagePageInfoTopSpaceIndex));

        UISizer* bottomSizer = new UIBoxSizer(dkHORIZONTAL);
        if (bottomSizer)
        {
            bottomSizer->Add(&m_labelItemCount, 1);
            bottomSizer->Add(&m_labelPageNum, 1);
            middleSizer->Add(bottomSizer, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIHorizonMarginIndex)));
        }
    }


    return middleSizer;
}

dk::bookstore::FetchDataResult UIPersonalBookNotesPage::FetchInfo()
{
    m_btnEmptyAllNotes.SetEnable(false);
    m_btnExportAllNotes.SetEnable(false);
    SetFetchStatus(FETCHING);
    FetchDataResult fdr = FDR_PENDING;
    if (!m_bookId.empty())
    {
        BookStoreInfoManager* pManager = BookStoreInfoManager::GetInstance();
        if (pManager)
        {
            fdr = pManager->FetchReadingData(m_bookId.c_str(), 0, !m_bookCoverUrl.empty());
        }
        else
        {
            SetFetchStatus(FETCH_FAIL);
        }
    }

    return fdr;
}

bool UIPersonalBookNotesPage::OnGetReadingDataUpdate(const EventArgs& args)
{
    const ReadingDataSyncUpdateArgs& readingDataSyncUpdateArgs = (const ReadingDataSyncUpdateArgs&)args;
    const ReadingDataSyncResult& result = readingDataSyncUpdateArgs.result;
    if (m_bookId.compare(result.GetReadingData().GetBookId()) 
            || CPageNavigator::GetCurrentPage() != this)
    {
        return true;
    }

    if (!readingDataSyncUpdateArgs.succeeded)
    {
        SetFetchStatus(FETCH_FAIL);
        return true;
    }

    if (result.HasReadingData())
    {
        m_readingBookInfo = result.GetReadingData();
        if (m_readingBookInfo.HasSyncResult())
        {
            m_objectList.clear();
            const SyncResult& syncResult = m_readingBookInfo.GetSyncResult();
            if (syncResult.GetResultCode() == SRC_SUCCEEDED || syncResult.GetResultCode() == SRC_CONFLICT)
            {
                size_t itemCount = m_readingBookInfo.GetReadingDataItemCount();
                for(size_t i = 0; i < itemCount; ++i)
                {
                    if ((m_readingBookInfo.GetReadingDataItem(i).GetType() == ReadingDataItem::COMMENT)
                    && !(m_readingBookInfo.GetReadingDataItem(i).GetRefContent().empty() && 
                         m_readingBookInfo.GetReadingDataItem(i).GetContent().empty())
                    &&   m_readingBookInfo.GetReadingDataItem(i).GetBeginRefPos().IsValid())
                    {
                        m_objectList.push_back(ICT_ReadingDataItemSPtr(
                                    dk::bookstore::sync::CreateICTReadingDataItemFromReadingDataItem(m_readingBookInfo.GetReadingDataItem(i))));
                    }
                }
                std::sort(m_objectList.begin(), m_objectList.end(), ICT_ReadingDataItem::CompareByCreateTime);
                m_bookNoteCount = m_objectList.size();
                char buf[64] = {0};
                snprintf(buf, DK_DIM(buf), "%s: %d%s", StringManager::GetPCSTRById(NOTE), m_bookNoteCount, StringManager::GetPCSTRById(BOOK_TIAO));
                m_labelBookNoteCount.SetText(buf);
            }
        }
    }
    SetFetchStatus(m_objectList.size() > 0 ? FETCH_SUCCESS : FETCH_NOELEM);
    m_btnEmptyAllNotes.SetEnable(m_objectList.size() > 0);
    m_btnExportAllNotes.SetEnable(m_objectList.size() > 0);
    UpdateListBox();
    Layout();

#ifndef KINDLE_FOR_TOUCH
    if (m_objectList.empty())
    {
        m_btnRefresh.SetFocus(true);
    }
    else
    {
        m_lstBookNotes.SetSelectedItem(0);
        m_lstBookNotes.SetFocus(true);
    }
#endif
    return true;
}

void UIPersonalBookNotesPage::UpdateListBox()
{
    if (m_currentPage < 0)
    {
        return;
    }
    const size_t itemCount = m_objectList.size();
    const int startIndex = m_currentPage * m_itemsPerPage;
    size_t i = 0;
    for (i = 0; i < (size_t)m_itemsPerPage; ++i)
    {
        const size_t index = startIndex + i;
        if (index >= itemCount)
        {
            break;
        }

        UIPersonalBookNoteItem* pItem = NULL;
        if (i == m_lstBookNotes.GetChildrenCount())
        {
            pItem = new UIPersonalBookNoteItem(m_bookTitle);
            if (pItem)
            {
                pItem->SetMinHeight(m_itemHeight);
                pItem->MoveWindow(0, i*m_itemHeight, m_iWidth - GetWindowMetrics(UIHorizonMarginIndex), m_itemHeight);
                m_lstBookNotes.AddItem(pItem);
            }
        }
        else
        {
            pItem = (UIPersonalBookNoteItem*)m_lstBookNotes.GetChildByIndex(i);
        }
        if (pItem)
        {
            pItem->SetVisible(true);
            pItem->SetBookNoteInfoSPtr(m_objectList[startIndex + i]);
        }
    }

    for (;i < m_lstBookNotes.GetChildrenCount(); ++i)
    {
        UIWindow* pChild = m_lstBookNotes.GetChildByIndex(i);
        if (pChild)
        {
            pChild->SetVisible(false);
        }
    }

    UpdatePageNum();
}

void UIPersonalBookNotesPage::UpdatePageNum()
{

    if (m_objectList.empty())
    {
        m_labelPageNum.SetVisible(false);
        m_labelItemCount.SetVisible(false);
    }
    else
    {
        char buf[64] = {0};
        size_t totalElemCount = m_objectList.size();
        size_t totalPage = totalElemCount%m_itemsPerPage ? totalElemCount/m_itemsPerPage + 1: totalElemCount/m_itemsPerPage;
        snprintf(buf, DK_DIM(buf), "%s%d%s", StringManager::GetPCSTRById(BOOK_TOTAL), totalElemCount, StringManager::GetPCSTRById(BOOK_TIAO));
        m_labelItemCount.SetText(buf);
        snprintf(buf, DK_DIM(buf), "%d/%d%s", m_currentPage + 1, totalPage, StringManager::GetPCSTRById(BOOK_PAGE));
        m_labelPageNum.SetText(buf);
        m_labelPageNum.SetVisible(true);
        m_labelItemCount.SetVisible(true);
    }
}

HRESULT UIPersonalBookNotesPage::DrawBackGround(DK_IMAGE drawingImg)
{
    HRESULT hr(S_OK);
    CTpGraphics grf(drawingImg);
#ifdef KINDLE_FOR_TOUCH
    grf.EraserBackGround(ColorManager::knWhite);
#else
    grf.EraserBackGround();
#endif
    hr = grf.DrawLine(m_lstBookNotes.GetX(),
            m_lstBookNotes.GetY() - 2,
            m_lstBookNotes.GetWidth(), 
            2,
            DOTTED_STROKE);
#ifndef KINDLE_FOR_TOUCH
    int lineTop = m_titleBar.GetHeight();
    grf.FillRect(0, lineTop, m_iWidth, lineTop + 2, ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
#endif

    const int lineTitle = m_title.GetAbsoluteY() + m_title.GetHeight();
    const int titleLineHeight = GetWindowMetrics(UIBookStoreNavigationTitleBottomLineHeightIndex);
    grf.DrawLine(0, lineTitle, m_iWidth, titleLineHeight, SOLID_STROKE);

    return hr;
}

bool UIPersonalBookNotesPage::OpenBook()
{
    if(!m_fileInfo)
    {
        return false;
    }
    int iFileId = m_fileInfo->GetFileID();
#ifdef KINDLE_FOR_TOUCH
    BookOpenManager::GetInstance()->Initialize(
            GUISystem::GetInstance()->GetTopFullScreenContainer(),
            iFileId, 
            10, 
            0);

    SNativeMessage msg;
    msg.iType = KMessageOpenBook;
    CNativeThread::Send(msg);
#else
    BookOpenManager bookOpen(this, iFileId, 10, 10);
    if(!bookOpen.OpenBook())
    {
       UIMessageBox messagebox(this, StringManager::GetStringById(CANNOT_OPEN_BOOK), MB_OK);
       messagebox.DoModal();
    }
#endif

    return true;
}

#ifdef KINDLE_FOR_TOUCH
bool UIPersonalBookNotesPage::OnMessageListBoxTurnPage(const EventArgs& args)
{
    const TouchCommonListBoxTurnPageEventArgs& msgArgs = 
        dynamic_cast<const TouchCommonListBoxTurnPageEventArgs&>(args);
    TurnPageUpOrDown(msgArgs.isDownPage);
    return true;
}
#endif

bool UIPersonalBookNotesPage::OnMessageBookCoverTouch(const EventArgs& args)
{
    if (OpenBook())
    {
        return true;
    }

    if (!m_bookCoverUrl.empty())
    {
        UIPage* pPage = new UIBookDetailedInfoPage(m_bookId, m_bookTitle);
        if(pPage)
        {
            pPage->MoveWindow(0, 0, DeviceInfo::GetDisplayScreenWidth(), DeviceInfo::GetDisplayScreenHeight());
            CPageNavigator::Goto(pPage);
        }
    }

    return true;
}

BOOL UIPersonalBookNotesPage::OnKeyPress(INT32 iKeyCode)
{
#ifndef KINDLE_FOR_TOUCH
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (!SendKeyToChildren(iKeyCode))
    {
        DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s if (!SendKeyToChildren(iKeyCode))", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
        return FALSE;
    }
    
    switch (iKeyCode)
    {
    case KEY_LPAGEDOWN:
    case KEY_RPAGEDOWN:
    case KEY_SPACE:
        TurnPageUpOrDown(TRUE);
        break;
    case KEY_LPAGEUP:
    case KEY_RPAGEUP:
        TurnPageUpOrDown(FALSE);
        break;
    case KEY_LEFT:
        if (m_lstBookNotes.IsFocus())
        {
            m_iSelectedItemIndex = m_lstBookNotes.GetSelectedItem();
            if (m_fileInfo)
            {
                ShowMenuDlg();
            }
            else
            {
                ShowNoteDeleteMsgBox();
            }
        }
        else if (m_btnEmptyAllNotes.IsFocus())
        {
            m_btnEmptyAllNotes.SetFocus(false);
            m_bookCover.SetFocus(true);
        }
        break;
    case KEY_RIGHT:
        if (m_bookCover.IsFocus())
        {
            m_btnEmptyAllNotes.SetFocus(true);
            m_bookCover.SetFocus(false);
        }
        break;
    //case KEY_UP:
        //if (!IsFocus() && (m_lstBookNotes.GetVisibleItemNum() > 0))
        //{
            //m_lstBookNotes.SetSelectedItem(m_lstBookNotes.GetVisibleItemNum() - 1);
            //SetFocus(true);
            //m_lstBookNotes.SetFocus(true);
        //}
        //else
        //{
            //m_lstBookNotes.SetFocus(false);
            //SetFocus(false);
            //return TRUE;
        //}
        //break;
    //case KEY_DOWN:
        //if (!IsFocus() && (m_lstBookNotes.GetVisibleItemNum() > 0))
        //{
            //m_lstBookNotes.SetSelectedItem(0);
            //SetFocus(true);
            //m_lstBookNotes.SetFocus(true);
        //}
        //else
        //{
            //m_lstBookNotes.SetFocus(false);
            //SetFocus(false);
            //return TRUE;
        //}
        //break;
    default:
        if(SendHotKeyToChildren(iKeyCode))
        {
            return UIBookStoreNavigation::OnKeyPress(iKeyCode);
        }
        break;
    }
#else
    return UIBookStoreNavigation::OnKeyPress(iKeyCode);
#endif
    return FALSE;
}

bool UIPersonalBookNotesPage::OnMessageListBoxSelectedItemChanged(const EventArgs& args)
{
    const TouchCommonListBoxSelectedChangedEventArgs& msgArgs = 
        dynamic_cast<const TouchCommonListBoxSelectedChangedEventArgs&>(args);
    int selectedItem = msgArgs.m_selectedItem;
    bool bRet = false;
    if(selectedItem >= 0 && selectedItem < (int)m_lstBookNotes.GetChildrenCount())
    {
        UIListItemBase *pItem = (UIListItemBase *)m_lstBookNotes.GetChildByIndex(selectedItem);
        if (pItem)
        {
            pItem->SetHighLight(true);
            bRet = pItem->ResponseTouchTap();
            pItem->SetHighLight(false);
        }
    }
    return bRet;
}

void UIPersonalBookNotesPage::ShowMenuDlg()
{
    std::vector<int> btnIDs;
    btnIDs.push_back(ID_BTN_CHECK_NOTE_IN_BOOK);
    //btnIDs.push_back(ID_BTN_SHARE_NOTE);
    btnIDs.push_back(ID_BTN_DELETE_NOTE);
    btnIDs.push_back(ID_INVALID);

    std::vector<int> strIDs;
    strIDs.push_back(CHECK_NOTE_IN_BOOK);
    //strIDs.push_back(SHARE_NOTE);
    strIDs.push_back(TOUCH_DELETE);
    strIDs.push_back(-1);
#ifdef KINDLE_FOR_TOUCH
    UIBookMenuDlg dlgMenu(this, btnIDs, strIDs); 
    dlgMenu.MakeCenter(dlgMenu.GetWidth(), dlgMenu.GetHeight());
#else
    vector<char> customizedShortKeys;
    customizedShortKeys.push_back('A');
    customizedShortKeys.push_back('B');
    UIBookMenuDlg dlgMenu(this, "",  btnIDs, strIDs, customizedShortKeys); 
    UIListItemBase *pItem = (UIListItemBase *)m_lstBookNotes.GetChildByIndex(m_iSelectedItemIndex);
    if (pItem)
    {
        dlgMenu.MoveWindow(GetWindowMetrics(UIHorizonMarginIndex)
                , pItem->GetY() + m_lstBookNotes.GetAbsoluteY() + pItem->GetHeight() - dlgMenu.GetHeight()
                , m_iWidth - (GetWindowMetrics(UIHorizonMarginIndex) << 1)
                , dlgMenu.GetHeight());
    }
#endif
    if (dlgMenu.DoModal() == IDOK)
    {
        OnCommand(dlgMenu.GetCommandId(), NULL, 0);
    }
}

bool UIPersonalBookNotesPage::OnMessageListBoxSelectedItemOperation(const EventArgs& args)
{
    const TouchCommonListBoxSelectedChangedEventArgs& msgArgs = 
        dynamic_cast<const TouchCommonListBoxSelectedChangedEventArgs&>(args);
    int selectedItem = msgArgs.m_selectedItem;
    bool bRet = false;
    if(selectedItem >= 0 && selectedItem < (int)m_lstBookNotes.GetChildrenCount())
    {
        UIListItemBase *pItem = (UIListItemBase *)m_lstBookNotes.GetChildByIndex(selectedItem);
        if (pItem)
        {
            pItem->SetHighLight(true);
            m_iSelectedItemIndex = selectedItem;
            if (m_fileInfo)
            {
                ShowMenuDlg();
            }
            else
            {
                ShowNoteDeleteMsgBox();
            }
            pItem->SetHighLight(false);
        }
    }
    return bRet;
}

void UIPersonalBookNotesPage::ShowNoteDeleteMsgBox()
{
    UIMessageBox msgBox(this, StringManager::GetPCSTRById(NOTE_DELETE_TIPS), MB_OK | MB_CANCEL);
    if (msgBox.DoModal() == MB_OK)
    {
        DeleteBookNote();
        UpdateListBox();
        UpdatePageNum();
    }
}

void UIPersonalBookNotesPage::DeleteBookNote()
{
    int itemIndex = m_currentPage*m_itemsPerPage + m_iSelectedItemIndex;
    if (itemIndex >= 0 && (unsigned int)itemIndex < m_objectList.size() && m_objectList.size() <= m_readingBookInfo.GetReadingDataItemCount())
    {
        int indexInReadingBookInfo = -1;
        for ( size_t i = 0; i < m_readingBookInfo.GetReadingDataItemCount(); ++i)
        {
            if (dk::bookstore::sync::IsEqual(*m_objectList[itemIndex], m_readingBookInfo.GetReadingDataItem(i)))
            {
                indexInReadingBookInfo = i;
                break;
            }
        }
        if (indexInReadingBookInfo == -1)
        {
            return;
        }
        m_objectList.erase(m_objectList.begin() + itemIndex);
        m_readingBookInfo.RemoveReadingDataItem(indexInReadingBookInfo);
        BookStoreInfoManager* pBookStoreInfoManager = BookStoreInfoManager::GetInstance();
        if (pBookStoreInfoManager)
        {
            pBookStoreInfoManager->PutReadingData(m_bookId.c_str(), m_bookTitle.c_str(), m_readingBookInfo.GetSyncResult().GetLatestVersion(), 
                    m_readingBookInfo, !m_bookCoverUrl.empty(), false);
        }
    }
}

bool UIPersonalBookNotesPage::TurnPageUpOrDown(bool pageDown)
{
    int currentPage = m_currentPage;
    size_t totalPage = m_objectList.size()%m_itemsPerPage ? m_objectList.size() / m_itemsPerPage + 1: m_objectList.size() / m_itemsPerPage;
    pageDown ? ++m_currentPage : --m_currentPage;
    if (m_currentPage <= 0)
    {
        m_currentPage = 0;
    }
    if ((size_t)m_currentPage >= totalPage - 1)
    {
        m_currentPage = totalPage - 1;
    }
    if (m_currentPage != currentPage)
    {
        UpdateListBox();
        UpdatePageNum();
        UpdateWindow();
    }

    return true;
}

