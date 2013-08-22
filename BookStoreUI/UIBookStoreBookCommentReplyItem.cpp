#include "BookStoreUI/UIBookStoreBookCommentReplyItem.h"
#include "Utility/ColorManager.h"
#include "GUI/UISizer.h"
#include "Common/WindowsMetrics.h"
#include "Utility/StringUtil.h" 

using dk::utility::StringUtil;
using namespace WindowsMetrics;
using namespace dk::bookstore;

UIBookStoreBookCommentReplyItem::UIBookStoreBookCommentReplyItem()
{
    Init();
}

UIBookStoreBookCommentReplyItem::~UIBookStoreBookCommentReplyItem()
{

}

void UIBookStoreBookCommentReplyItem::SetInfoSPtr(dk::bookstore::model::BasicObjectSPtr info, int index)
{
    if (info)
    {
        m_commentReply.reset((model::CommentReply*)info.get()->Clone());
        m_replyTime.SetText(m_commentReply->GetPublishTime().c_str());
        string nick = m_commentReply->GetReplierNick();
        StringUtil::HideEmailInfo(nick);
        m_userId.SetText(nick.c_str());
        m_replyContent.SetText(m_commentReply->GetContent().c_str());
        Layout();
    }
}

void UIBookStoreBookCommentReplyItem::MoveWindow(INT32 left, INT32 top, INT32 width, INT32 height)
{
    UIWindow::MoveWindow(left, top, width, height);

    if (!m_windowSizer)
    {
        UISizer* pMainSizer = new UIBoxSizer(dkVERTICAL);
        UISizer* pLine0Sizer = new UIBoxSizer(dkHORIZONTAL);
        if (pMainSizer && pLine0Sizer)
        {
            const int elemspacing = GetWindowMetrics(UIPixelValue10Index);
            const int topMargin = GetWindowMetrics(UIPixelValue20Index);
            const int bottomMargin = GetWindowMetrics(UIPixelValue16Index);
            pLine0Sizer->Add(&m_userId, UISizerFlags(1).Align(dkALIGN_CENTRE_VERTICAL));
            pLine0Sizer->Add(&m_replyTime, UISizerFlags().Border(dkLEFT, elemspacing).Align(dkALIGN_CENTRE_VERTICAL));

            pMainSizer->Add(pLine0Sizer, UISizerFlags().Expand().Border(dkTOP | dkBOTTOM, topMargin));
            pMainSizer->Add(&m_replyContent, UISizerFlags(1).Expand().Border(dkBOTTOM, bottomMargin));
            SetSizer(pMainSizer);
        }
        else
        {
            SafeDeletePointer(pMainSizer);
            SafeDeletePointer(pLine0Sizer);
        }
    }
}

void UIBookStoreBookCommentReplyItem::Init()
{
    const int fontsize14 = GetWindowFontSize(FontSize14Index);
    const int fontsize18 = GetWindowFontSize(FontSize18Index);
    m_userId.SetFontSize(fontsize14);
    AppendChild(&m_userId);

    m_replyTime.SetFontSize(fontsize14);
    AppendChild(&m_replyTime);

    m_replyContent.SetFontSize(fontsize18);
    m_replyContent.SetLineSpace(1);
    m_replyContent.SetVAlign(ALIGN_LEFT);
    m_replyContent.SetMode(UIAbstractText::TextMultiLine);
    AppendChild(&m_replyContent);
}

