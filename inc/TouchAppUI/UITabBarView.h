#ifndef _DKREADER_TOUCHAPPUI_UITABBARVIEW_H_
#define _DKREADER_TOUCHAPPUI_UITABBARVIEW_H_

#include <vector>
#include <string>
#include "GUI/UIImage.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UIContainer.h"
#include "GUI/EventArgs.h"


class UITabBarItem : public UIContainer
{
public:
	static const int ICONMARGINTOTITEL = 10;
	UITabBarItem();
	~UITabBarItem();
	void Initialize(const std::string& titel,SPtr<ITpImage> icon = SPtr<ITpImage>());
	virtual HRESULT Draw(DK_IMAGE drawingImg);
	virtual HRESULT DrawFocus(DK_IMAGE drawingImg);
private:
	SPtr<ITpImage> m_iconImage;
	std::string m_titelString;
	UIImage m_icon;
	UITextSingleLine m_titel;
};


class TabBarSelectedItemChangeEventArgs: public EventArgs
{
public:
	virtual EventArgs* Clone() const
	{
		return new TabBarSelectedItemChangeEventArgs(*this);
	}
	int m_selectedLine;
};

class UITabBarView : public UIContainer
{
public:
	static const char* EventTabBarSelectedItemChange;
	static const int ITEMSSPACE = 2;
	UITabBarView();
	~UITabBarView();
	void AddItemToTabBar(const std::string& titel, SPtr<ITpImage> icon = SPtr<ITpImage>());
	int GetSelectedLine();
	virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
	virtual HRESULT Draw(DK_IMAGE drawingImg);
	void SelectedLine(int selectLine);
#ifdef KINDLE_FOR_TOUCH
#endif
private:
	std::vector<UITabBarItem*>		m_itemList;
	int m_itemWidth;
	int m_selectedLine;
};
#endif//_DKREADER_TOUCHAPPUI_UITABBARVIEW_H_

