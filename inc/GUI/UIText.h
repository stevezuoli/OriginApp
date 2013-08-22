////////////////////////////////////////////////////////////////////////
// UIText.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////

#ifndef __UITEXT_H__
#define __UITEXT_H__

#include "GUI/UIWindow.h"
#include "GUI/UIContainer.h"
#include "Public/GUI/IUIText.h"
#include "../../Kernel/include/TxtKit/TxtLib/DKTAPI.h"
#include "FontManager/DKFont.h"
#include "GUI/UIAbstractText.h"
using DkFormat::DkBufferBase;

class UIText : public UIAbstractText
{
public:
    virtual LPCSTR GetClassName() const
    {
        return ("UIText");
    }

    UIText();
    UIText(UIContainer* pParent, const DWORD dwId);

    virtual ~UIText();

    virtual int GetTextHeight() const;
	//virtual void SetFontPath();

	void SetMode(UIAbstractText::TextMode);

	//associated with display
    void SetLineSpace(DK_DOUBLE lineSpace);

	//associated with render info
	//获得render后的总页数
	UINT GetPageCount();
	UINT GetCurPage();
    int  SetPageHeight(int height);
	void SetDrawPageNo(UINT _uPageNo);
    int  GetHeightByWidth(int width) const;
    int  GetPageNumByWidthHeight(int width, int height);
	int  GetPageHeightOfPageIndex(UINT uPageNo);

    virtual HRESULT Draw(DK_IMAGE drawingImg);
    virtual void MoveWindow(INT32 iLeft, INT32 iTop, INT32 iWidth, INT32 iHeight);
    virtual dkSize GetMinSize() const;
    void SetBackgroundImage(const char* imgFile);
    void SetBackgroundImage(const std::string& imgFile)
    {
        SetBackgroundImage(imgFile.c_str());
    }

protected:
    void Init();

    void OnDispose(BOOL bIsDisposed);

protected:
	int m_iCurPage;
    std::wstring m_imgBackground;
    DK_UINT m_imgBackgroundWidth, m_imgBackgroundHeight;
};
#endif
