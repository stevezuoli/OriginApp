#ifndef __UIFRONTLIGHTMANAGER_H__
#define __UIFRONTLIGHTMANAGER_H__


#include "GUI/UIContainer.h"
#include "GUI/UITouchComplexButton.h"

#include "singleton.h"
#include "TouchAppUI/UILightBar.h"
#include "drivers/FrontLight.h"


class UIFrontLightManager : public UIContainer
{
    SINGLETON_H(UIFrontLightManager)
public:
    UIFrontLightManager();
    ~UIFrontLightManager();

    virtual LPCSTR GetClassName() const
    {
        return ("UIFrontLightManager");
    }
    
    virtual void MoveWindow(INT32 _iLeft, INT32 _iTop, INT32 _iWidth, INT32 _iHeight);
    virtual HRESULT Draw(DK_IMAGE drawingImg);
    virtual void OnCommand(DWORD _dwCmdId, UIWindow* _pSender, DWORD _dwParam);

	virtual void SetEnableHookTouch(bool isEnable);
    virtual bool OnHookTouch(MoveEvent* moveEvent);
    virtual bool SetVisible(bool visible);
    virtual bool OnTouchEvent(MoveEvent* moveEvent);
    virtual void PerformClick(MoveEvent* moveEvent);
private:
	void OnBtnAddClicked();
    void OnBtnSubClicked();
    void UpdateLevelText();
    void UpdateLevelByX(int x);
protected:
   static const int TOTAL_LEVELS = 24;
    UITouchComplexButton m_btnAdd;
    UITouchComplexButton m_btnSub;
//    UILightBar m_LightBar;
    UIImage m_bars[TOTAL_LEVELS];
    void UpdateLighBar();
    SPtr<ITpImage> m_blackBarImage;
    SPtr<ITpImage> m_whiteBarImage;

    UITextSingleLine m_levelText;
    int m_downX, m_downY;
};


#endif
