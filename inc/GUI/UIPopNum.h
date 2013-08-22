#ifndef __UINEWCOUNT_H__
#define __UINEWCOUNT_H__

#include "GUI/UIAbstractText.h"
#include "GUI/UIImageTransparent.h"

class UIPopNum : public UIAbstractText
{
public:
    virtual LPCSTR GetClassName() const
    {
        return "UIPopNum";
    }

    UIPopNum(UIContainer* pParent);
    virtual ~UIPopNum();
    void MoveWindow(int iLeft, int iTop, int iWidth, int iHeight);
    HRESULT Draw(DK_IMAGE drawingImg);
    dkSize GetMinSize() const;
    void SetNum(int num);
private:
    void Init();
    void SetFontPath();
    UIImageTransparent m_image;
};
#endif
