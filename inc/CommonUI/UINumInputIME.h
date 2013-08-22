#ifndef __UINUMINPUT_H__
#define __UINUMINPUT_H__

#include "CommonUI/UIIME.h"
#include "singleton.h"
#include "GUI/UIAbstractTextEdit.h"
#include "GUI/UITouchComplexButton.h"

class UINumInputIME : public UIIME
{
	SINGLETON_H(UINumInputIME);
public:
    void ShowIME(UIContainer *pParent = NULL);
    void HideIME();
    
    virtual LPCSTR GetClassName() const
    {
        return ("UINumInputIME");
    }
    
    virtual void MoveWindow(int _iLeft, int _iTop, int _iWidth, int _iHeight);
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
	virtual bool OnHookTouch(MoveEvent* moveEvent);
	virtual void SetEnterText(const char* text);
	virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
    virtual int  GetTotalHeight() const;
    virtual int  GetTotalWidth() const;
private:
    UINumInputIME();
    ~UINumInputIME();
    void Init();
	void InitUI();
    
private:
	UITouchComplexButton  m_KeyboardButton[12];
    UITouchComplexButton m_btnEnter;
	const int m_btnWidth;
	const int m_btnHeight;
};


#endif

