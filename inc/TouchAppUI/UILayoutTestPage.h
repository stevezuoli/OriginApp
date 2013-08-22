#ifndef UILAYOUTTESTPAGE_H
#define UILAYOUTTESTPAGE_H

#include <vector>
#include "GUI/UITextSingleLine.h"
#include "GUI/UIText.h"
#include "GUI/UITextBox.h"
#include "GUI/UIPlainTextEdit.h"
#include "GUI/UITouchComplexButton.h"
#include "GUI/UISizer.h"
#include "GUI/UIPage.h"

class UILayoutTestPage : public UIPage {

public:
    UILayoutTestPage();
    ~UILayoutTestPage();

    virtual void MoveWindow(int iLeft, int iTop, int iWidth, int iHeight);
    virtual void OnCommand(DWORD dwCmdId, UIWindow* pSender, DWORD dwParam);


protected:
    bool Init();
    void UnInit();

private:
    UITextSingleLine m_txtTest1;
    UITextSingleLine m_txtTest2;
    UITextSingleLine m_txtTest3;

    UITextSingleLine m_txtTotal;
    UITextSingleLine m_txtPageNO;

    UIText m_textTest;

    UITextBox m_boxTest1;
    UITextBox m_boxTest2;
    UITextBox m_boxTest3;
    UIPlainTextEdit m_plainTextEditTest1;
    UIPlainTextEdit m_plainTextEditTest2;
    UIPlainTextEdit m_plainTextEditTest3;

    UITouchComplexButton m_btnTest1;
    UITouchComplexButton m_btnTest2;

    vector<UIWindow*> m_vecWins1;
    vector<UIWindow*> m_vecWins2;
    vector<UIWindow*> m_vecWins3;
};//UILayoutTestPage
#endif//UILAYOUTTESTPAGE_H