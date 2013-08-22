////////////////////////////////////////////////////////////////////////
// UITextSingleLine.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UITEXTSINGLELINE_H__
#define __UITEXTSINGLELINE_H__

#include "GUI/UIWindow.h"
#include "GUI/UIContainer.h"
#include "GUI/UIText.h"
#include "GUI/TextDrawer.h"

#include "Public/GUI/IUITextSingleLine.h"
#include "GUI/UIAbstractText.h"
#define USE_OLD_TEXTLIB 0


class UITextSingleLine : public UIAbstractText
{
public:
     virtual LPCSTR GetClassName() const
    {
        return ("UITextSingleLine");
    }

    UITextSingleLine();
    UITextSingleLine(UIContainer* pParent, const DWORD dwId);

    virtual ~UITextSingleLine();
	virtual void SetFontPath();
	virtual void SetEnglishGothic();
    virtual HRESULT Draw(DK_IMAGE drawingImg);

	/**
	 * @brief MoveWindow 移动窗体，移动时会根据大小属性自动修改高度与宽度
	 */
    virtual void MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight);

	/**
	 * @brief SetTextMaxWidth 设置控件的最大宽度,movewindow时会隐式的指定最大宽度。
	 *
	 * @param maxTextWidth 最大宽度
	 */
	void SetTextMaxWidth(int maxTextWidth);
	void SetReversedColor(bool reverse);
    virtual dkSize GetMinSize() const;

protected:
    void Init();

    void OnDispose(BOOL bIsDisposed);

private:
};//UITextSingleLine
#endif
