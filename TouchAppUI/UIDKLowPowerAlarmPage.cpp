#include "TouchAppUI/UIDKLowPowerAlarmPage.h"
#include "ImageHandler/ImageHandler.h"
#include "Utility/ImageManager.h"
#include "Utility/PathManager.h"
#include "../ImageHandler/DkImageHelper.h"
#include "GUI/CTpGraphics.h"
#include "Utility/ColorManager.h"

using namespace std;
using namespace DkFormat;

const std::string UIDKLowPowerAlarmPage::UIDKLOWPOWERALARMPAGE;

UIDKLowPowerAlarmPage::UIDKLowPowerAlarmPage()
{
    Init();
}

UIDKLowPowerAlarmPage::~UIDKLowPowerAlarmPage()
{

}

void UIDKLowPowerAlarmPage::Init()
{
    m_lowPowerImg.SetImage(ImageManager::GetImage(IMAGE_DK_LOWPOWER_ALARM));
    AppendChild(&m_lowPowerImg);
}

BOOL UIDKLowPowerAlarmPage::OnKeyPress(INT32 iKeyCode)
{
    return FALSE;
}

void UIDKLowPowerAlarmPage::MoveWindow(INT32 left, INT32 top, INT32 width, INT32 height)
{
    UIPage::MoveWindow(left, top, width, height);
    m_lowPowerImg.MoveWindow((width - m_lowPowerImg.GetImageWidth()) >> 1, 
        (height - m_lowPowerImg.GetImageHeight()) >> 1, 
        m_lowPowerImg.GetImageWidth(), 
        m_lowPowerImg.GetImageHeight());
}
