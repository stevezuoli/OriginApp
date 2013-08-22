/*
 * =====================================================================================
 *       Filename:  UIBookStoreChartsPageItem.h
 *    Description:  chart item showing in charts page, such as hot-pay,monthly-free,etc.
 *
 *        Version:  1.0
 *        Created:  04/01/2013 09:59:22 AM
 * =====================================================================================
 */

#ifndef _UIBOOKSTORECHARTSPAGEITEM_H_
#define _UIBOOKSTORECHARTSPAGEITEM_H_

#include <string>
#include "GUI/UIListItemBase.h"
#include "GUI/UISeperator.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UIImage.h"

class UIBookStoreChartsPageItem : public UIListItemBase
{
public:
    UIBookStoreChartsPageItem();
    ~UIBookStoreChartsPageItem();
    virtual LPCSTR GetClassName() const
    {
        return "UIBookStoreChartsPageItem";
    }

    void InitItem(const DWORD dwId, const std::string& title);
    virtual BOOL OnKeyPress(INT32 iKeyCode);

private:
    virtual HRESULT DrawBackGround(DK_IMAGE drawingImg);
    bool InitUI();
#ifdef KINDLE_FOR_TOUCH
    virtual void PerformClick(MoveEvent* moveEvent);
#endif

private:
    UISeperator m_seperator;
    UITextSingleLine m_titleLabel;
    UIImage m_arrow;
};//UIBookStoreChartsPageItem

#endif//_UIBOOKSTORECHARTSPAGEITEM_H_
