////////////////////////////////////////////////////////////////////////
// UIBookReaderProgressBar.h
// Contact: 
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UIBOOkREADERPROGRESSBBAR_H__
#define __UIBOOkREADERPROGRESSBBAR_H__

#include "GUI/UIProgressBar.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/GestureDetector.h"

typedef void (*RefreshProgress)(void* parm, double percentage, bool isStop);
class UIBookReaderProgressBar : public UIProgressBar
{
public:
    enum ProgressBarMode
	{
	    NormalMode,
	    MiniMode
	};

	enum ProgressBarDirection
	{
	    PBD_LTR_Horizontal,
	    PBD_RTL_Horizontal,
	    PBD_Vertical
	};

    virtual LPCSTR  GetClassName() const
    {
        return "UIBookReaderProgressBar";
    }

    UIBookReaderProgressBar();
    UIBookReaderProgressBar(UIContainer* pParent, const DWORD dwId);

    virtual ~UIBookReaderProgressBar();

	void SetProgressBarMode(ProgressBarMode iMode);
    void SetProgressBarMode(int iMode);
	ProgressBarMode GetProgressBarMode() const;
    void SetDirection(ProgressBarDirection direction);
    ProgressBarDirection GetDirection() const;
	void SetProgressColor(int color);
	void  SetProgressText(const std::string &_strText);
    std::string  GetProgressText() const;
    void SetStartPos(int startPos);
	virtual bool OnTouchEvent(MoveEvent* moveEvent);
	virtual HRESULT Draw(DK_IMAGE drawingImg);
	void SetEnableScroll(bool scrollable);
	void SetTransparent(bool transparent);
	void SetCallBackFun(RefreshProgress funcCallBack, void* parm);
	void SetBarHeight(int barHeight);

private:
	void Init();
    void RecalcBarLength();
	void UpdateLevelByX(int x);
	bool OnDown(MoveEvent* moveEvent);
    bool OnScroll(MoveEvent* moveEvent1, MoveEvent* moveEvent2, int distanceX, int distanceY);
    void OnHolding(int holdingX, int holdingY);
	
private:
	class UIProgressBarGestureListener : public SimpleGestureListener
	{
    public:
		virtual bool OnDown(MoveEvent* moveEvent);
        virtual bool OnScroll(MoveEvent* moveEvent1, MoveEvent* moveEvent2, int distanceX, int distanceY);
        virtual void OnHolding(int holdingX, int holdingY);
        void SetProgressBar(UIBookReaderProgressBar* progressBar)
        {
            m_pProgressBar = progressBar;
        }
    private:
        UIBookReaderProgressBar* m_pProgressBar;
	};

	UIProgressBarGestureListener m_gestureListener;
	GestureDetector m_gestureDetector;
    BOOL m_bIsDisposed;
    int m_startPos;
    ProgressBarDirection m_eDirection;
    ProgressBarMode m_eProgressBarMode;
    int m_iProgressBarColor;
    std::string m_strProgressText;        //ProgressBar上显示文本，目前紧跟进度条显示
    int m_iDistance;
    int m_iBorder;
    int m_iLineHeight;
	UITextSingleLine m_progress;
	int m_dragProgress;
	RefreshProgress m_refreshCallBack;
	void* m_callBackParm;
	bool m_scrollable;
	bool m_transparent;
};
#endif

