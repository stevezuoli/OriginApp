////////////////////////////////////////////////////////////////////////
// UITouchButton.h
// Contact: chenm
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////
#ifndef __UITOUCHCOMPLEXBUTTON_H__
#define __UITOUCHCOMPLEXBUTTON_H__

#include "GUI/UITouchButton.h"
#include "GUI/UIContainer.h"
#include "GUI/UISeperator.h"
#include "GUI/UIImageTransparent.h"

class UITouchComplexButton : public UITouchButton
{
public:
    UITouchComplexButton(UIContainer* pParent, const DWORD dwId);
    UITouchComplexButton();
    virtual ~UITouchComplexButton();

    virtual LPCSTR GetClassName() const
    {
        return "UITouchComplexButton";
    }

    void MoveWindow(INT32 left, INT32 top, INT32 width, INT32 height);
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
    virtual HRESULT Draw(DK_IMAGE drawingImg);

	virtual void SetIcons(SPtr<ITpImage> UnFocusedIcon, SPtr<ITpImage> focusedIcon, int _iIconPos = UIButton::ICON_TOP);
	virtual void SetDisabledIcon(SPtr<ITpImage> disabledIcon);
    virtual void SetBackgroundImage(int normalImageID);

    void SetCustomizedIcons(SPtr<ITpImage> UnFocusedIcon,
                            SPtr<ITpImage> focusedIcon,
                            const DK_RECT& textArea,
                            int _iIconPos = UIButton::ICON_CUSTOMIZE);
    void SetCornerStyle(bool _bUpLeftSquare = false, bool _bUpRightSquare = false, bool _bRightDownSquare = false, bool _bLeftDownSquare = false);
    void GetCornerStyle(bool* leftTopSquare, bool* rightTopSquare, bool* rightBottomSquare, bool* leftBottomSquare) const
    {
        leftTopSquare && (*leftTopSquare = m_bUpLeftSquare);
        rightTopSquare && (*rightTopSquare = m_bUpRightSquare);
        rightBottomSquare && (*rightBottomSquare = m_bRightDownSquare);
        leftBottomSquare && (*leftBottomSquare = m_bLeftDownSquare);
    }
    virtual dkSize GetMinSize() const;
    void SetInternalHorzSpacing(int internalHorzSpacing)
    {
        m_internalHorzSpacing = internalHorzSpacing;
    }
    int GetInternalHorzSpacing() const
    {
        return m_internalHorzSpacing;
    }
    void SetInternalVertSpacing(int internalVertSpacing)
    {
        m_internalVertSpacing = internalVertSpacing;
    }
    int GetInternalVertSpacing() const
    {
        return m_internalVertSpacing;
    }
    void SetElemSpacing(int spacing)
    {
        SetInternalHorzSpacing(spacing);
        SetInternalVertSpacing(spacing);
    }

private:
   // void Update;
   void Init();
   void SetBorderLine(int borderLine);
    
private:
    bool m_bUpLeftSquare;
    bool m_bUpRightSquare;
    bool m_bRightDownSquare;
    bool m_bLeftDownSquare;
    int m_internalHorzSpacing;
    int m_internalVertSpacing;
    int m_borderLine;
    UIImageTransparent m_imgBackground;

    DK_RECT m_rectCustomizedTextArea;
    SPtr<ITpImage> m_pFocusedIcon;
    SPtr<ITpImage> m_pDisabledIcon;
};

/////////////////////////////////////////////////////////
// UITouchBackButton
/////////////////////////////////////////////////////////
class UISizerItem;
class UITouchBackButton : public UIContainer
{
public:
    UITouchBackButton(UIContainer* pParent = NULL, DWORD dwId = ID_BTN_TOUCH_BACK);
    UITouchBackButton();
    ~UITouchBackButton();

    virtual LPCSTR GetClassName() const
    {
        return "UITouchBackButton";
    }
    virtual bool OnChildClick(UIWindow* child);
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    //dkSize GetMinSize() const;
    void ShowSeperator(bool show);
    void ShowLeftSpace(bool show);
    void SetBackButtonLongClickable(bool clickable);

    virtual bool OnChildLongPress(UIWindow* child);

protected:
private:
    void Init();
    UITouchComplexButton m_btn;
    UISeperator m_seperator;
    UISizerItem* m_leftSpace;
};


#endif //__UITOUCHCOMPLEXBUTTON_H__

