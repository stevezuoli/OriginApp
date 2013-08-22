#include <tr1/functional>
#include "PersonalUI/UIPersonalPushedMessagesItem.h"
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

using namespace WindowsMetrics;
using dk::bookstore::model::PushedMessageInfo;
using namespace dk::bookstore;

UIPersonalPushedMessagesItem::UIPersonalPushedMessagesItem()
{
#ifdef KINDLE_FOR_TOUCH
    SetLongClickable(true);
#endif
    Init();
}

UIPersonalPushedMessagesItem::~UIPersonalPushedMessagesItem()
{

}

void UIPersonalPushedMessagesItem::Init()
{
    const int fontsize14 = GetWindowFontSize(FontSize14Index);
    const int fontsize22 = GetWindowFontSize(FontSize22Index);
    const int messageFontSize = GetWindowFontSize(FontSize16Index);
    SetLeftMargin(GetWindowMetrics(UIPixelValue30Index));

    m_title.SetFontSize(fontsize22);
    AppendChild(&m_title);

    m_message.SetFontSize(messageFontSize);
    m_message.SetLineSpace(1);
    m_message.SetVAlign(ALIGN_LEFT);
    m_message.SetMode(UIAbstractText::TextMultiLine);
    AppendChild(&m_message);

    m_time.SetFontSize(fontsize14);
    m_time.SetEnglishGothic();
    AppendChild(&m_time);
    
    UISizer* pMainSizer = new UIBoxSizer(dkHORIZONTAL);
    UISizer* pMessageSizer = new UIBoxSizer(dkVERTICAL);
    UISizer* pTitleSizer = new UIBoxSizer(dkHORIZONTAL);
    if (pMainSizer && pMessageSizer && pTitleSizer)
    {
        const int pixelValue10 = GetWindowMetrics(UIPixelValue10Index);
        const int verticalMargin = GetWindowMetrics(UIPixelValue16Index);
        pTitleSizer->Add(&m_title, UISizerFlags().Expand().Border(dkRIGHT, pixelValue10));

        pMessageSizer->AddSpacer(verticalMargin);
        pMessageSizer->Add(pTitleSizer, UISizerFlags().Expand());
        pMessageSizer->Add(&m_message, UISizerFlags(1).Expand().Border(dkTOP | dkBOTTOM, verticalMargin));
        pMessageSizer->Add(&m_time, UISizerFlags().Expand());
        pMessageSizer->AddSpacer(verticalMargin);
        pMainSizer->Add(pMessageSizer, UISizerFlags(1).Expand().Border(dkLEFT | dkRIGHT, m_iLeftMargin));
        SetSizer(pMainSizer);
    }
    else
    {
        SafeDeletePointer(pMainSizer);
        SafeDeletePointer(pMessageSizer);
        SafeDeletePointer(pTitleSizer);
    }
}

void UIPersonalPushedMessagesItem::SetInfoSPtr(dk::bookstore::model::BasicObjectSPtr messageInfo, int index)
{
    m_messageInfo.reset((model::PushedMessageInfo*)messageInfo.get()->Clone());
    if(m_messageInfo)
    {
        m_messageID = m_messageInfo->GetMessageId();
        m_title.SetText(m_messageInfo->GetTitle().c_str());
        m_message.SetText(m_messageInfo->GetMessage().c_str());
        m_time.SetText(m_messageInfo->GetStartTime().c_str());
        Layout();
    }
}


bool UIPersonalPushedMessagesItem::ResponseTouchTap()
{
    if(m_messageInfo)
    {
        UIPersonalPushedTextPage* pPage = new UIPersonalPushedTextPage(m_messageInfo->GetTitle(), m_messageInfo->GetMessage());
        if (pPage)
        {
            pPage->MoveWindow(0, 0, DeviceInfo::GetDisplayScreenWidth(), DeviceInfo::GetDisplayScreenHeight());
            CPageNavigator::Goto(pPage);
        }
        else
        {
            SafeDeletePointer(pPage);
        }
    }
    return true;
}

bool UIPersonalPushedMessagesItem::ResponseOperation() 
{
    UIContainer* pParent = GUISystem::GetInstance()->GetTopFullScreenContainer();
    if (NULL == pParent)
    {
        pParent = m_pParent;
    }

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
    return false;
}
