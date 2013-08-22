/*
 * =====================================================================================
 *       Filename:  UIButtonGroup.h
 *    Description:  The UIButtonGrop class provides a container to organize grops of buttons.
 *
 *        Version:  1.0
 *        Created:  01/28/2013 01:46:17 PM
 * =====================================================================================
 */
#ifndef __UIBUTTONGROUP_H__
#define __UIBUTTONGROUP_H__

#include "GUI/UIContainer.h"
#include "GUI/EventArgs.h"
#include "GUI/EventSet.h"
#include <vector>

class ButtonClickedEventArgs : public EventArgs
{
public:
    ButtonClickedEventArgs(DWORD commandId, UIWindow* pSender, DWORD param)
        : m_clickedButtonId(commandId)
        , m_pSender(pSender)
        , m_param(param)
    {
    }

    virtual EventArgs* Clone() const
    {
        return new ButtonClickedEventArgs(*this);
    }

    DWORD m_clickedButtonId;
    UIWindow* m_pSender;
    DWORD m_param;
};

class UIButton;
class UISizerFlags;
class UISizerItem;
class CTpGraphics;

class UIButtonGroup : public UIContainer
{
public:
    enum ButtonGroupDirection
    {
        BGD_Horizontal,
        BGD_Vertical
    };

    static const char*  ButtonClickedChangeEvent;
    UIButtonGroup(UIContainer* pParent = 0);   
    virtual ~UIButtonGroup();
    const char* GetClassName() const
    {
        return "UIButtonGroup";
    }

    void AddButton(UIButton* button);
    void AddButton(UIButton* button, const UISizerFlags& flag);
    void AddSpace(const int pixel);

    void RemoveButton(UIButton* button);

    UIButton* GetButtonByCommandId(DWORD commandId);

    bool IsExclusive() const;
    void SetExclusive(bool exclusive);
    void SetSplitLineHeight(int height);
    void SetSplitLineWidth(int width);

    void SetTopLinePixel(int pixel);
    void SetBottomLinePixel(int pixel);

    void SetFocusedIndex(int index);
    void SetFocusedButton(UIWindow* pWin);

    void SetBtnsDirection(ButtonGroupDirection dir);
    ButtonGroupDirection GetBtnsDirection() const;

    void SetDisplaySpliteLine(bool enable) { m_displaySpliteLine = enable; }
    
    void SetElementBorder(int border)
    {
        m_elementBorder = border;
    }
    virtual BOOL OnKeyPress(INT32 iKeyCode);
    virtual void OnCommand(DWORD dwCmdId, UIWindow* pSender, DWORD dwParam);
    HRESULT Draw(DK_IMAGE drawingImg);
private:
    void Init();
    bool EnsureSizerExist();
    void DoInsert(int index, UIButton* button, const UISizerFlags& flag);
    void ValidChildBtnShown();

protected:
    virtual HRESULT DrawSplitLine(CTpGraphics& grf);
    virtual HRESULT DrawTopLine(CTpGraphics& grf);
    virtual HRESULT DrawBottomLine(CTpGraphics& grf);
    virtual HRESULT DrawFocusedSymbol(DK_IMAGE& image);
    int IndexOfWindow(UIWindow* win);
    //TODO:@zhangying exclusive is true for now
    bool m_exclusive;
    bool m_displaySpliteLine;
    int m_splitLineHeight;
    int m_splitLinePixel;
    int m_topLinePixel;
    int m_bottomLinePixel;
    int m_currentFocusedIndex;
    int m_elementBorder;
    ButtonGroupDirection m_btnsDirection;
    UISizer* m_pBtnListSizer;
    UISizerItem* m_pTopLineSizerItem;
    UISizerItem* m_pBottomLineSizerItem;
};//UIButotnGroup
#endif//__UIBUTTONGROUP_H__
