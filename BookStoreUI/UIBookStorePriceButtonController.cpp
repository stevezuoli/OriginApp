#include <tr1/functional>
#include "BookStoreUI/UIBookStorePriceButtonController.h"
#include "BookStoreUI/UIBookStorePayDlg.h"
#include "BookStoreUI/UIBookStorePayInfoDlg.h"
#include "BookStore/BookStoreInfoManager.h"
#include "BookStore/BookStoreOrderManager.h"
#include "BookStore/BookDownloadController.h"
#include "I18n/StringManager.h"
#include "Common/CAccountManager.h"
#include "Framework/CDisplay.h"
#ifdef KINDLE_FOR_TOUCH
#include "TouchAppUI/BookOpenManager.h"
#include "GUI/GUISystem.h"
#include "GUI/UITouchComplexButton.h"
#else
#include "AppUI/BookOpenManager.h"
#endif
#include "CommonUI/UIUtility.h"
#include "GUI/UIMessageBox.h"
#include "../Common/FileManager_DK.h"
#include "Utility/PathManager.h"
#include "Utility/SystemUtil.h"
#include "Framework/CNativeThread.h"
#include "Framework/CDisplay.h"
#include "Model/UpgradeModelImpl.h"
#include "KernelVersion.h"

using namespace dk::account;
using namespace dk::bookstore;
using namespace dk::utility;
using dk::bookstore::model::BookInfo;
using dk::utility::PathManager;

const char* UIBookStorePriceButtonListener::EventPriceButtonUpdate = "EventPriceButtonUpdate";

UIBookStorePriceButtonListener*  UIBookStorePriceButtonListener::GetInstance()
{
    static UIBookStorePriceButtonListener manager;
    return &manager;
}

void  UIBookStorePriceButtonListener::FireBookStatusUpdateEvent(BUTTONUPDATETYPE status, const std::string& bookId, int errorCode)
{
    PriceButtonEventArgs args;
    args.bookId = bookId;
    args.updateStatus = status;
    args.resultCode = errorCode;
    FireEvent(EventPriceButtonUpdate, args);
}

UIBookStorePriceButtonController::UIBookStorePriceButtonController(
            UIContainer* root,
            UIDiscountTextSingleLine* originPriceLine,
            UIWindow* actionWnd,
            UIWindow* trialWnd,
            UIWindow* addFavouriteWnd,
            UIWindow* addCartWnd,
            const std::string& duokanKey)
        : m_root(root)
        , m_originPriceLine(originPriceLine)
        , m_actionWnd(actionWnd)
        , m_trialWnd(trialWnd)
        , m_addFavouriteWindow(addFavouriteWnd)
        , m_addCartWindow(addCartWnd)
        , m_currentAction(NONE)
        , m_favouriteAction(FAVOURITE_NONE)
        , m_cartAction(CART_NONE)
        , m_duokanKey(duokanKey)
        , m_downloadingProgress(-1)
        , m_trialDownloadingProgress(-1)
{
#ifdef KINDLE_FOR_TOUCH
    SubscribeEvent(UIButton::EventClick,
        *m_actionWnd,
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStorePriceButtonController::OnActionButtonClick),
        this,
        std::tr1::placeholders::_1));
    if (m_trialWnd != 0)
    {
        SubscribeEvent(UIButton::EventClick,
            *m_trialWnd,
            std::tr1::bind(
            std::tr1::mem_fn(&UIBookStorePriceButtonController::OnTrialButtonClick),
            this,
            std::tr1::placeholders::_1));
    }
    if (m_addFavouriteWindow != 0)
    {
        SubscribeEvent(UIButton::EventClick,
            *m_addFavouriteWindow,
            std::tr1::bind(
            std::tr1::mem_fn(&UIBookStorePriceButtonController::OnAddFavouriteButtonClick),
            this,
            std::tr1::placeholders::_1));
    }
    if (m_addCartWindow != 0)
    {
        SubscribeEvent(UIButton::EventClick,
            *m_addCartWindow,
            std::tr1::bind(
            std::tr1::mem_fn(&UIBookStorePriceButtonController::OnAddCartButtonClick),
            this,
            std::tr1::placeholders::_1));
    }
#else 
    SubscribeEvent(UIButton::EventButtonClick,
        *m_actionWnd,
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStorePriceButtonController::OnActionButtonClick),
        this,
        std::tr1::placeholders::_1));
    if (m_trialWnd != 0)
    {
        SubscribeEvent(UIButton::EventButtonClick,
            *m_trialWnd,
            std::tr1::bind(
            std::tr1::mem_fn(&UIBookStorePriceButtonController::OnTrialButtonClick),
            this,
            std::tr1::placeholders::_1));
    }
    if (m_addFavouriteWindow != 0)
    {
        SubscribeEvent(UIButton::EventButtonClick,
            *m_addFavouriteWindow,
            std::tr1::bind(
            std::tr1::mem_fn(&UIBookStorePriceButtonController::OnAddFavouriteButtonClick),
            this,
            std::tr1::placeholders::_1));
    }
    if (m_addCartWindow != 0)
    {
        SubscribeEvent(UIButton::EventButtonClick,
            *m_addCartWindow,
            std::tr1::bind(
            std::tr1::mem_fn(&UIBookStorePriceButtonController::OnAddCartButtonClick),
            this,
            std::tr1::placeholders::_1));
    }
#endif
    SubscribeMessageEvent(
        BookStoreInfoManager::EventCreateOrderUpdate,
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
            std::tr1::mem_fn(&UIBookStorePriceButtonController::OnCreateOrderUpdate),
            this,
            std::tr1::placeholders::_1));

    SubscribeMessageEvent(
        BookStoreInfoManager::EventBookInfoUpdate,
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
            std::tr1::mem_fn(&UIBookStorePriceButtonController::OnBookInfoUpdate),
            this,
            std::tr1::placeholders::_1));

    SubscribeMessageEvent(
        BookStoreInfoManager::EventPurchasedListUpdate,
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
            std::tr1::mem_fn(&UIBookStorePriceButtonController::OnPurchasedListUpdate),
            this,
            std::tr1::placeholders::_1));

    SubscribeMessageEvent(
        BookDownloadController::EventDownloadBookUpdated,
        *BookDownloadController::GetInstance(),
        std::tr1::bind(
                std::tr1::mem_fn(&UIBookStorePriceButtonController::OnDownloadUpdate),
                this,
                std::tr1::placeholders::_1));

    SubscribeMessageEvent(
        BookDownloadController::EventDRMUpdated,
        *BookDownloadController::GetInstance(),
        std::tr1::bind(
            std::tr1::mem_fn(&UIBookStorePriceButtonController::OnDrmUpdate),
            this,
            std::tr1::placeholders::_1));

    SubscribeMessageEvent(BookStoreInfoManager::EventExchangeDuokanKey, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStorePriceButtonController::OnExchangeDuokanKeyUpdate),
        this,
        std::tr1::placeholders::_1)
        );

    SubscribeMessageEvent(BookStoreOrderManager::EventAddFavouriteUpdate, 
        *BookStoreOrderManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStorePriceButtonController::OnAddFavouriteUpdate),
        this,
        std::tr1::placeholders::_1)
        );

    SubscribeMessageEvent(BookStoreOrderManager::EventRemoveFavouriteUpdate, 
        *BookStoreOrderManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStorePriceButtonController::OnRemoveFavouriteUpdate),
        this,
        std::tr1::placeholders::_1)
        );

    SubscribeMessageEvent(BookStoreOrderManager::EventAddToCartUpdate, 
        *BookStoreOrderManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStorePriceButtonController::OnAddCartUpdate),
        this,
        std::tr1::placeholders::_1)
        );

    SubscribeMessageEvent(BookStoreOrderManager::EventRemoveFromCartUpdate, 
        *BookStoreOrderManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBookStorePriceButtonController::OnRemoveCartUpdate),
        this,
        std::tr1::placeholders::_1)
        );
}


void UIBookStorePriceButtonController::SetBookInfo(BookInfoSPtr bookInfo)
{
    if (0 == bookInfo)
    {
        return;
    }

    // 已经设置了多次设置会忽略，因为这里可能用户在界面上开始下载，已经获取了书籍url
    if (m_bookInfo)
    {
        if (m_bookInfo->GetBookId() != bookInfo->GetBookId())
        {
            m_currentAction = NONE;
            m_downloadingProgress = -1;
        }
        else
        {
            return;
        }
    }
    m_bookInfo.reset(dynamic_cast<model::BookInfo*>(bookInfo->Clone()));
    if (0 == m_bookInfo)
    {
        return;
    }
    FireBookStatusUpdateEvent(AllStatusUpdate);
}

bool UIBookStorePriceButtonController::OnActionButtonClick(const EventArgs&)
{
    if (0 == m_bookInfo)
    {
        return false;
    }
    //if (m_currentAction != NONE)
    //{
    //    return false;
    //}

    int bookStatus = GetBookStatus();
    BookStoreInfoManager* bookStoreInfoManager = BookStoreInfoManager::GetInstance();
    
    if (!m_duokanKey.empty())
    {
        if (UIUtility::CheckAndReLoginSync())
        {
            if (m_bookInfo->GetRevision().empty() || m_bookInfo->GetFileUrl().empty())
            {
                bookStoreInfoManager->FetchBookInfo(m_bookInfo->GetBookId().c_str());
                m_currentAction = FETCH_BOOKINFO;
            }
            else
            {
                DoExchangeDuokanKey();
            }
        }
        return true;
    }

#ifdef KINDLE_FOR_TOUCH
    if (m_actionWnd)
    {
        CDisplay::CacheDisabler cache;
        m_actionWnd->SetFocus(true);
        m_actionWnd->SetFocus(false);
    }
#endif
	if(!CheckBookKernelVersion(m_bookInfo->GetLowestKernelVersion()))
	{
		return false;
	}
    
    if (BS_UNPURCHASED & bookStatus)
    {
        if (UIUtility::CheckAndReLoginSync())
        {
            if (m_bookInfo->GetRevision().empty() || m_bookInfo->GetFileUrl().empty())
            {
                bookStoreInfoManager->FetchBookInfo(m_bookInfo->GetBookId().c_str());
                m_currentAction = FETCH_BOOKINFO;
            }
            else
            {
                if (DoCreateOrder())
                {
                    m_currentAction = CREATE_ORDER;
                }
                else
                {
                    m_currentAction = NONE;
                    FireBookStatusUpdateEvent(BookStatusUpdate);
                }
            }
        }
    }
    else if((BS_UNDOWNLOAD & bookStatus) || (BS_NEEDSUPDATE & bookStatus))
    {
        model::BookInfoSPtr purchasedBookInfo = 
            bookStoreInfoManager->GetPurchasedBookInfo(m_bookInfo->GetBookId().c_str());
        if (!purchasedBookInfo)
        {
            UIMessageBox msgBox(GetDlgParent(), 
                    StringManager::GetPCSTRById(UNKNOWN_ERROR),
                    MB_OK);
            msgBox.DoModal();
            return false;
        }

        m_bookInfo.reset(dynamic_cast<model::BookInfo*>(purchasedBookInfo->Clone()));
        if (m_bookInfo->GetOrderId().empty() ||
            m_bookInfo->GetRevision().empty() ||
            m_bookInfo->GetFileUrl().empty())
        {
            UIMessageBox msgBox(GetDlgParent(), 
                    StringManager::GetPCSTRById(UNKNOWN_ERROR),
                    MB_OK);
            msgBox.DoModal();
            return false;
        }
        BookDownloadController::GetInstance()->ExecuteDownload(m_bookInfo);
        FireBookStatusUpdateEvent(BookStatusUpdate);
    }
    else if(BS_DOWNLOADED & bookStatus)
    {
        CDKFileManager* pFileManager = CDKFileManager::GetFileManager();
        if(!pFileManager)
        {
            return false;
        }

        string filePath = PathManager::BookIdToEpubFile(m_bookInfo->GetBookId().c_str());
        int iFileId = pFileManager->GetFileIdByPath(filePath.c_str());
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
        BookOpenManager bookOpen(GetRepaintRoot(), iFileId, 10, 10);
        if(!bookOpen.OpenBook())
        {
            UIMessageBox messagebox(GetDlgParent(), StringManager::GetStringById(CANNOT_OPEN_BOOK), MB_OK);
            messagebox.DoModal();
        }
#endif
    }
    return true;
}

int UIBookStorePriceButtonController::GetBookStatus()
{
    int ret = BookDownloadController::GetInstance()->GetBookDownloadStatus(m_bookInfo);
    BookStoreInfoManager* bookStoreInfoManager = BookStoreInfoManager::GetInstance();
    //ret = ret | bookStoreInfoManager->GetBookStatus(m_bookInfo->GetBookId().c_str());
    ret = ret | bookStoreInfoManager->GetBookStatus(m_bookInfo);
    return ret;
}

int UIBookStorePriceButtonController::GetTrialBookStatus()
{
    int ret = BookDownloadController::GetInstance()->GetBookDownloadStatus(m_bookInfo);
    BookStoreInfoManager* bookStoreInfoManager = BookStoreInfoManager::GetInstance();
    ret = ret | bookStoreInfoManager->GetTrialBookStatus(m_bookInfo->GetBookId().c_str());
    return ret;
}

void UIBookStorePriceButtonController::UpdateBookStatus()
{
    if (m_currentAction == CREATE_ORDER || m_currentAction == FETCH_BOOKINFO || m_bookInfo == 0)
    {
        return;
    }
    
    if (m_currentAction == FETCH_DRM || m_currentAction == WAITING_QUEUE)
    {
        m_actionWnd->SetText(StringManager::GetPCSTRById(BOOKSTORE_WAITING));
        m_actionWnd->SetEnable(FALSE);
        return;
    }

    if (!m_duokanKey.empty())
    {
        m_actionWnd->SetText(StringManager::GetPCSTRById(BOOKSTORE_BOOK_DUOKANKEY_EXCHANGE));
        m_actionWnd->SetEnable(true);
        if (m_trialWnd)
        {
            m_trialWnd->SetEnable(false);
            m_trialWnd->SetVisible(false);
        }
    }
    else
    {
        int bookStatus = GetBookStatus();
        if (bookStatus & BS_WAITING)
        {
            // this book is in waiting list now
            m_actionWnd->SetText(StringManager::GetPCSTRById(BOOKSTORE_WAITING));
            m_actionWnd->SetEnable(FALSE);
            m_currentAction = WAITING_QUEUE;
        }
        else
        {
            if (BS_UNPURCHASED & bookStatus)
            {
                int discountPrice = m_bookInfo->GetDiscountPrice();
                int price = m_bookInfo->GetPrice();
                char szTmp[64] = {0};
                if(price > 0 && discountPrice >= 0)
                {
                    if (m_originPriceLine)
                    {
                        snprintf(szTmp, DK_DIM(szTmp), "%s%.2f",StringManager::GetPCSTRById(BOOKSTORE_RMB),((float)price) / 100.00);
                        m_originPriceLine->SetText(szTmp);
                        m_originPriceLine->SetVisible(true);
                    }
                    snprintf(szTmp, DK_DIM(szTmp), "%s%.2f",StringManager::GetPCSTRById(BOOKSTORE_RMB),((float)discountPrice) / 100.00);
                    m_actionWnd->SetText(szTmp);
                }
                else if (price > 0)
                {
                    //
                    if (m_originPriceLine)
                    {
                        m_originPriceLine->SetVisible(false);
                    }
                    snprintf(szTmp, DK_DIM(szTmp), "%s%.2f",StringManager::GetPCSTRById(BOOKSTORE_RMB),((float)price) / 100.00);
                    m_actionWnd->SetText(szTmp);
                }
                else if(price == 0)
                {
                    if (m_originPriceLine)
                    {
                        m_originPriceLine->SetVisible(false);
                    }
                    m_actionWnd->SetText(StringManager::GetPCSTRById(BOOKSTORE_FREE));
                }
                m_actionWnd->SetEnable(true);
            }
            else if (BS_DOWNLOADING & bookStatus)
            {
                if (m_originPriceLine)
                {
                    m_originPriceLine->SetVisible(false);
                }
                char buf[100];
                snprintf(buf, DK_DIM(buf), "%d%%", m_downloadingProgress == -1 ? 0 : m_downloadingProgress);
                m_actionWnd->SetText(buf);
                m_actionWnd->SetEnable(false);
            }
            else if ((BS_DOWNLOADED & bookStatus) || ((BS_DOWNLOADING & bookStatus) && (100 == m_downloadingProgress)))
            {
                if (m_originPriceLine)
                {
                    m_originPriceLine->SetVisible(false);
                }
#ifdef KINDLE_FOR_TOUCH
                m_actionWnd->SetText(StringManager::GetPCSTRById(BOOK_READING));
#else
                if (m_trialWnd)
                {
                    m_actionWnd->SetText(StringManager::GetPCSTRById(BOOK_READING));
                }
                else
                {
                    m_actionWnd->SetText(StringManager::GetPCSTRById(BOOKSTORE_DOWNLOADED));
                }
#endif
                m_actionWnd->SetEnable(true);
            }
            else if (BS_UNDOWNLOAD & bookStatus)
            {
                if (m_originPriceLine)
                {
                    m_originPriceLine->SetVisible(false);
                }
#ifdef KINDLE_FOR_TOUCH
                m_actionWnd->SetText(StringManager::GetPCSTRById(BOOKSTORE_DOWNLOAD));
#else
                if (m_trialWnd)
                {
                    m_actionWnd->SetText(StringManager::GetPCSTRById(BOOKSTORE_DOWNLOAD));
                }
                else
                {
                    m_actionWnd->SetText(StringManager::GetPCSTRById(BOOKSTORE_PURCHASED));
                }
#endif
                m_actionWnd->SetEnable(true);
            }
            else if (BS_NEEDSUPDATE & bookStatus)
            {
                if (m_originPriceLine)
                {
                    m_originPriceLine->SetVisible(false);
                }
                m_actionWnd->SetText(StringManager::GetPCSTRById(BOOKSTORE_UPDATE));
                m_actionWnd->SetEnable(true);
            }
        }
    }
}

void UIBookStorePriceButtonController::FireBookStatusUpdateEvent(BUTTONUPDATETYPE status, int errorCode)
{
    UIBookStorePriceButtonListener* pListener = UIBookStorePriceButtonListener::GetInstance();
    if (m_bookInfo && pListener)
    {
        pListener->FireBookStatusUpdateEvent(status, m_bookInfo->GetBookId(), errorCode);
    }
}

bool UIBookStorePriceButtonController::DoCreateOrder()
{
    BookStoreInfoManager* bookStoreInfoManager = BookStoreInfoManager::GetInstance();
    model::BookInfo* bookInfo = (BookInfo*)m_bookInfo.get();
    FetchDataResult fdr = bookStoreInfoManager->CreateOrder(bookInfo->GetBookId().c_str()); 
    if (fdr == FDR_FAILED)
    {
        UIMessageBox messagebox(
                GetDlgParent(),
                StringManager::GetStringById(
                    BOOKSTORE_CREATEORDER_FAILED),
                MB_OK);
        messagebox.DoModal();
        return false;
    }
    else
    {
        return true;
    }
}

bool UIBookStorePriceButtonController::OnCreateOrderUpdate(const EventArgs& args)
{
    if (!m_bookInfo)
    {
        return false;
    } 
    if (CREATE_ORDER != m_currentAction && VERIFY_PAY != m_currentAction)
    {
        return false;
    }
    BookStoreInfoManager* bookStoreInfoManager = BookStoreInfoManager::GetInstance();

    const CreateOrderUpdateArgs& createOrderUpdateArgs =
        (const CreateOrderUpdateArgs&)args;
    model::CreateOrderResultSPtr createOrderResult = createOrderUpdateArgs.result;
    if (!createOrderResult || (createOrderResult->GetBookId() != m_bookInfo->GetBookId()))
    {
        return false;
    }

    if (!createOrderResult->IsSuccessful())
    {
        if (m_currentAction == CREATE_ORDER)
        {
            if (IsLoginError(createOrderResult->GetResultCode()))
            {
                UIMessageBox messageBox(
                        GetDlgParent(),
                        StringManager::GetStringById(LOGIN_EXPIRED),
                        MB_OK);
                messageBox.DoModal();
                CAccountManager* pManager = CAccountManager::GetInstance();
                pManager->LogoutAsync();
                UIUtility::CheckAndReLogin();
                if (pManager->IsLoggedIn())
                {
                    if (DoCreateOrder())
                    {
                        m_currentAction = CREATE_ORDER;
                    }
                    else
                    {
                        m_currentAction = NONE;
                    }
                }
                else
                {
                    m_currentAction = NONE;
                }

                FireBookStatusUpdateEvent(BookStatusUpdate);
            }
            else
            {
                UIMessageBox messagebox(
                        GetDlgParent(),
                        StringManager::GetStringById(BOOKSTORE_CREATEORDER_FAILED),
                        MB_OK);
                messagebox.DoModal();
            }
            
        }
        m_currentAction = NONE;
        return false;
    }
    else
    {
        m_bookInfo->MutableOrderId() = createOrderResult->GetOrderId();
        // 已支付，判断是否需要获取drm信息
        if (createOrderResult->GetStatus() == model::OS_PAID)
        {
            bookStoreInfoManager->FetchPurchasedList();
            BookDownloadController::GetInstance()->ExecuteDownload(m_bookInfo);
            FireBookStatusUpdateEvent(BookStatusUpdate);
        }
        else if ((m_currentAction == CREATE_ORDER) && m_actionWnd->IsDisplay())//仍在当前页面才提示相关信息
        {
            // 未支付，提示用户支付渠道
            CAccountManager* pManager = CAccountManager::GetInstance();
            UIBookStorePayDlg bookStorePayDlg(GetDlgParent(), pManager && (pManager->GetLoginStatus() == CAccountManager::LoggedIn_DuoKan));
            bookStorePayDlg.SetOrderId(createOrderResult->GetOrderId().c_str());
            //CDisplay* pDisplay = CDisplay::GetDisplay();
			//const int payDlgWidth = bookStorePayDlg.GetWidth();
			//const int patDlgHeight = bookStorePayDlg.GetHeight();
            //const int screenWidth = pDisplay->GetScreenWidth();
            //const int screenHeight = pDisplay->GetScreenHeight();
			//bookStorePayDlg.MoveWindow((screenWidth - payDlgWidth)/2, (screenHeight - patDlgHeight)/2, payDlgWidth, patDlgHeight);
            if (bookStorePayDlg.DoModal() == IDOK)
            {
                switch (bookStorePayDlg.GetAction())
                {
                    case UIBookStorePayDlg::SEND_MAIL:
                        {
                            UIBookStorePayInfoDlg payInfoDlg(GetDlgParent(), 
							        StringManager::GetPCSTRById(BOOKSTORE_TITLE_ORDER_MAIL_SENT),
                                    UIBookStorePayInfoDlg::PM_EMAIL,
                                    BookStoreInfoManager::GetInstance()->GetCurrentUserName().c_str(),
                                    m_bookInfo->GetOrderId().c_str());
							//const int payInfoDlgWidth = payInfoDlg.GetWidth();
							//const int patInfoDlgHeight = payInfoDlg.GetHeight();
							//payInfoDlg.MoveWindow((screenWidth - payInfoDlgWidth)/2, (screenHeight - patInfoDlgHeight)/2, payInfoDlgWidth, patInfoDlgHeight);
                            if (payInfoDlg.DoModal() == IDOK && payInfoDlg.IsMailSended())
                            {
                                DoCreateOrder();
                                m_currentAction = VERIFY_PAY;
                            }
                            else
                            {
                                m_currentAction = NONE;
                            }
                        }
                        break;
                    case UIBookStorePayDlg::BUY_FROM_OTHER_DEVICE:
                        {
                            UIBookStorePayInfoDlg dlg(GetDlgParent(),
                               StringManager::GetPCSTRById(BOOKSTORE_TITLE_BUY_FROM_OTHER_DEVICE), 
                               UIBookStorePayInfoDlg::PM_OTHER_DEVICE,
                               NULL,
                               m_bookInfo->GetOrderId().c_str());
                            //const int payInfoDlgWidth = dlg.GetWidth();
                            //const int patInfoDlgHeight = dlg.GetHeight();
                            //dlg.MoveWindow((screenWidth - payInfoDlgWidth)/2, (screenHeight - patInfoDlgHeight)/2, payInfoDlgWidth, patInfoDlgHeight);
                            dlg.DoModal();
                            if (DoCreateOrder())
                            {
                                m_currentAction = VERIFY_PAY;
                            }
                            else
                            {
                                m_currentAction = NONE;
                            }
                        }
                        break;
                    case UIBookStorePayDlg::BUY_FROM_WEBSITE:
                        {
                            UIBookStorePayInfoDlg dlg(GetDlgParent(),
                               StringManager::GetPCSTRById(BOOKSTORE_TITLE_BUY_FROM_WEBSITE), 
                               UIBookStorePayInfoDlg::PM_WEBSITE,
                               NULL,
                               m_bookInfo->GetOrderId().c_str());
                            //const int payInfoDlgWidth = dlg.GetWidth();
                            //const int patInfoDlgHeight = dlg.GetHeight();
                            //dlg.MoveWindow((screenWidth - payInfoDlgWidth)/2, (screenHeight - patInfoDlgHeight)/2, payInfoDlgWidth, patInfoDlgHeight);
                            dlg.DoModal();
                            if (DoCreateOrder())
                            {
                                m_currentAction = VERIFY_PAY;
                            }
                            else
                            {
                                m_currentAction = NONE;
                            }
                        }
                        break;
                    default:
                        m_currentAction = NONE;
                        break;
                }
            }
            else
            {
                m_currentAction = NONE;
                FireBookStatusUpdateEvent(BookStatusUpdate);
            }
        }
        else
        {
            m_currentAction = NONE;
        }
        return true;
    }
    return false;
}

bool UIBookStorePriceButtonController::OnDrmUpdate(const EventArgs& args)
{
    const BookDownloadDRMUpdateArgs& drm_update_args = (const BookDownloadDRMUpdateArgs&)args;
    if (!m_bookInfo || drm_update_args.bookId != m_bookInfo->GetBookId())
    {
        return false;
    }

    // do not care about current action
    //if (m_currentAction != FETCH_DRM)
    //{
    //    return false;
    //}

    BookStoreInfoManager* bookStoreInfoManager = BookStoreInfoManager::GetInstance();
    if (drm_update_args.succeeded && !drm_update_args.drm_handled)
    {
        DebugPrintf(DLC_DIAGNOSTIC, "UIBookStorePriceButtonController::OnBookInfoUpdate, this: %08X", this);
        if (m_bookInfo->GetFileUrl().empty())
        {
            BookInfoSPtr purchasedBookInfo = bookStoreInfoManager->GetPurchasedBookInfo(
                    m_bookInfo->GetBookId().c_str());
            if (purchasedBookInfo && !purchasedBookInfo->GetFileUrl().empty())
            {
                bookStoreInfoManager->FetchBook(purchasedBookInfo);
            }
            else
            {
                return false;
            }
        }
        else
        {
            bookStoreInfoManager->FetchBook(m_bookInfo);
        }
        m_currentAction = DOWNLOADING;
    }
    else
    {
        m_currentAction = NONE;
    }
    FireBookStatusUpdateEvent(BookStatusUpdate);
    return true;
}


bool UIBookStorePriceButtonController::OnBookInfoUpdate(const EventArgs& args)
{
    if (m_currentAction != FETCH_BOOKINFO)
    {
        return false;
    }
    if (!m_bookInfo)
    {
        return false;
    }
    model::BookInfo* bookInfo = m_bookInfo.get();
    const BookInfoUpdateArgs& bookInfoUpdateArgs = 
        (const BookInfoUpdateArgs&)args;
    if (bookInfoUpdateArgs.succeeded 
            && bookInfoUpdateArgs.bookInfo->GetBookId() == bookInfo->GetBookId())
    {
        DebugPrintf(DLC_BOOKSTORE, "UIBookStorePriceButtonController::OnBookInfoUpdate, this: %08X", this);
        m_bookInfo = bookInfoUpdateArgs.bookInfo;
        if (m_bookInfo->GetFileUrl().empty())
        {
            return false;
        }

        bookInfo = (model::BookInfo*)m_bookInfo.get();
        if (DoCreateOrder())
        {
            m_currentAction = CREATE_ORDER;
        }
        else
        {
            m_currentAction = NONE;
            FireBookStatusUpdateEvent(BookStatusUpdate);
        }
    }
    return true;
}

bool UIBookStorePriceButtonController::OnPurchasedListUpdate(const EventArgs& args)
{
    // Remove book from cart
    // Free books won't be removed from cart. However this kind of books are not allowed to add into cart now
    // Therefore it's no necessary to remove it now.
    //RemoveFromCartOncePurchased();
    FireBookStatusUpdateEvent(AllStatusUpdate);
    return true;
}

bool UIBookStorePriceButtonController::OnAddFavouriteUpdate(const EventArgs& args)
{
    if (m_bookInfo == 0)
    {
        return false;
    }
    model::BookInfo* bookInfo = m_bookInfo.get();
    const BookStoreOrderArgs& addFavouriteArgs = (const BookStoreOrderArgs&)args;
    if (addFavouriteArgs.Contains(bookInfo->GetBookId()) && m_favouriteAction == ADDING_FAVOURITE)
    {
        //if (addFavouriteArgs.succeeded)
        //{
        //    DebugPrintf(DLC_DIAGNOSTIC, "UIBookStorePriceButtonController::OnAddFavouriteUpdate, this: %08X", this);
        //}
        m_favouriteAction = FAVOURITE_NONE;
        FireBookStatusUpdateEvent(FavouriteBookStatusUpdate, addFavouriteArgs.resultCode);
    }
    return true;
}

bool UIBookStorePriceButtonController::OnRemoveFavouriteUpdate(const EventArgs& args)
{
    if (m_bookInfo == 0)
    {
        return false;
    }
    model::BookInfo* bookInfo = m_bookInfo.get();
    const BookStoreOrderArgs& removeFavouriteArgs = (const BookStoreOrderArgs&)args;
    if (removeFavouriteArgs.Contains(bookInfo->GetBookId()) && m_favouriteAction == REMOVING_FAVOURITE)
    {
        //if (removeFavouriteArgs.succeeded)
        //{
        //    DebugPrintf(DLC_DIAGNOSTIC, "UIBookStorePriceButtonController::OnRemoveFavouriteUpdate, this: %08X", this);
        //}
        m_favouriteAction = FAVOURITE_NONE;
        FireBookStatusUpdateEvent(FavouriteBookStatusUpdate, removeFavouriteArgs.resultCode);
    }
    return true;
}

bool UIBookStorePriceButtonController::RemoveFromCartOncePurchased()
{
    int status = GetBookStatus();
    BookStoreOrderManager* bookStoreOrderManager = BookStoreOrderManager::GetInstance();
    bool isBookAddedToCart = bookStoreOrderManager->IsBookAddedToCart(m_bookInfo->GetBookId().c_str());
    if (!(status & BS_UNPURCHASED) && isBookAddedToCart)
    {
        model::BookInfoList cartBooks;
        cartBooks.push_back(m_bookInfo);
        return bookStoreOrderManager->RemoveBooksFromCart(cartBooks) != FDR_FAILED;
    }
    return true;
}

bool UIBookStorePriceButtonController::OnAddCartUpdate(const EventArgs& args)
{
    if (m_bookInfo == 0)
    {
        return false;
    }
    model::BookInfo* bookInfo = m_bookInfo.get();
    const BookStoreOrderArgs& addToCartArgs = (const BookStoreOrderArgs&)args;
    if (addToCartArgs.Contains(bookInfo->GetBookId()) && m_cartAction == ADDING_CART)
    {
        //if (addToCartArgs.succeeded)
        //{
        //    DebugPrintf(DLC_DIAGNOSTIC, "UIBookStorePriceButtonController::OnAddToCartUpdate, this: %08X", this);
        //}
        m_cartAction = CART_NONE;
        FireBookStatusUpdateEvent(BookInCartUpdate, addToCartArgs.resultCode);
    }
    return true;
}

bool UIBookStorePriceButtonController::OnRemoveCartUpdate(const EventArgs& args)
{
    if (m_bookInfo == 0)
    {
        return false;
    }
    model::BookInfo* bookInfo = m_bookInfo.get();
    const BookStoreOrderArgs& removeFromCartArgs = (const BookStoreOrderArgs&)args;
    if (removeFromCartArgs.Contains(bookInfo->GetBookId()) && m_cartAction == REMOVING_CART)
    {
        //if (removeFromCartArgs.succeeded)
        //{
        //    DebugPrintf(DLC_DIAGNOSTIC, "UIBookStorePriceButtonController::OnRemoveCartUpdate, this: %08X", this);
        //}
        m_cartAction = CART_NONE;
        FireBookStatusUpdateEvent(BookInCartUpdate, removeFromCartArgs.resultCode);
    }
    return true;
}

bool UIBookStorePriceButtonController::OnTrialButtonClick(const EventArgs&)
{
    if (!m_bookInfo)
    {
        return false;
    }
    if (m_currentAction != NONE)
    {
        return false;
    }
    if (!m_trialWnd->IsVisible())
    {
        return false;
    }
#ifdef KINDLE_FOR_TOUCH
    if (m_trialWnd)
    {
        CDisplay::CacheDisabler cache;
        m_trialWnd->SetFocus(true);
        m_trialWnd->SetFocus(false);
    }
#endif

	if(!CheckBookKernelVersion(m_bookInfo->GetLowestKernelVersion()))
	{
		return false;
	}
	
    BookStoreInfoManager* bookStoreInfoManager = BookStoreInfoManager::GetInstance();
    int trialBookStatus = GetTrialBookStatus();
    if (trialBookStatus == TBS_DOWNLOADED)
    {
            string filePath = PathManager::BookIdToEpubFile(m_bookInfo->GetBookId().c_str());
            CDKFileManager* pFileManager = CDKFileManager::GetFileManager();
            int iFileId = pFileManager->GetFileIdByPath(filePath.c_str());
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
            BookOpenManager bookOpen(GetRepaintRoot(), iFileId, 10, 10);
            if(!bookOpen.OpenBook())
            {
                UIMessageBox messagebox(GetDlgParent(), StringManager::GetStringById(CANNOT_OPEN_BOOK), MB_OK);
                messagebox.DoModal();
            }
#endif

    }
    if (trialBookStatus & (TBS_DOWNLOADED | TBS_DOWNLOADING | TBS_PURCHASED | TBS_WAITING))
    {
        return false;
    }
    bookStoreInfoManager->WriteTrialInfoFile(m_bookInfo);
    bookStoreInfoManager->FetchTrialBook(m_bookInfo);
    FireBookStatusUpdateEvent(TrialBookStatusUpdate);
    return true;
}

bool UIBookStorePriceButtonController::OnAddFavouriteButtonClick(const EventArgs&)
{
    if (!m_bookInfo)
    {
        return false;
    }
    if (m_currentAction != NONE)
    {
        return false;
    }
    if (!m_addFavouriteWindow->IsVisible())
    {
        return false;
    }

#ifdef KINDLE_FOR_TOUCH
    if (m_addFavouriteWindow != 0)
    {
        CDisplay::CacheDisabler cache;
        m_addFavouriteWindow->SetFocus(true);
        m_addFavouriteWindow->SetFocus(false);
    }
#endif

    BookStoreOrderManager* bookStoreOrderManager = BookStoreOrderManager::GetInstance();
    bool isBookAddedToFavourite = bookStoreOrderManager->IsBookAddedToFavourite(m_bookInfo->GetBookId().c_str());
    model::BookInfoList favouriteBooks;
    if (!isBookAddedToFavourite)
    {
        favouriteBooks.push_back(m_bookInfo);
        if (bookStoreOrderManager->AddFavouriteBooks(favouriteBooks) != FDR_FAILED)
        {
            m_favouriteAction = ADDING_FAVOURITE;
#ifdef KINDLE_FOR_TOUCH
            (dynamic_cast<UITouchComplexButton*>(m_addFavouriteWindow))->SetText(
                 StringManager::GetPCSTRById(BOOKSTORE_ADDING_FAVOURITE));
#else
            (dynamic_cast<UIComplexButton*>(m_addFavouriteWindow))->SetText(
                 StringManager::GetPCSTRById(BOOKSTORE_ADDING_FAVOURITE));
#endif
            m_addFavouriteWindow->SetEnable(false);
            CDisplay::CacheDisabler disableCache;
            m_addFavouriteWindow->UpdateWindow();
        }
    }
    else
    {
        favouriteBooks.push_back(m_bookInfo);
        if (bookStoreOrderManager->RemoveFavouriteBooks(favouriteBooks) != FDR_FAILED)
        {
            m_favouriteAction = REMOVING_FAVOURITE;
#ifdef KINDLE_FOR_TOUCH
            (dynamic_cast<UITouchComplexButton*>(m_addFavouriteWindow))->SetText(
                 StringManager::GetPCSTRById(BOOKSTORE_REMOVING_FAVOURITE));
#else
            (dynamic_cast<UIComplexButton*>(m_addFavouriteWindow))->SetText(
                 StringManager::GetPCSTRById(BOOKSTORE_REMOVING_FAVOURITE));
#endif
            m_addFavouriteWindow->SetEnable(false);
            CDisplay::CacheDisabler disableCache;
            m_addFavouriteWindow->UpdateWindow();
        }
    }
    FireBookStatusUpdateEvent(FavouriteBookStatusUpdate);
    return true;
}

bool UIBookStorePriceButtonController::OnAddCartButtonClick(const EventArgs&)
{
    if (!m_bookInfo)
    {
        return false;
    }
    if (m_currentAction != NONE)
    {
        return false;
    }
    if (!m_addCartWindow->IsVisible())
    {
        return false;
    }

#ifdef KINDLE_FOR_TOUCH
    if (m_addCartWindow != 0)
    {
        CDisplay::CacheDisabler cache;
        m_addCartWindow->SetFocus(true);
        m_addCartWindow->SetFocus(false);
    }
#endif

    BookStoreOrderManager* bookStoreOrderManager = BookStoreOrderManager::GetInstance();
    bool isBookAddedToCart = bookStoreOrderManager->IsBookAddedToCart(m_bookInfo->GetBookId().c_str());
    model::BookInfoList cartBooks;
    if (!isBookAddedToCart)
    {
        cartBooks.push_back(m_bookInfo);
        FetchDataResult fdr = bookStoreOrderManager->AddBooksToCart(cartBooks);
        if (fdr != FDR_FAILED)
        {
            m_cartAction = ADDING_CART;
#ifdef KINDLE_FOR_TOUCH
            (dynamic_cast<UITouchComplexButton*>(m_addCartWindow))->SetText(
                 StringManager::GetPCSTRById(BOOKSTORE_ADDING_TO_CART));
#else
            (dynamic_cast<UIComplexButton*>(m_addCartWindow))->SetText(
                 StringManager::GetPCSTRById(BOOKSTORE_ADDING_TO_CART));
#endif
            m_addCartWindow->SetEnable(false);
            CDisplay::CacheDisabler disableCache;
            m_addCartWindow->UpdateWindow();
        }
    }
    else
    {
        cartBooks.push_back(m_bookInfo);
        bookStoreOrderManager->RemoveBooksFromCart(cartBooks);
    }
    FireBookStatusUpdateEvent(AllStatusUpdate);
    return true;
}

void UIBookStorePriceButtonController::UpdateTrialBookStatus()
{
    if (NULL == m_trialWnd || !m_bookInfo || !m_duokanKey.empty())
    {
        return;
    }
    if (m_bookInfo->GetTrialUrl().empty())
    {
        m_trialWnd->SetEnable(false);
        m_trialWnd->SetVisible(false);
        return;
    }
    int trialBookStatus = TBS_UNKNOWN;
    if (0 == m_bookInfo->GetPrice()
        || 0 == m_bookInfo->GetDiscountPrice()
        || ((trialBookStatus = GetTrialBookStatus()) & TBS_PURCHASED))
    {
        m_trialWnd->SetEnable(false);
        m_trialWnd->SetVisible(false);
        return;
    }

    if (trialBookStatus & TBS_WAITING)
    {
        m_trialWnd->SetText(StringManager::GetPCSTRById(BOOKSTORE_WAITING));
        m_trialWnd->SetEnable(FALSE);
        m_currentAction = WAITING_QUEUE;
    }
    else
    {
        if (TBS_UNDOWNLOAD == trialBookStatus)
        {
            m_trialWnd->SetText(StringManager::GetPCSTRById(BOOKSTORE_FREECHAPTERS));
            m_trialWnd->SetVisible(true);
            m_trialWnd->SetEnable(true);
        }
        else if (TBS_DOWNLOADING == trialBookStatus)
        {
            char buf[100];
            snprintf(buf, DK_DIM(buf), "%d%%", m_trialDownloadingProgress < 0 ? 0 : m_trialDownloadingProgress);
            m_trialWnd->SetText(buf);
            m_trialWnd->SetVisible(true);
            m_trialWnd->SetEnable(false);
        }
        else if (TBS_DOWNLOADED == trialBookStatus)
        {
            m_trialWnd->SetText(StringManager::GetPCSTRById(BOOKSTORE_READTRIAL));
            m_trialWnd->SetVisible(true);
            m_trialWnd->SetEnable(true);
        }
    }
}

void UIBookStorePriceButtonController::UpdateFavouriteBookStatus()
{
    if (0 == m_addFavouriteWindow || 0 == m_bookInfo)
    {
        return;
    }

    if (m_favouriteAction == FAVOURITE_NONE)
    {
        BookStoreOrderManager* bookStoreOrderManager = BookStoreOrderManager::GetInstance();
        bool isBookAddedToFavourite = bookStoreOrderManager->IsBookAddedToFavourite(m_bookInfo->GetBookId().c_str());
        if (!isBookAddedToFavourite)
        {
            m_addFavouriteWindow->SetText(StringManager::GetPCSTRById(BOOKSTORE_ADD_FAVOURITE));
    #ifdef KINDLE_FOR_TOUCH
            (dynamic_cast<UITouchComplexButton*>(m_addFavouriteWindow))->SetIcons(ImageManager::GetImage(IMAGE_TOUCH_FAVOUR),
                                                                                    ImageManager::GetImage(IMAGE_TOUCH_FAVOUR),
                                                                                    UIButton::ICON_LEFT);
    #else
            (dynamic_cast<UIComplexButton*>(m_addFavouriteWindow))->SetRightIcon(ImageManager::GetImage(IMAGE_TOUCH_FAVOUR));
    #endif
            m_addFavouriteWindow->SetVisible(true);
            m_addFavouriteWindow->SetEnable(true);
        }
        else
        {
            m_addFavouriteWindow->SetText(StringManager::GetPCSTRById(BOOKSTORE_REMOVE_FAVOURITE));
    #ifdef KINDLE_FOR_TOUCH
            (dynamic_cast<UITouchComplexButton*>(m_addFavouriteWindow))->SetIcons(ImageManager::GetImage(IMAGE_TOUCH_FAVOURED),
                                                                                    ImageManager::GetImage(IMAGE_TOUCH_FAVOURED),
                                                                                    UIButton::ICON_LEFT);
    #else
            (dynamic_cast<UIComplexButton*>(m_addFavouriteWindow))->SetRightIcon(ImageManager::GetImage(IMAGE_TOUCH_FAVOURED));
    #endif
            m_addFavouriteWindow->SetVisible(true);
            m_addFavouriteWindow->SetEnable(true);
        }
    }
}

void UIBookStorePriceButtonController::UpdateBookInCartStatus()
{
    if (0 == m_addCartWindow || 0 == m_bookInfo)
    {
        return;
    }

    if (m_bookInfo->GetPrice() <= 0 && m_bookInfo->GetDiscountPrice() <= 0)
    {
        // Do not handle cart related events if this book is free.
        m_addCartWindow->SetVisible(false);
        m_addCartWindow->SetEnable(false);
        return;
    }

    if (m_cartAction == CART_NONE)
    {
        BookStoreOrderManager* bookStoreOrderManager = BookStoreOrderManager::GetInstance();
        int status = GetBookStatus();

        // Display add-to-cart button when it has not been purchased yet
        if (!(status & BS_UNPURCHASED) || (status & (BS_UNDOWNLOAD | BS_DOWNLOADED | BS_DOWNLOADING)))
        {
            m_addCartWindow->SetVisible(false);
            m_addCartWindow->SetEnable(false);
        }
        else
        {
            bool isBookAddedToCart = bookStoreOrderManager->IsBookAddedToCart(m_bookInfo->GetBookId().c_str());
            if (!isBookAddedToCart)
            {
                m_addCartWindow->SetText(StringManager::GetPCSTRById(BOOKSTORE_ADD_TO_CART));
                m_addCartWindow->SetVisible(true);
                m_addCartWindow->SetEnable(true);
            }
            else
            {
                m_addCartWindow->SetText(StringManager::GetPCSTRById(BOOKSTORE_REMOVE_FROM_CART));
                m_addCartWindow->SetVisible(true);
                m_addCartWindow->SetEnable(false);
            }
        }
    }
}

UIContainer* UIBookStorePriceButtonController::GetRepaintRoot()
{
#ifdef KINDLE_FOR_TOUCH
    //return m_root->GetParent();
    return m_root;
#else
    return m_root;
#endif
}
UIContainer* UIBookStorePriceButtonController::GetDlgParent()
{
#ifdef KINDLE_FOR_TOUCH
    return GUISystem::GetInstance()->GetTopFullScreenContainer();
#else
    return m_root;
#endif
}


bool UIBookStorePriceButtonController::OnDownloadUpdate(const EventArgs& args)
{
    if (!m_bookInfo)
    {
        return false;
    }

    static time_t last_update_time = 0;
    static const int UPDATE_OVERDUE = 2000;

    // arguments
    const BookDownloadStateUpdateArgs& update_args = (const BookDownloadStateUpdateArgs&)args;
    string task_id = update_args.task_id;
    string book_id = update_args.book_id;
    if (task_id.empty() && book_id.empty())
    {
        // Mandatory update
        FireBookStatusUpdateEvent(BookStatusUpdate);
        return true;
    }

    string this_book_url_id = BookStoreInfoManager::BuildUrlIdForBook(m_bookInfo->GetBookId().c_str());
    string this_book_trail_id = BookStoreInfoManager::BuildUrlIdForTrialVersion(m_bookInfo->GetBookId().c_str());
    if (task_id != this_book_url_id && task_id != this_book_trail_id && book_id != m_bookInfo->GetBookId())
    {
        return false;
    }

    int progress = update_args.progress;
    int state = update_args.state;
    bool need_update = false;
    time_t current_time = SystemUtil::GetUpdateTimeInMs();
    if (state & (IDownloadTask::WORKING | IDownloadTask::WAITING))
    {
        m_currentAction = DOWNLOADING;
        need_update = ((last_update_time == 0) ||
                       ((current_time - last_update_time) >= UPDATE_OVERDUE) ||
                       (progress >= 100));
    }
    else if (state & IDownloadTask::FETCHING_DRM)
    {
        m_currentAction = FETCH_DRM;
    }
    else if (state & (IDownloadTask::FAILED | IDownloadTask::CANCELED))
    {
        m_downloadingProgress = -1;
        if (m_currentAction != NONE)
        {
            m_currentAction = NONE;
            need_update = true;
        }
        else
        {
            return true;
        }
    }
    else if (state & IDownloadTask::WAITING_QUEUE)
    {
        m_downloadingProgress = -1;
        if (m_currentAction != WAITING_QUEUE)
        {
            m_currentAction = WAITING_QUEUE;
            need_update = true;
        }
        else
        {
            return true;
        }
    }

    if (task_id == this_book_url_id || book_id == m_bookInfo->GetBookId())
    {
        if (progress >= 0) m_downloadingProgress = progress;
        if (need_update)
        {
            FireBookStatusUpdateEvent(BookStatusUpdate);
        }
    }
    else if (task_id == this_book_trail_id || book_id == m_bookInfo->GetBookId())
    {
        if (progress >= 0) m_trialDownloadingProgress = progress;
        if (need_update)
        {
            FireBookStatusUpdateEvent(TrialBookStatusUpdate);
        }
    }

    if (need_update)
    {
        last_update_time = current_time;
    }
    return true;
}

void UIBookStorePriceButtonController::DoExchangeDuokanKey()
{
    BookStoreInfoManager*pManager = BookStoreInfoManager::GetInstance();
    if (pManager && !m_duokanKey.empty())
    {
        m_actionWnd->SetEnable(false);
        pManager->ExchangeDuokanKey(m_duokanKey.c_str());
    }
}

bool UIBookStorePriceButtonController::OnExchangeDuokanKeyUpdate(const EventArgs& args)
{
    BookStoreInfoManager*pManager = BookStoreInfoManager::GetInstance();
    model::BookInfo* bookInfo = m_bookInfo.get();
    if ((NULL == pManager) || (NULL == bookInfo))
    {
        return false;
    }

    m_duokanKey.clear();
    m_actionWnd->SetEnable(true);
    const ExchangeDuokanKeyUpdateArgs& msgArgs = dynamic_cast<const ExchangeDuokanKeyUpdateArgs&>(args);
    string resultMsg;
    if (msgArgs.result > 0)
    {
        switch(msgArgs.result)
        {
        case REDEEM_TOO_LONG_WORDS:
            resultMsg = StringManager::GetPCSTRById(BOOKSTORE_REDEEM_TOO_LONG_WORDS);
            break;
        case REDEEM_WRONG_DEST_EMAIL:
            resultMsg = StringManager::GetPCSTRById(BOOKSTORE_REDEEM_WRONG_DEST_EMAIL);
            break;
        case REDEEM_ILLIGEL_CODE:
            resultMsg = StringManager::GetPCSTRById(BOOKSTORE_REDEEM_ILLIGEL_CODE);
            break;
        case REDEEM_INVALID_CODE:
            resultMsg = StringManager::GetPCSTRById(BOOKSTORE_REDEEM_INVALID_CODE);
            break;
        case REDEEM_CAN_NOT_USE_FREE_BOOK:
            resultMsg = StringManager::GetPCSTRById(BOOKSTORE_REDEEM_CAN_NOT_USE_FREE_BOOK);
            break;
        case REDEEM_BOOK_PURCHASED:
            resultMsg = StringManager::GetPCSTRById(BOOKSTORE_REDEEM_BOOK_PURCHASED);
            break;
        case REDEEM_BOOK_NOT_EXIST:
            resultMsg = StringManager::GetPCSTRById(BOOKSTORE_REDEEM_BOOK_NOT_EXIST);
            break;
        default:
            resultMsg.clear();
            break;
        }

        if (!resultMsg.empty())
        {
            CDisplay::GetDisplay()->SetScreenTips(resultMsg.c_str());
        }
        FireBookStatusUpdateEvent(BookStatusUpdate);
    }
    else if (msgArgs.result == 0)
    {
        // TODO(JUGH):此处的if没有提到外层是因为下载书籍过程中有可能引起UIMessageBox响应不了按键响应
        const string& bookId = m_bookInfo->GetBookId();
        m_bookInfo->MutableOrderId() = msgArgs.order_uuid;
        pManager->FetchPurchasedList(); 
        pManager->FetchDrmCert(bookId.c_str(),
            m_bookInfo->GetRevision().c_str(),
            m_bookInfo->GetOrderId().c_str());
        m_currentAction = FETCH_DRM;
    }
    return true;
}

bool UIBookStorePriceButtonController::CheckBookKernelVersion(const std::string& kernelVersion)
{
	if(!kernelVersion.empty() && strcmp(kernelVersion.c_str(), DK_GetKernelVersion()))
	{
		UIMessageBox messagebox(GetDlgParent(), StringManager::GetStringById(BOOKSTORE_LOW_VERSION), MB_OK | MB_CANCEL);
		messagebox.SetButtonText(MB_OK, StringManager::GetPCSTRById(BOOKSTORE_REED_UPGRADE));
        if(MB_OK == messagebox.DoModal())
    	{
    		DKAutoUpgrade::UpgradeModelImpl* pUpgradeModel = DKAutoUpgrade::UpgradeModelImpl::GetInstance();
		    pUpgradeModel->CheckNewRelease(true);
    	}
		return false;
	}
	return true;
}

