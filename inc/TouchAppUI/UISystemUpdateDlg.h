#ifndef UISYSTEMUPDATEDLG_H
#define UISUSTEMUPDATEDLG_H

#include "GUI/UITextBox.h"
#include "GUI/UIModalDialog.h"
#include "GUI/UITextSingleLine.h"
#include "GUI/UITouchComplexButton.h"


class UISystemUpdateDlg : public UIModalDlg
{
public:
    UISystemUpdateDlg(UIContainer* pParent, LPCSTR title);
    virtual ~UISystemUpdateDlg();
    virtual void Popup();
    virtual void OnCommand(DWORD dwCmdId, UIWindow * pSender, DWORD dwParam);
    void SetText(const char* pstrUpdate);

private:
    void InitUI();
    void HandlePageUpDown(bool bPageUp);
    bool OnTouchMoveNext(GESTURE_EVENT _ge, int _x, int _y);
    bool OnTouchMovePrev(GESTURE_EVENT _ge, int _x, int _y);
    bool OnTouchTap(GESTURE_EVENT _ge, int _x, int _y);

private:
    UIText m_Text;
    int m_iCurPage;
    int m_iTotalPage;
    UITouchComplexButton m_btnOK;
    UITouchComplexButton m_btnCancel;

    UITouchComplexButton  m_leftPage;
    UITouchComplexButton  m_rightPage;
    UITextSingleLine      m_pageNum;
    wstring               m_wstrText;
};
#endif

