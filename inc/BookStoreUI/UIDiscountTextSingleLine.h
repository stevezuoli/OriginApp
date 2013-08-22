#ifndef __TOUCHAPPUI_UIDISCOUNTTEXTSINGLINE_H__
#define __TOUCHAPPUI_UIDISCOUNTTEXTSINGLINE_H__

#include "GUI/UITextSingleLine.h"
#include "GUI/CTpGraphics.h"
#ifndef KINDLE_FOR_TOUCH
    #include "Utility/ColorManager.h"
#endif

class UIDiscountTextSingleLine : public UITextSingleLine
{
public:
    UIDiscountTextSingleLine()
       : m_discount(true)
    {
#ifdef KINDLE_FOR_TOUCH
		SetStrikeThrough(true);
#endif
	}
    ~UIDiscountTextSingleLine(){}
    void SetDiscount(bool discount)
    {
        m_discount = discount;
#ifdef KINDLE_FOR_TOUCH
        SetStrikeThrough(m_discount);
#endif
    }
#ifndef KINDLE_FOR_TOUCH
    virtual HRESULT Draw(DK_IMAGE drawingImg)
    {
        if (!m_bIsVisible)
        {
            return S_OK;
        }

        if (drawingImg.pbData == NULL)
        {
            return E_FAIL;
        }
        SetForeColor(ColorManager::knBlack >> 1);
        UITextSingleLine::Draw(drawingImg);
        HRESULT hr(S_OK);
        if (m_discount)
        {
            DK_IMAGE imgSelf;
            DK_RECT rcSelf={m_iLeft, m_iTop, m_iLeft + m_iWidth, m_iTop + m_iHeight};
            RTN_HR_IF_FAILED(CropImage(
                        drawingImg,
                        rcSelf,
                        &imgSelf
                        ));
            CTpGraphics grf(imgSelf);
            RTN_HR_IF_FAILED(grf.DrawLine(0,(GetTextHeight() >> 1),GetTextWidth(),1,SOLID_STROKE));
        }
        return hr;
    }
#endif
private:
    bool m_discount;
};
#endif
