#include <tr1/functional>
#include "BookStoreUI/UIBookStoreBookCommentDetailedPage.h"
#include "PersonalUI/UIBoxMessagesCommentReplyItem.h"
#include "PersonalUI/UIPersonalPushedTextPage.h"
#include "PersonalUI/PushedMessagesManager.h"
#include "I18n/StringManager.h"
#include "GUI/CTpGraphics.h"
#include "CommonUI/CPageNavigator.h"
#include "drivers/DeviceInfo.h"
#include "GUI/GUISystem.h"
#include "GUI/UIMessageBox.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "Framework/CDisplay.h"
#include "Common/CAccountManager.h"
#include "CommonUI/UIUtility.h"
#ifdef KINDLE_FOR_TOUCH
#include "TouchAppUI/UIBookMenuDlg.h"
#else
#include "AppUI/UIBookMenuDlg.h"
#endif

using namespace dk::account;
using namespace dk::utility;
using namespace WindowsMetrics;
using dk::bookstore::model::PushedMessageInfo;
using namespace dk::bookstore;

enum BOXMESSAGES_CMD
{
    BOXMESSAGES_CMD_GOTOCOMMENT = 0x4000,
    BOXMESSAGES_CMD_GOTOUSER,
    BOXMESSAGES_CMD_DELETE
};

UIBoxMessagesCommentReplyItem::UIBoxMessagesCommentReplyItem()
{
#ifdef KINDLE_FOR_TOUCH
    SetLongClickable(true);
#endif
    Init();
    SubscribeMessageEvent(BookStoreInfoManager::EventCommentReplyUpdate, 
        *BookStoreInfoManager::GetInstance(),
        std::tr1::bind(
        std::tr1::mem_fn(&UIBoxMessagesCommentReplyItem::OnMessageCommentDetailedInfoUpdate),
        this,
        std::tr1::placeholders::_1)
        );
}

UIBoxMessagesCommentReplyItem::~UIBoxMessagesCommentReplyItem()
{

}

void UIBoxMessagesCommentReplyItem::Init()
{
    const int infoFontSize = GetWindowFontSize(FontSize16Index);
    const int commentFontSize = GetWindowFontSize(FontSize16Index);
    const int replyFontSize = GetWindowFontSize(FontSize18Index);

    m_replyAlias.SetFontSize(infoFontSize);
    AppendChild(&m_replyAlias);

    m_replyAliasFollow.SetFontSize(infoFontSize);
    m_replyAliasFollow.SetForeColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
    AppendChild(&m_replyAliasFollow);

    m_time.SetForeColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
    m_time.SetFontSize(infoFontSize);
    m_time.SetEnglishGothic();
    m_time.SetAlign(ALIGN_RIGHT);
    AppendChild(&m_time);

    m_replyContent.SetFontSize(replyFontSize);
    m_replyContent.SetLineSpace(1);
    m_replyContent.SetVAlign(ALIGN_LEFT);
    m_replyContent.SetMode(UIAbstractText::TextMultiLine);
    AppendChild(&m_replyContent);

    m_comment.SetForeColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
    m_comment.SetFontSize(commentFontSize);
    AppendChild(&m_comment);

    m_imgComment.SetImage(IMAGE_ICON_I_COMMENT);
    AppendChild(&m_imgComment);

    m_imgArrow.SetImage(IMAGE_ICON_I_TRIANGLE);
    m_imgArrow.SetAlign(ALIGN_CENTER);
    AppendChild(&m_imgArrow);
    
    UISizer* pMainSizer = new UIBoxSizer(dkVERTICAL);
    UISizer* pTitleSizer = new UIBoxSizer(dkHORIZONTAL);
    UISizer* pArrowSizer = new UIBoxSizer(dkHORIZONTAL);
    if (pMainSizer && pTitleSizer && pArrowSizer)
    {
        const int lineMargin = GetWindowMetrics(UIPixelValue8Index);
        const int pixelValue10 = GetWindowMetrics(UIPixelValue10Index);
        const int largeMargin = GetWindowMetrics(UIPixelValue14Index);
        const int leftMargin = GetWindowMetrics(UIPixelValue50Index);
        const int rightMargin = GetWindowMetrics(UIPixelValue30Index);
        const int imgCommentMargin = (leftMargin - m_imgComment.GetWidth()) >> 1;
        pTitleSizer->Add(&m_imgComment, UISizerFlags().Expand().Border(dkLEFT | dkRIGHT, imgCommentMargin));
        pTitleSizer->Add(&m_replyAlias, UISizerFlags().Expand().Reduce(1));
        pTitleSizer->Add(&m_replyAliasFollow, UISizerFlags().Expand().Border(dkLEFT, pixelValue10));
        pTitleSizer->AddStretchSpacer();
        pTitleSizer->Add(&m_time, UISizerFlags().Border(dkLEFT, largeMargin).Align(dkALIGN_RIGHT));

        pArrowSizer->AddStretchSpacer();
        pArrowSizer->Add(&m_imgArrow);
        pArrowSizer->AddStretchSpacer();

        pMainSizer->AddSpacer(largeMargin);
        pMainSizer->Add(pTitleSizer, UISizerFlags().Expand().Border(dkRIGHT, rightMargin));
        pMainSizer->Add(&m_replyContent, UISizerFlags(1).Expand().Border(dkTOP, lineMargin).Reduce(1).Border(dkLEFT, leftMargin).Border(dkRIGHT, rightMargin));
        pMainSizer->Add(pArrowSizer, UISizerFlags().Expand().Border(dkTOP | dkBOTTOM, lineMargin));
        pMainSizer->Add(&m_comment, UISizerFlags().Expand().Border(dkLEFT, leftMargin).Border(dkRIGHT, rightMargin));
        pMainSizer->AddSpacer(largeMargin);

        SetSizer(pMainSizer);
    }
    else
    {
        SafeDeletePointer(pMainSizer);
        SafeDeletePointer(pTitleSizer);
        SafeDeletePointer(pArrowSizer);
    }
}

void UIBoxMessagesCommentReplyItem::SetInfoSPtr(dk::bookstore::model::BasicObjectSPtr messageInfo, int index)
{
    m_messageInfo.reset((model::PushedMessageInfo*)messageInfo.get()->Clone());
    if(m_messageInfo)
    {
    	model::BoxMessagesCommentReplySPtr replyInfo(dynamic_cast<model::BoxMessagesCommentReply*>(m_messageInfo->GetMessageInfo()->Clone()));
        if (replyInfo)
        {
            string myAlias;
            CAccountManager* account = CAccountManager::GetInstance();
            if (account)
            {
                myAlias = account->GetAliasFromFile();
            }
            string replyAlias, time, replyContent, comment;
            switch(m_messageInfo->GetMessageType())
            {
            case model::BOXMESSAGES_COMMENT_REPLY:
                {
                    replyAlias = replyInfo->GetReplyAlias();
                    time = replyInfo->GetReplyTime();
                    comment = replyInfo->GetComment();

                    char tmp[512] = {0};
                    snprintf(tmp, DK_DIM(tmp), StringManager::GetPCSTRById(PERSONAL_MESSAGES_MYCOMMENT_REPLY), myAlias.c_str(), replyInfo->GetReply().c_str());
                    replyContent = tmp;
                }
                break;
            case model::BOXMESSAGES_COMMENT_REREPLY:
                {
                    replyAlias = replyInfo->GetRereplyAlias();
                    time = replyInfo->GetRereplyTime();
                    replyContent = replyInfo->GetRereply();

                    char tmp[512] = {0};
                    snprintf(tmp, DK_DIM(tmp), StringManager::GetPCSTRById(PERSONAL_MESSAGES_MYCOMMENT_REPLY), replyAlias.c_str(), replyInfo->GetReply().c_str());
                    comment = tmp;
                }
                break;
            default:
                break;
            }
            m_replyAlias.SetText(replyAlias.c_str());
            m_replyAliasFollow.SetText(StringManager::GetPCSTRById(PERSONAL_MESSAGES_COMMENT_REPLY_TITLE));
            m_time.SetText(time.c_str());
            m_replyContent.SetText(replyContent.c_str());
            m_comment.SetText(comment.c_str());
        }
    }
}

bool UIBoxMessagesCommentReplyItem::ResponseTouchTap()
{
    if(m_messageInfo && m_messageInfo->GetObjectType() == model::OT_BOXMESSAGES)
    {
    	model::BoxMessagesCommentReplySPtr replyInfo(dynamic_cast<model::BoxMessagesCommentReply*>(m_messageInfo->GetMessageInfo()->Clone()));
        if (replyInfo)
        {
            FetchCommentReply(replyInfo->GetBookId().c_str(), replyInfo->GetCommentId().c_str(), 0, 100);
        }
    }

    return true;
}

FetchDataResult UIBoxMessagesCommentReplyItem::FetchCommentReply(
    const std::string& bookId, 
    const std::string& commentId, 
    int start, 
    int length)
{
    BookStoreInfoManager* bookstoreManager = BookStoreInfoManager::GetInstance();
    if (bookstoreManager)
    {
        SetEnable(false);
        return bookstoreManager->FetchCommentReply(bookId.c_str(), commentId.c_str(), start, length);
    }
    return FDR_FAILED;
}

bool UIBoxMessagesCommentReplyItem::OnMessageCommentDetailedInfoUpdate(const EventArgs& args)
{
    if (IsDisplay() && m_messageInfo)
    {
        const CommentReplyArgs& msgArgs = (const CommentReplyArgs&)args;
        if (msgArgs.succeeded)
        {
        	model::BoxMessagesCommentReplySPtr replyInfo(dynamic_cast<model::BoxMessagesCommentReply*>(m_messageInfo->GetMessageInfo()->Clone()));
            model::BookCommentSPtr commentSptr = msgArgs.commentInfo;
            if (commentSptr && replyInfo
                && (commentSptr->GetBookId() == replyInfo->GetBookId()) 
                && (commentSptr->GetCommentId() == replyInfo->GetCommentId()))
            {
                std::string name = replyInfo->GetBookName();
                if (name.empty())
                {
                    name = StringManager::GetPCSTRById(PERSONAL_MESSAGES_COMMENTPAGE_TITLE);
                }
                UIBookStoreBookCommentDetailedPage *pDetailedPage = new UIBookStoreBookCommentDetailedPage(commentSptr->GetBookId(), name);
                if(pDetailedPage)
                {
                    pDetailedPage->SetCommentInfo(commentSptr);
                    pDetailedPage->MoveWindow(0, 0, DeviceInfo::GetDisplayScreenWidth(), DeviceInfo::GetDisplayScreenHeight());
                    CPageNavigator::Goto(pDetailedPage);
                }
            }
        }
        else
        {
            CDisplay::CacheDisabler cache;
            UIUtility::SetScreenTips(StringManager::GetCommentReplyErrorMessage(msgArgs.errorCode));
        }
    }
    SetEnable(true);
    return true; 
}

bool UIBoxMessagesCommentReplyItem::ResponseOperation() 
{
    return false;
    /*
    if (!m_messageInfo)
    {
        return false;
    }

    UIContainer* pParent = GUISystem::GetInstance()->GetTopFullScreenContainer();
    if (NULL == pParent)
    {
        pParent = m_pParent;
    }

    model::BoxMessagesCommentReplySPtr replyInfo =  m_messageInfo->GetReplyInfo();
    if (NULL == replyInfo)
    {
        return false;
    }
    string replyAlias;
    switch(m_messageInfo->GetMessageType())
    {
    case model::BOXMESSAGES_COMMENT_REPLY:
        {
            replyAlias = replyInfo->GetReplyAlias();
        }
        break;
    case model::BOXMESSAGES_COMMENT_REREPLY:
        {
            replyAlias = replyInfo->GetRereplyAlias();
        }
        break;
    default:
        break;
    }

    std::vector<int> customizedBtnIDs;
    customizedBtnIDs.push_back(BOXMESSAGES_CMD_GOTOCOMMENT);
    //customizedBtnIDs.push_back(BOXMESSAGES_CMD_GOTOUSER);
    customizedBtnIDs.push_back(BOXMESSAGES_CMD_DELETE);
    customizedBtnIDs.push_back(ID_INVALID);
    std::vector<string> customizedStrings;
    customizedStrings.push_back(StringManager::GetPCSTRById(GOTO_MESSAGES_COMMENTDETAILINFO));
    char name[256] = {0};
    snprintf(name, DK_DIM(name), StringManager::GetPCSTRById(GOTO_MESSAGES_USERDETAILINFO), replyAlias.c_str());
    //customizedStrings.push_back(name);
    customizedStrings.push_back(StringManager::GetPCSTRById(BOOK_DELETE));
    customizedStrings.push_back(string());
#ifndef KINDLE_FOR_TOUCH
    vector<char> customizedShortKeys;
    customizedShortKeys.push_back('A');
    customizedShortKeys.push_back('B');
    customizedShortKeys.push_back('C');
    customizedShortKeys.push_back('D');
#endif

#ifdef KINDLE_FOR_TOUCH
    UIBookMenuDlg dlgMenu(
        GUISystem::GetInstance()->GetTopFullScreenContainer(),
        customizedBtnIDs, 
        customizedStrings);
    dlgMenu.MakeCenter(dlgMenu.GetWidth(), dlgMenu.GetHeight());
#else
    UIBookMenuDlg dlgMenu(
        GUISystem::GetInstance()->GetTopFullScreenContainer(), 
        replyInfo->GetBookName().c_str(),
        customizedBtnIDs, 
        customizedStrings,
        customizedShortKeys);
    const int margin = GetWindowMetrics(UIPixelValue10Index);
    const int height = dlgMenu.GetHeight();
    const int y = GetAbsoluteY();
    const int left = GetWindowMetrics(UIPixelValue30Index);
    const int width = DeviceInfo::GetDisplayScreenWidth() - (left << 1);
    int top = y + GetHeight() + margin;
    if (top + height > DeviceInfo::GetDisplayScreenHeight())
    {
        top = y - margin - height;
    }
    dlgMenu.MoveWindow(left, top, width, height);
#endif
    if (dlgMenu.DoModal() == IDOK)
    {
        switch (dlgMenu.GetCommandId())
        {
        case BOXMESSAGES_CMD_GOTOCOMMENT:
            return ResponseTouchTap();
        case BOXMESSAGES_CMD_GOTOUSER:
            break;
        case BOXMESSAGES_CMD_DELETE:
            {
                UIMessageBox messagebox(pParent, StringManager::GetStringById(SURE_DELETE_PUSHEDMESSAGE), MB_OK | MB_CANCEL);
                if(MB_OK == messagebox.DoModal())
                {
                    PushedMessagesManager* pMgr = PushedMessagesManager::GetInstance();
                    if (pMgr && m_messageInfo)
                    {
                        pMgr->DeleteReadedMessage(m_messageInfo->GetMessageId());
                    }
                    return true;
                }
            }
            break;
        default:
            break;
        }
    }
    return false;*/
}

HRESULT UIBoxMessagesCommentReplyItem::DrawBackGround(DK_IMAGE drawingImg)
{
    CTpGraphics grf(drawingImg);
    HRESULT hr(S_OK);
    RTN_HR_IF_FAILED(grf.EraserBackGround());
    int borderLine = GetWindowMetrics(UIBtnBorderLineIndex);
    StrokeStyle style = DOTTED_STROKE;
    if (IsFocus())
    {
        borderLine = GetWindowMetrics(UIDialogBorderLineIndex);
        style = SOLID_STROKE;
    }
    const int leftMargin = GetWindowMetrics(UIPixelValue50Index);
    const int rightMargin = GetWindowMetrics(UIPixelValue30Index);
    RTN_HR_IF_FAILED(grf.DrawLine(leftMargin, m_iHeight - borderLine, m_iWidth - leftMargin - rightMargin, borderLine,style));
    return hr;
}
