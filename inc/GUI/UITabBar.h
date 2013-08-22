#ifndef UITABBAR_H
#define UITABBAR_H

#include <vector>

#include <GUI/UIContainer.h>
#include <GUI/EventArgs.h>
#include <GUI/EventSet.h>

class TabIndexChangedEventArgs : public EventArgs
{
public:
	TabIndexChangedEventArgs(int index)
		: m_currentTabIndex(index)
	{
	}

	virtual EventArgs* Clone() const
	{
		return new TabIndexChangedEventArgs(*this);
	}

	int m_currentTabIndex;
};//TabIndexChangedEventArgs

class UITouchComplexButton;
class UITabBar : public UIContainer
{

public:
	static const char* TabIndexChangedEvent;
    enum HighlightMode
    {
        HM_TOPLINE,
        HM_FULL
    };

	UITabBar(UIContainer* parent = 0);
	~UITabBar();

    void SetHighlightMode(HighlightMode highlightMode);

	virtual LPCSTR GetClassName() const
	{
		return "UITabBar";
	}
    HRESULT Draw(DK_IMAGE drawingImg);

	virtual void MoveWindow(INT32 left, INT32 top, INT32 width, INT32 height);
	virtual void OnCommand(DWORD dwCmdId, UIWindow* pSender, DWORD dwParam);
	bool FireTabIndexChangedEvent(int);

	/**
	 * @brief AddTab 增加tab项
	 *
	 * @param title tab项显示文字
	 * @param cmdId tab项对应的commandId.
	 *
	 * @return 该新添加tab项对应的index
	 */
	virtual int AddTab(const char* title);
	virtual int AddTab(const char* title, const DWORD cmdId);
	virtual int AddTab(const char* title, const DWORD cmdId, SPtr<ITpImage> focusedImage, SPtr<ITpImage> unFocusedImage, int iconPos);

	/**
	 * @brief InsertTab 增加新的tab项
	 *
	 * @param index 指定位置在该index前增加
	 * @param title
	 * @param cmdId
	 *
	 * @return 
	 */
	virtual int InsertTab(int index, const char* title);
	virtual int InsertTab(int index, const char* title, const DWORD cmdId);
	virtual int InsertTab(int index, const char* title, const DWORD cmdId, SPtr<ITpImage> focusedImage, SPtr<ITpImage> unFocusedImage, int iconPos);

	/**
	 * @brief RemoveTab 删除tab项
	 *
	 * @param index 指定删除tab的index
	 *
	 * @return 
	 */
	virtual bool RemoveTab(unsigned int index);

	/**
	 * @brief ElideText TODO 设置文字截断or省略号显示
	 *
	 * @param elideText
	 */
	void ElideText(bool elideText);
	bool IsElideText() const;

	/**
	 * @brief SetEnable 设置tab项是否可点击
	 *
	 * @param index tab项对应的index
	 * @param enable 是否可点击
	 */
	void SetEnable(int index, bool enable);
	bool IsEnable(int index) const;

	/**
	 * @brief CurrentIndex 返回当前tab项对应的索引值
	 *
	 * @return 
	 */
	int CurrentIndex() const;
	/**
	 * @brief SetCurrentIndex 设置选中的tab项
	 *
	 * @param int
	 */
	void SetCurrentIndex(int);

	/**
	 * @brief CurrentCmdId 返回当前tab项对应的命令ID
	 *
	 * @return 
	 */
	DWORD CurrentCmdId() const;
	/**
	 * @brief SetCurrentCmdId 设置选中的tab项
	 *
	 * @param int
	 */
	void SetCurrentCmdId(DWORD);

	/**
	 * @brief GetTabCounts tab项总数
	 *
	 * @return 
	 */
	unsigned int GetTabCounts() const;
	
	/**
	 * @brief SetFontSize 设置tab按键字体大小
	 *
	 * @param fontSize
	 */
	void SetFontSize(unsigned int fontSize);

	void SetSplitLineHeight(unsigned int height);


protected:
	virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);

private:
    void SetFocusTabColor(dkColor focusTabColor);
    void SetBackgroundTabColor(dkColor backgroundTabColor);
    void SetFocusTopLineColor(dkColor focusTopLineColor);
	void AutoModifySize();
	int IndexOfCommandId(DWORD cmdId);

private:
	std::vector<UITouchComplexButton*> m_tabButtons;
    void ResetTabButtonColors();

	int m_currentIndex;
	unsigned int m_fontSize;
	unsigned int m_normalLineWidth;
	unsigned int m_normalLineHeight;
	unsigned int m_focusedLineWidth;
	const unsigned int m_baseCommandId;
    dkColor m_focusTabColor;
    dkColor m_focusTopLineColor;
    dkColor m_backgroundTabColor;
    HighlightMode m_hightlightMode;
};//UITabBar


#endif//UITABBAR_H
