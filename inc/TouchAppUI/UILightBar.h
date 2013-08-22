#ifndef __UILIGHTBAR_H__
#define __UILIGHTBAR_H__

#include "GUI/UIContainer.h"
#include "GUI/EventArgs.h"
#include "GUI/EventSet.h"

class UILightBar : public UIWindow
{
public:
	virtual LPCSTR GetClassName() const
	{
		return "UILightBar";
	}

	UILightBar();
	UILightBar(UIContainer* pParent);

	virtual ~UILightBar();

	void SetLevel(int level);
    int GetLevel();

    void SetIcons(SPtr<ITpImage> solidIcon, SPtr<ITpImage> hollowIcon);
    void SetMargin(int margin);

	int GetMinWidth() const;
	int GetMinHeight() const;

	virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
	virtual HRESULT Draw(DK_IMAGE drawingImg);

	virtual bool OnTouchEvent(MoveEvent* moveEvent);

	virtual void MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight);

private:
	void Init();

private:
	int mTotal;
	int mLevel;
	int mBarMargin;
	
	SPtr<ITpImage> mImageBarBlack;
	SPtr<ITpImage> mImageBarWhite;
};//UILightBar

#endif  // __UILIGHTBAR_H__

