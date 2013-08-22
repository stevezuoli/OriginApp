#ifndef UISHORTCUTPAGE_H
#define UISHORTCUTPAGE_H

#include "GUI/UIPage.h"
#include "TouchAppUI/UIShortcutBar.h"
#include "GUI/UIComplexButton.h"
#include "TouchAppUI/UIDownloadPage.h"

class UIShortCutPage : public UIPage
{

public:
	UIShortCutPage();

	virtual LPCSTR GetClassName() const
	{
		return "UIShortCutPage";
	}

	virtual ~UIShortCutPage();
	//virtual HRESULT Draw(DK_IMAGE drawingImg);
    void UpdateSectionUI(bool bIsFull = false);
    void OnWifiMessage(SNativeMessage event);

	virtual void OnEnter();
	virtual void OnLeave();
	virtual void OnLoad();
	virtual void OnUnLoad();
    virtual void MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight);
    virtual bool OnTouchEvent(MoveEvent* moveEvent);

protected:
	virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
	void Init();
	void Dispose();

private:
	void OnDispose(BOOL bIsDisposed);
	void Finalize();

private:
	UIDownloadPage *m_pDownloadPage;

    virtual bool OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy);
    virtual bool OnSingleTapUp(MoveEvent* moveEvent);

    class FlingGestureListener : public SimpleGestureListener
    {
    public:
        void SetPage(UIShortCutPage* page)
        {
            m_page = page;
        }
        virtual bool OnSingleTapUp(MoveEvent* moveEvent)
        {
            return m_page->OnSingleTapUp(moveEvent);
        }
        virtual bool OnFling(MoveEvent* moveEvent1, MoveEvent* moveEvent2, FlingDirection direction, int vx, int vy)
        {
            return m_page->OnFling(moveEvent1, moveEvent2, direction, vx, vy);
        }
    private:
        UIShortCutPage* m_page;
    };
    GestureDetector m_gestureDetector;
    FlingGestureListener m_flingGestureListener;
    bool PtInBottomArea(int x, int y);

};//UIShortCutPage










#endif//UISHORTCUTPAGE_H
