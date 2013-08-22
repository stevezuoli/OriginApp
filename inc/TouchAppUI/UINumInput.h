#ifndef __UINUMINPUT_H__
#define __UINUMINPUT_H__

#include "GUI/UIDialog.h"
#include "GUI/UITextBox.h"
#include "GUI/UITouchComplexButton.h"

class UINumInput : public UIDialog
{
public:
    UINumInput(UIContainer* _pParent = NULL);
    ~UINumInput();
    
    virtual LPCSTR GetClassName() const
    {
        return ("UINumInput");
    }
    
    virtual void MoveWindow(int _iLeft, int _iTop, int _iWidth, int _iHeight);
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    virtual HRESULT Draw(DK_IMAGE image);
    
    virtual int  GetInputNum() const { return m_iInputNum; }
    virtual void SetInitNum(int _iInitNum);

    virtual int  GetTotalHeight() const;
    virtual int  GetTotalWidth() const;
private:
    void Init();
    
private:
    UITextSingleLine m_txtInput;

    UITouchButton  m_KeyboardButton[12];
    UITouchComplexButton m_btnCancel;
    int m_iInputNum;
};


#endif
