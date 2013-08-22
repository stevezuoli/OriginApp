#include <tr1/functional>
#include "PersonalUI/UIPersonalSystemMessageItem.h"
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

UIPersonalSystemMessageItem::UIPersonalSystemMessageItem()
{
    Init();
}

UIPersonalSystemMessageItem::~UIPersonalSystemMessageItem()
{

}

void UIPersonalSystemMessageItem::Init()
{
    const int fontsize16 = GetWindowFontSize(FontSize16Index);
    SetLeftMargin(GetWindowMetrics(UIPixelValue30Index));

    m_title.SetFontSize(fontsize16);
	m_title.SetForeColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
	m_title.SetText(StringManager::GetStringById(PERSONAL_MESSAGE_SYSTEM));
    AppendChild(&m_title);

    m_message.SetFontSize(fontsize16);
    m_message.SetLineSpace(1);
    m_message.SetVAlign(ALIGN_LEFT);
    m_message.SetMode(UIAbstractText::TextMultiLine);
    AppendChild(&m_message);

	m_time.SetForeColor(ColorManager::GetColor(COLOR_MENUITEM_INACTIVE));
    m_time.SetFontSize(fontsize16);
    m_time.SetEnglishGothic();
    AppendChild(&m_time);
    
    UISizer* pMainSizer = new UIBoxSizer(dkHORIZONTAL);
    UISizer* pMessageSizer = new UIBoxSizer(dkVERTICAL);
    UISizer* pTitleSizer = new UIBoxSizer(dkHORIZONTAL);
    if (pMainSizer && pMessageSizer && pTitleSizer)
    {
        const int verticalMargin = GetWindowMetrics(UIPixelValue16Index);
		pTitleSizer->Add(&m_title, UISizerFlags().Expand().Align(dkALIGN_LEFT));
		pTitleSizer->AddStretchSpacer();
        pTitleSizer->Add(&m_time, UISizerFlags().Expand().Align(dkALIGN_RIGHT));

        pMessageSizer->AddSpacer(verticalMargin);
        pMessageSizer->Add(pTitleSizer, UISizerFlags().Expand());
        pMessageSizer->Add(&m_message, UISizerFlags(1).Expand().Border(dkTOP | dkBOTTOM, verticalMargin));
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

void UIPersonalSystemMessageItem::SetInfoSPtr(dk::bookstore::model::BasicObjectSPtr messageInfo, int index)
{
    m_messageInfo.reset((model::PushedMessageInfo*)messageInfo.get()->Clone());
    if(m_messageInfo)
    {
        m_message.SetText(m_messageInfo->GetMessage().c_str());
        m_time.SetText(m_messageInfo->GetStartTime().c_str());
        Layout();
    }
}


bool UIPersonalSystemMessageItem::ResponseTouchTap()
{
    if(m_messageInfo)
    {
        UIPersonalPushedTextPage* pPage = new UIPersonalPushedTextPage(m_title.GetText(), m_messageInfo->GetMessage());
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

