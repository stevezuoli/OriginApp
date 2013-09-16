/*
 * =====================================================================================
 *       Filename:  UIButtonGroupWithFocusedLine.h
 *    Description:  The UIButtonGrop class provides a container to organize grops of buttons.
 *
 *        Version:  1.0
 *        Created:  01/28/2013 01:46:17 PM
 * =====================================================================================
 */
#ifndef __UIBUTTONGROUPWITHFOCUSEDLINE_H__
#define __UIBUTTONGROUPWITHFOCUSEDLINE_H__

#include "GUI/UIButtonGroup.h"

class UIButtonGroupWithFocusedLine : public UIButtonGroup
{
public:
    UIButtonGroupWithFocusedLine(UIContainer* pParent = 0);
    virtual ~UIButtonGroupWithFocusedLine();
    const char* GetClassName() const
    {
        return "UIButtonGroupWithFocusedLine";
    }
   
protected:
    virtual HRESULT DrawFocusedSymbol(DK_IMAGE& image);
};//UIButtonGroupWithFocusedLine
#endif//__UIBUTTONGROUPWITHFOCUSEDLINE_H__
