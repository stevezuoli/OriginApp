#ifndef UISTARS_H
#define UISTARS_H

#include "GUI/UIContainer.h"
#ifdef KINDLE_FOR_TOUCH
#include "GUI/EventArgs.h"
#include "GUI/EventSet.h"
#endif

class StarEditChangeEventArgs : public EventArgs
{
public:
    virtual EventArgs* Clone() const
    {
        return new StarEditChangeEventArgs(*this);
    }
};

class UIStars : public UIWindow
{
public:
    static const char* StarEditChange;

	virtual LPCSTR GetClassName() const
	{
		return "UIStars";
	}

	UIStars();
	UIStars(UIContainer* pParent);
	/**
	 * @brief UIStars 星级控件
	 *
	 * @param pParent
	 * @param blackStars 黑色星星的数目
	 * @param totalStars 星星总数
	 * @param readOnly 是否只读不可修改，默认false
	 */
	UIStars(UIContainer* pParent, double blackStars, double totalStars = 5, bool readOnly = false);

	virtual ~UIStars();

	/**
	 * @brief SetTotalStars 设置星的总数
	 *
	 * @param totalStars 默认5颗
	 */
	void SetTotalStars(int totalStars);
    bool SetScore(double score);
    double GetBlackStars() const;

    void SetStarsIcons(SPtr<ITpImage> solidIcon, SPtr<ITpImage> hollowIcon);
    void SetStarsMargin(int margin);
#ifdef KINDLE_FOR_TOUCH
    virtual dkSize GetMinSize() const 
    {
        return dkSize(GetMinWidth(), GetMinHeight());
    }
#endif
	int GetMinWidth() const;
	int GetMinHeight() const;

	virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
	virtual HRESULT Draw(DK_IMAGE drawingImg);
#ifdef KINDLE_FOR_TOUCH
    void PerformClick(MoveEvent* moveEvent);
#else
    virtual BOOL OnKeyPress(int iKeyCode);
	void ModifyStarsCountByStep(bool increase);
	void ChangeModifyStatus();
#endif
	virtual void MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight);

private:
	void Init();
    /**
	 * @brief SetBlackStars 设置黑色星星的数目
	 *
	 * @param blackStars double 分数，可以是小数
	 * @param ignoreHalfStar 是否忽略掉小数部分，默认false
	 */
	void SetBlackStars(double blackStars, bool ignoreHalfStar = false);

    int GetDrawStartLeft() const;
    int GetDrawStartTop() const;

private:
	int mTotalStars;
	int mBlackStars;
	bool mHalfStarFollow;
	int mStarMargin;
    HorizontalAlignMode mAlign;
    VerticalAlignMode mVAlign;

	SPtr<ITpImage> mImageBlackStar;
	SPtr<ITpImage> mImageHollowStar;
#ifdef KINDLE_FOR_TOUCH
#else
	SPtr<ITpImage> mImageRightArrow;//★
	SPtr<ITpImage> mImageLeftArrow;//☆
	int m_iLeftMargin;
	int m_iTopMargin;
	int m_iArrowMargin;
	bool mStarsModifing;
#endif
};//UIStars
#endif//UISTARS_H
