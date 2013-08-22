#include <tr1/functional>
#include "BookStoreUI/UIBookStorePersonCenteredPage.h"
#include "Common/CAccountManager.h"
#include "BookStore/DataUpdateArgs.h"
#include "BookStore/BookDownloadController.h"
#include "CommonUI/UIUtility.h"
#include "CommonUI/UISoftKeyboardIME.h"
#include "CommonUI/UIIMEManager.h"
#include "I18n/StringManager.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "BookStoreUI/UIBookStoreBookItem.h"
#include "Framework/CDisplay.h"
#include "TouchAppUI/UIBookMenuDlg.h"
#include "BookStore/BookStoreOrderManager.h"
#include "BookStore/BookInfo.h"
#include "Utility/Misc.h"

using namespace dk::bookstore;
using namespace dk::account;
using namespace WindowsMetrics;
using namespace dk::bookstore::model;


UIBookStorePersonCenteredPage::UIBookStorePersonCenteredPage()
    : UIBookStoreTablePage(StringManager::GetPCSTRById(PERSON_BUY_OR_DOWNLOAD))
    , m_activated(false)
    , m_pAllBooksPanel(NULL)
    , m_pNotDownloadPanel(NULL)
    , m_searchMode(false)
{
    SubscribeMessageEvent(BookStoreInfoManager::EventPurchasedListUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStoreTablePage::OnMessageListBoxUpdate),
        this,
        std::tr1::placeholders::_1)
        );

	SubscribeEvent(UIBookStoreBookItemStatusListener::EventBookItemLongTapped, 
        *UIBookStoreBookItemStatusListener::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStorePersonCenteredPage::OnBookItemLongTapped),
        this,
        std::tr1::placeholders::_1)
        );

	SubscribeEvent(BookStoreOrderManager::EventHideBookFinished, 
        *BookStoreOrderManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStorePersonCenteredPage::OnHideBookFinished),
        this,
        std::tr1::placeholders::_1)
        );

	SubscribeEvent(UITableController::EventPanelIsVisible, 
        m_tableController,
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStorePersonCenteredPage::OnPanelVisible),
        this,
        std::tr1::placeholders::_1)
        );

    SubscribeMessageEvent(
            BookDownloadController::EventDownloadBookUpdated,
            *BookDownloadController::GetInstance(),
            std::tr1::bind(
                std::tr1::mem_fn(&UIBookStorePersonCenteredPage::OnDownloadStateUpdate),
                this,
                std::tr1::placeholders::_1));

	Init();
#ifdef KINDLE_FOR_TOUCH
	HookTouch();
#endif
}

UIBookStorePersonCenteredPage::~UIBookStorePersonCenteredPage()
{

}

void UIBookStorePersonCenteredPage::Init()
{
	m_searchBox.SetId(ID_EDIT_SEARCH_BOOK);
	m_searchBox.SetImage(IMAGE_ICON_SEARCH_DISABLE, IMAGE_ICON_SEARCH);
	m_searchBox.SetMinHeight(GetWindowMetrics(UIHomePageSearchDlgHeightIndex));
	AppendChild(&m_searchBox);
	m_searchBox.SetVisible(false);
}

FetchDataResult UIBookStorePersonCenteredPage::FetchInfo() 
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    if (FDR_SUCCESSED != UIBookStoreTablePage::FetchInfo())
    {
        UIBookStoreListBoxPanel* pShowPanel = (UIBookStoreListBoxPanel*)m_tableController.GetShowPanel();
        BookStoreInfoManager* manager = BookStoreInfoManager::GetInstance();
        if (pShowPanel && manager && UIUtility::CheckAndReLogin())
        {
            m_pCurPanel = pShowPanel;
            return manager->FetchPurchasedList();
        }
        else
        {
            SetFetchStatus(FETCH_FAIL);
        }
    }
    UpdateDownloadTasks();
    return FDR_SUCCESSED;
}

void UIBookStorePersonCenteredPage::SetTableController()
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    const int itemCount = GetWindowMetrics(UIBookStoreListBoxPageItemCountIndex);
    const int itemHeight = GetWindowMetrics(UIBookStoreTablePageListBoxItemHeightIndex);
    m_pAllBooksPanel = new UIBookStoreListBoxWithBtn(itemCount, itemHeight);
    if (m_pAllBooksPanel)
    {
        m_tableController.AppendTableBox(ID_BIN_BOOKSTORE_PERSON_PURCHASED, 
            StringManager::GetPCSTRById(BOOKSTORE_PERSONALALLBOOKS),
            m_pAllBooksPanel,
            true
#ifndef KINDLE_FOR_TOUCH
            , 'A'
#endif
            );
        m_pAllBooksPanel->SetOnceAllItems(true);
		m_pAllBooksPanel->SetItemEnableLongClick(true);
    }

    m_pNotDownloadPanel = new UIBookStoreListBoxWithBtn(itemCount, itemHeight);
    if (m_pNotDownloadPanel)
    {
        m_tableController.AppendTableBox(ID_BIN_BOOKSTORE_PERSON_NONDOWNLOAD, 
            StringManager::GetPCSTRById(BOOKSTORE_NON_DOWNLOADED),
            m_pNotDownloadPanel
#ifndef KINDLE_FOR_TOUCH
            , false, 'B'
#endif
            );
        m_pNotDownloadPanel->SetOnceAllItems(true);
        m_pNotDownloadPanel->SetBookInfoType(BS_UNDOWNLOAD | BS_DOWNLOADING | BS_FREE);
		m_pNotDownloadPanel->SetItemEnableLongClick(true);
    }
    AppendChild(&m_tableController);
}

void UIBookStorePersonCenteredPage::OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam)
{
    DebugPrintf(DLC_BOOKSTORE, "%s, %d, %s, %s", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);
    switch (dwCmdId)
    {
#ifdef KINDLE_FOR_TOUCH
    case ID_BTN_TOUCH_BACK:
		{
			if(m_searchMode)
			{
				SwitchSearchMode();
			}
			else
			{
				UIBookStoreTablePage::OnCommand(dwCmdId, pSender, dwParam);
			}
			break;
    	}
#endif
    case ID_BIN_BOOKSTORE_PERSON_PURCHASED:
    case ID_BIN_BOOKSTORE_PERSON_NONDOWNLOAD:
		{
			if(m_searchMode)
			{
				SwitchSearchMode(false);
			}
			FetchInfo();
    	}
        break;
	case ID_BTN_HIDEBOOK:
        {
			BookStoreOrderManager* manager = BookStoreOrderManager::GetInstance();
	        if (manager && m_currentBookInfo.get() && UIUtility::CheckAndReLogin())
	        {
	        	model::BookInfoList hideBookList;
				hideBookList.push_back(m_currentBookInfo);
	            manager->HideBook(hideBookList);
				UIBookStoreListBoxPanel* pShowPanel = (UIBookStoreListBoxPanel*)m_tableController.GetShowPanel();
			    if (pShowPanel)
			    {
			        pShowPanel->RemoveBookItemFromList(m_currentBookInfo->GetBookId());
			    }
	        }
        }
        break;
    case ID_BTN_DOWNLOAD_ALL:
        {
            LPCSTR downloadBtnTitle = m_btnDownload.GetText();
            if (memcmp(downloadBtnTitle, StringManager::GetPCSTRById(BATCH_STOP_DOWNLOAD), sizeof(downloadBtnTitle)) == 0)
            {
                StopAllDownloads();
            }
            else
            {
                DownloadAll();
            }
        }
        break;
	case ID_BTN_SEARCH:
		{
			SwitchSearchMode();
		}
		break;
	case ID_EDIT_SEARCH_BOOK:
	    {
	        SearchPersonalBook();
	    }
	break;
    default:
        UIBookStoreTablePage::OnCommand(dwCmdId, pSender, dwParam);
        break;
    }
}

void UIBookStorePersonCenteredPage::StopAllDownloads()
{
    BookDownloadController * pDownloadManager = BookDownloadController::GetInstance();
    if(pDownloadManager)
    {
        UIBookStoreListBoxPanel* pShowPanel = (UIBookStoreListBoxPanel*)m_tableController.GetShowPanel();
        if (pShowPanel == 0)
        {
            return;
        }
        UIMessageBox messagebox(m_pParent,
                StringManager::GetStringById(pShowPanel == m_pAllBooksPanel ? SURE_STOP_UPDATE_ALL : SURE_STOP_DOWNLOAD_ALL),
                MB_OK | MB_CANCEL);
        if(MB_OK == messagebox.DoModal())
        {
            // TODO. Use bookStore controller to control the downloading
            pDownloadManager->StopAll();
            Repaint();
        }
    }
}

void UIBookStorePersonCenteredPage::DownloadAll()
{
    UIBookStoreListBoxPanel* pShowPanel = (UIBookStoreListBoxPanel*)m_tableController.GetShowPanel();
    if (pShowPanel == 0)
    {
        return;
    }

    UIMessageBox messagebox(m_pParent,
            StringManager::GetStringById(pShowPanel == m_pAllBooksPanel ? SURE_UPDATE_ALL : SURE_DOWNLOAD_ALL),
            MB_OK | MB_CANCEL);
    if(MB_OK == messagebox.DoModal())
    {
        if(!UIUtility::CheckAndReConnectWifi())
        {
            return;
        }
        UIUtility::SetScreenTips(StringManager::GetStringById(BATCH_UPDATE_PENDING));
		BookDownloadController* manager = BookDownloadController::GetInstance();
		if(manager)
		{
            FetchDataResult ret = manager->BatchUpdateBooks(pShowPanel == m_pAllBooksPanel ?
                (BS_NEEDSUPDATE) : (BS_UNDOWNLOAD | BS_DOWNLOADING | BS_FREE));
            switch (ret)
            {
            case FDR_PENDING:
                // Top download button
                m_btnDownload.SetText(StringManager::GetPCSTRById(BATCH_STOP_DOWNLOAD));
                UIUtility::SetScreenTips(StringManager::GetStringById(pShowPanel == m_pAllBooksPanel ? BATCH_UPDATE_PENDING : BATCH_DOWNLOAD_PENDING));
                break;
            case FDR_SUCCESSED:
                UIUtility::SetScreenTips(StringManager::GetStringById(pShowPanel == m_pAllBooksPanel ? BATCH_UPDATE_SUCCEED : BATCH_DOWNLOAD_SUCCEED));
                break;
            case FDR_EMPTY:
                UIUtility::SetScreenTips(StringManager::GetStringById(pShowPanel == m_pAllBooksPanel ? BATCH_UPDATE_EMPTY : BATCH_DOWNLOAD_EMPTY));
                break;
            case FDR_FAILED:
                UIUtility::SetScreenTips(StringManager::GetStringById(pShowPanel == m_pAllBooksPanel ? BATCH_UPDATE_FAILED : BATCH_DOWNLOAD_FAILED));
                break;
            default:
                break;
            }
		}        
    }
}

bool UIBookStorePersonCenteredPage::OnDownloadStateUpdate(const EventArgs& args)
{
    if (!IsDisplay())
    {
        return true;
    }
    const BookDownloadStateUpdateArgs& updateArgs = (const BookDownloadStateUpdateArgs&)args;
    return UpdateDownloadTasks();
}

bool UIBookStorePersonCenteredPage::UpdateDownloadTasks()
{
    int working_downloads = BookDownloadController::GetInstance()->GetWorkingAndWaitingDownloads();
    CString prevText = m_btnDownload.GetText();
    CString currentText;
    if (working_downloads <= 0)
    {
        UIBookStoreListBoxPanel* pShowPanel = (UIBookStoreListBoxPanel*)m_tableController.GetShowPanel();
        if (pShowPanel == 0)
        {
            return false;
        }
        if (pShowPanel == m_pAllBooksPanel)
        {
            currentText = StringManager::GetPCSTRById(BATCH_UPDATE);
        }
        else if (pShowPanel == m_pNotDownloadPanel)
        {
            currentText = StringManager::GetPCSTRById(BATCH_DOWNLOAD);
        }
    }
    else
    {
        currentText = StringManager::GetPCSTRById(BATCH_STOP_DOWNLOAD);
    }
    if (prevText != currentText)
    {
        m_btnDownload.SetText(currentText);
    }
    return true;
}

bool UIBookStorePersonCenteredPage::OnPanelVisible(const EventArgs& args)
{
    const EventPanelIsVisibleArgs& panelArgs = (const EventPanelIsVisibleArgs&)args;
    if (panelArgs.m_bIsVisible)
    {
        if (panelArgs.m_pShowPanel == m_pAllBooksPanel)
        {
            m_btnDownload.SetText(StringManager::GetPCSTRById(BATCH_UPDATE));
        }
        else if (panelArgs.m_pShowPanel == m_pNotDownloadPanel)
        {
            m_btnDownload.SetText(StringManager::GetPCSTRById(BATCH_DOWNLOAD));
        }
    }
    return true;
}


void UIBookStorePersonCenteredPage::OnEnter()
{
	m_activated = true;
    CAccountManager* accountManager = CAccountManager::GetInstance();
    if (!accountManager->IsLoggedIn())
    {
        UIUtility::CheckAndReLogin();
    }
    else
    {
        FetchInfo();
    }
    UpdateDownloadTasks();
}

void UIBookStorePersonCenteredPage::OnLeave()
{
    m_activated = false;
}

bool UIBookStorePersonCenteredPage::OnBookItemLongTapped(const EventArgs& args)
{
    if (!m_activated)
    {
        // Only handle the long tap message when this page is activated
        return false;
    }

    //DebugPrintf(DLC_DIAGNOSTIC, "%s, %d, %s, %s start", __FILE__,  __LINE__, GetClassName(), __FUNCTION__);

    CDisplay* pDisplay = CDisplay::GetDisplay();
    UIBookMenuDlg::BookMenuType type = UIBookMenuDlg::BMT_PERSONAL_HIDEBOOKLIST;
    UIBookMenuDlg dlgMenu(this, type);
	int dlgWidth = dlgMenu.GetWidth();
	int dlgHeight = dlgMenu.GetHeight();
	dlgMenu.MoveWindow((pDisplay->GetScreenWidth() - dlgWidth)/2, (pDisplay->GetScreenHeight() - dlgHeight)/2 - m_iTop, dlgWidth, dlgHeight);
    if (dlgMenu.DoModal() == IDOK)
    {
    	UIMessageBox messagebox(m_pParent, StringManager::GetStringById(PERSONAL_MYBOOK_HIDE), MB_OK | MB_CANCEL);
		messagebox.SetTipText(StringManager::GetStringById(PERSONAL_MYBOOK_HIDE_TIP));
        if(MB_OK == messagebox.DoModal())
    	{
    		const BookItemEventArgs& itemArgs = (const BookItemEventArgs&)args;
		    m_currentBookInfo = itemArgs.bookInfo;
			OnCommand(dlgMenu.GetCommandId(), NULL, 0);
    	}
    }
    return true;
}

bool UIBookStorePersonCenteredPage::OnHideBookFinished(const EventArgs& args)
{
	const BookStoreOrderArgs& bookListArgs = (const BookStoreOrderArgs&)args;
	if(bookListArgs.books.empty())
	{
		BookStoreInfoManager* manager = BookStoreInfoManager::GetInstance();
		if(manager)
		{
			model::BookInfoList bookList = manager->GetCachedPurchasedBookList();
			UIBookStoreListBoxPanel* pShowPanel = (UIBookStoreListBoxPanel*)m_tableController.GetShowPanel();
		    if (pShowPanel)
		    {
		        pShowPanel->ReLoadObjectList(bookList);
		    }
		}
		UIUtility::SetScreenTips(StringManager::GetStringById(PERSONAL_MYBOOK_HIDE_FAIL_TIP));
	}
	else
	{
		BookStoreInfoManager* manager = BookStoreInfoManager::GetInstance();
        if(manager != 0 && !bookListArgs.books.empty() && !bookListArgs.books[0].empty())
		{
			manager->RemoveBookFromPurchasedList(bookListArgs.books[0].c_str());
		}
	}
    return false;
}

UISizer* UIBookStorePersonCenteredPage::CreateNavigationRightSizer()
{
    if (NULL == m_pNavigationRightSizer)
    {
        m_pNavigationRightSizer = new UIBoxSizer(dkHORIZONTAL);
        if (m_pNavigationRightSizer)
        {
#ifdef KINDLE_FOR_TOUCH
            m_btnSearch.Initialize(ID_BTN_SEARCH, "", GetWindowFontSize(FontSize22Index));
            m_btnSearch.SetIcon(ImageManager::GetImage(IMAGE_TOUCH_ICON_SEARCH), UIButton::ICON_CENTER);
            m_btnSearch.SetAlign(ALIGN_CENTER);
            m_btnSearch.ShowBorder(false);
            m_btnSearch.SetElemSpacing(0);
            m_btnSearch.SetTopMargin(0);
            m_btnSearch.SetLeftMargin(0);

            m_btnDownload.Initialize(ID_BTN_DOWNLOAD_ALL, StringManager::GetPCSTRById(BATCH_UPDATE), GetWindowFontSize(FontSize22Index));
            m_btnDownload.SetAlign(ALIGN_CENTER);
            m_btnDownload.ShowBorder(false);
            m_btnDownload.SetElemSpacing(0);
            m_btnDownload.SetTopMargin(0);
            m_btnDownload.SetLeftMargin(0);

            m_topRightBtns.AddButton((UIButton*)&m_btnSearch, UISizerFlags().Expand().Center().Border(dkLEFT | dkRIGHT, GetWindowMetrics(UIPixelValue13Index)));
            m_topRightBtns.AddButton((UIButton*)&m_btnDownload, UISizerFlags().Expand().Center().Border(dkLEFT, GetWindowMetrics(UIPixelValue13Index)));
            m_topRightBtns.SetSplitLineHeight(GetWindowMetrics(UIPixelValue23Index));
            m_topRightBtns.SetTopLinePixel(0);
            m_topRightBtns.SetBottomLinePixel(0);
            AppendChild(&m_topRightBtns);

            m_pNavigationRightSizer->AddStretchSpacer();
            m_pNavigationRightSizer->Add(&m_topRightBtns
                , UISizerFlags().Expand().Center().Border(dkRIGHT, GetWindowMetrics(UIHorizonMarginIndex)));
#else
            m_btnSearch.Initialize(ID_BTN_SEARCH, StringManager::GetPCSTRById(SEARCH)
                , 'S', GetWindowFontSize(FontSize22Index), ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
            m_btnDownload.Initialize(ID_BTN_DOWNLOAD_ALL, StringManager::GetPCSTRById(DOWNLOAD_ALL)
                , 'D', GetWindowFontSize(FontSize22Index), ImageManager::GetImage(IMAGE_BUTTON_HEAD_24));
            AppendChild(&m_btnSearch);
            AppendChild(&m_btnDownload);

            m_pNavigationRightSizer->Add(&m_btnDownload, UISizerFlags().Center());
            m_pNavigationRightSizer->Add(&m_btnSearch, UISizerFlags().Center());
#endif
            m_pNavigationRightSizer->SetMinHeight(GetWindowMetrics(UIPixelValue61Index));
        }
    }
    return m_pNavigationRightSizer;
}

void UIBookStorePersonCenteredPage::AdjustTitlePos()
{
    if (m_pTopSizer && m_pNavigationLeftSizer && m_pNavigationRightSizer)
    {
        m_pTopSizer->Clear();
        const int elemSpacing = GetWindowMetrics(UIElementSpacingIndex);
        const int titleWidth = m_title.GetTextWidth();
        const int topLeftWidth = m_pNavigationLeftSizer->GetMinSize().GetWidth();
        const int topRightWidth = m_pNavigationRightSizer->GetMinSize().GetWidth();
        const int maxUsedWidth = dk_max(topLeftWidth, topRightWidth);
        const int remainWidth = CDisplay::GetDisplay()->GetScreenWidth() - ((maxUsedWidth + elemSpacing) << 1) - GetWindowMetrics(UIPixelValue10Index);
        if (titleWidth > remainWidth)
        {
            m_title.SetAlign((topLeftWidth >= topRightWidth) ? ALIGN_LEFT : ALIGN_RIGHT);
            //m_pNavigationRightSizer->SetMinWidth(-1);
            m_pTopSizer->Add(m_pNavigationLeftSizer, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
            m_pTopSizer->Add(&m_title, UISizerFlags(1).Expand().Border(dkLEFT | dkRIGHT, elemSpacing));
            m_pTopSizer->Add(&m_searchBox, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL).Border(dkLEFT | dkRIGHT, elemSpacing));
            m_pTopSizer->Add(m_pNavigationRightSizer, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL));
        }
        else
        {
            m_title.SetAlign(ALIGN_CENTER);
            //m_pNavigationRightSizer->SetMinSize(dkSize(maxUsedWidth, GetWindowMetrics(UIPixelValue61Index)));
            m_pTopSizer->Add(m_pNavigationLeftSizer, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Border(dkRIGHT, maxUsedWidth - topLeftWidth));
            m_pTopSizer->Add(&m_title, UISizerFlags(1).Expand().Border(dkLEFT | dkRIGHT, elemSpacing));
            m_pTopSizer->Add(&m_searchBox, UISizerFlags(1).Align(dkALIGN_CENTER_VERTICAL).Border(dkLEFT | dkRIGHT, elemSpacing));
            m_pTopSizer->Add(m_pNavigationRightSizer, UISizerFlags().Align(dkALIGN_CENTER_VERTICAL).Border(dkLEFT, maxUsedWidth - topRightWidth));
        }
        m_pTopSizer->SetMinHeight(GetWindowMetrics(UIPixelValue61Index));
    }
}

#ifdef KINDLE_FOR_TOUCH
bool UIBookStorePersonCenteredPage::OnHookTouch(MoveEvent* moveEvent)
{
	if(m_searchMode)
	{
		if (moveEvent->GetActionMasked() != MoveEvent::ACTION_DOWN)
	    {
	        return false;
	    }
		if(m_searchBox.PointInWindowAbsolute(moveEvent->GetX(), moveEvent->GetY()))
		{
			return false;
		}
    	UISoftKeyboardIME* pIME = UISoftKeyboardIME::GetInstance();
		if(pIME->IsShow())
		{
			if(pIME->PointInWindowAbsolute(moveEvent->GetX(), moveEvent->GetY()))
			{
				return false;
			}
			SwitchSearchMode();
			return true;
		}
		return false;
	}
    return false;
}
#else
BOOL UIBookStorePersonCenteredPage::OnKeyPress(INT32 iKeyCode)
{
	if(m_searchMode && !m_searchBox.OnKeyPress(iKeyCode) && KEY_BACK != iKeyCode)
	{
		if(!m_searchBox.IsFocus())
		{
			SetChildWindowFocus(GetChildIndex(&m_searchBox), false);
		}
		return false;
	}
	UIBookStoreListBoxPanel* pShowPanel = (UIBookStoreListBoxPanel*)m_tableController.GetShowPanel();
    switch (iKeyCode)
    {
    	case KEY_OKAY:
			{
				if(m_searchMode && m_searchBox.IsFocus())
				{
					SearchPersonalBook();
					return false;
				}
    		}
			break;
		case KEY_BACK:
			{
				if(m_searchMode)
				{
					SwitchSearchMode();
					return false;
				}
			}
			break;
        case KEY_UP:
            if (m_tableController.IsFocus() && !m_pCurPanel->IsFocus())
            {
            	m_searchMode ? m_searchBox.SetFocus(true) : m_btnSearch.SetFocus(true);
                return FALSE;
            }
            break;
        case KEY_DOWN:
            if (m_tableController.IsFocus() && pShowPanel->IsFocus() && (pShowPanel->GetSelectedItemIndex() == pShowPanel->GetNumberPerPage() - 1))
            {
                m_searchMode ? m_searchBox.SetFocus(true) : m_btnSearch.SetFocus(true);
                return FALSE;
            }
            break;
        default:
            break;
    }
    return UIBookStoreTablePage::OnKeyPress(iKeyCode);
}

#endif

void UIBookStorePersonCenteredPage::SwitchSearchMode(bool reloadInfo)
{
	m_searchMode = !m_searchMode;
	SetSearchMode(m_searchMode, !m_searchMode);
	if(!m_searchMode && reloadInfo)
	{
		BookStoreInfoManager* manager = BookStoreInfoManager::GetInstance();
		if(manager)
		{
			model::BookInfoList bookList = manager->GetCachedPurchasedBookList();
			UIBookStoreListBoxPanel* pShowPanel = (UIBookStoreListBoxPanel*)m_tableController.GetShowPanel();
		    if (pShowPanel)
		    {
		        pShowPanel->ReLoadObjectList(bookList);
		    }
		}
	}
}

void UIBookStorePersonCenteredPage::SearchPersonalBook()
{
	std::string keyword = m_searchBox.GetTextUTF8();
	BookStoreInfoManager* manager = BookStoreInfoManager::GetInstance();
	if(manager)
	{
		model::BookInfoList bookList = manager->GetCachedPurchasedBookList();
		BookInfoKeywordMatcher matcher(keyword.c_str());
		model::BookInfoList tmpFiles;
    	copy_if(bookList.begin(), bookList.end(), std::back_inserter(tmpFiles), matcher);
		UIBookStoreListBoxPanel* pShowPanel = (UIBookStoreListBoxPanel*)m_tableController.GetShowPanel();
	    if (pShowPanel)
	    {
	        pShowPanel->ReLoadObjectList(tmpFiles);
	    }
		SetSearchMode(m_searchMode, true);
		UIIME* ime = UIIMEManager::GetCurrentIME();
		if(ime)
		{
			ime->HideIME();
		}
	}
}

void UIBookStorePersonCenteredPage::SetSearchMode(bool isSearchMode, bool showBack)
{
	m_searchBox.SetVisible(isSearchMode);
    if (m_pNavigationRightSizer)
    {
        m_pNavigationRightSizer->Show(!isSearchMode);
    }
    if (m_pNavigationLeftSizer)
    {
        m_pNavigationLeftSizer->Show(!isSearchMode);
    }
	m_title.SetVisible(!isSearchMode);
	if(isSearchMode)
	{
		SetChildWindowFocus(GetChildIndex(&m_searchBox), false);
	    m_searchBox.SetTextBoxStyle(ENGLISH_NORMAL);
	    UIIME* pIME = UIIMEManager::GetIME(m_searchBox.GetInputMode(), &m_searchBox);
	    if (pIME)
	    {
	        pIME->SetShowDelay(true);
	    }
	}
	else
	{
		m_searchBox.ClearText();
	}
	Layout();
	UpdateWindow();
}
