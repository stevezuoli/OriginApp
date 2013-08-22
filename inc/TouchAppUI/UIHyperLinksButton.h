////////////////////////////////////////////////////////////////////////
// UIHyperLinksButton.h
// Contact: genchw
// Copyright (c) Duokan Corporation. All rights reserved.
//
//
////////////////////////////////////////////////////////////////////////


#ifndef __UIHYPERLINKSBUTTON_H__
#define __UIHYPERLINKSBUTTON_H__

#include "GUI/UIButton.h"
#include "GUI/FontManager.h"
#include "GUI/UIText.h"

typedef enum LayOutStyle
{
     UnknowLayOut=0
    ,LayOutByConstWidth
    ,LayOutByConstHeight
}HLBtnLayOutStyle;

class UIHyperLinksButton : public UIButton
{
public:
    UIHyperLinksButton(UIContainer* pParent, const DWORD dwId);
    UIHyperLinksButton();
    ~UIHyperLinksButton();

    virtual LPCSTR GetClassName() const
    {
        return ("UIHyperLinksButton");
    }

    void Initialize(DWORD dwId, LPCSTR szText, INT32 iFontSize,SPtr<ITpImage> Icon);
    virtual int GetLinkId();
    virtual void SetLinkId(int nLinkId);
    virtual void SetText(const CString& rstrTxt);

    virtual LPCSTR GetText() const;

    void SetIcon(SPtr<ITpImage> Icon);

    virtual HRESULT Draw(DK_IMAGE drawingImg);
    virtual BOOL    LayOutButton(int &iUsedWidth,int &iUsedHeight,HLBtnLayOutStyle style);

    HRESULT UpdateFocus();
private:
    SPtr<ITpImage> m_Icon;
    UIText    m_Text;
    int       m_iLinkId;
};

#endif

