
////////////////////////////////////////////////////////////////////////
// UIDateSettingDlg.h
// Contact:
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UIDATESETTINGDLG_H__
#define __UIDATESETTINGDLG_H__

#include "GUI/UIDialog.h"
#include "GUI/UITouchComplexButton.h"
#include "GUI/UITextSingleLine.h"

class UIDateOrTimeSettingDlg : public UIDialog
{
public:
    enum
    {
        UIDATESETTING = 0,
        UITIMESETTING,
        UITIMEZONESETTING,
    };
    UIDateOrTimeSettingDlg(UIContainer* _pParent, int settingType = UIDATESETTING);
    ~UIDateOrTimeSettingDlg();
    
    virtual LPCSTR GetClassName() const
    {
        return "UIDateOrTimeSettingDlg";
    }

    virtual void    MoveWindow(int left, int top, int width, int height);
    virtual void    OnCommand(DWORD dwCmdId, UIWindow* pSender, DWORD dwParam);
    virtual void    DrawBackground(DK_IMAGE drawingImg);
    virtual HRESULT Draw(DK_IMAGE drawingImg);
    
private:
    void Init();
    int  GetDaysOfMonth(unsigned int year, unsigned int month);
    void GetTime();
    void GetDate();
    void GetTimeZone();
    void UpdateContent();
    void UpdateYear(bool add);
    void UpdateMonth(bool add);
    void UpdateDay(bool add);
    void UpdateHour(bool add);
    void UpdateMinute(bool add);
    void UpdateAMorPM(bool add);
    void UpdateTimeZoneEast(bool add);
    void UpdateTimeZoneFull(bool add);
    void UpdateTimeZoneHalf(bool add);
    void Commit();
    inline void LimitNumber(unsigned int& number, unsigned int min, unsigned int max);
private:
    enum
    {
        INDEX_LEFT = 0,
        INDEX_MIDDLE,
        INDEX_RIGHT,
        INDEX_END
    };
    UITouchComplexButton m_btnUp[3];
    UITouchComplexButton m_btnDown[3];
    unsigned int m_content[3];
    UITextSingleLine m_txtContent[3];

    UITouchComplexButton m_btnCancel;
    UITouchComplexButton m_btnSave;

    UITextSingleLine m_txtTitle;
    int m_settingType;
};

#endif //__UIDATESETTINGDLG_H__

