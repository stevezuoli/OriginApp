#ifndef _DKREADER_APPUI_UIDKLOWPOWERALARMPAGE_H_
#define _DKREADER_APPUI_UIDKLOWPOWERALARMPAGE_H_

#include "GUI/UIPage.h"
#include "GUI/UIImage.h"
#include <string>

class UIDKLowPowerAlarmPage : public UIPage
{
public:
    static const std::string UIDKLOWPOWERALARMPAGE;
    UIDKLowPowerAlarmPage();
    ~UIDKLowPowerAlarmPage();
    virtual LPCSTR GetClassName() const
    {
        return (UIDKLOWPOWERALARMPAGE.c_str());
    }
    BOOL OnKeyPress(INT32 iKeyCode);
    void MoveWindow(INT32 left, INT32 top, INT32 width, INT32 height);

private:
    void Init();
private:
    UIImage m_lowPowerImg;
};
#endif//_DKREADER_APPUI_UIDKLOWPOWERALARMPAGE_H_