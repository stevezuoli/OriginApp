#ifndef __UIIPADDRESSTEXTBOX_H_
#define __UIIPADDRESSTEXTBOX_H_

#include "GUI/CTpGraphics.h"
#include "GUI/UIImage.h"
#include "GUI/UITextBox.h"
#include <vector>
#include <string>

#define IPV4DECNUMLENGTH 4
class UIIPAddressTextBox : public UIContainer
{
public:
    UIIPAddressTextBox();
    UIIPAddressTextBox(UIContainer* pParent, const DWORD dwId);

    virtual LPCSTR GetClassName() const
    {
        return "UIIPAddressTextBox";
    }

    void MoveWindow(int left, int top, int width, int height);
    HRESULT DrawBackGround(DK_IMAGE drawingImg);
    void SetDefaultIPAddress(const std::string& address);
    ~UIIPAddressTextBox(){};
    void InitUI();
    BOOL OnKeyPress(INT32 iKeyCode);
    std::string GetIP();

private:
    bool OnInputChange(const EventArgs& args);

private:
    std::string  m_defaultIPs[IPV4DECNUMLENGTH];
    UITextBox m_vIPTexts[IPV4DECNUMLENGTH];
};

#endif

